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

void SlideEvent::Update(TouchPoint* Point)
{
	if (Point->Status == ETouchUp)
		return;

	if (TouchPoint::CalLength(Point->PosDown - mIcon.mPosition) < TouchPoint::PointSize
		&& TouchPoint::CalLength(Point->Move) > TouchPoint::PointSize
		&& Point->Direction == mDirection
		&& Point->ExistTime.Elasped() < mTime)
		(mType == EKeyboard) ? mEmu.Send(mEventK) : mEmu.Send(mEventM, Point->PosCurrent);
};

void SlideContinousEvent::Update(TouchPoint* Point)
{
	if (TouchPoint::CalLength(Point->PosDown - mIcon.mPosition) < TouchPoint::PointSize
		&& TouchPoint::CalLength(Point->Move) > TouchPoint::PointSize
		&& Point->Direction == mDirection
		&& Point->ExistTime.Elasped() > mTime)
	{
		if (Point->Status == ETouchUp)
		{
			mEmu.Send(mEventK, true);
		}
		else
		{
			(mType == EKeyboard) ? mEmu.Send(mEventK,true) : mEmu.Send(mEventM, Point->PosCurrent);
		}
	}
}