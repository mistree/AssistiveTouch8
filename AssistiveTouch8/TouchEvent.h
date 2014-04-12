#ifndef TOUCHEVENT_H
#define TOUCHEVENT_H

#include "IconWindow.h"
#include "InputEmulation.h"
#include "TouchPoint.h"
#include "Timer.h"

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
	virtual bool Update(TouchPoint* Point)
	{
		return false;
	};
};

class DragEvent : public TouchEvent
{
private:
	IconWindow& mIcon;
	int         mTime;
	bool        mValid;

public:
	DragEvent(InputEmulation& Emu, IconWindow& Icon, int Time)
		: TouchEvent(Emu)
		, mIcon(Icon)
		, mTime(Time)
		, mValid(false)
	{};
	bool Update(TouchPoint* Point);
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
		, mType(EMouse)
		, mTime(Time)
	{};
	bool Update(TouchPoint* Point);
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
		, mType(EMouse)
		, mTime(Time)
		, mDirection(Direction)
	{};
	bool Update(TouchPoint* Point);
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
	bool          mPressed;
	Timer         mTimer;
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
		, mPressed(false)
	{
		mTimer.Start();
	};
	SlideContinousEvent(InputEmulation& Emu, IconWindow& Icon, MouseEvent Event, TouchDirection Direction, int Time, int Interval)
		: TouchEvent(Emu)
		, mIcon(Icon)
		, mEventM(Event)
		, mType(EMouse)
		, mTime(Time)
		, mDirection(Direction)
		, mInterval(Interval)
		, mPressed(false)
	{
		mTimer.Start();
	};
	bool Update(TouchPoint* Point);
};

class TopEvent : public TouchEvent
{
private:
	IconWindow& mIcon;
	int         mTime;

public:
	TopEvent(InputEmulation& Emu, IconWindow& Icon, int Time)
		: TouchEvent(Emu)
		, mIcon(Icon)
		, mTime(Time)
	{};
	bool Update(TouchPoint* Point);
};

class MouseControlEvent : public TouchEvent
{
private:
	IconWindow&   mIcon;
	Timer         mTimer;
	bool          mInMove;
	bool          mClickValid;
	float         mSpeed;
	Point         mCursor;
	Point         mCursorBak;
	int           mTouchId;

public:
	MouseControlEvent(InputEmulation& Emu, IconWindow& Icon, float Speed);
	~MouseControlEvent();
	bool Update(TouchPoint* Point);

};

#endif //TOUCHEVENT_H