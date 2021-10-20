; ============================================================
; File: CYCLES.ASM
;
; Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall
;
; Purpose: Gets elapsed CPU Clock Cycles from Pentium
;
; Interrupts: Already disabled; remain disabled.
;
; Designed for use with the NASM protected mode 386 assembler.
;
; Modification History:
;
; ============================================================

		SECTION	.data
has_RDTSC	DB	0

		SECTION	.text
		BITS	32

		GLOBAL	_CPU_Clock_Cycles

		; QWORD64 CPU_Clock_Cycles(void) ;

_CPU_Clock_Cycles:
		CMP	BYTE [has_RDTSC],1
		JNE	check4_RDTSC
		RDTSC
		RET

	; CPU has never been checked - do it now.

check4_RDTSC:	MOV	BYTE [has_RDTSC],1	; assume the best

		PUSHF				; get eflags
		POP	EAX
		OR	EAX,200000H		; Set bit 21 (ID Flag)
		PUSH	EAX
		POPF
		PUSHF				; get eflags
		POP	EAX
		TEST	EAX,200000H		; is ID set?
		JZ	no_CPUID
		AND	EAX,~200000H		; Clear ID
		PUSH	EAX
		POPF
		PUSHF				; get eflags
		POP	EAX
		TEST	EAX,200000H		; is ID clear?
		JNZ	no_CPUID

	; This CPU has the CPUID instruction. This instruction
	; modifies the contents of registers EAX, EBX, ECX, and
	; EDX. The gcc compiler requires that EBX be preserved.

		PUSH	EBX			; preserve EBX

		XOR	EAX,EAX
		CPUID
		CMP	EAX,1
		JB	no_CPUID

		XOR	DL,DL
		MOV	EAX,1
		CPUID
		
		AND	AH,0FH
		CMP	AH,5			; CPU Family = 5?
		JB	not_Pentium

		TEST	DL,10H			; RDTSC supported?
		JZ	no_RDTSC

	; Test the RDTSC instruction...

		RDTSC
		MOV	EBX,EAX
		MOV	ECX,EDX
		RDTSC
		CMP	EDX,ECX
		JB	no_RDTSC
		JA	_CPU_Clock_Cycles
		CMP	EAX,EBX
		POP	EBX			; Restore EBX
		JB	no_RDTSC
		JA	_CPU_Clock_Cycles

not_Pentium:
no_RDTSC:
no_CPUID:
		INT	06h			; Illegal opcode.

