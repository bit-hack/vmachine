//	VMachine
//	Implementations of miscellaneous instructions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../HelperFunctions/HelperFunctions.h"
#include "Instructions.h"

void OP_ENTER_O16(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		Dword r_esp = registers->r_esp;
		Dword r_ebp = registers->r_ebp;

		immByte &= 0x1f;

		WriteWord(registers->r_ss.base + r_esp - 2, r_ebp);
		r_esp -= 2;

		Dword frameTemp = r_esp;

		if(immByte > 0)
		{
			for(Byte i = 1; i < immByte; ++i)
			{
				r_ebp -= 2;
				WriteWord(registers->r_ss.base + r_esp - 2, r_ebp);
				r_esp -= 2;
			}

			WriteWord(registers->r_ss.base + r_esp - 2, frameTemp);
			r_esp -= 2;
		}

		registers->r_ebp = frameTemp;
		registers->r_esp = registers->r_ebp - immWord;
	}
	else
	{
		Word r_sp = registers->r_sp;
		Word r_bp = registers->r_bp;

		immByte &= 0x1f;

		WriteWord(registers->r_ss.base + ((Word)r_sp - 2), r_bp);
		r_sp -= 2;

		Word frameTemp = r_sp;

		if(immByte > 0)
		{
			for(Byte i = 1; i < immByte; ++i)
			{
				r_bp -= 2;
				WriteWord(registers->r_ss.base + ((Word)r_sp - 2), r_bp);
				r_sp -= 2;
			}

			WriteWord(registers->r_ss.base + ((Word)r_sp - 2), frameTemp);
			r_sp -= 2;
		}

		registers->r_bp = frameTemp;
		registers->r_sp = registers->r_bp - immWord;
	}

	asm (".global _OP_ENTER_O16_END\n _OP_ENTER_O16_END:");
}

void OP_ENTER_O32(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		Dword r_esp = registers->r_esp;
		Dword r_ebp = registers->r_ebp;

		immByte &= 0x1f;

		WriteDword(registers->r_ss.base + r_esp - 4, r_ebp);
		r_esp -= 4;

		Dword frameTemp = r_esp;

		if(immByte > 0)
		{
			for(Byte i = 1; i < immByte; ++i)
			{
				r_ebp -= 4;
				WriteDword(registers->r_ss.base + r_esp - 4, r_ebp);
				r_esp -= 4;
			}

			WriteDword(registers->r_ss.base + r_esp - 4, frameTemp);
			r_esp -= 4;
		}

		registers->r_ebp = frameTemp;
		registers->r_esp = registers->r_ebp - immWord;
	}
	else
	{
		Word r_sp = registers->r_sp;
		Word r_bp = registers->r_bp;

		immByte &= 0x1f;

		WriteDword(registers->r_ss.base + ((Word)r_sp - 4), r_bp);
		r_sp -= 4;

		Word frameTemp = r_sp;

		if(immByte > 0)
		{
			for(Byte i = 1; i < immByte; ++i)
			{
				r_bp -= 4;
				WriteWord(registers->r_ss.base + ((Word)r_sp - 4), r_bp);
				r_sp -= 4;
			}

			WriteWord(registers->r_ss.base + ((Word)r_sp - 4), frameTemp);
			r_sp -= 4;
		}

		registers->r_bp = frameTemp;
		registers->r_sp = registers->r_bp - immWord;
	}

	asm (".global _OP_ENTER_O32_END\n _OP_ENTER_O32_END:");
}

void OP_LEAVE_O16(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		Dword r_esp = registers->r_ebp;
		
		registers->r_bp = ReadWord(registers->r_ss.base + r_esp);
		registers->r_esp = r_esp + 2;
	}
	else
	{
		Word r_sp = registers->r_bp;
		
		registers->r_bp = ReadWord(registers->r_ss.base + r_sp);
		registers->r_sp = r_sp + 2;
	}

	asm (".global _OP_LEAVE_O16_END\n _OP_LEAVE_O16_END:");
}

