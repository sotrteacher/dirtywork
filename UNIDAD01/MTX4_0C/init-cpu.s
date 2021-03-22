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
                ;BITS    16
;----------------------------------------------------------------------------
; Global Descriptor Table (GDT)
;----------------------------------------------------------------------------
;gdt_info:	DW	(3*8)-1	; 16-bit GDT limit
;		DD	gdt	; 32-bit GDT address

gdt:
; Index 0x00 
; Required dummy
null_descriptor:
		DW	0	; seg length <0..15>
		DW	0	; base address <0..15>
		DB	0	; base address <16..23>
		DB	0	; seg type and misc flags
		DB	0	; seg length <16..19> & access flags
		DB	0	; base address <24..31>
;   .quad   0x00

; Index 0x08
; Unused
		DW	0	; seg length <0..15>
		DW	0	; base address <0..15>
		DB	0	; base address <16..23>
		DB	0	; seg type and misc flags
		DB	0	; seg length <16..19> & access flags
		DB	0	; base address <24..31>

; Index 0x10
; protected mode code segment
code_descriptor:
		DW	0FFFFh	; seg length <0..15>
		DW	0	; base address <0..15>
		DB	0	; base address <16..23>
		DB	09Ah	; seg type and misc flags
		DB	0CFh	; seg length <16..19> & access flags
		DB	0	; base address <24..31>
;   .quad 0x00cf9a000000ffff   

; Index 0x18
; protected mode data segment
data_descriptor:
		DW	0FFFFh	; seg length <0..15>
		DW	0	; base address <0..15>
		DB	0	; base address <16..23>
		DB	092h	; seg type and misc flags
		DB	0CFh	; seg length <16..19> & access flags
		DB	0	; base address <24..31>
;   .quad 0x00cf92000000ffff   

                ;BITS    16
; Index 0x20
; 16 bit code segment
;Level 0 code segment descriptor format:
code_sel:       DW      0FFFFh  ;limit (0 - 15)
                DW      0       ;segment base (0 - 15) |(16-31)
                DB      0       ;segment base (16 - 23)|(32-39) 
                DB      9Ah     ;present=1,readable=1  |access byte (40-47) - 10011010
                DB      0       ;limit (16 -19), G=0, D=0|flags + limit  (48-55)
                DB      0       ;base (24 - 31)        |base (56-63)

; Index 0x28
; 16 bit data segment
;Data segment descriptor format
dmy_selec:      DW      0FFFFh  ;limit (0 - 15)
                DW      0       ;segment base (0 - 15) |base (16-31)
                DB      0       ;segment base (16 - 23)|base (32-39)
                DB      092h    ;present=1,writable=1  |access byte (40-47) - 10010010
                DB      0       ;limit (16 -19), G=0, D=0|flags + limit  (48-55)
                DB      0       ;base (24 - 31)        |base (56-63)
gdt_end:
gdt_info:
                DW      gdt_end - gdt - 1 ; Size of GDT
                DW      0000h             ; Upper 2 Bytes of GDT address
                DW      0000h             ; Lower 2 Bytes of GDT address
                ;DD       gdt              ; 32 bit GDT address

CODE_SELECTOR	EQU	code_descriptor - gdt
DATA_SELECTOR	EQU	data_descriptor - gdt
prot_enable     EQU     01h             ; protected mode enable bit in CR0
SSP             EQU     32*1024

		GLOBAL	code_selector	; (Needed in init-idt.c)
code_selector:	DW	CODE_SELECTOR

; IDT for protected mode
idt_info:
                DW      0000h
                DW      0000h
                DW      0000h

; IDT for real mode
idt_real_info:
                DW      03ffh 
                DW      0000h
                DW      0000h

		SECTION	.start
		BITS	32

		GLOBAL	start
;============================================================================
; Start at phys adrs 0, real mode, A20-Gate Open, IF=0, CS=0, & IP=0 ...
;============================================================================
;start:		CLI			; no interrupts during initialization
;		DB	66h		; (JMP assembled with 32-bit offset)
;		JMP	Check_CPU

