%line 1+1 interrupt.s







%line 16+1 interrupt.s



%line 26+1 interrupt.s

[global isr0]
%line 27+0 interrupt.s
 isr0:
 cli
 push byte 0
 push byte 0
 jmp isr_common_stub
%line 28+1 interrupt.s
[global isr1]
%line 28+0 interrupt.s
 isr1:
 cli
 push byte 0
 push byte 1
 jmp isr_common_stub
%line 29+1 interrupt.s
[global isr2]
%line 29+0 interrupt.s
 isr2:
 cli
 push byte 0
 push byte 2
 jmp isr_common_stub
%line 30+1 interrupt.s
[global isr3]
%line 30+0 interrupt.s
 isr3:
 cli
 push byte 0
 push byte 3
 jmp isr_common_stub
%line 31+1 interrupt.s
[global isr4]
%line 31+0 interrupt.s
 isr4:
 cli
 push byte 0
 push byte 4
 jmp isr_common_stub
%line 32+1 interrupt.s
[global isr5]
%line 32+0 interrupt.s
 isr5:
 cli
 push byte 0
 push byte 5
 jmp isr_common_stub
%line 33+1 interrupt.s
[global isr6]
%line 33+0 interrupt.s
 isr6:
 cli
 push byte 0
 push byte 6
 jmp isr_common_stub
%line 34+1 interrupt.s
[global isr7]
%line 34+0 interrupt.s
 isr7:
 cli
 push byte 0
 push byte 7
 jmp isr_common_stub
%line 35+1 interrupt.s
[global isr8]
%line 35+0 interrupt.s
 isr8:
 cli
 push byte 8
 jmp isr_common_stub
%line 36+1 interrupt.s
[global isr9]
%line 36+0 interrupt.s
 isr9:
 cli
 push byte 0
 push byte 9
 jmp isr_common_stub
%line 37+1 interrupt.s
[global isr10]
%line 37+0 interrupt.s
 isr10:
 cli
 push byte 10
 jmp isr_common_stub
%line 38+1 interrupt.s
[global isr11]
%line 38+0 interrupt.s
 isr11:
 cli
 push byte 11
 jmp isr_common_stub
%line 39+1 interrupt.s
[global isr12]
%line 39+0 interrupt.s
 isr12:
 cli
 push byte 12
 jmp isr_common_stub
%line 40+1 interrupt.s
[global isr13]
%line 40+0 interrupt.s
 isr13:
 cli
 POP EAX
 push byte 13
 jmp isr_common_stub
%line 41+1 interrupt.s
[global isr14]
%line 41+0 interrupt.s
 isr14:
 cli
 POP EAX
 push byte 14
 jmp isr_common_stub
%line 42+1 interrupt.s
[global isr15]
%line 42+0 interrupt.s
 isr15:
 cli
 push byte 0
 push byte 15
 jmp isr_common_stub
%line 43+1 interrupt.s
[global isr16]
%line 43+0 interrupt.s
 isr16:
 cli
 push byte 0
 push byte 16
 jmp isr_common_stub
%line 44+1 interrupt.s
[global isr17]
%line 44+0 interrupt.s
 isr17:
 cli
 push byte 0
 push byte 17
 jmp isr_common_stub
%line 45+1 interrupt.s
[global isr18]
%line 45+0 interrupt.s
 isr18:
 cli
 push byte 0
 push byte 18
 jmp isr_common_stub
%line 46+1 interrupt.s
[global isr19]
%line 46+0 interrupt.s
 isr19:
 cli
 push byte 0
 push byte 19
 jmp isr_common_stub
%line 47+1 interrupt.s
[global isr20]
%line 47+0 interrupt.s
 isr20:
 cli
 push byte 0
 push byte 20
 jmp isr_common_stub
%line 48+1 interrupt.s
[global isr21]
%line 48+0 interrupt.s
 isr21:
 cli
 push byte 0
 push byte 21
 jmp isr_common_stub
%line 49+1 interrupt.s
[global isr22]
%line 49+0 interrupt.s
 isr22:
 cli
 push byte 0
 push byte 22
 jmp isr_common_stub
%line 50+1 interrupt.s
[global isr23]
%line 50+0 interrupt.s
 isr23:
 cli
 push byte 0
 push byte 23
 jmp isr_common_stub
%line 51+1 interrupt.s
[global isr24]
%line 51+0 interrupt.s
 isr24:
 cli
 push byte 0
 push byte 24
 jmp isr_common_stub
%line 52+1 interrupt.s
[global isr25]
%line 52+0 interrupt.s
 isr25:
 cli
 push byte 0
 push byte 25
 jmp isr_common_stub
%line 53+1 interrupt.s
[global isr26]
%line 53+0 interrupt.s
 isr26:
 cli
 push byte 0
 push byte 26
 jmp isr_common_stub
%line 54+1 interrupt.s
[global isr27]
%line 54+0 interrupt.s
 isr27:
 cli
 push byte 0
 push byte 27
 jmp isr_common_stub
%line 55+1 interrupt.s
[global isr28]
%line 55+0 interrupt.s
 isr28:
 cli
 push byte 0
 push byte 28
 jmp isr_common_stub
%line 56+1 interrupt.s
[global isr29]
%line 56+0 interrupt.s
 isr29:
 cli
 push byte 0
 push byte 29
 jmp isr_common_stub
%line 57+1 interrupt.s
[global isr30]
%line 57+0 interrupt.s
 isr30:
 cli
 push byte 0
 push byte 30
 jmp isr_common_stub
%line 58+1 interrupt.s
[global isr31]
%line 58+0 interrupt.s
 isr31:
 cli
 push byte 0
 push byte 31
 jmp isr_common_stub
%line 59+1 interrupt.s


[extern isr_handler]




isr_common_stub:
 pusha

 mov ax, ds
 push eax

 mov ax, 0x10
 mov ds, ax
 mov es, ax
 mov fs, ax
 mov gs, ax

 call isr_handler

 pop ebx
 mov ds, bx
 mov es, bx
 mov fs, bx
 mov gs, bx

 popa
 add esp, 8
 sti
 iret
