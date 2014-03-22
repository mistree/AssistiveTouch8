#ifndef EMULATION_H
#define EMULATION_H

#include <Windows.h>
#include "TypeDef.h"

#define MAXINPUT 3
#define NOTSET 3

namespace AssistiveTouch
{
	class Emulation
	{
	public:
		AssistiveTouch::Gesture PageDown = AssistiveTouch::None;
		AssistiveTouch::Gesture PageUp = AssistiveTouch::None;
		Point Mouse;

		void LoadCommandSet( int NumberOfSet );
		void Send( INPUT* Command );
		void SendContinous( INPUT* Command );
		void Reset();

	//private:
		void LoadKeys( INPUT* Command, char* Name, char* Path );
		int GapTime = 0;
		INPUT UpE[MAXINPUT];
		INPUT DownE[MAXINPUT];
		INPUT RightE[MAXINPUT];
		INPUT LeftE[MAXINPUT];
		INPUT UpLeftE[MAXINPUT];
		INPUT UpRightE[MAXINPUT];
		INPUT DownLeftE[MAXINPUT];
		INPUT DownRightE[MAXINPUT];
		INPUT ClickE[MAXINPUT];

		INPUT UpCE[MAXINPUT];
		INPUT DownCE[MAXINPUT];
		INPUT RightCE[MAXINPUT];
		INPUT LeftCE[MAXINPUT];
		INPUT UpLeftCE[MAXINPUT];
		INPUT UpRightCE[MAXINPUT];
		INPUT DownLeftCE[MAXINPUT];
		INPUT DownRightCE[MAXINPUT];

		bool ContinousDown = false;
		INPUT* LastContinousInput;
	};
};

#endif