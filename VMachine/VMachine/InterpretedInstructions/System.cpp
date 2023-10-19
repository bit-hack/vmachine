//	VMachine
//	System instruction implementations
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"

namespace
{
	const bool systemSegmentTypesValidForLAR[16] =
	{
		false, true, true, true,
		true, true, false, false,
		false, true, false, true,
		true, false, false, false
	};

	const bool systemSegmentTypesValidForLSL[16] =
	{
		false, true, true, true,
		false, false, false, false,
		false, true, false, true,
		false, false, false, false
	};
}

void VMachine::CLI(PrefixBitset prefixes)
{
	if((registers->r_cr0 & CR0_PE_FLAG) != 0)
	{
		if((registers->r_systemFlags & EFLAGS_VM_FLAG) == 0)
		
		{
			//Protected mode: If IOPL < CPL, #GP(0)
			if(((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >> EFLAGS_IOPL_SHIFT) <
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
					>> DESCRIPTOR_FLAGS_DPL_SHIFT))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = 0;
				return;
			}
		}
		else
		{
			//V8086 mode: If IOPL != 3, #GP(0)
			if((registers->r_systemFlags & EFLAGS_IOPL_FLAG) != EFLAGS_IOPL_FLAG)
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = 0;
				return;
			}
		}
	}

	registers->r_systemFlags &= ~EFLAGS_I_FLAG;
}

void VMachine::STI(PrefixBitset prefixes)
{
	if((registers->r_cr0 & CR0_PE_FLAG) != 0)
	{
		if((registers->r_systemFlags & EFLAGS_VM_FLAG) == 0)
		
		{
			//Protected mode: If IOPL < CPL, #GP(0)
			if(((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >> EFLAGS_IOPL_SHIFT) <
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
					>> DESCRIPTOR_FLAGS_DPL_SHIFT))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = 0;
				return;
			}
		}
		else
		{
			//V8086 mode: If IOPL != 3, #GP(0)
			if((registers->r_systemFlags & EFLAGS_IOPL_FLAG) != EFLAGS_IOPL_FLAG)
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = 0;
				return;
			}
		}
	}

	registers->r_systemFlags |= EFLAGS_I_FLAG;
}

void VMachine::SLDT_Mw(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Write the selector
	if(!WriteWord(*segReg, offset, registers->r_ldtr.selector))
		return;
}

void VMachine::SLDT_Rw(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	Word * reg2;

	if(!DecodeModRMByteRegRegWord(registers->r_eip, reg1, reg2))
		return;

	//Save the selector
	*reg2 = registers->r_ldtr.selector;
}

void VMachine::STR_Mw(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Write the selector
	if(!WriteWord(*segReg, offset, registers->r_tr.selector))
		return;
}

void VMachine::STR_Rw(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	Word * reg2;

	if(!DecodeModRMByteRegRegWord(registers->r_eip, reg1, reg2))
		return;

	//Save the selector
	*reg2 = registers->r_tr.selector;
}

void VMachine::LLDT_Mw(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Read the selector
	Word selector;

	if(!ReadWord(*segReg, offset, selector))
		return;

	//Read the segment descriptor
	Dword descriptorLow, descriptorFlags;

	if(	!ReadDwordLinear(registers->r_gdtr.base + (selector & SELECTOR_INDEX_MASK), descriptorLow) ||
		!ReadDwordLinear(registers->r_gdtr.base + (selector & SELECTOR_INDEX_MASK) + 4, descriptorFlags))
	{
		return;
	}

	//Save the values
	registers->r_ldtr.selector = selector;
	registers->r_ldtr.base = GetDescriptorBase(descriptorLow, descriptorFlags);
	registers->r_ldtr.limit = GetDescriptorLimit(descriptorLow, descriptorFlags);
	registers->r_ldtr.descriptorFlags = GetDescriptorFlags(descriptorFlags);
}

void VMachine::LLDT_Rw(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	Word * reg2;

	if(!DecodeModRMByteRegRegWord(registers->r_eip, reg1, reg2))
		return;

	//Get the selector
	Word selector = *reg2;

	//Read the segment descriptor
	Dword descriptorLow, descriptorFlags;

	if(	!ReadDwordLinear(registers->r_gdtr.base + (selector & SELECTOR_INDEX_MASK), descriptorLow) ||
		!ReadDwordLinear(registers->r_gdtr.base + (selector & SELECTOR_INDEX_MASK) + 4, descriptorFlags))
	{
		return;
	}

	//Save the values
	registers->r_ldtr.selector = selector;
	registers->r_ldtr.base = GetDescriptorBase(descriptorLow, descriptorFlags);
	registers->r_ldtr.limit = GetDescriptorLimit(descriptorLow, descriptorFlags);
	registers->r_ldtr.descriptorFlags = GetDescriptorFlags(descriptorFlags);
}

