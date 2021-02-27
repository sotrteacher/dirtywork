/* ============================================================ */
/* File: LDSCRIPT.C						*/
/*								*/
/* Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall	*/
/*								*/
/* Purpose: Creates linker script file for use with ld.		*/
/*								*/
/* Modification History:					*/
/*								*/
#define		 BUG_FEB_20_2002
/*	Found by Wayne Gibson (wgibson@speakeasy.net).		*/
/*	Execution of LGDT instruction in real mode still had	*/
/*	default size of 64KB for DS. If data segment contained	*/
/*	more than 64KB, the LGDT instruction would cause an	*/
/*	exception. Solution was to create a new "SECTION .init"	*/
/*	and place Init_CPU and Check_CPU code, as well as the	*/
/*	the gdt and gdt_info data structures. Also required	*/
/*	change to LDSCRIPT and resulting LINK.CMD file.		*/
/*								*/
/* ============================================================ */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* ---------------------------- */
/* Usage: ldscript >linker.cmd	*/
/* ---------------------------- */

char *title[] =
{
"/* ------------------------------------------------------------ */",
"/* Script file for linker ld.  Designed for use on an IBM-PC.   */",
"/* ------------------------------------------------------------ */",
NULL
} ;

char *script[] =
{
"OUTPUT_FORMAT(\"binary\")",
"OUTPUT(\"embedded.bin\")",
"EXTERN(start)",
"ENTRY(start)",
"MEMORY {",
"   /* --------------------------------------------------------- */",
"   /* The loader starts execution of the application at         */",
"   /* physical address zero, so .start must be placed in        */",
"   /* conventional memory, thus '>conventional'. You may put    */",
"   /* one or more of the remaining sections in extended memory  */",
"   /* by replacing their respective '>conventional' directives  */",
"   /* with a '>extended' directive where it appears below. If   */",
"   /* you do, however, the application will not run on a        */",
"   /* machine with no extended memory.                          */",
"   /* --------------------------------------------------------- */",
"   conventional : ORIGIN = 0x00000000, LENGTH = 640*1024",
"       reserved : ORIGIN = 0x000A0000, LENGTH = 384*1024",
"       extended : ORIGIN = 0x00100000, LENGTH = 4095*1024*1024",
"}",
"",
"SECTIONS {",
"   .text 0x00000000 : {",
"       text_frst = . ;",
"       *(.start)",
"       *(.init)",
"       *(.text)",
"       text_last = . - 1 ;",
"   } >conventional",
"",
"   .data : {",
"       data_frst = . ;",
"       *(.data)",
"       data_last = . - 1 ;",
"   } >conventional",
"",
"   .bss : {",
"       bss_frst = . ;",
"       *(.bss)",
"       *(COMMON)",
"       bss_last = . - 1 ;",
"   } >conventional",
"",
"   /* --------------------------------------------------------- */",
"   /* The stack sits just above the bss.  It's size is set here */",
"   /* so that it can be changed without recompiling the code.   */",
"   /* --------------------------------------------------------- */",
"   stack_frst = bss_last + 1 ;",
"   stack_last = bss_last + 32768 ;",
"",
"   /* --------------------------------------------------------- */",
"   /* The heap starts just above the stack and gets the rest.   */",
"   /* --------------------------------------------------------- */",
"   heap_frst = stack_last + 1 ;",
"}",
NULL
} ;

void Error(char *msg, ...)
	{
	va_list arg ;

	va_start(arg, msg) ;
	fprintf(stderr, "\nError: ") ;
	vfprintf(stderr, msg, arg) ;
	fprintf(stderr, "\n") ;
	va_end(arg) ;
	exit(255) ;
	}

void Output(char **text)
	{
	while (*text != NULL) printf("%s\n", *text++) ;
	}

void Library(char *name, char *spec, int size)
	{
	char cmd[500] ;
	FILE *fp ;

	sprintf(cmd, "gcc -print-file-name=%s.a", name) ;
	fp = popen(cmd, "r") ;
	if (!fp) Error("Can't popen: %s", cmd) ;
	fgets(spec, size, fp) ;
	*strchr(spec, '\n') = '\0' ;
	pclose(fp) ;
	}

int main(void)
	{
	char filespec[500] ;

	Output(title) ;
	printf("INPUT(\"./libepc.a\")\n") ;
	Library("libc", filespec, sizeof(filespec)) ;
	printf("INPUT(\"%s\")\n", filespec) ;
	Library("libgcc", filespec, sizeof(filespec)) ;
	printf("INPUT(\"%s\")\n", filespec) ;
	Output(script) ;

	return 0 ;
	}
