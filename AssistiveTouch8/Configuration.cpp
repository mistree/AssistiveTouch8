#include "Configuration.h"
#include <Psapi.h>

Configuration*   pConfig;

Configuration::Configuration(HINSTANCE Dll, TouchDetector& Touch, InputEmulation& Input, IconWindow& Icon)
: mDll(Dll)
, mTouch(Touch)
, mInput(Input)
, mIcon(Icon)
, mExistConfigNumber(0)
{
	pConfig = this;
	for (auto& i : mEvents) i = nullptr;
	for (auto& i : mExistAppPath) i = nullptr;
	for (auto& i : mExistAppConfigPath) i = nullptr;

	GetModuleFileName(Dll, mCurrentAppConfigPath, 256);
	wcscpy(wcsstr(mCurrentAppConfigPath, L"AssistiveTouch8.dll"), L"Default.ini");

	Refresh();
	UpdateEvents();
};

Configuration::~Configuration()
{
	UnregisterAll();
	for (auto& i : mExistAppPath)
	if (i != nullptr) delete[] i;
	for (auto& i : mExistAppConfigPath)
	if (i != nullptr) delete[] i;
};

void Configuration::Refresh()
{
	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;
	wchar_t sPath[256];
	wchar_t Temp[256];
	GetModuleFileName(mDll, sPath, 256);
	GetModuleFileName(mDll, Temp, 256);
	wcscpy(wcsstr(sPath, L"AssistiveTouch8.dll"), L"*.ini");
	wcscpy(wcsstr(Temp, L"AssistiveTouch8.dll"), L"");
	if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
		return;
	do
	{
		if (wcscmp(fdFile.cFileName, L".") != 0
			&& wcscmp(fdFile.cFileName, L"..") != 0)
		{
			mExistAppPath[mExistConfigNumber] = new wchar_t[256];
			mExistAppConfigPath[mExistConfigNumber] = new wchar_t[256];
			wsprintf(mExistAppConfigPath[mExistConfigNumber], L"%s%s", Temp, fdFile.cFileName);
			GetPrivateProfileString(L"Exe", L"Path", L"None", mExistAppPath[mExistConfigNumber], 256, mExistAppConfigPath[mExistConfigNumber]);
			mExistConfigNumber++;
		}
	} while (FindNextFile(hFind, &fdFile));
	FindClose(hFind);
};

void Configuration::Update(HWND FrontWindow)
{
	DWORD Pid;
	HANDLE PidHandle = NULL;
	wchar_t FilePath[256];
	GetWindowThreadProcessId(FrontWindow, &Pid);
	if (Pid == GetCurrentThreadId())
		return;
	PidHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, Pid);
	if (GetModuleFileNameEx(PidHandle, NULL, FilePath, 256) == 0)
		return;
	if (wcscmp(FilePath, mCurrentAppPath) == 0)
		return;

	int Offset = 0;
	for (auto& i : mExistAppPath)
	{
		if (i!= nullptr)
		if (wcscmp(FilePath, i) == 0)
		{
			wcscpy(mCurrentAppPath, FilePath);
			wcscpy(mCurrentAppConfigPath, mExistAppConfigPath[Offset]);
			UpdateEvents();
			return;
		}
		Offset++;
	}
	wcscpy(mCurrentAppPath, L"Default");
	GetModuleFileName(mDll, mCurrentAppConfigPath, 256);
	wcscpy(wcsstr(mCurrentAppConfigPath, L"AssistiveTouch8.dll"), L"Default.ini");
	UpdateEvents();
};

