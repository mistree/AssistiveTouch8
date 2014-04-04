#include "Configuration.h"

Configuration::Configuration(HINSTANCE Dll)
{
	mDll = Dll;
	//GetModuleFileName(mDll, mFullPath, 256);
	//wcscpy(wcsstr(mFullPath, L"AssistiveTouch8.dll"), L"Global.ini");
}