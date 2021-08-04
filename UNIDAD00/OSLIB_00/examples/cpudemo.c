/* Project:     OSLib
 * Description: The OS Construction Kit
 * Date:                1.6.2000
 * Idea by:             Luca Abeni & Gerardo Lamastra
 *
 * OSLin is an SO project aimed at developing a common, easy-to-use
 * low-level infrastructure for developing OS kernels and Embedded
 * Applications; it partially derives from the HARTIK project but it
 * currently is independently developed.
 *
 * OSLib is distributed under GPL License, and some of its code has
 * been derived from the Linux kernel source; also some important
 * ideas come from studying the DJGPP go32 extender.
 *
 * We acknowledge the Linux Community, Free Software Foundation,
 * D.J. Delorie and all the other developers who believe in the
 * freedom of software and ideas.
 *
 * For legalese, check out the included GPL license.
 */

/*	CPU Detection test file			*/

#include <ll/i386/stdlib.h>
#include <ll/i386/cons.h>
#include <ll/i386/mem.h>
#include <ll/i386/hw-arch.h>
#include <ll/sys/ll/ll-func.h>

#define T  1000

int main (int argc, char *argv[])
{
    DWORD sp1, sp2;
    struct ll_cpuInfo cpu;
    void *res;
    char vendorName[13];

    sp1 = get_SP();
    cli();
    res = ll_init();

    if (res == NULL) {
	    cprintf("Error in LowLevel initialization code...\n");
	    sti();
	    exit(-1);
    }
    sti();

    cprintf("LowLevel started...\n");
    cprintf("CPU informations:\n");
    X86_get_CPU(&cpu);

    cprintf("\tCPU type: %lu\n", cpu.X86_cpu);

    if (cpu.X86_cpuIdFlag) {
    	cprintf("CPUID instruction workin'...\n");
	cprintf("\tCPU Vendor ID: ");

	memcpy(vendorName, &(cpu.X86_vendor_1), 12);
	vendorName[12] = 0;
	cprintf("%s\n", vendorName);

	cprintf("\tCPU Stepping ID: %lx", cpu.X86_cpu & 0x0F);
	cprintf("\tCPU Model: %lx", (cpu.X86_cpu >> 4) & 0x0F);
	cprintf("\tCPU Family: %lx", (cpu.X86_cpu >> 8) & 0x0F);
	cprintf("\tCPU Type: %lx\n", (cpu.X86_cpu >> 12) & 0x0F);

	cprintf("\tStandard Feature Flags %lx\n", cpu.X86_StandardFeature);
	if (cpu.X86_StandardFeature & 0x01) {
		cprintf("Internal FPU present...\n");
	}
	if (cpu.X86_StandardFeature & 0x02) {
		cprintf("V86 Mode present...\n");
	}
	if (cpu.X86_StandardFeature & 0x04) {
		cprintf("Debug Extension present...\n");
	}
	if (cpu.X86_StandardFeature & 0x08) {
		cprintf("4MB pages present...\n");
	}
	if (cpu.X86_StandardFeature & 0x10) {
		cprintf("TimeStamp Counter present...\n");
	}
	if (cpu.X86_StandardFeature & 0x20) {
		cprintf("RDMSR/WRMSR present...\n");
	}
	if (cpu.X86_StandardFeature & 0x40) {
		cprintf("PAE present...\n");
	}
	if (cpu.X86_StandardFeature & 0x80) {
		cprintf("MC Exception present...\n");
	}
	if (cpu.X86_StandardFeature & 0x0100) {
		cprintf("CMPXCHG8B present...\n");
	}
	if (cpu.X86_StandardFeature & 0x0200) {
		cprintf("APIC on chip...\n");
	}
	if (cpu.X86_StandardFeature & 0x1000) {
		cprintf("MTRR present...\n");
	}
	if (cpu.X86_StandardFeature & 0x2000) {
		cprintf("Global Bit present...\n");
	}
	if (cpu.X86_StandardFeature & 0x4000) {
		cprintf("Machine Check present...\n");
	}
	if (cpu.X86_StandardFeature & 0x8000) {
		cprintf("CMOV present...\n");
	}
	if (cpu.X86_StandardFeature & 0x800000) {
		cprintf("MMX present...\n");
	}
    }
    cli();
    ll_end();
    sp2 = get_SP();
    cprintf("End reached!\n");
    cprintf("Actual stack : %lx - ", sp2);
    cprintf("Begin stack : %lx\n", sp1);
    cprintf("Check if same : %s\n",sp1 == sp2 ? "Ok :-)" : "No :-(");

    return 1;
}
