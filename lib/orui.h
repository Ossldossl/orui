#pragma once
#include "misc.h"
#include "platform.h"
#include "widgets.h"

typedef struct painter painter;

struct painter {
    platform_window* window;
    urect16 clip_rect;
    u32* bits;
    uvec2 cur_offset;
};

typedef enum {
    UPDATE_KEYPRESS,
    UPDATE_MOUSE_MOVE,
    UPDATE_HOVER,
    UPDATE_MOUSE_LEAVE,
    UPDATE_LDOWN,
    UPDATE_LUP,
    UPDATE_RESIZE,
    UPDATE_ANIM,
} orui_update_kind; 

typedef void (*on_tick_callback)(void);

ui_window* orui_init(u16 width, u16 height, char* title);
ui_window* orui_create_window(u16 width, u16 height, char* title);
void orui_set_reactive(bool reactive, on_tick_callback callback);
i32 orui_message_loop(void);
bool orui_is_animating(void);
void orui_set_animating(bool animating);
void orui_animate(double dt, double freq);
ui_panel* orui_get_root(ui_window* w);
void orui_relayout(ui_widget* w);
void orui_repaint(ui_widget* w);
void orui_set_state(ui_widget* w, u8 state_id, bool dont_animate);
void orui_set_state_forced(ui_widget *w, u8 state_id, bool dont_animate);
//=== PLATFORM LAYER INTEROP ====
void ui_input(void);
void orui_paint(ui_window* w);
void orui_update(ui_window* w, orui_update_kind kind, int key, void* dp);