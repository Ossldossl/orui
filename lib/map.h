#pragma once
#include "misc.h"

typedef struct map_t {
    struct map_t* left;
    struct map_t* right;
    struct map_t* parent;
    u64 hash;
    void* value;
    bool is_red;
} map_t;

void* map_get(map_t* root, char* key, u32 len);
void map_set(map_t* root, char* key, u32 len, void* value);
void* map_gets(map_t* root, str key);
void map_sets(map_t* root, str key, void* value);

map_t* map_get_at(map_t* root, u32 index);
map_t* map_next(map_t* cur);