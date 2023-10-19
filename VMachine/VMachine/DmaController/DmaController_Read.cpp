//	VMachine
//	Read memory via the DMA controller
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"
#include "DmaController.h"

Byte DmaController::ReadByte(Byte channel)
{
	assert(channel < 4);

	//TODO: Masking, TC etc
	--currentCountRegisters[channel];
	Byte data = vmachine.ReadBytePhysical(	(static_cast<Dword>(pageRegisters[channel]) << 16) |
											currentAddressRegisters[channel]++);

	if(autoInitEnabled[channel] && currentCountRegisters[channel] == 0xffff)
	{
		currentAddressRegisters[channel] = baseAddressRegisters[channel];
		currentCountRegisters[channel] = baseCountRegisters[channel];
	}

	return data;
}

Word DmaController::ReadWord(Byte channel)
{
	assert(channel >= 4 && channel < 8);

	--currentCountRegisters[channel];
	Word data = vmachine.ReadWordPhysical(	(static_cast<Dword>(pageRegisters[channel]) << 16) |
											(currentAddressRegisters[channel]++) << 1);

	if(autoInitEnabled[channel] && currentCountRegisters[channel] == 0xffff)
	{
		currentAddressRegisters[channel] = baseAddressRegisters[channel];
		currentCountRegisters[channel] = baseCountRegisters[channel];
	}

	return data;
}
