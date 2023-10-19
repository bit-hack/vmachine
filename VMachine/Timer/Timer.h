//	VMachine
//	Timer class
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

class Timer
{
public:

	//Constructor/Destructor
	Timer(void);
	~Timer(void);

	//Copy constructor/copy assignment operator handled by boost::noncopyable

	//Get the time in microseconds
	Qword GetTime(void);

	//Pause/Unpause the timer
	void Pause(void);
	void Unpause(void);

private:

	//Start time
	Qword startTime;

	//Pause data
	bool paused;
	Qword pauseTime;

	//High-performance counter frequency
	Qword frequency;
};

//The global Timer object
extern Timer timer;