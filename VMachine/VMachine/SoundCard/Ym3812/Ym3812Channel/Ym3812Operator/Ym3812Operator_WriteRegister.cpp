//	VMachine
//	Write to a register
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Ym3812Operator.h"

namespace
{
	const double multipleTable[16] =
	{
		0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0,
		8.0, 9.0, 10.0, 10.0, 12.0, 12.0, 15.0, 15.0
	};
}

void Ym3812Operator::WriteMultipleRegister(Byte data)
{
	//TODO: AM, VIB, Envelope type, Rate key scale
	multiple = multipleTable[data & 0x0f];

	CalculatePhaseIncrement();
}

void Ym3812Operator::WriteTotalLevelRegister(Byte data)
{
	//TODO: Level key scale
	totalLevel = -0.75 * (data & 0x3f);
}

void Ym3812Operator::WriteARDRRegister(Byte data)
{
	Byte attackRate = data >> 4;

	attackIncrement = clocksPerSample / (72 * 141280);
	attackIncrement *= (attackRate == 0) ? 0.0 : (1 << (attackRate - 1));
	attackIncrement *= 96.0;

	Byte decayRate = data & 0x0f;

	decayIncrement = -clocksPerSample / (72 * 1956000);
	decayIncrement *= (decayRate == 0) ? 0.0 : (1 << (decayRate - 1));
	decayIncrement *= 96.0;
}

void Ym3812Operator::WriteSLRRRegister(Byte data)
{
	sustainLevel = ((data >> 4) == 0x0f) ? -93.0 : -3.0 * (data >> 4);
	
	Byte releaseRate = data & 0x0f;

	releaseIncrement = -clocksPerSample / (72 * 1956000);
	releaseIncrement *= (releaseRate == 0) ? 0.0 : (1 << (releaseRate - 1));
	releaseIncrement *= 96.0;
}

void Ym3812Operator::WriteFNumberLowRegister(Byte data)
{
	//F-Number LSB
	fNumber &= 0xff00;
	fNumber |= data;

	CalculatePhaseIncrement();
}

void Ym3812Operator::WriteFNumberHighRegister(Byte data)
{
	//Key on
	if((data & 0x20) != 0)
	{
		if(!keyOn)
		{
			keyOn = true;

			phaseCounter = 0.0;

			envelopeStage = ENVELOPE_STAGE_ATTACK;
			envelopeCounter = -96.0;
		}
	}
	else
	{
		if(keyOn)
		{
			keyOn = false;

			envelopeStage = ENVELOPE_STAGE_RELEASE;
		}
	}

	//Block
	block = (data & 0x1c) >> 2;

	//F-Number most sig.
	fNumber &= 0x00ff;
	fNumber |= static_cast<Word>(data & 0x03) << 8;

	CalculatePhaseIncrement();
}

void Ym3812Operator::WriteWaveformRegister(Byte data)
{
	waveform = data & 0x03;
}
