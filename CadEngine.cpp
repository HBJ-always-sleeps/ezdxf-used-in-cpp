#include <Python.h>
#include <iostream>
#include <vector>
#include <string>
#include "CadEngine.h"

// 内部辅助函数：从字典中获取 double
double GetDictDouble(PyObject* dict, const char* key) {
    PyObject* val = PyDict_GetItemString(dict, key);
    return val ? PyFloat_AsDouble(val) : 0.0;
}

// 内部辅助函数：从字典中获取 int
int GetDictInt(PyObject* dict, const char* key) {
    PyObject* val = PyDict_GetItemString(dict, key);
    return val ? (int)PyLong_AsLong(val) : 256; // 默认随层
}

extern "C" __declspec(dllexport) CadDrawingData* get_cad_data(const char* file_path) {
    // 1. 初始化检查
    if (!Py_IsInitialized()) {
        Py_SetPythonHome(L".");
        Py_Initialize();
    }

    PyObject* pName = PyUnicode_DecodeFSDefault("CadLogic");
    PyObject* pModule = PyImport_Import(pName);
    Py_XDECREF(pName);

    if (!pModule) {
        PyErr_Print();
        return nullptr;
    }

    PyObject* pFunc = PyObject_GetAttrString(pModule, "analyze_dxf");
    if (!pFunc || !PyCallable_Check(pFunc)) {
        Py_XDECREF(pModule);
        return nullptr;
    }

    PyObject* pArgs = PyTuple_Pack(1, PyUnicode_FromString(file_path));
    PyObject* pValue = PyObject_CallObject(pFunc, pArgs);
    Py_XDECREF(pArgs);
    Py_XDECREF(pFunc);
    Py_XDECREF(pModule);

    if (!pValue || pValue == Py_None) {
        return nullptr;
    }

    CadDrawingData* data = new CadDrawingData();
    memset(data, 0, sizeof(CadDrawingData));

    // --- 解析 Lines ---
    PyObject* pLines = PyDict_GetItemString(pValue, "lines");
    if (pLines && PyList_Check(pLines)) {
        data->line_count = (int)PyList_Size(pLines);
        data->lines = new CadLine[data->line_count];
        for (int i = 0; i < data->line_count; i++) {
            PyObject* item = PyList_GetItem(pLines, i);
            PyObject* pts = PyDict_GetItemString(item, "points");
            data->lines[i].start.x = PyFloat_AsDouble(PyTuple_GetItem(PyTuple_GetItem(pts, 0), 0));
            data->lines[i].start.y = PyFloat_AsDouble(PyTuple_GetItem(PyTuple_GetItem(pts, 0), 1));
            data->lines[i].end.x = PyFloat_AsDouble(PyTuple_GetItem(PyTuple_GetItem(pts, 1), 0));
            data->lines[i].end.y = PyFloat_AsDouble(PyTuple_GetItem(PyTuple_GetItem(pts, 1), 1));
            data->lines[i].color = GetDictInt(item, "color");
        }
    }

    // --- 解析 Circles ---
    PyObject* pCircles = PyDict_GetItemString(pValue, "circles");
    if (pCircles && PyList_Check(pCircles)) {
        data->circle_count = (int)PyList_Size(pCircles);
        data->circles = new CadCircle[data->circle_count];
        for (int i = 0; i < data->circle_count; i++) {
            PyObject* item = PyList_GetItem(pCircles, i);
            PyObject* detail = PyDict_GetItemString(item, "data");
            PyObject* center = PyTuple_GetItem(detail, 0);
            data->circles[i].center.x = PyFloat_AsDouble(PyTuple_GetItem(center, 0));
            data->circles[i].center.y = PyFloat_AsDouble(PyTuple_GetItem(center, 1));
            data->circles[i].radius = PyFloat_AsDouble(PyTuple_GetItem(detail, 1));
            data->circles[i].color = GetDictInt(item, "color");
        }
    }

    // --- 解析 Texts (新逻辑) ---
    PyObject* pTexts = PyDict_GetItemString(pValue, "texts");
    if (pTexts && PyList_Check(pTexts)) {
        data->text_count = (int)PyList_Size(pTexts);
        data->texts = new CadText[data->text_count];
        for (int i = 0; i < data->text_count; i++) {
            PyObject* item = PyList_GetItem(pTexts, i);
            
            // 拷贝内容字符串
            const char* content = PyUnicode_AsUTF8(PyDict_GetItemString(item, "content"));
            data->texts[i].content = _strdup(content ? content : "");
            
            // 插入点
            PyObject* ins = PyDict_GetItemString(item, "insert");
            data->texts[i].insert_point.x = PyFloat_AsDouble(PyTuple_GetItem(ins, 0));
            data->texts[i].insert_point.y = PyFloat_AsDouble(PyTuple_GetItem(ins, 1));
            
            data->texts[i].color = GetDictInt(item, "color");
        }
    }

    // 注意：Arc 和 Polyline 的解析逻辑需要根据字典结构以此类推... (篇幅原因略)

    Py_XDECREF(pValue);
    return data;
}

extern "C" __declspec(dllexport) void free_cad_data(CadDrawingData* data) {
    if (!data) return;
    if (data->lines) delete[] data->lines;
    if (data->circles) delete[] data->circles;
    if (data->arcs) delete[] data->arcs;
    if (data->polylines) {
        for (int i = 0; i < data->poly_count; i++) delete[] data->polylines[i].points;
        delete[] data->polylines;
    }
    if (data->texts) {
        for (int i = 0; i < data->text_count; i++) free(data->texts[i].content); // 对应 _strdup
        delete[] data->texts;
    }
    delete data;
}