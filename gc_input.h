#ifndef GC_INPUT_H
#define GC_INPUT_H

#include <windows.h>

// Call this every frame to update input states
void gc_input_update();

// Keyboard queries
int gc_key_down(int vkey);       // Is key currently down?
int gc_key_pressed(int vkey);    // Key was pressed this frame
int gc_key_released(int vkey);   // Key was released this frame

// Mouse queries
POINT gc_get_mouse_pos();          // Current mouse position relative to client area
int gc_mouse_down(int button);   // Is mouse button down? (VK_LBUTTON, VK_RBUTTON, VK_MBUTTON)
int gc_mouse_pressed(int button);// Mouse button pressed this frame
int gc_mouse_released(int button);// Mouse button released this frame

// To be called from your window proc on WM_INPUT or WM_MOUSEMOVE if needed (optional)
LRESULT gc_input_wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif // GC_INPUT_H
