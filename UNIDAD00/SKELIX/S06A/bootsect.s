# Skelix by Xiaoming Mo (skelixos@gmail.com)
# Licence: GPLv2
		.text
		.globl	start
		.include "kernel.inc"
		.code16
start:
		jmpl    $0x0,   $code
gdt:   
		.quad	0x0000000000000000 # null descriptor, 0x00
		.quad	0x00cf9a000000ffff # cs, 0x08
		.quad	0x00cf92000000ffff # ds, 0x10
		.quad	0x0000e90000000068 # reserved for tssd0, TSS_SEL = 0x18 (include/kernel.h)
		.quad	0x0000e20000000040 # reserved for ldtd0, LDT_SEL = 0x20 (include/kernel.h)
#if 1 //LMC 2021.09.16
		.quad	0x0000e90000000068 # reserved for tssd1, TSS1_SEL = 0x28 (include/kernel.h)
		.quad	0x0000e20000000040 # reserved for ldtd1, LDT1_SEL = 0x30 (include/kernel.h)
		.quad	0x00409208000007ff # descriptor de IDT, MIDT_SEL = 0x38
                                           # Don't forget to update GDT_ENTRIES at 
                                           # include/kernel.inc [I missed this at 
                                           # first. Didn't work :-)] and the value of
                                           # MIDT_SEL at the begining of load.s 
                                           # GDT_ENTRIES also must be updated at 
                                           # include/kernel.h
		.quad	0x00409208080007ff # descriptor de GDT, MGDT_SEL = 0x40
#		.fill 256-GDT_ENTRIES,8,0  # /* space for LDT's and TSS's etc */
#                             /*There is no space in the boot sector for this */
#endif
gdt_48:
		.word	.-gdt-1
		.long	GDT_ADDR

code:
		xorw	%ax,	%ax
		movw	%ax,	%ds	# ds = 0x0000
		movw	%ax,	%ss	# stack segment = 0x0000
		movw	$0x1000,%sp	# arbitrary value 
					# used before pmode

		## read rest of kernel to 0x10000
		movw	$0x1000,%ax
		movw	%ax,	%es
		xorw	%bx,	%bx	# es:bs destination address
		movw	$KERNEL_SECT,%cx
		movw	$1,	%si	# 0 is boot sector
rd_kern:
		call	read_sect
		addw	$512,	%bx
		incw	%si
		loop	rd_kern

		cli
		
		## move first 512 bytes of kernel to 0x0000
		## it will move rest of kernel to 0x0200,
		## that is, next to this sector
		cld
		movw	$0x1000,%ax
		movw	%ax,	%ds
		movw	$0x0000,%ax
		movw	%ax,	%es
		xorw	%si,	%si
		xorw	%di,	%di
		movw	$512>>2,%cx
		rep
		movsl

		xorw	%ax,	%ax
		movw	%ax,	%ds	# reset ds to 0x0000
		## move	gdt 
		movw	$GDT_ADDR>>4,%ax
		movw	%ax,	%es
		movw	$gdt,	%si
		xorw	%di,	%di
		movw	$GDT_SIZE>>2,%cx
		rep
		movsl

enable_a20:
		## The Undocumented PC
		inb		$0x64,	%al	
		testb	$0x2,	%al
		jnz		enable_a20
		movb	$0xdf,	%al
		outb	%al,	$0x64

		lgdt	gdt_48
######################################################################
#if 1 //LMC 2021.09.26           #
	movb	$0x11,%al        #	#! initialization sequence
	out	%al,$0x20        #	#! send it to 8259A-1
	out	%al,$0xA0        #	#! and to 8259A-2
	movb	$0x20,%al        #	#! start of hardware int's (0x20)
	out	%al,$0x21        #
	movb	$0x28,%al        #	#! start of hardware int's 2 (0x28)
	out	%al,$0xA1        #
	movb	$0x04,%al        #	#! 8259-1 is master
	out	%al,$0x21        #
	movb	$0x02,%al        #	#! 8259-2 is slave
	out	%al,$0xA1        #
	movb	$0x01,%al        #	#! 8086 mode for both
	out	%al,$0x21        #
	out	%al,$0xA1        #
	movb	$0xFF,%al        #	#! mask off all interrupts for now
	out	%al,$0x21        #
	out	%al,$0xA1        #
#endif			         #
#####################################################################

		## enter pmode
		movl	%cr0,	%eax
		orl		$0x1,	%eax
		movl	%eax,	%cr0

		ljmp	$CODE_SEL, $0x0		# far? jump to the begining of load.o, because
						# $CODE_SEL is the kernel code selector (in GDT) 
						# and load.o is the first object file in the list 
						# of object files to be linked in the kernel (see  
						# KERNEL_OBJS and target kernel in Makefile). 
						# Remember at linking order matters!
		## in:	ax:	LBA address, starts from 0
		##		es:bx address for reading sector
read_sect:
		pushw	%ax
		pushw	%cx
		pushw	%dx
		pushw	%bx

		movw	%si,	%ax		
		xorw	%dx,	%dx
		movw	$18,	%bx	# 18 sectors per track 
					# for floppy disk
		divw	%bx
		incw	%dx
		movb	%dl,	%cl	# cl=sector number
		xorw	%dx,	%dx
		movw	$2,		%bx	# 2 headers per track 
							# for floppy disk
		divw	%bx

		movb	%dl,	%dh	# head
		xorb	%dl,	%dl	# driver
		movb	%al,	%ch	# cylinder
		popw	%bx		# save to es:bx
rp_read:
		movb	$0x1,	%al	# read 1 sector
		movb	$0x2,	%ah
		int		$0x13
		jc		rp_read
		popw	%dx
		popw	%cx
		popw	%ax
		ret
		
.org	0x1fe, 0x90
.word	0xaa55

