//	VMachine
//	Structures for a VMachine's registers
//	Included in both the VMachine project (MSVC, C++) and the helper functions (GCC, C)
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

struct SegmentRegisterData
{
	Word selector;
	Dword base, limit;
	Dword descriptorFlags;
};

struct TlbEntry
{
	Dword linearPageAndFlags;
	Dword linToHostOffset;
};

#define TLB_SIZE 0x100	//Must be a power of 2
#define TLB_MASK (TLB_SIZE - 1)

#define TLB_ENTRY_LINEAR_PAGE_MASK 0xfffff000

#define TLB_ENTRY_INVALID_FLAG					0x00000800
#define TLB_ENTRY_MMIO_FLAG						0x00000400
#define TLB_ENTRY_CONTAINS_TRANSLATIONS_FLAG	0x00000200

#define TLB_ENTRY_FLAGS_MASK (TLB_ENTRY_INVALID_FLAG)

struct EipToTranslationTLBEntry
{
	Dword physicalEip;
	Dword flags;
	Byte * translation;
};

#define EIP_TO_TRANSLATION_TLB_SIZE 0x1000	//Must be a power of 2
#define EIP_TO_TRANSLATION_TLB_MASK (EIP_TO_TRANSLATION_TLB_SIZE - 1)

#define EIP_TO_TRANSLATION_TLB_EMPTY_FLAG 0x80000000

struct Registers
{
	union { Dword r_eax; Word r_ax; struct { Byte r_al, r_ah; }; };
	union { Dword r_ecx; Word r_cx; struct { Byte r_cl, r_ch; }; };
	union { Dword r_edx; Word r_dx; struct { Byte r_dl, r_dh; }; };
	union { Dword r_ebx; Word r_bx; struct { Byte r_bl, r_bh; }; };

	union { Dword r_esp; Word r_sp; };
	union { Dword r_ebp; Word r_bp; };
	union { Dword r_esi; Word r_si; };
	union { Dword r_edi; Word r_di; };

	union { Dword r_eip; Word r_ip; };

	Dword r_eflags;			//OF, SF, ZF, AF, PF, CF. Other bits as set by host
	Dword r_systemFlags;	//ID, AC, VM, RF, NT, IOPL, DF, IF, TF, bit 1 set

	struct SegmentRegisterData r_cs, r_ds, r_es, r_fs, r_gs, r_ss;

	Dword r_cr0, r_cr2, r_cr3, r_cr4;
	Dword r_dr[8];

	struct { Dword base; Word limit; } r_gdtr, r_idtr;
	struct SegmentRegisterData r_ldtr, r_tr;

	Dword exception, errorCode;

	Dword zero;

	//Set esp to this value and return in order to return immediately from the translated code
	Dword returnImmediatelyEsp;
#define REGISTERS_RETURN_IMMEDIATELY_ESP_OFFSET 248
#define REGISTERS_RETURN_IMMEDIATELY_ESP_OFFSET_STRING "248"

	//Number of instructions to execute
	SignedDword numInstr;

	Byte scratchMemory[1024];

	//Memory
	Dword memoryPtr;
	Dword memorySize;
	Dword addressMask;

	//TLBs
	struct TlbEntry * supervisorReadTlb;
	struct TlbEntry * userReadTlb;
	struct TlbEntry * supervisorWriteTlb;
	struct TlbEntry * supervisorWriteWPTlb;
	struct TlbEntry * userWriteTlb;
#define REGISTERS_SUPERVISOR_READ_TLB_OFFSET_STRING "(1024 + 268)"
#define REGISTERS_USER_READ_TLB_OFFSET_STRING "(1024 + 272)"
#define REGISTERS_SUPERVISOR_WRITE_TLB_OFFSET_STRING "(1024 + 276)"
#define REGISTERS_SUPERVISOR_WRITE_WP_TLB_OFFSET_STRING "(1024 + 280)"
#define REGISTERS_USER_WRITE_TLB_OFFSET_STRING "(1024 + 284)"
		
	Byte * pageClassifications;

	//The physical page and flags of the currently executing translation, if any
	Dword currentTranslationPhysicalPage;
	Dword currentTranslationFlags;

	//Eip => translation TLB
	struct EipToTranslationTLBEntry * eipToTranslationTLB;

	Dword jumpToNextTranslationAddress;

	Dword vmachineThisPtr;
#define REGISTERS_VMACHINE_THIS_PTR_OFFSET_STRING "(1024 + 308)"
	
	Byte (*ReadBytePhysical)(Dword vmachineThisPtr, Dword address);
	Word (*ReadWordPhysical)(Dword vmachineThisPtr, Dword address);
	Dword (*ReadDwordPhysical)(Dword vmachineThisPtr, Dword address);
#define REGISTERS_READ_BYTE_PHYSICAL_OFFSET_STRING "(1024 + 312)"
#define REGISTERS_READ_WORD_PHYSICAL_OFFSET_STRING "(1024 + 316)"
#define REGISTERS_READ_DWORD_PHYSICAL_OFFSET_STRING "(1024 + 320)"
	
	void (*WriteBytePhysical)(Dword vmachineThisPtr, Dword address, Byte data);
	void (*WriteWordPhysical)(Dword vmachineThisPtr, Dword address, Word data);
	void (*WriteDwordPhysical)(Dword vmachineThisPtr, Dword address, Dword data);
#define REGISTERS_WRITE_BYTE_PHYSICAL_OFFSET_STRING "(1024 + 324)"
#define REGISTERS_WRITE_WORD_PHYSICAL_OFFSET_STRING "(1024 + 328)"
#define REGISTERS_WRITE_DWORD_PHYSICAL_OFFSET_STRING "(1024 + 332)"

	void (*InvalidateCodeTranslations)(Dword vmachineThisPtr, Dword startAddress, Dword endAddress, Dword data);
#define REGISTERS_INVALIDATE_CODE_TRANSLATIONS_OFFSET_STRING "(1024 + 336)"
};

#define PAGE_CLASS_NORMAL					0
#define PAGE_CLASS_MMIO						1
#define PAGE_CLASS_CONTAINS_TRANSLATIONS	2

#define TRANSLATION_DEFAULT_A_O_SIZE_32_FLAG			0x01
#define TRANSLATION_STACK_SIZE_32_FLAG					0x02
#define TRANSLATION_CS_DS_ES_SS_BASES_ZERO_FLAG			0x04
#define TRANSLATION_SUPERVISOR_MODE_WRITE_PROTECT_FLAG	0x08
#define TRANSLATION_USER_MODE_FLAG						0x10
#define TRANSLATION_EMPTY_FLAG							0x80000000
