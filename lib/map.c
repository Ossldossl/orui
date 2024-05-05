#include "map.h"

#include <stdlib.h>
#include <string.h>

u64 fnv1a(char* start, char* end)
{
    const u64 magic_prime = 0x00000100000001b3;
    u64 hash = 0xcbf29ce484222325;
    for (; start <= end; start++) {
        hash = (hash ^ *start) * magic_prime;
    }
    return hash;
}

void* map_get_node(map_t* tree, char* key, u32 len)
{
    if (len == 0) {
        len = strlen(key);
    }
    u64 hash = fnv1a(key, key + len-1);
    node_t* cur = tree->root;
    while (true) {
        if (cur == null || cur->hash == hash) break;
//        cur = hash < cur->hash ? cur->left : cur->right;
        if (hash < cur->hash) {
            cur = cur->left;
        } else {
            cur = cur->right;
        }
    }
    return cur;
}

void* map_get(map_t* tree, char* key, u32 len)
{
    node_t* result = map_get_node(tree, key, len);
    return result ? result->value : null;
}

static void rotate_right(map_t* tree, node_t* x)
{
    node_t* y = x->left;
    x->left = y->right;
    if (x->left) {
        x->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == null) {
        // root
        tree->root = y;
    }
    else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->right = x;
    x->parent = y;
}

static void rotate_left(map_t* tree, node_t* x)
{
    node_t* y = x->right;
    x->right = y->left;
    if (y->left) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == null) {
        tree->root = y;
    }
    else if (x == x->parent->left) {
        x->parent->left = y;
    }
    else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

static void rebalance_tree(map_t* tree, node_t* cur)
{
    cur->is_red = true;
    while ( (cur->parent) && (cur->parent->is_red)) {
        if (cur->parent->parent == null) break;
        if (cur->parent == cur->parent->parent->left) {
            node_t* uncle = cur->parent->parent->right;
            if (uncle == null) break;
            if (uncle->is_red) {
                // case 1: switch colors
                cur->parent->is_red = false; uncle->is_red = false;
                cur->parent->parent->is_red = true;
                cur = cur->parent->parent;
            } else {
                if (cur == cur->parent->right) {
                    // case 2: move cur up and rotate
                    cur = cur->parent;
                    rotate_left(tree, cur);
                }
                else {
                // case 3
                    cur->parent->is_red = false;
                    cur->parent->parent->is_red = true;
                    rotate_right(tree, cur->parent->parent);
                }
            }
        } else {
            node_t* uncle = cur->parent->parent->left;
            if (uncle == null) break;
            if (uncle->is_red) {
                // case 1: switch colors
                cur->parent->is_red = false; uncle->is_red = false;
                cur->parent->parent->is_red = true;
                cur = cur->parent->parent;
            } else {
                if (cur == cur->parent->left) {
                    // case 2: move cur up and rotate
                    cur = cur->parent;
                    rotate_left(tree, cur);
                }
                else {
                // case 3
                    cur->parent->is_red = false;
                    cur->parent->parent->is_red = true;
                    rotate_right(tree, cur->parent->parent);
                }
            }
        }
    }
}

void map_set(map_t* tree, char* key, u32 len, void* value)
{
    if (len == 0) {
        len = strlen(key);
    }
    u64 hash = fnv1a(key, key + len-1);
    node_t* cur = tree->root;
    node_t** place_to_insert = null;
    
    bool search = true;
    if (tree->root == null) {
        place_to_insert = &tree->root;
        search = false;
    }

    while (search) {
        if (hash == cur->hash) {
            cur->value = value;
            return;
        } else if (cur->hash == 0) {
            // tree->root
            cur->hash = hash; cur->value = value;
            return;
        }
        if (hash < cur->hash) {
            if (cur->left == null) {
                place_to_insert = &cur->left; break;
            }
            cur = cur->left;
        }
        else if (hash > cur->hash) {
            if (cur->right == null) {
                place_to_insert = &cur->right; break;
            }
            cur = cur->right;
        }
    }

    *place_to_insert = malloc(sizeof(node_t)); node_t* new = *place_to_insert;
    new->hash = hash; new->left = new->right = null; new->value = value; new->parent = cur;
    new->is_red = true;

    rebalance_tree(tree, new);
}

