//	VMachine
//	Keyboard controller class
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

class VMachine;
class Pic;

class KeyboardController : private boost::noncopyable
{
public:

	//Constructor/Destructor
	KeyboardController(VMachine & vmachine_, Pic & masterPic_, Pic & slavePic_);
	~KeyboardController();

	//Copy constructor/copy assignment operator handled by boost::noncopyable

	//Set the state of a key
	void SetKeyState(Byte scancode, bool pressed);

	//Set the state of the mouse
	void SetMouseState(	SignedWord dx, SignedWord dy,
						bool lButtonDown, bool mButtonDown,	bool rButtonDown);

	//Access ports
	Byte ReadPortByte(Byte offset);
	void WritePortByte(Byte offset, Byte data);

private:

	//VMachine this keyboard controller is part of
	VMachine & vmachine;

	//PICs
	Pic & masterPic;
	Pic & slavePic;

	//Data buffer
	std::deque <std::pair<Byte, bool> > dataBuffer;
	Byte lastDataByteRead;
	
	//Perform a command
	void StartCommand(Word newCommand);
	void CompleteCommand(void);

	static const Word PORT_64_COMMAND = 0x100;
	Word command;

	std::vector <Byte> commandBuffer;
	Dword numExtraCommandBytes;

	//Raise any appropriate IRQ
	void RaiseIRQ(void);

	//Command byte
	Byte commandByte;
	bool keyboardEnabled, mouseEnabled;
	bool keyboardIntEnabled, mouseIntEnabled;
};
