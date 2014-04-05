#ifndef TOUCHEVENT_H
#define TOUCHEVENT_H

#include "IconWindow.h"
#include "InputEmulation.h"
#include "TouchPoint.h"

class TouchEvent
{
protected:
	InputEmulation& mEmu;

public:
	TouchEvent(InputEmulation& Emu)
		: mEmu(Emu) 
	{};
	virtual ~TouchEvent()
	{};
	virtual void Update(TouchPoint* Point)
	{};
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
	void Update(TouchPoint* Point);
};

class ClickEvent : public TouchEvent
{
private:
	IconWindow&   mIcon;
	KeyboardEvent mEventK;
	MouseEvent    mEventM;
	EventType     mType;
	int           mTime;
public:
	ClickEvent(InputEmulation& Emu, IconWindow& Icon, KeyboardEvent Event,int Time)
		: TouchEvent(Emu)
		, mIcon(Icon)
		, mEventK(Event)
		, mType(EKeyboard)
		, mTime(Time)
	{};
	ClickEvent(InputEmulation& Emu, IconWindow& Icon, MouseEvent Event,int Time)
		: TouchEvent(Emu)
		, mIcon(Icon)
		, mEventM(Event)
		, mType(EKeyboard)
		, mTime(Time)
	{};
	void Update(TouchPoint* Point);
};

#endif //TOUCHEVENT_H