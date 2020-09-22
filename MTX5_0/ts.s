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

        MTXSEG  = 0x1000
	
       .globl _main,_running,_scheduler
       .globl _proc, _procSize
       .globl _tswitch
	
        jmpi   start,MTXSEG

start:	mov  ax,cs
	mov  ds,ax
	mov  ss,ax
        mov  es,ax
	mov  sp,#_proc
	add  sp,_procSize
		
	call _main

dead:   jmp  dead     ! De main no deber\'ia regresar, pero si regresa, quiero 
                      ! que se quede aqu\'i.

_tswitch:
SAVE:	
                cli
	push ax
	push bx
	push cx
	push dx
	push bp
	push si
	push di
	pushf
	mov  bx,_running
	mov  2[bx],sp

FIND:	call _scheduler

RESUME:
	mov  bx,_running
	mov  sp,2[bx]
	popf
	pop  di
	pop  si
	pop  bp
	pop  dx
	pop  cx
	pop  bx
	pop  ax
                sti
	ret

	
! added functions for KUMODE
	.globl _int80h,_goUmode,_kcinth!,_dummy
!These offsets are defined in struct proc
USS =   4                      ! offsets of uss, usp in PROC
USP =   6

_int80h:
KSAVE:
        push ax                 ! save all Umode registers in ustack
        push bx
        push cx
        push dx
        push bp
        push si
        push di
        push es
        push ds

! ustack contains : |flag,uCS,uPC|ax,bx,cx,dx,bp,si,di,ues,uds|
!                                                          usp 
! change DS to KDS in order to access data in Kernel space
!        push cs
!        pop  ds                 ! KDS now
!
!	mov si,_running  	! ready to access proc
!        mov USS[si],ss          ! save uSS  in proc.USS
!        mov USP[si],sp          ! save uSP  in proc.USP
!
!! Change ES,SS to kernel segment
!        mov  di,ds              ! stupid !!        
!        mov  es,di              ! CS=DS=SS=ES in Kmode
!        mov  ss,di
!
!! set sp to HI end of running's kstack[]
!	mov  sp,_running        ! proc's kstack [2 KB]
!        add  sp,_procSize       ! HI end of PROC
! change DS to KDS in order to access data in Kernel space
        mov ax,cs        ! assume one-segment kernel, change DS to kDS
        mov ds,ax        ! let DS=CS  = 0x1000 = kernel DS
! All variables are now offsets relative to DS=0x1000 of Kernel space
! Save running proc's Umode (uSS,uSP) into its PROC.(uss,usp)
        mov   bx,_running ! bx points to running PROC in K space
        mov   USS[bx],ss ! save uSS in proc.uss
        mov   USP[bx],sp ! save uSP in proc.usp
! change ES, SS to Kernel segment 0x1000
        mov ax,ds        ! CPU must move segments this way!
        mov es,ax
        mov ss,ax        ! SS is now KSS = 0x1000
! switch running proc's stack from U space to K space
        !mov sp,bx        ! sp points at running PROC
        mov sp,_running  ! sp points at running PROC
        add sp,_procSize ! sp -> HIGH END of running 's kstack
! We are now completely in K space, stack=running proc's EMPTY kstack 
!**************   CALL handler function in C  ***********************
        call  _kcinth    ! call kcinth() in C;
        jmp   _goUmode
!**************   RETURN TO Umode ***********************************  
_goUmode:
        cli                     ! mask out interrupts
	mov bx,_running 	! bx -> running PROC
        mov ax,USS[bx]
        mov ss,ax               ! restore uSS
        mov sp,USP[bx]          ! restore uSP
  
	pop ds
	pop es
	pop di
        pop si
        pop bp
        pop dx
        pop cx
        pop bx
        pop ax      ! NOTE: return value must be in AX in ustack
!        mov ax,#0xcafe
!dead:   jmp dead
!        jmp 0x2000, 0x0000
        iret

	
