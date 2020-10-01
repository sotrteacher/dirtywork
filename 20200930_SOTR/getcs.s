#================== getcs.s ======================
.globl getcs,get_ebp,get_esp,get_rbp,rbph,rbpl

getcs:
  movl %cs, %eax
  ret
 
get_ebp:
  movl %ebp, %eax
  ret

get_esp:
  movl %esp, %eax
  ret

get_rbp:
  movq %rbp, %rax
#  shrq %rax, $20
#  movl %eax, (rbph)
  ret

#.data
#rbph:  .asciz "0000"
#rbpl:  .asciz "0000"

