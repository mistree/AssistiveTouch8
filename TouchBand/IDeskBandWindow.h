#ifndef _IDESKBANDWINDOW_H_
#define _IDESKBANDWINDOW_H_

#include "Libs.h"

class IDeskBandWindow
{
public:
	virtual BOOL CreateDeskBand(HWND hParentWnd, HINSTANCE hInstance, LPVOID pData) = 0;
	virtual BOOL ShowDeskBand(BOOL fShowOrHide = TRUE) = 0;
	virtual void CloseDeskBand() = 0;
	virtual HWND GetDeskBandHandle() = 0;
	virtual SIZE GetDeskBandMinSize(BOOL fNormalViewMode = TRUE) = 0;
	virtual void RefreshDeskBand() = 0;
};
#endif // _IDESKBANDWINDOW_H_