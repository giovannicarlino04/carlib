#include "gc_graphics.h"
#include <windows.h>
#include <string.h>

#define MAX_CONTROLS 1024
#define MAX_FORMS 64

typedef struct {
    HWND hwnd;
    GC_FormProc proc;
} GCForm;

typedef struct {
    HWND hwnd;
    int id;
    GCFormCallback callback;
} GCControl;

static HWND gc_hwnd = NULL;
static HDC gc_backbuffer_dc = NULL;
static HBITMAP gc_backbuffer_bmp = NULL;
static HDC gc_window_dc = NULL;
static int gc_width = 1920;
static int gc_height = 1080;
BOOL gc_running = TRUE;

static DWORD gc_last_time = 0;

static struct {
    HINSTANCE hInstance;
    GCForm forms[MAX_FORMS];
    int form_count;
    GCControl controls[MAX_CONTROLS];
    int control_count;
} gc_ctx = {0};

void gc_init_backbuffer() {
    if (gc_backbuffer_dc) DeleteDC(gc_backbuffer_dc);
    if (gc_backbuffer_bmp) DeleteObject(gc_backbuffer_bmp);

    gc_window_dc = GetDC(gc_hwnd);
    gc_backbuffer_dc = CreateCompatibleDC(gc_window_dc);
    gc_backbuffer_bmp = CreateCompatibleBitmap(gc_window_dc, gc_width, gc_height);
    SelectObject(gc_backbuffer_dc, gc_backbuffer_bmp);
    ReleaseDC(gc_hwnd, gc_window_dc);
}

BOOL gc_process_messages() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT || 
           (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)) {
            gc_running = FALSE;
            return FALSE;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return TRUE;
}

static LRESULT gc_call_form_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    for (int i = 0; i < gc_ctx.form_count; i++) {
        if (gc_ctx.forms[i].hwnd == hwnd && gc_ctx.forms[i].proc)
            return gc_ctx.forms[i].proc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

static void gc_store_control(HWND hwnd, int id, GCFormCallback cb) {
    if (gc_ctx.control_count < MAX_CONTROLS) {
        gc_ctx.controls[gc_ctx.control_count++] = (GCControl){ hwnd, id, cb };
    }
}

LRESULT CALLBACK gc_wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COMMAND) {
        int id = LOWORD(wParam);
        for (int i = 0; i < gc_ctx.control_count; i++) {
            if (gc_ctx.controls[i].id == id && gc_ctx.controls[i].callback)
                return gc_ctx.controls[i].callback(gc_ctx.controls[i].hwnd, msg, wParam, lParam);
        }
    } else if (msg == WM_SIZE) {
        gc_width = LOWORD(lParam);
        gc_height = HIWORD(lParam);
        gc_init_backbuffer();
        return 0;
    } else if (msg == WM_CLOSE || msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    LRESULT res = gc_call_form_proc(hwnd, msg, wParam, lParam);
    return res ? res : DefWindowProc(hwnd, msg, wParam, lParam);
}

HWND gc_graphics_init(HINSTANCE hInstance, int nCmdShow, LPCSTR title, int width, int height) {
    gc_width = width;
    gc_height = height;
    gc_ctx.hInstance = hInstance;

    WNDCLASSA wc = {0};
    wc.lpfnWndProc = gc_wnd_proc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "GCGraphicsWindow";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassA(&wc)) return NULL;

    gc_hwnd = CreateWindowA(wc.lpszClassName, title,
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            width, height, NULL, NULL, hInstance, NULL);

    if (!gc_hwnd) return NULL;

    ShowWindow(gc_hwnd, nCmdShow);
    UpdateWindow(gc_hwnd);
    gc_init_backbuffer();
    gc_last_time = GetTickCount();

    gc_ctx.forms[gc_ctx.form_count++] = (GCForm){ gc_hwnd, NULL };
    return gc_hwnd;
}

void gc_form_set_proc(HWND hwnd, GC_FormProc proc) {
    for (int i = 0; i < gc_ctx.form_count; i++) {
        if (gc_ctx.forms[i].hwnd == hwnd) {
            gc_ctx.forms[i].proc = proc;
            return;
        }
    }
    if (gc_ctx.form_count < MAX_FORMS)
        gc_ctx.forms[gc_ctx.form_count++] = (GCForm){ hwnd, proc };
}

void gc_form_show(HWND hwnd) {
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
}

static HWND gc_create_control(LPCSTR className, DWORD style, HWND parent,
                              const char* text, int x, int y, int w, int h,
                              GCFormCallback cb) {
    int id = gc_ctx.control_count;
    HWND hwnd = CreateWindowA(className, text,
                              WS_CHILD | WS_VISIBLE | style,
                              x, y, w, h,
                              parent, (HMENU)(INT_PTR)id,
                              gc_ctx.hInstance, NULL);
    if (hwnd && cb) gc_store_control(hwnd, id, cb);
    return hwnd;
}

HWND gc_button_create(HWND parent, const char* text, int x, int y, int w, int h, GCFormCallback cb) {
    return gc_create_control("BUTTON", 0, parent, text, x, y, w, h, cb);
}

