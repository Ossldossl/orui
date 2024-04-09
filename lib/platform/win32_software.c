#include <windows.h>

#include "win32_software.h"
#include "../orui.h"
#include "../platform.h"
#include "../misc.h"
#include "../console.h"

typedef struct {
	u8 wnd_count;	
	platform_window* main_window;
} platform_state;

platform_state state;

char const * const normal_class = "normal";

static void recreate_window_bitmap(platform_window* window)
{
	window->bits = realloc(window->bits, window->width * window->height * sizeof(px));
	window->bmi.bmiHeader.biSize = sizeof(window->bmi);
	window->bmi.bmiHeader.biWidth = window->width;
	window->bmi.bmiHeader.biHeight = window->height;
	window->bmi.bmiHeader.biPlanes = 1;
	window->bmi.bmiHeader.biBitCount = 32;
	window->bmi.bmiHeader.biCompression = BI_RGB;
	window->bmi.bmiHeader.biSizeImage = 0;
	window->bmi.bmiHeader.biClrUsed = 0;
	window->bmi.bmiHeader.biSizeImage = 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	platform_window* wnd = (platform_window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	switch (msg) {
		case WM_CLOSE: {
			if (wnd == state.main_window) { PostQuitMessage(0); }
		} break;
		case WM_SIZE: {
			wnd->width = LOWORD(lParam); wnd->height = HIWORD(lParam);
			recreate_window_bitmap(wnd);
// TODO: update layout
//			ui_widget_move() 
		} break;
		case WM_PAINT: {
			log_debug("paint (%s)", wnd == state.main_window ? "true" : "false");
			PAINTSTRUCT ps;
			BeginPaint(wnd->hWnd, &ps);
			orui_paint(wnd);
			EndPaint(wnd->hWnd, &ps);
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void platform_init(void) 
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = normal_class;
	RegisterClass(&wc);

	state.wnd_count = 0;
	state.main_window = null;
}

void platform_destroy(void)
{
	return;
}

i32 platform_message_loop(void)
{
	i32 result = 0;
	while (true) {
		MSG msg;
		if (orui_is_animating()) {
			while (PeekMessage(&msg, null, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT) {
					return msg.wParam;
				}
				// TODO: send delta time
				orui_animate();
			}
		} else {
			bool exit = GetMessage(&msg, null, 0, 0);
			if (exit == 0) {
				return msg.wParam;	
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

bool platform_create_window(platform_window* wnd, u16 width, u16 height, char* title)
{
	state.wnd_count++;

	wnd->bits = null;
	wnd->hWnd = CreateWindowExA(
		WS_EX_ACCEPTFILES,
		normal_class,
		title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width,
		height,
		null, // owner
		null,
		null,
		null
	);
	if (wnd->hWnd == null) {
		log_error("Failed to create window: %lu!", GetLastError());
		return false;
	}
 
	wnd->dc = GetDC(wnd->hWnd);

	if (state.main_window == null) state.main_window = wnd;

	recreate_window_bitmap(wnd);

	SetWindowLongPtr(wnd->hWnd, GWLP_USERDATA, (long long) wnd);

	ShowWindow(wnd->hWnd, SW_SHOW);
	return true;
}

void platform_window_set_title(platform_window* wnd, char* new_title)
{
	platform_window* w = wnd;
	bool ok = SetWindowTextA(w->hWnd, new_title);
	if (!ok) {
		log_error("Failed to set window title: %lu!", GetLastError());
	}
}

void platform_window_set_dims(platform_window* wnd, u16 x, u16 y, u16 w, u16 h)
{
	platform_window* win = wnd;
	bool ok = SetWindowPos(
		win->hWnd,
		null,
		x, y,
		w, h,
		SWP_ASYNCWINDOWPOS | SWP_NOZORDER | SWP_NOSENDCHANGING | SWP_SHOWWINDOW
	);
	if (!ok) {
		log_error("Failed to set window dims: %lu", GetLastError());
	}
}

void platform_destroy_window(platform_window* wnd)
{
	platform_window* w = (platform_window*)wnd;
	ReleaseDC(w->hWnd, w->dc);
	bool ok = DestroyWindow(w->hWnd);
	if (!ok) {
		log_error("Failed to destroy window: %lu!", GetLastError());
		return;
	}
}

//==== PAINTING ====
void platform_begin_paint(platform_window* window)
{
	for (int y = 0; y < window->height; y++) {
		u32 yoff = y * window->width;
		for (int x = 0; x < window->width; x++) {
			px* p = &window->bits[yoff + x];	
			p->rgba = 0xFF00FF;
		}
	}	
}

void platform_end_paint(platform_window* window)
{
	StretchDIBits(window->dc,
		0, 0, // dest pos
		window->width, window->height,
		0, 0, // src pos
		window->width, window->height,
		window->bits, 
		&window->bmi,
		DIB_RGB_COLORS,
		SRCCOPY);
}