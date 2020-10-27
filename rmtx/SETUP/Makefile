AS=as -Iinclude
LD=ld
CC=gcc
CPP=gcc -E -nostdinc -Iinclude
CFLAGS=-W -nostdlib -Wno-long-long -I include -fomit-frame-pointer

.s.o:
	${AS} -a $< -o $*.o >$*.map

all: boot apentry setup

boot: boot.o
	${LD} --oformat binary -N -e start -Ttext 0x0 -o boot $<

apentry: apentry.o
	${LD} --oformat binary -N -e start -Ttext 0x0 -o apentry $<

setup: setup.o
	${LD} --oformat binary -N -e start -Ttext 0x0 -o setup $<

clean:
	rm -f boot setup *.o *.map
