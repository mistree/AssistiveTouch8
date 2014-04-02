#ifndef TOUCHDETECTOR_H_
#define TOUCHDETECTOR_H_

#include "Libs.h"
#include "Timer.h"
#include "InputEmulation.h"
#include "IconWindow.h"

class TouchDetector
{
private:
	HINSTANCE       mDll;
	HWND            mIconHwnd;
	HHOOK           mHook;
	InputEmulation& mEmu;
	IconWindow&     mIcon;

public:
	TouchDetector(HINSTANCE Dll, InputEmulation& Emu, IconWindow& Icon);
	~TouchDetector();

// hook functions
private:
	BOOL SetMessageHook(DWORD ThreadId, BOOL Install);
	HMODULE GetFunctionAddress(PVOID Function);

// static callback functions
private:
	static LRESULT CALLBACK MessageHookProc(int nCode, WPARAM wParam, LPARAM lParam);
};

#endif //TOUCHDETECTOR_H_