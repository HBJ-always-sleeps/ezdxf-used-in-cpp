#include <iostream>
#include <string>
#include <vector>
#include <iomanip> // 用于格式化数字输出

// 包含你提供的 SDK 头文件
#include "CadEngine.h"

// --- Python 环境初始化声明 ---

// 在 C++ 中，需要用 extern "C" 确保链接正确。
extern "C" {
    __declspec(dllimport) void Py_SetPythonHome(const wchar_t*);
    __declspec(dllimport) void Py_Initialize(void);
    __declspec(dllimport) void Py_Finalize(void);
}

int main() {
    std::cout << "=== CAD SDK C++ Client Test ===" << std::endl;

    // 1. 初始化绿色版 Python 环境

    try {
        Py_SetPythonHome(L".");
        Py_Initialize();
        std::cout << "[Init] Embedded Python environment loaded." << std::endl;
    }
    catch (...) {
        std::cerr << "[Fatal Error] Failed to initialize Python runtime." << std::endl;
        return -1;
    }

    std::string dxfFile = "test.dxf";
    std::cout << "\n[Load] Attempting to parse file: " << dxfFile << "..." << std::endl;

    // 2. 调用 SDK 获取数据
    // C++ 可以直接接收 C 结构体指针
    CadDrawingData* data = get_cad_data(dxfFile.c_str());

    if (data == nullptr) {
        std::cerr << "[Error] Failed to load or parse DXF file." << std::endl;
        std::cerr << "Troubleshooting: Check if 'test.dxf' exists and 'ezdxf' folder is present." << std::endl;
    } else {
        // 3. 读取并展示数据
        std::cout << "\n--- Analysis Success ---" << std::endl;
        std::cout << "Summary:" << std::endl;
        std::cout << "  Lines:     " << data->line_count << std::endl;
        std::cout << "  Circles:   " << data->circle_count << std::endl;
        std::cout << "  Arcs:      " << data->arc_count << std::endl;
        std::cout << "  Polylines: " << data->poly_count << std::endl;

        // 设置输出精度方便查看坐标
        std::cout << std::fixed << std::setprecision(2);

        // --- 示例：访问直线数据 ---
        if (data->line_count > 0) {
            std::cout << "\n[Sample Data] First Line Geometry:" << std::endl;
            // C++ 中直接使用 -> 访问结构体成员
            std::cout << "  Start Point: (" << data->lines[0].start.x << ", " << data->lines[0].start.y << ")" << std::endl;
            std::cout << "  End Point:   (" << data->lines[0].end.x << ", " << data->lines[0].end.y << ")" << std::endl;
        }

        // --- 示例：访问多段线（复杂结构）数据 ---
        if (data->poly_count > 0) {
             std::cout << "\n[Sample Data] First Polyline Info:" << std::endl;
             CadPolyline& firstPoly = data->polylines[0]; // 使用引用方便操作
             std::cout << "  Is Closed: " << (firstPoly.is_closed ? "Yes" : "No") << std::endl;
             std::cout << "  Vertex Count: " << firstPoly.point_count << std::endl;
             
             if (firstPoly.point_count > 0) {
                 // 访问多段线的第一个顶点
                 std::cout << "  First Vertex: (" << firstPoly.points[0].x << ", " << firstPoly.points[0].y << ")" << std::endl;
             }
        }

        // 4. 【重要】释放内存
        // 必须调用 SDK 提供的释放函数，不能直接 delete
        free_cad_data(data);
        std::cout << "\n[Memory] SDK data memory released safely." << std::endl;
    }

    // 5. 清理环境
    Py_Finalize();
    
    std::cout << "\nTest finished. Press Enter to exit..." << std::endl;
    std::cin.get(); 
    return 0;
}