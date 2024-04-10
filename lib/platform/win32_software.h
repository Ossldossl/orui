#pragma once
#include <windows.h>
#include "../misc.h"

struct platform_window {
	HWND hWnd;
	HDC dc;
	u32* bits;
	u16 width, height;
	BITMAPINFO bmi;
};