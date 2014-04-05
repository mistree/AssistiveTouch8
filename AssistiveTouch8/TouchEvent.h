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

class SlideEvent : public TouchEvent
{
private:
	IconWindow&   mIcon;
	KeyboardEvent mEventK;
	MouseEvent    mEventM;
	EventType     mType;
	int           mTime;
	TouchDirection mDirection;
public:
	SlideEvent(InputEmulation& Emu, IconWindow& Icon, KeyboardEvent Event, TouchDirection Direction, int Time)
		: TouchEvent(Emu)
		, mIcon(Icon)
		, mEventK(Event)
		, mType(EKeyboard)
		, mTime(Time)
		, mDirection(Direction)
	{};
	SlideEvent(InputEmulation& Emu, IconWindow& Icon, MouseEvent Event, TouchDirection Direction, int Time)
		: TouchEvent(Emu)
		, mIcon(Icon)
		, mEventM(Event)
		, mType(EKeyboard)
		, mTime(Time)
		, mDirection(Direction)
	{};
	void Update(TouchPoint* Point);
};

class SlideContinousEvent : public TouchEvent
{
private:
	IconWindow&   mIcon;
	KeyboardEvent mEventK;
	MouseEvent    mEventM;
	EventType     mType;
	int           mTime;
	int           mInterval;
	TouchDirection mDirection;
public:
	SlideContinousEvent(InputEmulation& Emu, IconWindow& Icon, KeyboardEvent Event, TouchDirection Direction, int Time, int Interval)
		: TouchEvent(Emu)
		, mIcon(Icon)
		, mEventK(Event)
		, mType(EKeyboard)
		, mTime(Time)
		, mDirection(Direction)
		, mInterval(Interval)
	{};
	SlideContinousEvent(InputEmulation& Emu, IconWindow& Icon, MouseEvent Event, TouchDirection Direction, int Time, int Interval)
		: TouchEvent(Emu)
		, mIcon(Icon)
		, mEventM(Event)
		, mType(EKeyboard)
		, mTime(Time)
		, mDirection(Direction)
		, mInterval(Interval)
	{};
	void Update(TouchPoint* Point);
};
#endif //TOUCHEVENT_H