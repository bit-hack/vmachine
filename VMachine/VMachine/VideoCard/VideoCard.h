//	VMachine
//	Video card class
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

class VideoCard : private boost::noncopyable
{
public:

	//Constructor/Destructor
	VideoCard();
	~VideoCard();

	//Copy constructor/copy assignment operator handled by boost::noncopyable

	//Get the required framebuffer size
	std::pair <Dword, Dword> GetFramebufferSize(void)
	{ return std::make_pair(width, height); }

	//Update the framebuffer
	void UpdateFramebuffer(Dword frameNumber, FramebufferData & framebuffer);

	//Access video memory
	Byte ReadByte(Dword base, Dword offset);
	Word ReadWord(Dword base, Dword offset);
	Dword ReadDword(Dword base, Dword offset);

	void WriteByte(Dword base, Dword offset, Byte data);
	void WriteWord(Dword base, Dword offset, Word data);
	void WriteDword(Dword base, Dword offset, Dword data);

	//Access ports
	Byte ReadPortByte(Word address);
	void WritePortByte(Word address, Byte data);
	
private:

	//Read from registers
	Byte ReadMiscOutputRegister(void);
	Byte ReadFeatureControlRegister(void);
	Byte ReadInputStatus0Register(void);
	Byte ReadInputStatus1Register(void);

	Byte ReadSequencerRegisterIndex(void);
	Byte ReadSequencerRegister(void);

	Byte ReadCRTCRegisterIndex(void);
	Byte ReadCRTCRegister(void);

	Byte ReadGraphicsRegisterIndex(void);
	Byte ReadGraphicsRegister(void);

	Byte ReadAttributeRegister(void);

	Byte ReadColorRegister(void);

	//Write to registers
	void WriteMiscOutputRegister(Byte data);
	void WriteFeatureControlRegister(Byte data);
	
	void WriteSequencerRegisterIndex(Byte data);
	void WriteSequencerRegister(Byte data);

	void WriteCRTCRegisterIndex(Byte data);
	void WriteCRTCRegister(Byte data);

	void WriteGraphicsRegisterIndex(Byte data);
	void WriteGraphicsRegister(Byte data);

	void WriteAttributeRegister(Byte data);

	void WriteColorRegisterReadIndex(Byte data);
	void WriteColorRegisterWriteIndex(Byte data);

	void WriteColorRegister(Byte data);

	//Framebuffer cache
	bool regenerateFramebuffer;
	std::vector <Dword> framebufferCache;

	//Video memory
	static const Dword videoMemoryPlaneSize = 0x10000;
	std::vector<Byte> videoMemory;
	std::vector<bool> videoMemoryDirty;

	//Read data latches
	Byte readDataLatches[4];

	//Screen size
	Dword width, height;

	//MISC OUTPUT REGISTER
	Byte miscOutputRegister;

	//SEQUENCER REGISTERS
	static const Byte numSequencerRegisters = 5;

	Byte sequencerIndexRegister;
	Byte sequencerRegisters[numSequencerRegisters];

	Byte characterWidth;

	Byte mapMask;

	bool chain4;
	bool oddEven;
	
	//CRTC REGISTERS
	static const Byte numCRTCRegisters = 24;

	Byte CRTCIndexRegister;
	Byte CRTCRegisters[numCRTCRegisters];

	Word charactersPerLine;
	Byte characterHeight;
	bool convert200To400;
	Word startAddress;
	Byte displayPitch;

	//GRAPHICS REGISTERS
	static const Byte numGraphicsRegisters = 9;

	Byte graphicsIndexRegister;
	Byte graphicsRegisters[numGraphicsRegisters];

	enum AluFunction { ALU_NOP = 0, ALU_AND, ALU_OR, ALU_XOR };

	Byte setReset;
	Byte enableSetReset;
	Byte colorCompare;
	AluFunction aluFunction;
	Byte rotateCount;
	Byte readMapSelect;
	Byte shiftRegister;
	Byte readMode;
	Byte writeMode;
	Dword videoMemoryBase;
	Dword videoMemoryTop;
	bool graphicsMode;	//As opposed to alphanumeric
	Byte colorDontCare;
	Byte bitMask;

	//ATTRIBUTE REGISTERS
	static const Byte numAttributeRegisters = 21;

	bool attributeRegisterFlipFlopData;

	Byte attributeIndexRegister;
	Byte attributeRegisters[numAttributeRegisters];

	bool lineGraphicsEnabled;
	bool blinkEnabled;
	bool internalPalette4Bit;
	bool colorPlaneEnabled[4];
	Byte colorSelect76;
	Byte colorSelect54;
	
	//COLOR REGISTERS

	//Color registers (in the format required for display)
	Dword colorRegisters[256];

	//Current color register, component (r, g, b) and mode
	Byte colorRegisterIndex;
	Byte colorRegisterComponent;
	bool colorRegisterWriteMode;
};
