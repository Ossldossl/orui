#pragma once

#include <stdbool.h>

typedef signed char        i8;
typedef short              i16;
typedef int                i32;
typedef long long          i64;
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef u32 wchar;

typedef struct str str;
typedef struct wstr wstr;

typedef struct box_constraint box_constraint;
typedef struct upoint16 upoint16;
typedef struct upoint16 uvec2;
typedef struct rect rect;
typedef struct urect16 urect16;

#define null ((void*)0)
#define ui_max(a,b) (((a) > (b)) ? (a) : (b))
#define ui_min(a,b) (((a) < (b)) ? (a) : (b))
#define ui_abs(a) (a = a<0 ? a*-1 : a)
#define reinterpret(value, as) (*(as*)&value)

#define foreach(name, array, type)          \
	for (int i = 0; i < array->len; i++) {  \
		type name = array_get(i);           


//==== UTF-8 ====

//==== STRINGS ====
struct str {
    char* data; // null terminated
    u16 len;
};

u16 str_len(char* data);
str make_str(char* data, u16 len);
#define new_str(_data, _len) (str){.data=_data,.len=_len}
#define snew_str(_data) (str){.data=_data,.len=sizeof(_data)}

struct wstr {
    wchar* data; // null terminated
    u16 len;
};

wstr str_to_wstr(str* a);
// TODO: str_cmp
bool str_cmp(str a, str b);
str concats(str a, str b);
str cconcat(char* a, char* b);
str cconcats1(char* a, str b);
str cconcats2(str a, char* b);

//==== COLOR ====
typedef u32 color;
#define a(color) ((color&0xFF000000) >> 24)
#define r(color) ((color&0x00FF0000) >> 16)
#define g(color) ((color&0x0000FF00) >>  8)
#define b(color) ((color&0x000000FF)      )
#define COLOR(r,g,b,a) ((a<<24) + (r<<16) + (g<<8) + (b))

//==== FIXED POINT NUMBERS ====
// 8.8 bits
/*struct fixed88 {
	union {
		struct {
			u8 high, low;
		};
		u16 value;
	};
};

// 24.8 bits
typedef u32 fixed32;*/

//==== POINT ====
struct upoint16 {
	u16 x, y;
};

#define make_upoint16(_x, _y) (upoint16){.x=_x,.y=_y}
#define new_uvec2(_x, _y) (uvec2){.x=_x, .y=_y}

uvec2 uvec2_sub(uvec2 a, uvec2 b);

//==== RECTS ====
// TODO: Make this more space efficient
struct rect {
	float t, l, b, r;
};

struct urect16 {
	u16 t, l, b, r;
};

#define make_urect16(_t, _l, _b, _r) (urect16){.t=_t, .l=_l, .b=_b, .r=_r}

urect16 rect_clip(urect16 r, urect16 clip);
urect16 rect_bounding_box(urect16 a, urect16 b);
bool rect_equals(urect16 a, urect16 b);
bool rect_contains(urect16 r, upoint16 p);

//==== BOX CONSTRAINT ====
struct box_constraint {
	u16 min_w, min_h;
	u16 max_w, max_h;
};

box_constraint bc_loose(u16 max_w, u16 max_h);
box_constraint bc_tight(u16 w, u16 h);
box_constraint bc_loose_from_bounds(urect16 bounds);
box_constraint bc_tight_from_bounds(urect16 bounds);
box_constraint bc_make(u16 min_w, u16 min_h, u16 max_w, u16 max_h);
bool bc_is_height_bounded(box_constraint bc);
bool bc_is_width_bounded(box_constraint bc);
uvec2 bc_constrain(box_constraint bc, uvec2 v);
