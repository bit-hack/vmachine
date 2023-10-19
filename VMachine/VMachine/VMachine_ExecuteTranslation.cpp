//	VMachine
//	Execute a translation
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

//Disable warning C4731: frame pointer register 'ebp' modified by inline assembly code
#pragma warning(disable: 4731)

void VMachine::ExecuteTranslation(Byte * translation)
{
	Registers * pRegisters = registers.get();

	__try
	{
		__asm
		{
			pushad
			pushfd

			mov eax, translation

			mov ebp, pRegisters

			lea ecx, [esp - 4]
			mov [ebp + REGISTERS_RETURN_IMMEDIATELY_ESP_OFFSET], ecx

			call eax

			popfd
			popad
		}
	}
	__except
	(
		(GetExceptionCode() == EXCEPTION_FLT_DENORMAL_OPERAND ||
		GetExceptionCode() == EXCEPTION_FLT_DIVIDE_BY_ZERO ||
		GetExceptionCode() == EXCEPTION_FLT_INEXACT_RESULT ||
		GetExceptionCode() == EXCEPTION_FLT_INVALID_OPERATION ||
		GetExceptionCode() == EXCEPTION_FLT_OVERFLOW ||
		GetExceptionCode() == EXCEPTION_FLT_STACK_CHECK ||
		GetExceptionCode() == EXCEPTION_FLT_UNDERFLOW)
			? (registers->exception = EXCEPTION_FLOATING_POINT_ERROR, EXCEPTION_EXECUTE_HANDLER)
			:
		(GetExceptionCode() == EXCEPTION_INT_DIVIDE_BY_ZERO ||
		GetExceptionCode() == EXCEPTION_INT_OVERFLOW)
			? (registers->exception = EXCEPTION_DIVIDE_ERROR, EXCEPTION_EXECUTE_HANDLER)
			: EXCEPTION_CONTINUE_SEARCH
	)
	{

	}
}
