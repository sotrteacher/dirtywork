#********************************************************************
#Copyright 2010-2015 K.C. Wang, <kwang@eecs.wsu.edu>
#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <http://www.gnu.org/licenses/>.
#********************************************************************/

#--------------------------- KCW: setup.s -----------------------------------
# booter reloads booter+setup to 0x9000. booter runs at 0x9000:0, load kernel
# image to 0x1000:0, then ljmp to 0x9020:0 to run setup here
# ------------------------- Algorithm: --------------------------------------
# 1. begin in 16-bit mode: set segment registers and stack.
# 2. move gdt data structure to 0x9F000
# 3. load GDTR with gdt_48
# 4. enter protected mode, set up segment selectors and stack
# 5. ljmp to 32-bit mode code at do32.
# 6. move kernel to K_HIM=0x100000 (1MB)
# 7. ljmp to CS:K_HIM to run kernel at K_HIM
#----------------------------------------------------------------------------
		.text
		.globl	start
 
                .set KCODE_SEL,  0x08	# code segment selector in kernel mode 
                .set KDATA_SEL,  0x10     # data segment selector in kernel mode
                .set GDT_ADDR,   0x9F000
                .set GDT_ENTRIES,3
                .set GDT_SIZE,  (8*GDT_ENTRIES)
                .set K_ADDR,     0x100000
                .set K_ORG,      0x10000
	        .set K_HIM,      0x100000

 	        .org 0
start:
                .code16
# set segments registers ds,ss=0x9020
	        ljmp    $0x9020,$go
go:	
	        movw	%cs,	%ax
		movw	%ax,	%ds  # ds = 0x9020
		movw	%ax,	%ss  # stack segment = 0x9020
		movw	$2048,  %sp  # 8KB stack size

# test SETUP.510 for 'R'
#	        movw    $510,%bx
#	        movb    (%bx),%al
                pushw   %ds
	        movw    $0x9000,%ax
	        movw    %ax,%ds
	        movb    510,%al
                cmpb    $0x52,%al
	        jne     pmode
	        ljmp    $0x1000,$0
pmode:
	        popw    %ds
	        pushw   $0x61        # show 'a' 
                call    putc
	        addw    $2,%sp

# mov gdt struct to 0x98000
		movw	$GDT_ADDR>>4,%ax
		movw	%ax,	%es
		movw	$gdt,	%si
		xorw	%di,	%di
		movw	$GDT_SIZE>>2,%cx
		rep
		movsl
	
# load GDTR with setup_gdt descriptor
	        lgdt	gdt_desc

	        pushw   $0x42        # show 'B'
                call    putc
	        addw    $2,%sp
	
# enter protected mode by writing 1 to CR0
		cli
	        movl	%cr0,	%eax
		orl	$0x1,	%eax
		movl	%eax,	%cr0

# do a ljmp to flush instruction cache and set CS to KCODE_SEL

#	        .byte   0x66, 0xea   # prefix + ljmp-opcode
#               .long	0x90200+do32
#        	.word	KCODE_SEL     # CS selector
                ljmpl   $KCODE_SEL,$(0x90200+do32)     
.code32
do32:	
#load other selector registers
                movl    $KDATA_SEL,%eax	
	        movw	%ax,	%ds
		movw	%ax,	%ss
        	movw	%ax,	%fs
		movw	%ax,	%gs

#               movw    $0,     %ax
		movw	%ax,	%es
# mv kernel from 0x10000 to 0x100000, then jump to 0x100000
	        cld
	        movl  $K_ORG,   %esi
	        movl  $K_HIM,   %edi
                movl  $1024*64, %ecx  # assume 128K kernel  
                rep
	        movsl

	        movl    $K_HIM, 0x90010       # K_HIM location into 0x90010
	        ljmp    $KCODE_SEL,$K_HIM

gdt:   
		.quad	0x0000000000000000 # null descriptor
		.quad	0x00cF9a000000FFFF # kcs 00cF 9=PpLS=1001
		.quad	0x00cF92000000FFFF # kds

gdt_desc:
		.word	.-gdt-1
		.long	GDT_ADDR  # hard coded but can be altered if needed

.code16

getc:
        xorb   %ah,%ah       
        int    $0x16        
        andb   $0x7F,%al    
        ret 

putc: #void putc(char c): call BIOS 0x10 to print char           
               pushw %bp
	       movw  %sp,%bp
               movb 4(%bp), %al
               movb $14, %ah
               movw $0x000B,%bx 
               int  $0x10       
               popw  %bp
	       ret
.org 510
	 .byte 'R','R'
.org 512
