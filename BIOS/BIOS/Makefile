ASM = nasm
ASMFLAGS = -f obj
CC = bcc
CFLAGS = -c -ms
LINK = tlink
LINKFLAGS = /c /C /x

BIOS.bin: BIOS.exe
	exe2bin BIOS.exe BIOS.bin
	copy BIOS.bin ..\..\VMachine\Resources

BIOS.exe: BIOS.obj SetupIVT.obj Init.obj ReadBoot.obj Int08.obj Int09.obj Int0e.obj Int74.obj Int76.obj Int11.obj Int12.obj Int13.obj Int15.obj Int16.obj Int1a.obj ReadSect.obj DiskType.obj DrvParam.obj WriteSec.obj CopyMem.obj CopyLoop.obj GetKey.obj CheckKey.obj H_LLSH.obj H_LURSH.obj
	echo $^ > objects.txt
	$(LINK) $(LINKFLAGS) @objects.txt

Bios.obj: BIOS.asm
	$(ASM) $(ASMFLAGS) $<
	
CopyLoop.obj: CopyLoop.asm
	$(ASM) $(ASMFLAGS) $<

%.obj: %.c
	$(CC) $(CFLAGS) $<

SetupIVT.obj: SetupIVT.c ..\Common\BIOS.h
Init.obj: Init.c ..\Common\BIOS.h
ReadBoot.obj: ReadBoot.c ..\Common\BIOS.h
Int08.obj: Int08.c ..\Common\BIOS.h
Int09.obj: Int09.c ..\Common\BIOS.h
Int0e.obj: Int0e.c ..\Common\BIOS.h
Int74.obj: Int74.c ..\Common\BIOS.h
Int76.obj: Int76.c ..\Common\BIOS.h
Int11.obj: Int11.c ..\Common\BIOS.h
Int12.obj: Int12.c ..\Common\BIOS.h
Int13.obj: Int13.c ..\Common\BIOS.h
Int15.obj: Int15.c ..\Common\BIOS.h
Int16.obj: Int16.c ..\Common\BIOS.h
Int1a.obj: Int1a.c ..\Common\BIOS.h
ReadSect.obj: ReadSect.c ..\Common\BIOS.h
DiskType.obj: DiskType.c ..\Common\BIOS.h
DrvParam.obj: DrvParam.c ..\Common\BIOS.h
WriteSec.obj: WriteSec.c ..\Common\BIOS.h
GetKey.obj: GetKey.c ..\Common\BIOS.h
CheckKey.obj: CheckKey.c ..\Common\BIOS.h

H_LURSH.obj: c:\bc31\lib\cs.lib
	tlib c:\bc31\lib\cs.lib *H_LURSH
	
H_LLSH.obj: c:\bc31\lib\cs.lib
	tlib c:\bc31\lib\cs.lib *H_LLSH

clean:
	del *.obj
	del *.exe
	del *.bin
