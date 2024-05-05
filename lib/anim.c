#include <stdlib.h>
#include <string.h>
#include "anim.h"
#include "orui.h"
#include "console.h"

// TODO: custom animations that send MSG_ANIMATE

//#region anim_handlers
double anim_ease_linear(double t)
{
    return t;
}

//#endregion

const anim_handler anim_handlers[EASING_COUNT] = {
    anim_ease_linear,
    anim_ease_linear,
    anim_ease_linear,
    anim_ease_linear,
    anim_ease_linear,
    anim_ease_linear,
    anim_ease_linear,
    anim_ease_linear,
    anim_ease_linear,
    anim_ease_linear,
    anim_ease_linear,
    anim_ease_linear,
    anim_ease_linear,
    anim_ease_linear,
    anim_ease_linear,
    anim_ease_linear,
};

// ptr -> anims
map_t anims = (map_t){0};
ui_anim_state anim_state;

void anim_init(void)
{
    anim_state.anim_cap = 20;
    anim_state.anim_count = 0;
    anim_state.anims = malloc(anim_state.anim_cap * sizeof(ui_anim));
}

char key_buf[9];
char* key_from_ptr(void* p)
{
    memcpy(key_buf, &p, 8);
    key_buf[8] = '\0';
    return key_buf;
}

    #define set_value(enum_type, type) case enum_type: {anim->start.type##_ = *(type*)value;}break;    
void add_anim(void *value, ui_state_property target, u32 duration, ui_easing easing)
{
    ui_anim* possible_anim = map_get(&anims, key_from_ptr(value), 9);
    if (possible_anim != null) {
        log_debug("changing animations!");
        ui_anim* anim = possible_anim; 
        anim->cur = 0.f;
        anim->target = target.as;
        anim->handler = anim_handlers[easing];
        anim->duration = duration;
        if (duration == 0) {
            anim->duration = 200;
        }
        switch (anim->type) {
            set_value(PROPERTY_U16, u16)
            set_value(PROPERTY_BOOL, bool)
            set_value(PROPERTY_FLOAT, float)
            set_value(PROPERTY_URECT16, urect16)
            set_value(PROPERTY_COLOR, color)
            set_value(PROPERTY_STR, str)
        }
        return;
    }

    anim_state.anim_count++;
    if (anim_state.anim_count >= anim_state.anim_cap) {
        anim_state.anim_cap *= 2;
        anim_state.anims = realloc(anim_state.anims, anim_state.anim_cap * sizeof(ui_anim));
    }
    ui_anim* anim = &anim_state.anims[anim_state.anim_count-1];
    anim->handler = anim_handlers[easing];  
    anim->duration = duration; anim->cur = 0.f;
    anim->duration = 200;
    anim->value = value;
    anim->type = target.kind;
    anim->target = target.as;

    switch (anim->type) {
        set_value(PROPERTY_U16, u16)
        set_value(PROPERTY_BOOL, bool)
        set_value(PROPERTY_FLOAT, float)
        set_value(PROPERTY_URECT16, urect16)
        set_value(PROPERTY_COLOR, color)
        set_value(PROPERTY_STR, str)
    }
#undef set_value
    map_set(&anims, key_from_ptr(value), 9, anim);
    orui_set_animating(true);
}

void stop_anim(void *changed_value)
{
    anim_state.anim_count--;
    ui_anim* anim = map_get(&anims, key_from_ptr(changed_value), 9);
    if (anim == null) {
        log_debug("cant stop non-existing animation!"); return;
    }
    *anim = anim_state.anims[anim_state.anim_count];
    map_remove(&anims, key_from_ptr(changed_value), 9);

    if (anim_state.anim_count == 0) {
        orui_set_animating(false);
    }
}

void anim_update(double dt) 
{
//    log_debug("%lf", dt);
    for (int i = 0; i < anim_state.anim_count; i++) {
        ui_anim* a = &anim_state.anims[i];
        a->cur += dt; 
        double t = a->cur / (double)a->duration;
        t = a->handler(t);

        bool last = false;
        if (t > (1 - 1.e-6)) {
            t = 1.f;
            last = true;
        }

        #define interp_val(enum_type)                                                  \
            case enum_type: {                                                          \
                REAL_TYPE_##enum_type *v = a->value;                                   \
                REAL_TYPE_##enum_type start = a->start.UREAL_TYPE_##enum_type;         \
                REAL_TYPE_##enum_type target = a->target.UREAL_TYPE_##enum_type;       \
                *v = (target - start) * t + start;                                     \
            } break;

        switch (a->type) {
            case PROPERTY_U16: {
                u16 *v = a->value;
                u16 start = a->start.u16_;
                u16 target = a->target.u16_;
                i32 diff = target-start;
                diff *= t;
                diff += start;
                *v = diff;
                if (target == 350) {
                    log_debug("100 - %d -> 350", diff);
                }
            } break;
            interp_val(PROPERTY_FLOAT) 

            case PROPERTY_BOOL: {
                if (t > 0.5f) {
                    bool* v = a->value;
                    *v  =a->target.bool_;
                    last = true;
                }
            } break;

            case PROPERTY_COLOR: {
                color *v = a->value;
                u8 sr = r(a->start.color_); u8 tr = r(a->target.color_);
                u8 sg = g(a->start.color_); u8 tg = g(a->target.color_);
                u8 sb = b(a->start.color_); u8 tb = b(a->target.color_);
                u8 sa = a(a->start.color_); u8 ta = a(a->target.color_);

                i16 dr = tr - sr; u8 nr = dr * t + (i16)sr;
                i16 dg = tg - sg; u8 ng = dg * t + (i16)sg;
                i16 db = tb - sb; u8 nb = db * t + (i16)sb;
                i16 da = ta - sa; u8 na = da * t + (i16)sa;

                *v = COLOR(nr, ng, nb, na);
            } break;

            default : {
                log_fatal("not implemented yet!");
            } break;
        }

        #undef interp_val

        if (last) {
            stop_anim(a->value);
        }
    }
}