//	VMachine
//	Generate (mono!) samples
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Ym3812.h"
#include "fmopl/fmopl.h"
#include "Ym3812Channel/Ym3812Channel.h"

#ifdef USE_FMOPL
void Ym3812::GenerateSamples(Dword numSamples, SignedWord * const dataBuffer)
{
	YM3812UpdateOne(opl, dataBuffer, numSamples);
}
#endif

#ifdef USE_YM3812_CHANNEL
void Ym3812::GenerateSamples(Dword numSamples, SignedDword * const dataBuffer)
{
	for(Dword i = 0; i < numSamples; ++i)
	{
		SignedDword data = 0;

		for(Dword channel = 0; channel < 9; ++channel)
			data += channels[channel]->GenerateSample();

		dataBuffer[i] = data;
	}
}
#endif