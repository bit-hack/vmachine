//	VMachine
//	YM3812 channel constructor/destructor
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Ym3812Channel.h"
#include "Ym3812Operator/Ym3812Operator.h"

//Constructor
Ym3812Channel::Ym3812Channel(double clocksPerSample_)
	:	algorithm(0), feedback(0.0), feedbackData(0.0)
{
	operators[0] = boost::shared_ptr<Ym3812Operator>(new Ym3812Operator(clocksPerSample_));
	operators[1] = boost::shared_ptr<Ym3812Operator>(new Ym3812Operator(clocksPerSample_));
}

//Destructor
Ym3812Channel::~Ym3812Channel()
{

}
