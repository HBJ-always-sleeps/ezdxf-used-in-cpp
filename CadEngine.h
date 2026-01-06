#ifndef CAD_ENGINE_H
#define CAD_ENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

// 基础坐标点
typedef struct { 
    double x; 
    double y; 
} CadPoint;

// 直线 (增加颜色)
typedef struct { 
    CadPoint start; 
    CadPoint end; 
    int color;      // AutoCAD Color Index (ACI)
} CadLine;

// 圆 (增加颜色)
typedef struct { 
    CadPoint center; 
    double radius; 
    int color; 
} CadCircle;

// 圆弧 (增加颜色)
typedef struct { 
    CadPoint center; 
    double radius; 
    double start_angle; 
    double end_angle; 
    int color; 
} CadArc;

// 多段线 (增加颜色)
typedef struct { 
    CadPoint* points; 
    int point_count; 
    int is_closed; 
    int color; 
} CadPolyline;

// 文字 (新增结构体)
typedef struct {
    char* content;          // 文字内容
    CadPoint insert_point;  // 插入点/位置
    int color;              // 颜色
} CadText;

// CAD数据汇总结构
typedef struct {
    CadLine* lines;         int line_count;
    CadCircle* circles;     int circle_count;
    CadArc* arcs;           int arc_count;
    CadPolyline* polylines; int poly_count;
    CadText* texts;         int text_count;   // 新增：文字数据
} CadDrawingData;

// 导出函数
__declspec(dllexport) CadDrawingData* get_cad_data(const char* file_path);
__declspec(dllexport) void free_cad_data(CadDrawingData* data);

#ifdef __cplusplus
}
#endif
#endif