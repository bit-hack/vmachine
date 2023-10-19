//	VMachine
//	YM3812 constructor/destructor
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Ym3812.h"
#include "fmopl/fmopl.h"
#include "Ym3812Channel/Ym3812Channel.h"

//Constructor
Ym3812::Ym3812(Dword clockRate, Dword samplingRate)
	:	addressRegister(0x00), lastUpdateTime(0),
		timerIrq(false), timer1Expired(false), timer2Expired(false),
		timer1Masked(false), timer2Masked(false), timer1Data(0x00), timer2Data(0x00),
		timer1ExpiryTime(0xffffffffffffffff), timer2ExpiryTime(0xffffffffffffffff)
{
#ifdef USE_FMOPL
	opl = YM3812Init(clockRate, samplingRate);
#endif

#ifdef USE_YM3812_CHANNEL
	for(int i = 0; i < 9; ++i)
		channels[i] = boost::shared_ptr<Ym3812Channel>(
			new Ym3812Channel(static_cast<double>(clockRate) / samplingRate));
#endif
}

//Destructor
Ym3812::~Ym3812()
{
#ifdef USE_FMOPL
	YM3812Shutdown(opl);
#endif
}
