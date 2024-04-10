#pragma once
#include "platform.h"
#include "misc.h"

typedef enum ui_msg ui_msg;
typedef enum ui_easing ui_easing;
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
typedef void (*empty_event_handler)(ui_widget* widget);
typedef void (*bool_event_handler)(ui_widget* widget, bool value);
typedef void (*str_event_handler)(ui_widget* widget, str value);

enum ui_msg {
    MSG_LAYOUT,
    MSG_PAINT,
    MSG_LDOWN,
    MSG_CLICK,
    MSG_RDOWN,
    MSG_RUP,
    MSG_HOVER_START,
    MSG_HOVER_END,
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

struct ui_widget {
    ui_window* root;
    ui_widget* parent; 
    ui_widget* children;

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
    ui_color background;
    ui_color foreground; // e.g. text
    ui_color border;
    urect16 border_radius;

    message_handler wmsg; // widget-class message
    message_handler umsg; // user message handler

    u8 cur_state;
    // active state -> new state
    // index for transition is cur_state,
    // so there always have to be STATE_COUNT-1 transitions
    // transitions[STATE_COUNT] ^= cur_transition, 
    // which is a synthesized transition 
    // and always the current one being used,
    // whereas all transitions before are "prototypes" for
    // synthesizing this current transition
    // TODO: update make_widget()
    ui_state_transition* transitions;
};

enum ui_state_property_kind {
    PROPERTY_U16,
    PROPERTY_FLOAT,
    PROPERTY_URECT16,
    PROPERTY_COLOR,
    PROPERTY_COUNT
};

struct ui_state_property {
    ui_state_property_kind kind;
    union {
        u16 u16_;
        float float_;
        urect16 urect16_;
        ui_color color_;
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

struct ui_window {
    ui_widget w;    
    platform_window n;
};

//==== PANEL ====
enum ui_align {
    ALIGN_START,
    ALIGN_CENTER,
    ALIGN_END,
    ALIGN_SPACE_BETWEEN,
    ALIGN_SPACE_AROUND,
    ALIGN_STRETCH,
};

enum ui_flow_direction {
    FLOW_ROW,
    FLOW_COLUMN,
};

struct ui_panel {
    ui_widget w;
    
    ui_align main_axis;
    ui_align cross_axis;
    ui_flow_direction flow;
};

ui_panel* panel(ui_widget* parent);

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