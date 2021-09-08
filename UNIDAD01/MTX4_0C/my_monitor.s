	.file	"monitor.c"
	.globl	video_memory
	.data
	.align 4
	.type	video_memory, @object
	.size	video_memory, 4
video_memory:
	.long	753664
	.globl	cursor_x
	.bss
	.type	cursor_x, @object
	.size	cursor_x, 1
cursor_x:
	.zero	1
	.globl	cursor_y
	.type	cursor_y, @object
	.size	cursor_y, 1
cursor_y:
	.zero	1
	.text
	.type	move_cursor, @function
move_cursor:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	movzbl	cursor_y, %eax
	movzbl	%al, %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$4, %eax
	movl	%eax, %edx
	movzbl	cursor_x, %eax
	movzbl	%al, %eax
	addl	%edx, %eax
	movw	%ax, -10(%ebp)
	subl	$8, %esp
	pushl	$14
	pushl	$980
	call	outb
	addl	$16, %esp
	movzwl	-10(%ebp), %eax
	shrw	$8, %ax
	movzbl	%al, %eax
	subl	$8, %esp
	pushl	%eax
	pushl	$981
	call	outb
	addl	$16, %esp
	subl	$8, %esp
	pushl	$15
	pushl	$980
	call	outb
	addl	$16, %esp
	movzwl	-10(%ebp), %eax
	movzbl	%al, %eax
	subl	$8, %esp
	pushl	%eax
	pushl	$981
	call	outb
	addl	$16, %esp
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	move_cursor, .-move_cursor
	.type	scroll, @function
scroll:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movb	$15, -5(%ebp)
	movzbl	-5(%ebp), %eax
	sall	$8, %eax
	orl	$32, %eax
	movw	%ax, -8(%ebp)
	movzbl	cursor_y, %eax
	cmpb	$24, %al
	jbe	.L8
	movl	$0, -4(%ebp)
	jmp	.L4
.L5:
	movl	video_memory, %eax
	movl	-4(%ebp), %edx
	addl	%edx, %edx
	addl	%eax, %edx
	movl	video_memory, %eax
	movl	-4(%ebp), %ecx
	addl	$80, %ecx
	addl	%ecx, %ecx
	addl	%ecx, %eax
	movzwl	(%eax), %eax
	movw	%ax, (%edx)
	addl	$1, -4(%ebp)
.L4:
	cmpl	$1919, -4(%ebp)
	jle	.L5
	movl	$1920, -4(%ebp)
	jmp	.L6
.L7:
	movl	video_memory, %eax
	movl	-4(%ebp), %edx
	addl	%edx, %edx
	addl	%eax, %edx
	movzwl	-8(%ebp), %eax
	movw	%ax, (%edx)
	addl	$1, -4(%ebp)
.L6:
	cmpl	$1999, -4(%ebp)
	jle	.L7
	movb	$24, cursor_y
.L8:
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	scroll, .-scroll
	.globl	monitor_put
	.type	monitor_put, @function
monitor_put:
.LFB2:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$40, %esp
	movl	8(%ebp), %eax
	movb	%al, -28(%ebp)
	movb	$0, -9(%ebp)
	movb	$15, -10(%ebp)
	movzbl	-9(%ebp), %eax
	sall	$4, %eax
	movl	%eax, %edx
	movzbl	-10(%ebp), %eax
	andl	$15, %eax
	orl	%edx, %eax
	movb	%al, -11(%ebp)
	movzbl	-11(%ebp), %eax
	sall	$8, %eax
	movw	%ax, -14(%ebp)
	cmpb	$8, -28(%ebp)
	jne	.L10
	movzbl	cursor_x, %eax
	testb	%al, %al
	je	.L10
	movzbl	cursor_x, %eax
	subl	$1, %eax
	movb	%al, cursor_x
	jmp	.L11
.L10:
	cmpb	$9, -28(%ebp)
	jne	.L12
	movzbl	cursor_x, %eax
	addl	$8, %eax
	andl	$-8, %eax
	movb	%al, cursor_x
	jmp	.L11
.L12:
	cmpb	$13, -28(%ebp)
	jne	.L13
	movb	$0, cursor_x
	jmp	.L11
.L13:
	cmpb	$10, -28(%ebp)
	jne	.L14
	movb	$0, cursor_x
	movzbl	cursor_y, %eax
	addl	$1, %eax
	movb	%al, cursor_y
	jmp	.L11
.L14:
	cmpb	$31, -28(%ebp)
	jle	.L11
	movl	video_memory, %ecx
	movzbl	cursor_y, %eax
	movzbl	%al, %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$4, %eax
	movl	%eax, %edx
	movzbl	cursor_x, %eax
	movzbl	%al, %eax
	addl	%edx, %eax
	addl	%eax, %eax
	addl	%ecx, %eax
	movl	%eax, -20(%ebp)
	movsbw	-28(%ebp), %dx
	movzwl	-14(%ebp), %eax
	orl	%edx, %eax
	movl	%eax, %edx
	movl	-20(%ebp), %eax
	movw	%dx, (%eax)
	movzbl	cursor_x, %eax
	addl	$1, %eax
	movb	%al, cursor_x
.L11:
	movzbl	cursor_x, %eax
	cmpb	$79, %al
	jbe	.L15
	movb	$0, cursor_x
	movzbl	cursor_y, %eax
	addl	$1, %eax
	movb	%al, cursor_y
.L15:
	call	scroll
	call	move_cursor
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	monitor_put, .-monitor_put
	.globl	monitor_clear
	.type	monitor_clear, @function
monitor_clear:
.LFB3:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	movb	$15, -13(%ebp)
	movzbl	-13(%ebp), %eax
	sall	$8, %eax
	orl	$32, %eax
	movw	%ax, -16(%ebp)
	movl	$0, -12(%ebp)
	jmp	.L17
