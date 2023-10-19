//	VMachine
//	Helper functions called from translated code
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "HelperFunctions.h"

//Meta-helper functions to call through function pointers in the Registers structure

Byte CallRegistersReadBytePhysical(Dword address) __attribute__((noinline));
Byte CallRegistersReadBytePhysical(Dword address)
{
	//return registers->ReadBytePhysical(registers->vmachineThisPtr, address);
	asm
	(
		"pushl %ecx														\n"
		"pushl %edx														\n"
		"movl 12(%esp), %eax											\n"
		"pushl %eax														\n"
		"movl "REGISTERS_VMACHINE_THIS_PTR_OFFSET_STRING"(%ebp), %eax	\n"
		"pushl %eax														\n"
		"call *"REGISTERS_READ_BYTE_PHYSICAL_OFFSET_STRING"(%ebp)		\n"
		"addl $8, %esp													\n"
		"popl %edx														\n"
		"popl %ecx														\n"
	);
}

Word CallRegistersReadWordPhysical(Dword address) __attribute__((noinline));
Word CallRegistersReadWordPhysical(Dword address)
{
	//return registers->ReadWordPhysical(registers->vmachineThisPtr, address);
	asm
	(
		"pushl %ecx														\n"
		"pushl %edx														\n"
		"movl 12(%esp), %eax											\n"
		"pushl %eax														\n"
		"movl "REGISTERS_VMACHINE_THIS_PTR_OFFSET_STRING"(%ebp), %eax	\n"
		"pushl %eax														\n"
		"call *"REGISTERS_READ_WORD_PHYSICAL_OFFSET_STRING"(%ebp)		\n"
		"addl $8, %esp													\n"
		"popl %edx														\n"
		"popl %ecx														\n"
	);
}

Dword CallRegistersReadDwordPhysical(Dword address) __attribute__((noinline));
Dword CallRegistersReadDwordPhysical(Dword address)
{
	//return registers->ReadDwordPhysical(registers->vmachineThisPtr, address);
	asm
	(
		"pushl %ecx														\n"
		"pushl %edx														\n"
		"movl 12(%esp), %eax											\n"
		"pushl %eax														\n"
		"movl "REGISTERS_VMACHINE_THIS_PTR_OFFSET_STRING"(%ebp), %eax	\n"
		"pushl %eax														\n"
		"call *"REGISTERS_READ_DWORD_PHYSICAL_OFFSET_STRING"(%ebp)		\n"
		"addl $8, %esp													\n"
		"popl %edx														\n"
		"popl %ecx														\n"
	);
}

void CallRegistersWriteBytePhysical(Dword address, Byte data) __attribute__((noinline));
void CallRegistersWriteBytePhysical(Dword address, Byte data)
{
	//registers->WriteBytePhysical(registers->vmachineThisPtr, address, data);
	asm
	(
		"pushl %ecx														\n"
		"pushl %edx														\n"
		"movl 16(%esp), %eax											\n"
		"pushl %eax														\n"
		"movl 16(%esp), %eax											\n"
		"pushl %eax														\n"
		"movl "REGISTERS_VMACHINE_THIS_PTR_OFFSET_STRING"(%ebp), %eax	\n"
		"pushl %eax														\n"
		"call *"REGISTERS_WRITE_BYTE_PHYSICAL_OFFSET_STRING"(%ebp)		\n"
		"addl $12, %esp													\n"
		"popl %edx														\n"
		"popl %ecx														\n"
	);
}

void CallRegistersWriteWordPhysical(Dword address, Word data) __attribute__((noinline));
void CallRegistersWriteWordPhysical(Dword address, Word data)
{
	//registers->WriteWordPhysical(registers->vmachineThisPtr, address, data);
	asm
	(
		"pushl %ecx														\n"
		"pushl %edx														\n"
		"movl 16(%esp), %eax											\n"
		"pushl %eax														\n"
		"movl 16(%esp), %eax											\n"
		"pushl %eax														\n"
		"movl "REGISTERS_VMACHINE_THIS_PTR_OFFSET_STRING"(%ebp), %eax	\n"
		"pushl %eax														\n"
		"call *"REGISTERS_WRITE_WORD_PHYSICAL_OFFSET_STRING"(%ebp)		\n"
		"addl $12, %esp													\n"
		"popl %edx														\n"
		"popl %ecx														\n"
	);
}

void CallRegistersWriteDwordPhysical(Dword address, Dword data) __attribute__((noinline));
void CallRegistersWriteDwordPhysical(Dword address, Dword data)
{
	//registers->WriteDwordPhysical(registers->vmachineThisPtr, address, data);
	asm
	(
		"pushl %ecx														\n"
		"pushl %edx														\n"
		"movl 16(%esp), %eax											\n"
		"pushl %eax														\n"
		"movl 16(%esp), %eax											\n"
		"pushl %eax														\n"
		"movl "REGISTERS_VMACHINE_THIS_PTR_OFFSET_STRING"(%ebp), %eax	\n"
		"pushl %eax														\n"
		"call *"REGISTERS_WRITE_DWORD_PHYSICAL_OFFSET_STRING"(%ebp)		\n"
		"addl $12, %esp													\n"
		"popl %edx														\n"
		"popl %ecx														\n"
	);
}

void CallRegistersInvalidateCodeTranslations(Dword startAddress, Dword endAddress, Dword data) __attribute__((noinline));
void CallRegistersInvalidateCodeTranslations(Dword startAddress, Dword endAddress, Dword data)
{
	//registers->InvalidateCodeTranslations(registers->vmachineThisPtr, startAddress, endAddress);
	asm
	(
		"pushl %ecx														\n"
		"pushl %edx														\n"
		"movl 20(%esp), %eax											\n"
		"pushl %eax														\n"
		"movl 20(%esp), %eax											\n"
		"pushl %eax														\n"
		"movl 20(%esp), %eax											\n"
		"pushl %eax														\n"
		"movl "REGISTERS_VMACHINE_THIS_PTR_OFFSET_STRING"(%ebp), %eax	\n"
		"pushl %eax														\n"
		"call *"REGISTERS_INVALIDATE_CODE_TRANSLATIONS_OFFSET_STRING"(%ebp)		\n"
		"addl $16, %esp													\n"
		"popl %edx														\n"
		"popl %ecx														\n"
	);
}

//Address translation functions
static inline Dword ReadPTE(Dword address)
{
	if(LIKELY(address < registers->memorySize - 3))
		return *(Dword *)(registers->memoryPtr + address);

	return 0;
}

static inline void WritePTE(Dword address, Dword data)
{
	if(LIKELY(address < registers->memorySize - 3))
		*(Dword *)(registers->memoryPtr + address) = data;
}

static inline void TranslateAddressForReadPagingDisabled(	Dword linearAddress,
															struct TlbEntry * tlbEntry)
{
	//Calculate the physical page (the contents of the PTE, if there were one)
	Dword pte = linearAddress & registers->addressMask;

	tlbEntry->linearPageAndFlags  = linearAddress & TLB_ENTRY_LINEAR_PAGE_MASK;

	if(LIKELY((pte & 0xfffff000) < registers->memorySize))
	{
		//If the physical page is marked as MMIO, set the MMIO flag
		if(registers->pageClassifications[pte >> 12] == PAGE_CLASS_MMIO)
			tlbEntry->linearPageAndFlags |= TLB_ENTRY_MMIO_FLAG;

		tlbEntry->linToHostOffset =
			registers->memoryPtr + (pte & 0xfffff000) - (linearAddress & 0xfffff000);
	}
	else
	{
		//If the physical address is beyond the end of memory, use the first extra page
		tlbEntry->linToHostOffset =
			registers->memoryPtr + registers->memorySize - (linearAddress & 0xfffff000);
	}
}

