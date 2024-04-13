#pragma once
#include "misc.h"

typedef enum {
    VALUE_BOOL,
    VALUE_INT,
    VALUE_UINT,
    VALUE_DOUBLE,
    VALUE_STR,
    VALUE_COLOR,
    VALUE_COUNT
} value_kind;

typedef struct value value;
typedef void (*value_callback)(value* v);

struct value {
    value_kind kind;
    union {
        bool bool_;
        i64 i64_;
        u64 u64_;
        double double_;
        str str_;
        color color_;
    } as;
    // NOTE: order is only guaranteed when you dont remove observants
    value_callback* on_change;
    u8 obs_count;
    u8 obs_cap;
};

#define value_make_prot(type, short_type)   \
    value short_type##value(type iv);        \

#define value_set_prot(type, short_type)            \
    type value_set##short_type(value* v, type nv);  \

#define value_get_prot(type, short_type)  \
    type value_get##short_type(value* v); \

value_make_prot(bool, b)
value_make_prot(i64, i)
value_make_prot(u64, u)
value_make_prot(double, d)
value_make_prot(color, c)
value_make_prot(str, s)

value_set_prot(bool, b)
value_set_prot(i64, i)
value_set_prot(u64, u)
value_set_prot(double, d)
value_set_prot(color, c)
value_set_prot(str, s)

value_get_prot(bool, b);
value_get_prot(i64, i);
value_get_prot(u64, u);
value_get_prot(double, d);
value_get_prot(color, c);
value_get_prot(str, s);

void value_connect(value* v, value_callback on_change);
void value_disconnect(value* v, value_callback on_change);