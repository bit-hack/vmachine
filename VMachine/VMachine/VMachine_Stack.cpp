//	VMachine
//	Access the stack
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

bool VMachine::PushWord(Word data)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		registers->r_esp -= 2;

		if(!WriteWord(r_ss, registers->r_esp, data))
			return false;
	}
	else
	{
		registers->r_sp -= 2;
		
		if(!WriteWord(r_ss, registers->r_sp, data))
			return false;
	}

	return true;
}

bool VMachine::PushDword(Dword data)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		registers->r_esp -= 4;

		if(!WriteDword(r_ss, registers->r_esp, data))
			return false;
	}
	else
	{
		registers->r_sp -= 4;
		
		if(!WriteDword(r_ss, registers->r_sp, data))
			return false;
	}

	return true;
}

bool VMachine::PopWord(Word & data)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		if(!ReadWord(r_ss, registers->r_esp, data))
			return false;

		registers->r_esp += 2;
	}
	else
	{
		if(!ReadWord(r_ss, registers->r_sp, data))
			return false;

		registers->r_sp += 2;
	}

	return true;
}

bool VMachine::PopDword(Dword & data)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		if(!ReadDword(r_ss, registers->r_esp, data))
			return false;

		registers->r_esp += 4;
	}
	else
	{
		if(!ReadDword(r_ss, registers->r_sp, data))
			return false;

		registers->r_sp += 4;
	}

	return true;
}

bool VMachine::ReadStackWord(Dword offset, Word & data)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		if(!ReadWord(r_ss, registers->r_esp + offset, data))
			return false;
	}
	else
	{
		if(!ReadWord(r_ss, static_cast<Word>(registers->r_sp + offset), data))
			return false;
	}

	return true;
}

bool VMachine::ReadStackDword(Dword offset, Dword & data)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		if(!ReadDword(r_ss, registers->r_esp + offset, data))
			return false;
	}
	else
	{
		if(!ReadDword(r_ss, static_cast<Word>(registers->r_sp + offset), data))
			return false;
	}

	return true;
}

bool VMachine::WriteStackWord(Dword offset, Word data)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		if(!WriteWord(r_ss, registers->r_esp + offset, data))
			return false;
	}
	else
	{
		if(!WriteWord(r_ss, static_cast<Word>(registers->r_sp + offset), data))
			return false;
	}

	return true;
}

bool VMachine::WriteStackDword(Dword offset, Dword data)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		if(!WriteDword(r_ss, registers->r_esp + offset, data))
			return false;
	}
	else
	{
		if(!WriteDword(r_ss, static_cast<Word>(registers->r_sp + offset), data))
			return false;
	}

	return true;
}
