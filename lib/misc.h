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

typedef struct px px;

typedef u32 wchar;

typedef struct str str;
typedef struct wstr wstr;

#define null ((void*)0)

//#define min(a,b) a<b?a:b
//#define max(a,b) a>b?a:b

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

//==== PIXEL ====
struct px {
	union {
		u32 rgba;
		struct {
			u8 r,g,b,a;
		};
	};
};
