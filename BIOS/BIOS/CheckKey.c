//	VMachine BIOS
//	Check for keystroke (int 0x16/ah=0x01)
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void CheckForKeystroke(struct Registers * registers)
{
	//Get the information about the keyboard buffer
	Word bufferStart = *(Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_START);
	Word bufferEnd = *(Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_END);
	Word bufferSize = bufferEnd - bufferStart;
	Word bufferReadPointer = *(Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_READ_PTR) - bufferStart;
	Word bufferWritePointer = *(Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_WRITE_PTR) - bufferStart;

	//There is a keystroke available if the buffer read pointer does not equal the write pointer
	if(bufferReadPointer != bufferWritePointer)
	{
		//Place the keystroke data into ax
		registers->r_ax.b.l = *(Word far *)MK_FP(BDA_SEG, bufferStart + bufferReadPointer);
		bufferReadPointer = (bufferReadPointer + 1) % bufferSize;
		registers->r_ax.b.h = *(Word far *)MK_FP(BDA_SEG, bufferStart + bufferReadPointer);
		
		//Indicate "keystroke available"
		registers->r_flags &= ~FLAGS_Z_FLAG;
	}
	else
	{
		//Otherwise, indicate "no keystroke available"
		registers->r_flags |= FLAGS_Z_FLAG;
	}
}
