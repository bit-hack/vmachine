//	VMachine BIOS
//	Int 0x11 handler
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void Int11Handler(struct Registers * registers)
{
	Word far * equipmentList;

	//Return the equipment list word from the BDA
	equipmentList = MK_FP(BDA_SEG, BDA_EQUIPMENT_LIST);
	registers->r_ax.w = *equipmentList;
}
