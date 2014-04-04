#ifndef TOUCHPOINT_H_
#define TOUCHPOINT_H_

#include "Timer.h"
#include <gdiplus.h>
using namespace Gdiplus;

enum TouchType     // Down -> Move -> Up
{
	ETouchDown = 0,
	ETouchMove = 1,
	ETouchUp   = 2
};

enum TouchDirection
{
	EDirectionUp         = 0,
	EDirectionDown       = 1,
	EDirectionLeft       = 2,
	EDirectionRight      = 3,
	EDirectionUpLeft     = 4,
	EDirectionUpRight    = 5,
	EDirectionDownLeft   = 6,
	EDirectionDownRight  = 7
};

class TouchPoint
{
public:
	TouchPoint(TouchType Type, Point Pos);
	TouchDirection   Direction;
	TouchType        Status;
	Timer            ExistTime;
	int              Angle;
	Point            PosCurrent;;
	Point            PosDown;
	Point            Move;
	static const int PointSize = 80;
	
	void             Update(TouchType Type, Point Pos);
	static int       CalLength(Point Vector);
private:
	Point            PosPast;
	void             CalAngle();
	void             CalDirection();
};


#endif //TOUCHPOINT_H_