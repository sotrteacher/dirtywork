CC=gcc
CFLAGS=-fno-builtin -m32



test: printf.c TestPrintf.c
	${CC} ${CFLAGS} $? -o $@


clean:
	rm -v test
