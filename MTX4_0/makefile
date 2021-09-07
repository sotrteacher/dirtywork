#VFD:=../FDimage

#run:mtx
#	cp -v mtx ./mount_point/boot/
#	genext2fs -v -b 1440 -d ./mount_point/  mFDimage
#	dd if=FDimage.img of=mFDimage count=2 conv=notrunc
#	qemu-system-i386 -fda mFDimage -no-fd-bootchk
#mtx:ts.s t.c
#	as86 -o ts.o ts.s
#	bcc  -c -ansi t.c
#	ld86 -d -o mtx ts.o t.o mtxlib /usr/lib/bcc/libc.a
#
mtx:ts.s t.c
	as86 -o ts.o ts.s
	bcc  -o t.o -c -ansi t.c
	ld86 -d -o mtx ts.o t.o mtxlib /usr/lib/bcc/libc.a
run:mtx booter
	cp -v mtx ./mount_point/boot/
	genext2fs  -b 1440 -d ./mount_point/  mFDimage
#	dd if=FDimage.img of=mFDimage count=2 conv=notrunc
	dd if=booter of=mFDimage bs=1024 count=1 conv=notrunc
	qemu-system-i386 -fda mFDimage -no-fd-bootchk
booter:mtxFS/bs.s mtxFS/bc.c
	@echo "compiling booter ..."
	as86 -o bs.o mtxFS/bs.s
	bcc -o bc.o -c -ansi mtxFS/bc.c
	@echo "linking booter ....."
	ld86 -d -o booter bs.o bc.o /usr/lib/bcc/libc.a
runwnofs: # run with no file system
	dd if=loader_for_mtx_4s.bin of=mFDimage bs=512 count=1 conv=notrunc
	dd if=mtx of=mFDimage seek=1 bs=512 conv=notrunc
	qemu-system-i386 -fda mFDimage -no-fd-bootchk
loader:loader_mtx.s
	@echo "compiling loader_mtx.s ..."
	as86 -o loader_mtx.o loader_mtx.s
	@echo "linking loader ....."
	ld86 -d -o $@ loader_mtx.o 
rwl: loader   # run with loader
#	fallocate -xv -l 1440k mFDimage
#	mkfs.ext2 -v -T floppy -d FDI mFD
	fallocate -v -l 1440k mFDimage
	mkfs.ext2 -v -T floppy  mFDimage
	dd if=loader of=mFDimage bs=512 count=1 conv=notrunc
	dd if=mtx of=mFDimage seek=1 bs=512 conv=notrunc
	qemu-system-i386 -fda mFDimage -no-fd-bootchk
# https://hackaday.io/project/18868-improbable-avr-8088-substitution-for-pcxt/log/51763-first-attempt-at-raw-808688-assembling
# https://www.win.tue.nl/~aeb/linux/lk/lk-3.html
#  "Without the export line, ld86 will complain ld86: no start symbol."

#echo cp mtx to $VFD/boot/
#mount -o loop $VFD /mnt
#
#cp mtx /mnt/boot
#umount /mnt
clean:
	rm -v *.o mtx ./mount_point/boot/mtx booter mFDimage loader
#echo done