static void TranslateAddressForReadSupervisorMode(Dword linearAddress, struct TlbEntry * tlbEntry)
{
	if((registers->r_cr0 & CR0_PG_FLAG) == 0)
	{
		TranslateAddressForReadPagingDisabled(linearAddress, tlbEntry);
		return;
	}

	Dword pde = ReadPTE(
		(registers->r_cr3 & 0xfffff000) | ((linearAddress & 0xffc00000) >> 20));

	if(UNLIKELY((pde & PDE_PRESENT_BIT) == 0))
	{
		registers->exception = EXCEPTION_PAGE_FAULT;
		registers->errorCode =
			PAGE_FAULT_ERROR_CODE_READ |
				PAGE_FAULT_ERROR_CODE_SUPERVISOR_MODE |
					PAGE_FAULT_ERROR_CODE_NOT_PRESENT_FAULT;
		registers->r_cr2 = linearAddress;
		ReturnImmediately();
	}

	Dword pte = ReadPTE(
		(pde & 0xfffff000) | ((linearAddress & 0x003ff000) >> 10));

	if(UNLIKELY((pte & PTE_PRESENT_BIT) == 0))
	{
		registers->exception = EXCEPTION_PAGE_FAULT;
		registers->errorCode =
			PAGE_FAULT_ERROR_CODE_READ |
				PAGE_FAULT_ERROR_CODE_SUPERVISOR_MODE |
					PAGE_FAULT_ERROR_CODE_NOT_PRESENT_FAULT;
		registers->r_cr2 = linearAddress;
		ReturnImmediately();
	}

	if(UNLIKELY((pde & PDE_ACCESSED_BIT) == 0))
	{
		WritePTE(	(registers->r_cr3 & 0xfffff000) | ((linearAddress & 0xffc00000) >> 20),
					pde | PDE_ACCESSED_BIT);
	}
	
	if(UNLIKELY((pte & PTE_ACCESSED_BIT) == 0))
	{
		WritePTE(	(pde & 0xfffff000) | ((linearAddress & 0x003ff000) >> 10),
					pte | PTE_ACCESSED_BIT);
	}

	pte &= registers->addressMask;
    
	tlbEntry->linearPageAndFlags  = linearAddress & TLB_ENTRY_LINEAR_PAGE_MASK;

	if(LIKELY((pte & 0xfffff000) < registers->memorySize))
	{
		//If the physical page is marked as MMIO, set the MMIO flag
		if(registers->pageClassifications[pte >> 12] == PAGE_CLASS_MMIO)
			tlbEntry->linearPageAndFlags |= TLB_ENTRY_MMIO_FLAG;

		tlbEntry->linToHostOffset =
			registers->memoryPtr + (pte & 0xfffff000) - (linearAddress & 0xfffff000);
	}
	else
	{
		//If the physical address is beyond the end of memory, use the first extra page
		tlbEntry->linToHostOffset =
			registers->memoryPtr + registers->memorySize - (linearAddress & 0xfffff000);
	}
}

static void TranslateAddressForReadUserMode(Dword linearAddress, struct TlbEntry * tlbEntry)
{
	if((registers->r_cr0 & CR0_PG_FLAG) == 0)
	{
		TranslateAddressForReadPagingDisabled(linearAddress, tlbEntry);
		return;
	}

	Dword pde = ReadPTE(
		(registers->r_cr3 & 0xfffff000) | ((linearAddress & 0xffc00000) >> 20));

	if(UNLIKELY((pde & PDE_PRESENT_BIT) == 0 || (pde & PDE_USER_BIT) == 0))
	{
		registers->exception = EXCEPTION_PAGE_FAULT;
		registers->errorCode =
			PAGE_FAULT_ERROR_CODE_READ |
				PAGE_FAULT_ERROR_CODE_USER_MODE;
		registers->errorCode |= ((pde & PDE_PRESENT_BIT) == 0)
			? PAGE_FAULT_ERROR_CODE_NOT_PRESENT_FAULT
			: PAGE_FAULT_ERROR_CODE_PROTECTION_FAULT;
		registers->r_cr2 = linearAddress;
		ReturnImmediately();
	}

	Dword pte = ReadPTE(
		(pde & 0xfffff000) | ((linearAddress & 0x003ff000) >> 10));

	if(UNLIKELY((pte & PTE_PRESENT_BIT) == 0 || (pte & PTE_USER_BIT) == 0))
	{
		registers->exception = EXCEPTION_PAGE_FAULT;
		registers->errorCode =
			PAGE_FAULT_ERROR_CODE_READ |
				PAGE_FAULT_ERROR_CODE_USER_MODE;
		registers->errorCode |= ((pte & PTE_PRESENT_BIT) == 0)
			? PAGE_FAULT_ERROR_CODE_NOT_PRESENT_FAULT
			: PAGE_FAULT_ERROR_CODE_PROTECTION_FAULT;
		registers->r_cr2 = linearAddress;
		ReturnImmediately();
	}

	if(UNLIKELY((pde & PDE_ACCESSED_BIT) == 0))
	{
		WritePTE(	(registers->r_cr3 & 0xfffff000) | ((linearAddress & 0xffc00000) >> 20),
					pde | PDE_ACCESSED_BIT);
	}
	
	if(UNLIKELY((pte & PTE_ACCESSED_BIT) == 0))
	{
		WritePTE(	(pde & 0xfffff000) | ((linearAddress & 0x003ff000) >> 10),
					pte | PTE_ACCESSED_BIT);
	}

	pte &= registers->addressMask;
    
	tlbEntry->linearPageAndFlags  = linearAddress & TLB_ENTRY_LINEAR_PAGE_MASK;

	if(LIKELY((pte & 0xfffff000) < registers->memorySize))
	{
		//If the physical page is marked as MMIO, set the MMIO flag
		if(registers->pageClassifications[pte >> 12] == PAGE_CLASS_MMIO)
			tlbEntry->linearPageAndFlags |= TLB_ENTRY_MMIO_FLAG;

		tlbEntry->linToHostOffset =
			registers->memoryPtr + (pte & 0xfffff000) - (linearAddress & 0xfffff000);
	}
	else
	{
		//If the physical address is beyond the end of memory, use the first extra page
		tlbEntry->linToHostOffset =
			registers->memoryPtr + registers->memorySize - (linearAddress & 0xfffff000);
	}
}

static inline void TranslateAddressForWritePagingDisabled(	Dword linearAddress,
															struct TlbEntry * tlbEntry,
															struct TlbEntry * readTlbEntry)
{
	//Calculate the physical page (the contents of the PTE, if there were one)
	Dword pte = linearAddress & registers->addressMask;

	tlbEntry->linearPageAndFlags  = linearAddress & TLB_ENTRY_LINEAR_PAGE_MASK;

	if(LIKELY((pte & 0xfffff000) < registers->memorySize))
	{
		//If the physical page is marked as containing translations, set the flag
		if(registers->pageClassifications[pte >> 12] == PAGE_CLASS_CONTAINS_TRANSLATIONS)
			tlbEntry->linearPageAndFlags |= TLB_ENTRY_CONTAINS_TRANSLATIONS_FLAG;

		//If the physical page is marked as MMIO, set the MMIO flag
		if(registers->pageClassifications[pte >> 12] == PAGE_CLASS_MMIO)
			tlbEntry->linearPageAndFlags |= TLB_ENTRY_MMIO_FLAG;

		tlbEntry->linToHostOffset =
			registers->memoryPtr + (pte & 0xfffff000) - (linearAddress & 0xfffff000);
	}
	else
	{
		//If the physical address is beyond the end of memory, use the second extra page
		tlbEntry->linToHostOffset =
			registers->memoryPtr + (registers->memorySize + 0x1000) - (linearAddress & 0xfffff000);
	}

	readTlbEntry->linearPageAndFlags =
		tlbEntry->linearPageAndFlags & ~TLB_ENTRY_CONTAINS_TRANSLATIONS_FLAG;
	readTlbEntry->linToHostOffset = tlbEntry->linToHostOffset;
}

