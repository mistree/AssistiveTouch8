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
	wchar_t         mCurrentAppConfigPath[256]; // path of configuration file
	wchar_t         mCurrentAppPath[256];       // path of fron app

	wchar_t*        mExistAppPath[50];          // paths of apps in config files
	wchar_t*        mExistAppConfigPath[50];    // paths of config files
	int             mExistConfigNumber;         // number of exist config files
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
	void          Refresh();
	void          Write(const wchar_t* EventName, KeyboardEvent Event, int Time, int Interval, const wchar_t* Path);
	void          Write(const wchar_t* EventName, MouseEvent Event, int Time, int Interval, const wchar_t* Path);

private:
	void          LoadSlideEvents(const wchar_t* EventName,TouchDirection Direction,int& ValidCount);
	void          LoadSlideContinousEvents(const wchar_t* EventName, TouchDirection Direction, int& ValidCount);
};

#endif //CONFIGURATION_H_