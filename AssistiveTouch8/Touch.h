#ifndef TOUCH_H
#define TOUCH_H

#include <cmath>
#include "TypeDef.h"
#include "Timer.h"
#include "Emulation.h"

namespace AssistiveTouch
{
	class Touch
	{
	private:
		Point DownP;
		Point UpP;
		Point MoveP;
		Timer DownTime;
		Timer MoveTime;
		Point MoveVector();
		Point UpVector();
		bool InMove;
		int LengthToIcon(Point Position);
		double GetAngle(Point A);

		Gesture g;
		Gesture GetRegion(Point Vector);
		Gesture GetGesture(Event Type);
		void Run();

	public:
		Emulation* Emu;
		int RelocateTime;
		int ClickTime;
		int SlideTime; // time gap between executing each continuous slide operation
		int SlideRegion;
		int IconRegion;
		bool InSlideOperation;
		Point IconP;
		PageFunction TurnPage;
		MoveFunction MoveIcon;

		void Update(Event Type, Point Position);
	};
};

#endif