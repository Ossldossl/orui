#include <stdlib.h>
#include <string.h>
#include "misc.h"
#include "widgets.h"
#include "console.h"

static ui_widget* make_widget(ui_widget* parent, ui_window* root, u64 size, message_handler widget_message, layout_handler lh, paint_handler ph)
{
    ui_widget* result = malloc(size);
    memset(result, 0, size);
    result->wmsg = widget_message;
    result->layout_handler = lh;
    result->paint_handler = ph;
    result->parent = parent;
    result->root = root;
    result->zindex = root->cur_zindex++;

    if (parent) {
        if (parent->child_cap == parent->child_count) {
            parent->child_cap += 2;
            parent->children = realloc(parent->children, parent->child_cap * sizeof(ui_widget*));
        }

        parent->children[parent->child_count] = result;
        parent->child_count++;
    }

    return result; 
}

static void remove_widget(ui_widget* child) 
{
    ui_widget* p = child->parent;

    u16 i = 0;
    for (; i < p->child_count; i++) {
        ui_widget* c = p->children[i];
        if (c == child) { break; }    
    }
    ui_widget* last = p->children[p->child_count-1];
    p->children[i] = last;
    free(child);
    return;
}

//==== PANELS ====
#define PANEL_SPACING 5
static uvec2 panel_layout(ui_widget* w, box_constraint bc)
{
    log_debug("Layout!");
    ui_panel* p = (ui_panel*)w;
    uvec2 own_size;
    
    // TODO: real layout 
    u16 x_cursor = 0;
    u16 biggest_height = 0;
    for (int i = 0; i < w->child_count; i++) {
        ui_widget* c = w->children[i];
        uvec2 size = c->layout_handler(c, bc_loose(own_size.x, own_size.y));
        if (size.y > biggest_height) biggest_height = size.y;
        c->bounds = make_urect16(0, x_cursor, size.y, size.x + x_cursor);
        x_cursor += size.x + PANEL_SPACING;
    }
    if (x_cursor > PANEL_SPACING) x_cursor -= PANEL_SPACING;

    if (w->fixed) {
        own_size = new_uvec2(w->pref_w, w->pref_h);        
    } else {
        own_size = new_uvec2(x_cursor, biggest_height);
    }

    own_size = bc_constrain(bc, own_size);
    return own_size;
}

static void panel_paint(ui_widget* w, painter* p)
{
    // TODO: accumulate the positions from the parents
    platform_draw_rect(p, w->bounds, w->background);
}

static i32 panel_message(ui_widget* widget, ui_msg msg, i32 di, void* dp)
{
    switch (msg) {
        case MSG_HOVER_START: {
            widget->background = 0xFFFF0000;
            if (widget != (ui_widget*)widget->root->root) {
                widget->pref_w = 200;
                orui_move(widget, make_urect16(0,0,200,200), true);
            }
            orui_repaint(widget);
        } break;
        case MSG_HOVER_END: {
            widget->background = 0xFF0000FF;
            if (widget != (ui_widget*)widget->root->root) {
                widget->pref_w = 100;
                orui_move(widget, make_urect16(0,0,200,100), true);
            }
            orui_repaint(widget); 
        } break;
    }
    return 0;
}

ui_panel* panel(ui_widget* parent)
{
    ui_panel* result = (ui_panel*)make_widget(parent, parent->root, sizeof(ui_panel), panel_message, panel_layout, panel_paint);
    return result;
}

ui_panel* panel_as_root(ui_window* root)
{
    ui_panel* result = (ui_panel*)make_widget(null, root, sizeof(ui_panel), panel_message, panel_layout, panel_paint);
    result->w.fixed = true;
    return result;
}
