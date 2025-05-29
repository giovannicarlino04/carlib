#include "gc_graphics.h"

static HWND gc_hwnd = NULL;
static HDC gc_backbuffer_dc = NULL;
static HBITMAP gc_backbuffer_bmp = NULL;
static HDC gc_window_dc = NULL;
static int gc_width = 1920;
static int gc_height = 1080;
BOOL gc_running = TRUE;

static DWORD gc_last_time = 0;
LRESULT CALLBACK gc_wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void gc_init_backbuffer() {
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
        if (msg.message == WM_QUIT) {
            gc_running = FALSE;
            return FALSE;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return TRUE;
}

BOOL gc_init(HINSTANCE hInstance, int nCmdShow, LPCSTR windowTitle, int width, int height) {
    gc_width = width;
    gc_height = height;
    
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = gc_wnd_proc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "GCGraphicsWindow";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    if (!RegisterClassA(&wc)) return FALSE;
    
    gc_hwnd = CreateWindowA("GCGraphicsWindow", windowTitle,
                            WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            width, height,
                            NULL, NULL, hInstance, NULL);
    
    if (!gc_hwnd) return FALSE;
    
    ShowWindow(gc_hwnd, nCmdShow);
    UpdateWindow(gc_hwnd);
    
    gc_init_backbuffer();
    gc_last_time = GetTickCount();
    return TRUE;
}

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
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    HGDIOBJ oldPen = SelectObject(gc_backbuffer_dc, pen);
    MoveToEx(gc_backbuffer_dc, x1, y1, NULL);
    LineTo(gc_backbuffer_dc, x2, y2);
    SelectObject(gc_backbuffer_dc, oldPen);
    DeleteObject(pen);
}

void gc_draw_thick_line(int x1, int y1, int x2, int y2, int thickness, COLORREF color) {
    HPEN pen = CreatePen(PS_SOLID, thickness, color);
    HGDIOBJ oldPen = SelectObject(gc_backbuffer_dc, pen);
    MoveToEx(gc_backbuffer_dc, x1, y1, NULL);
    LineTo(gc_backbuffer_dc, x2, y2);
    SelectObject(gc_backbuffer_dc, oldPen);
    DeleteObject(pen);
}

void gc_draw_rect(int x, int y, int w, int h, COLORREF color, BOOL filled) {
    if (filled) {
        HBRUSH brush = CreateSolidBrush(color);
        HGDIOBJ oldBrush = SelectObject(gc_backbuffer_dc, brush);
        RECT rect = {x, y, x + w, y + h};
        FillRect(gc_backbuffer_dc, &rect, brush);
        SelectObject(gc_backbuffer_dc, oldBrush);
        DeleteObject(brush);
    } else {
        HPEN pen = CreatePen(PS_SOLID, 1, color);
        HGDIOBJ oldPen = SelectObject(gc_backbuffer_dc, pen);
        Rectangle(gc_backbuffer_dc, x, y, x + w, y + h);
        SelectObject(gc_backbuffer_dc, oldPen);
        DeleteObject(pen);
    }
}

void gc_draw_circle(int x, int y, int radius, COLORREF color, BOOL filled) {
    if (filled) {
        HBRUSH brush = CreateSolidBrush(color);
        SelectObject(gc_backbuffer_dc, brush);
        Ellipse(gc_backbuffer_dc, x - radius, y - radius, x + radius, y + radius);
        DeleteObject(brush);
    } else {
        HPEN pen = CreatePen(PS_SOLID, 1, color);
        SelectObject(gc_backbuffer_dc, pen);
        Ellipse(gc_backbuffer_dc, x - radius, y - radius, x + radius, y + radius);
        DeleteObject(pen);
    }
}

void gc_draw_text(int x, int y, LPCSTR text, COLORREF color) {
    SetTextColor(gc_backbuffer_dc, color);
    SetBkMode(gc_backbuffer_dc, TRANSPARENT);
    TextOutA(gc_backbuffer_dc, x, y, text, lstrlenA(text));
}

HBITMAP gc_load_bitmap(LPCSTR filepath) {
    return (HBITMAP)LoadImageA(NULL, filepath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
}

void gc_draw_bitmap(HBITMAP bmp, int x, int y) {
    HDC memDC = CreateCompatibleDC(gc_backbuffer_dc);
    HGDIOBJ oldBmp = SelectObject(memDC, bmp);
    BITMAP bmpInfo;
    GetObject(bmp, sizeof(BITMAP), &bmpInfo);
    BitBlt(gc_backbuffer_dc, x, y, bmpInfo.bmWidth, bmpInfo.bmHeight, memDC, 0, 0, SRCCOPY);
    SelectObject(memDC, oldBmp);
    DeleteDC(memDC);
}

void gc_set_title(LPCSTR title) {
    SetWindowTextA(gc_hwnd, title);
}

float gc_get_delta_time() {
    DWORD current = GetTickCount();
    float delta = (current - gc_last_time) / 1000.0f;
    gc_last_time = current;
    return delta;
}

LRESULT CALLBACK gc_wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_SIZE:
            gc_width = LOWORD(lParam);
            gc_height = HIWORD(lParam);
            gc_init_backbuffer();
            return 0;
        case WM_CLOSE:
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
