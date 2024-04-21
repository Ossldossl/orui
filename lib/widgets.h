#pragma once
#include "platform.h"
#include "misc.h"
#include "style.h"

typedef enum ui_msg ui_msg;

typedef struct ui_widget ui_widget;
typedef struct ui_window ui_window;

typedef enum ui_align ui_align;
typedef enum ui_flow_direction ui_flow_direction;
typedef struct ui_panel ui_panel;

typedef struct ui_label ui_label;
typedef struct ui_button ui_button;

typedef i32 (*message_handler)(ui_widget* widget, ui_msg msg, i32 di, void* dp);
typedef uvec2 (*layout_handler)(ui_widget* widget, box_constraint bc); 
typedef void (*paint_handler)(ui_widget* widget, painter* p);
typedef void (*empty_event_handler)(ui_widget* widget);
typedef void (*bool_event_handler)(ui_widget* widget, bool value);
typedef void (*str_event_handler)(ui_widget* widget, str value);

void orui_move(ui_widget* w, urect16 new_bounds, bool relayout);
void orui_repaint(ui_widget* w);
void orui_relayout(ui_widget* w);

void widget_set_props(ui_widget* w, u8 state_id, str id, ui_state_property value);
void widget_set_prop(ui_widget* w, u8 state_id, char* id, u8 id_len, ui_state_property value);
void widget_finalize(ui_widget* w);

enum ui_msg {
    MSG_LDOWN,
    MSG_CLICK,

    MSG_RDOWN,
    MSG_RUP,
    
    MSG_HOVER_START,
    MSG_MOUSE_MOVE,
    MSG_HOVER_END,

    MSG_UPDATE,
    MSG_CREATE,
    MSG_REMOVE,

    MSG_STATE_CHANGED,

    MSG_COUNT,
};

struct ui_widget {
    ui_window* root;
    ui_widget* parent; 

    ui_widget** children;
    u16 child_count;
    u16 child_cap;

    str id;
    str hero_id; // for transitions

    bool dirty;
    bool fixed;
    // when a widget is absolute, it isn't part of the main widget tree, and rather gets its "own" tree
    urect16 bounds;
    bool absolute;
    urect16 offset; // for relative and absolute positioning
    u16 min_w, min_h;
    u16 max_w, max_h;
    u16 pref_w, pref_h;
    float grow, shrink;

    // siehe css box model
    urect16 border_width;
    urect16 margin;
    urect16 padding;
    urect16 border_radius;

    // TODO: image backgrounds
    color background;
    color foreground; // e.g. text
    color border;
    u16 zindex;

    message_handler wmsg;
    layout_handler layout_handler;
    paint_handler paint_handler;

    ui_style_state* style_state;
};

struct ui_window {
    // has to be first!
    platform_window n;
    uvec2 dims;
    ui_panel* root;
    bool repaint;
    ui_widget* relayout_start_point;
    u16 relayout_start_depth;
    u16 cur_zindex;

    ui_widget* hovered;
    ui_widget* active;
    uvec2 last_mouse_pos;
};

//==== PANEL ====
enum ui_align {
    ALIGN_START,
    ALIGN_CENTER,
    ALIGN_END,
    ALIGN_SPACE_BETWEEN,
    ALIGN_SPACE_AROUND,
    ALIGN_SPACE_EVEN,
    ALIGN_STRETCH,
};

enum ui_flow_direction {
    FLOW_ROW,
    FLOW_COLUMN,
};

struct ui_panel {
    ui_widget w;
    
    //@Property: main-axis enum { "start", "center", "end", "space-between", "space-around", "space-even", "stretch"}
    ui_align main_axis;
    //@Property: cross-axis enum { "start", "center", "end", "space-between", "space-around", "space-even", "stretch"}
    ui_align cross_axis;
    //@Property: flow enum { "row", "column" }
    ui_flow_direction flow;
};

void panel_init(void);
ui_panel* panel(ui_widget* parent, str id);
ui_panel* panel_as_root(ui_window* root, str id);

//==== BUTTON ====
typedef enum ui_button_states {
    BUTTON_STATE_ACTIVE,
    BUTTON_STATE_INACTIVE,
    BUTTON_STATE_HOVERED,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_COUNT,
} ui_button_states;

struct ui_button {
    ui_widget w;
    char* label;   
    empty_event_handler on_click;
};

//==== CHECKBOX ====
struct ui_check {
    ui_widget w;
    bool checked;
    bool_event_handler on_change;
};