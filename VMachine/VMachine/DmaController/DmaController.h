//	VMachine
//	DMA controller class
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

class VMachine;

class DmaController : private boost::noncopyable
{
public:

	//Constructor/Destructor
	DmaController(VMachine & vmachine_);
	~DmaController();

	//Copy constructor/copy assignment operator handled by boost::noncopyable

	//Access ports
	Byte ReadPortByte(Word base, Byte offset);
	void WritePortByte(Word base, Byte offset, Byte data);

	Byte ReadPageRegister(Byte channel);
	void WritePageRegister(Byte channel, Byte data);

	//Access memory via the DMA controller
	Byte ReadByte(Byte channel);
	Word ReadWord(Byte channel);

	void WriteByte(Byte channel, Byte data);
	void WriteWord(Byte channel, Word data);

private:

	//VMachine this PIC is part of
	VMachine & vmachine;

	//Byte select flip-flop
	bool highByte;

	//Page registers
	Byte pageRegisters[8];

	//Base address and count registers
	Word baseAddressRegisters[8];
	Word baseCountRegisters[8];

	//Current address and count registers
	Word currentAddressRegisters[8];
	Word currentCountRegisters[8];

	//Autoinitialise enabled?
	bool autoInitEnabled[8];
};
