//	VMachine
//	Floppy controller class
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

class Pic;
class DmaController;

class FloppyController
{
public:

	//Constructor/Destructor
	FloppyController(	const std::string & imageFilename, Pic & masterPic_,
						DmaController & dmaController_);
	~FloppyController();

	//Copy constructor/copy assignment operator handled by boost::noncopyable

	//Insert/eject disk
	void InsertDisk(const std::string & imageFilename);
	void EjectDisk(void);

	bool IsDiskInserted(void)
	{ return imageFile != 0; }

	//Access ports
	Byte ReadPortByte(Byte offset);
	void WritePortByte(Byte offset, Byte data);

private:

	//Perform a command
	void PerformCommand(void);

	//Master PIC and DMA controller used by this floppy controller
	Pic & masterPic;
	DmaController & dmaController;

	//Floppy disk image file
	HANDLE imageFile;

	//Command bytes written
	std::vector <Byte> commandBytes;

	//Number of command bytes for this command
	Byte numCommandBytes;

	//Result bytes to be read
	std::deque <Byte> resultBytes;

public:

	//Exceptions thrown by FloppyController functions
	class Ex : public std::exception
	{
	public:
		Ex(const std::string & what) : std::exception(what.c_str())
		{}
	};

	class ImageIncorrectSizeEx : public Ex
	{
	public:
		ImageIncorrectSizeEx(const std::string & what) : Ex(what)
		{}
	};
};
