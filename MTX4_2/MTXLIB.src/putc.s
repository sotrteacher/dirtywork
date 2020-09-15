        .globl _putc,_color
_putc:           
        push   bp
	mov    bp,sp
	
        movb   al,4[bp]        ! get the char into aL
        movb   ah,#14          ! aH = 14
        movb   bl,_color       ! bL = cyan color 
        int    0x10            ! call BIOS to display the char

        pop    bp
	ret
