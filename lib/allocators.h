#pragma once
#include "misc.h"

typedef struct bucka bucka;
//==== Bucket allocator =====
struct bucka {
	void* data;
	void* free;
	u16 element_size;
	u32 capacity;
	u32 used;
};

bucka bucka_init(u16 element_count, u16 element_size);
void* bucka_alloc(bucka* b);
void bucka_free(bucka* b, void* ptr);
void bucka_destroy(bucka* b);
