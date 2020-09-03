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
	genext2fs -v -b 1440 -d ./mount_point/  mFDimage
#	dd if=FDimage.img of=mFDimage count=2 conv=notrunc
	dd if=booter of=mFDimage bs=1024 count=1 conv=notrunc
	qemu-system-i386 -fda mFDimage -no-fd-bootchk
booter:mtxFS/bs.s mtxFS/bc.c
	@echo "compiling booter ..."
	as86 -o bs.o mtxFS/bs.s
	bcc -o bc.o -c -ansi mtxFS/bc.c
	@echo "linking booter ....."
	ld86 -d -o booter bs.o bc.o /usr/lib/bcc/libc.a

#echo cp mtx to $VFD/boot/
#mount -o loop $VFD /mnt
#
#cp mtx /mnt/boot
#umount /mnt
clean:
	rm -v *.o mtx ./mount_point/boot/mtx booter mFDimage
#echo done
