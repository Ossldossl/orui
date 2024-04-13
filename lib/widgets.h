#pragma once
#include "platform.h"
#include "misc.h"

typedef enum ui_msg ui_msg;
typedef enum ui_easing ui_easing;
typedef enum ui_states ui_states;
typedef enum ui_state_property_kind ui_state_property_kind;
typedef struct ui_state_property ui_state_property;
typedef struct ui_state_transition_delta ui_state_transition_delta;
typedef struct ui_state_transition ui_state_transition;

typedef struct ui_widget ui_widget;
typedef struct ui_window ui_window;
typedef enum ui_align ui_align;
typedef enum ui_flow_direction ui_flow_direction;
typedef struct ui_panel ui_panel;
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

// see https://easings.net
enum ui_easing {
    EASE_LINEAR,
    EASE_IN_SINE,
    EASE_OUT_SINE,
    EASE_INOUT_SINE,

    EASE_IN_QUAD,
    EASE_OUT_QUAD,
    EASE_INOUT_QUAD,

    EASE_IN_CUBIC,
    EASE_OUT_CUBIC,
    EASE_INOUT_CUBIC,

    EASE_IN_QUART,
    EASE_OUT_QUART,
    EASE_INOUT_QUART,

    EASE_IN_ELASTIC,
    EASE_OUT_ELASTIC,
    EASE_INOUT_ELASTIC,
};

enum ui_states {
    STATE_ACTIVE,
    STATE_INACTIVE,
    STATE_HOVERED,
    STATE_PRESSED,
    
    STATE_COUNT,
};

enum ui_state_property_kind {
    PROPERTY_U16,
    PROPERTY_FLOAT,
    PROPERTY_URECT16,
    PROPERTY_COLOR,
    PROPERTY_STR, // won't be interpolated
    PROPERTY_COUNT
};

struct ui_state_property {
    ui_state_property_kind kind;
    union {
        u16 u16_;
        float float_;
        urect16 urect16_;
        color color_;
        str str_;
    } as;
};

struct ui_state_transition_delta {
    u8 offset; // byte offset from start to property
    ui_state_property target;
};

struct ui_state_transition {
    u32 duration; // in ms
    ui_easing ease;
    
    u8 delta_count;
    ui_state_transition_delta deltas[1];
};

struct ui_widget {
    ui_window* root;
    ui_widget* parent; 

    ui_widget** children;
    u16 child_count;
    u16 child_cap;

    bool dirty;
    bool fixed;
    urect16 bounds;
    u16 min_w, min_h;
    u16 max_w, max_h;
    u16 pref_w, pref_h;
    float grow, shrink;

    // siehe css box model
    urect16 border_width;
    urect16 margin;
    urect16 padding;

    // TODO: image backgrounds
    color background;
    color foreground; // e.g. text
    color border;
    urect16 border_radius;
    u16 zindex;

    message_handler wmsg;
    layout_handler layout_handler;
    paint_handler paint_handler;

    u8 cur_state;
    // active state -> new state
    // index for transition is cur_state,
    // so there always have to be STATE_COUNT-1 transitions
    // transitions[STATE_COUNT] ^= cur_transition, 
    // which is a synthesized transition 
    // and always the current one being used,
    // whereas all transitions before are "prototypes" for
    // synthesizing this current transition
    ui_state_transition transitions[STATE_COUNT];
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

ui_panel* panel(ui_widget* parent);
ui_panel* panel_as_root(ui_window* root);

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