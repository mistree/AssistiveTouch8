
#include "IconWindow.h"
#include "TouchDetector.h"
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

IconWindow* pIcon = nullptr;

extern TouchDetector* pTouch;

IconWindow::IconWindow(HINSTANCE Dll)
{
	pIcon = this;
	mShow = false;
	GdiplusStartup(&mGdiToken, &mGdiStartup, NULL);

	GetModuleFileName(Dll, mFullPath, 256);
	wcscpy(wcsstr(mFullPath, L"AssistiveTouch8.dll"), L"Global.ini");

	mAlpha = GetPrivateProfileInt(L"Settings", L"Alpha", 100, mFullPath);
	mGdiBlend.BlendOp = 0;
	mGdiBlend.BlendFlags = 0;
	mGdiBlend.AlphaFormat = 1;
	mGdiBlend.SourceConstantAlpha = mAlpha;

	wchar_t IconName[256];
	GetModuleFileName(Dll, IconName, 256);
	wcscpy(wcsstr(IconName, L"AssistiveTouch8.dll"), L"Icon.png");
	Image* IconImage = Gdiplus::Image::FromFile(IconName);
	if (IconImage->GetLastStatus() == Gdiplus::Ok)
		mImage = IconImage;
	else
	{
		HRSRC hResource = ::FindResource(Dll, MAKEINTRESOURCE(IDB_PNG1), L"png");
		DWORD Length = SizeofResource(Dll, hResource);
		BYTE* lpResource = (BYTE*)LoadResource(Dll, hResource);
		HGLOBAL mMemory = GlobalAlloc(GMEM_FIXED, Length);
		BYTE* pmem = (BYTE*)GlobalLock(mMemory);
		memcpy(pmem, lpResource, Length);
		IStream* pStream;
		CreateStreamOnHGlobal(mMemory, FALSE, &pStream);
		mImage = Image::FromStream(pStream);
		GlobalUnlock(mMemory);
		pStream->Release();
		FreeResource(lpResource);
	}

	WNDCLASS IconClass;
	IconClass.style = CS_HREDRAW | CS_VREDRAW;
	IconClass.lpfnWndProc = IconProc;
	IconClass.cbClsExtra = 0;
	IconClass.cbWndExtra = 0;
	IconClass.hInstance = Dll;
	IconClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	IconClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	IconClass.hbrBackground = (HBRUSH)0;
	IconClass.lpszMenuName = NULL;
	IconClass.lpszClassName = TEXT("AssistiveTouch8");
	RegisterClass(&IconClass);

	mPosition.X = GetPrivateProfileInt(L"Settings", L"PosX", 500, mFullPath);
	mPosition.Y = GetPrivateProfileInt(L"Settings", L"PosY", 500, mFullPath);

	mIconHwnd = CreateWindowEx(
		                        WS_EX_NOACTIVATE | WS_EX_TOPMOST,
		                        L"AssistiveTouch8",
		                        L"AssistiveTouch8", 
		                        WS_POPUP,                
								mPosition.X,
								mPosition.Y,
								mImage->GetWidth(),
								mImage->GetHeight(),
		                        NULL,                  
		                        NULL,                     
								Dll,            
		                        NULL                
							  );
};

IconWindow::~IconWindow()
{
	DestroyWindow(mIconHwnd);
	GdiplusShutdown(mGdiToken);
};

void IconWindow::Show(bool Visible)
{
	mShow = Visible;
	ShowWindow(mIconHwnd, Visible);
	UpdateWindow(mIconHwnd);
};

void IconWindow::Move(Point Pos)
{
	MoveWindow(
		mIconHwnd,
		Pos.X - 0.5*mImage->GetWidth(),
		Pos.Y - 0.5*mImage->GetHeight(),
		mImage->GetWidth(),
		mImage->GetHeight(),
		true
		);
	mPosition = Pos;
};

void IconWindow::OnPaint()
{
	PAINTSTRUCT  Paint;
	HDC mBuffer = BeginPaint(mIconHwnd, &Paint);
	mGdiBlend.SourceConstantAlpha = mAlpha;	
	HDC mMemory = CreateCompatibleDC(mBuffer);
	HBITMAP hBitmap = CreateCompatibleBitmap(mBuffer, mImage->GetWidth(), mImage->GetHeight());
	SelectObject(mMemory, hBitmap);
	HDC Screen = ::GetDC(mIconHwnd);
	RECT rc;
	GetWindowRect(mIconHwnd, &rc);
	POINT WinPos = { rc.left, rc.top };
	Graphics Graph(mMemory);
	Point Points[] = { 
		               Point(0, 0),
		               Point(mImage->GetWidth(), 0),
		               Point(0, mImage->GetHeight()) 
	                 };
	Graph.DrawImage(mImage, Points, 3);
	SIZE WinSize = { mImage->GetWidth(), mImage->GetHeight() };
	POINT Src = { 0, 0 };
	DWORD dwExStyle = GetWindowLong(mIconHwnd, GWL_EXSTYLE);
	if ((dwExStyle & 0x80000) != 0x80000)
		SetWindowLong(mIconHwnd, GWL_EXSTYLE, dwExStyle ^ 0x80000);
	UpdateLayeredWindow(mIconHwnd, Screen, &WinPos, &WinSize, mMemory, &Src, 0, &mGdiBlend, 2);
	Graph.ReleaseHDC(mMemory);
	::ReleaseDC(mIconHwnd, Screen);
	::ReleaseDC(mIconHwnd, mBuffer);
	DeleteObject(hBitmap);
	DeleteDC(mMemory);
	EndPaint(mIconHwnd, &Paint);
};

LRESULT CALLBACK IconWindow::IconProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{	
	case WM_PAINT:
	{
		pIcon->OnPaint();
		break;
	}
	case WM_MOUSEACTIVATE:
	{
		return MA_NOACTIVATEANDEAT;
	}
	case WM_DESTROY:
	{
		wchar_t Temp[32];
		swprintf(Temp, L"%d", pIcon->mPosition.X);
		WritePrivateProfileString(L"Settings", L"PosX", Temp, pIcon->mFullPath);
		swprintf(Temp, L"%d", pIcon->mPosition.Y);
		WritePrivateProfileString(L"Settings", L"PosY", Temp, pIcon->mFullPath);		
        break;
	}
	case WM_POINTERDOWN:
	case WM_NCPOINTERDOWN:
	case WM_POINTERUP:
	case WM_NCPOINTERUP:
	case WM_POINTERUPDATE:
	case WM_NCPOINTERUPDATE:
		return MA_NOACTIVATEANDEAT;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	};
	return 0;
};

