#pragma once
#include "misc.h"

typedef enum ui_easing ui_easing;
typedef enum ui_states ui_states;
typedef enum ui_state_property_kind ui_state_property_kind;
typedef struct ui_state_property_def ui_state_property_def;
typedef struct ui_state_property ui_state_property;
typedef struct ui_state_transition_delta ui_state_transition_delta;
typedef struct ui_state_transition ui_state_transition;
typedef struct ui_style_state ui_style_state;

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
    PROPERTY_BOOL,
    PROPERTY_U16,
    PROPERTY_FLOAT,
    PROPERTY_URECT16,
    PROPERTY_COLOR,
    PROPERTY_STR, // won't be interpolated
    PROPERTY_ENUM,
    PROPERTY_COUNT
};

#ifdef STYLE_STRINGS_IMPLEMENTATION
char* ui_state_property_kind_strings[] = {
    "BOOL",
    "U16",
    "FLOAT",
    "URECT16",
    "COLOR",
    "STR",
    "ENUM",
    "invalid",
};
#endif

struct ui_state_property_def {
    ui_state_property_kind kind;
    u8 offset;
};

struct ui_state_property {
    ui_state_property_kind kind;
    union {
        bool bool_;
        u16 u16_;
        float float_;
        urect16 urect16_;
        color color_;
        str str_;
        u64 enum_;
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

struct ui_style_state {
    // active state -> new state
    // index for transition is cur_state,
    // so there always have to be STATE_COUNT transitions
    // transitions[STATE_COUNT] ^= cur_transition, 
    // which is a synthesized transition 
    // and always the current one being used,
    // whereas all transitions before are "prototypes" for
    // synthesizing this current transition

    u8 cur_state;
    u8 state_count;
    ui_state_transition** transitions;
};

ui_style_state* new_style(char* name, u8 len, u8 state_count);
ui_style_state* new_styles(str name, u8 state_count);
ui_style_state* style_inherit(char* name, u8 name_len, char* from, u8 len);
ui_style_state* style_inherits(str name, str from);
void style_add_prop(char* id, u8 len, u8 offset, ui_state_property_kind kind);
void style_add_props(str id, u8 offset, ui_state_property_kind type);

void style_add_enum_value(char* id, u8 len, u64 value);
void style_add_enum_values(str id, u64 value);
u64 style_get_enum_value(char* id, u8 len);
u64 style_get_enum_values(str id);

ui_state_property_def style_get_prop(char* id, u8 len);
ui_state_property_def style_get_props(str id);

ui_state_transition* style_config_state(ui_style_state* style_state, u8 state_id, u8 change_count);
#define PROP_VALUE(type, type_enum, value) ((ui_state_property){.kind=type_enum, .as.type##_ = value})
#define STYLE_ADD_CHANGE_STATIC(transition, name, type, enum_type, value) style_add_changes(transition, new_str(name, sizeof(name)), PROP_VALUE(type, enum_type, value))
void style_add_change(ui_state_transition* trans, char* id, u8 len, ui_state_property target);
void style_add_changes(ui_state_transition* trans, str id, ui_state_property target);
void style_set_transition(ui_style_state* style_state, u8 state_id, u32 duration, ui_easing easing);
