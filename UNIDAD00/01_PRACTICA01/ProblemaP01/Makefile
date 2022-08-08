CC = gcc
LD = ld

TARGET_PREFIX = /usr/x86_64-linux-uclibc
STARTUP_FILES = ${TARGET_PREFIX}/usr/lib/crt1.o ${TARGET_PREFIX}/usr/lib/crti.o
END_FILES = ${TARGET_PREFIX}/usr/lib/crtn.o

LINKED_FILES = ${STARTUP_FILES} ${OBJS} ${END_FILES}
LIBS = -lc
USER_CFLAGS += -static -nostdlib ${LINKED_FILES} -L${TARGET_PREFIX}/usr/lib ${LIBS}

all: lock.xtn  mycatl.xtn
.c.o:
	$(CC) -Wall -c $<
lock.xtn:lock.o
	$(LD) $< ${USER_CFLAGS} -o $@
mycatl.xtn:mycatl.o
	$(LD) $< ${USER_CFLAGS} -o $@
clean:
	rm -v *.xtn *.o

