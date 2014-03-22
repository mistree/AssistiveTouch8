#include <string>
#include <windows.h>
#include <windowsx.h>

#include <objidl.h>
#include <gdiplus.h>

#include "..\CppHookDll\HookDll.h"
#include "TypeDef.h"
#include "Emulation.h"
#include "Touch.h"

#include "resource.h"  
#pragma comment(lib,"TouchHook.lib")
#pragma comment (lib,"Gdiplus.lib")
using namespace Gdiplus;

HWND                hWnd;
WNDCLASS            wndClass;
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR           gdiplusToken;
tagRECT             Pos;
HDC                 m_hdcMemory;
BLENDFUNCTION       m_Blend;
Gdiplus::Image*     image;
NOTIFYICONDATA      nid;   
HMENU               hMenu;    

AssistiveTouch::Touch mTouch;
int Alpha = 100;
int PageN = 0;
Point LastPosition;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Process(MSG msg);
void InitVar();
void InitForm(HINSTANCE hInstance, INT iCmdShow);
void InitTray(HINSTANCE hInstance, HWND hWnd);
void OnPaint(HDC m_hDC);
void Move( AssistiveTouch::Point Position );
void Page( AssistiveTouch::Gesture g);



INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
	InitVar();
	InitForm(hInstance, iCmdShow);
	SetMessageHook(true, 0, hWnd, GetCurrentThreadId());
	InitTray(hInstance, hWnd);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		switch (msg.message)
		{
		case USER_DOWN:
		case USER_UP:
		case USER_MOVE:
			Process(msg);
			//if (mTouch.InSlideOperation)
			//	continue;
		}
		DispatchMessage(&msg);
	}

	GdiplusShutdown(gdiplusToken);
	return msg.wParam;
} 

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	HDC          hdc;
	PAINTSTRUCT  ps;

	switch (message)
	{
	case WM_PAINT:
	{
	    hdc = BeginPaint(hWnd, &ps);
		OnPaint(hdc);
	    EndPaint(hWnd, &ps);
	    return 0;
	}
	case WM_TRAY:
		switch (lParam)
		{
		case WM_RBUTTONDOWN:
		{ 
		   POINT pt; GetCursorPos(&pt); 
		   SetForegroundWindow(hWnd);  
		   int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd,
			   NULL);
		   if (cmd == ID_SHOW)
			   MessageBox(hWnd, APP_TIP, APP_NAME, MB_OK);
		   if (cmd == ID_EXIT)
			   PostMessage(hWnd, WM_DESTROY, NULL, NULL);
		}
			break;
		case WM_LBUTTONDOWN:
			MessageBox(hWnd, APP_TIP, APP_NAME, MB_OK);
			break;
		case WM_LBUTTONDBLCLK:
			break;
		}
		break;
	case WM_DESTROY:
	{
	   char Path[256];
	   char FullPath[256];
	   GetCurrentDirectory(256, Path);
	   sprintf(FullPath, "%s\\Global.ini", Path);
	   std::string PosX = std::to_string(LastPosition.X);
	   std::string PosY = std::to_string(LastPosition.Y);
	   WritePrivateProfileString("Settings", "PosX", PosX.c_str(), FullPath);
	   WritePrivateProfileString("Settings", "PosY", PosY.c_str(), FullPath);
	   Shell_NotifyIcon(NIM_DELETE, &nid);
	   PostQuitMessage(0);
	   break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
} // WndProc

