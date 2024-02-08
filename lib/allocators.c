#include "common.h"
#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <sanitizer/asan_interface.h>

void* arena_alloc(allocator* alloc, u32 size)
{
    arena* a = (arena*)alloc;
    arena_body* bucket = a->buckets[a->bucket_count-1];
    void* new_cur = INC_PTR(bucket->cur, size);
    if ((u64)new_cur - (u64)bucket >= ARENA_SIZE) {
        // allocation too big, make new bucket
        a->bucket_count++; HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, &a->buckets, a->bucket_count * sizeof(arena_body*));
        if (a->buckets == null) {
            log_fatal("Failed to realloc bucket ptr array!"); exit(-1);
        }
        arena_body* body = VirtualAlloc(a->buckets[a->bucket_count-2], ARENA_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (body == null) {
            log_fatal("Failed to allocate new body!: %lu", GetLastError()); exit(-1);
        }
        body->cur = ARENA_DATA(body); body->last_alloc_size = 0; body->last = null;
        a->buckets[a->bucket_count-1] = body;
        bucket = body;
    }
    void* result = bucket->cur;
    bucket->last_alloc_size = size;
    bucket->cur = new_cur; 
    return result;
}

void arena_free(allocator* alloc, void* ptr)
{
    arena* a = (arena*) alloc;
    arena_body* bucket = a->buckets[a->bucket_count-1]; 
    void* old_alloc_ptr = INC_PTR(bucket->cur, -bucket->last_alloc_size);
    if (old_alloc_ptr == ptr) {
        log_debug("Freed memory from last allocation!");
        ASAN_POISON_MEMORY_REGION(old_alloc_ptr, bucket->last_alloc_size);
        bucket->cur = ptr;
    }
    // else do nothing, because we can only free the arena all at once
}

void arena_begin_section(arena* arena)
{
    arena_section* sec = arena_alloc((allocator*)arena, sizeof(arena_section));
    arena_body* bucket = arena->buckets[arena->bucket_count-1];
    sec->prev = bucket->last;
    bucket->last = sec;
}

void arena_end_section(arena* arena)
{
    arena_body* bucket = arena->buckets[arena->bucket_count-1];
    ASAN_POISON_MEMORY_REGION(bucket->last, (u64)bucket->cur - (u64)bucket->last);
    bucket->cur = bucket->last; // free memory up until the section
    bucket->last = bucket->last->prev;
}

arena arena_init()
{
    arena result;
    result.a.alloc = arena_alloc;
    result.a.free = arena_free;
    result.bucket_count = 1;

    HANDLE heap = GetProcessHeap();    
    result.buckets = HeapAlloc(heap, HEAP_ZERO_MEMORY, sizeof(arena_body*));
    if (result.buckets == null) {
        log_fatal("Failed to allocate buckets!"); exit(-1);
    }

    arena_body* first = VirtualAlloc(null, ARENA_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (first == null) {
        log_fatal("Failed to allocate arena data: %lu", GetLastError()); exit(-1);
    }
    first->cur = ARENA_DATA(first); first->last = null; first->last_alloc_size = 0;
    return result;
}

void destroy_arena(arena* arena)
{
    for_to(i, arena->bucket_count) {
        VirtualFree(arena->buckets[i], ARENA_SIZE, MEM_RELEASE);
    }
}