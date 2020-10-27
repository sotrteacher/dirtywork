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


.globl _getcs, _getds,_setds
.globl _main0,_prints,_syscall,_exit

.globl _int1,_int2,_int3,_int4,_int5,_int6,_int7
.globl _myint
.globl _iret
				
export auto_start
auto_start:	

start:
        call _main0
	
! exit(0) for normal termination
        push  #0
        call  _exit

dead:   jmp   dead

_iret:	pop  ax
	pop  ax
	pop  ax
	iret

_int1:	int 1
	ret
_int2:	int 2
	ret
_int3:	int 3
	ret
_int4:	int 4
	ret
_int5:	int 5
	ret
_int6:	int 6
	ret
_int7:	int 7
	ret
	
_myint:
	push  bp
	mov   bp,sp

        mov   bx,4[bp]
	movb  ah,bl
	movb  al, #0xCD

	mov   fake,ax
	jmp fake
	
	pop   bp  	
	ret
fake:	.word	0
        pop  bp
	ret
	 
_getcs:
        mov   ax, cs
        ret
_getds: mov   ax, ds
        ret
_setds:
	push  bp
	mov   bp,sp
	mov   ax,4[bp]
	mov   ds,ax
	mov   sp,bp
	pop   bp
	ret
	
	
_syscall:	
!_int80:
	int    80
	ret