static void TranslateAddressForWriteSupervisorMode(Dword linearAddress, struct TlbEntry * tlbEntry)
{
	struct TlbEntry * readTlbEntry = &registers->supervisorReadTlb[(linearAddress >> 12) & TLB_MASK];
	
	if((registers->r_cr0 & CR0_PG_FLAG) == 0)
		return TranslateAddressForWritePagingDisabled(linearAddress, tlbEntry, readTlbEntry);

	Dword pde = ReadPTE(
		(registers->r_cr3 & 0xfffff000) | ((linearAddress & 0xffc00000) >> 20));

	if(UNLIKELY((pde & PDE_PRESENT_BIT) == 0))
	{
		registers->exception = EXCEPTION_PAGE_FAULT;
		registers->errorCode =
			PAGE_FAULT_ERROR_CODE_WRITE |
				PAGE_FAULT_ERROR_CODE_SUPERVISOR_MODE |
					PAGE_FAULT_ERROR_CODE_NOT_PRESENT_FAULT;
		registers->r_cr2 = linearAddress;
		ReturnImmediately();
	}

	Dword pte = ReadPTE(
		(pde & 0xfffff000) | ((linearAddress & 0x003ff000) >> 10));

	if(UNLIKELY((pte & PTE_PRESENT_BIT) == 0))
	{
		registers->exception = EXCEPTION_PAGE_FAULT;
		registers->errorCode =
			PAGE_FAULT_ERROR_CODE_WRITE |
				PAGE_FAULT_ERROR_CODE_SUPERVISOR_MODE |
					PAGE_FAULT_ERROR_CODE_NOT_PRESENT_FAULT;
		registers->r_cr2 = linearAddress;
		ReturnImmediately();
	}

	if(UNLIKELY((pde & PDE_ACCESSED_BIT) == 0))
	{
		WritePTE(	(registers->r_cr3 & 0xfffff000) | ((linearAddress & 0xffc00000) >> 20),
					pde | PDE_ACCESSED_BIT);
	}
	
	if(UNLIKELY((pte & (PTE_ACCESSED_BIT | PTE_DIRTY_BIT)) != (PTE_ACCESSED_BIT | PTE_DIRTY_BIT)))
	{
		WritePTE(	(pde & 0xfffff000) | ((linearAddress & 0x003ff000) >> 10),
					pte | PTE_ACCESSED_BIT | PTE_DIRTY_BIT);
	}

	pte &= registers->addressMask;

	tlbEntry->linearPageAndFlags  = linearAddress & TLB_ENTRY_LINEAR_PAGE_MASK;

	if(LIKELY((pte & 0xfffff000) < registers->memorySize))
	{
		//If the physical page is marked as containing translations, set the flag
		if(registers->pageClassifications[pte >> 12] == PAGE_CLASS_CONTAINS_TRANSLATIONS)
			tlbEntry->linearPageAndFlags |= TLB_ENTRY_CONTAINS_TRANSLATIONS_FLAG;

		//If the physical page is marked as MMIO, set the MMIO flag
		if(registers->pageClassifications[pte >> 12] == PAGE_CLASS_MMIO)
			tlbEntry->linearPageAndFlags |= TLB_ENTRY_MMIO_FLAG;

		tlbEntry->linToHostOffset =
			registers->memoryPtr + (pte & 0xfffff000) - (linearAddress & 0xfffff000);
	}
	else
	{
		//If the physical address is beyond the end of memory, use the second extra page
		tlbEntry->linToHostOffset =
			registers->memoryPtr + (registers->memorySize + 0x1000) - (linearAddress & 0xfffff000);
	}

	readTlbEntry->linearPageAndFlags =
		tlbEntry->linearPageAndFlags & ~TLB_ENTRY_CONTAINS_TRANSLATIONS_FLAG;
	readTlbEntry->linToHostOffset = tlbEntry->linToHostOffset;
}

static void TranslateAddressForWriteSupervisorModeWriteProtect(Dword linearAddress, struct TlbEntry * tlbEntry)
{
	struct TlbEntry * readTlbEntry = &registers->userReadTlb[(linearAddress >> 12) & TLB_MASK];

	if((registers->r_cr0 & CR0_PG_FLAG) == 0)
		return TranslateAddressForWritePagingDisabled(linearAddress, tlbEntry, readTlbEntry);

	Dword pde = ReadPTE(
		(registers->r_cr3 & 0xfffff000) | ((linearAddress & 0xffc00000) >> 20));

	if(UNLIKELY((pde & PDE_PRESENT_BIT) == 0 || (pde & PDE_WRITABLE_BIT) == 0))
	{
		registers->exception = EXCEPTION_PAGE_FAULT;
		registers->errorCode =
			PAGE_FAULT_ERROR_CODE_WRITE |
				PAGE_FAULT_ERROR_CODE_SUPERVISOR_MODE;
		registers->errorCode |= ((pde & PDE_PRESENT_BIT) == 0)
			? PAGE_FAULT_ERROR_CODE_NOT_PRESENT_FAULT
			: PAGE_FAULT_ERROR_CODE_PROTECTION_FAULT;
		registers->r_cr2 = linearAddress;
		ReturnImmediately();
	}

	Dword pte = ReadPTE(
		(pde & 0xfffff000) | ((linearAddress & 0x003ff000) >> 10));

	if(UNLIKELY((pte & PTE_PRESENT_BIT) == 0 || (pte & PTE_WRITABLE_BIT) == 0))
	{
		registers->exception = EXCEPTION_PAGE_FAULT;
		registers->errorCode =
			PAGE_FAULT_ERROR_CODE_WRITE |
				PAGE_FAULT_ERROR_CODE_SUPERVISOR_MODE;
		registers->errorCode |= ((pte & PTE_PRESENT_BIT) == 0)
			? PAGE_FAULT_ERROR_CODE_NOT_PRESENT_FAULT
			: PAGE_FAULT_ERROR_CODE_PROTECTION_FAULT;
		registers->r_cr2 = linearAddress;
		ReturnImmediately();
	}

	if(UNLIKELY((pde & PDE_ACCESSED_BIT) == 0))
	{
		WritePTE(	(registers->r_cr3 & 0xfffff000) | ((linearAddress & 0xffc00000) >> 20),
					pde | PDE_ACCESSED_BIT);
	}
	
	if(UNLIKELY((pte & (PTE_ACCESSED_BIT | PTE_DIRTY_BIT)) != (PTE_ACCESSED_BIT | PTE_DIRTY_BIT)))
	{
		WritePTE(	(pde & 0xfffff000) | ((linearAddress & 0x003ff000) >> 10),
					pte | PTE_ACCESSED_BIT | PTE_DIRTY_BIT);
	}

	pte &= registers->addressMask;

	tlbEntry->linearPageAndFlags  = linearAddress & TLB_ENTRY_LINEAR_PAGE_MASK;

	if(LIKELY((pte & 0xfffff000) < registers->memorySize))
	{
		//If the physical page is marked as containing translations, set the flag
		if(registers->pageClassifications[pte >> 12] == PAGE_CLASS_CONTAINS_TRANSLATIONS)
			tlbEntry->linearPageAndFlags |= TLB_ENTRY_CONTAINS_TRANSLATIONS_FLAG;

		//If the physical page is marked as MMIO, set the MMIO flag
		if(registers->pageClassifications[pte >> 12] == PAGE_CLASS_MMIO)
			tlbEntry->linearPageAndFlags |= TLB_ENTRY_MMIO_FLAG;

		tlbEntry->linToHostOffset =
			registers->memoryPtr + (pte & 0xfffff000) - (linearAddress & 0xfffff000);
	}
	else
	{
		//If the physical address is beyond the end of memory, use the second extra page
		tlbEntry->linToHostOffset =
			registers->memoryPtr + (registers->memorySize + 0x1000) - (linearAddress & 0xfffff000);
	}

	readTlbEntry->linearPageAndFlags =
		tlbEntry->linearPageAndFlags & ~TLB_ENTRY_CONTAINS_TRANSLATIONS_FLAG;
	readTlbEntry->linToHostOffset = tlbEntry->linToHostOffset;
}