void VMachine::LTR_Mw(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Read the selector
	Word selector;

	if(!ReadWord(*segReg, offset, selector))
		return;

	//Read the segment descriptor
	Dword descriptorLow, descriptorFlags;

	if(	!ReadDwordLinear(registers->r_gdtr.base + (selector & SELECTOR_INDEX_MASK), descriptorLow) ||
		!ReadDwordLinear(registers->r_gdtr.base + (selector & SELECTOR_INDEX_MASK) + 4, descriptorFlags))
	{
		return;
	}

	//Set the busy flag in the descriptor
	descriptorFlags |= TSS_DESCRIPTOR_FLAGS_BUSY_BIT;

	if(!WriteDwordLinear(	registers->r_gdtr.base + (selector & SELECTOR_INDEX_MASK) + 4,
							descriptorFlags))
	{
		return;
	}

	//Save the values
	registers->r_tr.selector = selector;
	registers->r_tr.base = GetDescriptorBase(descriptorLow, descriptorFlags);
	registers->r_tr.limit = GetDescriptorLimit(descriptorLow, descriptorFlags);
	registers->r_tr.descriptorFlags = GetDescriptorFlags(descriptorFlags);
}

void VMachine::LTR_Rw(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	Word * reg2;

	if(!DecodeModRMByteRegRegWord(registers->r_eip, reg1, reg2))
		return;

	//Get the selector
	Word selector = *reg2;

	//Read the segment descriptor
	Dword descriptorLow, descriptorFlags;

	if(	!ReadDwordLinear(registers->r_gdtr.base + (selector & SELECTOR_INDEX_MASK), descriptorLow) ||
		!ReadDwordLinear(registers->r_gdtr.base + (selector & SELECTOR_INDEX_MASK) + 4, descriptorFlags))
	{
		return;
	}

	//Set the busy flag in the descriptor
	descriptorFlags |= TSS_DESCRIPTOR_FLAGS_BUSY_BIT;

	if(!WriteDwordLinear(	registers->r_gdtr.base + (selector & SELECTOR_INDEX_MASK) + 4,
							descriptorFlags))
	{
		return;
	}

	//Save the values
	registers->r_tr.selector = selector;
	registers->r_tr.base = GetDescriptorBase(descriptorLow, descriptorFlags);
	registers->r_tr.limit = GetDescriptorLimit(descriptorLow, descriptorFlags);
	registers->r_tr.descriptorFlags = GetDescriptorFlags(descriptorFlags);
}

void VMachine::DoVERR(Word selector)
{
	//Get the base of the descriptor table
	Dword tableBase = registers->r_gdtr.base;

	if((selector & SELECTOR_TI_BIT) != 0)
		tableBase = registers->r_ldtr.base;

	//Read the descriptor high dword
	Dword descriptorFlags;

	if(!ReadDwordLinear(tableBase + (selector & SELECTOR_INDEX_MASK) + 4, descriptorFlags))
		return;

	//If CPL > DPL or RPL > DPL, ZF = 0. Otherwise, ZF = 1
	if(	(registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) > 
			(descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) ||
		static_cast<Dword>((selector & SELECTOR_RPL_MASK) >> SELECTOR_RPL_SHIFT) >
			(descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) >> DESCRIPTOR_FLAGS_DPL_SHIFT)
	{
		registers->r_eflags &= ~EFLAGS_Z_FLAG;
	}
	else
	{
		registers->r_eflags |= EFLAGS_Z_FLAG;
	}
}

void VMachine::VERR_Mw(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Read the selector
	Word selector;
	if(!ReadWord(*segReg, offset, selector))
		return;

	DoVERR(selector);
}

void VMachine::VERR_Rw(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	Word * reg2;

	if(!DecodeModRMByteRegRegWord(registers->r_eip, reg1, reg2))
		return;

	DoVERR(*reg2);
}

