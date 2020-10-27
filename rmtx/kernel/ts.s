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

!**************************************************************************
!                ts.x file: MTX assembly code
!**************************************************************************
                BOOTSEG = 0x1000
                SSP     = 0
!               IMPORTS and EXPORTS
.globl _main,_prints
.globl _procsize,_proc
.globl _getds,_setds
.globl _tswitch,_running,_nextrun,_schedule,_sw_flag

.globl _int80h,_kcinth,_goUmode
.globl _tinth,_thandler
.globl _s0inth,_s0handler
.globl _s1inth,_s1handler
.globl _kbinth,_kbhandler,_kreboot, _ksync
.globl _fdinth, _fdhandler
.globl _hdinth,_hdhandler
.globl _cdinth,_cdhandler

.globl _divide,_kdivide,_pinth, _phandler
.globl _trap,_ktrap

.globl _lock,_unlock,_int_on, _int_off
.globl _in_byte,_out_byte,_in_word,_out_word
.globl _idle, _reschedule, _kpsig


export auto_start
auto_start:
        jmp     start
! these are patched in by h.c, which reads mtx header and writes DS,dsize,bsize

kds:     .word   0       ! DS = 0x1000 + tsize/16                
dsize:   .word   0       ! dsize in bytes
bsize:   .word   0       ! bsize in bytes
rootdev: .word   0x0303  ! boot device (major,minor) number
lo:	 .word   0       ! 10
hi:	 .word   0       ! 12
	
start:
        mov     ax,cs                   ! establish segments 
        mov     ds,ax                   ! we know ES,CS=0x1000. Let DS=CS  

        mov     ax,2                    ! get DS, bss, bsize while ds=0x1000 
        mov     di,4                    ! dsize 
        mov     si,6                    ! bsize
        mov     dx,8                    ! root device 

        mov     ds,ax
        mov     ss,ax                   ! SS = DS ===> all to 0x1000+tsize/16
        mov     es,ax

! must clear bss before using P0's kstack
! Segment=DS; startAddress=dsize from DS; #words to move=bsize/2
        mov     cx,si           ! bsize
        shr     cx,#1           ! divide by 2
        mov     bx,di           ! starting address
again:
        mov     0[bx],#0        ! write zero to 0[bx]
        add     bx,#2           ! bx += 2
        dec     cx              ! cx--
        jne     again

! now we may use P0's stack in bss section
        mov     sp,#_proc        ! proc[0]
        add     sp,_procsize
       
        push    si                      ! push bsize 
        push    di                      ! push dsize
        push    dx                      ! push rootdev

! KCW : no-color before calling vid_init() in main(), else no video
        mov     ax,#0x0002
        int     0x10

mtx:    call    _main                   ! call main(dsize, bzise) in C

! MTX main() never returns
hang:   jmp     hang

_idle:  hlt
        ret
!*************************************************************
!             tswitch()
!*************************************************************
_tswitch:
          cli
          push   ax
          push   bx
          push   cx
          push   dx
          push   bp
          push   si
          push   di
	  pushf
	  mov	 bx, _running
 	  mov	 2[bx], sp

find:     call	 _nextrun

resume:	  mov	 bx, _running
	  mov	 sp, 2[bx]
	  popf
	  pop    di
          pop    si
          pop    bp
          pop    dx
          pop    cx
          pop    bx
          pop    ax
	
          sti
          ret
! -------------- end of tswitch() -------------------
INK =   4
USS =   6
USP =   8
! as86 macro: parameters are ?1 ?2, etc
        MACRO INTH
          push ax
          push bx
          push cx
          push dx
          push bp
          push si
          push di
          push es
          push ds

          mov  ax,cs
          mov  ds,ax           ! DS = CS = 0x1000 
          mov  ax,2
          mov  ds,ax           ! get real DS

	  mov bx,_running   	! ready to access proc
          inc INK[bx]
          cmp INK[bx],#1
          jg   ?1

          ! was in Umode: save interrupted (SS,SP) into proc
          mov USS[bx],ss        ! save SS  in proc.USS
          mov USP[bx],sp        ! save SP  in proc.USP

          ! Set kernel sp to proc[i].ksp 
          mov  ax,ds            ! stupid !!        
          mov  es,ax            ! DS=SS=ES in Kmode
          mov  ss,ax

	  mov  sp,bx     	! proc's kstack [2 KB]
          add  sp,_procsize
          sti
