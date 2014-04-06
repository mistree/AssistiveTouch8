#include "TouchEvent.h"

bool DragEvent::Update(TouchPoint* Point)
{
	if (Point->Status == ETouchUp)
		return false;
	if (Point->Status == ETouchDown)
		mValid = true;

	if (mValid)
	{
		if (TouchPoint::CalLength(Point->PosCurrent - mIcon.mPosition) < TouchPoint::PointSize)
		{
			if (Point->ExistTime.Elasped() > mTime)
				mIcon.Move(Point->PosCurrent);
			return true;
		}
		else
			mValid = false;
	}
	
	return false;
};

bool ClickEvent::Update(TouchPoint* Point)
{
	if (Point->Status != ETouchUp)
		return false;

	if (TouchPoint::CalLength(Point->PosDown - mIcon.mPosition) < TouchPoint::PointSize
		&& TouchPoint::CalLength(Point->Move) < TouchPoint::PointSize
		&& Point->ExistTime.Elasped() < mTime)
	{
		(mType == EKeyboard) ? mEmu.Send(mEventK) : mEmu.Send(mEventM, Point->PosCurrent);
		return true;
	}
	return false;
};

bool SlideEvent::Update(TouchPoint* Point)
{
	if (Point->Direction != mDirection
	||  Point->Status != ETouchUp)
		return false;

	if (TouchPoint::CalLength(Point->PosDown - mIcon.mPosition) < TouchPoint::PointSize
		&& TouchPoint::CalLength(Point->Move) > TouchPoint::PointSize
		&& Point->ExistTime.Elasped() < mTime)
	{
		(mType == EKeyboard) ? mEmu.Send(mEventK) : mEmu.Send(mEventM, Point->PosCurrent);
		return true;
	}
	return false;
};

bool SlideContinousEvent::Update(TouchPoint* Point)
{
	if (Point->Direction != mDirection)
		return false;
	
	if (TouchPoint::CalLength(Point->PosDown - mIcon.mPosition) < TouchPoint::PointSize
		&& TouchPoint::CalLength(Point->Move) > TouchPoint::PointSize
		&& Point->ExistTime.Elasped() > mTime)
	{
		if (Point->Status == ETouchUp)
		{
			mEmu.Send(mEventK, false);
		}
		else if (mTimer.Elasped()>mInterval)
		{
			mTimer.Start();
			(mType == EKeyboard) ? mEmu.Send(mEventK,true) : mEmu.Send(mEventM, Point->PosCurrent);
			return true;
		}
	}
	return false;
}