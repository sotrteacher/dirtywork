#UOBJ = u1.o u.o  # Stupid me.
UOBJ = u.o u1.o   # HERE ORDER IS CRITICALLY IMPORTANT!!!!!!!!! 2020.09.20.23.52
AS = as86
CC = bcc
AR = ar
LD = ld86
../u1:$(UOBJ)
	$(LD) -o $@ $^ ../mtxlib /usr/lib/bcc/libc.a
u1.o:u1.c ucode.c
	$(CC) -c -ansi $< -o $@
u.o:u.s
	$(AS) -o u.o u.s