void Configuration::UpdateEvents()
{
	UnregisterAll();

	mEvents[0] = new DragEvent(mInput, mIcon, 1500);

	int ValidCount = 1;
	if (GetPrivateProfileInt(L"Click", L"KeyboardValid", 0, mCurrentAppConfigPath))
	{
		mEvents[ValidCount] = new ClickEvent(mInput, mIcon, 
			  LoadKeyboard(L"Click"), 
			  GetPrivateProfileInt(L"Click", L"Time", 1000, mCurrentAppConfigPath)
			  );
		ValidCount++;
	}
	else if (GetPrivateProfileInt(L"Click", L"MouseValid", 0, mCurrentAppConfigPath))
	{	
		mEvents[ValidCount] = new ClickEvent(mInput, mIcon,
			LoadMouse(L"Click"),
			GetPrivateProfileInt(L"Click", L"Time", 1000, mCurrentAppConfigPath)
			);
		ValidCount++;
	}

	LoadSlideEvents(L"SlideUp", EDirectionUp, ValidCount);
	LoadSlideEvents(L"SlideDown", EDirectionDown, ValidCount);
	LoadSlideEvents(L"SlideLeft", EDirectionLeft, ValidCount);
	LoadSlideEvents(L"SlideRight", EDirectionRight, ValidCount);
	LoadSlideEvents(L"SlideUpLeft", EDirectionUpLeft, ValidCount);
	LoadSlideEvents(L"SlideUpRight", EDirectionUpRight, ValidCount);
	LoadSlideEvents(L"SlideDownLeft", EDirectionDownLeft, ValidCount);
	LoadSlideEvents(L"SlideDownRight", EDirectionDownRight, ValidCount);

	LoadSlideContinousEvents(L"SlideContinousUp", EDirectionUp, ValidCount);
	LoadSlideContinousEvents(L"SlideContinousDown", EDirectionDown, ValidCount);
	LoadSlideContinousEvents(L"SlideContinousLeft", EDirectionLeft, ValidCount);
	LoadSlideContinousEvents(L"SlideContinousRight", EDirectionRight, ValidCount);
	LoadSlideContinousEvents(L"SlideContinousUpLeft", EDirectionUpLeft, ValidCount);
	LoadSlideContinousEvents(L"SlideContinousUpRight", EDirectionUpRight, ValidCount);
	LoadSlideContinousEvents(L"SlideContinousDownLeft", EDirectionDownLeft, ValidCount);
	LoadSlideContinousEvents(L"SlideContinousDownRight", EDirectionDownRight, ValidCount);
	
	RegisterAll();
};

void Configuration::UnregisterAll()
{
	for (auto& i : mEvents)
	{
		if (i != nullptr)
		{
			mTouch.Unregister(i);
			delete i;
			i = nullptr;
		}
	}
};

void Configuration::RegisterAll()
{
	for (auto& i : mEvents)
	{
		if (i != nullptr)
			mTouch.Register(i);
	}
};

KeyboardEvent Configuration::LoadKeyboard(const wchar_t* EventName)
{
	KeyboardEvent k;
	k.NumberOfKeys = GetPrivateProfileInt(EventName, L"NumberOfKeys", 0, mCurrentAppConfigPath);
	k.Keys[0] = GetPrivateProfileInt(EventName, L"Key1", 0, mCurrentAppConfigPath);
	k.Keys[1] = GetPrivateProfileInt(EventName, L"Key2", 0, mCurrentAppConfigPath);
	k.Keys[2] = GetPrivateProfileInt(EventName, L"Key3", 0, mCurrentAppConfigPath);
	return k;
};

MouseEvent    Configuration::LoadMouse(const wchar_t* EventName)
{
	MouseEvent m;
	m.Continous = GetPrivateProfileInt(EventName, L"MouseContinous", 0, mCurrentAppConfigPath);
	m.Button = GetPrivateProfileInt(EventName, L"MouseButton", 0, mCurrentAppConfigPath);
	m.WheelMovement = GetPrivateProfileInt(EventName, L"WheelMovement", 0, mCurrentAppConfigPath);
	if (!m.Continous)
		m.ButtonUp = GetPrivateProfileInt(EventName, L"MouseButtonUp", 0, mCurrentAppConfigPath);
	return m;
};

