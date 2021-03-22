/* Link.ld -- Linker script for the kernel - ensure everything goes in the */
/*            Correct place.  */
/*            Original file taken from Bran's Kernel Development */
/*            tutorials: http://www.osdever.net/bkerndev/index.php. */

ENTRY(start)
SECTIONS
{

    .text 0x100000 :
    /* .text 0x1000 : */
    {
       text_frst = . ;
        code = .; _code = .; __code = .;
        *(.text)
        . = ALIGN(4096);
       text_last = . - 1 ;
    }

    .data :
    {
       data_frst = . ;
        data = .; _data = .; __data = .;
        *(.data)
        *(.rodata)
        . = ALIGN(4096);
       data_last = . - 1 ;
    }

    .bss :
    {
       bss_frst = . ;
        bss = .; _bss = .; __bss = .;
        *(.bss)
        . = ALIGN(4096);
       bss_last = . - 1 ;
    }

    end = .; _end = .; __end = .;
    stack_frst = end + 1 ;
    stack_last = end + 32768 ;
   /* --------------------------------------------------------- */
   /* The heap starts just above the stack and gets the rest.   */
   /* --------------------------------------------------------- */
   heap_frst = stack_last + 1 ;
}
