#ifndef TYPEDEF_H
#define TYPEDEF_H

#define WM_TRAY (WM_USER + 100)  
#define WM_TASKBAR_CREATED RegisterWindowMessage(TEXT("TaskbarCreated"))  
#define APP_NAME    TEXT("AssistiveTouch8")  
#define APP_TIP     TEXT("Assistive Touch Tool")  

namespace AssistiveTouch
{

	struct Point
	{
		int PosX;
		int PosY;
	};

	enum Event
	{
		TouchDown,
		TouchUp,
		TouchMove
	};

	enum Gesture
	{
		Click = 0,
		Up = 1,
		Down = 2,
		Left = 3,
		Right = 4,
		UpLeft = 5,
		UpRight = 6,
		DownLeft = 7,
		DownRight = 8,
		UpC = 9,
		DownC = 10,
		LeftC = 11,
		RightC = 12,
		UpLeftC = 13,
		UpRightC = 14,
		DownLeftC = 15,
		DownRightC = 16,
		None = 17
	};
	typedef void(*PageFunction)(Gesture g);
	typedef void(*MoveFunction)(Point Position);

};

#endif