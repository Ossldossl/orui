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

typedef struct upoint16 upoint16;
typedef struct rect rect;
typedef struct urect16 urect16;

#define null ((void*)0)
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

//==== UTF-8 ====

//==== STRINGS ====
struct str {
    char* data; // null terminated
    u16 len;
};

u16 str_len(char* data);
str make_str(char* data, u16 len);

struct wstr {
    wchar* data; // null terminated
    u16 len;
};

wstr str_to_wstr(str* a);

//==== COLOR ====
typedef u32 ui_color;
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
bool rect_contains(urect16 r, upoint16 p);