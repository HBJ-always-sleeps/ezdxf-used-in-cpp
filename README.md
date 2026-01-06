
## Impossible CAD SDK v2.1 (C++/Python Hybrid)
```
**"Impossible but useless assignment from my idiot boss, but I did it—and I made it better."**

This SDK is a high-performance C++ wrapper for the Python `ezdxf` library.
It enables C++ applications to extract complex geometric data, text content,
and visual attributes (colors) from DXF files without requiring the end-user to install Python.
```
## 🚀 What's New in v2.1
```
* **Text Support**: Full extraction of `TEXT` and `MTEXT` entities (content and insertion points).
* **Color Attributes**: Support for **ACI (AutoCAD Color Index)** across all entities.
* **Smart Memory**: Implemented deep-copying for strings (`_strdup`) to ensure memory safety between Python and C++.
* **Auto-Initialization**: Enhanced internal logic to handle Python environment setup gracefully.

## 📂 Repository Structure
```text
.
├── dist/                   # Deployment package (Send this to users)
│   ├── CadEngine.h         # Clean C++ Header (No Python dependency)
│   ├── CadEngine.lib       # Import Library
│   ├── CadEngine.dll       # Core Logic
│   ├── python311.dll       # Embedded Python Engine
│   ├── Lib/                # Python Standard Library
│   ├── ezdxf/              # CAD Parsing Engine
│   └── CadLogic.py         # Updated Logic Script (v1.2)
├── src/                    # Source Code
│   ├── CadEngine.cpp       # DLL Implementation with Dict parsing
│   └── main.cpp            # Test Application
└── build.bat               # MSVC Build Script

```

## 🛠️ Quick Start (C++)

```cpp
#include "CadEngine.h"

int main() {
    // 1. Get data (Python init is handled internally in v1.2)
    CadDrawingData* data = get_cad_data("design.dxf");
    
    if (data) {
        // 2. Access Text and Colors
        for(int i=0; i < data->text_count; i++) {
            printf("Text: %s | Color Index: %d\n", 
                   data->texts[i].content, 
                   data->texts[i].color);
        }
        // 3. Cleanup
        free_cad_data(data);
    }
    return 0;
}

```

## 📜 License

Provided "as-is" because my boss made me do it. Now with 100% more colors.

```

---


### 内部技术总结：那些年踩过的坑 (Post-Mortem)


#### 🚩 坑 1：环境路径与 `Py_SetPythonHome`
* **现象**：程序在开发机正常，发给别人就报 `0xC0000005` 崩溃。
* **教训**：内嵌 Python 必须明确告诉它“家”在哪。在 C++ 里必须调用 `Py_SetPythonHome(L".")`，且运行目录必须有 `Lib` 文件夹。
* **方案**：将所有依赖打入 `dist` 包，并在 DLL 初始化时强制指定当前路径。

#### 🚩 坑 2：头文件污染 (Python.h vs SDK)
* **现象**：需求方抱怨他们的项目没装 Python，包含你的头文件直接报错 `Python.h not found`。
* **教训**：永远不要在发给客户的 `.h` 里包含外部依赖。
* **方案**：在 `CadEngine.h` 中只使用原生 C++ 类型（`double`, `int`, `struct`），将 `#include <Python.h>` 留在 `.cpp` 内部隐藏起来。

#### 🚩 坑 3：内存生命周期的“跨界”问题
* **现象**：获取到的字符串显示乱码，或者程序在释放内存时崩溃。
* **教训**：Python 返回的字符串指针由 Python 管理，Python 函数结束该内存可能失效。
* **方案**：C++ 端接收到字符串后，立即使用 `_strdup` 进行**深拷贝**。在释放总结构体时，必须手动循环释放这些字符串内存。

#### 🚩 坑 4：编码与 ACI 颜色
* **现象**：中文路径读取失败，或者颜色数据对不上。
* **教训**：`ezdxf` 默认处理 UTF-8，而 Windows 路径可能是本地编码。
* **方案**：在 Python 端统一使用 `e.dxf.color` 获取标准的 ACI 索引，这比处理 RGB 更符合 CAD 用户的习惯。


```



