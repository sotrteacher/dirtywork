AS = as86
CC = bcc
LD = ld86
MTXLIBSRC = MTXLIB.src/bio.c MTXLIB.src/queue.c \
            MTXLIB.src/getc.s MTXLIB.src/putc.s MTXLIB.src/setds.s
USERPROG = USER/u1.c USER/u.s

mtx:ts.s t.c mtxlib
	$(AS) -o ts.o ts.s
	$(CC)  -o t.o -c -ansi t.c
	$(LD) -d -o mtx ts.o t.o mtxlib /usr/lib/bcc/libc.a
run:mtx u1 booter 
	cp -v mtx ./mount_point/boot/
	cp -v u1 ./mount_point/bin/
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
mFDimage:mtx u1 booter
	cp -v mtx ./mount_point/boot/
	cp -v u1 ./mount_point/bin/
	genext2fs -b 1440 -d ./mount_point/  mFDimage
	dd if=booter of=mFDimage bs=1024 count=1 conv=notrunc
u1:$(USERPROG) mtxlib
	cd USER/ && $(MAKE)
mtx.asm:mtx
	objdump -D -b binary -mi386 -Maddr16,data16 mtx > $@
hexdump:
	hexdump -C mtx > mtx.hexdump
	hexdump -C u1 > u1.hexdump
#objdump86 only shows section-info, can't do disassembly :/
objdump:
	objdump86 u1 > u1.objdump
clean:
	rm -v *.o MTXLIB.src/*.o mtx u1 ./mount_point/boot/mtx booter mFDimage mtxlib
	rm -v ./mount_point/bin/u1 ./USER/*.o