static void TranslateAddressForWriteUserMode(Dword linearAddress, struct TlbEntry * tlbEntry)
{
	struct TlbEntry * readTlbEntry = &registers->userReadTlb[(linearAddress >> 12) & TLB_MASK];

	if((registers->r_cr0 & CR0_PG_FLAG) == 0)
		return TranslateAddressForWritePagingDisabled(linearAddress, tlbEntry, readTlbEntry);

	Dword pde = ReadPTE(
		(registers->r_cr3 & 0xfffff000) | ((linearAddress & 0xffc00000) >> 20));

	if(UNLIKELY((pde & PDE_PRESENT_BIT) == 0 ||
				(pde & PDE_WRITABLE_BIT) == 0 ||
				(pde & PDE_USER_BIT) == 0))
	{
		registers->exception = EXCEPTION_PAGE_FAULT;
		registers->errorCode =
			PAGE_FAULT_ERROR_CODE_WRITE |
				PAGE_FAULT_ERROR_CODE_USER_MODE;
		registers->errorCode |= ((pde & PDE_PRESENT_BIT) == 0)
			? PAGE_FAULT_ERROR_CODE_NOT_PRESENT_FAULT
			: PAGE_FAULT_ERROR_CODE_PROTECTION_FAULT;
		registers->r_cr2 = linearAddress;
		ReturnImmediately();
	}

	Dword pte = ReadPTE(
		(pde & 0xfffff000) | ((linearAddress & 0x003ff000) >> 10));

	if(UNLIKELY((pte & PTE_PRESENT_BIT) == 0 ||
				(pte & PTE_WRITABLE_BIT) == 0 ||
				(pte & PTE_USER_BIT) == 0))
	{
		registers->exception = EXCEPTION_PAGE_FAULT;
		registers->errorCode =
			PAGE_FAULT_ERROR_CODE_WRITE |
				PAGE_FAULT_ERROR_CODE_USER_MODE;
		registers->errorCode |= ((pte & PTE_PRESENT_BIT) == 0)
			? PAGE_FAULT_ERROR_CODE_NOT_PRESENT_FAULT
			: PAGE_FAULT_ERROR_CODE_PROTECTION_FAULT;
		registers->r_cr2 = linearAddress;
		ReturnImmediately();
	}

	if(UNLIKELY((pde & PDE_ACCESSED_BIT) == 0))
	{
		WritePTE(	(registers->r_cr3 & 0xfffff000) | ((linearAddress & 0xffc00000) >> 20),
					pde | PDE_ACCESSED_BIT);
	}
	
	if(UNLIKELY((pte & (PTE_ACCESSED_BIT | PTE_DIRTY_BIT)) != (PTE_ACCESSED_BIT | PTE_DIRTY_BIT)))
	{
		WritePTE(	(pde & 0xfffff000) | ((linearAddress & 0x003ff000) >> 10),
					pte | PTE_ACCESSED_BIT | PTE_DIRTY_BIT);
	}

	pte &= registers->addressMask;

	tlbEntry->linearPageAndFlags  = linearAddress & TLB_ENTRY_LINEAR_PAGE_MASK;

	if(LIKELY((pte & 0xfffff000) < registers->memorySize))
	{
		//If the physical page is marked as containing translations, set the flag
		if(registers->pageClassifications[pte >> 12] == PAGE_CLASS_CONTAINS_TRANSLATIONS)
			tlbEntry->linearPageAndFlags |= TLB_ENTRY_CONTAINS_TRANSLATIONS_FLAG;

		//If the physical page is marked as MMIO, set the MMIO flag
		if(registers->pageClassifications[pte >> 12] == PAGE_CLASS_MMIO)
			tlbEntry->linearPageAndFlags |= TLB_ENTRY_MMIO_FLAG;

		tlbEntry->linToHostOffset =
			registers->memoryPtr + (pte & 0xfffff000) - (linearAddress & 0xfffff000);
	}
	else
	{
		//If the physical address is beyond the end of memory, use the second extra page
		tlbEntry->linToHostOffset =
			registers->memoryPtr + (registers->memorySize + 0x1000) - (linearAddress & 0xfffff000);
	}

	readTlbEntry->linearPageAndFlags =
		tlbEntry->linearPageAndFlags & ~TLB_ENTRY_CONTAINS_TRANSLATIONS_FLAG;
	readTlbEntry->linToHostOffset = tlbEntry->linToHostOffset;
}

//Memory access
#define READ_BYTE_TLB_HIT																\
	if(LIKELY(	tlbEntry->linearPageAndFlags ==											\
					(address & TLB_ENTRY_LINEAR_PAGE_MASK)))							\
	{																					\
		/*TLB hit*/																		\
		return *(Byte *)(address + tlbEntry->linToHostOffset);							\
	}

#define READ_BYTE_TLB_HIT_MMIO															\
	if(tlbEntry->linearPageAndFlags ==													\
				((address & TLB_ENTRY_LINEAR_PAGE_MASK) | TLB_ENTRY_MMIO_FLAG))			\
	{																					\
		/*TLB hit, MMIO*/																\
		return CallRegistersReadBytePhysical(											\
			address + tlbEntry->linToHostOffset - registers->memoryPtr);				\
	}

#define READ_BYTE(modeString, tlbString)												\
	asm																					\
	(																					\
		"mov    %ecx,%edx												\n"				\
		"shr    $0x9,%edx												\n"				\
		"mov    %ecx,%eax												\n"				\
		"and    $0x7f8,%edx												\n"				\
		"and    $0xfffff000,%eax										\n"				\
tlbString"Patch1a:														\n"				\
		"cmp    %eax,0x80000000(%edx)									\n"				\
		"jne    @ReadByte"modeString"Slow@8								\n"				\
tlbString"Patch1b:														\n"				\
		"mov    0x80000004(%edx),%edx									\n"				\
		"movzbl (%ecx,%edx,1),%eax										\n"				\
		"ret															\n"				\
	);

Dword __fastcall ReadByteSupervisorModeSlow(Dword address, Dword tlbIndexTimes8) __attribute__((noinline));
Dword __fastcall ReadByteSupervisorModeSlow(Dword address, Dword tlbIndexTimes8)
{
	struct TlbEntry * tlbEntry = &registers->supervisorReadTlb[tlbIndexTimes8 >> 3];

	READ_BYTE_TLB_HIT_MMIO
	TranslateAddressForReadSupervisorMode(address, tlbEntry);
	READ_BYTE_TLB_HIT
	READ_BYTE_TLB_HIT_MMIO
}

Dword __fastcall ReadByteSupervisorMode(Dword address) __attribute__((noinline));
Dword __fastcall ReadByteSupervisorMode(Dword address)
{
	READ_BYTE("SupervisorMode", "supervisorReadTlb");
}

Dword __fastcall ReadByteUserModeSlow(Dword address, Dword tlbIndexTimes8) __attribute__((noinline));
Dword __fastcall ReadByteUserModeSlow(Dword address, Dword tlbIndexTimes8)
{
	struct TlbEntry * tlbEntry = &registers->userReadTlb[tlbIndexTimes8 >> 3];

	READ_BYTE_TLB_HIT_MMIO
	TranslateAddressForReadUserMode(address, tlbEntry);
	READ_BYTE_TLB_HIT
	READ_BYTE_TLB_HIT_MMIO
}

Dword __fastcall ReadByteUserMode(Dword address) __attribute__((noinline));
Dword __fastcall ReadByteUserMode(Dword address)
{
	READ_BYTE("UserMode", "userReadTlb");
}

#define READ_WORD_TLB_HIT																\
	if(LIKELY(	tlbEntry->linearPageAndFlags ==											\
					(address & TLB_ENTRY_LINEAR_PAGE_MASK)))							\
	{																					\
		/*TLB hit*/																		\
		return *(Word *)(address + tlbEntry->linToHostOffset);							\
	}

