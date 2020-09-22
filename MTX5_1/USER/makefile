##UOBJ = u1.o u.o  # Stupid me.
#UOBJ = u.o u1.o   # HERE ORDER IS CRITICALLY IMPORTANT!!!!!!!!! 2020.09.20.23.52
AS = as86
CC = bcc
AR = ar
LD = ld86
allus:u.o u1.o u2.o
	$(LD) -o ../u1 u.o u1.o ../mtxlib /usr/lib/bcc/libc.a
	$(LD) -o ../u2 u.o u2.o ../mtxlib /usr/lib/bcc/libc.a
u.o:u.s
	$(AS) -o u.o u.s
u1.o:u1.c ucode.c
	$(CC) -c -ansi $< -o $@
u2.o:u2.c ucode.c
	$(CC) -c -ansi $< -o $@
