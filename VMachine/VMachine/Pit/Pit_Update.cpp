//	VMachine
//	PIT update function
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"
#include "Pit.h"

void Pit::Update()
{
	timer0.Update();
	timer1.Update();
	timer2.Update();
}