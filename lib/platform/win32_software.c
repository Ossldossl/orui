#include <windows.h>

#include "win32_software.h"
#include "../orui.h"
#include "../platform.h"
#include "../misc.h"
#include "../console.h"
#include <stdio.h>

typedef struct {
	u8 wnd_count;	
	platform_window* main_window;
} platform_state;

platform_state state;

char const * const normal_class = "normal";

static void recreate_window_bitmap(platform_window* window)
{
	window->bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	window->bmi.bmiHeader.biWidth = window->width;
	window->bmi.bmiHeader.biHeight = -window->height;
	window->bmi.bmiHeader.biPlanes = 1;
	window->bmi.bmiHeader.biBitCount = 32;
	window->bmi.bmiHeader.biCompression = BI_RGB;
	window->bits = realloc(window->bits, window->width * window->height * sizeof(u32));
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
			log_debug("SIZE");
			orui_update((ui_window*)wnd, UPDATE_RESIZE, 0, new_uvec2(wnd->width, wnd->height));
		} break;
		case WM_PAINT: {
			log_debug("paint (%s)", wnd == state.main_window ? "true" : "false");
			PAINTSTRUCT ps;
			BeginPaint(wnd->hWnd, &ps);
			platform_end_paint(wnd);
			EndPaint(wnd->hWnd, &ps);
		} break;
		case WM_MOUSEMOVE: {
			i16 xpos = LOWORD(lParam); i16 ypos = HIWORD(lParam);
			xpos = max(0, xpos);
			ypos = max(0, ypos);
			uvec2 pos;
			pos.x = *(i16*)&xpos; pos.y = *(i16*)&ypos;
			if (!wnd->tracking_mouse) {
				wnd->me.cbSize = sizeof(TRACKMOUSEEVENT);
				wnd->me.dwFlags = TME_HOVER | TME_LEAVE;
				wnd->me.dwHoverTime = HOVER_DEFAULT;
				wnd->me.hwndTrack = wnd->hWnd;
				TrackMouseEvent(&wnd->me);
				wnd->tracking_mouse = true;
			}
			log_debug("MOUSE MOVE (%d:%d)", xpos, ypos);
			orui_update((ui_window*)wnd, UPDATE_MOUSE_MOVE, 0, pos);	
		} break;
		case WM_LBUTTONDOWN: {
			SetCapture(wnd->hWnd);
			i16 xpos = LOWORD(lParam); i16 ypos = HIWORD(lParam);
			uvec2 pos;
			pos.x = *(i16*)&xpos; pos.y = *(i16*)&ypos;
			orui_update((ui_window*)wnd, UPDATE_LDOWN, 0, pos);
		} break;
		case WM_LBUTTONUP: {
			ReleaseCapture();
		} break;
		case WM_MOUSEHOVER: {
			log_debug("HOVER!");
			wnd->tracking_mouse = false;
			orui_update((ui_window*)wnd, UPDATE_HOVER, 0, new_uvec2(0,0));
		} break;
		case WM_MOUSELEAVE: {
			log_debug("LEAVE!");
			wnd->tracking_mouse = false;
			orui_update((ui_window*)wnd, UPDATE_MOUSE_LEAVE, 0, new_uvec2(0,0));
		} break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void platform_init(void) 
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = normal_class;
	wc.style = CS_DBLCLKS;
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
		WS_OVERLAPPEDWINDOW | WS_THICKFRAME,
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
	// clear screen
	u16 w = window->width; u16 h = window->height;

	for (u64 i = 0; i < w*h; i++) {
		u32* p = &window->bits[i];	
		*p = 0xFF00FF;
	}
}

void platform_end_paint(platform_window* window)
{
	// TODO: dont paint the entire window on small changes
	StretchDIBits(window->dc,
		0, 0, // dest pos
		window->width, window->height,
		0, 0, // src pos
		window->width, window->height,
		window->bits, 
		&window->bmi,
		DIB_RGB_COLORS,
		SRCCOPY
	);
}

void debug_as_ppm(u32* bits, u16 width, u16 height)
{
	printf("\n\n");
	printf("P3\n%d %d\n255\n", width, height);
	for (u64 i = 0; i < width*height; i++) {
		u32 p = bits[i];
		printf("%3d %3d %3d ", r(p), g(p), b(p));
		
		if (i % 32 == 0) printf("\n");
	}
}

void platform_draw_rect(painter* painter, urect16 r, u32 c)
{
	r.t += painter->cur_offset.y; r.l += painter->cur_offset.x;
	r.b += painter->cur_offset.y; r.r += painter->cur_offset.x;

	r = rect_clip(r, painter->clip_rect);
	u32* bits = painter->bits;
	u16 width = painter->window->width; u16 height = painter->window->height;
	for (u16 y = r.t; y < r.b; y++) {
		u64 yoff = y*width;
		for (u16 x = r.l; x < r.r; x++) {
			bits[yoff+(u64)x] = c;
		}
	}
}