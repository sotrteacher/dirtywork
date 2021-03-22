; ============================================================
; File: FIXEDPT.ASM
;
; Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall
;
; Purpose: Arithmetic operations for 16.16 and 32.32 format
; fixed-point real numbers.
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

		GLOBAL	_Product32, _Quotient32, _Inverse32, _Sqrt32
		GLOBAL	_Product64

		; FIXED32 Product32(FIXED32 mpier32, FIXED32 mpcnd32)

%define	mpcnd32		[ESP+8]
%define	mpier32		[ESP+4]

_Product32:	MOV	EAX,mpcnd32
		IMUL	DWORD mpier32
		SHRD	EAX,EDX,16
		RET

		; FIXED32 Quotient32(FIXED32 dividend32, FIXED32 divisor32)

%define	divisor32	[ESP+8]
%define	dividend32	[ESP+4]

_Quotient32:	MOV	EDX,dividend32
		XOR	EAX,EAX
		SHRD	EAX,EDX,16
		SAR	EDX,16
		IDIV	DWORD divisor32
		RET

		; FIXED32 Inverse32(FIXED32 operand32)

%define	operand32	[ESP+4]

_Inverse32:	XOR	EAX,EAX
		MOV	EDX,1
		IDIV	DWORD operand32
		RET

		; FIXED32 Sqrt32(FIXED32 operand32)

_Sqrt32:	PUSH	EBX
		XOR	EAX,EAX
		MOV	EBX,40000000h
		MOV	ECX,operand32
sqrtHP1:	MOV	EDX,ECX
		SUB	EDX,EBX
		JB	sqrtHP2
		SUB	EDX,EAX
		JB	sqrtHP2
		MOV	ECX,EDX
		SHR	EAX,1
		OR	EAX,EBX
		SHR	EBX,2
		JNZ	sqrtHP1
		JMP	sqrtHP5
sqrtHP2:	SHR 	EAX,1
		SHR	EBX,2
		JNZ	sqrtHP1
sqrtHP5:	MOV	EBX,00004000h
		SHL	EAX,16
		SHL	ECX,16
sqrtHP3:	MOV	EDX,ECX
		SUB	EDX,EBX
		JB	sqrtHP4
		SUB	EDX,EAX
		JB	sqrtHP4
		MOV	ECX,EDX
		SHR	EAX,1
		OR	EAX,EBX
		SHR	EBX,2
		JNZ	sqrtHP3
		JMP	sqrtHP6
sqrtHP4:	SHR	EAX,1
		SHR	EBX,2
		JNZ	sqrtHP3
sqrtHP6:	POP	EBX
		RET

		; FIXED64 Product64(FIXED64 mpier64, FIXED64 mpcnd64)

%define	mpier64_hi	[ESP+20]
%define	mpier64_lo	[ESP+16]
%define	mpcnd64_hi	[ESP+12]
%define	mpcnd64_lo	[ESP+8]

_Product64:	PUSH	EBX

		MOV	EAX,mpcnd64_lo	; 1st cross-product
		MUL	DWORD mpier64_lo
		MOV	EBX,EDX
		XOR	ECX,ECX

		MOV	EAX,mpcnd64_lo	; 2nd cross-product
		MUL	DWORD mpier64_hi
		ADD	EBX,EAX
		ADC	ECX,EDX

		MOV	EAX,mpier64_lo	; 3rd cross-product
		MUL	DWORD mpcnd64_hi
		ADD	EBX,EAX
		ADC	ECX,EDX

		MOV	EAX,mpcnd64_hi	; 4th cross-product
		IMUL	EAX,mpier64_hi
		ADD	ECX,EAX

		MOV	EAX,mpcnd64_hi	; 1st correction term
		CDQ
		AND	EDX,mpier64_lo
		SUB	ECX,EDX

		MOV	EAX,mpier64_hi	; 2nd correction term
		CDQ
		AND	EDX,mpcnd64_lo
		SUB	ECX,EDX

		MOV	EAX,EBX		; leave results in EDX.EAX
		MOV	EDX,ECX

		POP	EBX
		RET

