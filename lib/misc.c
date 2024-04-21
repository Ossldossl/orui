#include "misc.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

u16 str_len(char* data)
{
    char* start = data;
    while (*data != 0) {
        data++;
    }
    return (u64)data - (u64)start;
}

str make_str(char* data, u16 len)
{
    if (len == 0) len = str_len(data);
    return (str) {
        .data = data, .len = len
    };
}

str concat(str a, str b)
{
    str result;
    // excluding null character
    result.len = a.len + b.len;
    result.data = malloc(result.len+1);
    memcpy(result.data, a.data, a.len);
    memcpy(result.data + a.len, b.data, b.len);
    result.data[result.len] = '\0';
    return result;
}

str cconcat(char* a, char* b)
{
    str result;
    u32 al = strlen(a); u32 bl = strlen(b);
    result.len = al + bl;
    result.data = malloc(result.len + 1);
    memcpy(result.data, a, al);
    memcpy(result.data + al, b, bl);
    result.data[result.len] = '\0';
    return result;
}

str cconcats1(char* a, str b)
{
    str result;
    u32 al = strlen(a);
    result.len = al + b.len;
    result.data = malloc(result.len+1);
    memcpy(result.data, a, al);
    memcpy(result.data + al, b.data, b.len);
    result.data[result.len] = '\0';
    return result;
}

str cconcats2(str a, char* b)
{
    str result;
    u32 bl = strlen(b);
    result.len = bl + a.len;
    result.data = malloc(result.len+1);
    memcpy(result.data, a.data, a.len);
    memcpy(result.data + a.len, b, bl);
    result.data[result.len] = '\0';
    return result;
}

//=== POINTS ====
uvec2 uvec2_sub(uvec2 a, uvec2 b) 
{
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

//==== RECTS ====
urect16 rect_clip(urect16 r, urect16 clip)
{
    r.t = ui_max(r.t, clip.t);
    r.l = ui_max(r.l, clip.l);
    r.b = ui_min(r.b, clip.b);
    r.r = ui_min(r.r, clip.r);
    return r;
}

urect16 rect_bounding_box(urect16 a, urect16 b)
{
    a.t = ui_min(a.t, b.t);
    a.l = ui_min(a.l, b.l);
    a.b = ui_max(a.b, b.b);
    a.r = ui_max(a.r, b.r);
    return a;
}

bool rect_equals(urect16 a, urect16 b)
{
    return reinterpret(a, u64) == reinterpret(b, u64);
}

bool rect_contains(urect16 r, upoint16 p)
{
    return 
        p.x >= r.l && p.x < r.r
    &&  p.y >= r.t && p.y < r.b;
}

//==== BOX CONSTRAINT ====
box_constraint bc_loose(u16 max_w, u16 max_h)
{
    box_constraint bc;
    bc.max_w = max_w; bc.max_h = max_h;
    bc.min_w = bc.min_h = 0;
    return bc;
}

box_constraint bc_tight(u16 w, u16 h)
{
    box_constraint bc;
    bc.min_w = bc.max_w = w;
    bc.min_h = bc.max_h = h;
    return bc;
}

box_constraint bc_tight_from_bounds(urect16 bounds)
{
    box_constraint bc;
    bc.min_w = bc.max_w = bounds.r - bounds.l;
    bc.min_h = bc.max_h = bounds.b - bounds.t;
    return bc;
}

box_constraint bc_loose_from_bounds(urect16 bounds)
{
    box_constraint bc;
    bc.min_h = bc.min_w = 0;
    bc.max_w = bounds.r - bounds.l;
    bc.max_h = bounds.b - bounds.t;
    return bc;
}

box_constraint bc_make(u16 min_w, u16 min_h, u16 max_w, u16 max_h)
{
    box_constraint bc;
    bc.min_w = min_w; bc.min_h = min_h;
    bc.max_w = max_w; bc.max_h = max_h;
    return bc;
}

bool bc_is_height_bounded(box_constraint bc)
{
    return (bc.max_h != UINT16_MAX);
}

bool bc_is_width_bounded(box_constraint bc)
{
    return (bc.max_w != UINT16_MAX);
}

uvec2 bc_constrain(box_constraint bc, uvec2 v)
{
    v.x = ui_max(v.x, bc.min_w);
    v.y = ui_max(v.y, bc.min_h);
    if (bc_is_height_bounded(bc)) {
        v.y = ui_min(v.y, bc.max_h);
    }
    if (bc_is_width_bounded(bc)) {
        v.x = ui_min(v.x, bc.max_w);
    }
    return v;
}