; ============================================================
; File: CHECKCPU.ASM
;
; Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall
;
; Purpose: Verifies a 386 or better CPU.
;
; Interrupts: Already disabled; remain disabled.
;
; Designed for use with the NASM protected mode 386 assembler.
;
; Modification History:
;
%define		 BUG_FEB_20_2002
;		 Found by Wayne Gibson (wgibson@speakeasy.net).
;		 Execution of LGDT instruction in real mode still had
;                default size of 64KB for DS. If data segment contained
;                more than 64KB, the LGDT instruction would cause an
;                exception. Solution was to create a new "SECTION .init"
;                and place Init_CPU and Check_CPU code, as well as the
;                the gdt and gdt_info data structures. Also required
;		 change to LDSCRIPT and resulting LINK.CMD file.
;
; ============================================================

%ifdef BUG_FEB_20_2002
		SECTION	.init
%else
		SECTION	.text
%endif
		BITS	32
		GLOBAL	Check_CPU
		EXTERN	Init_CPU

	; Note: This code is executed while still in real mode!

; ----------------------------------------------------------------
Check_CPU:	; Check for 386 or better CPU
; ----------------------------------------------------------------
		MOV	EAX,CS			; Setup a temp stack
		MOV	DS,EAX
		MOV	SS,EAX
		MOV	ESP,TopOfStack
		CLD

		PUSHF				; AX <- Flags
		POP	EAX
		OR	AH,70h			; Set AX<12..14>
		PUSH	EAX			; Flags <- AX
		POPF
		PUSHF				; AX <- Flags
		POP	EAX
		XOR	AH,70h			; Toggle AX<12..14>
		TEST	AH,70h			; Should now be zeroes
		JNZ	BadCPU
		PUSH	EAX			; Flags <- AX
		POPF
		PUSHF				; AX <- Flags
		POP	EAX
		TEST	AH,70h			; Should be zeroes
		JNZ	BadCPU
		DB	66h
		JMP	Init_CPU

; ----------------------------------------------------------------
BadCPU:		; Clear screen, display error message, and halt.
; ----------------------------------------------------------------
		DB	66h
		MOV	EBX,BadCPU0
		XOR	AL,AL		; 1st use segment B000
		MOV	AH,0B0h
		MOV	ES,EAX
		CALL	EBX
		XOR	AL,AL		; 2nd use segment B800
		MOV	AH,0B8h
		MOV	ES,EAX
		CALL	EBX
		HLT

BadCPU0:	XOR	EDI,EDI		; Erase the entire screen
		DB	66h
		MOV	ECX,25*80
		DB	66h
		MOV	EAX,0720h
		REP	STOSD
		DB	66h		; Display the error message
		MOV	ESI,BadCPUMsg
		DB	66h
		MOV	EDI,2*(12*80+25); Row 12, Col 25 (screen center)
		MOV	AH,07h
BadCPU1:	LODSB
		OR	AL,AL
		JZ	BadCPU2
		STOSD
		JMP	SHORT BadCPU1
BadCPU2:	RET

BadCPUMsg:	DB	'Requires a 386 or better CPU!',0

		DD	0,0,0,0
TopOfStack:

