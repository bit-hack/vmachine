; ------------- VMachine BIOS
;
; ------------- Copyright (c) 2006, Paul Baker
;				Distributed under the New BSD Licence. (See accompanying file License.txt
;				or copy at http://www.paulsprojects.net/NewBSDLicense.txt)

				cpu 8086					; Use 8086 instructions only
				bits 16						; BIOS runs in real mode

				section _TEXT class=CODE	; Place all code in the _TEXT section
				
				global ..start
				global _NullIntHandler
				global _flInt08Handler, _flInt09Handler, _flInt0eHandler, _flInt74Handler, _flInt76Handler
				global _flInt11Handler, _flInt12Handler, _flInt13Handler, _flInt15Handler, _flInt16Handler, _flInt1aHandler
				global _Int19Handler, _Int1cHandler
				
				extern _SetupIVT, _Init, _ReadBootSector
				extern _Int08Handler, _Int09Handler, _Int0eHandler, _Int74Handler, _Int76Handler
				extern _Int11Handler, _Int12Handler, _Int13Handler, _Int15Handler, _Int16Handler, _Int1aHandler

; ------------- BIOS entry point

..start:		cli

	; --------- Set up a stack

				xor ax, ax
				mov ss, ax
				mov sp, 0x7c00
			
	; --------- Set ds = ss
	
				mov ds, ax

	; --------- Set up the IVT

				call _SetupIVT
				
	; --------- Initialise the BDA and the hardware
	
				call _Init
				
	; --------- Call the VGA BIOS entry point

				call 0xc000:0x0000
				
	; --------- Read the boot sector into memory

				sti
				call _ReadBootSector
				cli
				
	; --------- Move the boot drive number into dl
	
				mov dl, al
				
	; --------- Jump to the boot sector code

				jmp 0x0000:0x7c00

; ------------- Null int handler

_NullIntHandler:
				iret

; ------------- First level interrupt handlers

				%macro flIntHandler 1

				push ds
				push es
				push ss
				pop ds
				push ax
				push cx
				push dx
				push bx
				push bp
				push si
				push di

				push sp
				call %1
				add sp, 2

				pop di
				pop si
				pop bp
				pop bx
				pop dx
				pop cx
				pop ax
				pop es
				pop ds
				iret
				
				%endmacro
				
_flInt08Handler:
				flIntHandler _Int08Handler
				
_flInt09Handler:
				flIntHandler _Int09Handler

_flInt0eHandler:
				flIntHandler _Int0eHandler
				
_flInt74Handler:
				flIntHandler _Int74Handler
				
_flInt76Handler:
				flIntHandler _Int76Handler
				
_flInt11Handler:
				sti
				flIntHandler _Int11Handler
				
_flInt12Handler:
				sti
				flIntHandler _Int12Handler
				
_flInt13Handler:
				sti
				flIntHandler _Int13Handler
				
_flInt15Handler:
				sti
				flIntHandler _Int15Handler
				
_flInt16Handler:
				sti
				flIntHandler _Int16Handler
				
_flInt1aHandler:
				sti
				flIntHandler _Int1aHandler

; ------------- Int 0x19 handler

_Int19Handler:	jmp $

; ------------- Int 0x1c handler

_Int1cHandler:	iret
