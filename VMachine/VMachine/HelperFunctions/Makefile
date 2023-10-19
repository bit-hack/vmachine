CC = gcc
CFLAGS = -c -std=gnu99 -Wall -Winline -O3 -fomit-frame-pointer

all: HelperFunctions.elf
	copy HelperFunctions.elf ..\..\Resources

%.elf: %.o
	objcopy -O elf32-i386 $< $@

HelperFunctions.o: HelperFunctions.c HelperFunctions.h ../x86Constants.h ../Registers.h
	$(CC) $(CFLAGS) $<

clean:
	del *.o
	del *.elf
	