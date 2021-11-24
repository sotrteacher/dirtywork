#------------ ts.s file -----------
.global tswitch, running, scheduler
tswitch:
SAVE: 
  pushal
  pushfl
  movl running,%ebx   # ebx -> PROC
  movl %esp, 4(%ebx)  # integers in GCC are 4 bytes # PROC.save_sp = esp
FIND: 
  call scheduler
RESUME: 
  movl running, %ebx  # ebx -> PROC
  movl 4(%ebx), %esp  # esp = PROC.saved_sp
  popfl
  popal
  ret



