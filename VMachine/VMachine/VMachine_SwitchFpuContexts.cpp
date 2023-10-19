//	VMachine
//	Switch FPU contexts
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

void VMachine::SwitchToVMachineFpuContext(void)
{
	void * pFpuSaveArea = fpuSaveArea;
	void * pHostFpuSaveArea = hostFpuSaveArea;
		
	__asm
	{
		mov eax, pFpuSaveArea
		mov ecx, pHostFpuSaveArea
		fxsave [ecx]
		fxrstor [eax]
	}
}

void VMachine::SwitchToHostFpuContext(void)
{
	void * pFpuSaveArea = fpuSaveArea;
	void * pHostFpuSaveArea = hostFpuSaveArea;
		
	__asm
	{
		mov eax, pFpuSaveArea
		mov ecx, pHostFpuSaveArea
		fxsave [eax]
		fxrstor [ecx]
	}
}
