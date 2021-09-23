%macro no_error_code_irq_handler %1
global irq_handler_%1
irq_handler_%1:
	push dword 0	; for the IRQs that don't have an error code, we
			; use 0 as a sentinel value
	push dword %1	; the IRQ number
	jmp common_irq_handler
%endmacro

%macro error_code_irq_handler %1
global irq_handler_%1
irq_handler_%1:
	push dword %1	; the IRQ number
	jmp common_irq_handler
%endmacro

common_irq_handler:
	;
	; We must store the context in which the CPU was before running
	; the IRQ handler. By pushing all the usable registers into the
	; stack (they will fill the cpu_state struct), we can later
	; restore them.
	;
	push edi
	push esi
	push ebp
	push edx
	push ecx
	push ebx
	push eax
	push esp

	; call the C generic handler
	call interrup_handler

	; restore the context
	pop esp
	pop eax
	pop ebx
	pop ecx
	pop edx
	pop ebp
	pop esi
	pop edi

	add esp, 8

	iret

no_error_irq_handler 0
no_error_irq_handler 1
no_error_irq_handler 2
no_error_irq_handler 3
no_error_irq_handler 4
no_error_irq_handler 5
no_error_irq_handler 6
no_error_irq_handler 7
error_irq_handler 8
no_error_irq_handler 9
error_irq_handler 10
error_irq_handler 11
error_irq_handler 12
error_irq_handler 13
error_irq_handler 14
no_error_irq_handler 15
no_error_irq_handler 16
error_irq_handler 17

global load_idt
;
; stack: [esp + 4] - the address of the first entry
;	[esp] - the return address
;
load_idt:
	mov eax, [esp + 4]
	lidt eax
	ret
