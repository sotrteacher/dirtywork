TARGET_PREFIX = /usr/x86_64-linux-uclibc
all: hola_4_1_3.xtn
.c.o:
	gcc -Wall -c $<
hola_4_1_3.xtn: hola.o
#	ld hola.o -static -nostdlib /usr/lib/x86_64-linux-gnu/crt1.o /usr/lib/x86_64-linux-gnu/crti.o /usr/lib/x86_64-linux-gnu/crtn.o -o hola_4_1_3.xtn
#	ld hola.o /usr/lib/x86_64-linux-gnu/crt1.o -o hola_4_1_3.xtn
#	ld hola.o /usr/lib/x86_64-linux-gnu/crti.o -o hola_4_1_3.xtn
#	ld hola.o /usr/lib/x86_64-linux-gnu/crtn.o -o hola_4_1_3.xtn
#	ld hola.o /usr/lib/x86_64-linux-gnu/crtn.o -o hola_4_1_3.xtn
	ld hola.o -static -nostdlib ${TARGET_PREFIX}/usr/lib/crt1.o ${TARGET_PREFIX}/usr/lib/crti.o ${TARGET_PREFIX}/usr/lib/crtn.o -L${TARGET_PREFIX}/usr/lib -lc -o hola_4_1_3.xtn
clean:
	rm -v *.xtn *.o
