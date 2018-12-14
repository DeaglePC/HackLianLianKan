#include "stdafx.h"
#include "CProcessBitmap.h"


CProcessBitmap::CProcessBitmap()
{
}


CProcessBitmap::~CProcessBitmap()
{
}

HBITMAP CProcessBitmap::GetWindowBitmap(HWND hWnd)
{
	HDC hDcGame = ::GetDC(hWnd);
	RECT rect;
	::GetWindowRect(hWnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	// 得到整个窗口截图
	HBITMAP hBitmap = ::CreateCompatibleBitmap(hDcGame, width, height);
	HDC hDcMem = ::CreateCompatibleDC(hDcGame);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hDcMem, hBitmap);

	::BitBlt(hDcMem, 0, 0, width, height, hDcGame, 0, 0, SRCCOPY);
	hBitmap = (HBITMAP)::SelectObject(hDcMem, hOldBmp);

	::DeleteObject(hOldBmp);
	::DeleteDC(hDcMem);
	::DeleteDC(hDcGame);
	return hBitmap;
}

HBITMAP CProcessBitmap::GetCutBitmap(HWND hWnd, HBITMAP hSourceBmp, int x, int y, int width, int height)
{
	HDC hDcGame = ::GetDC(hWnd);
	HDC hDcSource = CreateCompatibleDC(hDcGame);
	HDC hDcDest = CreateCompatibleDC(hDcGame);

	HBITMAP hResBmp = CreateCompatibleBitmap(hDcGame, width, height);
	HBITMAP hOldBmpSource = (HBITMAP)SelectObject(hDcSource, hSourceBmp);
	HBITMAP hOldBmpDest = (HBITMAP)SelectObject(hDcDest, hResBmp);
	BitBlt(hDcDest, 0, 0, width, height, hDcSource, x, y, SRCCOPY);
	SelectObject(hDcSource, hOldBmpSource);

	DeleteDC(hDcGame);
	DeleteDC(hDcSource);
	DeleteDC(hDcDest);
	DeleteObject(hOldBmpSource);
	DeleteObject(hOldBmpDest);
	return hResBmp;
}

HBITMAP CProcessBitmap::GetCopyBitmap(HWND hWnd, HBITMAP hSourceBmp)
{
	if (hSourceBmp == NULL)
	{
		return NULL;
	}
	HDC hDc = ::GetDC(hWnd);
	HDC hDcSource = CreateCompatibleDC(hDc);
	HDC hDcDest = CreateCompatibleDC(hDc);
	BITMAP bmp;
	GetObject(hSourceBmp, sizeof(bmp), &bmp);

	HBITMAP hResBmp = CreateCompatibleBitmap(hDc, bmp.bmWidth, bmp.bmHeight);
	HBITMAP hOldBmpSource = (HBITMAP)SelectObject(hDcSource, hSourceBmp);
	HBITMAP hOldBmpDest = (HBITMAP)SelectObject(hDcDest, hResBmp);
	BitBlt(hDcDest, 0, 0, bmp.bmWidth, bmp.bmHeight, hDcSource, 0, 0, SRCCOPY);

	hResBmp = (HBITMAP)SelectObject(hDcDest, hOldBmpDest);
	SelectObject(hDcSource, hOldBmpSource);

	DeleteDC(hDc);
	DeleteDC(hDcSource);
	DeleteDC(hDcDest);
	DeleteObject(hOldBmpSource);
	DeleteObject(hOldBmpDest);
	return hResBmp;
}

// 比较两个长宽相等的图片是否一样
bool CProcessBitmap::IsBitmapEqual(HWND hWnd, HBITMAP hBitmapSource, HBITMAP hBitmapDestination)
{
	if (hBitmapSource == NULL  || hBitmapDestination == NULL)
	{
		printf("IsBitmapEqual null\n");
		return false;
	}
	BITMAP bmp;
	GetObject(hBitmapSource, sizeof(bmp), &bmp);

	HDC hDc = ::GetDC(hWnd);
	//HDC hDcSource = CreateCompatibleDC(hDc);
	//HDC hDcDest = CreateCompatibleDC(hDc);
	//HBITMAP hOldSourceBmp = (HBITMAP)SelectObject(hDcSource, hBitmapSource);
	//HBITMAP hOldDestBmp = (HBITMAP)SelectObject(hDcDest, hBitmapDestination);
	//BitBlt(hDcDest, 0, 0, bmp.bmWidth, bmp.bmHeight, hDcSource, 0, 0, SRCINVERT);
	//HBITMAP hResBitmap = CreateCompatibleBitmap(hDc, bmp.bmWidth, bmp.bmHeight);
	//hResBitmap = (HBITMAP)SelectObject(hDcDest, hOldDestBmp);
	//DeleteDC(hDcDest);
	//DeleteDC(hDcSource);
	//DeleteObject(hOldSourceBmp);
	//DeleteObject(hOldDestBmp);
	//return hResBitmap;

	BITMAPINFO bmpInfo;
	UINT* pDataSource;
	UINT* pDataDest;

	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = bmp.bmWidth;
	bmpInfo.bmiHeader.biHeight = -bmp.bmHeight;	// 倒过来
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biBitCount = 32;

	pDataSource = new UINT[bmp.bmWidth * bmp.bmHeight];
	pDataDest = new UINT[bmp.bmWidth * bmp.bmHeight];
	GetDIBits(hDc, hBitmapSource, 0, bmp.bmHeight, pDataSource, &bmpInfo, DIB_RGB_COLORS);
	GetDIBits(hDc, hBitmapDestination, 0, bmp.bmHeight, pDataDest, &bmpInfo, DIB_RGB_COLORS);

	bool res = memcmp(pDataDest, pDataSource, bmp.bmHeight*bmp.bmWidth) == 0;
	DeleteDC(hDc);
	delete[] pDataSource;
	delete[] pDataDest;
	return res;
}

bool CProcessBitmap::IsAllSameRGB(HWND hWnd, HBITMAP hBitmapSource)
{
	if (hWnd == NULL || hBitmapSource == NULL)
	{
		return false;
	}
	BITMAP bmp;
	GetObject(hBitmapSource, sizeof(bmp), &bmp);

	HDC hDc = ::GetDC(hWnd);
	BITMAPINFO bmpInfo;
	UINT* pDataSource;

	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = bmp.bmWidth;
	bmpInfo.bmiHeader.biHeight = -bmp.bmHeight;	// 倒过来
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biBitCount = 32;

	pDataSource = new UINT[bmp.bmWidth * bmp.bmHeight];
	GetDIBits(hDc, hBitmapSource, 0, bmp.bmHeight, pDataSource, &bmpInfo, DIB_RGB_COLORS);

	// 判断是不是所有的颜色都一样
	LONG len = bmp.bmWidth * bmp.bmHeight;
	if (len	== 0)
	{
		return false;
	}
	UINT uFirst = pDataSource[0];
	bool res = true;
	for (int i = 1; i < len; ++i)
	{
		if (pDataSource[i] != uFirst)
		{
			res = false;
			break;
		}
	}

	delete[] pDataSource;
	DeleteDC(hDc);
	return res;
}
