//	VMachine
//	Write to I/O address space
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"
#include "Pic/Pic.h"
#include "DmaController/DmaController.h"
#include "Pit/Pit.h"
#include "KeyboardController/KeyboardController.h"
#include "FloppyController/FloppyController.h"
#include "IdeController/IdeController.h"
#include "VideoCard/VideoCard.h"
#include "SoundCard/SoundCard.h"

void VMachine::OutputByte(Word address, Byte data)
{
	//0x0000 - 0x0010: Slave DMA controller (channels 0-3)
	if(address < 0x0010)
	{
		dmaController->WritePortByte(0x0000, static_cast<Byte>(address), data);
		return;
	}

	//0x0020 - 0x0021: Master PIC
	if(address == 0x0020)
	{
		masterPic->WritePortByte(0, data);
		return;
	}

	if(address == 0x0021)
	{
		masterPic->WritePortByte(1, data);
		return;
	}

	//0x0040 - 0x0043: Pit
	if(address >= 0x0040 && address < 0x0044)
	{
		pit->WritePortByte(static_cast<Byte>(address - 0x0040), data);
		return;
	}

	//0x0060 & 0x0064: Keyboard controller
	if(address == 0x0060)
	{
		keyboardController->WritePortByte(0, data);
		return;
	}

	if(address == 0x0064)
	{
		keyboardController->WritePortByte(4, data);
		return;
	}

	//0x0080 - 0x0090: DMA page registers
	if(address >= 0x0080 && address < 0x0090)
	{
		switch(address & 0x000f)
		{
		case 0x1:
			dmaController->WritePageRegister(2, data);
		case 0x2:
			dmaController->WritePageRegister(3, data);
		case 0x3:
			dmaController->WritePageRegister(1, data);
		case 0x7:
			dmaController->WritePageRegister(0, data);

		case 0x9:
			dmaController->WritePageRegister(6, data);
		case 0xa:
			dmaController->WritePageRegister(7, data);
		case 0xb:
			dmaController->WritePageRegister(5, data);
		case 0xf:
			dmaController->WritePageRegister(4, data);
		}

		return;
	}

	//0x00a0 - 0x00a1: Slave PIC
	if(address == 0x00a0)
	{
		slavePic->WritePortByte(0, data);
		return;
	}

	if(address == 0x00a1)
	{
        slavePic->WritePortByte(1, data);
		return;
	}

	//0x00c0 - 0x00d0: Master DMA controller (channels 4-7)
	if(address >= 0x00c0 && address < 0x00d0)
	{
		dmaController->WritePortByte(0x00c0, static_cast<Byte>(address - 0x00c0), data);
		return;
	}

	//0x01f0 - 0x01f7: IDE controller
	if(address >= 0x01f0 && address < 0x01f8)
	{
		ideController->WritePortByte(0x01f0, static_cast<Byte>(address - 0x01f0), data);
		return;
	}

	//0x0220 - 0x022f: Sound Blaster
	if(address >= 0x0220 && address < 0x022f)
	{
		soundCard->WritePortByte(0x0220, static_cast<Byte>(address - 0x0220), data);
		return;
	}

	//0x0388 & 0x0389: Adlib
	if(address == 0x0388 || address == 0x0389)
	{
		soundCard->WritePortByte(0x0388, static_cast<Byte>(address - 0x0388), data);
		return;
	}

	//0x03b0 - 0x03df: Video card
	if(address >= 0x03b0 && address < 0x03e0)
	{
		videoCard->WritePortByte(address, data);
		return;
	}

	//0x03f2 & 0x03f5: Floppy controller
	if(address == 0x3f2)
	{
		floppyController->WritePortByte(2, data);
		return;
	}

	if(address == 0x3f5)
	{
		floppyController->WritePortByte(5, data);
		return;
	}
	
	//0x03f6 & 0x03f7: IDE controller
	if(address == 0x3f6)
	{
		ideController->WritePortByte(0x03f0, 6, data);
		return;
	}

	if(address == 0x3f7)
	{
		ideController->WritePortByte(0x03f0, 7, data);
		return;
	}

#ifdef DEBUG_OUTPUT
	dbgOut << "Byte 0x" << static_cast<Dword>(data) << " written to unrecognised port 0x";
	dbgOut << address << std::endl;
#endif
}

void VMachine::OutputWord(Word address, Word data)
{
	//0x01f0 - 0x01f7: IDE controller
	if(address >= 0x01f0 && address < 0x01f8)
	{
		ideController->WritePortWord(0x01f0, static_cast<Byte>(address - 0x01f0), data);
		return;
	}

	//0x03f6 & 0x03f7: IDE controller
	if(address == 0x3f6)
	{
		ideController->WritePortWord(0x03f0, 6, data);
		return;
	}

	if(address == 0x3f7)
	{
		ideController->WritePortWord(0x03f0, 7, data);
		return;
	}

	//Otherwise, perform 2 byte outputs
	OutputByte(address, static_cast<Byte>(data));
	OutputByte(address + 1, static_cast<Byte>(data >> 8));
}

void VMachine::OutputDword(Word address, Dword data)
{
	//0x01f0 - 0x01f7: IDE controller
	if(address >= 0x01f0 && address < 0x01f8)
	{
		ideController->WritePortDword(0x01f0, static_cast<Byte>(address - 0x01f0), data);
		return;
	}

	//0x03f6 & 0x03f7: IDE controller
	if(address == 0x3f6)
	{
		ideController->WritePortDword(0x03f0, 6, data);
		return;
	}

	if(address == 0x3f7)
	{
		ideController->WritePortDword(0x03f0, 7, data);
		return;
	}

	//Otherwise, perform 2 word outputs
	OutputWord(address, static_cast<Word>(data));
	OutputWord(address + 2, static_cast<Word>(data >> 16));
}
