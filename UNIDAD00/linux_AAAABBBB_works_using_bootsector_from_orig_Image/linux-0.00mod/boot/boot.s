!
!	boot.s
!
! boot.s is loaded at 0x7c00 by the bios-startup routines, and moves itself
! out of the way to address 0x90000, and jumps there.
!
! It then loads the system at 0x10000, using BIOS interrupts. Thereafter
! it disables all interrupts, changes to protected mode, and calls the 
! start of system. System then must RE-initialize the protected mode in
! it's own tables, and enable interrupts as needed.
!

! 1.44Mb disks:
sectors = 18

.globl begtext, begdata, begbss, endtext, enddata, endbss
.text
begtext:
.data
begdata:
.bss
begbss:
.text
BOOTSEG = 0x07c0
SYSSEG  = 0x1000			! system loaded at 0x10000 (65536).
SYSLEN  = 17				! sectors occupied.

entry start
start:
	jmpi	go,#BOOTSEG
go:
	mov	ax,cs
	mov	ds,ax
	mov	es,ax
	mov	ss,ax
	mov	sp,#0x400		! arbitrary value >>512

! ok, we've written the message, now
! we want to load the system (at 0x10000)
load_system:
	mov	dx,#0x0000
	mov	cx,#0x0002
	mov	ax,#SYSSEG
	mov	es,ax
	mov	bx,#0x0000
	mov	ax,#0x200+SYSLEN
	int 	0x13
	jnc	ok_load
	mov	dx,#0x0000
	mov	ax,#0x0000
	int	0x13
	jmp	load_system

ok_load:

! now we want to move to protected mode ...
	cli			! no interrupts allowed !

! then we load the segment descriptors

	mov	ax,cs		! right, forgot this at first. didn't work :-)
	mov	ds,ax
	lidt	idt_48		! load idt with 0,0
	lgdt	gdt_48		! load gdt with whatever appropriate

! well, that went ok, I hope. Now we have to reprogram the interrupts :-(
! we put them right after the intel-reserved hardware interrupts, at
! int 0x20-0x2F. There they won't mess up anything. Sadly IBM really
! messed this up with the original PC, and they haven't been able to
! rectify it afterwards. Thus the bios puts interrupts at 0x08-0x0f,
! which is used for the internal hardware interrupts as well. We just
! have to reprogram the 8259's, and it isn't fun.

	mov	al,#0x11		! initialization sequence
	out	#0x20,al		! send it to 8259A-1
	out	#0xA0,al		! and to 8259A-2
	mov	al,#0x20		! start of hardware int's (0x20)
	out	#0x21,al
	mov	al,#0x28		! start of hardware int's 2 (0x28)
	out	#0xA1,al
	mov	al,#0x04		! 8259-1 is master
	out	#0x21,al
	mov	al,#0x02		! 8259-2 is slave
	out	#0xA1,al
	mov	al,#0x01		! 8086 mode for both
	out	#0x21,al
	out	#0xA1,al
	mov	al,#0xFF		! mask off all interrupts for now
	out	#0x21,al
	out	#0xA1,al

! well, that certainly wasn't fun :-(. Hopefully it works, and we don't
! need no steenking BIOS anyway (except for the initial loading :-).
! The BIOS-routine wants lots of unnecessary data, and it's less
! "interesting" anyway. This is how REAL programmers do it.
!
! Well, now's the time to actually move into protected mode. To make
! things as simple as possible, we do no register set-up or anything,
! we let the gnu-compiled 32-bit programs do that. We just jump to
! absolute address 0x00000, in 32-bit protected mode.

	mov	bx,#SYSSEG      ! loaded place.
	mov	ax,#0x0001	! protected mode (PE) bit
	lmsw	ax		! This is it!
	jmpi	0,8		! jmp offset 0 of segment 8 (cs)

gdt:
	.word	0,0,0,0		! dummy

	.word	0x07FF		! 8Mb - limit=2047 (2048*4096=8Mb)
	.word	0x0000		! base address=0x10000
	.word	0x9A01		! code read/exec
	.word	0x00C0		! granularity=4096, 386

	.word	0x07FF		! 8Mb - limit=2047 (2048*4096=8Mb)
	.word	0x0000		! base address=0x10000
	.word	0x9201		! data read/write
	.word	0x00C0		! granularity=4096, 386

idt_48:
	.word	0		! idt limit=0
	.word	0,0		! idt base=0L

gdt_48:
	.word	0x7ff		! gdt limit=2048, 256 GDT entries
	.word	0x7c00+gdt,0	! gdt base = 07xxx
	
msg1:
	.byte 13,10
	.ascii "Loading system ..."
	.byte 13,10,13,10

.text
endtext:
.data
enddata:
.bss
endbss:
