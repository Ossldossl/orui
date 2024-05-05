#pragma once
#include "misc.h"
#include "style.h"
#include "map.h"

typedef struct ui_anim ui_anim;
typedef struct ui_anim_state ui_anim_state;
typedef double (*anim_handler)(double t);
typedef struct anim_val anim_val;

// TODO: make this smaller
struct ui_anim {
    anim_handler handler;
    float cur;
    u32 duration;
    void* value;
    ui_state_property_kind type;
    ui_state_property_value target;
    ui_state_property_value start;
};

struct ui_anim_state {
    ui_anim* anims;
    u16 anim_cap;
    u16 anim_count; 
};

void anim_init(void);
void anim_update(double dt);
void add_anim(void *value, ui_state_property target, u32 duration, ui_easing easing);
void stop_anim(void* changed_value);