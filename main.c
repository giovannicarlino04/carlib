#include <windows.h>
#include <stdio.h>
#include "gc_graphics.h"
#include "gc_input.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (!gc_init(hInstance, nCmdShow, "GC Graphics Test", 800, 600)) {
        MessageBoxA(NULL, "Failed to initialize GC Graphics", "Error", MB_ICONERROR);
        return 1;
    }
    
    //HBITMAP bmp = gc_load_bitmap("test.bmp");
    
    float fpsTimer = 0.0f;
    int frames = 0;
    
    while (gc_running) {
        if (!gc_process_messages()) break;
        
        // Update input states each frame
        gc_input_update();
        
        float dt = gc_get_delta_time();
        fpsTimer += dt;
        frames++;
        
        if (fpsTimer >= 1.0f) {
            char title[64];
            snprintf(title, sizeof(title), "GC Graphics Test - FPS: %d", frames);
            gc_set_title(title);
            fpsTimer = 0.0f;
            frames = 0;
        }
        
        gc_clear(RGB(30, 30, 30));
        
        gc_draw_rect(50, 50, 150, 100, RGB(0, 128, 255), TRUE);
        gc_draw_circle(400, 300, 75, RGB(255, 100, 0), FALSE);
        gc_draw_thick_line(0, 0, 799, 599, 5, RGB(255, 255, 255));
        gc_draw_text(10, 10, "Press ESC to quit", RGB(255, 255, 0));
        
        //if (bmp) {
        //    gc_draw_bitmap(bmp, 600, 400);
        //}
        
        POINT mp = gc_get_mouse_pos();
        char mouseText[32];
        snprintf(mouseText, sizeof(mouseText), "Mouse: %d, %d", mp.x, mp.y);
        gc_draw_text(10, 30, mouseText, RGB(0, 255, 0));
        
        // Use gc_input functions now for input checking:
        if (gc_key_down(VK_ESCAPE)) break;
        
        gc_present();
        Sleep(16);
    }
    
    //if (bmp) DeleteObject(bmp);
    gc_quit();
    return 0;
}
