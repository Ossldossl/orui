#include <stdlib.h>
#include <stdbool.h>
#include "react.h" 
#include "console.h"

#define value_make_impl(type, short_type, enum_type)    \
    value short_type##value(type iv) {                  \
        value result;                                   \
        result.kind = enum_type;                        \
        result.as.type##_ = iv;                         \
        result.obs_count = 0; result.obs_cap = 0;       \
        result.on_change = null;                        \
        return result;                                  \
    }                                                   \

// TODO: type checking
#define value_set_impl(type, short_type, enum_type) \
    type value_set##short_type(value* v, type nv) { \
        v->kind = enum_type;                        \
        type result = v->as.type##_;                \
        v->as.type##_ = nv;                         \
        for (int i = 0; i < v->obs_count; i++) {    \
            v->on_change[i](v);                     \
        }                                           \
        return result;                              \
    }                                               \

#define value_get_impl(type, short_type)    \
    type value_get##short_type(value* v) {  \
        type result = v->as.type##_;        \
        return result;                      \
    }                                       \

value_make_impl(bool, b, VALUE_BOOL)
value_make_impl(i64, i, VALUE_INT)
value_make_impl(u64, u, VALUE_UINT)
value_make_impl(double, d, VALUE_DOUBLE)
value_make_impl(color, c, VALUE_COLOR)
value_make_impl(str, s, VALUE_STR)

value_set_impl(bool, b, VALUE_BOOL)
value_set_impl(i64, i, VALUE_INT)
value_set_impl(u64, u, VALUE_UINT)
value_set_impl(double, d, VALUE_DOUBLE)
value_set_impl(color, c, VALUE_COLOR)
value_set_impl(str, s, VALUE_STR)

value_get_impl(bool, b);
value_get_impl(i64, i);
value_get_impl(u64, u);
value_get_impl(double, d);
value_get_impl(color, c);
value_get_impl(str, s);

void value_connect(value* v, value_callback on_change) 
{
    if (v->obs_cap == v->obs_count) {
        v->obs_cap+=2;
        v->on_change = realloc(v->on_change, v->obs_cap * sizeof(value));
    }
    v->on_change[v->obs_count] = on_change;
    v->obs_count++;
    return;
}

void value_disconnect(value *v, value_callback on_change)
{
    for (int i = 0; i < v->obs_count; i++) {
        value_callback vc = v->on_change[i];
        if (vc == on_change) {
            value_callback swap = v->on_change[v->obs_count-1];
            v->on_change[i] = swap; 
            v->obs_count--;
            return;
        }
    }
    log_error("Failed to disconnect callback: Callback was never registered");
}