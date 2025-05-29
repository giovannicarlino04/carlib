#ifndef GC_FORMS_H
#define GC_FORMS_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GC_FORM_BUTTON,
    GC_FORM_LABEL,
    GC_FORM_TEXTBOX,
    GC_FORM_CHECKBOX
} GCFormControlType;

typedef void (*GCFormCallback)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Init and run
BOOL gc_forms_init(HINSTANCE hInstance);
void gc_forms_run_loop();

// Window creation
HWND gc_form_create(const char* title, int width, int height);
void gc_form_show(HWND hwnd);

// Controls
HWND gc_button_create(HWND parent, const char* text, int x, int y, int w, int h, GCFormCallback on_click);
HWND gc_label_create(HWND parent, const char* text, int x, int y, int w, int h);
HWND gc_textbox_create(HWND parent, int x, int y, int w, int h);
HWND gc_checkbox_create(HWND parent, const char* text, int x, int y, int w, int h, GCFormCallback on_toggle);

// Text
void gc_set_control_text(HWND control, const char* text);
void gc_get_control_text(HWND control, char* out, int max);

// Internal message handler
LRESULT CALLBACK gc_form_wnd_proc(HWND, UINT, WPARAM, LPARAM);

#ifdef __cplusplus
}
#endif

#endif // GC_FORMS_H
