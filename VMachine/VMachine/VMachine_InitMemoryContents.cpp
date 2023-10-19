//	VMachine
//	Initialise the contents of memory
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"
#include "IdeController/IdeController.h"
#include "../Resources/resource.h"

namespace
{
	//Table to convert scan codes into ASCII characters
	//For each scan code, contains the character used with:
	//no modifiers, shift, caps lock, shift and caps lock
	//To be patched into the BIOS at 0xe700-0xe8ff
	const Byte scancodeToAsciiTable[512] =
	{
		0x00,	0x00,	0x00,	0x00,	//Unused
		0x1b,	0x1b,	0x1b,	0x1b,	//Esc
		'1',	'!',	'1',	'!',
		'2',	'@',	'2',	'@',
		'3',	'#',	'3',	'#',
		'4',	'$',	'4',	'$',
        '5',	'%',	'5',	'%',
		'6',	'^',	'6',	'^',
		'7',	'&',	'7',	'&',
		'8',	'*',	'8',	'*',
		'9',	'(',	'9',	'(',
		'0',	')',	'0',	')',
		'-',	'_',	'-',	'_',
		'=',	'+',	'=',	'+',
		0x08,	0x08,	0x08,	0x08,	//Backspace
		0x09,	0x09,	0x09,	0x09,	//Tab
		'q',	'Q',	'Q',	'q',
		'w',	'W',	'W',	'w',
		'e',	'E',	'E',	'e',	
		'r',	'R',	'R',	'r',	
		't',	'T',	'T',	't',	
		'y',	'Y',	'Y',	'y',	
		'u',	'U',	'U',	'u',	
		'i',	'I',	'I',	'i',	
		'o',	'O',	'O',	'o',	
		'p',	'P',	'P',	'p',	
		'[',	'{',	'[',	'{',
		']',	'}',	']',	'}',
		0x0d,	0x0d,	0x0d,	0x0d,	//Enter
		0x00,	0x00,	0x00,	0x00,	//Ctrl
		'a',	'A',	'A',	'a',	
		's',	'S',	'S',	's',	
		'd',	'D',	'D',	'd',
		'f',	'F',	'F',	'f',
		'g',	'G',	'G',	'g',
		'h',	'H',	'H',	'h',
		'j',	'J',	'J',	'j',
		'k',	'K',	'K',	'k',
		'l',	'L',	'L',	'l',
		';',	':',	';',	':',
		'\'',	'"',	'\'',	'"',
		'`',	'~',	'`',	'~',
		0x00,	0x00,	0x00,	0x00,	//Left shift
		'\\',	'|',	'\\',	'|',
		'z',	'Z',	'Z',	'z',
		'x',	'X',	'X',	'x',
		'c',	'C',	'C',	'c',
		'v',	'V',	'V',	'v',
		'b',	'B',	'B',	'b',
		'n',	'N',	'N',	'n',
		'm',	'M',	'M',	'm',
		',',	'<',	',',	'<',
		'.',	'>',	'.',	'>',
		'/',	'?',	'/',	'?',
		0x00,	0x00,	0x00,	0x00,	//Right shift
		0x00,	0x00,	0x00,	0x00,	//Prt Scrn/SysRq
		0x00,	0x00,	0x00,	0x00,	//Alt
		' ',	' ',	' ',	' ',
		0x00,	0x00,	0x00,	0x00,	//Caps Lock
	};
}

void VMachine::InitMemoryContents(void)
{
	//BIOS ROM => 0xf0000
	Byte * biosData;
	Dword biosSize;

	if(!GetResourcePtrAndSize(IDR_BIOS, biosData, biosSize))
		throw Ex("Internal Error: Unable to access resource IDR_BIOS");

	std::copy(biosData, biosData + biosSize, &memory[0xf0000]);

	//Far jump to 0xf000:0x0000 => 0xffff0
	Byte jumpInstr[5] = {0xea, 0x00, 0x00, 0x00, 0xf0};

	std::copy(jumpInstr, jumpInstr + 5, &memory[0xffff0]);

	//Scancode-to-ASCII table => 0xfe700
	memcpy(&memory[0xfe700], scancodeToAsciiTable, 512);

	//Hard disk geometry => 0xfe900
	HardDiskGeometry hardDiskGeometry = ideController->GetHardDiskGeometry();

	*reinterpret_cast<Word *>(&memory[0xfe900]) =
		static_cast<Word>(hardDiskGeometry.numCylinders);
	*reinterpret_cast<Word *>(&memory[0xfe902]) =
		static_cast<Word>(hardDiskGeometry.numHeads);
	*reinterpret_cast<Word *>(&memory[0xfe904]) =
		static_cast<Word>(hardDiskGeometry.numSectors);
	*reinterpret_cast<Dword *>(&memory[0xfe906]) = hardDiskGeometry.numSectorsTotal;

	//Memory size (MB) => 0xfe90a
	*reinterpret_cast<Word *>(&memory[0xfe90a]) =
		static_cast<Word>(registers->memorySize / (1024 * 1024));

	//Hard disk 0 parameter table => 0xfe401
	//TODO: Control byte, landing zone etc
	*reinterpret_cast<Word *>(&memory[0xfe401]) =
		static_cast<Word>(hardDiskGeometry.numCylinders);
	memory[0xfe403] = static_cast<Byte>(hardDiskGeometry.numHeads);
	memory[0xfe40f] = static_cast<Byte>(hardDiskGeometry.numSectors);

	//System configuration table => 0xfe6f5

	//Word at 0xfe6f5: Number of bytes of configuration information following
	*(reinterpret_cast<Word *>(&memory[0xfe6f5])) = 0x0008;

	//3 bytes at 0xfe6f7: Model, submodel, revision
	memory[0xfe6f7] = 0xfc;
	memory[0xfe6f8] = 0x01;
	memory[0xfe6f9] = 0x00;

	//Byte at 0xfe6fa: Feature byte 1
	//0111 0100b
	//|||| |||`-- Not dual bus (both MCA and ISA)
	//|||| ||`--- Not MCA bus instead of ISA bus
	//|||| |`---- EBDA present
	//|||| `----- int 0x15/ah=0x41 not supported
	//|||`------- int 0x15/ah=0x4f called on int 0x09
	//||`-------- RTC installed
	//|`--------- 2nd PIC installed
	//`---------- DMA channel 3 not used by hard disk BIOS
	memory[0xfe6fa] = 0x74;

	//4 bytes at 0xfe6fb: Feature bytes 2-5
	memory[0xfe6fb] = 0x00;
	memory[0xfe6fc] = 0x00;
	memory[0xfe6fd] = 0x00;
	memory[0xfe6fe] = 0x00;

	//Timer ticks count (ticks since midnight) => 0x046c
	SYSTEMTIME currentTime;
	GetLocalTime(&currentTime);
	Dword milliseconds =
		((currentTime.wHour * 60 + currentTime.wMinute) * 60
			+ currentTime.wSecond) * 1000
				+ currentTime.wMilliseconds;
	Dword ticks = static_cast<Dword>(milliseconds * 0.0182);
	*(reinterpret_cast<Dword *>(&memory[0x046c])) = ticks;

	//VGA BIOS ROM => 0xc0000
	Byte * vgaBiosData;
	Dword vgaBiosSize;

	if(!GetResourcePtrAndSize(IDR_VGABIOS, vgaBiosData, vgaBiosSize))
		throw Ex("Internal Error: Unable to access resource IDR_VGABIOS");

	std::copy(vgaBiosData, vgaBiosData + vgaBiosSize, &memory[0xc0000]);
}
