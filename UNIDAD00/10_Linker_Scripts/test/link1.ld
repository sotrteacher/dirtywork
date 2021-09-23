SECTIONS {
  . = 0x10000;
  abc :{
    *(abc)
  }
  . = 0x8000000;
  def :{
    *(def)
  }
  .bss :{
    *(.bss)
  }
}