void OP_LEAVE_O32(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		Dword r_esp = registers->r_ebp;
		
		registers->r_ebp = ReadDword(registers->r_ss.base + r_esp);
		registers->r_esp = r_esp + 4;
	}
	else
	{
		Word r_sp = registers->r_bp;
		
		registers->r_ebp = ReadDword(registers->r_ss.base + r_sp);
		registers->r_sp = r_sp + 4;
	}

	asm (".global _OP_LEAVE_O32_END\n _OP_LEAVE_O32_END:");
}

void OP_LEA_Gw_M_A16(void)
{
	Gw = baseReg + indexReg + displacement;

	asm (".global _OP_LEA_Gw_M_A16_END\n _OP_LEA_Gw_M_A16_END:");
}

void OP_LEA_Gd_M_A16(void)
{
	Gd = (baseReg + indexReg + displacement) & 0xffff;

	asm (".global _OP_LEA_Gd_M_A16_END\n _OP_LEA_Gd_M_A16_END:");
}

void OP_LEA_Gw_M_A32(void)
{
	Gw = baseReg + (indexReg << indexShift) + displacement;

	asm (".global _OP_LEA_Gw_M_A32_END\n _OP_LEA_Gw_M_A32_END:");
}

void OP_LEA_Gd_M_A32(void)
{
	Gd = baseReg + (indexReg << indexShift) + displacement;

	asm (".global _OP_LEA_Gd_M_A32_END\n _OP_LEA_Gd_M_A32_END:");
}

void OP_NOP(void)
{
	asm (".global _OP_NOP_END\n _OP_NOP_END:");
}

void OP_XLAT_A16(void)
{
	registers->r_al = ReadByte(segRegBase + registers->r_bx + registers->r_al);

	asm (".global _OP_XLAT_A16_END\n _OP_XLAT_A16_END:");
}

void OP_XLAT_A32(void)
{
	registers->r_al = ReadByte(segRegBase + registers->r_ebx + registers->r_al);

	asm (".global _OP_XLAT_A32_END\n _OP_XLAT_A32_END:");
}

void OP_BOUND_Gw_Ma_A16(void)
{
	Dword address = CalculateAddressA16();

	SignedWord lowerBound = ReadWord(address);
	SignedWord upperBound = ReadWord(address + 2);

    if((SignedWord)Gw < lowerBound || (SignedWord)Gw > (upperBound + 2))
	{
		registers->exception = EXCEPTION_BOUND_RANGE_EXCEEDED;
		ReturnImmediately();
	}

	asm (".global _OP_BOUND_Gw_Ma_A16_END\n _OP_BOUND_Gw_Ma_A16_END:");
}

void OP_BOUND_Gw_Ma_A32(void)
{
	Dword address = CalculateAddressA32();

	SignedWord lowerBound = ReadWord(address);
	SignedWord upperBound = ReadWord(address + 2);

    if((SignedWord)Gw < lowerBound || (SignedWord)Gw > (upperBound + 2))
	{
		registers->exception = EXCEPTION_BOUND_RANGE_EXCEEDED;
		ReturnImmediately();
	}

	asm (".global _OP_BOUND_Gw_Ma_A32_END\n _OP_BOUND_Gw_Ma_A32_END:");
}

void OP_BOUND_Gd_Ma_A16(void)
{
	Dword address = CalculateAddressA16();

	SignedDword lowerBound = ReadDword(address);
	SignedDword upperBound = ReadDword(address + 4);

    if((SignedDword)Gd < lowerBound || (SignedDword)Gd > (upperBound + 4))
	{
		registers->exception = EXCEPTION_BOUND_RANGE_EXCEEDED;
		ReturnImmediately();
	}

	asm (".global _OP_BOUND_Gd_Ma_A16_END\n _OP_BOUND_Gd_Ma_A16_END:");
}

void OP_BOUND_Gd_Ma_A32(void)
{
	Dword address = CalculateAddressA32();

	SignedDword lowerBound = ReadDword(address);
	SignedDword upperBound = ReadDword(address + 4);

    if((SignedDword)Gd < lowerBound || (SignedDword)Gd > (upperBound + 4))
	{
		registers->exception = EXCEPTION_BOUND_RANGE_EXCEEDED;
		ReturnImmediately();
	}

	asm (".global _OP_BOUND_Gd_Ma_A32_END\n _OP_BOUND_Gd_Ma_A32_END:");
}

