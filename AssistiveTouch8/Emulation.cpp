#include "Emulation.h"
#include <string>

using namespace AssistiveTouch;

void Emulation::Send( INPUT* Command )
{
  int i=0;
  for (; i<MAXINPUT && Command[i].type != NOTSET; i++)
	  Command[i].ki.dwFlags = KEYEVENTF_SCANCODE;
  if (i == 0) return;
  else if (Command[0].type == INPUT_MOUSE)
  {
	  Command[0].mi.dx = Command[1].mi.dx = Mouse.PosX;
	  Command[0].mi.dy = Command[1].mi.dy = Mouse.PosY;
	  SendInput(1, &Command[0], sizeof(INPUT));
	  Sleep(GapTime);
	  SendInput(1, &Command[1], sizeof(INPUT));
	  return;
  }
  SendInput(i, Command, sizeof(INPUT));
  if ( Command[0].type != INPUT_KEYBOARD )
	  return;
  Sleep(GapTime);
  for (i = 0; i<MAXINPUT && Command[i].type != NOTSET; i++)
	  Command[i].ki.dwFlags |= KEYEVENTF_KEYUP;
  SendInput(i, Command, sizeof(INPUT));
};

void Emulation::SendContinous( INPUT* Command )
{
  LastContinousInput = Command;
  int i=0;
  for (; i<MAXINPUT && Command[i].type != NOTSET; i++)
	  LastContinousInput[i].ki.dwFlags = (Command[0].type == INPUT_KEYBOARD) ? KEYEVENTF_SCANCODE : 0;
  LastContinousInput = (Command[0].type == INPUT_KEYBOARD) ? Command : 0;
  if (i == 0) return;
  SendInput(i, Command, sizeof(INPUT));
};

void Emulation::Reset()
{
  if (LastContinousInput == 0) return;
  int i=0;
  for (; i<MAXINPUT && LastContinousInput[i].type == INPUT_KEYBOARD; i++)
    LastContinousInput[i].ki.dwFlags |= KEYEVENTF_KEYUP;
  if (LastContinousInput[0].type != INPUT_KEYBOARD || i==0)
	  return;
  SendInput(i, LastContinousInput, sizeof(INPUT));
  LastContinousInput = 0;
};

void Emulation::LoadCommandSet(int NumberOfSet)
{
	LastContinousInput = 0;

	char File[] = "%s\\Setting0.ini";
	File[10] = (char)(((int)'0') + NumberOfSet);
	char Path[256];
	char FullPath[256];
	GetCurrentDirectory(256, Path);
	sprintf(FullPath, File, Path);

	LoadKeys(ClickE, "Click", FullPath);
	LoadKeys(UpE, "Up", FullPath);
	LoadKeys(DownE, "Down", FullPath);
	LoadKeys(RightE, "Right", FullPath);
	LoadKeys(LeftE, "Left", FullPath);
	LoadKeys(UpRightE, "UpRight", FullPath);
	LoadKeys(UpLeftE, "UpLeft", FullPath);
	LoadKeys(DownRightE, "DownRight", FullPath);
	LoadKeys(DownLeftE, "DownLeft", FullPath);

	LoadKeys(UpCE, "UpC", FullPath);
	LoadKeys(DownCE, "DownC", FullPath);
	LoadKeys(RightCE, "RightC", FullPath);
	LoadKeys(LeftCE, "LeftC", FullPath);
	LoadKeys(UpRightCE, "UpRightC", FullPath);
	LoadKeys(UpLeftCE, "UpLeftC", FullPath);
	LoadKeys(DownRightCE, "DownRightC", FullPath);
	LoadKeys(DownLeftCE, "DownLeftC", FullPath);
}

void Emulation::LoadKeys(INPUT* Command,char* Name, char* Path)
{
	Command[0].type = Command[1].type = Command[2].type = NOTSET;
	int Type = GetPrivateProfileInt(Name, "Type", 3, Path);
	int Mouse1 = GetPrivateProfileInt(Name, "Mouse1", 0, Path);
	int Mouse2 = GetPrivateProfileInt(Name, "Mouse2", 0, Path);
	int Wheel = GetPrivateProfileInt(Name, "Wheel", 0, Path);
	int Key1 = GetPrivateProfileInt(Name, "Key1", 0, Path);
	int Key2 = GetPrivateProfileInt(Name, "Key2", 0, Path);
	int Key3 = GetPrivateProfileInt(Name, "Key3", 0, Path);

	switch (Type)
	{
	case INPUT_MOUSE:
		Command[0].type = (Mouse1 != 0) ? INPUT_MOUSE : NOTSET;
		Command[0].mi.dwFlags = Mouse1 | MOUSEEVENTF_ABSOLUTE;
		Command[0].mi.mouseData = Wheel;
		Command[0].mi.dwExtraInfo = 0;
		Command[0].mi.time = 0;
		Command[1].type = (Mouse2 != 0) ? INPUT_MOUSE : NOTSET;
		Command[1].mi.dwFlags = Mouse2 | MOUSEEVENTF_ABSOLUTE;
		Command[1].mi.mouseData = Wheel;
		Command[1].mi.dwExtraInfo = 0;
		Command[1].mi.time = 0;
		break;
	case INPUT_KEYBOARD:
		Command[0].ki.wVk = Key1;
		Command[1].ki.wVk = Key2;
		Command[2].ki.wVk = Key3;
		Command[0].type = (Key1 != 0) ? INPUT_KEYBOARD : NOTSET;
		Command[1].type = (Key2 != 0) ? INPUT_KEYBOARD : NOTSET;
		Command[2].type = (Key3 != 0) ? INPUT_KEYBOARD : NOTSET;
		Command[0].ki.wScan = MapVirtualKey(Key1, MAPVK_VK_TO_VSC);
		Command[1].ki.wScan = MapVirtualKey(Key2, MAPVK_VK_TO_VSC);
		Command[2].ki.wScan = MapVirtualKey(Key3, MAPVK_VK_TO_VSC);
		Command[0].ki.dwFlags = Command[1].ki.dwFlags = Command[2].ki.dwFlags = KEYEVENTF_SCANCODE;
		break;
	}
	
}