.L18:
	movl	video_memory, %eax
	movl	-12(%ebp), %edx
	addl	%edx, %edx
	addl	%eax, %edx
	movzwl	-16(%ebp), %eax
	movw	%ax, (%edx)
	addl	$1, -12(%ebp)
.L17:
	cmpl	$1999, -12(%ebp)
	jle	.L18
	movb	$0, cursor_x
	movb	$0, cursor_y
	call	move_cursor
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	monitor_clear, .-monitor_clear
	.globl	monitor_write
	.type	monitor_write, @function
monitor_write:
.LFB4:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	movl	$0, -12(%ebp)
	jmp	.L20
.L21:
	movl	-12(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, -12(%ebp)
	movl	%eax, %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	subl	$12, %esp
	pushl	%eax
	call	monitor_put
	addl	$16, %esp
.L20:
	movl	-12(%ebp), %edx
	movl	8(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	.L21
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE4:
	.size	monitor_write, .-monitor_write
	.section	.rodata
.LC0:
	.string	"0x"
	.text
	.globl	monitor_write_hex
	.type	monitor_write_hex, @function
monitor_write_hex:
.LFB5:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	subl	$12, %esp
	pushl	$.LC0
	call	monitor_write
	addl	$16, %esp
	movb	$1, -9(%ebp)
	movl	$28, -16(%ebp)
	jmp	.L23
.L27:
	movl	-16(%ebp), %eax
	movl	8(%ebp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$15, %eax
	movl	%eax, -20(%ebp)
	cmpl	$0, -20(%ebp)
	jne	.L24
	cmpb	$0, -9(%ebp)
	jne	.L30
.L24:
	cmpl	$9, -20(%ebp)
	jle	.L26
	movb	$0, -9(%ebp)
	movl	-20(%ebp), %eax
	addl	$87, %eax
	movsbl	%al, %eax
	subl	$12, %esp
	pushl	%eax
	call	monitor_put
	addl	$16, %esp
	jmp	.L25
.L26:
	movb	$0, -9(%ebp)
	movl	-20(%ebp), %eax
	addl	$48, %eax
	movsbl	%al, %eax
	subl	$12, %esp
	pushl	%eax
	call	monitor_put
	addl	$16, %esp
	jmp	.L25
.L30:
	nop
.L25:
	subl	$4, -16(%ebp)
.L23:
	cmpl	$0, -16(%ebp)
	jg	.L27
	movl	8(%ebp), %eax
	andl	$15, %eax
	movl	%eax, -20(%ebp)
	cmpl	$9, -20(%ebp)
	jle	.L28
	movl	-20(%ebp), %eax
	addl	$87, %eax
	movsbl	%al, %eax
	subl	$12, %esp
	pushl	%eax
	call	monitor_put
	addl	$16, %esp
	jmp	.L31
.L28:
	movl	-20(%ebp), %eax
	addl	$48, %eax
	movsbl	%al, %eax
	subl	$12, %esp
	pushl	%eax
	call	monitor_put
	addl	$16, %esp
.L31:
	nop
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE5:
	.size	monitor_write_hex, .-monitor_write_hex
	.globl	monitor_write_dec
	.type	monitor_write_dec, @function
monitor_write_dec:
.LFB6:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$88, %esp
	cmpl	$0, 8(%ebp)
	jne	.L33
	subl	$12, %esp
	pushl	$48
	call	monitor_put
	addl	$16, %esp
	jmp	.L32
.L33:
	movl	8(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	$0, -16(%ebp)
	jmp	.L35
.L36:
	movl	-12(%ebp), %ecx
	movl	$1717986919, %edx
	movl	%ecx, %eax
	imull	%edx
	sarl	$2, %edx
	movl	%ecx, %eax
	sarl	$31, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	addl	%eax, %eax
	subl	%eax, %ecx
	movl	%ecx, %edx
	movl	%edx, %eax
	addl	$48, %eax
	movl	%eax, %ecx
	leal	-52(%ebp), %edx
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movb	%cl, (%eax)
	movl	-12(%ebp), %ecx
	movl	$1717986919, %edx
	movl	%ecx, %eax
	imull	%edx
	sarl	$2, %edx
	movl	%ecx, %eax
	sarl	$31, %eax
	subl	%eax, %edx
	movl	%edx, %eax
	movl	%eax, -12(%ebp)
	addl	$1, -16(%ebp)
.L35:
	cmpl	$0, -12(%ebp)
	jg	.L36
	leal	-52(%ebp), %edx
	movl	-16(%ebp), %eax
	addl	%edx, %eax
	movb	$0, (%eax)
	movl	-16(%ebp), %eax
	leal	-1(%eax), %edx
	movl	%edx, -16(%ebp)
	movb	$0, -84(%ebp,%eax)
	movl	$0, -20(%ebp)
	jmp	.L37
.L38:
	movl	-16(%ebp), %eax
	leal	-1(%eax), %edx
	movl	%edx, -16(%ebp)
	movl	-20(%ebp), %edx
	leal	1(%edx), %ecx
	movl	%ecx, -20(%ebp)
	movzbl	-52(%ebp,%edx), %edx
	movb	%dl, -84(%ebp,%eax)
.L37:
	cmpl	$0, -16(%ebp)
	jns	.L38
	subl	$12, %esp
	leal	-84(%ebp), %eax
	pushl	%eax
	call	monitor_write
	addl	$16, %esp
.L32:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE6:
	.size	monitor_write_dec, .-monitor_write_dec
	.ident	"GCC: (Debian 6.3.0-18+deb9u1) 6.3.0 20170516"
	.section	.note.GNU-stack,"",@progbits
