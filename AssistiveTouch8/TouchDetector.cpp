#include "TouchDetector.h"
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

TouchDetector* pTouch;

TouchDetector::TouchDetector(HINSTANCE Dll, InputEmulation& Emu)
: mEmu(Emu)
, mDll(Dll)
{
	pTouch = this;
	for (auto& i : mPoints) i = nullptr;
	for (auto& i : mEvents) i = nullptr;
	SetMessageHook(GetCurrentThreadId(), true);
};

TouchDetector::~TouchDetector()
{
	for (auto& i : mPoints) if (i != nullptr) delete i;
	SetMessageHook(GetCurrentThreadId(), false);
};

BOOL TouchDetector::SetMessageHook(DWORD ThreadId, BOOL Install)
{
	if (Install)	
		return ( (mHook = ::SetWindowsHookEx(WH_GETMESSAGE, MessageHookProc, GetFunctionAddress(MessageHookProc), ThreadId)) != NULL);
	else
		return UnhookWindowsHookEx(mHook);
};

HMODULE TouchDetector::GetFunctionAddress(PVOID Function)
{
	MEMORY_BASIC_INFORMATION MemoryInfo;
	if (::VirtualQuery(Function, &MemoryInfo, sizeof(MemoryInfo)) != 0)
		return (HMODULE)MemoryInfo.AllocationBase;
	else
		return NULL;
};

LRESULT CALLBACK TouchDetector::MessageHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return ::CallNextHookEx(pTouch->mHook, nCode, wParam, lParam);

	UINT Message = ((MSG*)lParam)->message;
	Point Location = { GET_X_LPARAM(((MSG*)lParam)->lParam), GET_Y_LPARAM(((MSG*)lParam)->lParam) };
	switch (Message)
	{
	case WM_POINTERDOWN:    
	case WM_NCPOINTERDOWN:
		pTouch->Update(ETouchDown, Location);
		break;
	case WM_POINTERUP:      
	case WM_NCPOINTERUP:
		pTouch->Update(ETouchUp, Location);
		break;
	case WM_POINTERUPDATE:  
	case WM_NCPOINTERUPDATE:
		pTouch->Update(ETouchMove, Location);
		break;
	}

	return ::CallNextHookEx(pTouch->mHook, nCode, wParam, lParam);
};

void TouchDetector::Update(TouchType Type, Point Pos)
{
	switch (Type)
	{

	case ETouchDown:
	{
					   bool NewPoint = true;
					   for (auto& i : mPoints)
					   {
						   if (i != nullptr)
						   if (TouchPoint::CalLength(Pos - i->PosCurrent) < TouchPoint::PointSize)
							   NewPoint = false;
					   }
					   if (NewPoint)
					   {
						   for (auto& i : mPoints)
						   {
							   if (i == nullptr)
							   {
								   i = new TouchPoint(Type, Pos);
								   break;
							   }
						   }
					   }
					   break;
	}

	case ETouchMove:
	{
					   for (auto& i : mPoints)
					   {
						   if (i != nullptr)
						   if (TouchPoint::CalLength(Pos - i->PosCurrent) < TouchPoint::PointSize)
						   {
							   i->Update(Type, Pos);
							   QueryEvent(i);
							   break;
						   }
					   }
					   break;
	}
	case ETouchUp:
	{
					 for (auto& i : mPoints)
					 {
						 if (i != nullptr)
						 if (TouchPoint::CalLength(Pos - i->PosCurrent) < TouchPoint::PointSize)
						 {
							 i->Update(Type, Pos);
							 QueryEvent(i);
							 delete i;
							 i = nullptr;
							 break;
						 }
					 }
					 break;
	}
	}
};

void TouchDetector::Register(TouchEvent* Event)
{
	for (auto& i : mEvents)
	{
		if (i == nullptr)
		{
			i = Event;
			return;
		}
	}
};

void TouchDetector::Unregister(TouchEvent* Event)
{
	for (auto& i : mEvents)
	{
		if (i == Event)
		{
			i = nullptr;
			return;
		}
	}
};

void TouchDetector::QueryEvent(TouchPoint* Point)
{
	for (auto& i : mEvents)
	{
		if (i != nullptr)
		{
			i->Update(Point);
		}
	}
};