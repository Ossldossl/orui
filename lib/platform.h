#pragma once

#include "misc.h"

typedef struct painter painter;

#ifdef UI_BACKEND_WIN32_SOFTWARE
#include "platform/win32_software.h"
#endif

typedef struct platform_window platform_window;

void platform_init(void);
// the first created window is expected to be the main window
bool platform_create_window(platform_window* wnd, u16 width, u16 height, char* title); 
i32 platform_message_loop(void);
void platform_window_set_title(platform_window* wnd, char* new_title);
void platform_window_set_dims(platform_window* wnd, u16 x, u16 y, u16 w, u16 h);
void platform_destroy_window(platform_window* wnd);
void platform_destroy(void);

void platform_begin_paint(platform_window* window);
void platform_end_paint(platform_window* window);

#define ui_draw(type, args, ...) platform_draw_##type(widget->root, __VA_ARGS__) 
void platform_draw_rect(painter* p, urect16 r, u32 c);
