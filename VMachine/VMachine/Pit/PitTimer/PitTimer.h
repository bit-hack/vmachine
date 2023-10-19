//	VMachine
//	Class for a single PIT timer
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

class Pic;

class PitTimer : private boost::noncopyable
{
public:

	//Constructor/Destructor
	PitTimer(bool timer0_, Word initialCounter_, Byte mode_, Pic & masterPic_);
	~PitTimer();

	//Copy constructor/copy assignment operator handled by boost::noncopyable

	//Update the timer
	void Update(void);

	//Read from and write to the timer
	Byte ReadByte(void);
	void WriteByte(Byte data);

	//Set the timer's mode
	enum DataMode { DATA_MODE_LSB, DATA_MODE_MSB, DATA_MODE_16BIT };
	void SetMode( DataMode newDataMode, Byte newMode, bool newBcd);

	//Latch the value of the counter
	void LatchCounter(void);

	//Latch the status byte
	void LatchStatus(void);

private:

	//Is this timer 0?
	bool timer0;

	//Master PIC used by this timer
	Pic & masterPic;

	//The actual counter
	Word counter;

	//Output level
	Byte outputLevel;

	//Counter initial value
	Word initialCounter;

	//Data and counting modes
	DataMode dataMode;
	Byte mode;
	bool bcd;

	//In 16-bit mode, is the MSB the next byte to be accessed?
	bool nextByteMsb;

	//Latched data
	Byte numBytesLatched;
	Word latchedData;
};
