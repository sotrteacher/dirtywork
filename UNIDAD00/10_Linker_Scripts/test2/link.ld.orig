OUTPUT_FORMAT("elf32-i386","elf32-i386","elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(notmain)
SECTIONS {
  .text :{
    *(.text)
  }
  data_start = .;
  .data :{
    *(.data)
  }
  .bss :{
    *(.bss)
  }
}
