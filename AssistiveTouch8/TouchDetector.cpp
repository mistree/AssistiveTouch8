#include "TouchDetector.h"
#include "Configuration.h"

#define USER_F_CHANGE WM_USER+103
#define USER_UP WM_USER+102
#define USER_DOWN WM_USER+100
#define USER_MOVE WM_USER+101


TouchDetector*   pTouch = nullptr;

#pragma data_seg(".shared")
HHOOK mHookMessage = NULL;
HHOOK mHookWin = NULL;
HWND  mHookHwnd = NULL;
#pragma data_seg()
#pragma comment(linker,"/section:.shared,rws")


HMODULE GetFunctionAddress(PVOID Function);
static LRESULT CALLBACK MessageHookProc(int nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK WinHookProc(int nCode, WPARAM wParam, LPARAM lParam);

TouchDetector::TouchDetector(HINSTANCE Dll, InputEmulation& Emu)
: mEmu(Emu)
, mDll(Dll)
{
	pTouch = this;
	for (auto& i : mPoints) i = nullptr;
	for (auto& i : mEvents) i = nullptr;
	SetMessageHook(true);
};

TouchDetector::~TouchDetector()
{
	for (auto& i : mPoints) if (i != nullptr) delete i;
	SetMessageHook(false);
};

BOOL TouchDetector::SetMessageHook(BOOL Install)
{
	if (Install)
	{
		return ((mHookMessage = ::SetWindowsHookEx(WH_GETMESSAGE, MessageHookProc, mDll, 0)) != NULL)
			 && ((mHookWin = ::SetWindowsHookEx(WH_CBT, WinHookProc, mDll, 0)) != NULL);
	}
	else
	{
		return UnhookWindowsHookEx(mHookMessage)
		    && UnhookWindowsHookEx(mHookWin);
	}
};

void TouchDetector::Update(int Id,TouchType Type, Point Pos)
{
	switch (Type)
	{

	case ETouchDown:
	{
					   for (auto& i : mPoints)
					   {
						   if (i == nullptr)
						   {
							   i = new TouchPoint(Id,Type, Pos);
							   QueryEvent(i);
							   break;
						   }
					   }
					   break;
	}

	case ETouchMove:
	{
					   for (auto& i : mPoints)
					   {
						   if (i != nullptr)
						   if (i->Id == Id)
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
						 if (i->Id == Id)
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
		if (i != nullptr)
			i->Update(Point);
};

HMODULE GetFunctionAddress(PVOID Function)
{
	MEMORY_BASIC_INFORMATION MemoryInfo;
	if (::VirtualQuery(Function, &MemoryInfo, sizeof(MemoryInfo)) != 0)
		return (HMODULE)MemoryInfo.AllocationBase;
	else
		return NULL;
};

LRESULT CALLBACK WinHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	switch (nCode)
	{
	case HCBT_ACTIVATE:
	case HCBT_MOVESIZE:
	case HCBT_SETFOCUS:
		PostMessage(mHookHwnd, USER_F_CHANGE, wParam, lParam);
	};

	return ::CallNextHookEx(mHookWin, nCode, wParam, lParam);
};

LRESULT CALLBACK MessageHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return ::CallNextHookEx(mHookMessage, nCode, wParam, lParam);
	UINT Message = ((MSG*)lParam)->message;
	switch (Message)
	{
	case WM_POINTERDOWN:
	case WM_NCPOINTERDOWN:
		PostMessage(mHookHwnd, USER_DOWN, ((MSG*)lParam)->wParam, ((MSG*)lParam)->lParam);
		break;
	case WM_POINTERUP:
	case WM_NCPOINTERUP:
		PostMessage(mHookHwnd, USER_UP, ((MSG*)lParam)->wParam, ((MSG*)lParam)->lParam);
		break;
	case WM_POINTERUPDATE:
	case WM_NCPOINTERUPDATE:
		PostMessage(mHookHwnd, USER_MOVE, ((MSG*)lParam)->wParam, ((MSG*)lParam)->lParam);
		break;
	}
	return 0;// ::CallNextHookEx(mHookMessage, nCode, wParam, lParam);
};
