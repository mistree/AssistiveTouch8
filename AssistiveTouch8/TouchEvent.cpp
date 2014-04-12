#include "TouchEvent.h"

bool DragEvent::Update(TouchPoint* Point)
{
	if (Point->Status == ETouchUp)
		return false;
	if (Point->Status == ETouchDown)
		mValid = true;

	if (mValid)
	{
		if (TouchPoint::CalLength(Point->PosCurrent - mIcon.mPosition) < PointSize)
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

	if (TouchPoint::CalLength(Point->PosDown - mIcon.mPosition) < PointSize
		&& TouchPoint::CalLength(Point->Move) < PointSize
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

	if (TouchPoint::CalLength(Point->PosDown - mIcon.mPosition) < PointSize
		&& TouchPoint::CalLength(Point->Move) > PointSize
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
	else if (mPressed && mType == EKeyboard)
	{
		mEmu.Send(mEventK, false);
		mPressed = false;
	}
	
	if (TouchPoint::CalLength(Point->PosDown - mIcon.mPosition) < PointSize
		&& TouchPoint::CalLength(Point->Move) > PointSize
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
			mPressed = true;
			return true;
		}
	}
	return false;
}

bool TopEvent::Update(TouchPoint* Point)
{
	if (Point->Status == ETouchUp)
		return false;

	if (TouchPoint::CalLength(Point->Move) < PointSize)
	{
		if (Point->ExistTime.Elasped() > mTime)
			BringWindowToTop(mIcon.mIconHwnd);
		return true;
	}

	return false;
};

MouseControlEvent::MouseControlEvent(InputEmulation& Emu, IconWindow& Icon, float Speed)
: TouchEvent(Emu)
, mIcon(Icon)
, mInMove(false)
, mSpeed(Speed)
{
	mCursor = { GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2 };
};

MouseControlEvent::~MouseControlEvent()
{
};

bool MouseControlEvent::Update(TouchPoint* Point)
{
	if (!mIcon.mShow)
		return true;
	if (mInMove && mTouchId != Point->Id)
		return true;
	switch (Point->Status)
	{
	case ETouchDown:
		if (!mInMove)
		if (TouchPoint::CalLength(Point->PosCurrent - mIcon.mPosition) < PointSize)
		{
			mInMove = true;
			mTouchId = Point->Id;
			mCursorBak = mCursor;
		}
		break;
	case ETouchMove:
		if (mInMove)
		{
			int X = Point->Move.X;
			int Y = Point->Move.Y;
			mCursor.X = ((mCursorBak.X + mSpeed*X) <= GetSystemMetrics(SM_CXSCREEN)
			          && (mCursorBak.X + mSpeed*X) >= 0)
				        ? mCursorBak.X + mSpeed*X
				       : (mCursorBak.X + mSpeed*X) > GetSystemMetrics(SM_CXSCREEN) ? GetSystemMetrics(SM_CXSCREEN) : 0;
			mCursor.Y = ((mCursorBak.Y + mSpeed*Y) <= GetSystemMetrics(SM_CYSCREEN)
				      && (mCursorBak.Y + mSpeed*Y) >= 0)
				        ? mCursorBak.Y + mSpeed*Y
				       : (mCursorBak.Y + mSpeed*Y) > GetSystemMetrics(SM_CYSCREEN) ? GetSystemMetrics(SM_CYSCREEN) : 0;
			
			INPUT Command = { 0 };
			Command.type = INPUT_MOUSE;
			Command.mi.time = 0;
			
			if (mCursor.Y >= GetSystemMetrics(SM_CYSCREEN))
			{
				Command.mi.dx = 65536 / 2;
				Command.mi.dy = 65536 / 2;
				Command.mi.mouseData = -25;
				Command.mi.dwFlags = 0x0800 | MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
				SendInput(1, &Command, sizeof(INPUT));
			}
			else if (mCursor.Y <= 0)
			{
				Command.mi.dx = 65536 / 2;
				Command.mi.dy = 65536 / 2;
				Command.mi.mouseData = 25;
				Command.mi.dwFlags = 0x0800 | MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
				SendInput(1, &Command, sizeof(INPUT));
			}
			Command.mi.dx = (mCursor.X * 65536) / GetSystemMetrics(SM_CXSCREEN);
			Command.mi.dy = (mCursor.Y * 65536) / GetSystemMetrics(SM_CYSCREEN);
			Command.mi.mouseData = 0;
			Command.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
			SendInput(1, &Command, sizeof(INPUT));

			if (TouchPoint::CalLength(Point->PosCurrent - mIcon.mPosition) > PointSize)
				mClickValid = false;
		}
		break;
	case ETouchUp:
		if (mInMove)
		{
			SetCursorPos(mCursor.X, mCursor.Y);
			mInMove = false;
		}
		if (!mClickValid)
		{
			mClickValid = true;
			return true;
		}
		if (Point->ExistTime.Elasped() < 500)
		if (TouchPoint::CalLength(Point->PosDown - mIcon.mPosition) < PointSize
			&& TouchPoint::CalLength(Point->PosCurrent - mIcon.mPosition) < PointSize)
		{
			INPUT Command = { 0 };
			Command.type = INPUT_MOUSE;
			Command.mi.time = 0;
			Command.mi.dx = (mCursor.X * 65536) / GetSystemMetrics(SM_CXSCREEN);
			Command.mi.dy = (mCursor.Y * 65536) / GetSystemMetrics(SM_CYSCREEN);
			Command.mi.mouseData = 0;

			Command.mi.dwFlags = 0x0004 | 0x0002 | MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
			SendInput(1, &Command, sizeof(INPUT));

			if (mTimer.Elasped() < 500)
				SendInput(1, &Command, sizeof(INPUT));

			mTimer.Start();
		}
		if (Point->ExistTime.Elasped() > 1000
		 && Point->ExistTime.Elasped() < 3000)
		if (TouchPoint::CalLength(Point->PosDown - mIcon.mPosition) < PointSize
			&& TouchPoint::CalLength(Point->PosCurrent - mIcon.mPosition) < PointSize)
		{
			INPUT Command = { 0 };
			Command.type = INPUT_MOUSE;
			Command.mi.time = 0;
			Command.mi.dx = (mCursor.X * 65536) / GetSystemMetrics(SM_CXSCREEN);
			Command.mi.dy = (mCursor.Y * 65536) / GetSystemMetrics(SM_CYSCREEN);
			Command.mi.mouseData = 0;

			Command.mi.dwFlags = 0x0010 | 0x0008 | MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
			SendInput(1, &Command, sizeof(INPUT));

			if (mTimer.Elasped() < 500)
				SendInput(1, &Command, sizeof(INPUT));

			mTimer.Start();
		}
		break;
	}
	return true;
};
