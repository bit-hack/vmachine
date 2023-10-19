//	VMachine
//	Access DMA controller page registers
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "DmaController.h"

Byte DmaController::ReadPageRegister(Byte channel)
{
	assert(channel < 8);

	return pageRegisters[channel];
}

void DmaController::WritePageRegister(Byte channel, Byte data)
{
	assert(channel < 8);

	pageRegisters[channel] = data;
}
