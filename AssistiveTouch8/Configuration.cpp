#include "Configuration.h"
#include <Psapi.h>

Configuration*   pConfig;

Configuration::Configuration(HINSTANCE Dll, TouchDetector& Touch, InputEmulation& Input, IconWindow& Icon)
: mDll(Dll)
, mTouch(Touch)
, mInput(Input)
, mIcon(Icon)
, mExistNumber(0)
{
	pConfig = this;
	for (auto& i : mEvents) i = nullptr;

	GetModuleFileName(Dll, mFullPath, 256);
	wcscpy(wcsstr(mFullPath, L"AssistiveTouch8.dll"), L"Default.ini");
	UpdateEvents();

	for (auto& i : mExistFullPath) i = nullptr;
};

Configuration::~Configuration()
{
	UnregisterAll();
	for (auto& i : mExistFullPath) 
	if (i != nullptr) delete i;
};

void Configuration::Refresh()
{
	//mExistFullPath
	//mExistNumber

	//WIN32_FIND_DATA fdFile;
	//HANDLE hFind = NULL;
	//wchar_t sPath[2048];
	////Specify a file mask. *.* = We want everything! 
	//wsprintf(sPath, L"%s\\*.*", sDir);
	//if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
	//{
	//	wprintf(L"Path not found: [%s]\n", sDir);
	//	return false;
	//}
	//do
	//{
	//	//Find first file will always return "."
	//	//    and ".." as the first two directories. 
	//	if (wcscmp(fdFile.cFileName, L".") != 0
	//		&& wcscmp(fdFile.cFileName, L"..") != 0)
	//	{
	//		//Build up our file path using the passed in 
	//		//  [sDir] and the file/foldername we just found: 
	//		wsprintf(sPath, L"%s\\%s", sDir, fdFile.cFileName);
	//
	//		//Is the entity a File or Folder? 
	//		if (fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
	//		{
	//			wprintf(L"Directory: %s\n", sPath);
	//			ListDirectoryContents(sPath); //Recursion, I love it! 
	//		}
	//		else{
	//			wprintf(L"File: %s\n", sPath);
	//		}
	//	}
	//} while (FindNextFile(hFind, &fdFile)); //Find the next file. 
	//FindClose(hFind); //Always, Always, clean things up! 
};

void Configuration::Update(HWND FrontWindow)
{
	DWORD Pid;
	HANDLE PidHandle = NULL;
	wchar_t FilePath[512];
	GetWindowThreadProcessId(FrontWindow, &Pid);
	if (Pid == GetCurrentThreadId())
		return;
	PidHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, Pid);
	if (GetModuleFileNameEx(PidHandle, NULL, FilePath, 512) == 0)
		return;
	if (wcscmp(FilePath, mFilePath) == 0)
		return;
	wcscpy(mFilePath, FilePath);
	wchar_t* FileName;
	wchar_t* Temp = wcsstr(FilePath, L"\\");
	for (;;)
	{
		FileName = Temp + 1;
		Temp = wcsstr(FileName, L"\\");
		if (Temp == L'\0')
			break;
	};	
	FileName = _wcslwr(FileName);
	wcsncpy(wcsstr(FileName, L"exe"), L"ini", 3);
	GetModuleFileName(mDll, mFullPath, 512);
	wcscpy(wcsstr(mFullPath, L"AssistiveTouch8.dll"), FileName);
	bool Exist = false;
	for (auto& i : mExistFullPath)
	{
		if (i == nullptr)
			continue;
		if (wcscmp(mFullPath, i))
		{
			Exist = true;
			break;
		}
	}

	if (!Exist && wcscmp(mConfigName, L"Default.ini"))
		return;
	if (!Exist)
		wcscpy(wcsstr(mFullPath, FileName), L"Default.ini");
	else
		wcscpy(mConfigName, FileName);	
};

void Configuration::UpdateEvents()
{
	UnregisterAll();

	mEvents[0] = new DragEvent(mInput, mIcon, 1500);

	int ValidCount = 1;
	if (GetPrivateProfileInt(L"Click", L"KeyboardValid", 0, mFullPath))
	{
		mEvents[ValidCount] = new ClickEvent(mInput, mIcon, 
			  LoadKeyboard(L"Click"), 
			  GetPrivateProfileInt(L"Click", L"Time", 1000, mFullPath)
			  );
		ValidCount++;
	}
	else if (GetPrivateProfileInt(L"Click", L"MouseValid", 0, mFullPath))
	{	
		mEvents[ValidCount] = new ClickEvent(mInput, mIcon,
			LoadMouse(L"Click"),
			GetPrivateProfileInt(L"Click", L"Time", 1000, mFullPath)
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
	k.NumberOfKeys = GetPrivateProfileInt(EventName, L"NumberOfKeys", 0, mFullPath);
	k.Keys[0] = GetPrivateProfileInt(EventName, L"Key1", 0, mFullPath);
	k.Keys[1] = GetPrivateProfileInt(EventName, L"Key2", 0, mFullPath);
	k.Keys[2] = GetPrivateProfileInt(EventName, L"Key3", 0, mFullPath);
	return k;
};

MouseEvent    Configuration::LoadMouse(const wchar_t* EventName)
{
	MouseEvent m;
	m.Continous = GetPrivateProfileInt(EventName, L"MouseContinous", 0, mFullPath);
	m.Button = GetPrivateProfileInt(EventName, L"MouseButton", 0, mFullPath);
	m.WheelMovement = GetPrivateProfileInt(EventName, L"WheelMovement", 0, mFullPath);
	if (!m.Continous)
		m.ButtonUp = GetPrivateProfileInt(EventName, L"MouseButtonUp", 0, mFullPath);
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
	if (GetPrivateProfileInt(EventName, L"KeyboardValid", 0, mFullPath))
	{
		mEvents[ValidCount] = new SlideEvent(mInput, mIcon,
			LoadKeyboard(EventName),
			Direction,
			GetPrivateProfileInt(EventName, L"Time", 1000, mFullPath)
			);
		ValidCount++;
	}
	else if (GetPrivateProfileInt(EventName, L"MouseValid", 0, mFullPath))
	{
		mEvents[ValidCount] = new SlideEvent(mInput, mIcon,
			LoadMouse(EventName),
			Direction,
			GetPrivateProfileInt(EventName, L"Time", 1000, mFullPath)
			);
		ValidCount++;
	}
};

void          Configuration::LoadSlideContinousEvents(const wchar_t* EventName, TouchDirection Direction, int& ValidCount)
{
	if (GetPrivateProfileInt(EventName, L"KeyboardValid", 0, mFullPath))
	{
		mEvents[ValidCount] = new SlideContinousEvent(mInput, mIcon,
			LoadKeyboard(EventName),
			Direction,
			GetPrivateProfileInt(EventName, L"Time", 1000, mFullPath),
			GetPrivateProfileInt(EventName, L"Interval", 1000, mFullPath)
			);
		ValidCount++;
	}
	else if (GetPrivateProfileInt(EventName, L"MouseValid", 0, mFullPath))
	{
		mEvents[ValidCount] = new SlideContinousEvent(mInput, mIcon,
			LoadMouse(EventName),
			Direction,
			GetPrivateProfileInt(EventName, L"Time", 1000, mFullPath),
			GetPrivateProfileInt(EventName, L"Interval", 1000, mFullPath)
			);
		ValidCount++;
	}
};