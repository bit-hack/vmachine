//	VMachine
//	Generate a sample
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Ym3812Operator.h"

double Ym3812Operator::GenerateSample(double phaseModulation)
{
	if(envelopeStage == ENVELOPE_STAGE_QUIET)
		return 0.0;

	//Update the phase generator
	phaseCounter += phaseIncrement;

	//Update the envelope generator
	switch(envelopeStage)
	{
	case ENVELOPE_STAGE_ATTACK:
		envelopeCounter += attackIncrement;
		if(envelopeCounter >= 0.0)
		{
			envelopeStage = ENVELOPE_STAGE_DECAY;
			envelopeCounter = 0.0;
		}
		break;

	case ENVELOPE_STAGE_DECAY:
		envelopeCounter += decayIncrement;
		if(envelopeCounter <= sustainLevel)
		{
			envelopeStage = ENVELOPE_STAGE_SUSTAIN;
			envelopeCounter = sustainLevel;
		}
		break;

	case ENVELOPE_STAGE_SUSTAIN:
		break;

	case ENVELOPE_STAGE_RELEASE:
		envelopeCounter += releaseIncrement;
		if(envelopeCounter <= -96.0)
		{
			envelopeStage = ENVELOPE_STAGE_QUIET;
			envelopeCounter = -96.0;
		}
		break;
	}

	Dword sinTableIndex = static_cast<Dword>(sinTableEntries * (phaseCounter + phaseModulation)) & sinTableMask;
	sinTableIndex += waveform * sinTableEntries;

	double sinDB = sinTable[sinTableIndex];
	bool sinNegative = sinNegativeTable[sinTableIndex];

	double envelopeDB = (envelopeStage == ENVELOPE_STAGE_ATTACK)
		? -std::pow(-envelopeCounter / 96.0, 2) * 96.0
		: envelopeCounter;

	double dB = sinDB + totalLevel + envelopeDB;

	return (sinNegative ? -1.0 : 1.0) * pow(10.0, dB / 20.0);
}
