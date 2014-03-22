#include "Touch.h"

using namespace AssistiveTouch;
#include "Emulation.h"

Point Touch::MoveVector()
{
	Point Vector = { MoveP.PosX - DownP.PosX, MoveP.PosY - DownP.PosY };
	return Vector;
};

Point Touch::UpVector()
{
	Point Vector = { UpP.PosX - DownP.PosX, UpP.PosY - DownP.PosY };
	return Vector;
};

void Touch::Update( Event Type, Point Position )
{
   switch(Type)
   {
    case TouchDown:
	  DownP = Position;
	  DownTime.Start();
	  MoveTime.Start();
	return;
	case TouchUp:
	  UpP = Position;
	  g = GetGesture( Type );
	break;
	case TouchMove:
	  MoveP = Position;
	  g = GetGesture( Type );
	break;
   }
   if( LengthToIcon(DownP) < IconRegion )
      Run();
};

void Touch::Run()
{
	if (g == None)
		return;
	else if (g == Emu->PageDown || g == Emu->PageUp)
	{
		TurnPage( g );
		return;
	}
	Emu->Mouse = MoveP;
   switch( g )
   {
    case Up:
		Emu->Send(Emu->UpE);
	break;
	case Down:
		Emu->Send(Emu->DownE);
	break;
	case Right:
		Emu->Send(Emu->RightE);
	break;
	case Left:
		Emu->Send(Emu->LeftE);
	break;
	case UpLeft:
		Emu->Send(Emu->UpLeftE);
	break;
	case UpRight:
		Emu->Send(Emu->UpRightE);
	break;
	case DownLeft:
		Emu->Send(Emu->DownLeftE);
	break;
	case DownRight:
		Emu->Send(Emu->DownRightE);
	break;
	case Click:
		Emu->Send(Emu->ClickE);
	break;
	case UpC:
		Emu->SendContinous(Emu->UpCE);
	break;
	case DownC:
		Emu->SendContinous(Emu->DownCE);
	break;
	case RightC:
		Emu->SendContinous(Emu->RightCE);
	break;
	case LeftC:
		Emu->SendContinous(Emu->LeftCE);
	break;
	case UpLeftC:
		Emu->SendContinous(Emu->UpLeftCE);
	break;
	case UpRightC:
		Emu->SendContinous(Emu->UpRightCE);
	break;
	case DownLeftC:
		Emu->SendContinous(Emu->DownLeftCE);
	break;
	case DownRightC:
		Emu->SendContinous(Emu->DownRightCE);
	break;
	default:
		Emu->Reset();
	break;
   }
};

Gesture Touch::GetGesture( Event Type )
{
    switch(Type)
	{
	  case TouchUp:
		InMove = false;
	    InSlideOperation = false;
	    if( LengthToIcon(DownP) < IconRegion )
		{
		  if( LengthToIcon(UpP) > SlideRegion && DownTime.Elasped() < ClickTime )
			return GetRegion(UpVector());
		  if( LengthToIcon(UpP) < SlideRegion && DownTime.Elasped() < ClickTime )
		    return Click;
		}
	  break;
	  case TouchMove:
	    if( LengthToIcon(DownP) < IconRegion || InMove)
		{
			if (LengthToIcon(MoveP) > SlideRegion && MoveTime.Elasped() > SlideTime )
			{
				MoveTime.Start();
				InSlideOperation = true; 
				return (Gesture)(GetRegion( MoveVector() ) + 8);
			}
			else if (LengthToIcon(MoveP) < SlideRegion && DownTime.Elasped() > RelocateTime && !InSlideOperation)
			{
				InMove = true;
				MoveIcon(IconP = MoveP);
			}
		}
	  break;
	}
	return None;
}

Gesture Touch::GetRegion( Point Vector )
{
	double Angle = GetAngle(Vector) + 22.5; // divided into 8 regions
	int Region = Angle / 45;
	switch( Region )
	{
	   case 0:
	   return Right;
	   case 1:
	   return UpRight;
	   case 2:
	   return Up;
	   case 3:
	   return UpLeft;
	   case 4:
	   return Left;
	   case 5:
	   return DownLeft;
	   case 6:
	   return Down;
	   case 7:
	   return DownRight;
	}
}

int Touch::LengthToIcon( Point Position )
{
  return sqrt( pow( Position.PosX - IconP.PosX, 2 ) + pow( Position.PosY - IconP.PosY, 2 ) );
};

double Touch::GetAngle(Point A)
{
	double x2 = 100;
	double y2 = 0;
	double x1 = A.PosX;
	double y1 = A.PosY;
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
	return degree;
}