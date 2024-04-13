#include <stdio.h>

#define UI_BACKEND_WIN32_SOFTWARE
#include "lib/orui.h"
#include "lib/console.h"

int main(int argc, char** argv) 
{
	ui_window* w = orui_init(800, 600, "ORUI Test Application");
	ui_panel* root = orui_get_root(w);
	root->w.background = 0xFFFFFFFF;

	ui_panel* a = null;
	a = panel(&root->w);
	a->w.background = 0xFFFF00FF;
	a->w.fixed = true; a->w.pref_w = 100; a->w.pref_h = 200;

	a = panel(&root->w);
	a->w.background = 0xFF00FFFF;
	a->w.fixed = true; a->w.pref_w = 100; a->w.pref_h = 200;

	a = panel(&root->w);
	a->w.background = 0xFF00ff00;
	a->w.fixed = true; a->w.pref_w = 100; a->w.pref_h = 200;

	a = panel(&root->w);
	a->w.background = 0xFFFF00FF;
	a->w.fixed = true; a->w.pref_w = 100; a->w.pref_h = 200;

	a = panel(&root->w);
	a->w.background = 0xFFFF00FF;
	a->w.fixed = true; a->w.pref_w = 100; a->w.pref_h = 200;
	return orui_message_loop();
}