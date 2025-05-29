#include "gc_forms.h"
#include <string.h>

#define MAX_CONTROLS 1024

static GCFormCallback callbacks[MAX_CONTROLS] = {0};
static HWND handles[MAX_CONTROLS] = {0};
static int control_count = 0;

static HINSTANCE gc_hInstance = NULL;

BOOL gc_forms_init(HINSTANCE hInstance) {
    gc_hInstance = hInstance;

    WNDCLASSA wc = {0};
    wc.lpfnWndProc = gc_form_wnd_proc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "GCFormsWindow";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    return RegisterClassA(&wc);
}

HWND gc_form_create(const char* title, int width, int height) {
    return CreateWindowA("GCFormsWindow", title, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                         CW_USEDEFAULT, CW_USEDEFAULT, width, height,
                         NULL, NULL, gc_hInstance, NULL);
}

void gc_form_show(HWND hwnd) {
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
}

void gc_forms_run_loop() {
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

static void store_control(HWND hwnd, GCFormCallback cb) {
    if (control_count < MAX_CONTROLS) {
        handles[control_count] = hwnd;
        callbacks[control_count] = cb;
        control_count++;
    }
}

HWND gc_button_create(HWND parent, const char* text, int x, int y, int w, int h, GCFormCallback on_click) {
    HWND hwnd = CreateWindowA("BUTTON", text, WS_CHILD | WS_VISIBLE,
                              x, y, w, h, parent, (HMENU)(INT_PTR)control_count,
                              gc_hInstance, NULL);
    store_control(hwnd, on_click);
    return hwnd;
}

HWND gc_label_create(HWND parent, const char* text, int x, int y, int w, int h) {
    return CreateWindowA("STATIC", text, WS_CHILD | WS_VISIBLE,
                         x, y, w, h, parent, NULL, gc_hInstance, NULL);
}

HWND gc_textbox_create(HWND parent, int x, int y, int w, int h) {
    return CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                         x, y, w, h, parent, NULL, gc_hInstance, NULL);
}

HWND gc_checkbox_create(HWND parent, const char* text, int x, int y, int w, int h, GCFormCallback on_toggle) {
    HWND hwnd = CreateWindowA("BUTTON", text, WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
                              x, y, w, h, parent, (HMENU)(INT_PTR)control_count,
                              gc_hInstance, NULL);
    store_control(hwnd, on_toggle);
    return hwnd;
}

void gc_set_control_text(HWND control, const char* text) {
    SetWindowTextA(control, text);
}

void gc_get_control_text(HWND control, char* out, int max) {
    GetWindowTextA(control, out, max);
}

LRESULT CALLBACK gc_form_wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND: {
            int id = LOWORD(wParam);
            if (id >= 0 && id < control_count && callbacks[id]) {
                callbacks[id](handles[id], msg, wParam, lParam);
            }
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
