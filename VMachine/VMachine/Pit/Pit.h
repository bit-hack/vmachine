//	VMachine
//	Programmable interval timer class
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

#include "PitTimer/PitTimer.h"

class VMachine;

class Pit : private boost::noncopyable
{
public:

	//Constructor/Destructor
	Pit(VMachine & vmachine_, Pic & masterPic_);
	~Pit();

	//Copy constructor/copy assignment operator handled by boost::noncopyable

	//Update the PIT
	void Update();

	//Access ports
	Byte ReadPortByte(Byte offset);
	void WritePortByte(Byte offset, Byte data);

private:

	//VMachine this PIT is part of
	VMachine & vmachine;

	//The actual timers
	PitTimer timer0, timer1, timer2;
};
