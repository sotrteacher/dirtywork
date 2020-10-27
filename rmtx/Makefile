# To recompile MTX: 
# Either run make followed by make install on this Makefile
# OR     run the sh script mk 3
# Both install mtx to partition 3 of a VMware VM

# Switch is controlled by the bcc preprocessor -DMK 
# if -DMK, t.c includes all the .c files during compilation
# else     each .c file includes type.h and is compiled separately

H     =	 type.h
AS    =  as86
CC    =  bcc -ansi
FLAGS =  -ansi -c
LD    =  ld86
LIB   =  /usr/lib/bcc/libc.a
K     =  kernel/
F     =  fs/
D     =  driver/

DOBJ  =	$(D)vid.o $(D)timer.o $(D)pv.o $(D)kbd.o $(D)pr.o $(D)serial.o \
	$(D)fd.o $(D)hd.o $(D)atapi.o

FOBJ  =	$(F)alloc_dealloc.o $(F)buffer.o $(F)cd_pwd.o $(F)chmod_chown.o \
	$(F)dev.o $(F)fs.o $(F)global.o $(F)link_unlink.o $(F)misc1.o \
	$(F)mkdir_creat.o $(F)mount.o $(F)mount_root.o $(F)open_close.o \
	$(F)read.o $(F)rmdir.o $(F)stat.o $(F)touch.o $(F)util.o $(F)write.o

KOBJ  =	$(K)t.o $(K)io.o $(K)queue.o $(K)wait.o $(K)mem.o $(K)loader.o \
	$(K)fork.o $(K)syscall.o $(K)int.o $(K)exec.o $(K)threads.o \
	$(K)signal.o $(K)mes.o $(K)pipe.o

mtx:
	cd kernel && $(MAKE)
	cd driver && $(MAKE)
	cd fs && $(MAKE)
	$(AS) -o ts.o kernel/ts.s	
	$(LD) -i -o mtx ts.o $(KOBJ) $(FOBJ) $(DOBJ) $(LIB)


install:
	./vminstall mtx 3

clean:
	rm kernel/*.o fs/*.o driver/*.o