#define READ_WORD_TLB_HIT_MMIO															\
	if(tlbEntry->linearPageAndFlags ==													\
				((address & TLB_ENTRY_LINEAR_PAGE_MASK) | TLB_ENTRY_MMIO_FLAG))			\
	{																					\
		/*TLB hit, MMIO*/																\
		return CallRegistersReadWordPhysical(											\
			address + tlbEntry->linToHostOffset - registers->memoryPtr);				\
	}

#define READ_WORD(modeString, tlbString)												\
	asm																					\
	(																					\
		"mov    %ecx,%eax												\n"				\
		"and    $0xfff,%eax												\n"				\
		"cmp    $0xfff,%eax												\n"				\
		"je     1f														\n"				\
		"mov    %ecx,%edx												\n"				\
		"shr    $0x9,%edx												\n"				\
		"mov    %ecx,%eax												\n"				\
		"and    $0x7f8,%edx												\n"				\
		"and    $0xfffff000,%eax										\n"				\
tlbString"Patch2a:														\n"				\
		"cmp    %eax,0x80000000(%edx)									\n"				\
		"jne    @ReadWord"modeString"Slow@8								\n"				\
tlbString"Patch2b:														\n"				\
		"mov    0x80000004(%edx),%edx									\n"				\
		"movzwl (%ecx,%edx,1),%eax										\n"				\
		"ret															\n"				\
																						\
"1:																		\n"				\
		"push   %ebx													\n"				\
		"mov    %ecx, %ebx												\n"				\
		"call   @ReadByte"modeString"@4									\n"				\
		"lea    0x1(%ebx),%ecx											\n"				\
		"mov    %al,%bl													\n"				\
		"call   @ReadByte"modeString"@4									\n"				\
		"movzbl %bl,%ecx												\n"				\
		"shl    $0x8,%eax												\n"				\
		"and    $0xff00,%eax											\n"				\
		"or     %ecx,%eax												\n"				\
		"pop    %ebx													\n"				\
		"ret															\n"				\
	);

Dword __fastcall ReadWordSupervisorModeSlow(Dword address, Dword tlbIndexTimes8) __attribute__((noinline));
Dword __fastcall ReadWordSupervisorModeSlow(Dword address, Dword tlbIndexTimes8)
{
	struct TlbEntry * tlbEntry = &registers->supervisorReadTlb[tlbIndexTimes8 >> 3];

	READ_WORD_TLB_HIT_MMIO
	TranslateAddressForReadSupervisorMode(address, tlbEntry);
	READ_WORD_TLB_HIT
	READ_WORD_TLB_HIT_MMIO
}

Dword __fastcall ReadWordSupervisorMode(Dword address) __attribute__((noinline));
Dword __fastcall ReadWordSupervisorMode(Dword address)
{
	READ_WORD("SupervisorMode", "supervisorReadTlb");
}

Dword __fastcall ReadWordUserModeSlow(Dword address, Dword tlbIndexTimes8) __attribute__((noinline));
Dword __fastcall ReadWordUserModeSlow(Dword address, Dword tlbIndexTimes8)
{
	struct TlbEntry * tlbEntry = &registers->userReadTlb[tlbIndexTimes8 >> 3];

	READ_WORD_TLB_HIT_MMIO
	TranslateAddressForReadUserMode(address, tlbEntry);
	READ_WORD_TLB_HIT
	READ_WORD_TLB_HIT_MMIO
}

Dword __fastcall ReadWordUserMode(Dword address) __attribute__((noinline));
Dword __fastcall ReadWordUserMode(Dword address)
{
	READ_WORD("UserMode", "userReadTlb");
}

#define READ_DWORD_TLB_HIT																\
	if(LIKELY(	tlbEntry->linearPageAndFlags ==											\
					(address & TLB_ENTRY_LINEAR_PAGE_MASK)))							\
	{																					\
		/*TLB hit*/																		\
		return *(Dword *)(address + tlbEntry->linToHostOffset);							\
	}

#define READ_DWORD_TLB_HIT_MMIO															\
	if(tlbEntry->linearPageAndFlags ==													\
				((address & TLB_ENTRY_LINEAR_PAGE_MASK) | TLB_ENTRY_MMIO_FLAG))			\
	{																					\
		/*TLB hit, MMIO*/																\
		return CallRegistersReadDwordPhysical(											\
			address + tlbEntry->linToHostOffset - registers->memoryPtr);				\
	}

#define READ_DWORD(modeString, tlbString)												\
	asm																					\
	(																					\
		"mov    %ecx,%eax												\n"				\
		"and    $0xfff,%eax												\n"				\
		"cmp    $0xffc,%eax												\n"				\
		"ja     1f														\n"				\
		"mov    %ecx,%edx												\n"				\
		"shr    $0x9,%edx												\n"				\
		"mov    %ecx,%eax												\n"				\
		"and    $0x7f8,%edx												\n"				\
		"and    $0xfffff000,%eax										\n"				\
tlbString"Patch3a:														\n"				\
		"cmp    %eax,0x80000000(%edx)									\n"				\
		"jne    @ReadDword"modeString"Slow@8							\n"				\
tlbString"Patch3b:														\n"				\
		"mov    0x80000004(%edx),%edx									\n"				\
		"mov    (%ecx,%edx,1),%eax										\n"				\
		"ret															\n"				\
																						\
"1:																		\n"				\
		"push   %ebx													\n"				\
		"mov    %ecx, %ebx												\n"				\
		"call   @ReadWord"modeString"@4									\n"				\
		"lea    0x2(%ebx),%ecx											\n"				\
		"movzwl %ax,%ebx												\n"				\
		"call   @ReadWord"modeString"@4									\n"				\
		"shl    $0x10,%eax												\n"				\
		"or     %ebx,%eax												\n"				\
		"pop    %ebx													\n"				\
		"ret															\n"				\
	);

Dword __fastcall ReadDwordSupervisorModeSlow(Dword address, Dword tlbIndexTimes8) __attribute__((noinline));
Dword __fastcall ReadDwordSupervisorModeSlow(Dword address, Dword tlbIndexTimes8)
{
	struct TlbEntry * tlbEntry = &registers->supervisorReadTlb[tlbIndexTimes8 >> 3];

	READ_DWORD_TLB_HIT_MMIO
	TranslateAddressForReadSupervisorMode(address, tlbEntry);
	READ_DWORD_TLB_HIT
	READ_DWORD_TLB_HIT_MMIO
}

Dword __fastcall ReadDwordSupervisorMode(Dword address) __attribute__((noinline));
Dword __fastcall ReadDwordSupervisorMode(Dword address)
{
	READ_DWORD("SupervisorMode", "supervisorReadTlb");
}

Dword __fastcall ReadDwordUserModeSlow(Dword address, Dword tlbIndexTimes8) __attribute__((noinline));
Dword __fastcall ReadDwordUserModeSlow(Dword address, Dword tlbIndexTimes8)
{
	struct TlbEntry * tlbEntry = &registers->userReadTlb[tlbIndexTimes8 >> 3];

	READ_DWORD_TLB_HIT_MMIO
	TranslateAddressForReadUserMode(address, tlbEntry);
	READ_DWORD_TLB_HIT
	READ_DWORD_TLB_HIT_MMIO
}

Dword __fastcall ReadDwordUserMode(Dword address) __attribute__((noinline));
Dword __fastcall ReadDwordUserMode(Dword address)
{
	READ_DWORD("UserMode", "userReadTlb");
}

#define WRITE_BYTE_TLB_HIT																\
	if(LIKELY(tlbEntry->linearPageAndFlags == (address & TLB_ENTRY_LINEAR_PAGE_MASK)))	\
	{																					\
		/*TLB hit*/																		\
		*(Byte *)(address + tlbEntry->linToHostOffset) = data;							\
		return;																			\
	}

