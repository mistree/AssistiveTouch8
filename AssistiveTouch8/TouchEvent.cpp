#include "TouchEvent.h"

void DragEvent::Update(TouchPoint* Point)
{
	if (Point->Status == ETouchUp)
		return;

	if (TouchPoint::CalLength(Point->PosCurrent - mIcon.mPosition) < TouchPoint::PointSize && Point->ExistTime.Elasped() > mTime)
		mIcon.Move(Point->PosCurrent);
};

void ClickEvent::Update(TouchPoint* Point)
{
	if (Point->Status != ETouchUp)
		return;

	if (TouchPoint::CalLength(Point->PosCurrent - mIcon.mPosition) < TouchPoint::PointSize
		&& TouchPoint::CalLength(Point->PosCurrent - Point->PosDown) < TouchPoint::PointSize
		&& Point->ExistTime.Elasped() < mTime)
		(mType == EKeyboard) ? mEmu.Send(mEventK) : mEmu.Send(mEventM,Point->PosCurrent);
};