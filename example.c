#include <stdio.h>

#define UI_BACKEND_WIN32_SOFTWARE
#include "lib/orui.h"
#include "lib/console.h"

int main(int argc, char** argv) 
{
	ui_window* w = orui_init(800, 600, "ORUI Test Application");
	orui_create_window(800,600,"second window!");
	orui_create_window(800,800,"third window");	
	return orui_message_loop();
}