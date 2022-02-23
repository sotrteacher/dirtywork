# Archivo make para crear un programa hola mundo enlazado est\'aticamente 
# con la biblioteca uClibc (versi\'on 1.0.39), 2022.02.07.
CC = gcc
LD = ld

TARGET_PREFIX = /usr/x86_64-linux-uclibc
STARTUP_FILES = ${TARGET_PREFIX}/usr/lib/crt1.o ${TARGET_PREFIX}/usr/lib/crti.o
END_FILES = ${TARGET_PREFIX}/usr/lib/crtn.o
LINKED_FILES = ${STARTUP_FILES} ${OBJS} ${END_FILES}
LIBS = -lc
USER_CFLAGS += -static -nostdlib ${LINKED_FILES} -L${TARGET_PREFIX}/usr/lib ${LIBS}

all: holauclibc.xtn
# Suffix Rule
.c.o:
	$(CC) -Wall -c $<
holauclibc.xtn:hola.o
	$(LD) $< ${USER_CFLAGS} -o $@
clean:
	rm -v holauclibc.xtn *.o

