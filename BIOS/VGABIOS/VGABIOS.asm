; ------------- VMachine VGA BIOS
;
; ------------- Copyright (c) 2006, Paul Baker
;				Distributed under the New BSD Licence. (See accompanying file License.txt
;				or copy at http://www.paulsprojects.net/NewBSDLicense.txt)


				cpu 8086					; Use 8086 instructions only
				bits 16						; VGA BIOS runs in real mode

				section _TEXT class=CODE	; Place all code in the _TEXT section
				
				global ..start
				extern _Init
				extern _Int10Handler

; ------------- VGA BIOS entry point

..start:

	; --------- Hook int 0x10

				xor ax, ax
				mov es, ax
				mov di, 0x40
				cld

				mov ax, flInt10Handler
				stosw
				mov ax, 0xc000
				stosw

	; --------- Call the initialise function

				call _Init
			
				retf

; ------------- First level int 0x10 handler

flInt10Handler:	sti
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
				call _Int10Handler
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