%ifdef BUG_FEB_20_2002
		SECTION	.init
%else
		SECTION	.text
%endif
		BITS	32

		GLOBAL	Init_CPU

		EXTERN	Check_CPU
		EXTERN	Init8259
		EXTERN	Init8253
		EXTERN	Init_IDT
		EXTERN	Init_CRT
		;EXTERN	_main
		EXTERN	main
                EXTERN  text_frst
                EXTERN  data_frst
                EXTERN  stack_frst
                EXTERN  stack_last

;Init_CPU:	XOR	EAX,EAX		; Still in Real Mode, so
;		MOV	DS,EAX		; this is really AX
;STEP 19:  return to real-address mode
Init_CPU:       
                ; Setup stack.
                MOV     ESP,00007000h
                MOV     EBP,ESP

                ; Setup GDT
                ;MOV     [gdt_info + 2],gdt
                MOV     EAX,gdt_info + 2
                ADD     EAX,2
                MOV     DWORD [EAX],gdt  
                ; Interrupts should already be disabled...
                CLI                     ; Disable interrupt
                LGDT    [gdt_info]
    ; *********************
    ; Return to real mode *
    ; *********************

    ; Load a new segment with a limit of 0xffff
    ; This is the segment limit required in real-
    ; address mode.
                JMP     20h:loadRMSeg
loadRMSeg:
                ;;MOV     AX,dmy_selec    ; dummy selector   <-----------------------
                ;MOV     AX,text_frst    ; dummy selector   <-----------------------
                MOV     EAX,28h
                MOV     DS,AX
                MOV     GS,AX
                MOV     FS,AX
                MOV     ES,AX           ; reset segment registers
                MOV     SS,AX

        ; This line allows the interrupt to work.
        LIDT   [idt_real_info]
                MOV     EAX,CR0         ; load the content of CR0
                ;AND     EAX,~prot_enable; disable protected mode
                AND     EAX,0FFFFFFFEh; disable protected mode
                MOV     CR0,EAX         ; restore the content of CR0
                ;DB      0EAh            ; far jump to flush instruction queue
                ;DW      next_instruction; new EIP           <-----------------------
                ;DW      code            ; new CS
                ;DW      01000h          ; new CS
                ;DW      0000h          ; new CS
                JMP      00h:next_instruction
; STEP 20: execute in real-address mode, set DS, SS and SP
next_instruction:
;                MOV     AX,Gdata        ; get data segment address
                MOV     AX,0000h
                MOV     DS,AX           ; put in DS
                MOV     GS,AX
                MOV     FS,AX
                MOV     ES,AX           ; reset segment registers
;                MOV     AX,stk0        ; get stack segment address
                MOV     SS,AX           ; put in SS
   ; Enable interrupts for memory checking.
   STI
                ;MOV     SP,SSP          ; set stack pointer

                XOR	EAX,EAX		; Still in Real Mode, so
                MOV	DS,EAX		; this is really AX
		DB	66h		; Need 32-bit operand size.
		DB	67h		; (LGDT assembled with 32-bit offset)
		LGDT	[gdt_info]	; load GDT register
;---------------------------------------------
		CALL	Init_IDT	; Interrupt Descriptor Table
;---------------------------------------------

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
ProtMode: 	MOV	AL,DATA_SELECTOR; load all other selectors
		XOR	AH,AH
		MOV	DS,EAX
		MOV	ES,EAX
		MOV	FS,EAX
		MOV	GS,EAX
		MOV	SS,EAX
;2021.03.19.15.45
 	 	LEA	ESP,[stack_last + 1]
		CALL	Init_CRT	; C Run-Time Environment
		CALL	Init8259	; Programmable Interrupt Controller
		CALL	Init8253	; Programmable Interrupt Timer
		;CALL	Init_IDT	; Interrupt Descriptor Table
		;CALL	_main		; User's Embedded Application
		CALL	main		; User's Embedded Application
		JMP	$
		HLT

