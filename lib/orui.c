#include "orui.h"
#include "misc.h"
#include "allocators.h"
#include "console.h"
#include "anim.h"

typedef struct {
    bool is_animating;
    bool continous;
    on_tick_callback on_tick;
    ui_window* wnds;
    u16 window_count;
    u16 window_cap;
} ui_state_t;

ui_state_t ui_state;

ui_window* orui_create_window(u16 width, u16 height, char* title)
{
    ui_state.window_count++; 
    if (ui_state.window_count >= ui_state.window_cap) {
        ui_state.window_cap += 2;
        ui_state.wnds = realloc(ui_state.wnds, ui_state.window_cap * sizeof(ui_window));
    }
    ui_window* result = &ui_state.wnds[ui_state.window_count-1];
    memset(result, 0, sizeof(ui_window));

    result->relayout_start_point = null;

    str id = cconcats1(title, snew_str("-root"));

    ui_panel* p = panel_as_root(result, id);
    p->w.root = result;
    p->w.pref_w = width; p->w.pref_h = height;
    result->root = p;
    platform_create_window(&result->n, width, height, title);
    return result;
}

void orui_style_init(void)
{
    style_add_props(new_str("absolute", 9), offsetof(ui_widget, absolute), PROPERTY_BOOL);
    style_add_props(new_str("offsets", 8), offsetof(ui_widget, offset), PROPERTY_URECT16);
    style_add_props(new_str("min-width", 10), offsetof(ui_widget, min_w), PROPERTY_U16);
    style_add_props(new_str("min-height", 11), offsetof(ui_widget, min_h), PROPERTY_U16);
    style_add_props(new_str("max-width", 10), offsetof(ui_widget, max_w), PROPERTY_U16);
    style_add_props(new_str("max-height", 11), offsetof(ui_widget, max_h), PROPERTY_U16);
    style_add_props(new_str("width", 6), offsetof(ui_widget, pref_w), PROPERTY_U16);
    style_add_props(new_str("height", 7), offsetof(ui_widget, pref_h), PROPERTY_U16);

    style_add_props(new_str("grow", 5), offsetof(ui_widget, grow), PROPERTY_FLOAT);
    style_add_props(new_str("shrink", 7), offsetof(ui_widget, shrink), PROPERTY_FLOAT);
    
    style_add_props(new_str("border-width", 13), offsetof(ui_widget, border_width), PROPERTY_URECT16);
    style_add_props(new_str("margin", 7), offsetof(ui_widget, margin), PROPERTY_URECT16);
    style_add_props(new_str("padding", 8), offsetof(ui_widget, padding), PROPERTY_URECT16);
    
    style_add_props(new_str("background", 11), offsetof(ui_widget, background), PROPERTY_COLOR);
    style_add_props(new_str("foreground", 11), offsetof(ui_widget, foreground), PROPERTY_COLOR);
    style_add_props(new_str("border", 7), offsetof(ui_widget, border), PROPERTY_COLOR);
    
    style_add_props(new_str("z-index", 8), offsetof(ui_widget, zindex), PROPERTY_U16);

    panel_init();
}

ui_window* orui_init(u16 width, u16 height, char* title)
{
    platform_init();
    orui_style_init();
    anim_init();
    ui_state.window_count = 0;
    ui_state.window_cap = 2;
    ui_state.wnds = malloc(ui_state.window_cap * sizeof(ui_window)); 
    ui_window* result = orui_create_window(width, height, title);
    return result;
}

void orui_destroy_window(ui_window* window)
{
    platform_destroy_window(&window->n);
    // swap freed window with the last active window
    memcpy(window, &ui_state.wnds[ui_state.window_count-1], sizeof(ui_window));
    ui_state.window_count--;
}

// ==== LIFECYCLE ====

ui_widget* orui_find_element_by_point(ui_widget* w, uvec2 point)
{
    if (rect_contains(w->bounds, point)) {
        for (int i = 0; i < w->child_count; i++) {
            ui_widget* c = w->children[i];
            ui_widget* result = orui_find_element_by_point(c, point);
            if (result) return result;;
        }
        return w;
    }
    else return null;
}

void orui_update(ui_window* w, orui_update_kind kind, int key, void* dp)
{
    switch (kind) {
        case UPDATE_RESIZE: {
            // vd ^= new size
            w->repaint = true;
            w->relayout_start_point = (ui_widget*)w->root;
            w->relayout_start_depth = 0;
            uvec2 vd = *(uvec2*)dp;
            w->dims = vd;
            ui_panel* p = w->root;
            p->w.pref_w = vd.x; p->w.pref_h = vd.y;
            w->relayout_start_point = (ui_widget*)p; 
            w->relayout_start_depth = 0;
        } break;
        case UPDATE_MOUSE_MOVE: {
            // vd ^= mouse pos relative to window corner
            // HANDLE ABSOLUTE POSITIONED WIDGETS
            uvec2 vd = *(uvec2*)dp;
            ui_widget* hovered = orui_find_element_by_point((ui_widget*)w->root, vd);
            if (hovered == null) {
                log_error("no hovered found!");
                break;
            }
            if (hovered != w->hovered) {
                if (w->hovered) {
                    w->hovered->wmsg(w->hovered, MSG_HOVER_END, 0, null);
                }
                hovered->wmsg(hovered, MSG_HOVER_START, 0, null);
                w->hovered = hovered;
            }
            else if (w->hovered) {
                uvec2 vd = *(uvec2*)dp;
                uvec2 delta = uvec2_sub(vd, w->last_mouse_pos);
                w->hovered->wmsg(w->hovered, MSG_MOUSE_MOVE, 0, &delta);
            }
            w->last_mouse_pos = vd;
        } break;
        case UPDATE_MOUSE_LEAVE: {
            if (w->hovered) {
                w->hovered->wmsg(w->hovered, MSG_HOVER_END, 0, null);
                w->hovered = null;
            }
        } break;
        case UPDATE_ANIM: {
            if (ui_state.on_tick) { ui_state.on_tick(); }
            double dt = *(double*)dp;
            log_debug("dt=%lf", dt);
            anim_update(dt);
            w->relayout_start_point = (ui_widget*)w->root;
            w->relayout_start_depth = 0;
            w->repaint = true;
        } break;
    }

    if (w->relayout_start_point) {
        ui_widget* sp = w->relayout_start_point;
        box_constraint bc;
        if (sp->parent) {
            bc = bc_tight_from_bounds(sp->bounds); // when there is a parent that means sp is fixed
        } else {
            bc = bc_tight(sp->pref_w, sp->pref_h);
        }
        uvec2 size = sp->layout_handler(sp, bc);
        sp->bounds.r = sp->bounds.l + size.x; sp->bounds.b = sp->bounds.t + size.y;
        w->relayout_start_point = null;
        orui_update(w, UPDATE_MOUSE_MOVE, 0, &w->last_mouse_pos);
    }

    if (w->repaint) {
        orui_paint(w);
        w->repaint = false;
    }
}

