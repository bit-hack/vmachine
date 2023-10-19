//	VMachine
//	Calculate a physical address
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

//TODO: USER_BIT checking?
bool VMachine::CalculatePhysicalAddress(Dword linearAddress, Dword & physicalAddress)
{
	//If paging is disabled, physical address == linear address
	if((registers->r_cr0 & CR0_PG_FLAG) == 0)
	{
		physicalAddress = linearAddress;
		return true;
	}

	//Otherwise, walk the page tables and calculate the physical address
	Dword pde = ReadDwordPhysical(
		(registers->r_cr3 & 0xfffff000) | ((linearAddress & 0xffc00000) >> 20));

	if((pde & PDE_PRESENT_BIT) == 0)
		return false;

	Dword pte = ReadDwordPhysical(
		(pde & 0xfffff000) | ((linearAddress & 0x003ff000) >> 10));

	if((pte & PTE_PRESENT_BIT) == 0)
		return false;

	Dword tempPhysicalAddress = (pte & 0xfffff000) | (linearAddress & 0x00000fff);

	//Ensure the physical address is within the bounds of memory
	if(tempPhysicalAddress >= registers->memorySize)
		return false;

	physicalAddress = tempPhysicalAddress;
	return true;
}
