#pragma once

#include "misc.h"

void platform_init();
// returns the custom window type
void* create_window(u16 width, u16 height, char* title);
void window_set_title(void* window, char* new_title);
void window_set_dims(void* window, u16 x, u16 y, u16 w, u16 h);
void destroy_window(void* window);