void InitVar()
{
	char Path[256];
	char FullPath[256];
	GetCurrentDirectory(256, Path);
	sprintf(FullPath, "%s\\Global.ini", Path);

	Alpha = GetPrivateProfileInt("Settings", "Alpha", 0, FullPath);
	int GapTime = GetPrivateProfileInt("Settings", "GapTime", 0, FullPath);
	mTouch.ClickTime = GetPrivateProfileInt("Settings", "ClickTime", 0, FullPath);
	mTouch.SlideTime = GetPrivateProfileInt("Settings", "SlideTime", 0, FullPath);
	mTouch.SlideRegion = GetPrivateProfileInt("Settings", "SlideRegion", 0, FullPath);
	mTouch.IconRegion = GetPrivateProfileInt("Settings", "IconRegion", 0, FullPath);
	mTouch.RelocateTime = GetPrivateProfileInt("Settings", "RelocateTime", 0, FullPath);
	LastPosition.X = GetPrivateProfileInt("Settings", "PosX", 500, FullPath);
	LastPosition.Y = GetPrivateProfileInt("Settings", "PosY", 500, FullPath);

	AssistiveTouch::Emulation* Emu = new AssistiveTouch::Emulation;
	Emu->LoadCommandSet(0);
	Emu->GapTime = GapTime;
	Emu->PageDown = (AssistiveTouch::Gesture)GetPrivateProfileInt("Settings", "PageDown", 0, FullPath);
	Emu->PageUp = (AssistiveTouch::Gesture)GetPrivateProfileInt("Settings", "PageUp", 0, FullPath);
	mTouch.Emu = Emu;
	mTouch.MoveIcon = &Move;
	mTouch.TurnPage = &Page;
}
void InitForm(HINSTANCE hInstance, INT iCmdShow)
{
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	m_Blend.BlendOp = 0; 
	m_Blend.BlendFlags = 0;
	m_Blend.AlphaFormat = 1; 
	m_Blend.SourceConstantAlpha = Alpha;//AC_SRC_ALPHA 
	image = Gdiplus::Image::FromFile(L"picture.png");
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)0;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("AssistiveTouch8");
	RegisterClass(&wndClass);
	hWnd = CreateWindowEx(
		WS_EX_NOACTIVATE | WS_EX_TOPMOST,
		TEXT("AssistiveTouch8"),   // window class name
		TEXT("AssistiveTouch8"),   // window caption
		WS_POPUP,                  // window style
		LastPosition.X,            // initial x position
		LastPosition.Y,            // initial y position
		image->GetWidth(),         // initial x size
		image->GetHeight(),        // initial y size
		NULL,                      // parent window handle
		NULL,                      // window menu handle
		hInstance,                 // program instance handle
		NULL);                     // creation parameters
	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);
}
void InitTray(HINSTANCE hInstance, HWND hWnd)
{
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = IDI_TRAY;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	nid.uCallbackMessage = WM_TRAY;
	nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAY));
	lstrcpy(nid.szTip, APP_NAME);

	hMenu = CreatePopupMenu();
	 
	AppendMenu(hMenu, MF_STRING, ID_EXIT, TEXT("exit"));

	Shell_NotifyIcon(NIM_ADD, &nid);
}

void OnPaint(HDC m_hDC)
{
	HDC hdcTemp = m_hDC;
	m_hdcMemory = CreateCompatibleDC(hdcTemp);
	HBITMAP hBitMap = CreateCompatibleBitmap(hdcTemp, image->GetWidth(), image->GetHeight());
	SelectObject(m_hdcMemory, hBitMap);
	HDC hdcScreen = ::GetDC(hWnd);
	RECT rct;
	GetWindowRect(hWnd, &rct);
	POINT ptWinPos = { rct.left, rct.top };
	Graphics graph(m_hdcMemory);
	Point points[] = { Point(0, 0),
		Point(image->GetWidth(), 0),
		Point(0, image->GetHeight()) };
	static bool bFly = false;
	graph.DrawImage(image, points, 3);
	SIZE sizeWindow = { image->GetWidth(), image->GetHeight() };
	POINT ptSrc = { 0, 0 };
	DWORD dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
	if ((dwExStyle & 0x80000) != 0x80000)
		SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle ^ 0x80000);
	UpdateLayeredWindow(hWnd, hdcScreen, &ptWinPos, &sizeWindow, m_hdcMemory, &ptSrc, 0, &m_Blend, 2);
	graph.ReleaseHDC(m_hdcMemory);
	::ReleaseDC(hWnd, hdcScreen);
	hdcScreen = NULL;
	::ReleaseDC(hWnd, hdcTemp);
	hdcTemp = NULL;
	DeleteObject(hBitMap);
	DeleteDC(m_hdcMemory);
	m_hdcMemory = NULL;
}
void Process(MSG msg)
{
	GetWindowRect(hWnd, &Pos);
	mTouch.IconP = { (Pos.left + Pos.right) / 2, (Pos.top + Pos.bottom) / 2 };
	AssistiveTouch::Point touchP = { GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam) };

	switch (msg.message)
	{
	case USER_MOVE:
		mTouch.Update(AssistiveTouch::TouchMove, touchP);
		break;
	case USER_DOWN:
		mTouch.Update(AssistiveTouch::TouchDown, touchP);
		break;
	case USER_UP:
		mTouch.Update(AssistiveTouch::TouchUp, touchP);
		break;
	case WM_WINDOWPOSCHANGED:
		BringWindowToTop(hWnd);
	}
}

void Move(AssistiveTouch::Point Position)
{
	LastPosition.X = Position.PosX - (Pos.right - Pos.left) / 2;
	LastPosition.Y = Position.PosY - (Pos.bottom - Pos.top) / 2;
	MoveWindow(
		hWnd,
		LastPosition.X,
		LastPosition.Y,
		image->GetWidth(),
		image->GetHeight(),
		true
		);
}

void Page(AssistiveTouch::Gesture g)
{
	if (g == mTouch.Emu->PageDown)
	{
		PageN = ((PageN + 1) < 10) ? PageN + 1 : 0;
	}
	else if (g == mTouch.Emu->PageUp)
	{
		PageN = ((PageN - 1) < 0) ? 9 : PageN - 1;
	}
	mTouch.Emu->LoadCommandSet(PageN);
}