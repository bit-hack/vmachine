//	VMachine
//	Timer functions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Timer.h"

//The global Timer object
Timer timer;

//Constructor
Timer::Timer(void) : paused(false), pauseTime(0)
{
	LARGE_INTEGER temp;

	//Get the high-performance counter frequency
	QueryPerformanceFrequency(&temp);
	frequency = temp.QuadPart;

	//Get the start time
	QueryPerformanceCounter(&temp);
	startTime = temp.QuadPart;
}

//Destructor
Timer::~Timer(void)
{

}

//Get the time in microseconds
Qword Timer::GetTime(void)
{
	if(paused)
		return pauseTime;

	//Get the current time
	LARGE_INTEGER currentTime;
	
	QueryPerformanceCounter(&currentTime);

	//Return the offset from the start time, in microseconds
	return static_cast<Qword>(
			static_cast<double>(1000000 * (currentTime.QuadPart - startTime)) / frequency);
}

//Pause/Unpause the timer
void Timer::Pause(void)
{
	if(paused)
		return;

	pauseTime = GetTime();

	paused = true;
}

void Timer::Unpause(void)
{
	if(!paused)
		return;

	paused = false;

	startTime += GetTime() - pauseTime;

	pauseTime = 0;
}
