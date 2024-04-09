#include "orui.h"
#include "misc.h"
#include "allocators.h"

typedef struct {
    bool is_animating;
    on_tick_callback on_tick;
    bucka windows;
} ui_state_t;

ui_state_t ui_state;

ui_window* orui_init(u16 width, u16 height, char* title)
{
    platform_init();
    ui_state.windows = bucka_init(32, sizeof(ui_window));
    ui_window* result = bucka_alloc(&ui_state.windows);
    platform_create_window(&result->n, width, height, title);
    return result;
}

ui_window* orui_create_window(u16 width, u16 height, char* title)
{
    ui_window* result = bucka_alloc(&ui_state.windows);
    platform_create_window(&result->n, width, height, title);
    return result;
}

void orui_destroy_window(ui_window* window)
{
    platform_destroy_window(&window->n);
    bucka_free(&ui_state.windows, &window);
}

i32 orui_message_loop(void)
{
    i32 result = platform_message_loop();
    // cleanup
    platform_destroy();
    bucka_destroy(&ui_state.windows);
    return result;    
}

void orui_set_reactive(bool reactive, on_tick_callback callback)
{
    if (!reactive) {
        ui_state.is_animating = true;
        ui_state.on_tick = callback;
    } else {
        ui_state.on_tick = null;
    }
}

[[always_inline]] bool orui_is_animating(void)
{
    return ui_state.is_animating;
}

void orui_set_animating(bool animating)
{
    if (ui_state.on_tick != null) ui_state.is_animating = true;
    else ui_state.is_animating = animating;
}

void orui_animate(void)
{
    if (ui_state.on_tick) { ui_state.on_tick(); }
    // TODO: animations
}

void orui_paint(platform_window* window)
{
    platform_begin_paint(window);
    // TODO: paint widget tree
    platform_end_paint(window);
}