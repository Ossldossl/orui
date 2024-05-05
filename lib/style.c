#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "allocators.h"
#define STYLE_STRINGS_IMPLEMENTATION
#include "style.h"
#include "map.h"
#include "console.h"
#include "widgets.h"
#include "anim.h"

// map_t of ui_state_property_def(s)
// TODOOOO: somehow check that a property is valid for a given widget
map_t prop_map = (map_t){0};
// map_t of ui_style_state(s)
map_t classes = (map_t){0};
// map_t of u64
map_t enum_values = (map_t){0};

void style_add_prop(char* id, u8 len, u8 offset, ui_state_property_kind kind)
{
    ui_state_property_def def;
    def.kind = kind; def.offset = offset;
    // oh no
    u64 conv_hack = reinterpret(def, u64);
    map_set(&prop_map, id, len, (void*)conv_hack);
}

void style_add_props(str id, u8 offset, ui_state_property_kind kind)
{
    return style_add_prop(id.data, id.len, offset, kind);
}

void style_add_enum_value(char *id, u8 len, u64 value)
{
    u64 v = (u64)map_get(&enum_values, id, len);
    if (v != 0) {
        if (v == value) return;

        log_fatal("Enum value for %s already exists!", id);
        PostQuitMessage(-1);
    }
    map_set(&enum_values, id, len, (void*)value);
}

void style_add_enum_values(str id, u64 value)
{
    return style_add_enum_value(id.data, id.len, value);
}

u64 style_get_enum_value(char *id, u8 len)
{
    void* a = map_get(&enum_values, id, len);
    if (a == null) {
        log_fatal("Enum value %s does not exist!", id);
        PostQuitMessage(-1);
    }
    return (u64)a;
}

u64 style_get_enum_values(str id)
{
    return style_get_enum_value(id.data, id.len);
}

ui_state_property_def style_get_prop(char* id, u8 len)
{
    void* result = map_get(&prop_map, id, len);
    u64 conv_hack = (u64)result;
    return reinterpret(conv_hack, ui_state_property_def);
}

ui_state_property_def style_get_props(str id)
{
    return style_get_prop(id.data, id.len);
}

// default state is always state 0
ui_style_state* new_style(char* name, u8 len, u8 state_count)
{
    if (map_get(&classes, name, len) != null) {
        log_fatal("style class %s already exists!", name);
        PostQuitMessage(-1);
    }
    u32 size = sizeof(ui_style_state) + (state_count-1) * sizeof(ui_state_transition);
    ui_style_state* result = malloc(size);
    result->state_count = state_count;
    result->cur_state = 0;
    memset(&result->transition, 0, (state_count) * sizeof(ui_state_transition));
    map_set(&classes, name, len, result);
    return result;
}

ui_style_state* new_styles(str name, u8 state_count)
{
    return new_style(name.data, name.len, state_count);
}

//TODO: test this
ui_style_state* style_inherit(char* name, u8 name_len, char* from, u8 len)
{
    ui_style_state* from_style = map_get(&classes, from, len);
    if (from_style == null) {
        log_error("Style class %s not found!", from);
    } 

    ui_style_state* result = new_style(name, name_len, from_style->state_count);
    // copy transition data over
    for (int i = 0; i < result->state_count; i++) {
        ui_state_transition* from_trans = &style_get_transition(from_style, i);
        ui_state_transition* to_trans = &style_get_transition(result, i);
        memcpy(to_trans, from_trans, sizeof(ui_state_transition));
        to_trans->deltas = malloc(sizeof(ui_state_transition_delta) * to_trans->delta_cap);
        memcpy(to_trans->deltas, from_trans->deltas, sizeof(ui_state_transition_delta) * to_trans->delta_cap);
    }

    return result;
}

ui_style_state* style_inherits(str name, str from)
{
    return style_inherit(name.data, name.len, from.data, from.len);
}

//=== STYLE CONFIG
void style_set_transition(ui_style_state* style_state, u8 state_id, u32 duration, ui_easing easing)
{
    ui_state_transition* trans = &style_get_transition(style_state, state_id);
    trans->duration = duration; trans->ease = easing;
}