void OP_CLTS(void)
{
	registers->r_cr0 &= ~CR0_TS_FLAG;

	asm (".global _OP_CLTS_END\n _OP_CLTS_END:");
}

void OP_ARPL_Rw_Gw(void)
{
	//Real mode / V8086 mode: #UD
	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		registers->exception = EXCEPTION_INVALID_OPCODE;
		ReturnImmediately();
	}

	if((Rw & SELECTOR_RPL_MASK) < (Gw & SELECTOR_RPL_MASK))
	{
		registers->r_eflags |= EFLAGS_Z_FLAG;
		
		Rw &= ~SELECTOR_RPL_MASK;
		Rw |= Gw & SELECTOR_RPL_MASK;
	}
	else
		registers->r_eflags &= ~EFLAGS_Z_FLAG;

	asm (".global _OP_ARPL_Rw_Gw_END\n _OP_ARPL_Rw_Gw_END:");
}

void OP_ARPL_Mw_Gw_A16(void)
{
	//Real mode / V8086 mode: #UD
	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		registers->exception = EXCEPTION_INVALID_OPCODE;
		ReturnImmediately();
	}

	Dword address = CalculateAddressA16();
	Word dest = ReadWord(address);

	if((dest & SELECTOR_RPL_MASK) < (Gw & SELECTOR_RPL_MASK))
	{
		registers->r_eflags |= EFLAGS_Z_FLAG;
		
		dest &= ~SELECTOR_RPL_MASK;
		dest |= Gw & SELECTOR_RPL_MASK;

		WriteWord(address, dest);
	}
	else
		registers->r_eflags &= ~EFLAGS_Z_FLAG;

	asm (".global _OP_ARPL_Mw_Gw_A16_END\n _OP_ARPL_Mw_Gw_A16_END:");
}

void OP_ARPL_Mw_Gw_A32(void)
{
	//Real mode / V8086 mode: #UD
	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		registers->exception = EXCEPTION_INVALID_OPCODE;
		ReturnImmediately();
	}

	Dword address = CalculateAddressA32();
	Word dest = ReadWord(address);

	if((dest & SELECTOR_RPL_MASK) < (Gw & SELECTOR_RPL_MASK))
	{
		registers->r_eflags |= EFLAGS_Z_FLAG;
		
		dest &= ~SELECTOR_RPL_MASK;
		dest |= Gw & SELECTOR_RPL_MASK;

		WriteWord(address, dest);
	}
	else
		registers->r_eflags &= ~EFLAGS_Z_FLAG;

	asm (".global _OP_ARPL_Mw_Gw_A32_END\n _OP_ARPL_Mw_Gw_A32_END:");
}

void OP_CPUID(void)
{
	if(registers->r_eax == 0)
	{
		registers->r_eax = 1;			//Maximum input value
		registers->r_ebx = 0x756e6547;	//EBX:EDX:ECX = "GenuineIntel"
		registers->r_ecx = 0x6c65746e;
		registers->r_edx = 0x49656e69;
	}
	else
	{
		registers->r_eax = 0x525;		//Family 5, Model 2, Stepping 5
										//Taken from an Intel Pentium @ 75MHz (EDX = 0x1bf)
		registers->r_ebx = 0;
		registers->r_ecx = 0;
		registers->r_edx =	CPUID_FEATURE_CMOV_BIT |
							CPUID_FEATURE_PGE_BIT |
							CPUID_FEATURE_TSC_BIT |
							CPUID_FEATURE_FPU_BIT;
	}

	asm (".global _OP_CPUID_END\n _OP_CPUID_END:");
}

void OP_RDTSC(void)
{
	//TODO: Only take into account the time spent executing instructions
	//TODO: Make this multi-core/processor safe
	Dword r_eax, r_edx;
	
	__asm__ volatile
	(
		"rdtsc		\n"
		: "=a"(r_eax), "=d"(r_edx)
		:
	);

	registers->r_eax = r_eax;
	registers->r_edx = r_edx;

	asm (".global _OP_RDTSC_END\n _OP_RDTSC_END:");
}
