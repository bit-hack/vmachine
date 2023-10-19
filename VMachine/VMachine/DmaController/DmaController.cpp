//	VMachine
//	DMA controller constructor/destructor
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "DmaController.h"

//Constructor
DmaController::DmaController(VMachine & vmachine_) : vmachine(vmachine_), highByte(false)
{
	//Loop through channels and zero the registers
	for(int i = 0; i < 8; ++i)
	{
		pageRegisters[i] = 0;
		baseAddressRegisters[i] = 0;
		baseCountRegisters[i] = 0;
		currentAddressRegisters[i] = 0;
		currentCountRegisters[i] = 0;
		autoInitEnabled[i] = 0;
	}
}

//Destructor
DmaController::~DmaController()
{
	
}
