#ifdef HOOKDLL_EXPORTS
#define HOOKDLL_API __declspec(dllexport)
#else
#define HOOKDLL_API __declspec(dllimport)
#endif

#define        USER_UP       (WM_USER + 1)
#define        USER_DOWN     (WM_USER + 2)
#define        USER_MOVE     (WM_USER + 3)


//exported functions.
BOOL HOOKDLL_API WINAPI SetMessageHook(BOOL bInstall, 
										DWORD dwThreadId = 0, 
										HWND hWndCaller = NULL,
										DWORD idThread = NULL);