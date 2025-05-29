#ifndef GC_GRAPHICS_H
#define GC_GRAPHICS_H

#include <windows.h>

extern BOOL gc_running;

BOOL gc_init(HINSTANCE hInstance, int nCmdShow, LPCSTR windowTitle, int width, int height);
void gc_quit();
BOOL gc_process_messages();

void gc_clear(COLORREF color);
void gc_present();

void gc_draw_line(int x1, int y1, int x2, int y2, COLORREF color);
void gc_draw_thick_line(int x1, int y1, int x2, int y2, int thickness, COLORREF color);
void gc_draw_rect(int x, int y, int w, int h, COLORREF color, BOOL filled);
void gc_draw_circle(int x, int y, int radius, COLORREF color, BOOL filled);
void gc_draw_text(int x, int y, LPCSTR text, COLORREF color);

HBITMAP gc_load_bitmap(LPCSTR filepath);
void gc_draw_bitmap(HBITMAP bmp, int x, int y);
void gc_set_title(LPCSTR title);
float gc_get_delta_time();

#endif
