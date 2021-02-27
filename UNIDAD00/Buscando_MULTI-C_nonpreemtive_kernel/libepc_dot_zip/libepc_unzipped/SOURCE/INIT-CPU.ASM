; ============================================================
; File: INIT-CPU.ASM
;
; Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall
;
; Purpose: Switches the processor from real to protected mode,
; establishes a flat memory model with all segments starting
; at address zero and set to a size of 4GB.
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
		SECTION	.data
%endif
;----------------------------------------------------------------------------
; Global Descriptor Table (GDT)
;----------------------------------------------------------------------------
gdt_info:	DW	(3*8)-1	; 16-bit GDT limit
		DD	gdt	; 32-bit GDT address

gdt:

null_descriptor:
		DW	0	; seg length <0..15>
		DW	0	; base address <0..15>
		DB	0	; base address <16..23>
		DB	0	; seg type and misc flags
		DB	0	; seg length <16..19> & access flags
		DB	0	; base address <24..31>

code_descriptor:
		DW	0FFFFh	; seg length <0..15>
		DW	0	; base address <0..15>
		DB	0	; base address <16..23>
		DB	09Ah	; seg type and misc flags
		DB	0CFh	; seg length <16..19> & access flags
		DB	0	; base address <24..31>

data_descriptor:
		DW	0FFFFh	; seg length <0..15>
		DW	0	; base address <0..15>
		DB	0	; base address <16..23>
		DB	092h	; seg type and misc flags
		DB	0CFh	; seg length <16..19> & access flags
		DB	0	; base address <24..31>

CODE_SELECTOR	EQU	code_descriptor - gdt
DATA_SELECTOR	EQU	data_descriptor - gdt

		GLOBAL	code_selector	; (Needed in init-idt.c)
code_selector:	DW	CODE_SELECTOR

		SECTION	.start
		BITS	32

		GLOBAL	start
;============================================================================
; Start at phys adrs 0, real mode, A20-Gate Open, IF=0, CS=0, & IP=0 ...
;============================================================================
start:		CLI			; no interrupts during initialization
		DB	66h		; (JMP assembled with 32-bit offset)
		JMP	Check_CPU

%ifdef BUG_FEB_20_2002
		SECTION	.init
%else
		SECTION	.text
%endif
		BITS	32

		GLOBAL	Init_CPU

		EXTERN	Check_CPU
		EXTERN	stack_last
		EXTERN	Init8259
		EXTERN	Init8253
		EXTERN	Init_IDT
		EXTERN	Init_CRT
		EXTERN	_main

Init_CPU:	XOR	EAX,EAX		; Still in Real Mode, so
		MOV	DS,EAX		; this is really AX

		DB	66h		; Need 32-bit operand size.
		DB	67h		; (LGDT assembled with 32-bit offset)
		LGDT	[gdt_info]	; load GDT register

		MOV	EAX,CR0		; get CR0 into EAX
		OR	AL,1		; set Protected Mode bit
		MOV	CR0,EAX		; go to Protected Mode!
		DB	66h
		DB	0EAh		; far jump (to set CS)
		DD	ProtMode
		DW	CODE_SELECTOR

;============================================================================
; Execution continues here in protected mode ...
;============================================================================
ProtMode:	MOV	AL,DATA_SELECTOR; load all other selectors
		XOR	AH,AH
		MOV	DS,EAX
		MOV	ES,EAX
		MOV	FS,EAX
		MOV	GS,EAX
		MOV	SS,EAX
		LEA	ESP,[stack_last + 1]
		CALL	Init_CRT	; C Run-Time Environment
		CALL	Init8259	; Programmable Interrupt Controller
		CALL	Init8253	; Programmable Interrupt Timer
		CALL	Init_IDT	; Interrupt Descriptor Table
		CALL	_main		; User's Embedded Application
		JMP	$
		HLT