void          Configuration::Write(const wchar_t* EventName, KeyboardEvent Event, int Time, int Interval, const wchar_t* Path)
{
	WritePrivateProfileString(EventName, L"KeyboardValid", L"1", Path);
	wchar_t Temp[32];
	swprintf(Temp, L"%d", Event.NumberOfKeys);
	WritePrivateProfileString(EventName, L"NumberOfKeys", Temp, Path);
	swprintf(Temp, L"%d", Event.Keys[0]);
	WritePrivateProfileString(EventName, L"Key1", Temp, Path);
	swprintf(Temp, L"%d", Event.Keys[1]);
	WritePrivateProfileString(EventName, L"Key2", Temp, Path);
	swprintf(Temp, L"%d", Event.Keys[2]);
	WritePrivateProfileString(EventName, L"Key3", Temp, Path);
	swprintf(Temp, L"%d", Time);
	WritePrivateProfileString(EventName, L"Time", Temp, Path);
	swprintf(Temp, L"%d", Interval);
	WritePrivateProfileString(EventName, L"Interval", Temp, Path);
};

void          Configuration::Write(const wchar_t* EventName, MouseEvent Event, int Time, int Interval, const wchar_t* Path)
{
	WritePrivateProfileString(EventName, L"MouseValid", L"1", Path);
	wchar_t Temp[32];
	swprintf(Temp, L"%d", Event.Continous);
	WritePrivateProfileString(EventName, L"MouseContinous", Temp, Path);
	swprintf(Temp, L"%d", Event.Button);
	WritePrivateProfileString(EventName, L"MouseButton", Temp, Path);
	swprintf(Temp, L"%d", Event.WheelMovement);
	WritePrivateProfileString(EventName, L"WheelMovement", Temp, Path);
	swprintf(Temp, L"%d", Event.ButtonUp);
	WritePrivateProfileString(EventName, L"MouseButtonUp", Temp, Path);
	swprintf(Temp, L"%d", Time);
	WritePrivateProfileString(EventName, L"Time", Temp, Path);
	swprintf(Temp, L"%d", Interval);
	WritePrivateProfileString(EventName, L"Interval", Temp, Path);
};

void          Configuration::LoadSlideEvents(const wchar_t* EventName, TouchDirection Direction, int& ValidCount)
{
	if (GetPrivateProfileInt(EventName, L"KeyboardValid", 0, mCurrentAppConfigPath))
	{
		mEvents[ValidCount] = new SlideEvent(mInput, mIcon,
			LoadKeyboard(EventName),
			Direction,
			GetPrivateProfileInt(EventName, L"Time", 1000, mCurrentAppConfigPath)
			);
		ValidCount++;
	}
	else if (GetPrivateProfileInt(EventName, L"MouseValid", 0, mCurrentAppConfigPath))
	{
		mEvents[ValidCount] = new SlideEvent(mInput, mIcon,
			LoadMouse(EventName),
			Direction,
			GetPrivateProfileInt(EventName, L"Time", 1000, mCurrentAppConfigPath)
			);
		ValidCount++;
	}
};

void          Configuration::LoadSlideContinousEvents(const wchar_t* EventName, TouchDirection Direction, int& ValidCount)
{
	if (GetPrivateProfileInt(EventName, L"KeyboardValid", 0, mCurrentAppConfigPath))
	{
		mEvents[ValidCount] = new SlideContinousEvent(mInput, mIcon,
			LoadKeyboard(EventName),
			Direction,
			GetPrivateProfileInt(EventName, L"Time", 1000, mCurrentAppConfigPath),
			GetPrivateProfileInt(EventName, L"Interval", 1000, mCurrentAppConfigPath)
			);
		ValidCount++;
	}
	else if (GetPrivateProfileInt(EventName, L"MouseValid", 0, mCurrentAppConfigPath))
	{
		mEvents[ValidCount] = new SlideContinousEvent(mInput, mIcon,
			LoadMouse(EventName),
			Direction,
			GetPrivateProfileInt(EventName, L"Time", 1000, mCurrentAppConfigPath),
			GetPrivateProfileInt(EventName, L"Interval", 1000, mCurrentAppConfigPath)
			);
		ValidCount++;
	}
};