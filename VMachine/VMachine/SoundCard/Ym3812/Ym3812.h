//	VMachine
//	YM3812 FM synthesizer class
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

class Ym3812Channel;

class Ym3812
{
public:

	//Constructor/Destructor
	Ym3812(Dword clockRate, Dword samplingRate);
	~Ym3812();

	//Copy constructor/copy assignment operator handled by boost::noncopyable

	//Access ports
	Byte ReadPortByte(Byte offset);
	void WritePortByte(Byte offset, Byte data);

	//Update the sound buffer
	void UpdateSoundBuffer(Dword frameNumber, SoundBufferData & soundBuffer);

private:

	//Address register
	Byte addressRegister;

	//Non-timer register writes
	struct RegisterWrite
	{
		Qword time;
		Dword numSamples;
		Byte address, data;
	};

	std::vector <RegisterWrite> registerWrites;

	//Apply a register write
	void ApplyRegisterWrite(Byte address, Byte data);

	//Generate (mono!) samples
#ifdef USE_FMOPL
	void GenerateSamples(Dword numSamples, SignedWord * const dataBuffer);
#endif

#ifdef USE_YM3812_CHANNEL
	void GenerateSamples(Dword numSamples, SignedDword * const dataBuffer);
#endif

	//The time of the last call to UpdateSoundBuffer
	Qword lastUpdateTime;

	//Timer data
	bool timerIrq, timer1Expired, timer2Expired;
	bool timer1Masked, timer2Masked;
	Byte timer1Data, timer2Data;
	Qword timer1ExpiryTime, timer2ExpiryTime;

#ifdef USE_FMOPL
	//The OPL chip
	void * opl;
#endif

#ifdef USE_YM3812_CHANNEL
	//FM channels
	std::shared_ptr<Ym3812Channel> channels[9];
#endif
};
