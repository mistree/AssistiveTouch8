#include "TouchPoint.h"
#include <math.h>

TouchPoint::TouchPoint(TouchType Type, Point Pos)
: PosCurrent(Pos),
  PosPast(Pos),
  PosDown(Pos),
  Status(Type),
  Angle(-1)
{

};

void TouchPoint::Update(TouchType Type, Point Pos)
{
	if (Type > Status) Status = Type;
	PosPast = PosCurrent;
	PosCurrent = Pos;
	Move = PosCurrent - PosPast;
	CalAngle();
	CalDirection();
};

void TouchPoint::CalAngle()
{
	const double Epsilon = 1.0e-6;
	const double NyPi = acos(-1.0);
	double X1 = Move.X;
	double Y1 = Move.Y;
	double X2 = 100;
	double Y2 = 0;	
	double DotProduct = X1 * X2 + Y1 * Y2;
	double Distance = sqrt(X1 * X1 + Y1 * Y1);
	
	X1 /= Distance;
	Y1 /= Distance;
	Distance = sqrt(X2 * X2 + Y2 * Y2);
	X2 /= Distance;
	Y2 /= Distance;

	if (fabs(DotProduct - 1.0) <= Epsilon)
		Angle = 0.0 * 180.0 / NyPi;
	else if (fabs(DotProduct + 1.0) <= Epsilon)
		Angle = NyPi * 180.0 / NyPi;
	else
		Angle = ((X1 * Y2 - X2 * Y1) < 0) ? (2 * NyPi - acos(DotProduct)) * 180.0 / NyPi : acos(DotProduct) * 180.0 / NyPi;
};

void TouchPoint::CalDirection()
{
	double Degree = Angle + 22.5; // divided into 8 regions
	int Region = Degree / 45;
	switch (Region)
	{
	case 0:
		Direction = EDirectionRight;
	case 1:
		Direction = EDirectionUpRight;
	case 2:
		Direction = EDirectionUp;
	case 3:
		Direction = EDirectionUpLeft;
	case 4:
		Direction = EDirectionLeft;
	case 5:
		Direction = EDirectionDownLeft;
	case 6:
		Direction = EDirectionDown;
	case 7:
		Direction = EDirectionDownRight;
	}
};

int  TouchPoint::CalLength(Point Vector)
{
	double Square = Vector.X * Vector.X + Vector.Y * Vector.Y;
	return sqrt(Square);
};
