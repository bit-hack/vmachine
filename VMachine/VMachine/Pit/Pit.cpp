//	VMachine
//	PIT constructor/destructor
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"
#include "Pit.h"

//Constructor
Pit::Pit(VMachine & vmachine_, Pic & masterPic_) :	vmachine(vmachine_),
								timer0(true, 0x0000, 3, masterPic_),
								timer1(false, 0x0012, 2, masterPic_),
								timer2(false, 0x0100, 3, masterPic_)
{

}

//Destructor
Pit::~Pit()
{
	
}
