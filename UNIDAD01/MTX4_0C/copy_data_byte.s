# copy_data_byte.s - Function copy_data_byte() uses segmentation through the 
# GDT provided by grub. The first descriptors of the GDT were obtained with 
# the first version of the kernel MTX4_0C. With the output of that version 
# of MTX4_0C I found that program is using the flat memory model.
#
MKRD_SEL	= 0x10	/* The kernel data selector: 0000000000010000b 
			 = 0000000000010 0 00, index=2, T=0 (GDT), RPL=00 */
.globl copy_data_byte

#
#void copy_data_byte(const char *s,unsigned short int *dest);
#
# Example of use of the function:
# char c = 'A';
# @param row: number of row 
# @param col: number of column
# copy_data_byte(&c,(unsigned short int *)0x0b8000+row*25+col);
# print character A at the cordinates (col,row) on the screen.
copy_data_byte:
		pushl	%ebp
		movl	%esp, %ebp
		push	%gs
		pushl	%ebx
		pushl	%eax
		mov	$MKRD_SEL, %ebx
		mov	%bx, %gs
		#Poner en %al el byte a escribir (ASCII code)
		movl	8(%ebp), %ebx
		movb	(%ebx), %al
		#movb	%gs:(%ebx), %al
		#Poner en %ebx el offset en el que se va a escribir 
		#dentro del segmento de datos del kernel.
		movl	12(%ebp), %ebx
		#shl	$1, %ebx  # Used in write_char, at boot/head.s of linux-0.00, why?
		movb	%al, %gs:(%ebx)
#dumm_loop0:	jmp 	dumm_loop0 # Used while debbuging.
		popl	%eax
		popl	%ebx
		pop	%gs
		movl	%ebp, %esp
		popl	%ebp
		ret
		

