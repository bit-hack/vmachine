; ------------- VMachine BIOS
; ------------- Extended memory copy loop
;
; ------------- Copyright (c) 2006, Paul Baker
;				Distributed under the New BSD Licence. (See accompanying file License.txt
;				or copy at http://www.paulsprojects.net/NewBSDLicense.txt)

				cpu 386						; Use 386 instructions only
				bits 16						; Runs in real mode/16-bit protected mode

				section _TEXT class=CODE	; Place all code in the _TEXT section
				
				global _CopyLoop

_CopyLoop:

	; --------- Enable protected mode
	
				pusha
				push ds
				push es
					
				lgdt [es:si]
				
				mov eax, cr0
				or al, 0x01
				mov cr0, eax
				
				jmp 0x08:.l1
.l1:			mov ax, 0x10
				mov ds, ax
				mov ax, 0x18
				mov es, ax
				
	; --------- Perform the copy
	
				xor si, si
				xor di, di
				rep movsw
				
	; --------- Return to real mode
	
				mov eax, cr0
				and al, 0xfe
				mov cr0, eax
				
				jmp 0xf000:.l2
.l2:			
				pop es
				pop ds
				popa
				ret
				