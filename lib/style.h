#pragma once
#include "misc.h"
#include <stdbool.h>

typedef enum ui_easing ui_easing;
typedef enum ui_states ui_states;
typedef enum ui_state_property_kind ui_state_property_kind;
typedef struct ui_state_property_def ui_state_property_def;
typedef struct ui_state_property ui_state_property;
typedef union ui_state_property_value ui_state_property_value;
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
    EASING_COUNT,
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

//#define bool bool

#define UREAL_TYPE_PROPERTY_BOOL bool_
#define UREAL_TYPE_PROPERTY_U16 u16_
#define UREAL_TYPE_PROPERTY_FLOAT float_
#define UREAL_TYPE_PROPERTY_URECT16 urect16_
#define UREAL_TYPE_PROPERTY_COLOR color_
#define UREAL_TYPE_PROPERTY_STR str_
#define UREAL_TYPE_PROPERTY_ENUM u64_

#define REAL_TYPE_PROPERTY_BOOL bool
#define REAL_TYPE_PROPERTY_U16 u16
#define REAL_TYPE_PROPERTY_FLOAT float
#define REAL_TYPE_PROPERTY_URECT16 urect16
#define REAL_TYPE_PROPERTY_COLOR color
#define REAL_TYPE_PROPERTY_STR str
#define REAL_TYPE_PROPERTY_ENUM u64

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

union ui_state_property_value {
    bool bool_;
    u16 u16_;
    float float_;
    urect16 urect16_;
    color color_;
    str str_;
    u64 enum_;
};

struct ui_state_property {
    ui_state_property_kind kind;
    ui_state_property_value as;
};

struct ui_state_transition_delta {
    u8 offset; // byte offset from start to property
    ui_state_property target;
};

struct ui_state_transition {
    u32 duration; // in ms
    ui_easing ease;
    
    u8 delta_count;
    u8 delta_cap;
    ui_state_transition_delta* deltas;
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
    ui_state_transition transition;
};

#define style_get_transition(style, index) (&style->transition)[index]

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

#define GET_TYPE(type_enum) UREAL_TYPE_##type_enum
#define PROP_VALUE(type_enum, value) ((ui_state_property){.kind=type_enum, .as.GET_TYPE(type_enum) = value})
void style_add_change(ui_style_state* state, u8 state_id, char* id, u8 len, ui_state_property target);
void style_add_changes(ui_style_state* state, u8 state_id, str id, ui_state_property target);
void style_set_transition(ui_style_state* style_state, u8 state_id, u32 duration, ui_easing easing);
