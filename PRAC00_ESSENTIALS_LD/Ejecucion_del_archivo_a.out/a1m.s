.section abc, "a"
#.globl i
i:
  .byte 1
  .byte 2
  .byte 3

.globl k
.section def, "a"
k:
  .byte 7
aqui:
  jmp aqui
  .byte 8
  .byte 9

