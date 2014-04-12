#include "TouchPoint.h"
#include <math.h>

int PointSize = 60;

TouchPoint::TouchPoint(int PointId, TouchType Type, Point Pos)
: PosCurrent(Pos),
  PosPast(Pos),
  PosDown(Pos),
  Status(Type),
  Angle(-1),
  Id(PointId)
{
	
};

void TouchPoint::Update(TouchType Type, Point Pos)
{
	if (Type >= Status)
	{
		Status = Type;
		PosPast = PosCurrent;
		PosCurrent = Pos;
		Move = PosCurrent - PosDown;
		CalAngle();
		CalDirection();
	}
};

//void TouchPoint::CalAngle()
//{
//	const double Epsilon = 1.0e-6;
//	const double NyPi = acos(-1.0);
//	double X1 = Move.X;
//	double Y1 = Move.Y;
//	double X2 = 100;
//	double Y2 = 0;	
//	double DotProduct = X1 * X2 + Y1 * Y2;
//	double Distance = sqrt(X1 * X1 + Y1 * Y1);
//	
//	X1 /= Distance;
//	Y1 /= Distance;
//	Distance = sqrt(X2 * X2 + Y2 * Y2);
//	X2 /= Distance;
//	Y2 /= Distance;
//
//	if (fabs(DotProduct - 1.0) <= Epsilon)
//		Angle = 0.0 * 180.0 / NyPi;
//	else if (fabs(DotProduct + 1.0) <= Epsilon)
//		Angle = NyPi * 180.0 / NyPi;
//	else
//		Angle = ((X1 * Y2 - X2 * Y1) < 0) ? (2 * NyPi - acos(DotProduct)) * 180.0 / NyPi : acos(DotProduct) * 180.0 / NyPi;
//};

void TouchPoint::CalDirection()
{
	double Degree = Angle + 22.5; // divided into 8 regions
	int Region = Degree / 45;
	switch (Region)
	{
	case 0:
		Direction = EDirectionRight;
		break;
	case 1:
		Direction = EDirectionUpRight;
		break;
	case 2:
		Direction = EDirectionUp;
		break;
	case 3:
		Direction = EDirectionUpLeft;
		break;
	case 4:
		Direction = EDirectionLeft;
		break;
	case 5:
		Direction = EDirectionDownLeft;
		break;
	case 6:
		Direction = EDirectionDown;
		break;
	case 7:
		Direction = EDirectionDownRight;
		break;
	}
};

int  TouchPoint::CalLength(Point Vector)
{
	double Square = Vector.X * Vector.X + Vector.Y * Vector.Y;
	return sqrt(Square);
};



void TouchPoint::CalAngle()
{
	double x2 = 100;
	double y2 = 0;
	double x1 = Move.X;
	double y1 = Move.Y;
	const double epsilon = 1.0e-6;
	const double nyPI = acos(-1.0);
	double dist, dot, degree, angle;

	// normalize  
	dist = sqrt(x1 * x1 + y1 * y1);
	x1 /= dist;
	y1 /= dist;
	dist = sqrt(x2 * x2 + y2 * y2);
	x2 /= dist;
	y2 /= dist;
	// dot product  
	dot = x1 * x2 + y1 * y2;
	if (fabs(dot - 1.0) <= epsilon)
		angle = 0.0;
	else if (fabs(dot + 1.0) <= epsilon)
		angle = nyPI;
	else {
		double cross;

		angle = acos(dot);
		//cross product  
		cross = x1 * y2 - x2 * y1;
		// vector p2 is clockwise from vector p1   
		// with respect to the origin (0.0)  
		if (cross < 0) {
			angle = 2 * nyPI - angle;
		}
	}
	degree = angle *  180.0 / nyPI;
	Angle = degree;
}