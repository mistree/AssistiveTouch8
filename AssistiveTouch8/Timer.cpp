#include "Timer.h"

Timer::Timer()
{
   Start();
};

void Timer::Start()
{
   StartTime = GetTickCount();
};

int Timer::Elasped()
{
	return GetTickCount() - StartTime;
};