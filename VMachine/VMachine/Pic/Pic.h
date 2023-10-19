//	VMachine
//	Programmable interrupt controller class
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

class VMachine;

class Pic
{
public:

	//Constructor/Destructor
	Pic(bool isMaster_, VMachine & vmachine_, Byte vectorOffset_);
	~Pic();

	//Copy constructor/copy assignment operator handled by boost::noncopyable

	//Set the pointer to the other PIC
	void SetOtherPic(Pic * otherPic_)
	{ otherPic = otherPic_; }

	//Access ports
	Byte ReadPortByte(Byte offset);
	void WritePortByte(Byte offset, Byte data);

	//Raise/Lower an IRQ
	void RaiseIRQ(Byte irq);
	void LowerIRQ(Byte irq);

	//Acknowledge an interrupt
	Byte AcknowledgeInt(void);

private:

	//Handle an EOI
    void NonSpecificEOI(void);
	void SpecificEOI(Byte level);

	//Is this the master PIC or the slave?
	bool isMaster;

	//VMachine this PIC is part of
	VMachine & vmachine;

	//Pointer to the other PIC
	Pic * otherPic;

	//Offset of the interrupt vectors used
	Byte vectorOffset;

	//Which IRQs are raised?
	Byte irqsRaised;

	//Interrupt request, mask and in-service registers
	Byte irr, imr, isr;

	//Is this PIC configured for edge- or level-triggered interrupts?
	bool levelTriggered;

	//Which ICW, if any, are we waiting for?
	Byte nextIcw;
	bool icw4Required;

	//Will the ISR or the IRR be read from offset 0?
	bool readIsr;

public:

	//Exception thrown by Pic functions
	class Ex : public std::exception
	{
	public:
		Ex(const std::string & what) : std::exception(what.c_str())
		{}
	};
};
