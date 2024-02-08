#pragma once
#include <stdint.h>
#include "allocators.h"

typedef struct {
    char* text;
    uint32_t size;
} string;

typedef struct {
   char* text;
   uint16_t size; 
} string16;

typedef struct {
    char* text;
    uint8_t size;
} string8;

string make_string(char* text);
string8 make_string8(char* text);
string16 make_string16(char* text);

string* make_stringa(allocator* alloc, char* text);
string8* make_string8a(allocator* alloc, char* text);
string16* make_string16a(allocator* alloc, char* text);

void string_append(allocator* alloc, string* str, char* appendix);
string* string_concat(allocator* alloc, string* a, string* b); // leaves a and b untouched
void string_concata(allocator* alloc, string* a, string* b); // modifies a
