OUTPUT_FORMAT("a.out-i386-linux","a.out-i386-linux","a.out-i386-linux")
OUTPUT_ARCH(i386)
/*OUTPUT_FORMAT("elf32-i386","elf32-i386","elf32-i386")
OUTPUT_ARCH(i386)*/
SECTIONS
{
        . = 0x0;
        abc : {
                *(abc)
        }
        /*. = 100;*/
        . = 100;
        def : {
                *(def)
        }

}
