#pragma once
#include "misc.h"

typedef struct bucka bucka;
typedef struct arena arena;
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

//==== ARENA ====
typedef struct {
	void* data;
	void* cur;
	u32 last_alloc_size;
} arena_bucket;

struct arena {
	u32 bucket_size; // up to ~4mb
	arena_bucket* buckets; 
	u8 bucket_count;
};

arena arena_init(u32 bucket_size);
void* arena_alloc(arena* arena, u32 size);
void arena_free_last(arena* arena);
void arena_reset(arena* arena);
void arena_destroy(arena* arena);