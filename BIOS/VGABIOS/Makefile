ASM = nasm
ASMFLAGS = -f obj
CC = bcc
CFLAGS = -c -ms
LINK = tlink
LINKFLAGS = /c /C /x

VGABIOS.bin: VGABIOS.exe VGAREGS.DAT COLREGS.DAT
	exe2bin VGABIOS.exe VGABIOS.bin
	..\..\Tools\PadFile\Release\PadFile VGABIOS.bin 19456
	copy /b VGABIOS.bin+VGAREGS.DAT VGABIOS.bin
	..\..\Tools\PadFile\Release\PadFile VGABIOS.bin 20480
	copy /b VGABIOS.bin+COLREGS.DAT VGABIOS.bin
	..\..\Tools\PadFile\Release\PadFile VGABIOS.bin 32768
	copy VGABIOS.bin ..\..\VMachine\Resources

VGABIOS.exe: VGABIOS.obj Init.obj Int10.obj SetMode.obj Cursor.obj Scroll.obj ReadChar.obj WriteCha.obj SetPal.obj Blink.obj SetCol.obj
	echo $^ > objects.txt
	$(LINK) $(LINKFLAGS) @objects.txt

VGABIOS.obj: VGABIOS.asm
	$(ASM) $(ASMFLAGS) $<

%.obj: %.c
	$(CC) $(CFLAGS) $<
	
Init.obj: Init.c ..\Common\BIOS.h
Int10.obj: Int10.c ..\Common\BIOS.h
SetMode.obj: SetMode.c ..\Common\BIOS.h
Cursor.obj: Cursor.c ..\Common\BIOS.h
Scroll.obj: Scroll.c ..\Common\BIOS.h
ReadChar.obj: ReadChar.c ..\Common\BIOS.h
WriteCha.obj: WriteCha.c ..\Common\BIOS.h
SetPal.obj: SetPal.c ..\Common\BIOS.h
Blink.obj: Blink.c ..\Common\BIOS.h
SetCol.obj: SetCol.c ..\Common\BIOS.h

clean:
	del *.obj
	del *.exe
	del *.bin
