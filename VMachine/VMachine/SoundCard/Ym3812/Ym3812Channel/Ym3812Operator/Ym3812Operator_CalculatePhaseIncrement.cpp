//	VMachine
//	Calculate the phaseIncrement
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Ym3812Operator.h"

void Ym3812Operator::CalculatePhaseIncrement(void)
{
	phaseIncrement = (fNumber * multiple) * (1 << block) * (clocksPerSample / 72);
	phaseIncrement /= (1 << 20);
}
