//	VMachine
//	Read ss:esp from the TSS
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

bool VMachine::ReadSsEspFromTss(Dword privilegeLevel, Word & newSs, Dword & newEsp)
{
	if((registers->r_tr.descriptorFlags & DESCRIPTOR_FLAGS_TYPE_MASK) ==
													DESCRIPTOR_FLAGS_TYPE_TSS_16_BIT_BUSY)
	{
		Dword ssOffset = privilegeLevel * 4 + 4;
		Dword spOffset = ssOffset - 2;

		Word newSp;

		if(	!ReadWordLinear(registers->r_tr.base + ssOffset, newSs, true) ||
			!ReadWordLinear(registers->r_tr.base + spOffset, newSp, true))
		{
			return false;
		}

		newEsp = newSp;

		return true;
	}
	else if((registers->r_tr.descriptorFlags & DESCRIPTOR_FLAGS_TYPE_MASK) ==
													DESCRIPTOR_FLAGS_TYPE_TSS_32_BIT_BUSY)
	{
		Dword ssOffset = privilegeLevel * 8 + 8;
		Dword espOffset = ssOffset - 4;

		if(	!ReadWordLinear(registers->r_tr.base + ssOffset, newSs, true) ||
			!ReadDwordLinear(registers->r_tr.base + espOffset, newEsp, true))
		{
			return false;
		}

		return true;
	}
	else
		throw Ex("CPU Error: Unexpected task register type in ReadSsEspFromTss");
}
