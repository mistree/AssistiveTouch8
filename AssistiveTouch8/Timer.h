#ifndef TIMER_H
#define TIMER_H

#include <Windows.h>

class Timer
{
private:
    int StartTime;
	
public:
    Timer();
	void Start();
	int  Elasped(); // in millisecond
};

#endif