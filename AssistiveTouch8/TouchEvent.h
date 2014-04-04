#ifndef TOUCHEVENT_H
#define TOUCHEVENT_H

#include "IconWindow.h"
#include "InputEmulation.h"
#include "TouchPoint.h"

class TouchEvent
{
private:
	InputEmulation& mEmu;

public:
	TouchEvent(InputEmulation& Emu)
		: mEmu(Emu) 
	{};
	virtual ~TouchEvent()
	{};
	virtual bool Update(TouchPoint* Point)
	{
		return true;
	};
};


class DragEvent : public TouchEvent
{
private:
	IconWindow& mIcon;
	int         mTime;

public:
	DragEvent(InputEmulation& Emu, IconWindow& Icon, int Time)
		: TouchEvent(Emu)
		, mIcon(Icon)
		, mTime(Time)
	{};
	bool Update(TouchPoint* Point);
};

#endif //TOUCHEVENT_H