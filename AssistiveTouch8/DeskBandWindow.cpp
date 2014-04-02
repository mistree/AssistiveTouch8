#include "DeskBandWindow.h"

#include "DeskBandDef.h"
#include <Vsstyle.h>
#include "resource.h"

void*     pBandWin;
const wchar_t Toolbar[] = L"::TOOLBAR";

DeskBandWindow::DeskBandWindow()
   : mIconWin(g_hDllInst)
   , mTouchDetector(g_hDllInst, mInputEmulation, mIconWin)
{
	mHwnd = NULL;
	pBandWin = this;
	ZeroMemory(CurrentTheme, sizeof(CurrentTheme));
	ZeroMemory(Theme, sizeof(Theme));
	wcscat(Theme, Toolbar);
	ThemeReady = false;
	Mouse = Left;
};

DeskBandWindow::~DeskBandWindow()
{

};

BOOL DeskBandWindow::CreateDeskBand(HWND hParentWnd, HINSTANCE hInstance, LPVOID pData)
{
	    WNDCLASS wc;
		UnregisterClass(DB_CLASS_NAME,NULL);
		if (!GetClassInfo(hInstance, DB_CLASS_NAME, &wc))
		{
			wc = { 0 };
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hInstance = hInstance;
			wc.lpfnWndProc = WndProc;
			wc.lpszClassName = DB_CLASS_NAME;
			wc.hbrBackground = (HBRUSH)0;
			if (!RegisterClass(&wc))
				return FALSE;
		}
		mHwnd = CreateWindowEx(
			0,
			DB_CLASS_NAME,
			NULL,
			WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			0,//rc.left,
			0,//rc.top,
			30,//rc.right - rc.left,
			24,//rc.bottom - rc.top,
			hParentWnd,
			NULL,
			hInstance,
			pData);//(LPVOID)this);
		

		//SetMessageHook(true, 0, mHwnd, GetCurrentThreadId());

	return (NULL != mHwnd);
};

BOOL DeskBandWindow::ShowDeskBand(BOOL fShowOrHide)
{
	if (fShowOrHide)
		ShowWindow(mHwnd, SW_SHOW);
	else
		ShowWindow(mHwnd, SW_HIDE);
	return S_OK;
};

void DeskBandWindow::CloseDeskBand()
{
	ShowDeskBand(FALSE);
	if (IsWindow(mHwnd))
		DestroyWindow(mHwnd);
};

HWND DeskBandWindow::GetDeskBandHandle()
{
	return mHwnd;
};

SIZE DeskBandWindow::GetDeskBandMinSize(BOOL fNormalViewMode)
{
	SIZE BandSize;
	BandSize.cx = 30;
	BandSize.cy = 24;
	return BandSize;
};

void DeskBandWindow::RefreshDeskBand()
{
	InvalidateRect(mHwnd, NULL, TRUE);
	UpdateWindow(mHwnd);
};

LRESULT CALLBACK DeskBandWindow::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 0;
	DeskBandWindow *pDeskBand = (DeskBandWindow*)pBandWin;
	TRACKMOUSEEVENT tme;
	switch (uMessage)
	{

	case WM_MOUSEMOVE:
	    tme.cbSize = sizeof(TRACKMOUSEEVENT);
	    tme.dwFlags = TME_LEAVE;
	    tme.hwndTrack = pDeskBand->mHwnd;
	    tme.dwHoverTime = 0;
	    ::TrackMouseEvent(&tme);

		if (pDeskBand->Mouse == Left)
		{
			pDeskBand->Mouse = Entered;
			InvalidateRect(pDeskBand->mHwnd, NULL, true);
			UpdateWindow(pDeskBand->mHwnd);
		}
	    break;

	case WM_LBUTTONDOWN:
		if (pDeskBand->Mouse == Entered)
		{
			pDeskBand->Mouse = ClickDown;
			InvalidateRect(pDeskBand->mHwnd, NULL, true);
			UpdateWindow(pDeskBand->mHwnd);
		}
		break;

	case WM_LBUTTONUP:
		if (pDeskBand->Mouse == ClickDown)
		{
			pDeskBand->Mouse = Entered;
			InvalidateRect(pDeskBand->mHwnd, NULL, true);
			UpdateWindow(pDeskBand->mHwnd);
			pDeskBand->mIconWin.Show(true);
		}
		break;

	case WM_MOUSELEAVE:
		if (pDeskBand->Mouse != Left)
		{
			pDeskBand->Mouse = Left;
			InvalidateRect(pDeskBand->mHwnd, NULL, true);
			UpdateWindow(pDeskBand->mHwnd);
		}
		break;

	case WM_PAINT:
		pDeskBand->OnPaint(NULL);
		break;

	case WM_PRINTCLIENT:
		pDeskBand->OnPaint(reinterpret_cast<HDC>(wParam));
		break;

	case WM_ERASEBKGND:
		if (pDeskBand->m_fCompositionEnabled)
		{
			lResult = 1;
		}
		break;
	}

	if (uMessage != WM_ERASEBKGND)
	{
		lResult = DefWindowProc(hWnd, uMessage, wParam, lParam);
	}

	return lResult;
};

LRESULT DeskBandWindow::OnPaint(const HDC hdcIn)
{
	if (mHwnd == NULL) return 0;
	HDC hdc = hdcIn;
	PAINTSTRUCT ps;

	if (!hdc)
	{
		hdc = BeginPaint(mHwnd, &ps);
	}

	if (hdc)
	{
		RECT rc;
		GetClientRect(mHwnd, &rc);

		if (m_fCompositionEnabled)
		{		
			HTHEME hTheme;
			if (ThemeReady)
			{
				ZeroMemory(CurrentTheme, sizeof(CurrentTheme));
				wcscat(CurrentTheme, Theme);
				wcscat(CurrentTheme, Toolbar);
				hTheme = OpenThemeData(mHwnd, CurrentTheme);
			}
			else
			{
				hTheme = OpenThemeData(mHwnd, L"TOOLBAR");
			}
			
			
			if (hTheme)
			{
				HDC hdcPaint = NULL;
				HPAINTBUFFER hBufferedPaint = BeginBufferedPaint(hdc, &rc, BPBF_TOPDOWNDIB, NULL, &hdcPaint);
		
				DrawThemeParentBackground(mHwnd, hdcPaint, &rc);

				switch (Mouse)
				{
				case Entered:
					DrawThemeBackground(hTheme, hdcPaint, TP_BUTTON, TS_HOT, &rc, NULL);
					break;
				case ClickDown:
					DrawThemeBackground(hTheme, hdcPaint, TP_BUTTON, TS_PRESSED, &rc, NULL);
					break;
				case Left:
				case ClickUp:
					break;
				}

				HICON m_iconSafe = LoadIcon(g_hDllInst, MAKEINTRESOURCE(IDI_ICON1));
				DrawIconEx(hdcPaint, rc.left+(rc.right-rc.left-30)/2, rc.top+(rc.bottom-rc.top-24)/2, m_iconSafe, 30, 24, 0, NULL, DI_DEFAULTSIZE | DI_NORMAL);
		
				EndBufferedPaint(hBufferedPaint, TRUE);
		
				CloseThemeData(hTheme);
			}
		}
	}

	if (!hdcIn)
	{
		EndPaint(mHwnd, &ps);
	}

	return 0;
};