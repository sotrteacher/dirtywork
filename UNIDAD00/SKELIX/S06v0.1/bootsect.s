# Skelix by Xiaoming Mo (skelixos@gmail.com)
# Licence: GPLv2
		.text
		.globl	start
		.globl	read_sect
		#.globl	read_it
		#.globl	read_track
		.globl	before_copy_first_frag_of_kern
		.globl	rd_kern
		.globl	rp_read
		.globl	after_get_drive_params
		.globl	get_disk_drive_params
		.include "kernel.inc"
		.code16
start:							# An absolute long jump instruction to a label that 
								# represents the next line of code.
		jmpl    $0x0,   $code	# Canonicalizing %cs:%eip to a known segment:offset pair.
gdt:   
		.quad	0x0000000000000000 # null descriptor, 0x00
		.quad	0x00cf9a000000ffff # cs, 0x08
		.quad	0x00cf92000000ffff # ds, 0x10, DATA_SEL = 0x10
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
##if 1 //LMC 2021.10.31	
##sread:	.word 1+ SETUPLEN	# sectors read of current track
#sread:	.word 0			# sectors read of current track
#head:	.word 0			# current head
#track:	.word 0			# current track	
#sectors:
#	.word 0
##endif

code:
		xorw	%ax,	%ax
		movw	%ax,	%ds	# ds = 0x0000
		movw	%ax,	%ss	# stack segment = 0x0000
		movw	$0x1000,%sp	# arbitrary value 
					# used before pmode
##if 1 //LMC 2021.11.01
# Get disk drive parameters, specifically nr of sectors/track
get_disk_drive_params:
#	mov	$0x00, %dl
	movw	$0x0800, %ax		# AH=8 is get drive parameters
	int	$0x13
after_get_drive_params:			# With (gdb) info reg dh I got 0x0, (= Number of heads - 1),
								# => Number of heads = 1, and with (gdb) info reg cl I got
								# 0x24     36, => 0x3f & %cl = 0x24 = $36 sectors per track.
#endif
		## read rest of kernel to 0x10000
		movw	$0x1000,%ax
		movw	%ax,	%es
		xorw	%bx,	%bx	# es:bx destination address
		movw	$KERNEL_SECT,%cx
		movw	$1,	%si	# 0 is boot sector
rd_kern:
		call	read_sect
		addw	$512,	%bx
		incw	%si
		loop	rd_kern
	#else	
		#call	read_it
	#endif	

		cli
		
before_copy_first_frag_of_kern:	
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
#if 0 //LMC 2021.10.31
#if 1 //LMC 2021.10.31.09.24
read_sect:
		pushw	%ax
		pushw	%cx
		pushw	%dx
		pushw	%bx

		movw	%si,	%ax		
		xorw	%dx,	%dx
	#if 0 //LMC 2021.11.01	
		#movw	$18,	%bx	# 18 sectors per track 
	#else	
		movw	$36,	%bx	# 36 sectors per track (parameter got above)
	#endif	
							# for floppy disk
		divw	%bx			# %ax = %ax / %bx, %dx = LBA % 18  o %dx = LBA % 36 
		incw	%dx			# %dx = LBA % 18 + 1  o %dx = LBA % 36 + 1
		movb	%dl,	%cl	# cl=sector number
		xorw	%dx,	%dx
		movw	$2,		%bx	# 2 heads per track 
							# for floppy disk
		divw	%bx			# %ax = (LBA / 18) / 2, %dx = (LBA / 18) % 2  
						# o %ax = (LBA / 36) / 2, %dx = (LBA / 36) % 2
		movb	%dl,	%dh	# head     (head number)
		xorb	%dl,	%dl	# driver   (floppy drive)
		movb	%al,	%ch	# cylinder (cylinder number)
		popw	%bx		# save to es:bx
rp_read:
		movb	$0x1,	%al	# read 1 sector
		movb	$0x2,	%ah # read sectors function, interrupt 0x13
		int		$0x13
		jc		rp_read
		popw	%dx
		popw	%cx
		popw	%ax
		ret
#else
#read_it:
#	mov	%es, %ax
#	test	$0x0fff, %ax
#die:jne 	die			# es must be at 64kB boundary
#	xor 	%bx, %bx		# bx is starting address within segment		
#rp_read:
#	mov 	%es, %ax
# 	cmp 	$ENDSEG, %ax		# have we loaded all yet?
#	jb	ok1_read
#	ret	
#ok1_read:
#	#seg cs
#	mov	%cs:sectors+0, %ax
#	sub	sread, %ax
#	mov	%ax, %cx
#	shl	$9, %cx
#	add	%bx, %cx
#	jnc 	ok2_read
#	je 	ok2_read
#	xor 	%ax, %ax
#	sub 	%bx, %ax
#	shr 	$9, %ax
#ok2_read:
#	call 	read_track
#	mov 	%ax, %cx
#	add 	sread, %ax
#	#seg cs
#	cmp 	%cs:sectors+0, %ax
#	jne 	ok3_read
#	mov 	$1, %ax
#	sub 	head, %ax
#	jne 	ok4_read
#	incw    track
#ok4_read:
#	mov	%ax, head
#	xor	%ax, %ax
#ok3_read:
#	mov	%ax, sread
#	shl	$9, %cx
#	add	%cx, %bx
#	jnc	rp_read
#	mov	%es, %ax
#	add	$0x1000, %ax
#	mov	%ax, %es
#	xor	%bx, %bx
#	jmp	rp_read
#
#read_track:
#	push	%ax
#	push	%bx
#	push	%cx
#	push	%dx
#	mov	track, %dx
#	mov	sread, %cx
#	inc	%cx
#	mov	%dl, %ch
#	mov	head, %dx
#	mov	%dl, %dh
#	mov	$0, %dl
#	and	$0x0100, %dx
#	mov	$2, %ah
#	int	$0x13
#	jc	bad_rt
#	pop	%dx
#	pop	%cx
#	pop	%bx
#	pop	%ax
#	ret
#bad_rt:	mov	$0, %ax
#	mov	$0, %dx
#	int	$0x13
#	pop	%dx
#	pop	%cx
#	pop	%bx
#	pop	%ax
#	jmp	read_track
#endif
		
.org	0x1fe, 0x90
.word	0xaa55