#define WRITE_BYTE_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO									\
	if(tlbEntry->linearPageAndFlags ==													\
				((address & TLB_ENTRY_LINEAR_PAGE_MASK) | TLB_ENTRY_CONTAINS_TRANSLATIONS_FLAG))	\
	{																					\
		/*TLB hit, contains translations*/												\
		address += tlbEntry->linToHostOffset - registers->memoryPtr;					\
		CallRegistersInvalidateCodeTranslations(address, address + 1, data);			\
		*(Byte *)(registers->memoryPtr + address) = data;								\
		return;																			\
	}																					\
	if(tlbEntry->linearPageAndFlags ==													\
				((address & TLB_ENTRY_LINEAR_PAGE_MASK) | TLB_ENTRY_MMIO_FLAG))			\
	{																					\
		/*TLB hit, MMIO*/																\
		CallRegistersWriteBytePhysical(													\
			address + tlbEntry->linToHostOffset - registers->memoryPtr, data);			\
		return;																			\
	}

#define WRITE_BYTE(modeString, tlbString)												\
	asm																					\
	(																					\
		"push   %ebx													\n"				\
		"mov    %ecx,%ebx												\n"				\
		"shr    $0x9,%ebx												\n"				\
		"and    $0x7f8,%ebx												\n"				\
		"mov    %ecx,%eax												\n"				\
		"and    $0xfffff000,%eax										\n"				\
tlbString"Patch4a:														\n"				\
		"cmp    %eax,0x80000000(%ebx)									\n"				\
		"jne    1f														\n"				\
tlbString"Patch4b:														\n"				\
		"mov    0x80000004(%ebx),%eax									\n"				\
		"mov    %dl,(%ecx,%eax,1)										\n"				\
		"pop    %ebx													\n"				\
		"ret															\n"				\
"1:																		\n"				\
		"pop    %ebx													\n"				\
		"jmp    @WriteByte"modeString"Slow@8							\n"				\
	);

void __fastcall WriteByteSupervisorModeSlow(Dword address, Byte data) __attribute__((noinline));
void __fastcall WriteByteSupervisorModeSlow(Dword address, Byte data)
{
	struct TlbEntry * tlbEntry = &registers->supervisorWriteTlb[(address >> 12) & TLB_MASK];

	WRITE_BYTE_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
	TranslateAddressForWriteSupervisorMode(address, tlbEntry);
	WRITE_BYTE_TLB_HIT
	WRITE_BYTE_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
}

void __fastcall WriteByteSupervisorMode(Dword address, Dword data_) __attribute__((noinline));
void __fastcall WriteByteSupervisorMode(Dword address, Dword data_)
{
	WRITE_BYTE("SupervisorMode", "supervisorWriteTlb");
}

void __fastcall WriteByteSupervisorModeWriteProtectSlow(Dword address, Byte data) __attribute__((noinline));
void __fastcall WriteByteSupervisorModeWriteProtectSlow(Dword address, Byte data)
{
	struct TlbEntry * tlbEntry = &registers->supervisorWriteWPTlb[(address >> 12) & TLB_MASK];

	WRITE_BYTE_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
	TranslateAddressForWriteSupervisorModeWriteProtect(address, tlbEntry);
	WRITE_BYTE_TLB_HIT
	WRITE_BYTE_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
}

void __fastcall WriteByteSupervisorModeWriteProtect(Dword address, Dword data_) __attribute__((noinline));
void __fastcall WriteByteSupervisorModeWriteProtect(Dword address, Dword data_)
{
	WRITE_BYTE("SupervisorModeWriteProtect", "supervisorWriteWPTlb");
}

void __fastcall WriteByteUserModeSlow(Dword address, Byte data) __attribute__((noinline));
void __fastcall WriteByteUserModeSlow(Dword address, Byte data)
{
	struct TlbEntry * tlbEntry = &registers->userWriteTlb[(address >> 12) & TLB_MASK];

	WRITE_BYTE_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
	TranslateAddressForWriteUserMode(address, tlbEntry);
	WRITE_BYTE_TLB_HIT
	WRITE_BYTE_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
}

void __fastcall WriteByteUserMode(Dword address, Dword data_) __attribute__((noinline));
void __fastcall WriteByteUserMode(Dword address, Dword data_)
{
	WRITE_BYTE("UserMode", "userWriteTlb");
}

#define WRITE_WORD_TLB_HIT																\
	if(LIKELY(tlbEntry->linearPageAndFlags == (address & TLB_ENTRY_LINEAR_PAGE_MASK)))	\
	{																					\
		/*TLB hit*/																		\
		*(Word *)(address + tlbEntry->linToHostOffset) = data;							\
		return;																			\
	}

#define WRITE_WORD_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO									\
	if(tlbEntry->linearPageAndFlags ==													\
				((address & TLB_ENTRY_LINEAR_PAGE_MASK) | TLB_ENTRY_CONTAINS_TRANSLATIONS_FLAG))	\
	{																					\
		/*TLB hit, contains translations*/												\
		address += tlbEntry->linToHostOffset - registers->memoryPtr;					\
		CallRegistersInvalidateCodeTranslations(address, address + 2, data);			\
		*(Word *)(registers->memoryPtr + address) = data;								\
		return;																			\
	}																					\
	if(tlbEntry->linearPageAndFlags ==													\
			((address & TLB_ENTRY_LINEAR_PAGE_MASK) | TLB_ENTRY_MMIO_FLAG))				\
	{																					\
		/*TLB hit, MMIO*/																\
		CallRegistersWriteWordPhysical(													\
			address + tlbEntry->linToHostOffset - registers->memoryPtr, data);			\
		return;																			\
	}

#define WRITE_WORD(modeString, tlbString)												\
	asm																					\
	(																					\
		"mov    %ecx,%eax												\n"				\
		"sub    $0x8,%esp												\n"				\
		"and    $0xfff,%eax												\n"				\
		"mov    %ebx,(%esp)												\n"				\
		"cmp    $0xfff,%eax												\n"				\
		"mov    %ecx,%ebx												\n"				\
		"mov    %esi,0x4(%esp)											\n"				\
		"movzwl %dx,%esi												\n"				\
		"je     1f														\n"				\
		"mov    %ecx,%edx												\n"				\
		"shr    $0x9,%edx												\n"				\
		"and    $0x7f8,%edx												\n"				\
		"and    $0xfffff000,%ecx										\n"				\
tlbString"Patch5a:														\n"				\
		"cmp    %ecx,0x80000000(%edx)									\n"				\
		"jne    2f														\n"				\
tlbString"Patch5b:														\n"				\
		"mov    0x80000004(%edx),%ecx									\n"				\
		"mov    %si,(%ebx,%ecx,1)										\n"				\
		"mov    (%esp),%ebx												\n"				\
		"mov    0x4(%esp),%esi											\n"				\
		"add    $0x8,%esp												\n"				\
		"ret															\n"				\
"1:																		\n"				\
		"mov    %esi,%eax												\n"				\
		"movzbl %al,%edx												\n"				\
		"call   @WriteByte"modeString"@8								\n"				\
		"mov    %esi,%ecx												\n"				\
		"shr    $0x8,%ecx												\n"				\
		"mov    0x4(%esp),%esi											\n"				\
		"movzbl %cl,%edx												\n"				\
		"lea    0x1(%ebx),%ecx											\n"				\
		"mov    (%esp),%ebx												\n"				\
		"add    $0x8,%esp												\n"				\
		"jmp    @WriteByte"modeString"@8								\n"				\
"2:																		\n"				\
		"mov    %esi,%edx												\n"				\
		"mov    %ebx,%ecx												\n"				\
		"mov    0x4(%esp),%esi											\n"				\
		"mov    (%esp),%ebx												\n"				\
		"add    $0x8,%esp												\n"				\
		"jmp    @WriteWord"modeString"Slow@8							\n"				\
	);

void __fastcall WriteWordSupervisorModeSlow(Dword address, Word data) __attribute__((noinline));
void __fastcall WriteWordSupervisorModeSlow(Dword address, Word data)
{
	struct TlbEntry * tlbEntry = &registers->supervisorWriteTlb[(address >> 12) & TLB_MASK];

	WRITE_WORD_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
	TranslateAddressForWriteSupervisorMode(address, tlbEntry);
	WRITE_WORD_TLB_HIT
	WRITE_WORD_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
}

