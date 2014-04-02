#include "TouchDetector.h"
TouchDetector* pTouch;

TouchDetector::TouchDetector(HINSTANCE Dll, InputEmulation& Emu, IconWindow& Icon)
: mEmu(Emu)
, mIcon(Icon)
{

};

TouchDetector::~TouchDetector()
{

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
	//Location = ((MSG*)lParam)->lParam;

	switch (Message)
	{
	case WM_POINTERDOWN:    // 按下
	case WM_NCPOINTERDOWN:

		break;
	case WM_POINTERUP:      // 松开
	case WM_NCPOINTERUP:

		break;
	case WM_POINTERUPDATE:  // 移位
	case WM_NCPOINTERUPDATE:

		break;
	}

	return ::CallNextHookEx(pTouch->mHook, nCode, wParam, lParam);
};