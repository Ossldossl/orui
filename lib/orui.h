#pragma once
#include "common.h"

typedef struct {
    u16 t, l, b, r;
} urect;

typedef struct {
    u8 t, l, b, r;
} urect8;

typedef struct {
    u16 x, y;
} uvec2;

typedef struct {
    bool min_width : 1;
    bool max_width : 1;
    bool min_height : 1;
    bool max_height : 1;
    bool width : 1;
    bool height : 1;

    bool margin_t : 1;
    bool margin_l : 1;
    bool margin_r : 1;
    bool margin_b : 1;

    bool border_t : 1;
    bool border_l : 1;
    bool border_r : 1;
    bool border_b : 1;

    bool padding_t : 1;
    bool padding_l : 1;
    bool padding_r : 1;
    bool padding_b : 1;

    bool position_absolute : 1;
    bool hidden : 1;
} element_flags;

typedef struct {
    u8 r, g, b, a;
} color;

typedef struct {
    color background_color;
    // background-image
    color border_color;
    urect border_radius;
    float opacity;

    color text_color;
    char* text_content;
    float font_size;
} ou_visual;

typedef struct ou_element ou_element;
struct ou_element {
    element_flags flags;
    u16 min_width, min_height;
    u16 max_width, max_height;
    u16 width, height;

    urect position;
    urect8 margin, border, padding;

    ou_element* children; u16 child_count;
}; 


