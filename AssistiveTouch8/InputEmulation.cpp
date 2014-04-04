#include "InputEmulation.h"

void InputEmulation::Send(KeyboardEvent Event)
{	
	Send(Event, true);
	Sleep(Interval);
	Send(Event, false);
};

void InputEmulation::Send(KeyboardEvent Event, bool PressDown)
{
	for (int i = 0; i < Event.NumberOfKeys; i++)
	{
		INPUT Command = { 0 };
		Command.type = INPUT_KEYBOARD;
		Command.ki.time = 0;
		Command.ki.wVk = Event.Keys[i];
		Command.ki.wScan = MapVirtualKey(Event.Keys[i], MAPVK_VK_TO_VSC);
		Command.ki.dwFlags = (0) ? KEYEVENTF_SCANCODE | ((PressDown) ? 0 : KEYEVENTF_KEYUP) : KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY | ((PressDown) ? 0 : KEYEVENTF_KEYUP);
		SendInput(1, &Command, sizeof(INPUT));
	}
};

void InputEmulation::Send(MouseEvent Event, Point Pos)
{
	INPUT Command = { 0 };
	Command.type = INPUT_MOUSE;
	Command.mi.time = 0;
	Command.mi.dx = Pos.X;
	Command.mi.dy = Pos.Y;
	Command.mi.dwFlags = Event.Button | MOUSEEVENTF_ABSOLUTE;
	Command.mi.mouseData = Event.WheelMovement;
	SendInput(1, &Command, sizeof(INPUT));
};