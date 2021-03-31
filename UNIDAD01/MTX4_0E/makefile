# Makefile for JamesM's kernel tutorials.
# The C and C++ rules are already setup by default.
# The only one that needs changing is the assembler 
# rule, as we use nasm instead of GNU as.

#SOURCES=boot.o tswitch.o t.o display.o io.o init-idt.o timer.o init-cpu.o \
#init-crt.o init8259.o init8253.o checkcpu.o descriptor_tables.o common.o \
#gdt.o interrupt.o isr.o monitor.o pit_timer.o keyb.o lowlevel.o
SOURCES=boot.o tswitch.o t.o io.o  \
descriptor_tables.o common.o \
gdt.o interrupt.o isr.o monitor.o pit_timer.o keyb.o lowlevel.o

CFLAGS=-m32 -fno-PIC -nostdlib -nostdinc -fno-builtin -fno-stack-protector
LDFLAGS=-m elf_i386 -Tlink.ld
ASFLAGS=-felf

all: $(SOURCES) link

clean:
	-rm -v *.o kernel

link:
	ld $(LDFLAGS) -o kernel $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<

newrun:all
	qemu-system-i386 -kernel kernel

#Not working :-(, why?
qemu-nox:
	qemu-system-i386 -serial mon:stdio -kernel kernel 
#	qemu-system-i386 -kernel kernel -serial mon:stdio -nographic

