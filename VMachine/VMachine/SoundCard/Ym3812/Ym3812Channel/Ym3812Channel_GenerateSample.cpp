//	VMachine
//	Generate a sample
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Ym3812Channel.h"
#include "Ym3812Operator/Ym3812Operator.h"

SignedDword Ym3812Channel::GenerateSample(void)
{
	double op0Output = operators[0]->GenerateSample(feedback * feedbackData);
	feedbackData = op0Output;

	switch(algorithm)
	{
	case 0:	//Algorithm 0: op0 -> op1 ->
		{
			double op1Output = operators[1]->GenerateSample(op0Output);
			return static_cast<SignedDword>(0x2000 * op1Output);
		}

	case 1:	//Algorithm 1: op0 + op1 ->
		{
			double op1Output = operators[1]->GenerateSample(0.0);
			return static_cast<SignedDword>(0x2000 * (op0Output + op1Output));
		}
	}

	assert(false);
	return 0x0000;
}