void orui_animate(double dt, double freq)
{
    // TODO: find better solution
    LARGE_INTEGER start;
    QueryPerformanceCounter(&start);
    for (int i = 0; i < ui_state.window_count; i++) {
        ui_window* w = &ui_state.wnds[i];

        LARGE_INTEGER end;
        QueryPerformanceCounter(&end);
//        dt += ((end.QuadPart - start.QuadPart)) / freq;

        orui_update(w, UPDATE_ANIM, 0, &dt);

        QueryPerformanceCounter(&start);
    } 
}

void orui_move(ui_widget* w, urect16 new_bounds, bool relayout)
{
    w->bounds = new_bounds;
    w->dirty = false;
    if (relayout) {
        ui_widget* cur = w->parent;
        while (cur->parent) {
            cur->dirty = true;
            if (cur->fixed) break; // when a panel is fixed, any layout change inside of it does not affect its parents
            cur = cur->parent;
        }
        orui_relayout(cur);
    }
}

void orui_repaint(ui_widget* w)
{
    // TODO: bounds checking (or maybe delegate diffing to the platform layer)
    w->root->repaint = true;
}

void orui_relayout(ui_widget* w)
{
    // determine depth
    u16 own_depth = 0;
    ui_widget* cur = w;
    while (cur->parent) {
        own_depth++; cur = cur->parent;
    }

    ui_window* wnd = w->root;
    if (wnd->relayout_start_point) {
        if (wnd->relayout_start_point == w) return;
        // TODO: FUNKTIONIERT NICHT FÜR ABSOLUT POSITIONIERTE WIDGETS
        // find common ancestor
        u16 other_depth = wnd->relayout_start_depth;
        int diff = own_depth - other_depth;

        cur = wnd->relayout_start_point;
        ui_widget* other = w;

        // wenn das neue widget höher sitzt als das alte
        u16 depth = own_depth;
        if (diff > 0) {
            cur = other; diff *= -1;
            other = wnd->relayout_start_point;
            depth = other_depth;
        }

        // bring them to the same level
        for (int i = 0; i < diff; i++) {
            cur = cur->parent;
        }

        // find common ancestor
        while (cur->parent) {
            if (cur == other) break;
            cur = cur->parent; w = w->parent;
            depth--; 
        }

        wnd->relayout_start_point = cur;
        wnd->relayout_start_depth = depth;
        return;
    }

    if (w->parent) {
        wnd->relayout_start_point = w->parent;
    } else {
        wnd->relayout_start_point = w;
    }

    wnd->relayout_start_depth = own_depth;
}

i32 orui_message_loop(void)
{
    ui_window* w = (ui_window*)&ui_state.wnds[0];
    uvec2 v = new_uvec2(w->dims.x, w->dims.y);
    orui_update(w, UPDATE_RESIZE, 0, &v);
    i32 result = platform_message_loop();
    // cleanup
    platform_destroy();
    free(ui_state.wnds);
    return result;    
}

void orui_set_continous(bool continous, on_tick_callback callback)
{
    ui_state.continous = continous;
    if (continous) {
        ui_state.on_tick = callback;
    } else {
        ui_state.on_tick = null;
    }
}

bool orui_is_animating(void)
{
    return ui_state.is_animating || ui_state.continous;
}

void orui_set_animating(bool animating)
{
    if (ui_state.on_tick != null) ui_state.is_animating = true;
    else ui_state.is_animating = animating;
}

void orui_paint_main(painter* p, ui_widget* w)
{
    w->paint_handler(w, p);
    for (int i = 0; i < w->child_count; i++) {
        ui_widget* c = w->children[i];
        uvec2 temp = p->cur_offset;
        p->cur_offset.x += w->bounds.l;
        p->cur_offset.y += w->bounds.t;
        orui_paint_main(p, c);
        p->cur_offset = temp;
    }
}

void orui_paint(ui_window* w)
{
    platform_window* window = &w->n;
    platform_begin_paint(window);

    painter p; p.window = window; 
    p.clip_rect = make_urect16(0, 0, window->height, window->width);
    p.bits = window->bits;
    p.cur_offset = new_uvec2(0, 0);
    
    ui_widget* root = (ui_widget*)w->root;
    orui_paint_main(&p, root);

    platform_end_paint(window);
}

ui_panel* orui_get_root(ui_window *w)
{
    return w->root;
}