AS = as86
CC = bcc
LD = ld86

mtx:ts.s t.c
	$(AS) -o ts.o ts.s
	$(CC)  -o t.o -c -ansi t.c
	$(LD) -d -o mtx ts.o t.o mtxlib /usr/lib/bcc/libc.a
run:mtx booter
	cp -v mtx ./mount_point/boot/
	genext2fs -v -b 1440 -d ./mount_point/  mFDimage
#	dd if=FDimage.img of=mFDimage count=2 conv=notrunc
	dd if=booter of=mFDimage bs=1024 count=1 conv=notrunc
	qemu-system-i386 -fda mFDimage -no-fd-bootchk
booter:mtxFS/bs.s mtxFS/bc.c
	@echo "compiling booter ..."
	$(AS) -o bs.o mtxFS/bs.s
	$(CC) -o bc.o -c -ansi mtxFS/bc.c
	@echo "linking booter ....."
	$(LD) -d -o booter bs.o bc.o /usr/lib/bcc/libc.a

clean:
	rm -v *.o mtx ./mount_point/boot/mtx booter mFDimage

