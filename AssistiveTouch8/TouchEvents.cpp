#include "TouchEvent.h"

bool DragEvent::Update(TouchPoint* Point)
{
	if (Point->Status == ETouchUp)
		return false;
	int Length = TouchPoint::CalLength(Point->PosCurrent - mIcon.mPosition);
	if (TouchPoint::CalLength(Point->PosCurrent - mIcon.mPosition) < TouchPoint::PointSize && Point->ExistTime.Elasped() > mTime)
		mIcon.Move(Point->PosCurrent);

	return true;
};