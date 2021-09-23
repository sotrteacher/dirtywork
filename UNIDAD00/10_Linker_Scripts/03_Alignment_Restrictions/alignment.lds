SECTIONS {
  abc :{
    *(abc)
  }   
  def :{
    . = ALIGN(8);
    *(def)
  }
}
