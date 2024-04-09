#pragma once
#include <windows.h>
#include "../misc.h"

struct platform_window {
	HWND hWnd;
	HDC dc;
	px* bits;
	u16 width, height;
	BITMAPINFO bmi;
};