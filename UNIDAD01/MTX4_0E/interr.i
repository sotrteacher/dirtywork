%line 1+1 interrupt.s







%line 17+1 interrupt.s



%line 27+1 interrupt.s



%line 38+1 interrupt.s

[global isr0]
%line 39+0 interrupt.s
 isr0:
 cli
 push byte 0
 push byte 0

 jmp isr_common_stub
%line 40+1 interrupt.s
[global isr1]
%line 40+0 interrupt.s
 isr1:
 cli
 push byte 0
 push byte 1

 jmp isr_common_stub
%line 41+1 interrupt.s
[global isr2]
%line 41+0 interrupt.s
 isr2:
 cli
 push byte 0
 push byte 2

 jmp isr_common_stub
%line 42+1 interrupt.s
[global isr3]
%line 42+0 interrupt.s
 isr3:
 cli
 push byte 0
 push byte 3

 jmp isr_common_stub
%line 43+1 interrupt.s
[global isr4]
%line 43+0 interrupt.s
 isr4:
 cli
 push byte 0
 push byte 4

 jmp isr_common_stub
%line 44+1 interrupt.s
[global isr5]
%line 44+0 interrupt.s
 isr5:
 cli
 push byte 0
 push byte 5

 jmp isr_common_stub
%line 45+1 interrupt.s
[global isr6]
%line 45+0 interrupt.s
 isr6:
 cli
 push byte 0
 push byte 6

 jmp isr_common_stub
%line 46+1 interrupt.s
[global isr7]
%line 46+0 interrupt.s
 isr7:
 cli
 push byte 0
 push byte 7

 jmp isr_common_stub
%line 47+1 interrupt.s
[global isr8]
%line 47+0 interrupt.s
 isr8:
 cli
 push byte 8
 jmp isr_common_stub
%line 48+1 interrupt.s






[global isr9]
%line 54+0 interrupt.s
 isr9:
 cli
 push byte 0
 push byte 9

 jmp isr_common_stub
%line 55+1 interrupt.s
[global isr10]
%line 55+0 interrupt.s
 isr10:
 cli
 push byte 10
 jmp isr_common_stub
%line 56+1 interrupt.s
[global isr11]
%line 56+0 interrupt.s
 isr11:
 cli
 push byte 11
 jmp isr_common_stub
%line 57+1 interrupt.s
[global isr12]
%line 57+0 interrupt.s
 isr12:
 cli
 push byte 12
 jmp isr_common_stub
%line 58+1 interrupt.s
[global isr13]
%line 58+0 interrupt.s
 isr13:
 cli
 push byte 13
 jmp isr_common_stub
%line 59+1 interrupt.s
[global isr14]
%line 59+0 interrupt.s
 isr14:
 cli
 push byte 14
 jmp isr_common_stub
%line 60+1 interrupt.s

[global isr15]
%line 61+0 interrupt.s
 isr15:
 cli
 push byte 0
 push byte 15

 jmp isr_common_stub
%line 62+1 interrupt.s
[global isr16]
%line 62+0 interrupt.s
 isr16:
 cli
 push byte 0
 push byte 16

 jmp isr_common_stub
%line 63+1 interrupt.s
[global isr17]
%line 63+0 interrupt.s
 isr17:
 cli
 push byte 0
 push byte 17

 jmp isr_common_stub
%line 64+1 interrupt.s
[global isr18]
%line 64+0 interrupt.s
 isr18:
 cli
 push byte 0
 push byte 18

 jmp isr_common_stub
%line 65+1 interrupt.s
[global isr19]
%line 65+0 interrupt.s
 isr19:
 cli
 push byte 0
 push byte 19

 jmp isr_common_stub
%line 66+1 interrupt.s
[global isr20]
%line 66+0 interrupt.s
 isr20:
 cli
 push byte 0
 push byte 20

 jmp isr_common_stub
%line 67+1 interrupt.s
[global isr21]
%line 67+0 interrupt.s
 isr21:
 cli
 push byte 0
 push byte 21

 jmp isr_common_stub
%line 68+1 interrupt.s
[global isr22]
%line 68+0 interrupt.s
 isr22:
 cli
 push byte 0
 push byte 22

 jmp isr_common_stub
%line 69+1 interrupt.s
[global isr23]
%line 69+0 interrupt.s
 isr23:
 cli
 push byte 0
 push byte 23

 jmp isr_common_stub
%line 70+1 interrupt.s
[global isr24]
%line 70+0 interrupt.s
 isr24:
 cli
 push byte 0
 push byte 24

 jmp isr_common_stub
