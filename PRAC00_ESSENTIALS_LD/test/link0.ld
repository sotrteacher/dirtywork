SECTIONS
{
        . = 0x0;
        abc : {
                *(abc)
        }
       .text : {
               *(.text)
        }
        def : {
                *(def)
        }
}
