//	VMachine
//	YM3812 operator constructor/destructor
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Ym3812Operator.h"

bool Ym3812Operator::tablesInitialised = false;
std::vector <double> Ym3812Operator::sinTable(sinTableEntries * 4);
std::vector <bool> Ym3812Operator::sinNegativeTable(sinTableEntries * 4);

//Constructor
Ym3812Operator::Ym3812Operator(double clocksPerSample_)
	:	clocksPerSample(clocksPerSample_),
		keyOn(false), envelopeStage(ENVELOPE_STAGE_QUIET),
		totalLevel(0), sustainLevel(0),
		envelopeCounter(-96.0), attackIncrement(0), decayIncrement(0), releaseIncrement(0),
		block(0), fNumber(0), multiple(0.0), waveform(0),
		phaseCounter(0.0), phaseIncrement(0.0)
{
	if(!tablesInitialised)
		InitialiseLookupTables();
}

//Destructor
Ym3812Operator::~Ym3812Operator()
{

}
