OBJ = bio.o\
      getc.o\
      putc.o\
      queue.o\
      setds.o\
      diskio.o\
      loader.o

AS = as86
CC = bcc
AR = ar86
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
diskio.o:diskio.s
	$(AS) $< -o $@
loader.o:loader.c
	$(CC) -c -ansi $< -o $@
