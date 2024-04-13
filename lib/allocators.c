#include "allocators.h"
#include "misc.h"
#include "console.h"

#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//==== BUCKET ALLOCATOR ====
typedef struct {
	u32 left_offset;
	u32 right_offset;
} empty_bucket;

// TODO: switch to real offsets instead of multiples of element_size

bucka bucka_init(u16 element_count, u16 element_size)
{
	bucka result;
	result.element_size = max(8, element_size);
	result.capacity = element_count;
	result.used = 0;
	result.data = VirtualAlloc(null, result.capacity * result.element_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	result.free = result.data;
	
	// iterate over every bucket and make it point to the next
	char* b = result.data; char* end = b + result.capacity * (element_size-1);
	
	// the first bucket header
	*(empty_bucket*)b = (empty_bucket) { .left_offset = 0, .right_offset = 1 };
	b += element_size;

	while (b < end) {
		*(empty_bucket*)b = (empty_bucket) { .left_offset = 1, .right_offset = 1 };
		b += element_size;
	}
	// the last bucket points to null
	*(empty_bucket*)(end-element_size) = (empty_bucket) { .left_offset = 1, .right_offset = 0 };

	return result;
}

void* bucka_alloc(bucka* b)
{
	void* result = b->free;
	empty_bucket* prev_slot = b->free;
	if (prev_slot->right_offset == 0) {
		log_fatal("Bucket allocator is empty!"); exit(-1);
	}
	b->free = ((char*)b->free) + prev_slot->right_offset * b->element_size;
	return result;
}

void bucka_free(bucka* b, void* ptr)
{
	if (ptr == null) return;
	u64 offset = ((u64)ptr - (u64)b->data);
	// ptr was not allocated by this bucka
	if (offset % b->element_size != 0) return; 

	if (ptr < b->free) {
		offset = (u64)b->free - (u64)ptr;
		offset /= b->element_size;
		empty_bucket* prev_f = b->free;
		prev_f->left_offset = offset;
		b->free = ptr;
		empty_bucket* new_f = ptr;
		new_f->right_offset = offset; new_f->left_offset = 0;
		return;
	} else if (ptr > b->free) {
		empty_bucket* prev = b->free;
		empty_bucket* cur = b->free;

		while ((u64)cur > (u64)ptr) {
			prev = cur;
			cur = ((char*) cur) + cur->right_offset * b->element_size;
		}
		u64 poff = (u64)ptr - (u64)prev; poff /= b->element_size;
		u64 coff = (u64)cur - (u64)ptr; coff /= b->element_size;

		prev->right_offset = poff; cur->left_offset = coff;
		empty_bucket* new_f = ptr;
		new_f->left_offset = poff; new_f->right_offset = coff;
		return;
	} else {
		log_fatal("Unreachable!"); return;
	}
}

void bucka_destroy(bucka* b)
{
	VirtualFree(b->data, b->element_size * b->capacity, MEM_DECOMMIT | MEM_RELEASE);
	return;
}

//==== ARENA ====
arena arena_init(u32 bucket_size)
{
	arena result;
	result.bucket_count = 1;
	result.bucket_size = bucket_size;
	result.buckets = malloc(1 * sizeof(arena_bucket));
	arena_bucket* bucket = &result.buckets[0];
	bucket->data = malloc(bucket_size);
	bucket->cur = bucket->data;
	bucket->last_alloc_size = 0;
	return result;
}

void* arena_alloc(arena* arena, u32 size)
{
	arena_bucket* bucket = &arena->buckets[arena->bucket_count-1];
	u64 diff = (u64)((u64)bucket->cur + size) - (u64)bucket->data;
	if (diff > arena->bucket_size) {
		// alloc new bucket
		arena->bucket_count++;
		arena->buckets = realloc(arena->buckets, arena->bucket_count * sizeof(arena_bucket));
		bucket = &arena->buckets[arena->bucket_count-1];
		bucket->data = malloc(arena->bucket_size);
		bucket->cur = bucket->data;
		bucket->last_alloc_size = 0;
	}
	void* result = bucket->cur;
	bucket->cur = ((char*)bucket->cur) + size;
	return result;
}

void arena_free_last(arena* arena) 
{
	arena_bucket* bucket = &arena->buckets[arena->bucket_count-1];
	bucket->cur = ((char*)bucket->cur) - bucket->last_alloc_size;
}

void arena_reset(arena* arena)
{
	// free all other buckets, except the first one
	arena->buckets = realloc(arena->buckets, sizeof(arena_bucket));
	arena_bucket* bucket = &arena->buckets[0];
	bucket->cur = bucket->data;
	arena->bucket_count = 1;
}

void arena_destroy(arena* arena)
{
	for (int i = 0; i < arena->bucket_count; i++) {
		arena_bucket* bucket = &arena->buckets[i];
		free(bucket->data);
	}
	free(arena->buckets);
}