void __fastcall WriteWordSupervisorMode(Dword address, Dword data_) __attribute__((noinline));
void __fastcall WriteWordSupervisorMode(Dword address, Dword data_)
{
	WRITE_WORD("SupervisorMode", "supervisorWriteTlb");
}

void __fastcall WriteWordSupervisorModeWriteProtectSlow(Dword address, Word data) __attribute__((noinline));
void __fastcall WriteWordSupervisorModeWriteProtectSlow(Dword address, Word data) 
{
	struct TlbEntry * tlbEntry = &registers->supervisorWriteWPTlb[(address >> 12) & TLB_MASK];

	WRITE_WORD_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
	TranslateAddressForWriteSupervisorModeWriteProtect(address, tlbEntry);
	WRITE_WORD_TLB_HIT
	WRITE_WORD_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
}

void __fastcall WriteWordSupervisorModeWriteProtect(Dword address, Dword data_) __attribute__((noinline));
void __fastcall WriteWordSupervisorModeWriteProtect(Dword address, Dword data_)
{
	WRITE_WORD("SupervisorModeWriteProtect", "supervisorWriteWPTlb");
}

void __fastcall WriteWordUserModeSlow(Dword address, Word data) __attribute__((noinline));
void __fastcall WriteWordUserModeSlow(Dword address, Word data)
{
	struct TlbEntry * tlbEntry = &registers->userWriteTlb[(address >> 12) & TLB_MASK];

	WRITE_WORD_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
	TranslateAddressForWriteUserMode(address, tlbEntry);
	WRITE_WORD_TLB_HIT
	WRITE_WORD_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
}

void __fastcall WriteWordUserMode(Dword address, Dword data_) __attribute__((noinline));
void __fastcall WriteWordUserMode(Dword address, Dword data_)
{
	WRITE_WORD("UserMode", "userWriteTlb");
}

#define WRITE_DWORD_TLB_HIT																\
	if(LIKELY(tlbEntry->linearPageAndFlags == (address & TLB_ENTRY_LINEAR_PAGE_MASK)))	\
	{																					\
		/*TLB hit*/																		\
		*(Dword *)(address + tlbEntry->linToHostOffset) = data;							\
		return;																			\
	}

#define WRITE_DWORD_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO									\
	if(tlbEntry->linearPageAndFlags ==													\
				((address & TLB_ENTRY_LINEAR_PAGE_MASK) | TLB_ENTRY_CONTAINS_TRANSLATIONS_FLAG))	\
	{																					\
		/*TLB hit, contains translations*/												\
		address += tlbEntry->linToHostOffset - registers->memoryPtr;					\
		CallRegistersInvalidateCodeTranslations(address, address + 4, data);			\
		*(Dword *)(registers->memoryPtr + address) = data;								\
		return;																			\
	}																					\
	if(tlbEntry->linearPageAndFlags ==													\
			((address & TLB_ENTRY_LINEAR_PAGE_MASK) | TLB_ENTRY_MMIO_FLAG))				\
	{																					\
		/*TLB hit, MMIO*/																\
		CallRegistersWriteDwordPhysical(												\
			address + tlbEntry->linToHostOffset - registers->memoryPtr, data);			\
		return;																			\
	}

#define WRITE_DWORD(modeString, tlbString)												\
	asm																					\
	(																					\
		"mov    %ecx,%eax												\n"				\
		"sub    $0x8,%esp												\n"				\
		"and    $0xfff,%eax												\n"				\
		"mov    %ebx,(%esp)												\n"				\
		"cmp    $0xffc,%eax												\n"				\
		"mov    %ecx,%ebx												\n"				\
		"mov    %esi,0x4(%esp)											\n"				\
		"mov    %edx,%esi												\n"				\
		"ja     1f														\n"				\
		"mov    %ecx,%edx												\n"				\
		"shr    $0x9,%edx												\n"				\
		"and    $0x7f8,%edx												\n"				\
		"and    $0xfffff000,%ecx										\n"				\
tlbString"Patch6a:														\n"				\
		"cmp    %ecx,0x80000000(%edx)									\n"				\
		"jne    2f														\n"				\
tlbString"Patch6b:														\n"				\
		"mov    0x80000004(%edx),%ecx									\n"				\
		"mov    %esi,(%ebx,%ecx,1)										\n"				\
		"mov    (%esp),%ebx												\n"				\
		"mov    0x4(%esp),%esi											\n"				\
		"add    $0x8,%esp												\n"				\
		"ret															\n"				\
"1:																		\n"				\
		"movzwl %dx,%edx												\n"				\
		"shr    $0x10,%esi												\n"				\
		"call   @WriteWord"modeString"@8								\n"				\
		"lea    0x2(%ebx),%ecx											\n"				\
		"mov    %esi,%edx												\n"				\
		"mov    (%esp),%ebx												\n"				\
		"mov    0x4(%esp),%esi											\n"				\
		"add    $0x8,%esp												\n"				\
		"jmp    @WriteWord"modeString"@8								\n"				\
"2:																		\n"				\
		"mov    %esi,%edx												\n"				\
		"mov    %ebx,%ecx												\n"				\
		"mov    0x4(%esp),%esi											\n"				\
		"mov    (%esp),%ebx												\n"				\
		"add    $0x8,%esp												\n"				\
		"jmp    @WriteDword"modeString"Slow@8							\n"				\
	);

void __fastcall WriteDwordSupervisorModeSlow(Dword address, Dword data) __attribute__((noinline));
void __fastcall WriteDwordSupervisorModeSlow(Dword address, Dword data)
{
	struct TlbEntry * tlbEntry = &registers->supervisorWriteTlb[(address >> 12) & TLB_MASK];

	WRITE_DWORD_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
	TranslateAddressForWriteSupervisorMode(address, tlbEntry);
	WRITE_DWORD_TLB_HIT
	WRITE_DWORD_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
}

void __fastcall WriteDwordSupervisorMode(Dword address, Dword data) __attribute__((noinline));
void __fastcall WriteDwordSupervisorMode(Dword address, Dword data)
{
	WRITE_DWORD("SupervisorMode", "supervisorWriteTlb");
}

void __fastcall WriteDwordSupervisorModeWriteProtectSlow(Dword address, Dword data) __attribute__((noinline));
void __fastcall WriteDwordSupervisorModeWriteProtectSlow(Dword address, Dword data)
{
	struct TlbEntry * tlbEntry = &registers->supervisorWriteWPTlb[(address >> 12) & TLB_MASK];

	WRITE_DWORD_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
	TranslateAddressForWriteSupervisorModeWriteProtect(address, tlbEntry);
	WRITE_DWORD_TLB_HIT
	WRITE_DWORD_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
}

void __fastcall WriteDwordSupervisorModeWriteProtect(Dword address, Dword data) __attribute__((noinline));
void __fastcall WriteDwordSupervisorModeWriteProtect(Dword address, Dword data)
{
	WRITE_DWORD("SupervisorModeWriteProtect", "supervisorWriteWPTlb");
}

void __fastcall WriteDwordUserModeSlow(Dword address, Dword data) __attribute__((noinline));
void __fastcall WriteDwordUserModeSlow(Dword address, Dword data)
{
	struct TlbEntry * tlbEntry = &registers->userWriteTlb[(address >> 12) & TLB_MASK];

	WRITE_DWORD_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
	TranslateAddressForWriteUserMode(address, tlbEntry);
	WRITE_DWORD_TLB_HIT
	WRITE_DWORD_TLB_HIT_CONTAINS_TRANSLATIONS_MMIO
}

void __fastcall WriteDwordUserMode(Dword address, Dword data) __attribute__((noinline));
void __fastcall WriteDwordUserMode(Dword address, Dword data)
{
	WRITE_DWORD("UserMode", "userWriteTlb");
}

