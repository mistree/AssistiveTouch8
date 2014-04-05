#include "Configuration.h"
#include <Psapi.h>

extern Configuration*   pConfig;

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
};

Configuration::~Configuration()
{
	UnregisterAll();
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
	wcscpy(wcsstr(FileName, L"exe"), L"ini");
	GetModuleFileName(mDll, mFullPath, 256);
	wcscpy(wcsstr(mFullPath, L"AssistiveTouch8.dll"), FileName);
	bool Exist = false;
	for (int i = 0; i < mExistNumber; i++)
	{
		if (wcscmp(mFullPath, mExistFullPath[i]))
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

void          Configuration::Write(const wchar_t* EventName, KeyboardEvent Event)
{

};

void          Configuration::Write(const wchar_t* EventName, MouseEvent Event)
{

};