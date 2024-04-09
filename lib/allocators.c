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
