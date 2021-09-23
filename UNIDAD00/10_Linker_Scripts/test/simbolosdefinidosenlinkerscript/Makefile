CC=gcc
LD=ld
CFLAGS=-fno-builtin -m32

all:test

printf.o: printf.c Cfile.c
	${CC} ${CFLAGS} -c $< 

Cfile.o:Cfile.c
	${CC} ${CFLAGS} -c $<

#test:printf.o Cfile.o
test: Cfile.o
	${LD} -T link.ld $^ -o $@

clean:
	rm -v test *.o
