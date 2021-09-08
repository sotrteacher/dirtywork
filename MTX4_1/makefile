AS = as86
CC = bcc
LD = ld86

mtx:ts.s t.c
	$(AS) -o ts.o ts.s
	$(CC)  -o t.o -c -ansi t.c
	$(LD) -d -o mtx ts.o t.o mtxlib /usr/lib/bcc/libc.a
run:mtx booter
	cp -v mtx ./mount_point/boot/
#	genext2fs -v -b 1440 -d ./mount_point/  mFDimage
	fallocate -v -l 1440k mFDimage
	mkfs.ext2 -v -b 1024 -d ./mount_point/ -T floppy  mFDimage
#	dd if=FDimage.img of=mFDimage count=2 conv=notrunc
	dd if=booter of=mFDimage bs=1024 count=1 conv=notrunc
	qemu-system-i386 -fda mFDimage -no-fd-bootchk
booter:mtxFS/bs.s mtxFS/bc.c
	@echo "compiling booter ..."
	$(AS) -o bs.o mtxFS/bs.s
	$(CC) -o bc.o -c -ansi mtxFS/bc.c
	@echo "linking booter ....."
	$(LD) -d -o booter bs.o bc.o /usr/lib/bcc/libc.a
loader:loader_mtx.s
	@echo "compiling loader_mtx.s ..."
	as86 -o loader_mtx.o loader_mtx.s
	@echo "linking loader ....."
	ld86 -d -o $@ loader_mtx.o 
rwl: loader mtx  # run with loader
#	fallocate -xv -l 1440k mFDimage
#	mkfs.ext2 -v -T floppy -d FDI mFD
	fallocate -v -l 1440k mFDimage
	mkfs.ext2 -v -b 1024 -T floppy  mFDimage
	dd if=loader of=mFDimage bs=512 count=1 conv=notrunc
	dd if=mtx of=mFDimage seek=1 bs=512 conv=notrunc
	qemu-system-i386 -fda mFDimage -no-fd-bootchk

clean:
	rm -v *.o mtx ./mount_point/boot/mtx booter mFDimage

