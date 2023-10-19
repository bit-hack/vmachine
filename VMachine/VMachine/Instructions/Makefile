CC = gcc
CFLAGS = -c -std=gnu99 -Wall -Winline -O3 -fomit-frame-pointer -fno-align-functions -fno-reorder-blocks

all: BinaryArithmeticInstructions.elf BitByteInstructions.elf DataTransferInstructions.elf DecimalArithmeticInstructions.elf FlagControlInstructions.elf FpuInstructions.elf NearControlTransferInstructions.elf SegmentRegisterInstructions.elf ShiftAndRotateInstructions.elf StringInstructions.elf MiscInstructions.elf
	copy BinaryArithmeticInstructions.elf ..\..\Resources
	copy BitByteInstructions.elf ..\..\Resources
	copy DataTransferInstructions.elf ..\..\Resources
	copy DecimalArithmeticInstructions.elf ..\..\Resources
	copy FlagControlInstructions.elf ..\..\Resources
	copy FpuInstructions.elf ..\..\Resources
	copy NearControlTransferInstructions.elf ..\..\Resources
	copy SegmentRegisterInstructions.elf ..\..\Resources
	copy ShiftAndRotateInstructions.elf ..\..\Resources
	copy StringInstructions.elf ..\..\Resources
	copy MiscInstructions.elf ..\..\Resources

%.elf: %.o
	objcopy -O elf32-i386 $< $@

%.o: %.c
	$(CC) $(CFLAGS) $<

BinaryArithmeticInstructions.o: BinaryArithmeticInstructions.c ../HelperFunctions/HelperFunctions.h ../x86Constants.h ../Registers.h Instructions.h
BitByteInstructions.o: BitByteInstructions.c ../HelperFunctions/HelperFunctions.h ../x86Constants.h ../Registers.h Instructions.h
DataTransferInstructions.o: DataTransferInstructions.c ../HelperFunctions/HelperFunctions.h ../x86Constants.h ../Registers.h Instructions.h
DecimalArithmeticInstructions.o: DecimalArithmeticInstructions.c ../HelperFunctions/HelperFunctions.h ../x86Constants.h ../Registers.h Instructions.h
FlagControlInstructions.o: FlagControlInstructions.c ../HelperFunctions/HelperFunctions.h ../x86Constants.h ../Registers.h Instructions.h
FpuInstructions.o: FpuInstructions.c ../HelperFunctions/HelperFunctions.h ../x86Constants.h ../Registers.h Instructions.h
NearControlTransferInstructions.o: NearControlTransferInstructions.c ../HelperFunctions/HelperFunctions.h ../x86Constants.h ../Registers.h Instructions.h
SegmentRegisterInstructions.o: SegmentRegisterInstructions.c ../HelperFunctions/HelperFunctions.h ../x86Constants.h ../Registers.h Instructions.h
ShiftAndRotateInstructions.o: ShiftAndRotateInstructions.c ../HelperFunctions/HelperFunctions.h ../x86Constants.h ../Registers.h Instructions.h
StringInstructions.o: StringInstructions.c ../HelperFunctions/HelperFunctions.h ../x86Constants.h ../Registers.h Instructions.h
MiscInstructions.o: MiscInstructions.c ../HelperFunctions/HelperFunctions.h ../x86Constants.h ../Registers.h Instructions.h

clean:
	del *.o
	del *.elf