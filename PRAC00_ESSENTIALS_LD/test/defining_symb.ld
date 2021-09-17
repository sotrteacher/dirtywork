SECTIONS {
  abc :{
    _sabc = .;
    *(abc)
    _eabc = .;
  }
  def :{
    *(def)
  }
}
