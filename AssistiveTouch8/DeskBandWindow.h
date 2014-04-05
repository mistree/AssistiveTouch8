#ifndef _IDESKBANDWINDOW_H_
#define _IDESKBANDWINDOW_H_

#include "Libs.h"
#include "TouchDetector.h"
#include "InputEmulation.h"
#include "IconWindow.h"
#include "ConfigurationWindow.h"
#include "Configuration.h"

class DeskBandWindow
{
public:
	HWND                mHwnd;
	BOOL                m_fCompositionEnabled;
	wchar_t             Theme[50];
	wchar_t             CurrentTheme[50];
	bool                ThemeReady;

private:
	TouchDetector       mTouchDetector;
	InputEmulation      mInputEmulation;
	Configuration       mConfig;
	IconWindow          mIconWin;
	ConfigurationWindow mConfigurationWin;
	bool                mShow;

public:
	DeskBandWindow();
	~DeskBandWindow();
	BOOL CreateDeskBand(HWND hParentWnd, HINSTANCE hInstance, LPVOID pData);
	BOOL ShowDeskBand(BOOL fShowOrHide = TRUE);
	void CloseDeskBand();
	HWND GetDeskBandHandle();
	SIZE GetDeskBandMinSize(BOOL fNormalViewMode = TRUE);
	void RefreshDeskBand();
	
private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(const HDC hdcIn);

	enum MouseStatus
	{
		Entered,
		Left,
		ClickDown,
		ClickUp
	} Mouse;
};
#endif // _IDESKBANDWINDOW_H_