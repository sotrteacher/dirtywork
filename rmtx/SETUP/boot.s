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

# protected mode MTX by KCW
#------------------------- Algorithm ---------------------------------------
# 1. reload booter+setup to 0x9000 ; apentry to 0x9100
# 2. set ds,ss to 0x9000, sp=8KB in ss
# 3. load whole image to 0x1000:0 from CYL 1, 1,2,...
# 4. move MTX kernel DOWNward by 2KB ; segment by segment
# 5. if BOOT.510='RR' : ljmp to 0x1000:0 to run real mode MTX
# 6. if BOOT.510='PP' : ljmp to 0x9020:0 to run SETUP	
#---------------------------------------------------------------------------
		.text
		.globl	start
               
	        .code16
start:
# reload booter+setup to 0x9000:0
	        movw    $0x9000,%ax
	        movw    %ax,%es      # set ES=0x9000
	        xorw    %dx,%dx      # head 0, drive 0
	        xorw    %cx,%cx      # cyl 0
	        incb    %cl          # sector 1
	        xorw    %bx,%bx      # loading address=0x9000:0
	        movw    $0x0202,%ax  # read 2 sectors |boot+setup| to 0x90000
	        int     $0x13        # call BIOS

	        movw    $0x9100,%ax
	        movw    %ax,%es      # set ES=0x9000
	        xorw    %dx,%dx      # head 0, drive 0
	        xorw    %cx,%cx      # cyl 0
                addb    $4,%cl       # sector #4
	        xorw    %bx,%bx      # loading address=0x9100:0
	        movw    $0x0202,%ax  # read 2 sectors |apentry| to 0x91000
	        int     $0x13        # call BIOS
	
# long jump to 0x9000:next to continue execution there
#	        ljmp    $0x9000,$next
next:	
# set segments registers ds,ss=0x9000
	        movw	%cs,	%ax
		movw	%ax,	%ds  # ds = 0x9000
		movw	%ax,	%ss  # stack segment = 0x9000
		movw	$8192,  %sp  # 8KB stack size

# load all CYLsto 0x10000	; then move MTX kernel downward by 2KB
                movw    $0x1000,%ax
	        movw    %ax,%es      # ES = 0x1000
                movw    $16,%cx      # max image size = 256KB = 16 CYLs
load_image:	
                call	load_cyl
		incw	cyl          # inc cyl by 1 for next CYL

# inc ES by 36*0x20 for next loading address ES:0
                movw    %es,%ax
	        addw    $0x20*36,%ax
	        movw    %ax,%es
	        loop	load_image

# must move loaded image DOWNward by 2KB since |BOOT+SETUP|APentry|
                movw $8,%cx          # assume at most 8 segment 
	        movw $0x1000,%ax
again:
	        pushw %ax
	        call  move_down
	        popw  %ax
	        addw  $0x1000,%ax
	        loop  again

                movw  510,%ax
	        cmpw  $0x5252,%ax
	        jne   protected
# ljmp to 0x1000:0x0000 to run real mode MTX kernel
	        ljmp    $0x1000,$0x0000
protected:	ljmp    $0x9020,$0x0000
#================= the following are subroutines ============================
cyl:	        .word   0            # kernel begins at cyl 1 		
load_cyl:
		pushw	%cx          # cx = iteration count ; preserve it
                xorw    %dx,%dx      # h=0, drive=0
	        xorw    %cx,%cx
	        movb    cyl,%ch
                movb    $1,%cl
	        xorw    %bx,%bx
                movw    $0x0224,%ax  # read 36 sectors
		int	$0x13
		popw	%cx          # restore cx
		ret

# move(segment) move 32K words from segment+0x80 to segment

move_down:
	pushw %bp
        movw  %sp, %bp
	
	movw  4(%bp), %ax
	movw  %ax,%es
	xorw  %di,%di

        addw  $0x80,%ax
	movw  %ax, %ds
	xorw  %si,%si

        movw  $32768,%cx
	rep
	movsw
	
        popw  %bp
	ret

	
#======== really do not need these, just ensure booter is 512 bytes ==========
.org	510
.word	0x5252   # 'RR' for real mode kernel
#===================== end of boot.s =========================================
