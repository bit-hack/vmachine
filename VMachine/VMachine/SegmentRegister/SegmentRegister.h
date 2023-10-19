//	VMachine
//	Abstract base class for a segment register
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

class SegmentRegister
{
public:

	SegmentRegister(SegmentRegisterData & data_) : data(data_)
	{}
	virtual ~SegmentRegister() = 0
	{}

	//Copy constructor/copy assignment operator handled by boost::noncopyable

	//Verify an access using this segment register is allowed
	//If so, return the linear address
	virtual bool VerifyByteRead(Dword offset, Dword & linAddr) const = 0;
	virtual bool VerifyWordRead(Dword offset, Dword & linAddr) const = 0;
	virtual bool VerifyDwordRead(Dword offset, Dword & linAddr) const = 0;

	virtual bool VerifyByteWrite(Dword offset, Dword & linAddr) const = 0;
	virtual bool VerifyWordWrite(Dword offset, Dword & linAddr) const = 0;
	virtual bool VerifyDwordWrite(Dword offset, Dword & linAddr) const = 0;

	SegmentRegisterData & data;
};