HWND gc_checkbox_create(HWND parent, const char* text, int x, int y, int w, int h, GCFormCallback cb) {
    return gc_create_control("BUTTON", BS_CHECKBOX, parent, text, x, y, w, h, cb);
}

HWND gc_label_create(HWND parent, const char* text, int x, int y, int w, int h) {
    return gc_create_control("STATIC", 0, parent, text, x, y, w, h, NULL);
}

HWND gc_textbox_create(HWND parent, int x, int y, int w, int h) {
    return gc_create_control("EDIT", WS_BORDER | ES_AUTOHSCROLL, parent, "", x, y, w, h, NULL);
}

void gc_set_control_text(HWND ctrl, const char* text) {
    SetWindowTextA(ctrl, text);
}

void gc_get_control_text(HWND ctrl, char* out, int max) {
    GetWindowTextA(ctrl, out, max);
}

// Graphics API

void gc_quit() {
    gc_running = FALSE;
    if (gc_backbuffer_dc) DeleteDC(gc_backbuffer_dc);
    if (gc_backbuffer_bmp) DeleteObject(gc_backbuffer_bmp);
}

void gc_clear(COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    RECT rect = {0, 0, gc_width, gc_height};
    FillRect(gc_backbuffer_dc, &rect, brush);
    DeleteObject(brush);
}

void gc_present() {
    gc_window_dc = GetDC(gc_hwnd);
    BitBlt(gc_window_dc, 0, 0, gc_width, gc_height, gc_backbuffer_dc, 0, 0, SRCCOPY);
    ReleaseDC(gc_hwnd, gc_window_dc);
}

void gc_draw_line(int x1, int y1, int x2, int y2, COLORREF color) {
    gc_draw_thick_line(x1, y1, x2, y2, 1, color);
}

void gc_draw_thick_line(int x1, int y1, int x2, int y2, int thickness, COLORREF color) {
    HPEN pen = CreatePen(PS_SOLID, thickness, color);
    HGDIOBJ old = SelectObject(gc_backbuffer_dc, pen);
    MoveToEx(gc_backbuffer_dc, x1, y1, NULL);
    LineTo(gc_backbuffer_dc, x2, y2);
    SelectObject(gc_backbuffer_dc, old);
    DeleteObject(pen);
}

void gc_draw_rect(int x, int y, int w, int h, COLORREF color, BOOL filled) {
    if (filled) {
        HBRUSH brush = CreateSolidBrush(color);
        RECT rect = {x, y, x + w, y + h};
        FillRect(gc_backbuffer_dc, &rect, brush);
        DeleteObject(brush);
    } else {
        gc_draw_thick_line(x, y, x + w, y, 1, color);
        gc_draw_thick_line(x + w, y, x + w, y + h, 1, color);
        gc_draw_thick_line(x + w, y + h, x, y + h, 1, color);
        gc_draw_thick_line(x, y + h, x, y, 1, color);
    }
}

void gc_draw_circle(int x, int y, int r, COLORREF color, BOOL filled) {
    if (filled) {
        HBRUSH brush = CreateSolidBrush(color);
        HGDIOBJ old = SelectObject(gc_backbuffer_dc, brush);
        Ellipse(gc_backbuffer_dc, x - r, y - r, x + r, y + r);
        SelectObject(gc_backbuffer_dc, old);
        DeleteObject(brush);
    } else {
        HPEN pen = CreatePen(PS_SOLID, 1, color);
        HGDIOBJ old = SelectObject(gc_backbuffer_dc, pen);
        Ellipse(gc_backbuffer_dc, x - r, y - r, x + r, y + r);
        SelectObject(gc_backbuffer_dc, old);
        DeleteObject(pen);
    }
}

void gc_draw_text(int x, int y, LPCSTR text, COLORREF color) {
    SetTextColor(gc_backbuffer_dc, color);
    SetBkMode(gc_backbuffer_dc, TRANSPARENT);
    TextOutA(gc_backbuffer_dc, x, y, text, lstrlenA(text));
}

HBITMAP gc_load_bitmap(LPCSTR path) {
    return (HBITMAP)LoadImageA(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
}

void gc_draw_bitmap(HBITMAP bmp, int x, int y) {
    HDC memDC = CreateCompatibleDC(gc_backbuffer_dc);
    HGDIOBJ old = SelectObject(memDC, bmp);
    BITMAP bmpInfo;
    GetObject(bmp, sizeof(BITMAP), &bmpInfo);
    BitBlt(gc_backbuffer_dc, x, y, bmpInfo.bmWidth, bmpInfo.bmHeight, memDC, 0, 0, SRCCOPY);
    SelectObject(memDC, old);
    DeleteDC(memDC);
}

void gc_set_title(LPCSTR title) {
    SetWindowTextA(gc_hwnd, title);
}

float gc_get_delta_time() {
    DWORD now = GetTickCount();
    float delta = (now - gc_last_time) / 1000.0f;
    gc_last_time = now;
    return delta;
}
