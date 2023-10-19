//	VMachine
//	Structure holding data about a sound buffer
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

struct SoundBufferData
{
	SoundBufferData(const Dword numSamples)
		: numSamples(numSamples), data(numSamples * 2, 0)
	{}

	const Dword numSamples;
	std::vector <SignedDword> data;
};