?1:
          call _?1
          br   _ireturn
         MEND

! interrupt handlers; by MACRO INTH(label, handler, parameter)

_int80h: INTH kcinth

_s0inth: INTH s0handler

_s1inth: INTH s1handler

_tinth:  INTH thandler

_kbinth: INTH kbhandler

_pinth:  INTH phandler

_divide: INTH kdivide

_trap:   INTH ktrap

_fdinth: INTH fdhandler

_hdinth: INTH hdhandler

_cdinth: INTH cdhandler

!*===========================================================================*
!*		_ireturn  and  goUmode()       				     *
!*===========================================================================*
! ustack contains    flag,ucs,upc, ax,bx,cx,dx,bp,si,di,es,ds
! uSS and uSP are in proc
_ireturn:
_goUmode:
        cli
	mov bx,_running 	! bx -> proc
        dec INK[bx]
        cmp INK[bx],#0
        jg  xkmode

! return to Umode: 
! handle any outstanding signal;
! switch process if readyQueue has procs with higher priority
        call _kpsig             ! try to handle signal if any
        call _reschedule        !  
	mov bx,_running 	! bx -> proc
        mov ax,USS[bx]
        mov ss,ax               ! restore SS
        mov sp,USP[bx]          ! restore SP
xkmode:  
	pop ds
	pop es
	pop di
        pop si
        pop bp
        pop dx
        pop cx
        pop bx
        pop ax 
        iret
!*===========================================================================*
!*		      getds()/setds()                      		     *
!*===========================================================================*
_getds:  mov ax,ds
         ret
_setds: 
	push bp			
	mov  bp,sp		
	mov  ds,4[bp]		! load ds with segment value
	pop  bp
	ret
!*===========================================================================*
!*		      reboot()                              		     *
!*===========================================================================*
_kreboot:
        call _ksync
        jmpi 0,0xFFFF	

!*===========================================================================*
!*		      int_on()/int_off()                      		     *
!*===========================================================================*
_int_off:                       ! cli, return old flag register
        pushf
        cli
        pop ax
        ret

_int_on:                        ! int_on(int SR)
        push bp
        mov  bp,sp
        mov  ax,4[bp] ! get SR passed in
        push ax
        popf
        pop  bp
        ret

_lock:  
	cli			! disable interrupts
	ret			! return to caller
_unlock:
	sti			! enable interrupts
	ret			! return to caller

!*===========================================================================*
!*		in_byte/in_word					     *
!*===========================================================================*
! Read a byte from i/o port

_in_byte:
        push    bp
        mov     bp,sp
        mov     dx,4[bp]
	inb     al,dx		! input 1 byte
	subb	ah,ah		! unsign extend
        pop     bp
        ret

! in_word(port)
_in_word:
        push    bp
        mov     bp,sp
        mov     dx,4[bp]
	in      ax,dx		! input 1 word
        pop     bp
        ret
!*==================================================================*
!*	out_byte/out_word: out_byte(port, int)                     *
!*==================================================================*
_out_byte:
        push    bp
        mov     bp,sp
        mov     dx,4[bp]
        mov     ax,6[bp]
	outb	dx,al   	! output 1 byte
        pop     bp
        ret

_out_word:
        push    bp
        mov     bp,sp
        mov     dx,4[bp]
        mov     ax,6[bp]
	out	dx,ax   	! output 2 bytes
        pop     bp
        ret
!*===========================================================================*
!			  biostime
!                get real time from BIOS
!*===========================================================================*

.globl _biostime,_btime !centry,_year,_month,_day, _bhr,_bmin,_bsec

_biostime: ! read BIOS time by INT 0x1A;2,4
        mov     ax,#0x0400
        int     0x1A

! upon return: CH=centry, CL=year, DH=month,DL=day; all in BCD
        movb   _btime+0,ch
        movb   _btime+1,cl
        movb   _btime+2,dh
        movb   _btime+3,dl

        mov     ax,#0x0200
        int     0x1A
! upon return: CH=bhr, CL=bmin, DH=bsec, DL=0
        movb    _btime+4,ch
        movb    _btime+5,cl
        movb    _btime+6,dh
        movb    _btime+7,dl
        ret
!----------------------- end of ts.x file ----------------------------------
