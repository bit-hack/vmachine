//	VMachine
//	YM3812 channel class
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

class Ym3812Operator;

class Ym3812Channel : private boost::noncopyable
{
public:

	//Constructor/Destructor
	Ym3812Channel(double clocksPerSample_);
	~Ym3812Channel();

	//Copy constructor/copy assignment operator handled by boost::noncopyable

	//Generate a sample
	SignedDword GenerateSample(void);

	//Write to a register
	void WriteMultipleRegister(Byte op, Byte data);
	void WriteTotalLevelRegister(Byte op, Byte data);
	void WriteARDRRegister(Byte op, Byte data);
	void WriteSLRRRegister(Byte op, Byte data);
	void WriteFNumberLowRegister(Byte data);
	void WriteFNumberHighRegister(Byte data);
	void WriteAlgorithmRegister(Byte data);
	void WriteWaveformRegister(Byte op, Byte data);

private:

	//Algorithm
	Byte algorithm;
	double feedback;

	//Feedback data
	double feedbackData;

	//Operators
	boost::shared_ptr<Ym3812Operator> operators[2];
};
