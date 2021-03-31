; ============================================================
; File: IO.ASM
;
; Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall
;
; Purpose: Interrupt Service Routines for LIBEPC
;
; Designed for use with the NASM protected mode 386 assembler.
;
; Modification History:
;
; ============================================================

		SECTION	.data

		GLOBAL	msec, old_tick_isr, old_kybd_isr

msec		DD	0
old_tick_isr	DD	0
old_kybd_isr	DD	0

		SECTION	.text
		BITS	32

		GLOBAL	TimerTickISR

TimerTickISR:	INC	DWORD [msec]
		JMP	DWORD [old_tick_isr]

		GLOBAL	KeyboardISR
		EXTERN	Enqueue

;KeyboardISR:	STI
;		PUSHA
;		IN	AL,64h	; Keyboard Status Port
;		AND	AL,01h	; Data Ready?
;		JZ	KybdISR1
;		IN	AL,60h	; Keyboard Data Port
;		MOVZX	EAX,AL
;		PUSH	EAX
;		CALL	Enqueue
;		ADD	ESP,4
;KybdISR1:	POPA
;		JMP	DWORD [old_kybd_isr]
                

		GLOBAL	ISR_PIC1

ISR_PIC1:	PUSH	EAX
		MOV	AL,20h
		OUT	020h,AL
		POP	EAX
		IRET

		GLOBAL	ISR_PIC2

ISR_PIC2:	PUSH	EAX
		MOV	AL,20h
		OUT	020h,AL
		OUT	0A0h,AL
		POP	EAX
		IRET

		GLOBAL	___main
___main:	RET

		GLOBAL	inportb
inportb:	MOV	DX,[ESP+4]
		IN	AL,DX
		MOVZX	EAX,AL
		RET

		GLOBAL	outportb
outportb:	MOV	DX,[ESP+4]	; port
		MOV	AL,[ESP+8]	; data
		OUT	DX,AL
		MOV	ECX,1000	; delay
delay:		LOOP	delay
		RET

		GLOBAL	_exit
_exit:		CLI
		JMP	$

;		GLOBAL	int_off
;int_off:
;                PUSHFL
;                CLI
;                POP     EAX
;                RET

;		GLOBAL	int_on
;int_on:
;                PUSH    EBP
;                MOV     EBP,ESP
;                MOV     EAX,[EBP+8]
;                PUSH    EAX
;                POPFL
;                POP     EBP
;                RET

