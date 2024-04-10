#include "misc.h"

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

//==== RECTS ====
urect16 rect_clip(urect16 r, urect16 clip)
{
    r.t = max(r.t, clip.t);
    r.l = max(r.l, clip.l);
    r.b = min(r.b, clip.b);
    r.r = min(r.r, clip.r);
    return r;
}

urect16 rect_bounding_box(urect16 a, urect16 b)
{
    a.t = min(a.t, b.t);
    a.l = min(a.l, b.l);
    a.b = max(a.b, b.b);
    a.r = max(a.r, b.r);
    return a;
}

bool rect_contains(urect16 r, upoint16 p)
{
    return 
        p.x >= r.l && p.x < r.r
    &&  p.y >= r.t && p.y < r.b;
}