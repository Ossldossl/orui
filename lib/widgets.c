#include <stdlib.h>
#include <string.h>
#include "misc.h"
#include "widgets.h"

static ui_widget* make_widget(ui_widget* parent, u64 size, message_handler widget_message)
{
    ui_widget* result = malloc(size);
    memset(result, 0, size);
    result->wmsg = widget_message;
    result->parent = parent;
    result->root = parent->root;
    return result; 
}




//==== PANELS ====
static i32 panel_message(ui_widget* widget, ui_msg msg, i32 di, void* dp)
{

    return 0;
}

ui_panel* panel(ui_widget* parent)
{
    ui_panel* result = (ui_panel*)make_widget(parent, sizeof(ui_panel), panel_message);
    return result;
}

