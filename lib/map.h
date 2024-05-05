#pragma once
#include "misc.h"

typedef struct node_t {
    u64 hash;
    struct node_t* parent;
    struct node_t* left;
    struct node_t* right;
    void* value;
    bool is_red;
} node_t;

typedef struct {
    node_t* root;
} map_t;

void* map_get(map_t* tree, char* key, u32 len);
void map_set(map_t* tree, char* key, u32 len, void* value);
void* map_gets(map_t* tree, str key);
void map_sets(map_t* tree, str key, void* value);
void map_remove(map_t* tree, char* key, u32 len);

node_t* map_get_at(map_t* tree, u32 index);
node_t* map_next(node_t* cur);