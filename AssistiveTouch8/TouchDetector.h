#ifndef TOUCHDETECTOR_H_
#define TOUCHDETECTOR_H_

#include <Windows.h>
#include "InputEmulation.h"

#include "TouchPoint.h"
#include "TouchEvent.h"

class TouchDetector
{
private:
	HINSTANCE        mDll;
	HWND             mIconHwnd;
	HHOOK            mHook;
	InputEmulation&  mEmu;
	TouchPoint*      mPoints[5];
	TouchEvent*      mEvents[50];

public:
	TouchDetector(HINSTANCE Dll, InputEmulation& Emu);
	~TouchDetector();

	void Register(TouchEvent* Event);
	void Unregister(TouchEvent* Event);

private:
	void QueryEvent(TouchPoint* Point);
	void Update(TouchType Type, Point Pos);

	BOOL SetMessageHook(DWORD ThreadId, BOOL Install);
	HMODULE GetFunctionAddress(PVOID Function);
	static LRESULT CALLBACK MessageHookProc(int nCode, WPARAM wParam, LPARAM lParam);
};

#endif //TOUCHDETECTOR_H_