#include <stdio.h>

#define UI_BACKEND_WIN32_SOFTWARE
#include "lib/orui.h"
#include "lib/console.h"

int main(int argc, char** argv) 
{
	ui_window* w = orui_init(800, 600, "ORUI Test Application");
	ui_panel* root = orui_get_root(w);
	style_add_changes(root->w.style_state, STATE_ACTIVE, snew_str("background"), PROP_VALUE(PROPERTY_COLOR, 0xFFFFFFFF));
	widget_finalize(&root->w);

	ui_panel* a = null;
	a = panel(&root->w, snew_str("first-panel"));
	a->w.fixed = true; 
	style_add_changes(a->w.style_state, STATE_ACTIVE, snew_str("background"), PROP_VALUE(PROPERTY_COLOR, 0xFFFF00FF));
	style_add_changes(a->w.style_state, STATE_ACTIVE, snew_str("width"), PROP_VALUE(PROPERTY_U16, 100));
	style_add_changes(a->w.style_state, STATE_ACTIVE, snew_str("height"), PROP_VALUE(PROPERTY_U16, 200));
	widget_finalize(&a->w);

	a = panel(&root->w, snew_str("second-panel"));
	a->w.fixed = true; 
	style_add_changes(a->w.style_state, STATE_ACTIVE, snew_str("background"), PROP_VALUE(PROPERTY_COLOR, 0xFF00FFFF));
	style_add_changes(a->w.style_state, STATE_ACTIVE, snew_str("width"), PROP_VALUE(PROPERTY_U16, 100));
	style_add_changes(a->w.style_state, STATE_ACTIVE, snew_str("height"), PROP_VALUE(PROPERTY_U16, 200));
	widget_finalize(&a->w);

	a = panel(&root->w, snew_str("third-panel"));
	a->w.fixed = true; 
	style_add_changes(a->w.style_state, STATE_ACTIVE, snew_str("background"), PROP_VALUE(PROPERTY_COLOR, 0xFF00ff00));
	style_add_changes(a->w.style_state, STATE_ACTIVE, snew_str("width"), PROP_VALUE(PROPERTY_U16, 100));
	style_add_changes(a->w.style_state, STATE_ACTIVE, snew_str("height"), PROP_VALUE(PROPERTY_U16, 200));
	style_add_changes(a->w.style_state, STATE_ACTIVE, snew_str("margin"), PROP_VALUE(PROPERTY_URECT16, make_urect16(0, 15, 0, 0)));

	style_add_changes(a->w.style_state, STATE_HOVERED, snew_str("width"), PROP_VALUE(PROPERTY_U16, 350));
	style_add_changes(a->w.style_state, STATE_HOVERED, snew_str("height"), PROP_VALUE(PROPERTY_U16, 350));
	widget_finalize(&a->w);

	a = panel(&root->w, snew_str("fourth-panel"));
	a->w.fixed = true; 
	style_add_changes(a->w.style_state, STATE_ACTIVE, snew_str("background"), PROP_VALUE(PROPERTY_COLOR, 0xFFFF00FF));
	style_add_changes(a->w.style_state, STATE_ACTIVE, snew_str("width"), PROP_VALUE(PROPERTY_U16, 100));
	style_add_changes(a->w.style_state, STATE_ACTIVE, snew_str("height"), PROP_VALUE(PROPERTY_U16, 200));
	widget_finalize(&a->w);

	a = panel(&root->w, snew_str("fifth-panel"));
	a->w.fixed = true; 
	style_add_changes(a->w.style_state, STATE_ACTIVE, snew_str("background"), PROP_VALUE(PROPERTY_COLOR, 0xFFFF00FF));
	style_add_changes(a->w.style_state, STATE_ACTIVE, snew_str("width"), PROP_VALUE(PROPERTY_U16, 100));
	style_add_changes(a->w.style_state, STATE_ACTIVE, snew_str("height"), PROP_VALUE(PROPERTY_U16, 200));
	widget_finalize(&a->w);
	return orui_message_loop();
}