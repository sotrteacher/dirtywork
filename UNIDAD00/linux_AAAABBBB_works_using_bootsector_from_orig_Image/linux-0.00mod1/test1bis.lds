/*OUTPUT_FORMAT("a.out-i386-linux","a.out-i386-linux","a.out-i386-linux")
OUTPUT_ARCH(i386)*/
/*OUTPUT_FORMAT("elf32-i386","elf32-i386","elf32-i386")
OUTPUT_ARCH(i386)*/
OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386)
ENTRY(startup_32)
SECTIONS
{
        . = 0x0;
        .data : {
        /*.bss : {*/
                *(.rawdata*)
              /*  *(.text) */
         /* *( .rowdata* .text ) */
         /*  *( .rowdata .text )*/
         /*  . = 0x2000;*/
        }
        . = 0x400;
        /*. = 0x15a4;*/
        .text : {
                *(.text) 
        /*        *(.text .data)*/
          . = 0x2000;     
        }
        /*. = 100;*/
        /*. = 0x2000;*/
        /*def : {
                *(def)
        }*/
		/*. = 0x2400;*/
        .data : {
	      /*. = ALIGN(2);*/
                *(.data) 
        }
        /*.bss : {*/
	  	/*. = ALIGN(0x1000);*/
        /*        *(.bss)*/
        /*}*/

}
