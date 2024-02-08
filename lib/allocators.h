#pragma once
#include <stdint.h>

#define ARENA_SIZE UINT32_MAX
#define ARENA_DATA(body) (((char*)body + sizeof(arena_body)))
#define INC_PTR(ptr, inc) (((char*)(ptr)) + (inc))
#define ALLOC(alloc, size) (alloc)->allocate((alloc), (size))
#define FREE(alloc, ptr) (alloc)->free((alloc), (ptr))

typedef struct allocator allocator;
struct allocator {
    void (*free)(allocator* alloc, void* ptr);
    void* (*allocate)(allocator* alloc, uint32_t size);
};

typedef struct arena_section arena_section;

struct arena_section {
    arena_section* prev;
};

typedef struct arena_body arena_body;
struct arena_body {
    void* cur;
    uint32_t last_alloc_size;
    arena_section* last;
    // 2mb memory;
};

typedef struct {
    allocator a;
    arena_body** buckets;
    uint32_t bucket_count; 
} arena;

arena make_arena();
void arena_begin_section(arena* arena);
void arena_end_section(arena* arena);
void destroy_arena(arena* arena);