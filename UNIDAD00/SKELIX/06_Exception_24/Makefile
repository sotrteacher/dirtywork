AS=as -Iinclude
LD=ld
CC=gcc
CPP=gcc -E -nostdinc -Iinclude
#CFLAGS=-Wall -pedantic -W -nostdlib -nostdinc -Wno-long-long -I include -fomit-frame-pointer
#CFLAGS=-m32 -Wall -pedantic -W -nostdlib -nostdinc -Wno-long-long -I include -fomit-frame-pointer
#CFLAGS=-g -m32 -fno-PIC -Wall -pedantic -W -nostdlib -nostdinc -Wno-long-long -I include -fomit-frame-pointer
CFLAGS = -g -I include -std=gnu99  -nostdlib -m32 -march=i386 -ffreestanding -fno-PIC -Wl,--nmagic,--oformat=binary

KERNEL_OBJS= load.o init.o isr.o timer.o libcc.o scr.o kb.o task.o kprintf.o exceptions.o

#.c.s:
#	${CC} ${CFLAGS} -S -o $*.s $<

.s.o:
	${AS} -a $< -o $*.o >$*.map
#	${AS} -o $*.o $<               # De esta forma tmb funciona en el Tut 2.

all: final.img

final.img: bootsect kernel
	cat bootsect kernel > final.img
	@wc -c final.img

bootsect: bootsect.o
	${LD} --oformat binary -N -e start -Ttext 0x7c00 -o bootsect $<
#bootsect: bootsect.s
#	as -Iinclude -a $< -o $@.o >$*.map
#	${LD} --oformat binary -N -e start -Ttext 0x7c00 -o bootsect $@.o
isr.o: isr.s
	${AS} --32 -a $< -o $@ 
load.o: load.s
#	${AS} --32 -a $< -o $@ 
##	${AS} --32  $< -o $@ 
	gcc $(CFLAGS) -x assembler-with-cpp -c $< -o $@

kernel: ${KERNEL_OBJS}
#	${LD} --oformat binary -N -e pm_mode -Ttext 0x0000 -o $@ ${KERNEL_OBJS}
	${LD} -m elf_i386 --oformat binary -N -e pm_mode -Ttext 0x0000 -o $@ ${KERNEL_OBJS}
	@wc -c kernel

clean:
	rm -f *.img kernel bootsect *.o
run: final.img
	qemu-system-i386 -fda final.img -no-fd-bootchk

dep:
	sed '/\#\#\# Dependencies/q' < Makefile > tmp_make
	(for i in *.c;do ${CPP} -M $$i;done) >> tmp_make
	mv tmp_make Makefile

### Dependencies:
exceptions.o: exceptions.c include/kprintf.h include/scr.h include/asm.h \
  include/task.h include/kernel.h
init.o: init.c include/scr.h include/isr.h include/asm.h include/kernel.h \
  include/task.h include/libcc.h include/timer.h include/kprintf.h \
  include/kb.h
kb.o: kb.c include/asm.h include/scr.h
kprintf.o: kprintf.c include/scr.h include/asm.h include/kprintf.h
libcc.o: libcc.c include/libcc.h
scr.o: scr.c include/asm.h include/scr.h include/libcc.h
task.o: task.c include/task.h include/kernel.h include/asm.h
timer.o: timer.c include/asm.h include/task.h include/kernel.h \
  include/scr.h include/kprintf.h
