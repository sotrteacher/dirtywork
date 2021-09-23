SECTIONS {
  . =0x0;
  abc :{
    *(abc)
  }
  . =100;/*Esto determina la VMA*/
  def :AT(ADDR(abc)+SIZEOF(abc)){/*esto determina la LMA*/
    *(def)
  }
}
