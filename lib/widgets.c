#include <stdint.h>
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

uvec2 finalize_size(ui_widget* w, uvec2 size)
{
    size.x += w->padding.l + w->padding.r + w->margin.l + w->margin.r + w->border_width.l + w->border_width.r;
    size.y += w->padding.t + w->padding.b + w->margin.t + w->margin.b + w->border_width.t + w->border_width.b;
    return size;
}

//==== PANELS ====
#define PANEL_SPACING 5
static uvec2 panel_layout(ui_widget* w, box_constraint bc)
{
    log_debug("Layout!");
    ui_panel* p = (ui_panel*)w;
    uvec2 own_size;
    if (w->fixed) {
        own_size.x = w->pref_w - w->padding.l - w->padding.r;
        own_size.y = w->pref_h - w->padding.t - w->padding.b;
    } else {
        own_size = new_uvec2(UINT16_MAX, UINT16_MAX);
    }
    
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

    own_size = finalize_size(w, own_size);
    return bc_constrain(bc, own_size);
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

void panel_apply_style(ui_panel* p)
{
    // all widgets have to provide every property, 
    // or else it will be left to be the default initialized value, 
    // which may be unpredictable or garbage
    
    // first we register the panel's extra properties
    style_add_props(new_str("main-axis", 10), offsetof(ui_panel, main_axis), PROPERTY_ENUM);
    style_add_props(new_str("cross-axis", 11), offsetof(ui_panel, cross_axis), PROPERTY_ENUM);
    style_add_props(new_str("direction", 10), offsetof(ui_panel, flow), PROPERTY_ENUM);
    // and their enum values, if they are of type enum
    style_add_enum_values(new_str("start", 6), ALIGN_START);
    style_add_enum_values(new_str("center", 7), ALIGN_CENTER);
    style_add_enum_values(new_str("end", 4), ALIGN_END);
    style_add_enum_values(new_str("space-between", 14), ALIGN_SPACE_BETWEEN);
    style_add_enum_values(new_str("space-around", 13), ALIGN_SPACE_AROUND);
    style_add_enum_values(new_str("space-even", 11), ALIGN_SPACE_EVEN);
    style_add_enum_values(new_str("stretch", 8), ALIGN_STRETCH);
    style_add_enum_values(new_str("row", 4), FLOW_ROW);
    style_add_enum_values(new_str("column", 7), FLOW_COLUMN);

    // panels do not have any extra states, so state_count ^= STATE_COUNT
    ui_style_state* style = new_styles(new_str("panel-default", 14), STATE_COUNT);
    ui_state_transition* active_state = style_config_state(style, STATE_ACTIVE, 4);
    style_add_changes(active_state, new_str("shrink", 7), PROP_VALUE(float, PROPERTY_FLOAT, 1.));
    style_add_changes(active_state, new_str("padding", 8), PROP_VALUE(urect16, PROPERTY_URECT16, make_urect16(5, 5, 5, 5)));
    style_add_changes(active_state, new_str("background", 11), PROP_VALUE(color, PROPERTY_COLOR, 0xFF181818));
    style_add_changes(active_state, new_str("foreground", 11), PROP_VALUE(color, PROPERTY_COLOR, 0xFFFFFFFF));

    ui_state_transition* hovered_state = style_config_state(style, STATE_HOVERED, 1);
    style_add_changes(hovered_state, new_str("background", 11), PROP_VALUE(color, PROPERTY_COLOR, 0xFFcccccc));
}

ui_panel* panel(ui_widget* parent)
{
    ui_panel* result = (ui_panel*)make_widget(parent, parent->root, sizeof(ui_panel), panel_message, panel_layout, panel_paint);
    panel_apply_style(result);
    return result;
}

ui_panel* panel_as_root(ui_window* root)
{
    ui_panel* result = (ui_panel*)make_widget(null, root, sizeof(ui_panel), panel_message, panel_layout, panel_paint);
    result->w.fixed = true;
    panel_apply_style(result);
    return result;
}
