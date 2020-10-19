AS = as86
CC = bcc
LD = ld86
MTXLIBSRCC = MTXLIB.src/bio.c MTXLIB.src/queue.c \
             MTXLIB.src/getc.s MTXLIB.src/putc.s MTXLIB.src/setds.s

mtx:ts.s t.c mtxlib
	$(AS) -o ts.o ts.s
	$(CC)  -o t.o -c -ansi t.c
	$(LD) -d -o mtx ts.o t.o mtxlib /usr/lib/bcc/libc.a
run:mtx booter 
	cp -v mtx ./mount_point/boot/
#	genext2fs -v -b 1440 -d ./mount_point/  mFDimage
	genext2fs -b 1440 -d ./mount_point/  mFDimage
#	dd if=FDimage.img of=mFDimage count=2 conv=notrunc
	dd if=booter of=mFDimage bs=1024 count=1 conv=notrunc
	qemu-system-i386 -fda mFDimage -no-fd-bootchk
booter:mtxFS/bs.s mtxFS/bc.c
	@echo "compiling booter ..."
	$(AS) -o bs.o mtxFS/bs.s
	$(CC) -o bc.o -c -ansi mtxFS/bc.c
	@echo "linking booter ....."
	$(LD) -d -o booter bs.o bc.o /usr/lib/bcc/libc.a
mtxlib:$(MTXLIBSRC)
	cd MTXLIB.src/ && $(MAKE)
mFDimage:mtx booter
	cp -v mtx ./mount_point/boot/
	genext2fs -b 1440 -d ./mount_point/  mFDimage
	dd if=booter of=mFDimage bs=1024 count=1 conv=notrunc
loader:loader_mtx.s
	as86 -o loader_mtx.o loader_mtx.s
	ld86 -d -o loader loader_mtx.o
rwl: loader # run with loader
	dd if=loader of=mFDimage bs=512 count=1 conv=notrunc
	dd if=mtx of=mFDimage seek=1 bs=512 conv=notrunc
	qemu-system-i386 -fda mFDimage -no-fd-bootchk

clean:
	rm -v *.o MTXLIB.src/*.o mtx ./mount_point/boot/mtx booter mFDimage mtxlib

