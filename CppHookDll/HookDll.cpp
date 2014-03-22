#include <windows.h>
#include <iostream>
#include <fstream>

#define HOOKDLL_EXPORTS
#include "HookDll.h"

// Shared data among all instances.
#pragma data_seg(".HOOKDATA")
HWND g_hWnd = NULL;	        // Window handle
HHOOK g_hHook = NULL;		// Hook handle
DWORD g_idThread = NULL;    // caller thread
UINT Message = NULL;
LPARAM Location = NULL;


// Get module from address
HMODULE WINAPI ModuleFromAddress(PVOID pv) 
{
	MEMORY_BASIC_INFORMATION mbi;
	if (::VirtualQuery(pv, &mbi, sizeof(mbi)) != 0)
	{
		return (HMODULE)mbi.AllocationBase;
	}
	else
	{
		return NULL;
	}
}

// Hook callback
LRESULT CALLBACK MessageHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
	
	Message = ((MSG*)lParam)->message;
	Location = ((MSG*)lParam)->lParam;

	switch (Message)
	{
	case WM_POINTERDOWN:    // 按下
	case WM_NCPOINTERDOWN:
		PostMessage(g_hWnd, USER_DOWN, 0, Location);
		break;
	case WM_POINTERUP:      // 松开
	case WM_NCPOINTERUP:
		PostMessage(g_hWnd, USER_UP, 0, Location);
		break;
	case WM_POINTERUPDATE:  // 移位
	case WM_NCPOINTERUPDATE:
		PostMessage(g_hWnd, USER_MOVE, 0, Location);
		break;
	}
	
    return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

// Install or uninstall the hook
BOOL WINAPI SetMessageHook(BOOL bInstall, DWORD dwThreadId, HWND hWndCaller, DWORD idThread)
{
	BOOL bOk;
	g_hWnd = hWndCaller;
	g_idThread = idThread;
	
	if (bInstall)
	{
		g_hHook = ::SetWindowsHookEx(WH_GETMESSAGE, MessageHookProc,
			ModuleFromAddress(MessageHookProc), dwThreadId);
		bOk = (g_hHook != NULL);
	}
	else 
	{
		bOk = ::UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
	}
	
	return bOk;
}

//case WM_NCPOINTERUPDATE:      
//case WM_NCPOINTERDOWN:        
//case WM_NCPOINTERUP:          
//case WM_POINTERUPDATE:        
//case WM_POINTERDOWN:          
//case WM_POINTERUP:            
//case WM_POINTERENTER:         
//case WM_POINTERLEAVE:         
//case WM_POINTERACTIVATE:      
//case WM_POINTERCAPTURECHANGED:
//case WM_TOUCHHITTESTING:      
//case WM_POINTERWHEEL:         
//case WM_POINTERHWHEEL:        
//case DM_POINTERHITTEST:

//switch (m.message)
//{
//case WM_POINTERDOWN:    // 按下
//	Result = PostMessage(g_hWnd, WM_POINTERDOWN, m.wParam, m.lParam);
//	break;
//case WM_POINTERUP:      // 松开
//	Result = PostMessage(g_hWnd, WM_POINTERUP, m.wParam, m.lParam);
//	break;
//case WM_POINTERUPDATE:  // 移位
//	Result = PostMessage(g_hWnd, WM_POINTERUPDATE, m.wParam, m.lParam);
//	break;
//}


//switch (m.message)
//{
//case WM_POINTERDOWN:    // 按下
//	Result = PostThreadMessage(g_idThread, WM_POINTERDOWN, m.wParam, m.lParam);
//	break;
//case WM_POINTERUP:      // 松开
//	Result = PostThreadMessage(g_idThread, WM_POINTERUP, m.wParam, m.lParam);
//	break;
//case WM_POINTERUPDATE:  // 移位
//	Result = PostThreadMessage(g_idThread, WM_POINTERUPDATE, m.wParam, m.lParam);
//	break;
//}

//PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
//while (GetMessage(...)
//{
//	switch (msg.message)
//	{
//		case ...
//	}
//}


//if (Result)
//{
//	std::ofstream mcfile; //创建个对象 
//	mcfile.open("d:\\hook.txt"); //创建文件 
//	mcfile << "succeed " << Message << "Location x:" << (Location & 0xffff) << " y:" << (Location >> 16); //写入内容 
//	mcfile.close();
//}