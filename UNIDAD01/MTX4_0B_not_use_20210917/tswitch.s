;!********************************************************************
;!Copyright 2010-2015 K.C. Wang, <kwang@eecs.wsu.edu>
;!This program is free software: you can redistribute it and/or modify
;!it under the terms of the GNU General Public License as published by
;!the Free Software Foundation, either version 3 of the License, or
;!(at your option) any later version.
;
;!This program is distributed in the hope that it will be useful,
;!but WITHOUT ANY WARRANTY; without even the implied warranty of
;!MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;!GNU General Public License for more details.
;
;!You should have received a copy of the GNU General Public License
;!along with this program.  If not, see <http://www.gnu.org/licenses/>.
;!********************************************************************/
;
;# -------------- tswitch.asm file --------------------------- 
;          MTXSEG = 0x1000
;	
;         .globl  _main,_proc,_procSize,_running,_scheduler
;         .global  _tswitch
extern running
extern scheduler
global running
global tswitch
;	
;          jmpi  start, MTXSEG
;start:    mov   ax,cs
;          mov   ds,ax
;          mov   ss,ax
;	
;          mov   sp,#_proc       ! sp->proc
;          add   sp, _procSize   ! sp->high end of proc
;	
;          call  _main
;
;dead:	  jmp   dead
;	
;_tswitch:
tswitch:
SAVE:	  push  eax
          push  ebx
          push  ecx
          push  edx
          push  ebp
          push  esi
          push  edi
	  pushf
	  mov   ebx,running
 	  ;mov   2[ebx], esp
 	  mov   [ebx+2], esp

NEXT_RUN: call	scheduler

_resume:
RESUME:   mov	ebx,running
	  ;mov	esp, 2[ebx]
	  mov	esp, [ebx+2]
	  popf
	  pop   edi
          pop   esi
          pop   ebp
          pop   edx
          pop   ecx
          pop   ebx
          pop   eax

	  ret
