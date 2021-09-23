ENTRY(mymain)

SECTIONS {
  . = 0x10000;
  .text :{
    *(.text)
  }
  . = 0x8000000;
  data_start = .;
  .data :{
    *(.data)
  }
  data_end = .;
  .bss :{
    *(.bss)
  }
}
