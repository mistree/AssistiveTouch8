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
	wchar_t         mFullPath[512]; // path of configuration file
	wchar_t         mFilePath[512]; // path of fron app
	wchar_t*        mExistFullPath[50]; // paths of exist configuration files
	int             mExistNumber;   // number of exist configuration files
	wchar_t         mConfigName[512]; // current configuration file name
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