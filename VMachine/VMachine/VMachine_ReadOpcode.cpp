//	VMachine
//	Read the opcode at a given eip
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

const Dword maxPrefixes = 32;

bool VMachine::ReadOpcode(	Dword r_eip, Dword & opcode, OpcodeProperties & properties,
							PrefixBitset & prefixes, Dword & numBytes)
{
	prefixes = 0;
	bool opcodeFound = false;

	//Loop until a non-prefix is found
	for(numBytes = 0; numBytes < maxPrefixes; ++numBytes)
	{
		//Read a byte of the opcode
		Byte tempByte;

		if(!ReadCodeByte(r_eip + numBytes, tempByte))
			return false;

		opcode = tempByte;

		//If the opcode is 0x0f, read the rest of the opcode and get its properties
		if(opcode == 0x0f)
		{
			opcode <<= 8;

			if(!ReadCodeByte(r_eip + numBytes + 1, tempByte))
				return false;

			opcode |= tempByte;

			properties = prefix0fOpcodeProperties[opcode & 0xff];

			numBytes += 2;
			
			opcodeFound = true;
			break;
		}

		//Otherwise, get the properties of this opcode
		properties = noPrefixOpcodeProperties[opcode & 0xff];

		//If this is not a prefix, we're done
		if((properties & OPCODE_PREFIX) == 0)
		{
			numBytes += 1;
			
			opcodeFound = true;
			break;
		}

		//The latest segment register prefix overrides any earlier ones
		if(	opcode == 0x2e || opcode == 0x3e || opcode == 0x26 ||
			opcode == 0x64 || opcode == 0x65 || opcode == 0x36)
		{
			prefixes &= ~(	PREFIX_CS | PREFIX_DS | PREFIX_ES |
							PREFIX_FS | PREFIX_GS | PREFIX_SS);
		}

		//Ditto for repeat prefixes
		if(opcode == 0xf2 || opcode == 0xf3)
			prefixes &= ~(PREFIX_REP | PREFIX_REPNE);

		if(opcode == 0x2e) prefixes |= PREFIX_CS;
		if(opcode == 0x3e) prefixes |= PREFIX_DS;
		if(opcode == 0x26) prefixes |= PREFIX_ES;
		if(opcode == 0x64) prefixes |= PREFIX_FS;
		if(opcode == 0x65) prefixes |= PREFIX_GS;
		if(opcode == 0x36) prefixes |= PREFIX_SS;

		if(opcode == 0xf3) prefixes |= PREFIX_REP;
		if(opcode == 0xf2) prefixes |= PREFIX_REPNE;

		if(opcode == 0x67) prefixes |= PREFIX_A_SIZE;
		if(opcode == 0x66) prefixes |= PREFIX_O_SIZE;
	}

	//Were the maximum number of prefixes found?
	if(!opcodeFound)
	{
		//TODO: #GP?
		throw Ex("CPU Error: Too many prefixes found");
	}

	//If the opcode has a ModR/M byte, read it and add any necessary fields to the opcode
	//Do not advance r_eip past the ModR/M byte
	if((properties & OPCODE_HAS_MOD_RM) != 0)
	{
		Byte modRM;
		
		if(!ReadCodeByte(r_eip + numBytes, modRM))
			return false; 

		opcode <<= 8;
	
		if((modRM & 0xc0) == 0xc0)
			opcode |= 0xc0;

		if((properties & OPCODE_GROUP) != 0)
		{
			opcode |= modRM & 0x38;
		}

		if((properties & OPCODE_EXTENDED_GROUP) != 0 && (modRM & 0xc0) == 0xc0)
		{
			opcode |= modRM;
		}
	}

	//Add the O32 and A32 flags to the opcode, if necessary
	bool default32 =
		(registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0;

	if(default32 != ((prefixes & PREFIX_A_SIZE) != 0))	//default 32 XOR address size prefix
		opcode |= OPCODE_A32;

	if(default32 != ((prefixes & PREFIX_O_SIZE) != 0))	//default 32 XOR operand size prefix
		opcode |= OPCODE_O32;

	return true;
}