# To recompile MTX: 
# Either run make followed by make install on this Makefile
# OR     run the sh script mk 3
# Both install mtx to partition 3 of a VMware VM

# Switch is controlled by the bcc preprocessor -DMK 
# if -DMK, t.c includes all the .c files during compilation
# else     each .c file includes type.h and is compiled separately

H     =	 ../type.h
AS    =  as86
CC    =  bcc -ansi
FLAGS =  -c
LD    =  ld86
LIB   =  /usr/lib/bcc/libc.a

MOUNT="vmware-mount"
VMDISK="/root/vmware/Other/Other.vmdk " 
UMOUNT="vmware-mount -d"

OBJ =	cat.o catcher.o chmod.o chown.o cp.o divide.o grep.o hits.o init.o \
	itimer.o kill.o link.o login.o loop.o lpr.o ls.o man.o matrix.o \
	mkdir.o more.o mount.o mv.o norace.o ps.o qsort.o race.o rm.o \
	rmdir.o sendrecv.o sh.o signal.o sleep.o symlink.o sync.o touch.o \
	trap.o umount.o unlink.o while1.o whoami.o vfork.o 

CMD =	cat catcher chmod chown cp divide grep hits init \
	itimer kill link login loop lpr ls man matrix \
	mkdir more mount mv norace ps qsort race rm \
	rmdir sendrecv sh signal sleep symlink sync touch \
	trap umount unlink while1 whoami vfork

all:	$(CMD)

cat:	cat.o
	$(AS) -o u.o u.s
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 

catcher:chmod.o
	$(AS) -o u.o u.s
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 
chmod:
	$(AS) -o u.o u.s
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
chown:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
cp:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
divide:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
grep:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
hits:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
init:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
itimer:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
kill:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
link:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
login:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
loop:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
lpr:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
ls:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
man:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
matrix:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
mkdir:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
more:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
mount:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
mv:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
norace:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
ps:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
qsort:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
race:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
rm:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
rmdir:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
sendrecv:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
sh:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
signal:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
sleep:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
symlink:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
sync:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
touch:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
trap:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
umount:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
unlink:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
while1:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
whoami:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null
vfork:
	$(CC) $(FLAGS) $@.c	
	$(LD) -o bin/$@ u.o $@.o mtxlib /usr/lib/bcc/libc.a 2> /dev/null

install:

	dump 3

clean:
	rm *.o bin/*



