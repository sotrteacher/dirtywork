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

# apentry.s: entry code of APs in 16-bit real mode, in 2nd 1KB of mtximage.
# loaded to 0x9100:0000 during booting and runs from there.

      .text	
      .global start
.code16           
start: # assembled at 0 but runs at 0x91000 ==> ADD 0x91000 to all addresses
      cli            
      movw    %cs,%ax    # cs=0x9100
      movw    %ax,%ds
      movw    %ax,%es
      movw    %ax,%ss
	
# enter protected mode
      lgdt    gdtdesc
      movl    %cr0, %eax
      orl     $0x01,%eax
      movl    %eax, %cr0
	
# do a ljmp to flush instruction cache and set CS to KCODE_SEL
      ljmpl   $0x08, $(0x91000+start32)  # need ljmpl for 4-byte target address
.code32
start32:
      movl    $0x10, %eax   # set KDS=0x10
      movw    %ax, %ds
      movw    %ax, %es
      movw    %ax, %ss
      movw    %ax, %fs
      movw    %ax, %gs

# use 4GB flat segments to get APIC's ID at 0xFEE00000
      movl $0xFEE00000, %ebx
      addl $0x0020, %ebx       # ID register at offset 0x0020
      movl 0(%ebx), %eax       # read APIC.ID register
      shrl $24,  %eax          # shift ID to low byte
      movl %eax, %edi          # save ID in edi
	
# turn on paging by using the pgdir at 101000, which ID maps 0-8MB
      movl   $0x101000,%eax
      movl   %eax, %cr3
      movl   %cr0, %eax
      orl    $0x80000000,%eax
      movl   %eax, %cr0
      jmp    1f
1:

#----- BSP deposited these at 0x90000 for APPs ----------
#  0x90000 : APstart() entry address
#       +4 : AP1's stack pointer,
#	+8 : AP2's stack pointer
#      +12 : AP3's stack pointer, etc.
#--------------------------------------------------------	
      movl   $(0x90000+start), %ebx  
      movl   %edi,%ecx                 # AP ID number
      shll   $2,%ecx                   # multiply by 4                
      addl   %ecx, %ebx                # this AP's stack pointer
      movl   (%ebx),%esp               # set %esp
      pushl  %edi                      # push on ID number
      call   *(0x90000+start)          # call APstart(ID) in smp.c
	
spin: jmp    spin                      # should never reach here  
gdt: .quad   0x0000000000000000        # null descriptor
     .quad   0x00cF9a000000FFFF        # kcs 00cF 9=PpLS=1001
     .quad   0x00cF92000000FFFF        # kds
gdtdesc:
     .word   24-1
     .long   0x91000+gdt

.org 512

