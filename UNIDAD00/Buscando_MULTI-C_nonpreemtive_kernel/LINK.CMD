/* ------------------------------------------------------------ */
/* Script file for linker ld.  Designed for use on an IBM-PC.   */
/* ------------------------------------------------------------ */
INPUT("./libepc.a")
INPUT("c:/djgpp/lib/libc.a")
INPUT("c:/djgpp/lib/gcc-lib/djgpp/2.953/libgcc.a")
OUTPUT_FORMAT("binary")
OUTPUT("embedded.bin")
EXTERN(start)
ENTRY(start)
MEMORY {
   /* --------------------------------------------------------- */
   /* The loader starts execution of the application at         */
   /* physical address zero, so .start must be placed in        */
   /* conventional memory, thus '>conventional'. You may put    */
   /* one or more of the remaining sections in extended memory  */
   /* by replacing their respective '>conventional' directives  */
   /* with a '>extended' directive where it appears below. If   */
   /* you do, however, the application will not run on a        */
   /* machine with no extended memory.                          */
   /* --------------------------------------------------------- */
   conventional : ORIGIN = 0x00000000, LENGTH = 640*1024
       reserved : ORIGIN = 0x000A0000, LENGTH = 384*1024
       extended : ORIGIN = 0x00100000, LENGTH = 4095*1024*1024
}

SECTIONS {
   .text 0x00000000 : {
       text_frst = . ;
       *(.start)
       *(.init)
       *(.text)
       text_last = . - 1 ;
   } >conventional

   .data : {
       data_frst = . ;
       *(.data)
       data_last = . - 1 ;
   } >conventional

   .bss : {
       bss_frst = . ;
       *(.bss)
       *(COMMON)
       bss_last = . - 1 ;
   } >conventional

   /* --------------------------------------------------------- */
   /* The stack sits just above the bss.  It's size is set here */
   /* so that it can be changed without recompiling the code.   */
   /* --------------------------------------------------------- */
   stack_frst = bss_last + 1 ;
   stack_last = bss_last + 32768 ;

   /* --------------------------------------------------------- */
   /* The heap starts just above the stack and gets the rest.   */
   /* --------------------------------------------------------- */
   heap_frst = stack_last + 1 ;
}
