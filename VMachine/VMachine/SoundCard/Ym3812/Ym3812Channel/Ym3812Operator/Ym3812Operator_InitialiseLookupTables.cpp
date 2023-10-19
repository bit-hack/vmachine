//	VMachine
//	Initialise lookup tables
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Ym3812Operator.h"

void Ym3812Operator::InitialiseLookupTables(void)
{
	//Waveform 1 - sin
	for(Dword i = 0; i < sinTableEntries; ++i)
	{
		double sinAngle = std::sin((2.0 * M_PI * i) / sinTableEntries);
		sinNegativeTable[i] = sinAngle < 0.0;

		double dB = 20.0 * std::log10(sinAngle);
		sinTable[i] = dB;
	}

	//Waveform 2 - first half of sin
	for(Dword i = 0; i < sinTableEntries; ++i)
	{
		sinTable[sinTableEntries + i] = (i < (sinTableEntries / 2)) ? sinTable[i] : 0.0;
		sinNegativeTable[sinTableEntries + i] = sinNegativeTable[i];
	}

	//Waveform 3 - abs(sin)
	for(Dword i = 0; i < sinTableEntries; ++i)
	{
		sinTable[sinTableEntries * 2 + i] = sinTable[i];
		sinNegativeTable[sinTableEntries * 2 + i] = false;
	}

	//Waveform 4 - first and third quarters of abs(sin)
	for(Dword i = 0; i < sinTableEntries; ++i)
	{
		sinTable[sinTableEntries * 3 + i] =
			(i < (sinTableEntries / 4) ||
				(i >= (sinTableEntries / 2) && i < (3 * sinTableEntries / 4)))
					? sinTable[i] : 0.0;
		sinNegativeTable[sinTableEntries * 3 + i] = false;
	}

	tablesInitialised = true;
}
