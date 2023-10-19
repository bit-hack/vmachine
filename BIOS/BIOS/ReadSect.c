//	VMachine BIOS
//	Read sectors (int 0x13/ah=0x02)
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void FDReadSectors(struct Registers * registers)
{
	Word i;
	Dword numBytes, destAddr;
	Byte far * int0eReceived;
	Byte status0;

	//If this command is for a drive other than 0, return failure
	if(registers->r_dx.b.l != 0x00)
	{
		registers->r_flags |= FLAGS_C_FLAG;
		registers->r_ax.b.h = 0x01;

		return;
	}

	//Extract the parameters from the registers
	numBytes = registers->r_ax.b.l * 0x200;
	destAddr = ((Dword)registers->r_es << 4) + registers->r_bx.w;

	//Does this read does span a 64k boundary?
	if(destAddr >> 16 != (destAddr + numBytes) >> 16)
	{
		//If so, return a data boundary error
		registers->r_flags |= FLAGS_C_FLAG;
		registers->r_ax.b.h = 0x09;

		return;
	}

	//Set up the DMA controller (channel 2)
	//Clear the internal flip-flop
	outportb(0x0c, 0x00);

	//Set the starting address
	outportb(0x04, destAddr & 0xff);
	outportb(0x04, (destAddr >> 8) & 0xff);

	//Set the page
	outportb(0x81, (destAddr >> 16) & 0xff);

	//Set the byte count
	outportb(0x05, (numBytes - 1) & 0xff);
	outportb(0x05, ((numBytes - 1) >> 8) & 0xff);

	//Start floppy drive 0's motor
	outportb(0x3f2, 0x1c);

	//Reset the "int 0x0e received" flag
	int0eReceived = MK_FP(EBDA_SEG, EBDA_INT0E_RECVD);
	*int0eReceived = 0x00;

	//Send the command to the floppy controller
	outportb(0x3f5, 0x06);	//Non-multi-track, non-MFM, non-skip-deleted-data, Read data command
	outportb(0x3f5, registers->r_dx.b.h << 2);		//Head number, drive 0
	outportb(0x3f5, registers->r_cx.b.h);			//Cylinder number
	outportb(0x3f5, registers->r_dx.b.h);			//Head number
	outportb(0x3f5, registers->r_cx.b.l & 0x3f);	//Starting sector number
	outportb(0x3f5, 0x02);							//512 Bytes per sector
	outportb(0x3f5, (registers->r_cx.b.l & 0x3f) + registers->r_ax.b.l - 1);//End sector number
	outportb(0x3f5, 0x00);							//Gap 3 length (ignored)
    outportb(0x3f5, 0xff);							//Data length (unused)

	//Await an int 0x0e (IRQ 6)
	while(*int0eReceived == 0x00)
		asm hlt;

	//Read the results from the floppy controller
	status0 = inportb(0x3f5);
	
	for(i = 0; i < 6; ++i)
		inportb(0x3f5);

	//Stop floppy drive 0's motor
	outportb(0x3f2, 0x0c);

	//If the read failed, return failure
	if((status0 & 0x80) != 0)
	{
		registers->r_flags |= FLAGS_C_FLAG;
		registers->r_ax.b.h = 0x01;

		return;
	}

	//Return success
	registers->r_flags &= ~FLAGS_C_FLAG;
	registers->r_ax.b.h = 0x00;
}

void HDReadSectors(struct Registers * registers)
{
	Dword destAddr;
	Byte far * int76Received = MK_FP(EBDA_SEG, EBDA_INT76_RECVD);
	Word sector;
	
	//If this command is for a drive other than 0x80, return failure
	if(registers->r_dx.b.l != 0x80)
	{
		registers->r_flags |= FLAGS_C_FLAG;
		registers->r_ax.b.h = 0x01;

		return;
	}

	//Extract the parameters from the registers
	destAddr = ((Dword)registers->r_es << 4) + registers->r_bx.w;

	//Reset the "int 0x76 received" flag
	*int76Received = 0x00;

	//Send the command to the IDE controller
	outportb(0x1f2, registers->r_ax.b.l);			//Number of sectors
	outportb(0x1f3, registers->r_cx.b.l & 0x3f);	//Starting sector number
	outportb(0x1f4, registers->r_cx.b.h);			//Cylinder low
	outportb(0x1f5, registers->r_cx.b.l >> 6);		//Cylinder High
	outportb(0x1f6, 0xa0 | registers->r_dx.b.h);	//Drive 0, head number
	outportb(0x1f7, 0x20);							//Read sectors

	//Loop through sectors
	for(sector = 0; sector < registers->r_ax.b.l; ++sector)
	{
		Word i;

		//Await an int 0x76 (IRQ 14)
		while(*int76Received == 0x00)
			asm hlt;

		//Reset the "int 0x76 received" flag
		*int76Received = 0x00;

		//Dismiss the interrupt
		inportb(0x1f7);

		//Read the data from the IDE controller
		for(i = 0; i < 0x20; ++i)
		{
			Word far * dest = MK_FP((Word)(destAddr >> 4) + sector * 0x20 + i,
									(Word)destAddr & 0x0f);
			Byte j;

			for(j = 0; j < 8; ++j)
				*dest++ = inport(0x1f0);
		}
	}

	//Return success
	//TODO: Handle failure
	registers->r_flags &= ~FLAGS_C_FLAG;
	registers->r_ax.b.h = 0x00;
}
