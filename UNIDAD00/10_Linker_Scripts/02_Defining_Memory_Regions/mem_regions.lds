MEMORY {
  flash (rx) : ORIGIN = 0, LENGTH = 100
  ram (rwx) : ORIGIN = 100, LENGTH = 50
}
SECTIONS {
  abc : {
    *(abc)
  } >flash
  def : AT(ADDR(abc)+SIZEOF(abc)) {
    *(def)
  } >ram
}
