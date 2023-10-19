//	VMachine
//	PIT timer constructor/destructor
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "PitTimer.h"

//Constructor
PitTimer::PitTimer(bool timer0_, Word initialCounter_, Byte mode_, Pic & masterPic_)
	:	timer0(timer0_), masterPic(masterPic_), counter(initialCounter_), outputLevel(0),
		initialCounter(initialCounter_), dataMode(DATA_MODE_LSB), mode(mode_), bcd(false),
		nextByteMsb(false), numBytesLatched(0), latchedData(0x0000)
{
	
}

//Destructor
PitTimer::~PitTimer()
{
	
}