void VMachine::DoVERW(Word selector)
{
	//Get the base of the descriptor table
	Dword tableBase = registers->r_gdtr.base;

	if((selector & SELECTOR_TI_BIT) != 0)
		tableBase = registers->r_ldtr.base;

	//Read the descriptor high dword
	Dword descriptorFlags;

	if(!ReadDwordLinear(tableBase + (selector & SELECTOR_INDEX_MASK) + 4, descriptorFlags))
		return;

	//If CPL > DPL or RPL > DPL, ZF = 0. Otherwise, ZF = 1
	if(	(registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) > 
			(descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) ||
		static_cast<Dword>((selector & SELECTOR_RPL_MASK) >> SELECTOR_RPL_SHIFT) >
			(descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) >> DESCRIPTOR_FLAGS_DPL_SHIFT)
	{
		registers->r_eflags &= ~EFLAGS_Z_FLAG;
	}
	else
	{
		registers->r_eflags |= EFLAGS_Z_FLAG;
	}
}

void VMachine::VERW_Mw(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Read the selector
	Word selector;
	if(!ReadWord(*segReg, offset, selector))
		return;

	DoVERW(selector);
}

void VMachine::VERW_Rw(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	Word * reg2;

	if(!DecodeModRMByteRegRegWord(registers->r_eip, reg1, reg2))
		return;

	DoVERW(*reg2);
}

void VMachine::SGDT_Ms_O16(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Store the GDTR
	if(	!WriteWord(*segReg, offset, registers->r_gdtr.limit) ||
		!WriteDword(*segReg, offset + 2, registers->r_gdtr.base & 0x00ffffff))
		return;
}

void VMachine::SGDT_Ms_O32(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Store the GDTR
	if(	!WriteWord(*segReg, offset, registers->r_gdtr.limit) ||
		!WriteDword(*segReg, offset + 2, registers->r_gdtr.base))
		return;
}

void VMachine::SIDT_Ms_O16(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Store the IDTR
	if(	!WriteWord(*segReg, offset, registers->r_idtr.limit) ||
		!WriteDword(*segReg, offset + 2, registers->r_idtr.base & 0x00ffffff))
		return;
}	

void VMachine::SIDT_Ms_O32(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Store the IDTR
	if(	!WriteWord(*segReg, offset, registers->r_idtr.limit) ||
		!WriteDword(*segReg, offset + 2, registers->r_idtr.base))
		return;
}

void VMachine::LGDT_Ms_O16(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Get the new GDTR
	Word gdtrLimit;
	Dword gdtrBase;

	if(!ReadWord(*segReg, offset, gdtrLimit) || !ReadDword(*segReg, offset + 2, gdtrBase))
		return;

	registers->r_gdtr.base = gdtrBase & 0x00ffffff;
	registers->r_gdtr.limit = gdtrLimit;
}
	
void VMachine::LGDT_Ms_O32(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Get the new GDTR
	Word gdtrLimit;
	Dword gdtrBase;

	if(!ReadWord(*segReg, offset, gdtrLimit) || !ReadDword(*segReg, offset + 2, gdtrBase))
		return;

	registers->r_gdtr.base = gdtrBase;
	registers->r_gdtr.limit = gdtrLimit;
}

void VMachine::LIDT_Ms_O16(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Get the new IDTR
	Word idtrLimit;
	Dword idtrBase;

	if(!ReadWord(*segReg, offset, idtrLimit) || !ReadDword(*segReg, offset + 2, idtrBase))
		return;

	registers->r_idtr.base = idtrBase & 0x00ffffff;
	registers->r_idtr.limit = idtrLimit;
}

void VMachine::LIDT_Ms_O32(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Get the new IDTR
	Word idtrLimit;
	Dword idtrBase;

	if(!ReadWord(*segReg, offset, idtrLimit) || !ReadDword(*segReg, offset + 2, idtrBase))
		return;

	registers->r_idtr.base = idtrBase;
	registers->r_idtr.limit = idtrLimit;
}


void VMachine::SMSW_Rw(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	Word * reg2;

	if(!DecodeModRMByteRegRegWord(registers->r_eip, reg1, reg2))
		return;

	//Store the lower word of CR0
	*reg2 = static_cast<Word>(registers->r_cr0);
}

void VMachine::SMSW_Mw(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Write the lower word of CR0
	if(!WriteWord(*segReg, offset, static_cast<Word>(registers->r_cr0)))
		return;
}

void VMachine::LMSW_Rw(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	Word * reg2;

	if(!DecodeModRMByteRegRegWord(registers->r_eip, reg1, reg2))
		return;

	SetCR0Value((registers->r_cr0 & 0xffff0000) | *reg2);
}

