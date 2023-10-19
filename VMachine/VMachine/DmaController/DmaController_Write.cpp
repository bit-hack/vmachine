//	VMachine
//	Write memory via the DMA controller
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"
#include "DmaController.h"

void DmaController::WriteByte(Byte channel, Byte data)
{
	assert(channel < 4);

	//TODO: Masking, TC etc
	--currentCountRegisters[channel];
	vmachine.WriteBytePhysical(	(static_cast<Dword>(pageRegisters[channel]) << 16) |
								currentAddressRegisters[channel]++, data);

	if(autoInitEnabled[channel] && currentCountRegisters[channel] == 0xffff)
	{
		currentAddressRegisters[channel] = baseAddressRegisters[channel];
		currentCountRegisters[channel] = baseCountRegisters[channel];
	}
}

void DmaController::WriteWord(Byte channel, Word data)
{
	assert(channel >= 4 && channel < 8);

	--currentCountRegisters[channel];
	vmachine.WriteWordPhysical(	(static_cast<Dword>(pageRegisters[channel]) << 16) |
								(currentAddressRegisters[channel]++) << 1, data);

	if(autoInitEnabled[channel] && currentCountRegisters[channel] == 0xffff)
	{
		currentAddressRegisters[channel] = baseAddressRegisters[channel];
		currentCountRegisters[channel] = baseCountRegisters[channel];
	}
}