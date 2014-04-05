#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <Windows.h>
#include "IconWindow.h"
#include "TouchDetector.h"
#include "InputEmulation.h"

class Configuration
{
private:
	HINSTANCE       mDll;
	wchar_t         mFullPath[256]; // path of configuration file
	wchar_t         mFilePath[256]; // path of fron app
	wchar_t**       mExistFullPath; // paths of exist configuration files
	int             mExistNumber;   // number of exist configuration files
	wchar_t         mConfigName[256]; // current configuration file name
	TouchDetector&  mTouch;
	InputEmulation& mInput;
	IconWindow&     mIcon;
	TouchEvent*     mEvents[50];

public:
	Configuration(HINSTANCE Dll, TouchDetector& Touch, InputEmulation& Input, IconWindow& Icon);
	~Configuration();

	void Update(HWND FrontWindow);
	void UpdateEvents();
	void RegisterAll();
	void UnregisterAll();

	KeyboardEvent LoadKeyboard(const wchar_t* EventName);
	MouseEvent    LoadMouse(const wchar_t* EventName);
	void          Write(const wchar_t* EventName, KeyboardEvent Event);
	void          Write(const wchar_t* EventName, MouseEvent Event);
};

#endif //CONFIGURATION_H_