void VMachine::LMSW_Mw(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Read the lower word of CR0
	Word msw;
	if(!ReadWord(*segReg, offset, msw))
		return;

	SetCR0Value((registers->r_cr0 & 0xffff0000) | msw);
}

void VMachine::INVLPG_M(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Calculate the linear address to invalidate
	Dword linAddr = segReg->data.base + offset;

	//Invalidate the corresponding entry in the TLBs
	Dword tlbIndex = (linAddr >> 12) & TLB_MASK;

	registers->supervisorReadTlb[tlbIndex].linearPageAndFlags		= TLB_ENTRY_INVALID_FLAG;
	registers->userReadTlb[tlbIndex].linearPageAndFlags				= TLB_ENTRY_INVALID_FLAG;
	registers->supervisorWriteTlb[tlbIndex].linearPageAndFlags		= TLB_ENTRY_INVALID_FLAG;
	registers->supervisorWriteWPTlb[tlbIndex].linearPageAndFlags	= TLB_ENTRY_INVALID_FLAG;
	registers->userWriteTlb[tlbIndex].linearPageAndFlags			= TLB_ENTRY_INVALID_FLAG;
}

void VMachine::DoLAR_O16(Word * Gw, Word selector)
{
	Dword Gd;
	
	if(DoLAR_O32(&Gd, selector))
		*Gw = static_cast<Word>(Gd);
}

bool VMachine::DoLAR_O32(Dword * Gd, Word selector)
{
	//Clear the Z flag (assume failure)
	registers->r_eflags &= ~EFLAGS_Z_FLAG;

	//If the selector is NULL, failure
	if((selector & SELECTOR_INDEX_MASK) == 0)
		return false;

	//Get the base of the descriptor table
	Dword tableBase = registers->r_gdtr.base;

	if((selector & SELECTOR_TI_BIT) != 0)
		tableBase = registers->r_ldtr.base;

	//Read the descriptor high dword
	Dword descriptorHigh;

	if(!ReadDwordLinear(tableBase + (selector & SELECTOR_INDEX_MASK) + 4, descriptorHigh))
		return false;

	//If segment type != conforming code segment and ((CPL > DPL) or (RPL > DPL)), failure
	if(!IsConformingCodeSegment(descriptorHigh) &&
		((((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) >>
			DESCRIPTOR_FLAGS_DPL_SHIFT) >
				((descriptorHigh & DESCRIPTOR_FLAGS_DPL_MASK) >>
					DESCRIPTOR_FLAGS_DPL_SHIFT)) ||
		(Dword)((selector & SELECTOR_RPL_MASK) >> SELECTOR_RPL_SHIFT) >
			((descriptorHigh & DESCRIPTOR_FLAGS_DPL_MASK) >>
				DESCRIPTOR_FLAGS_DPL_SHIFT)))
	{
		return false;
	}

	//If the segment type is not valid for LAR, failure
	if(	IsSystemSegment(descriptorHigh) &&
		!systemSegmentTypesValidForLAR[(descriptorHigh & 0x00000f00) >> 8])
	{
		return false;
	}

	//Save the descriptor flags into Gd
	*Gd = GetDescriptorFlags(descriptorHigh);

	//Set Z flag to indicate successful completion
	registers->r_eflags |= EFLAGS_Z_FLAG;

	return true;
}

void VMachine::LAR_Gw_Rw(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	Word * reg2;

	if(!DecodeModRMByteRegRegWord(registers->r_eip, reg1, reg2))
		return;

	DoLAR_O16(reg1, *reg2);
}

void VMachine::LAR_Gw_Mw(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Get the segment selector
	Word selector;
	if(!ReadWord(*segReg, offset, selector))
		return;

	DoLAR_O16(reg1, selector);
}

void VMachine::LAR_Gd_Rw(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Dword * reg1;
	Dword * reg2;

	if(!DecodeModRMByteRegRegDword(registers->r_eip, reg1, reg2))
		return;

	DoLAR_O32(reg1, static_cast<Word>(*reg2));
}

