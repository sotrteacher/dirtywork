; ============================================================
; File: SETJMP.ASM
;
; Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall
;
; Purpose: Implements setjmp and longjmp for C.
;
; Interrupts: Already disabled; remain disabled.
;
; Designed for use with the NASM protected mode 386 assembler.
;
; Modification History:
;
; ============================================================

		SECTION	.text
		BITS	32

		GLOBAL	_setjmp
		GLOBAL	_longjmp

; jmp_buf:
;	 eax ebx ecx edx esi edi ebp esp eip fl cs ds es fs gs ss
;	 0   4   8   12  16  20  24  28  32  36 40 42 44 46 48 50

		; int setjmp(jmp_buf) ;

_setjmp:	PUSH 	EDI		; preserve EDI
		MOV 	EDI,[ESP+8]	; &jmp_buf
		MOV 	[EDI],EAX	; save EAX
		MOV 	[EDI+4],EBX	; save EBX
		MOV 	[EDI+8],ECX	; save ECX
		MOV 	[EDI+12],EDX	; save EDX
		MOV 	[EDI+16],ESI	; save ESI
		MOV 	EAX,[ESP]	; save
		MOV 	[EDI+20],EAX	;      EDI
		MOV 	[EDI+24],EBP	; save EBP
		LEA	EAX,[ESP+12]	; save
		MOV 	[EDI+28],EAX	;      ESP
		MOV 	EAX,[ESP+4]	; save
		MOV 	[EDI+32],EAX	;      EIP
		PUSHF			; save
		POP 	DWORD [EDI+36]	;      EFlags
		MOV 	WORD [EDI+40],CS; save CS
		MOV 	WORD [EDI+42],DS; save DS
		MOV 	WORD [EDI+44],ES; save ES
		MOV 	WORD [EDI+46],FS; save FS
		MOV 	WORD [EDI+48],GS; save GS
		MOV 	WORD [EDI+50],SS; save SS
		XOR 	EAX,EAX		; return 0
		POP 	EDI		; restore EDI
		RET

	; void longjmp(jmp_buf, int) ;

_longjmp:	MOV 	EDI,[ESP+4]
		MOV 	EAX,[ESP+8]
		MOV 	[EDI],EAX
		MOV 	FS,[EDI+46]
		MOV 	GS,[EDI+48]
		MOV 	EBX,[EDI+4]
		MOV 	ECX,[EDI+8]
		MOV 	EDX,[EDI+12]
		MOV 	EBP,[EDI+24]

	; Now for some uglyness.  The jmp_buf structure may be ABOVE the
	; point on the new SS:ESP we are moving to.  We don't allow overlap,
	; but do force that it always be valid.  We will use ES:ESI for
	; our new stack before swapping to it.

		MOV 	ES,[EDI+50]
		MOV 	ESI,[EDI+28]
		SUB 	ESI,24

		MOVZX 	EAX,WORD [EDI+42]
		ES
		MOV 	[ESI],EAX	; DS

		MOV 	EAX,[EDI+20]
		ES
		MOV 	[ESI+4],EAX	; EDI

		MOV 	EAX,[EDI+16]
		ES
		MOV 	[ESI+8],EAX	; ESI

		MOV 	EAX,[EDI+32]
		ES
		MOV 	[ESI+12],EAX	; EIP - start of IRET frame

		MOV 	EAX,[EDI+40]
		ES
		MOV 	[ESI+16],EAX	; CS

		MOV 	EAX,[EDI+36]
		ES
		MOV 	[ESI+20],EAX	; EFLAGS

		MOV 	EAX,[EDI]
		MOV 	ES,[EDI+44]
 
		MOV 	SS,[EDI+50]
		MOV 	ESP,ESI

		POP 	DS
		POP 	EDI
		POP 	ESI
		IRET 		; Actually jump to new CS:EIP loading EFLAGS
 