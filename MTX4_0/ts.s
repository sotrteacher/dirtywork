!********************************************************************
!Copyright 2010-2015 K.C. Wang, <kwang@eecs.wsu.edu>
!This program is free software: you can redistribute it and/or modify
!it under the terms of the GNU General Public License as published by
!the Free Software Foundation, either version 3 of the License, or
!(at your option) any later version.

!This program is distributed in the hope that it will be useful,
!but WITHOUT ANY WARRANTY; without even the implied warranty of
!MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!GNU General Public License for more details.

!You should have received a copy of the GNU General Public License
!along with this program.  If not, see <http://www.gnu.org/licenses/>.
!********************************************************************/

# -------------- ts.s file --------------------------- 
          MTXSEG = 0x1000
	
         .globl  _main,_proc,_procSize,_running,_scheduler
         .global  _tswitch

	
          jmpi  start, MTXSEG
start:    mov   ax,cs
          mov   ds,ax
          mov   ss,ax
	
          mov   sp,#_proc       ! sp->proc
          add   sp, _procSize   ! sp->high end of proc
	
          call  _main

dead:	  jmp   dead
	
_tswitch:
SAVE:	  push  ax
          push  bx
          push  cx
          push  dx
          push  bp
          push  si
          push  di
	  pushf
	  mov   bx,_running
 	  mov   2[bx], sp

NEXT_RUN: call	_scheduler

_resume:
RESUME:   mov	bx,_running
	  mov	sp, 2[bx]
	  popf
	  pop   di
          pop   si
          pop   bp
          pop   dx
          pop   cx
          pop   bx
          pop   ax

	  ret
