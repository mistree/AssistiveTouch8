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
	InputEmulation&  mEmu;
	TouchPoint*      mPoints[5];
	TouchEvent*      mEvents[50];
	bool             mEnable;
	bool             mTop;

public:
	TouchDetector(HINSTANCE Dll, InputEmulation& Emu);
	~TouchDetector();
	void Set(bool Enable);
	void Update(int Id,TouchType Type, Point Pos);
	void Register(TouchEvent* Event);
	void Unregister(TouchEvent* Event);

private:
	void QueryEvent(TouchPoint* Point);

	BOOL SetMessageHook(BOOL Install);
};

#endif //TOUCHDETECTOR_H_