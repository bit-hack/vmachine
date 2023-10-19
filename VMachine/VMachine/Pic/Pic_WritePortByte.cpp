//	VMachine
//	Write programmable interrupt controller ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"
#include "Pic.h"

void Pic::WritePortByte(Byte offset, Byte data)
{
	assert(offset == 0 || offset == 1);

	//If the offset is zero, this is ICW1, OCW2 or OCW3
	if(offset == 0)
	{
		//ICW1: ---1X-XX
		//          | |`- ICW4 required
		//          | `-- Cascaded PICs exist (1)
		//          `---- Level triggered
		if((data & 0x10) != 0)
		{
#ifdef DEBUG_OUTPUT
			if((data & 0xf6) != 0x12)
			{
				dbgOut << "Unusual byte 0x" << static_cast<Dword>(data) << " written to ";
				dbgOut << (isMaster ? "master" : "slave") << " PIC ICW1" << std::endl;
			}
#endif

			levelTriggered = (data & 0x08) != 0;
			icw4Required = (data & 0x01) != 0;

			nextIcw = 2;
		}

		//OCW2: XXX00XXX
		//      \_/  ```- Interrupt level
		//       `------- Command
		if((data & 0x18) == 0x00)
		{
			Byte command = data >> 5;
			Byte level = data & 0x07;

			switch(command)
			{
			//1: Non-specific EOI
			case 1:
				NonSpecificEOI();
				break;

			//3: Specific EOI
			case 3:
				SpecificEOI(level);
				break;

#ifdef DEBUG_OUTPUT
			default:
				//TODO: Other commands
				dbgOut << "Unsupported command " << static_cast<Dword>(command);
				dbgOut << " written to " << (isMaster ? "master" : "slave");
				dbgOut << " PIC OCW2" << std::endl;
#endif
			}
		}

		//OCW3: 0XX01XXX
		//       \/  |``- Read register command
		//       |   `--- Poll command
		//       `------- Special mask mode
		if((data & 0x18) == 0x08)
		{
			//Handle read register command
			if((data & 0x03) == 0x02)
				readIsr = false;

			if((data & 0x03) == 0x03)
				readIsr = true;

			//Polling and special mask mode unsupported
#ifdef DEBUG_OUTPUT
			if((data & 0x64) != 0x00)
			{
				dbgOut << "Unsupported command " << static_cast<Dword>(data);
				dbgOut << " written to " << (isMaster ? "master" : "slave");
				dbgOut << " PIC OCW3" << std::endl;
			}
#endif
		}
	}
	else
	{
		//Offset 1, this is ICW2, 3 or 4 or OCW1

		//ICW2: XXXXX000
		//      \___/
		//        `------ New vector offset
		if(nextIcw == 2)
		{
			vectorOffset = data & 0xf8;
			nextIcw = 3;
		}

		//ICW3: Master-slave relationship - ignored
		else if(nextIcw == 3)
			nextIcw = icw4Required ? 4 : 1;

		//ICW4: Special fully nested mode, buffered mode, auto EOI, MCS mode - ignored
		else if(nextIcw == 4)
			nextIcw = 1;

		//OCW1 - Set IMR
		else
		{
			imr = data;

			//If the irr has an unmasked request of higher priority than any remaining in the isr,
			//raise the interrupt line
			if((irr & ~imr) != 0)
			{
				if(isr == 0 || GetLowestSetBit((irr & ~imr)) < GetLowestSetBit(isr))
				{
					if(isMaster)
						vmachine.RaiseINT();
					else
						otherPic->RaiseIRQ(2);
				}
			}
		}
	}
}
