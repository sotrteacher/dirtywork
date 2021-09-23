SECTIONS {
  . =0x0;
  abc :{
    *(abc)
  }
  . =100;
  def :{
    *(def)
  }
}