void style_add_change(ui_style_state* state, u8 state_id, char* id, u8 len, ui_state_property target)
{
    ui_state_transition* trans = &style_get_transition(state, state_id);

    ui_state_property_def def = style_get_prop(id, len);
    if (reinterpret(def, u64) == 0) {
        log_fatal("style property \"%s\" not found!", id);
        PostQuitMessage(-1);
    }
    ui_state_transition_delta* delta;
    // check if we need to override an offset
    bool replace = false;
    for (int i = 0; i < trans->delta_count; i++) {
        ui_state_transition_delta* d = &trans->deltas[i];
        if (d->offset == def.offset) {
            delta = d;
            replace = true; break;
        }
    }

    if (!replace) {
        trans->delta_count++;
        if (trans->delta_count >= trans->delta_cap) {
            trans->delta_cap += 5;
            trans->deltas = realloc(trans->deltas, trans->delta_cap * sizeof(ui_state_transition_delta));
        }
        delta = &trans->deltas[trans->delta_count-1];
    } 
    
    if (target.kind != def.kind) {
        log_fatal("expected type %s for property %s, but got %s", ui_state_property_kind_strings[def.kind], id, ui_state_property_kind_strings[target.kind]);
        PostQuitMessage(-1);
    }
    delta->offset = def.offset;
    delta->target = target;
}

void style_add_changes(ui_style_state* state, u8 state_id, str id, ui_state_property target)
{
    return style_add_change(state, state_id, id.data, id.len, target);
}

//==== STATE CHANGING ====
// TODO: check if offsets are applicable
// TODO: support animation

#define animate_value(enum_type)                                            \
    case enum_type: {                                                       \
        REAL_TYPE_##enum_type* prop = (REAL_TYPE_##enum_type*)unknown_prop; \
        if (*prop != d->target.as.UREAL_TYPE_##enum_type) {                 \
            if (dont_animate) {                                             \
                *prop = d->target.as.UREAL_TYPE_##enum_type;                \
            } else {                                                        \
                log_debug("adding anim of type %s", #enum_type);            \
                add_anim(prop, d->target, trans->duration, trans->ease);    \
            }                                                               \
        }                                                                   \
    } break;

static void apply_transition(ui_widget* w, ui_state_transition* trans, bool dont_animate)
{
    for (int i = 0; i < trans->delta_count; i++) {
        ui_state_transition_delta* d = &trans->deltas[i];
        void* unknown_prop = (char*)w + d->offset;
        
        switch (d->target.kind) {
            case PROPERTY_U16: {
              u16 *prop = (u16 *)unknown_prop;
              if (*prop != d->target.as.u16_) {
                if (dont_animate) {
                  *prop = d->target.as.u16_;
                } else {
                  _log_("f:\\Programmieren\\cpp\\orui\\lib\\style.c", 199,
                        LOG_DEBUG, "adding anim of type %s", "PROPERTY_U16");
                  add_anim(prop, d->target, trans->duration, trans->ease);
                }
              }
            } break;
            animate_value(PROPERTY_FLOAT) 
            case PROPERTY_COLOR: {
              color *prop = (color *)unknown_prop;
              if (*prop != d->target.as.color_) {
                if (dont_animate) {
                  *prop = d->target.as.color_;
                } else {
                  _log_("f:\\Programmieren\\cpp\\orui\\lib\\style.c", 212,
                        LOG_DEBUG, "adding anim of type %s", "PROPERTY_COLOR");
                  add_anim(prop, d->target, trans->duration, trans->ease);
                }
              }
            } break;

            case PROPERTY_URECT16: {
                urect16 *prop = (urect16 *)unknown_prop;
                if (!rect_equals(*prop, d->target.as.urect16_)) {
                  if (dont_animate) {
                    *prop = d->target.as.urect16_;
                  } else {
                    add_anim(prop, d->target, trans->duration, trans->ease);
                  }
                }
            } break;
            case PROPERTY_ENUM: {
                u64* prop = (u64*)unknown_prop;
                *prop = d->target.as.enum_;
            } break;
            case PROPERTY_BOOL: {
                bool* prop = (bool*)unknown_prop;
                *prop = d->target.as.bool_;
            } break;
            case PROPERTY_STR: {
                str* prop = (str*)unknown_prop;
                *prop = d->target.as.str_;
            } break;
        }
    }
}
#undef animate_value

void orui_set_state_forced(ui_widget *w, u8 state_id, bool dont_animate)
{

    // collect all changes we need to make
    //  first we need to get from the current state to the active state
    ui_state_transition* to_active = null;
    if (w->style_state->cur_state != 0) {
        to_active = &style_get_transition(w->style_state, 0);
        apply_transition(w, to_active, dont_animate);
    }

    // then we need to get from the active state to the new state
    // duplicate values are handled (overriden) by the animation system, so we don't need to do any bookkeeping here
    ui_state_transition* to_new = &style_get_transition(w->style_state, state_id);
    if (to_new && to_new != to_active) {
        apply_transition(w, to_new, dont_animate);
    }

    w->style_state->cur_state = state_id;

    log_debug("new_width: %u", w->pref_w);
    log_debug("new_color: %lu", w->background);

    orui_relayout(w);
    orui_repaint(w);
}

void orui_set_state(ui_widget *w, u8 state_id, bool dont_animate)
{
    if (state_id == w->style_state->cur_state) return;
    return orui_set_state_forced(w, state_id, dont_animate);
}