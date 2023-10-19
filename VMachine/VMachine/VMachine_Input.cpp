//	VMachine
//	Read from I/O address space
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

Byte VMachine::InputByte(Word address)
{
	//0x0000 - 0x0010: Slave DMA controller (channels 0-3)
	if(address < 0x0010)
		return dmaController->ReadPortByte(0x0000, static_cast<Byte>(address));

	//0x0020 - 0x0021: Master PIC
	if(address == 0x0020)
		return masterPic->ReadPortByte(0);

	if(address == 0x0021)
		return masterPic->ReadPortByte(1);

	//0x0040 - 0x0042: Pit
	if(address >= 0x0040 && address < 0x0043)
		return pit->ReadPortByte(static_cast<Byte>(address - 0x0040));

	//0x0060 & 0x0064: Keyboard controller
	if(address == 0x0060)
		return keyboardController->ReadPortByte(0);

	if(address == 0x0064)
		return keyboardController->ReadPortByte(4);

	//0x0061: Misc functions and speaker control
	//TODO: Proper implementation
	if(address == 0x0061)
		return ((rand() % 2) == 0) ? 0x2c : 0x3c;

	//TODO: 0x0071: CMOS memory data
	if(address == 0x0071)
		return rand();

	//0x0080 - 0x0090: DMA page registers
	if(address >= 0x0080 && address < 0x0090)
	{
		switch(address & 0x000f)
		{
		case 0x1:
			return dmaController->ReadPageRegister(2);
		case 0x2:
			return dmaController->ReadPageRegister(3);
		case 0x3:
			return dmaController->ReadPageRegister(1);
		case 0x7:
			return dmaController->ReadPageRegister(0);

		case 0x9:
			return dmaController->ReadPageRegister(6);
		case 0xa:
			return dmaController->ReadPageRegister(7);
		case 0xb:
			return dmaController->ReadPageRegister(5);
		case 0xf:
			return dmaController->ReadPageRegister(4);
		}
	}

	//0x00a0 - 0x00a1: Slave PIC
	if(address == 0x00a0)
		return slavePic->ReadPortByte(0);

	if(address == 0x00a1)
		return slavePic->ReadPortByte(1);

	//0x00c0 - 0x00d0: Master DMA controller (channels 4-7)
	if(address >= 0x00c0 && address < 0x00d0)
		return dmaController->ReadPortByte(0x00c0, static_cast<Byte>(address - 0x00c0));

	//0x01f0 - 0x01f7: IDE controller
	if(address >= 0x01f0 && address < 0x01f8)
		return ideController->ReadPortByte(0x01f0, static_cast<Byte>(address - 0x01f0));

	//0x0220 - 0x022f: Sound Blaster
	if(address >= 0x0220 && address < 0x022f)
		return soundCard->ReadPortByte(0x0220, static_cast<Byte>(address - 0x0220));

	//0x0388 & 0x0389: Adlib
	if(address == 0x0388 || address == 0x0389)
		return soundCard->ReadPortByte(0x0388, static_cast<Byte>(address - 0x0388));

	//0x03b0 - 0x03df: Video card
	if(address >= 0x03b0 && address < 0x03e0)
		return videoCard->ReadPortByte(address);

	//0x03f4 & 0x03f5: Floppy controller
	if(address == 0x3f4)
		return floppyController->ReadPortByte(4);

	if(address == 0x3f5)
		return floppyController->ReadPortByte(5);

	//0x03f6 & 0x03f7: IDE controller
	if(address == 0x3f6)
		return ideController->ReadPortByte(0x03f0, 6);

	if(address == 0x3f7)
		return ideController->ReadPortByte(0x03f0, 7);

#ifdef DEBUG_OUTPUT
	dbgOut << "Byte read from unrecognised port 0x" << address << std::endl;
#endif

	return 0xff;
}

Word VMachine::InputWord(Word address)
{
	//0x01f0 - 0x01f7: IDE controller
	if(address >= 0x01f0 && address < 0x01f8)
		return ideController->ReadPortWord(0x01f0, static_cast<Byte>(address - 0x01f0));

	//0x03f6 & 0x03f7: IDE controller
	if(address == 0x3f6)
		return ideController->ReadPortWord(0x03f0, 6);

	if(address == 0x3f7)
		return ideController->ReadPortWord(0x03f0, 7);

	//Otherwise, split into 2 byte inputs
	return static_cast<Word>(InputByte(address + 1)) << 8 | InputByte(address);
}

Dword VMachine::InputDword(Word address)
{
	//0x01f0 - 0x01f7: IDE controller
	if(address >= 0x01f0 && address < 0x01f8)
		return ideController->ReadPortDword(0x01f0, static_cast<Byte>(address - 0x01f0));

	//0x03f6 & 0x03f7: IDE controller
	if(address == 0x3f6)
		return ideController->ReadPortDword(0x03f0, 6);

	if(address == 0x3f7)
		return ideController->ReadPortDword(0x03f0, 7);

	//Otherwise, split into 2 word inputs
	return static_cast<Dword>(InputWord(address + 2)) << 16 | InputWord(address);
}
