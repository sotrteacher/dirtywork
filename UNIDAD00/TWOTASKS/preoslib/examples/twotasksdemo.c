typedef unsigned long DWORD;
typedef unsigned short WORD;
DWORD _stkbase;
DWORD _stktop;

void halt(void)
{
	#if 0 //LMC 2021.08.18
	message("Halt called");
        #endif
	__asm__("hlt");
}