void VMachine::LAR_Gd_Mw(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Dword * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryDword(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Get the segment selector
	Word selector;
	if(!ReadWord(*segReg, offset, selector))
		return;

	DoLAR_O32(reg1, selector);
}

void VMachine::DoLSL_O16(Word * Gw, Word selector)
{
	Dword Gd;
	
	if(DoLSL_O32(&Gd, selector))
		*Gw = static_cast<Word>(Gd);
}

bool VMachine::DoLSL_O32(Dword * Gd, Word selector)
{
	//Clear the Z flag (assume failure)
	registers->r_eflags &= ~EFLAGS_Z_FLAG;

	//If the selector is NULL, failure
	if((selector & SELECTOR_INDEX_MASK) == 0)
		return false;

	//Get the base of the descriptor table
	Dword tableBase = registers->r_gdtr.base;

	if((selector & SELECTOR_TI_BIT) != 0)
		tableBase = registers->r_ldtr.base;

	//Read the descriptor
	Dword descriptorLow, descriptorHigh;

	if(	!ReadDwordLinear(tableBase + (selector & SELECTOR_INDEX_MASK), descriptorLow) ||
		!ReadDwordLinear(tableBase + (selector & SELECTOR_INDEX_MASK) + 4, descriptorHigh))
	{
		return false;
	}

	//If segment type != conforming code segment and ((CPL > DPL) or (RPL > DPL)), failure
	if(!IsConformingCodeSegment(descriptorHigh) &&
		((((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) >>
			DESCRIPTOR_FLAGS_DPL_SHIFT) >
				((descriptorHigh & DESCRIPTOR_FLAGS_DPL_MASK) >>
					DESCRIPTOR_FLAGS_DPL_SHIFT)) ||
		(Dword)((selector & SELECTOR_RPL_MASK) >> SELECTOR_RPL_SHIFT) >
			((descriptorHigh & DESCRIPTOR_FLAGS_DPL_MASK) >>
				DESCRIPTOR_FLAGS_DPL_SHIFT)))
	{
		return false;
	}

	//If the segment type is not valid for LSL, failure
	if(	IsSystemSegment(descriptorHigh) &&
		!systemSegmentTypesValidForLSL[(descriptorHigh & 0x00000f00) >> 8])
	{
		return false;
	}

	//Save the segment limit
	*Gd = GetDescriptorLimit(descriptorLow, descriptorHigh);

	//Set Z flag to indicate successful completion
	registers->r_eflags |= EFLAGS_Z_FLAG;

	return true;
}

void VMachine::LSL_Gw_Rw(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	Word * reg2;

	if(!DecodeModRMByteRegRegWord(registers->r_eip, reg1, reg2))
		return;

	DoLSL_O16(reg1, *reg2);
}

void VMachine::LSL_Gw_Mw(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Get the segment selector
	Word selector;
	if(!ReadWord(*segReg, offset, selector))
		return;

	DoLSL_O16(reg1, selector);
}

void VMachine::LSL_Gd_Rw(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Dword * reg1;
	Dword * reg2;

	if(!DecodeModRMByteRegRegDword(registers->r_eip, reg1, reg2))
		return;

	DoLSL_O32(reg1, static_cast<Word>(*reg2));
}

void VMachine::LSL_Gd_Mw(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Dword * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryDword(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Get the segment selector
	Word selector;
	if(!ReadWord(*segReg, offset, selector))
		return;

	DoLSL_O32(reg1, selector);
}

void VMachine::MOV_Cd_Rd(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Dword * reg1;
	Dword * reg2;

	if(!DecodeModRMByteRegRegDword(registers->r_eip, reg1, reg2))
		return;

	if(reg1 == &registers->r_eax)
		SetCR0Value(*reg2);
	else if(reg1 == &registers->r_edx)
		registers->r_cr2 = *reg2;
	else if(reg1 == &registers->r_ebx)
		SetCR3Value(*reg2);
	else if(reg1 == &registers->r_esp)
		registers->r_cr4 = (*reg2 & CR4_MASK);
	else
		registers->exception = EXCEPTION_INVALID_OPCODE;
}

void VMachine::MOV_Dd_Rd(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Dword * reg1;
	Dword * reg2;

	if(!DecodeModRMByteRegRegDword(registers->r_eip, reg1, reg2))
		return;

	if(reg1 == &registers->r_eax)
		registers->r_dr[0] = *reg2;
	else if(reg1 == &registers->r_ecx)
		registers->r_dr[1] = *reg2;
	else if(reg1 == &registers->r_edx)
		registers->r_dr[2] = *reg2;
	else if(reg1 == &registers->r_ebx)
		registers->r_dr[3] = *reg2;
	else if(reg1 == &registers->r_esp)
		registers->r_dr[4] = *reg2;
	else if(reg1 == &registers->r_ebp)
		registers->r_dr[5] = *reg2;
	else if(reg1 == &registers->r_esi)
		registers->r_dr[6] = *reg2;
	else if(reg1 == &registers->r_edi)
		registers->r_dr[7] = *reg2;
}
