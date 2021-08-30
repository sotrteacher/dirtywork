SECTIONS
{
        . = 0x0;
        abc : {
                *(abc)
        }
        . = 100;
        def : AT (ADDR(abc) + SIZEOF(abc)) {
                *(def)
        }

}
