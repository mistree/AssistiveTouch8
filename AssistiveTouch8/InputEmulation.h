#ifndef INPUTEMULATION_H_
#define INPUTEMULATION_H_

#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

#include "Timer.h"

enum EventType
{
	EKeyboard,
	EMouse
};

struct KeyboardEvent
{
	int NumberOfKeys = 0;
	int Keys[3];
};

struct MouseEvent
{
	int WheelMovement = 0;
	int Button = 0;
	bool Continous = false;
	int ButtonUp = 0;
};

class InputEmulation
{
public:
	int  Interval = 100; // ms

	void Send(KeyboardEvent Event);
	void Send(KeyboardEvent Event, bool PressDown);
	void Send(MouseEvent Event, Point Pos);

private:
	INPUT Command[3];
};

#endif //INPUTEMULATION_H_