void map_transplant(map_t* tree, node_t* u, node_t* v)
{
    if (u->parent == null) {
        // is root
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    if (v) {
        v->parent = u->parent;
    }
}

bool is_left_child(node_t* x)
{
    if (x == x->parent->left) {
        return true;
    } else {
        return false;
    }
}

bool is_right_child(node_t* x)
{
    return !is_left_child(x);
}

#define w_left_black(w) (w->left == null || w->left->is_red == false)
#define w_right_black(w) (w->right == null || w->right->is_red == false)

void map_fixup(map_t* tree, node_t* x)
{
    node_t* w = null;
    if (x == null) return;
    while (true) {    
        // case 1
        if (x->is_red) {
            x->is_red = false; return;
        }

        // sibling
        if (x == x->parent->left) {
            w = x->parent->right;
        } else {
            w = x->parent->left;
        }

        // case 1
        if (w->is_red) {
            w->is_red = false;
            x->parent->is_red = true;
            if (is_left_child(x)) {
                rotate_left(tree, x->parent);
                w = x->parent->right;
            } else {
                rotate_right(tree, x->parent);
                w = x->parent->left;
            }
        }
        if (w->is_red) continue;

        if (w_left_black(w) && w_right_black(w)) {
            w->is_red = true;
            x = x->parent;
            if (x->is_red) {
                x->is_red = false; return;
            }
            // x is black
            if (x->parent == null) {
                // x is root
                return;
            }
            // x is black and not the root, so start over
            continue;
        }

        // case 3
        if (is_left_child(x) && w_left_black(w) == false && w_right_black(w) == true)
        {
            // left child is red and right child is black
            w->left->is_red = false;
            w->is_red = true;
            rotate_right(tree, w);
            w = x->parent->right; 
        } else if (is_left_child(x) == false && w_right_black(w) == false && w_left_black(w) == true) 
        {
            w->right->is_red = false;
            w->is_red = true;
            rotate_left(tree, w);
            w = x->parent->left;
        }

        // case 4
        if (is_left_child(x) && w_right_black(w) == false) {
            w->is_red = x->parent->is_red;
            x->parent->is_red = false;
            if (w->right) {
                w->right->is_red = false;
            }
            rotate_left(tree, x->parent);
            return;
        }
        if (is_right_child(x) && w_left_black(w) == false) {
            w->is_red = x->parent->is_red;
            x->parent->is_red = false;
            if (w->left) {
                w->left->is_red = false;
            }
            rotate_right(tree, x->parent);
            return;
        }
    }
}

void map_remove(map_t* tree, char *key, u32 len)
{
    node_t* node = map_get_node(tree, key, len);
    if (node == null) return;

    bool original_is_red = false;
    node_t* x = null;
    if (node->left == null) {
        original_is_red = node->is_red;
        x = node->right;
        map_transplant(tree, node, x);
    } else if (node->right == null) {
        original_is_red = node->is_red;
        x = node->left;
        map_transplant(tree, node, x);
    } else {
        node_t* min = node->right;
        while (min->left) min = min->left;
        original_is_red = min->is_red;

        x = min->right;
        if (min->parent == node && x) {
            x->parent = min;
        } else {
            map_transplant(tree, min, x);
            min->right = node->right;
            if (min->right) {
                min->right->parent = min;
            }
        }

        map_transplant(tree, node, min);
        min->left = node->left;
        min->left->parent = min;
        min->is_red = node->is_red;
        map_fixup(tree, x);
    }
    if (original_is_red == false) {
        map_fixup(tree, x);
    }
}

void map_removes(map_t* tree, str key)
{
    return map_remove(tree, key.data, key.len);
}

void* map_gets(map_t* tree, str key)
{
    return map_get(tree, key.data, key.len);
}

void map_sets(map_t* tree, str key, void* value)
{
    return map_set(tree, key.data, key.len, value);
}

node_t* map_next(node_t* cur)
{
    if (cur->right) {
        cur = cur->right;
        while (cur->left) {
            cur = cur->left;
        }
        return cur;
    } else { // cur right and cur left are always null at this point
        while (cur->parent && cur == cur->parent->right) {
            cur = cur->parent;
        }
        // when we're left of parent
        //  on the next iteration we should return parent->right
        return cur->parent; 
    }
}

// traverses tree from the first node to the node with the zero-based index 'index'
node_t* map_get_at(map_t* tree, u32 index)
{
    while (tree->root->parent) {
        tree->root = tree->root->parent;
    }
    // first find the first node
    if (tree->root->hash == 0) return null; // when tree->root is empty we have no entries
    node_t* cur = tree->root;
    while (cur->left) {
        cur = cur->left;
    }   
    // then advance the node until the right node is found
    for (int i = 0; i < index; i++) {
        cur = map_next(cur);
        if (cur == null) break;
    }
    return cur;
}