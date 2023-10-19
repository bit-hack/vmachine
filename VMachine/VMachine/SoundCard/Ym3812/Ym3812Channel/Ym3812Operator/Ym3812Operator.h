//	VMachine
//	YM3812 operator class
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

class Ym3812Operator : private boost::noncopyable
{
public:

	//Constructor/Destructor
	Ym3812Operator(double clocksPerSample_);
	~Ym3812Operator();

	//Copy constructor/copy assignment operator handled by boost::noncopyable

	//Generate a sample
	double GenerateSample(double phaseModulation);

	//Write to a register
	void WriteMultipleRegister(Byte data);
	void WriteTotalLevelRegister(Byte data);
	void WriteARDRRegister(Byte data);
	void WriteSLRRRegister(Byte data);
	void WriteFNumberLowRegister(Byte data);
	void WriteFNumberHighRegister(Byte data);
	void WriteWaveformRegister(Byte data);

private:

	//The number of clock ticks per sample
	double clocksPerSample;

	//Is the key on?
	bool keyOn;

	//The current stage of the envelope
	enum EnvelopeStage
	{
		ENVELOPE_STAGE_QUIET,
		ENVELOPE_STAGE_ATTACK,
		ENVELOPE_STAGE_DECAY,
		ENVELOPE_STAGE_SUSTAIN,
		ENVELOPE_STAGE_RELEASE
	};

	EnvelopeStage envelopeStage;

	//Envelope generator parameters
	double totalLevel;
	double sustainLevel;

	double envelopeCounter;
	double attackIncrement, decayIncrement, releaseIncrement;

	//Block and F-Number
	Byte block;
	Word fNumber;
	double multiple;

	//Waveform
	Byte waveform;

	//Counters
	void CalculatePhaseIncrement(void);

	double phaseCounter;
	double phaseIncrement;

	//Lookup tables
	void InitialiseLookupTables(void);

	static bool tablesInitialised;
	static const Dword sinTableEntries = 0x800;
	static const Dword sinTableMask = sinTableEntries - 1;
	static std::vector <double> sinTable;
	static std::vector <bool> sinNegativeTable;
};
