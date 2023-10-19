//	VMachine BIOS
//	Get keystroke (int 0x16/ah=0x00)
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void GetKeystroke(struct Registers * registers)
{
	//Get the information about the keyboard buffer
	Word bufferStart = *(Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_START);
	Word bufferEnd = *(Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_END);
	Word bufferSize = bufferEnd - bufferStart;
	Word far * pBufferReadPointer = (Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_READ_PTR);
	Word far * pBufferWritePointer = (Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_WRITE_PTR);
	Word bufferReadPointer;

	//Wait until a keystroke is available
	while(*pBufferReadPointer == *pBufferWritePointer)
		asm hlt;

	bufferReadPointer = *pBufferReadPointer - bufferStart;

	//Place the keystroke data into ax
	registers->r_ax.b.l = *(Word far *)MK_FP(BDA_SEG, bufferStart + bufferReadPointer);
	bufferReadPointer = (bufferReadPointer + 1) % bufferSize;
	registers->r_ax.b.h = *(Word far *)MK_FP(BDA_SEG, bufferStart + bufferReadPointer);
	bufferReadPointer = (bufferReadPointer + 1) % bufferSize;

	//Save the new value of the buffer read pointer
	*pBufferReadPointer = bufferStart + bufferReadPointer;
}
