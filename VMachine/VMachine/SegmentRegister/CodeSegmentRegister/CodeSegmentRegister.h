//	VMachine
//	Class for a code segment register
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

class VMachine;

class CodeSegmentRegister : public SegmentRegister
{
public:

	CodeSegmentRegister(const boost::shared_ptr<Registers> & registers_,
						SegmentRegisterData & data_)
		: registers(registers_), SegmentRegister(data_)
	{}
	virtual ~CodeSegmentRegister()
	{}

	//Verify an access using this segment register is allowed
	//If so, return the linear address
	virtual bool VerifyByteRead(Dword offset, Dword & linAddr) const;
	virtual bool VerifyWordRead(Dword offset, Dword & linAddr) const;
	virtual bool VerifyDwordRead(Dword offset, Dword & linAddr) const;

	virtual bool VerifyByteWrite(Dword offset, Dword & linAddr) const;
	virtual bool VerifyWordWrite(Dword offset, Dword & linAddr) const;
	virtual bool VerifyDwordWrite(Dword offset, Dword & linAddr) const;

private:

	//Registers of the VMachine this segment register is part of
	boost::shared_ptr<Registers> registers;
};