//	VMachine
//	IDE controller class
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

class Pic;

struct HardDiskGeometry
{
	Dword numCylinders, numHeads, numSectors;
	Dword numSectorsTotal;
};

class IdeController : private boost::noncopyable
{
public:

	//Constructor/Destructor
	IdeController(	const std::string & hdImageFilename,
					Pic & slavePic_);
	~IdeController();

	//Copy constructor/copy assignment operator handled by boost::noncopyable

	//Access ports
	Byte ReadPortByte(Word base, Byte offset);
	Word ReadPortWord(Word base, Byte offset);
	Dword ReadPortDword(Word base, Byte offset);
	
	void WritePortByte(Word base, Byte offset, Byte data);
	void WritePortWord(Word base, Byte offset, Word data);
	void WritePortDword(Word base, Byte offset, Dword data);

	//Get the hard disk geometry
	HardDiskGeometry GetHardDiskGeometry(void)
	{ return hdGeometry; }

private:

	//Perform a command
	void StartCommand(Byte newCommand);
	void CompleteCommand(void);

	void DoRead(Word numSectors);
	void DoWrite(Word numSectors);

	//Slave PIC used by this IDE controller
	Pic & slavePic;
	
	//Hard disk image file
	HANDLE hdImageFile;

	//Hard disk geometry
	HardDiskGeometry hdGeometry;

	//Registers
	Word sectorCount;
	Byte sector;
	Word cylinder;
	Byte drive, head;
	Byte command;

	bool dataRequest;

	Byte multipleSectorCount;

	//Data buffer
	static const std::vector<Byte>::size_type dataBufferSize = 16 * 0x200;
	std::vector <Byte> dataBuffer;
	std::vector<Byte>::size_type readOffset, writeOffset;

public:

	//Exception thrown by IdeController functions
	class Ex : public std::exception
	{
	public:
		Ex(const std::string & what) : std::exception(what.c_str())
		{}
	};
};
