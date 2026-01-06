#include <iostream>
#include <Python.h>
#include "CadEngine.h"

int main() {
    Py_SetPythonHome(L"."); 
    Py_Initialize();

    std::cout << "--- CAD SDK Client Start ---" << std::endl;
    
    // Make sure test.dxf is in D:\code
    const char* filename = "test.dxf";
    CadDrawingData* data = get_cad_data(filename);

    if (data) {
        std::cout << "Success!" << std::endl;
        std::cout << "Lines: " << data->line_count << std::endl;
        std::cout << "Circles: " << data->circle_count << std::endl;
        
        if (data->line_count > 0) {
            std::cout << "First Line: (" << data->lines[0].start.x << "," << data->lines[0].start.y << ")" << std::endl;
        }
        if (data->text_count > 0) {
            printf("First Text: %s at (%.2f, %.2f) Color: %d\n", 
            data->texts[0].content, 
            data->texts[0].insert_point.x, 
            data->texts[0].insert_point.y, 
            data->texts[0].color);
}
        free_cad_data(data);
    } else {
        std::cout << "Error: No data returned from Python." << std::endl;
    }

    Py_Finalize();
    return 0;
}