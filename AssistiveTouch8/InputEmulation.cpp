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
		Command.ki.dwFlags = (Event.Keys[i] < 0xA6)
			                ? (((PressDown) ? 0 : KEYEVENTF_KEYUP) | KEYEVENTF_SCANCODE)
							: (((PressDown) ? 0 : KEYEVENTF_KEYUP) | KEYEVENTF_SCANCODE | KEYEVENTF_EXTENDEDKEY);
		SendInput(1, &Command, sizeof(INPUT));
	}
};

void InputEmulation::Send(MouseEvent Event, Point Pos)
{
	if (!Event.Continous)
	{
		INPUT CommandMove = { 0 };
		CommandMove.type = INPUT_MOUSE;
		CommandMove.mi.time = 0;
		CommandMove.mi.dx = Pos.X;
		CommandMove.mi.dy = Pos.Y;
		CommandMove.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
		CommandMove.mi.mouseData = Event.WheelMovement;
		SendInput(1, &CommandMove, sizeof(INPUT));
	}
	
	INPUT Command = { 0 };
	Command.type = INPUT_MOUSE;
	Command.mi.time = 0;
	Command.mi.dx = Pos.X;
	Command.mi.dy = Pos.Y;
	Command.mi.dwFlags = Event.Button | MOUSEEVENTF_ABSOLUTE;
	Command.mi.mouseData = Event.WheelMovement;
	SendInput(1, &Command, sizeof(INPUT));
	if (!Event.Continous)
	{
		Sleep(Interval);
		Command.mi.dwFlags = Event.ButtonUp | MOUSEEVENTF_ABSOLUTE;
	}
};