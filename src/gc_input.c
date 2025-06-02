// Maximum virtual-key code (256 is enough for Windows VK codes)
#include "common.h"
#define GC_KEY_COUNT 256

static int keys_down[GC_KEY_COUNT];
static int keys_prev[GC_KEY_COUNT];

static int mouse_down[3];      // Left=0, Right=1, Middle=2
static int mouse_prev[3];
static POINT mouse_pos = {0,0};

// Map mouse button VK codes to array indices
internal int gc_mouse_button_to_index(int vkey) {
    switch (vkey) {
        case VK_LBUTTON: return 0;
        case VK_RBUTTON: return 1;
        case VK_MBUTTON: return 2;
        default: return -1;
    }
}

DLLEXPORT void gc_input_update() {
    // Copy current states to previous
    for (int i = 0; i < GC_KEY_COUNT; i++) {
        keys_prev[i] = keys_down[i];
        keys_down[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }
    for (int i = 0; i < 3; i++) {
        mouse_prev[i] = mouse_down[i];
    }

    // Update mouse position relative to the foreground window
    POINT p;
    if (GetCursorPos(&p)) {
        HWND fg = GetForegroundWindow();
        if (fg && ScreenToClient(fg, &p)) {
            mouse_pos = p;
        }
    }

    // Update mouse button states
    mouse_down[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    mouse_down[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    mouse_down[2] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
}

DLLEXPORT int gc_key_down(int vkey) {
    if (vkey < 0 || vkey >= GC_KEY_COUNT) return 0;
    return keys_down[vkey];
}

DLLEXPORT int gc_key_pressed(int vkey) {
    if (vkey < 0 || vkey >= GC_KEY_COUNT) return 0;
    return keys_down[vkey] && !keys_prev[vkey];
}

DLLEXPORT int gc_key_released(int vkey) {
    if (vkey < 0 || vkey >= GC_KEY_COUNT) return 0;
    return !keys_down[vkey] && keys_prev[vkey];
}

DLLEXPORT POINT gc_get_mouse_pos() {
    return mouse_pos;
}

DLLEXPORT int gc_mouse_down(int button) {
    if (button < 0 || button >= 3) return 0;
    return mouse_down[button];
}

DLLEXPORT int gc_mouse_pressed(int button) {
    if (button < 0 || button >= 3) return 0;
    return mouse_down[button] && !mouse_prev[button];
}

DLLEXPORT int gc_mouse_released(int button) {
    if (button < 0 || button >= 3) return 0;
    return !mouse_down[button] && mouse_prev[button];
}