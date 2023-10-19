//	VMachine
//	Write to a register
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Ym3812Channel.h"
#include "Ym3812Operator/Ym3812Operator.h"

void Ym3812Channel::WriteMultipleRegister(Byte op, Byte data)
{
	operators[op]->WriteMultipleRegister(data);
}

void Ym3812Channel::WriteTotalLevelRegister(Byte op, Byte data)
{
	operators[op]->WriteTotalLevelRegister(data);
}

void Ym3812Channel::WriteARDRRegister(Byte op, Byte data)
{
	operators[op]->WriteARDRRegister(data);
}

void Ym3812Channel::WriteSLRRRegister(Byte op, Byte data)
{
	operators[op]->WriteSLRRRegister(data);
}

void Ym3812Channel::WriteFNumberLowRegister(Byte data)
{
	operators[0]->WriteFNumberLowRegister(data);
	operators[1]->WriteFNumberLowRegister(data);
}

void Ym3812Channel::WriteFNumberHighRegister(Byte data)
{
	operators[0]->WriteFNumberHighRegister(data);
	operators[1]->WriteFNumberHighRegister(data);
}

namespace
{
	const double feedbackTable[8] =
	{
		0.0, 0.03125, 0.0625, 0.125, 0.25, 0.5, 1.0, 2.0
	};
}

void Ym3812Channel::WriteAlgorithmRegister(Byte data)
{
	feedback = feedbackTable[(data & 0x0e) >> 1];
	algorithm = data & 0x01;
}

void Ym3812Channel::WriteWaveformRegister(Byte op, Byte data)
{
	operators[op]->WriteWaveformRegister(data);
}
