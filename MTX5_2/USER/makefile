##UOBJ = u1.o u.o  # Stupid me.
#UOBJ = u.o u1.o   # HERE ORDER IS CRITICALLY IMPORTANT!!!!!!!!! 2020.09.20.23.52
AS = as86
CC = bcc
AR = ar
LD = ld86
allus:u.o u1.o u2.o init.o sh.o ps.o fork.o exec.o
	$(LD) -o u1 u.o u1.o ../mtxlib /usr/lib/bcc/libc.a
	$(LD) -o u2 u.o u2.o ../mtxlib /usr/lib/bcc/libc.a
	$(LD) -o init u.o init.o ../mtxlib /usr/lib/bcc/libc.a
	$(LD) -o sh u.o sh.o ../mtxlib /usr/lib/bcc/libc.a
	$(LD) -o ps u.o ps.o ../mtxlib /usr/lib/bcc/libc.a
	$(LD) -o fork u.o fork.o ../mtxlib /usr/lib/bcc/libc.a
	$(LD) -o exec u.o exec.o ../mtxlib /usr/lib/bcc/libc.a
u.o:u.s
	$(AS) -o u.o u.s
u1.o:u1.c ucode.c
	$(CC) -c -ansi $< -o $@
u2.o:u2.c ucode.c
	$(CC) -c -ansi $< -o $@
init.o:init.c ucode.c
	$(CC) -c -ansi $< -o $@
sh.o:sh.c ucode.c
	$(CC) -c -ansi $< -o $@
ps.o:ps.c ucode.c
	$(CC) -c -ansi $< -o $@
fork.o:fork.c ucode.c
	$(CC) -c -ansi $< -o $@
exec.o:exec.c ucode.c
	$(CC) -c -ansi $< -o $@
