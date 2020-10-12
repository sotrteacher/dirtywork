! This is a very basic example of bootlader for a tiny operating system.
! This is an os loader only!
! It can be loaded at the first sector of a floppy disk:
!   cylinder: 0
!   sector: 1
!   head: 0

! The code in this file is supposed to load
! the kernel (mtx) and to pass control over it.
! The kernel code should be on floppy at:

!   cylinder: 0
!   sector: 2
!   head: 0

! loader file produced by this code should be less or
! equal to 512 bytes, since this is the size of the boot sector.

! boot record is loaded at 0000:7c00h by BIOS
!org 7c00h
!.globl _print_string
        BOOTSEG =   0x9800     ! Boot block is loaded again to here.
        OSSEG    =  0x1000
        SSP      =  32*1024
	BSECTORS =  1         ! load 18 sectors initially
# https://hackaday.io/project/18868-improbable-avr-8088-substitution-for-pcxt/log/51763-first-attempt-at-raw-808688-assembling
# https://www.win.tue.nl/~aeb/linux/lk/lk-3.html
#  "Without the export line, ld86 will complain ld86: no start symbol."
export _main
_main:
!! initialize the stack:
!mov     ax, #0x7c0
!mov     ss, ax
!mov     sp, #0x3fe ! top of the stack.

!! set data segment:
!xor     ax, ax
!mov     ds, ax
        mov  ax,#BOOTSEG
        mov  es,ax           ! set ES to BOOTSEG=0x9800
        xor  bx,bx

	xor  dx,dx
	xor  cx,cx
	incb cl

	movb ah,#2
	movb al,#BSECTORS
	int  0x13

	jmpi next,BOOTSEG
next:                    
	mov  ax,cs
	mov  ds,ax
	mov  ss,ax
	mov  sp,#SSP


! set default video mode 80x25:
mov     ah, #0x00
!mov     al, #0x03   ! To use only white color for text (for emu8086)
mov     al, #0x12   ! In order to use colors for text (for qemu-system-i386)
int     0x10

! print welcome message:
!lea     si, msg
mov     si, #msg
call    _print_string

!===================================
! load the kernel at 1000h:0000h
! 10 sectors starting at:
!   cylinder: 0
!   sector: 2
!   head: 0        

! BIOS passes drive number in dl,
! so it's not changed:

mov     ah, #0x02 ! read function.
mov     al, #5; <-- UPDATE THIS TO THE PROPER VALUE for each 
             ; 16-bit MTX version. 2020.10.09. 
mov     ch, #0   ; cylinder.
mov     cl, #2   ; sector.
mov     dh, #0   ; head.
! dl not changed! - drive number.

! es:bx points to receiving
!  data buffer:
mov     bx, #0x1000   
mov     es, bx
mov     bx, #0

! read!
int     0x13
!===================================

! pass control to kernel:
!jmp     1000h:0000h
!jmpi     #0x0000,#0x1000
jmpi     0x0000,OSSEG

!===========================================

!print_string proc near
_print_string: 
push    ax      ! store registers...
push    si      !
next_char:      
        mov     al, [si]
        cmp     al, #0
        jz      printed
        inc     si
        mov     ah, #0x0e   ! teletype function.
        mov     bx, #0x000c ! bL = cyan color
        int     0x10
        jmp     next_char
printed:
pop     si      ! re-store registers...
pop     ax      !
ret
!print_string endp

msg:    .asciz  "Loading..."