%line 71+1 interrupt.s
[global isr25]
%line 71+0 interrupt.s
 isr25:
 cli
 push byte 0
 push byte 25

 jmp isr_common_stub
%line 72+1 interrupt.s
[global isr26]
%line 72+0 interrupt.s
 isr26:
 cli
 push byte 0
 push byte 26

 jmp isr_common_stub
%line 73+1 interrupt.s
[global isr27]
%line 73+0 interrupt.s
 isr27:
 cli
 push byte 0
 push byte 27

 jmp isr_common_stub
%line 74+1 interrupt.s
[global isr28]
%line 74+0 interrupt.s
 isr28:
 cli
 push byte 0
 push byte 28

 jmp isr_common_stub
%line 75+1 interrupt.s
[global isr29]
%line 75+0 interrupt.s
 isr29:
 cli
 push byte 0
 push byte 29

 jmp isr_common_stub
%line 76+1 interrupt.s
[global isr30]
%line 76+0 interrupt.s
 isr30:
 cli
 push byte 0
 push byte 30

 jmp isr_common_stub
%line 77+1 interrupt.s
[global isr31]
%line 77+0 interrupt.s
 isr31:
 cli
 push byte 0
 push byte 31

 jmp isr_common_stub
%line 78+1 interrupt.s



















[global irq0]
%line 97+0 interrupt.s
 irq0:
 cli
 push byte 0
 push byte 32
 jmp irq_common_stub
%line 98+1 interrupt.s
[global irq1]
%line 98+0 interrupt.s
 irq1:
 cli
 push byte 0
 push byte 33
 jmp irq_common_stub
%line 99+1 interrupt.s
[global irq2]
%line 99+0 interrupt.s
 irq2:
 cli
 push byte 0
 push byte 34
 jmp irq_common_stub
%line 100+1 interrupt.s
[global irq3]
%line 100+0 interrupt.s
 irq3:
 cli
 push byte 0
 push byte 35
 jmp irq_common_stub
%line 101+1 interrupt.s
[global irq4]
%line 101+0 interrupt.s
 irq4:
 cli
 push byte 0
 push byte 36
 jmp irq_common_stub
%line 102+1 interrupt.s
[global irq5]
%line 102+0 interrupt.s
 irq5:
 cli
 push byte 0
 push byte 37
 jmp irq_common_stub
%line 103+1 interrupt.s
[global irq6]
%line 103+0 interrupt.s
 irq6:
 cli
 push byte 0
 push byte 38
 jmp irq_common_stub
%line 104+1 interrupt.s
[global irq7]
%line 104+0 interrupt.s
 irq7:
 cli
 push byte 0
 push byte 39
 jmp irq_common_stub
%line 105+1 interrupt.s
[global irq8]
%line 105+0 interrupt.s
 irq8:
 cli
 push byte 0
 push byte 40
 jmp irq_common_stub
%line 106+1 interrupt.s
[global irq9]
%line 106+0 interrupt.s
 irq9:
 cli
 push byte 0
 push byte 41
 jmp irq_common_stub
%line 107+1 interrupt.s
[global irq10]
%line 107+0 interrupt.s
 irq10:
 cli
 push byte 0
 push byte 42
 jmp irq_common_stub
%line 108+1 interrupt.s
[global irq11]
%line 108+0 interrupt.s
 irq11:
 cli
 push byte 0
 push byte 43
 jmp irq_common_stub
%line 109+1 interrupt.s
[global irq12]
%line 109+0 interrupt.s
 irq12:
 cli
 push byte 0
 push byte 44
 jmp irq_common_stub
%line 110+1 interrupt.s
[global irq13]
%line 110+0 interrupt.s
 irq13:
 cli
 push byte 0
 push byte 45
 jmp irq_common_stub
%line 111+1 interrupt.s
[global irq14]
%line 111+0 interrupt.s
 irq14:
 cli
 push byte 0
 push byte 46
 jmp irq_common_stub
%line 112+1 interrupt.s
[global irq15]
%line 112+0 interrupt.s
 irq15:
 cli
 push byte 0
 push byte 47
 jmp irq_common_stub
%line 113+1 interrupt.s


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



[extern irq_handler]




irq_common_stub:
 pusha

 mov ax, ds
 push eax

 mov ax, 0x10
 mov ds, ax
 mov es, ax
 mov fs, ax
 mov gs, ax

 call irq_handler

 pop ebx
 mov ds, bx
 mov es, bx
 mov fs, bx
 mov gs, bx

 popa
 add esp, 8
 sti
 iret

