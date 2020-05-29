#---------------- ts.s file -----------------------
.globl running, scheduler, tswitch
tswitch:
SAVE: 
  pushl %eax
  pushl %ebx
  pushl %ecx
  pushl %edx
  pushl %ebp
  pushl %esi
  pushl %edi
  pushfl
  movl running,%ebx # ebx -> PROC
  movl %esp,4(%ebx) # PROC.save_sp = esp
FIND: 
  call scheduler
RESUME: 
  movl running,%ebx # ebx -> PROC
  movl 4(%ebx),%esp # esp = PROC.saved_sp
  popfl
  popl %edi
  popl %esi
  popl %ebp
  popl %edx
  popl %ecx
  popl %ebx
  popl %eax
  ret

# stack contents = |retPC|eax|ebx|ecx|edx|ebp|esi|edi|eflag|
#                    -1    -2  -3  -4  -5  -6  -7  -8  -9


