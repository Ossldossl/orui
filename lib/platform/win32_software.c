#include <windows.h>

#include "../platform.h"
#include "../misc.h"

typedef struct window {
	HWND hWnd;
	DC dc;
	px* bits;
}

char const * const normal_class = "normal";

void platform_init() 
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = wnd_proc;
    wc.lpszClassName = normal_class;
	RegisterClass(&wc);
}

void* create_window(u16 width, u16 height, char* title)
{
	CreateWindowEx(
			WS_EX_ACCEPTFILES,
			normal_class,
			title,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			width ? width : CW_USEDEFAULT,
			height ? height : CW_USEDEFAULT,
			null, // owner
			null,
			null,
			null);

	
}

void window_set_title(void* window, char* new_title);
void window_set_dims(void* window, u16 x, u16 y, u16 w, u16 h);
