//	VMachine BIOS
//	Copy extended memory (int 0x15/ah=0x87)
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void CopyLoop(void);

//Wait for the keyboard controller's input buffer to be empty
static void WaitForKeyboardWrite(void)
{
	while((inportb(0x64) & 0x02) != 0x00)
		;
}

//Wait for the keyboard controller's output buffer to contain a byte
static void WaitForKeyboardRead(void)
{
	while((inportb(0x64) & 0x01) == 0x00)
		;
}

//Set the A20 line
static BOOL SetA20Line(BOOL enable)
{
	Byte outputPort;

	WaitForKeyboardWrite();
	outportb(0x64, 0xd0);
	WaitForKeyboardRead();
	outputPort = inportb(0x60);

	WaitForKeyboardWrite();
	outportb(0x64, 0xd1);
	WaitForKeyboardWrite();

	if(enable)
		outportb(0x60, (outputPort | 0x02));
	else
		outportb(0x60, (outputPort & ~0x02));

	WaitForKeyboardWrite();

	return (outputPort & 0x02) != 0;
}

void CopyMemory(struct Registers * registers)
{
	Word far * gdt = MK_FP(registers->r_es, registers->r_si);
	Dword gdtAddress = ((Dword)registers->r_es << 4) + registers->r_si;
	BOOL a20Enabled;
	Word r_cx, r_si, r_es;

	//Disable interrupts
	asm cli;

	//Enable the A20 line
	a20Enabled = SetA20Line(TRUE);

	//Write the GDT limit and base at the beginning of the GDT
	gdt[0] = 0x1f;
	gdt[1] = (Word)gdtAddress;
	gdt[2] = (Word)(gdtAddress >> 16);

	//Write the CS descriptor into offset 8 of the GDT
	gdt[4] = 0xffff;
	gdt[5] = 0x0000;
	gdt[6] = 0x9b0f;
	gdt[7] = 0x0000;
	
	//Perform the copy
	r_cx = registers->r_cx.w;
	r_si = registers->r_si;
	r_es = registers->r_es;

	asm {
		push cx
		push si
		push ds
		push es
		mov cx, r_cx
		mov si, r_si
		mov es, r_es
	}

	CopyLoop();
	
	asm {
		pop es
		pop ds
		pop si
		pop cx
	}

	//Restore the A20 line
	SetA20Line(a20Enabled);

	//Re-enable interrupts
	asm sti;

	//Return success
	registers->r_flags &= ~FLAGS_C_FLAG;
	registers->r_ax.b.h = 0x00;
}
