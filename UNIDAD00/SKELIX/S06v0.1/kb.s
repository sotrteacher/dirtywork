	.file	"kb.c"
.globl tty0
	.bss
	.align 32
	.type	tty0, @object
	.size	tty0, 258
tty0:
	.zero	258
	.local	shf_p
	.comm	shf_p,1,1
	.local	ctl_p
	.comm	ctl_p,1,1
	.local	alt_p
	.comm	alt_p,1,1
	.section	.rodata
	.align 32
	.type	key_map.0, @object
	.size	key_map.0, 116
key_map.0:
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.byte	49
	.byte	33
	.byte	50
	.byte	64
	.byte	51
	.byte	35
	.byte	52
	.byte	36
	.byte	53
	.byte	37
	.byte	54
	.byte	94
	.byte	55
	.byte	38
	.byte	56
	.byte	42
	.byte	57
	.byte	40
	.byte	48
	.byte	41
	.byte	45
	.byte	95
	.byte	61
	.byte	43
	.byte	8
	.byte	8
	.byte	9
	.byte	9
	.byte	113
	.byte	81
	.byte	119
	.byte	87
	.byte	101
	.byte	69
	.byte	114
	.byte	82
	.byte	116
	.byte	84
	.byte	121
	.byte	89
	.byte	117
	.byte	85
	.byte	105
	.byte	73
	.byte	111
	.byte	79
	.byte	112
	.byte	80
	.byte	91
	.byte	123
	.byte	93
	.byte	125
	.byte	10
	.byte	10
	.byte	0
	.byte	0
	.byte	97
	.byte	65
	.byte	115
	.byte	83
	.byte	100
	.byte	68
	.byte	102
	.byte	70
	.byte	103
	.byte	71
	.byte	104
	.byte	72
	.byte	106
	.byte	74
	.byte	107
	.byte	75
	.byte	108
	.byte	76
	.byte	59
	.byte	58
	.byte	39
	.byte	34
	.byte	96
	.byte	126
	.byte	0
	.byte	0
	.byte	92
	.byte	124
	.byte	122
	.byte	90
	.byte	120
	.byte	88
	.byte	99
	.byte	67
	.byte	118
	.byte	86
	.byte	98
	.byte	66
	.byte	110
	.byte	78
	.byte	109
	.byte	77
	.byte	44
	.byte	60
	.byte	46
	.byte	62
	.byte	47
	.byte	63
	.byte	0
	.byte	0
	.byte	42
	.byte	42
	.byte	0
	.byte	0
	.byte	32
	.byte	32
	.text
	.type	pln, @function
pln:
	subl	$12, %esp
	cmpb	$0, scan_code
	jns	.L2
	jmp	.L1
.L2:
	movzbl	tty0+256, %eax
	cmpb	tty0+257, %al
	je	.L3
	movzbl	tty0+257, %eax
	movzbl	%al, %ecx
	movzbl	shf_p, %edx
	movzbl	scan_code, %eax
	andl	$127, %eax
	movzbl	key_map.0(%edx,%eax,2), %eax
	movb	%al, tty0(%ecx)
	incb	tty0+257
.L3:
	movl	$0, 8(%esp)
	movl	$7, 4(%esp)
	movzbl	shf_p, %edx
	movzbl	scan_code, %eax
	andl	$127, %eax
	movsbl	key_map.0(%edx,%eax,2),%eax
	movl	%eax, (%esp)
	call	print_c
.L1:
	addl	$12, %esp
	ret
	.size	pln, .-pln
	.section	.rodata
.LC0:
	.string	"ESC"
	.text
	.type	esc, @function
esc:
	subl	$12, %esp
	cmpb	$0, scan_code
	jns	.L5
	jmp	.L4
.L5:
	movzbl	tty0+256, %eax
	cmpb	tty0+257, %al
	je	.L6
	movzbl	tty0+257, %eax
	movzbl	%al, %edx
	movzbl	scan_code, %eax
	movb	%al, tty0(%edx)
	incb	tty0+257
.L6:
	movl	$.LC0, 4(%esp)
	movl	$0, (%esp)
	call	kprintf
.L4:
	addl	$12, %esp
	ret
	.size	esc, .-esc
	.type	ctl, @function
ctl:
	xorb	$1, ctl_p
	ret
	.size	ctl, .-ctl
	.type	alt, @function
alt:
	xorb	$1, alt_p
	ret
	.size	alt, .-alt
	.type	shf, @function
shf:
	xorb	$1, shf_p
	ret
	.size	shf, .-shf
	.type	fun, @function
fun:
	ret
	.size	fun, .-fun
	.type	unp, @function
unp:
	ret
	.size	unp, .-unp
	.section	.rodata
	.align 4
.LC1:
	.long	unp
	.long	esc
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	ctl
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	shf
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	pln
	.long	shf
	.long	pln
	.long	alt
	.long	pln
	.long	unp
	.long	fun
	.long	fun
	.long	fun
	.long	fun
	.long	fun
	.long	fun
	.long	fun
	.long	fun
	.long	fun
	.long	fun
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	fun
	.long	fun
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.long	unp
	.text
.globl do_kb
	.type	do_kb, @function
do_kb:
	subl	$572, %esp
	movl	$0, 556(%esp)
	leal	32(%esp), %ecx
	movl	$.LC1, %edx
	movl	$512, %eax
	movl	%eax, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%ecx, (%esp)
	call	memcpy
	movl	$96, %edx
#APP
	inb	%dx,	%al
	
#NO_APP
	movb	%al, 31(%esp)
	movzbl	31(%esp), %eax
	movb	%al, scan_code
	movzbl	scan_code, %eax
	andl	$127, %eax
	movl	32(%esp,%eax,4), %eax
	call	*%eax
	movl	$97, %edx
#APP
	inb	%dx,	%al
	
#NO_APP
	movb	%al, 31(%esp)
	movzbl	31(%esp), %eax
	movzbl	%al, %eax
	movl	%eax, 556(%esp)
	movl	556(%esp), %eax
	orl	$128, %eax
	movl	$97, %edx
#APP
	outb	%al,	%dx
	
#NO_APP
	movl	556(%esp), %eax
	andl	$127, %eax
	movl	$97, %edx
#APP
	outb	%al,	%dx
	
#NO_APP
	movl	$32, %eax
	movl	$32, %edx
#APP
	outb	%al,	%dx
	
#NO_APP
	addl	$572, %esp
	ret
	.size	do_kb, .-do_kb
	.local	scan_code
	.comm	scan_code,1,1
	.section	.note.GNU-stack,"",@progbits
	.ident	"GCC: (GNU) 3.4.6 (Gentoo 3.4.6-r1, ssp-3.4.5-1.0, pie-8.7.9)"
