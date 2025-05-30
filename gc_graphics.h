#ifndef GC_GRAPHICS_H
#define GC_GRAPHICS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>
#include <stdint.h>


typedef LRESULT (CALLBACK *GC_FormProc)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef LRESULT (CALLBACK *GCFormCallback)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND gc_graphics_init(HINSTANCE hInstance, int nCmdShow, LPCSTR windowTitle, int width, int height);
void gc_init_backbuffer();
BOOL gc_process_messages(void);
void gc_quit(void);
HWND gc_form_create(const char* title, int width, int height);
void gc_form_set_proc(HWND hwnd, GC_FormProc proc);
void gc_form_show(HWND hwnd);
HWND gc_button_create(HWND parent, const char* text, int x, int y, int w, int h, GCFormCallback cb);
HWND gc_label_create(HWND parent, const char* text, int x, int y, int w, int h);
HWND gc_textbox_create(HWND parent, int x, int y, int w, int h);
HWND gc_checkbox_create(HWND parent, const char* text, int x, int y, int w, int h, GCFormCallback cb);
void gc_set_control_text(HWND ctrl, const char* text);
void gc_get_control_text(HWND ctrl, char* out, int max);
void gc_clear(COLORREF color);
void gc_present(void);
void gc_draw_line(int x1, int y1, int x2, int y2, COLORREF color);
void gc_draw_thick_line(int x1, int y1, int x2, int y2, int thickness, COLORREF color);
void gc_draw_rect(int x, int y, int w, int h, COLORREF color, BOOL filled);
void gc_draw_circle(int x, int y, int radius, COLORREF color, BOOL filled);
void gc_draw_text(int x, int y, LPCSTR text, COLORREF color);
HBITMAP gc_load_bitmap(LPCSTR filepath);
void gc_draw_bitmap(HBITMAP bmp, int x, int y);
void gc_set_title(LPCSTR title);
float gc_get_delta_time(void);

#ifdef __cplusplus
}
#endif

#endif // GC_GRAPHICS_H
