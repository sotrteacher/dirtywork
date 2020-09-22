        .globl _main,_syscall,_exit
        .globl _getcs

start:
        call _main

! if ever return, exit(0)
	push  #0            ! push exitValue 0
        call  _exit         ! call exit(value) function in Umode
! int syscall(a,b,c,d) from C code 
_syscall:
        int    80
        ret
_getcs:                     ! getcs() return CS segment register
        mov   ax, cs
        ret

