OBJ = bio.o\
      getc.o\
      putc.o\
      queue.o\
      setds.o

AS = as86
CC = bcc
AR = ar
../mtxlib:$(OBJ)
	$(AR) r $@ $^
bio.o:bio.c
	$(CC) -c -ansi $< -o $@
getc.o:getc.s
	$(AS) $< -o $@
putc.o:putc.s
	$(AS) $< -o $@
queue.o:queue.c
	$(CC) -c -ansi $< -o $@
setds.o:setds.s
	$(AS) $< -o $@