//Set the value of a segment register
static inline void SetDataSegmentRegisterValuePtr(	struct SegmentRegisterData * segReg,
													Dword selector_,
													Dword __fastcall (*ReadDwordFunc)(Dword))
{
	Word selector = selector_;

	if((registers->r_cr0 & CR0_PE_FLAG) == 0)
	{
		//Real mode
		segReg->selector = selector;
		segReg->base = (Dword)selector << 4;
	}
	else if((registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//V8086 mode
		segReg->selector = selector;
		segReg->base = (Dword)selector << 4;
		segReg->limit = 0xffff;
		segReg->descriptorFlags = 0x0000f300;
	}
	else
	{
		//Protected mode

		//Is the selector NULL?
		if(selector < 3)
		{
			//If so, zero all components of the segment register data
			segReg->selector = 0;
			segReg->base = 0;
			segReg->limit = 0;
			segReg->descriptorFlags = 0;
		}
		else
		{
			//Otherwise, get the base of the descriptor table
			Dword tableBase = registers->r_gdtr.base;

			if((selector & SELECTOR_TI_BIT) != 0)
				tableBase = registers->r_ldtr.base;

			//Read the segment descriptor
			Dword descriptorLow = ReadDwordFunc(tableBase + (selector & SELECTOR_INDEX_MASK));
			Dword descriptorHigh = ReadDwordFunc(tableBase + (selector & SELECTOR_INDEX_MASK) + 4);

			//If RPL > DPL and CPL > DPL, #GP(selector)
			if(	(Dword)((selector & SELECTOR_RPL_MASK) >> SELECTOR_RPL_SHIFT) >
					((descriptorHigh & DESCRIPTOR_FLAGS_DPL_MASK) >>
						DESCRIPTOR_FLAGS_DPL_SHIFT) &&
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) >>
						DESCRIPTOR_FLAGS_DPL_SHIFT) >
							((descriptorHigh & DESCRIPTOR_FLAGS_DPL_MASK) >>
								DESCRIPTOR_FLAGS_DPL_SHIFT))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = selector;
				ReturnImmediately();
			}

			//If the descriptor is not present, #NP(selector)
			if((descriptorHigh & DESCRIPTOR_FLAGS_PRESENT_BIT) == 0)
			{
				registers->exception = EXCEPTION_SEGMENT_NOT_PRESENT;
				registers->errorCode = selector;
				ReturnImmediately();
			}
			
			//Save the values
			segReg->selector = selector;
			segReg->base = GetDescriptorBase(descriptorLow, descriptorHigh);
			segReg->limit = GetDescriptorLimit(descriptorLow, descriptorHigh);
			segReg->descriptorFlags = GetDescriptorFlags(descriptorHigh);
		}
	}

	//Update the flags of the currently executing translation
	if(segReg == &registers->r_ds || segReg == &registers->r_es)
	{
		if(	registers->r_cs.base == 0 && registers->r_ds.base == 0 &&
			registers->r_es.base == 0 && registers->r_ss.base == 0)
		{
			registers->currentTranslationFlags |= TRANSLATION_CS_DS_ES_SS_BASES_ZERO_FLAG;
		}
		else
		{
			registers->currentTranslationFlags &= ~TRANSLATION_CS_DS_ES_SS_BASES_ZERO_FLAG;
		}
	}
}

static inline void SetDataSegmentRegisterValue(	Dword reg, Dword selector_,
												Dword __fastcall (*ReadDwordFunc)(Dword))
{
	struct SegmentRegisterData * segReg = 0;

	if(reg == REG_DS)
		segReg = &registers->r_ds;

	else if(reg == REG_ES)
		segReg = &registers->r_es;

	else if(reg == REG_FS)
		segReg = &registers->r_fs;

	else if(reg == REG_GS)
		segReg = &registers->r_gs;

	SetDataSegmentRegisterValuePtr(segReg, selector_, ReadDwordFunc);
}

void SetDataSegmentRegisterValueSupervisorMode(Dword reg, Dword selector_)
{
	SetDataSegmentRegisterValue(reg, selector_, ReadDwordSupervisorMode);
}

void SetDataSegmentRegisterValueUserMode(Dword reg, Dword selector_)
{
	SetDataSegmentRegisterValue(reg, selector_, ReadDwordUserMode);
}

void SetDataSegmentRegisterValuePtrSupervisorMode(struct SegmentRegisterData * segReg, Dword selector_)
{
	SetDataSegmentRegisterValuePtr(segReg, selector_, ReadDwordSupervisorMode);
}

void SetDataSegmentRegisterValuePtrUserMode(struct SegmentRegisterData * segReg, Dword selector_)
{
	SetDataSegmentRegisterValuePtr(segReg, selector_, ReadDwordUserMode);
}

static BOOL CalculatePhysicalAddress(Dword linearAddress, Dword * physicalAddress)
{
	if((registers->r_cr0 & CR0_PG_FLAG) == 0)
	{
		*physicalAddress = linearAddress;
		return TRUE;
	}
	
	//TODO: USER_BIT checking?
	Dword pde = ReadPTE(
		(registers->r_cr3 & 0xfffff000) | ((linearAddress & 0xffc00000) >> 20));

	if(UNLIKELY((pde & PDE_PRESENT_BIT) == 0))
		return FALSE;

	Dword pte = ReadPTE(
		(pde & 0xfffff000) | ((linearAddress & 0x003ff000) >> 10));

	if(UNLIKELY((pte & PTE_PRESENT_BIT) == 0))
		return FALSE;

	*physicalAddress = (pte & 0xfffff000) | (linearAddress & 0x00000fff);

	return TRUE;
}

//Jump directly to the next translation
void JumpToNextTranslation(void)
{
	Dword physicalEip;
	if(UNLIKELY(!CalculatePhysicalAddress(registers->r_cs.base + registers->r_eip, &physicalEip)))
		return;
	
	struct EipToTranslationTLBEntry * tlbEntry =
		&registers->eipToTranslationTLB[physicalEip & EIP_TO_TRANSLATION_TLB_MASK];

	if(LIKELY(	tlbEntry->physicalEip == physicalEip &&
				tlbEntry->flags == registers->currentTranslationFlags))
	{
		registers->currentTranslationPhysicalPage = physicalEip >> 12;

		asm volatile
		(
			//The offset added to esp needs to match that in the function epilogue
			//However, there is no need to restore any registers
			"addl $0x0c, %%esp			\n"
			"jmp *%0					\n"
			: 
			: "r" (tlbEntry->translation)
		);
	}
}

//Jump directly to the next translation, and patch the jump instruction
void JumpToNextTranslationWithPatch(void)
{
	Dword patchAddress = (Dword)(__builtin_return_address(0) - 5);

	Dword physicalEip;
	if(UNLIKELY(!CalculatePhysicalAddress(registers->r_cs.base + registers->r_eip, &physicalEip)))
		ReturnImmediately();
	
	struct EipToTranslationTLBEntry * tlbEntry =
		&registers->eipToTranslationTLB[physicalEip & EIP_TO_TRANSLATION_TLB_MASK];

	if(LIKELY(	tlbEntry->physicalEip == physicalEip &&
				tlbEntry->flags == registers->currentTranslationFlags))
	{
		//If the jump is within the current physical page, patch the call to this function
		//with a direct jump
		if((physicalEip >> 12) == registers->currentTranslationPhysicalPage)
		{
			*(Byte *)patchAddress = 0xe9;
			*(Dword *)(patchAddress + 1) =
				(Dword)(tlbEntry->translation - (patchAddress + 5));
		}
		else
		{
			//If not, patch the call with a jump to jumpToNextTranslation
			*(Byte *)patchAddress = 0xe9;
			*(Dword *)(patchAddress + 1) =
				(Dword)(registers->jumpToNextTranslationAddress - (patchAddress + 5));
		}

		registers->currentTranslationPhysicalPage = physicalEip >> 12;

		asm volatile
		(
			//The offset added to esp needs to match that in the function epilogue + 4
			//However, there is no need to restore any registers
			"addl $0x14, %%esp			\n"
			"jmp *%0					\n"
			: 
			: "r" (tlbEntry->translation)
		);
	}

	ReturnImmediately();
}
