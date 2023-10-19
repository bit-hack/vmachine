//	VMachine BIOS
//	Int 0x09 (IRQ 1 - keyboard controller) handler
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void Int09Handler(struct Registers * registers)
{
	Byte far * keyboardFlags0;
	Byte far * keyboardFlags1;
	Byte scancode;

	//Get pointers to the keyboard flag bytes
	keyboardFlags0 = MK_FP(BDA_SEG, BDA_KEYBOARD_FLAGS0);
	keyboardFlags1 = MK_FP(BDA_SEG, BDA_KEYBOARD_FLAGS1);

	//Read the scan code from the keyboard controller
	scancode = inportb(0x60);

	switch(scancode)
	{
	//Right shift pressed
	case 0x36:
		*keyboardFlags0 |= BDA_KEYBOARD_FLAGS0_RIGHT_SHIFT_PRESSED;
		break;

	//Right shift released
	case 0xb6:
		*keyboardFlags0 &= ~BDA_KEYBOARD_FLAGS0_RIGHT_SHIFT_PRESSED;
		break;

	//Left shift pressed
	case 0x2a:
		*keyboardFlags0 |= BDA_KEYBOARD_FLAGS0_LEFT_SHIFT_PRESSED;
		break;

	//Left shift released
	case 0xaa:
		*keyboardFlags0 &= ~BDA_KEYBOARD_FLAGS0_LEFT_SHIFT_PRESSED;
		break;

	//Caps lock pressed
	case 0x3a:
		//If not already pressed, toggle the caps lock status
		if((*keyboardFlags1 & BDA_KEYBOARD_FLAGS1_CAPS_LOCK_PRESSED) == 0)
			*keyboardFlags0 ^= BDA_KEYBOARD_FLAGS0_CAPS_LOCK_ACTIVE;

		//Caps lock now pressed
		*keyboardFlags1 |= BDA_KEYBOARD_FLAGS1_CAPS_LOCK_PRESSED;

		break;

	//Caps lock released
	case 0xba:
		*keyboardFlags1 &= ~BDA_KEYBOARD_FLAGS1_CAPS_LOCK_PRESSED;
		break;

	//Standard keys
	default:
		{
			Word tableIndex;
			Byte ascii;
			
			//Get the information about the keyboard buffer
			Word bufferStart = *(Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_START);
			Word bufferEnd = *(Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_END);
			Word bufferSize = bufferEnd - bufferStart;
			Word bufferReadPointer = *(Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_READ_PTR) - bufferStart;
			Word bufferWritePointer = *(Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_WRITE_PTR) - bufferStart;

			//If this is a key release, ignore it
			if((scancode & 0x80) != 0)
				break;

			//If there is not space for 3 bytes in the keyboard buffer, ignore this key
			//TODO: Beep
			if(	bufferWritePointer != bufferReadPointer &&
				((bufferWritePointer + 1) % bufferSize == bufferReadPointer ||
				(bufferWritePointer + 2) % bufferSize == bufferReadPointer))
			{
				break;
			}

			//Convert the scan code into an ASCII character
			tableIndex = ((Word)scancode) << 2;

			if((*keyboardFlags0 & (	BDA_KEYBOARD_FLAGS0_LEFT_SHIFT_PRESSED |
									BDA_KEYBOARD_FLAGS0_RIGHT_SHIFT_PRESSED)) != 0)
			{
				tableIndex |= 0x0001;
			}

			if((*keyboardFlags1 & BDA_KEYBOARD_FLAGS1_CAPS_LOCK_PRESSED) != 0)
				tableIndex |= 0x0002;

            ascii = *(Byte far *)MK_FP(BIOS_SEG, SCANCODE_TO_ASCII_TABLE_OFFSET + tableIndex);

			//Write the ASCII code and the scan code to the keyboard buffer
			*(Byte far *)MK_FP(BDA_SEG, bufferStart + bufferWritePointer) = ascii;
			bufferWritePointer = (bufferWritePointer + 1) % bufferSize;
			*(Byte far *)MK_FP(BDA_SEG, bufferStart + bufferWritePointer) = scancode;
			bufferWritePointer = (bufferWritePointer + 1) % bufferSize;

			//Save the new value of the buffer write pointer
			*(Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_WRITE_PTR) = bufferStart + bufferWritePointer;
		}
	}

	//Send end-of-interrupt to the PIC
	outportb(0x20, 0x20);
}
