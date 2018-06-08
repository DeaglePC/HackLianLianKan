#pragma once

#include <windows.h>

class CProcessBitmap
{
public:
	CProcessBitmap();
	~CProcessBitmap();

	// get 之后的都需要delete
	static HBITMAP GetWindowBitmap(HWND hWnd);
	static HBITMAP GetCutBitmap(HWND hWnd,HBITMAP hSourceBmp, int x, int y, int width, int height);
	static HBITMAP GetCopyBitmap(HWND hWnd, HBITMAP hSourceBmp);
	static bool IsBitmapEqual(HWND hWnd, HBITMAP hBitmapSource, HBITMAP hBitmapDestination);
};
