//	VMachine
//	Handle an exception
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

void VMachine::HandleException(void)
{
	//Get the exception being handled
	assert(registers->exception < 0x100);

	Byte exceptionNumber = static_cast<Byte>(registers->exception);

	//Clear the exception flag
	registers->exception = EXCEPTION_NONE;

	//Process the exception
	DoInt(exceptionNumber, INT_TYPE_EXCEPTION, registers->errorCode);
}