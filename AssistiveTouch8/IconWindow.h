#ifndef ICONWINDOW_H
#define ICONWINDOW_H

#include "Libs.h"
#include <gdiplus.h>
using namespace Gdiplus;

class IconWindow
{
private:
	GdiplusStartupInput mGdiStartup;
	ULONG_PTR           mGdiToken;
	BLENDFUNCTION       mGdiBlend;
	Image*              mImage;
	HWND                mIconHwnd;
	wchar_t             mFullPath[256];
	int                 mAlpha;

public:
	Point               mPosition;

public:
	IconWindow(HINSTANCE Dll);
	~IconWindow();
	void Show(bool Visible);
	void Move(int PosX, int PosY);

private:
	void OnPaint();

// static callback function
private:
	static LRESULT CALLBACK IconProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

#endif //ICONWINDOW_H