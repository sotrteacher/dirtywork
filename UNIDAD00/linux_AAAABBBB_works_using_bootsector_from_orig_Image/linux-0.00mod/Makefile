#
# if you want the ram-disk device, define this to be the
# size in blocks.
#
#AS86	=as86 -0 -a
#LD86	=ld86 -0
AS86	=as86 
LD86	=ld86
#LD86	=ld86 -0 -z

AS	=gcc -m32 -x assembler-with-cpp -Wl,--nmagic,--oformat=a.out-i386-linux
#LD	=gld
LD	= ld --verbose 
STRIP   = strip
OBJDUMP = objdump
LDFLAGS	=  -s -x -M -nmagic -T   test1.lds -nostdlib
CC	=gcc $(RAMDISK)
#CFLAGS	=-m32 -Wl,--oformat=a.out-i386-linux -Wall -O -fstrength-reduce -fomit-frame-pointer 
CFLAGSX64	=  -Wall -O -fstrength-reduce -fomit-frame-pointer 

CPP	=cpp -nostdinc -Iinclude

.c.s:
	$(CC) $(CFLAGS) \
	-nostdinc -Iinclude -S -o $*.s $<
.s.o:
	$(AS) -c -o $*.o $<
.c.o:
	$(CC) $(CFLAGS) \
	-nostdinc -Iinclude -c -o $*.o $<

all:	Image

#Image: boot/boot tools/system tools/build
#	tools/build boot/boot tools/system  > Image
#	sync
Image: boot/boot tools/system
#	genext2fs -b 1440 -d ./mount_point/  Image
	fallocate  -l 1440k Image
	mkfs.ext2  -b 1024 -T floppy  Image
	dd if=boot/boot of=Image bs=512 count=1 conv=notrunc
	dd if=tools/system of=Image seek=1 bs=512 conv=notrunc
	sync
run: Image
	qemu-system-i386 -fda Image -no-fd-bootchk


loader:loader_mtx.s
	as86 -o loader_mtx.o loader_mtx.s
	ld86 -d -o loader loader_mtx.o

disk: Image
	dd bs=8192 if=Image of=/dev/fd0
	sync;sync;sync

#tools/build: tools/build.c
#	$(CC) $(CFLAGS) \
#	-o tools/build tools/build.c
tools/build: tools/build.c
	$(CC) $(CFLAGSX64) \
	-o tools/build tools/build.c
#LMC 2021.08.27
#boot/head.o: boot/head.s
#
#tools/system:	boot/head.o 
#	$(LD) $(LDFLAGS) boot/head.o  -o tools/system > System.map
#REF:
# https://www.rowleydownload.co.uk/arm/documentation/gnu/ld/Options.html
# -x
# --discard-all 
# Delete all local symbols 
tools/system: boot/head.s
	gcc -g -std=gnu99  -nostdlib -m32 -march=i386 -ffreestanding -x assembler-with-cpp -Wl,--nmagic,--oformat=binary -o boot/head.o -c $<  
	gcc -std=gnu99  -nostdlib -m32 -march=i386 -ffreestanding  -o $@ -Wl,--nmagic,--script=test1.lds,-Map=System.map,--retain-symbols-file=filename.rtd boot/head.o  


boot/boot:	boot/boot.s
	$(AS86) -o boot/boot.o boot/boot.s
	$(LD86) -d -o boot/boot boot/boot.o
#	$(LD86) -s -M -o boot/boot boot/boot.o

tools/system_1: boot/head_1.s
	gcc -g -std=gnu99  -nostdlib -m32 -march=i386 -ffreestanding -x assembler-with-cpp -Wl,--nmagic,--oformat=binary -o boot/head_1.o -c $<  
	gcc -std=gnu99  -nostdlib -m32 -march=i386 -ffreestanding  -o $@ -Wl,--nmagic,--script=test1bis.lds,-Map=System_1.map,--retain-symbols-file=filename.rtd boot/head_1.o  


clean:
	rm -vf Image System.map tmp_make core boot/boot System_1.map
	rm -vf init/*.o tools/system tools/build boot/*.o tools/system_1

