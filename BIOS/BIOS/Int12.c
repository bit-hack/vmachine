//	VMachine BIOS
//	Int 0x12 handler
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void Int12Handler(struct Registers * registers)
{
	//Return the memory size (kilobytes of contiguous memory starting at 0x0000) from the BDA
	registers->r_ax.w = *(Word far *)MK_FP(BDA_SEG, BDA_MEMSIZE);;
}
