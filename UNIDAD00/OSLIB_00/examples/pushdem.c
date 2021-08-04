#include <ll/i386/stdlib.h>
#include <ll/i386/tss-ctx.h>
#include <ll/i386/cons.h>
#include <ll/sys/ll/ll-instr.h>
#include <ll/sys/ll/ll-func.h>

#define STACK_SIZE      4096U           /* Stack size in bytes          */
#define USE_FPU         0x0001

WORD th1, thm;

#define T 1000

#if 1
#define TO     ll_context_to
#define FROM   ll_context_from 
#else
#define TO     ll_context_load
#define FROM   ll_context_save 
#endif

/* For stack allocation checking */
BYTE stack1[STACK_SIZE];
BYTE stack2[STACK_SIZE];
	
void tfunc(void)
{
	cprintf("ThreadFunc: Switching to main thread\n");
	TO(thm);
	cprintf("ThreadFunc: I'm back\n");
}

void thread1(void *px)
{
	cprintf("Thread 1 running\n");
	cprintf("Calling ThreadFunc\n");
	tfunc();
	cprintf("Another time thread 1\n");
	cprintf("Back to main\n");
	TO(thm);
	cprintf("And now, finishing thread 1\n");
}

void alienfunc(void)
{
	cprintf("Who am I???\n");
}

void killer(void)
{
	cli();
	cprintf("Killer!!!\n");
	TO(thm);
}

int main (int argc, char *argv[])
{
    DWORD sp1, sp2;
    void *mbi;

    sp1 = get_SP();
    cli();

    mbi = ll_init();

    if (mbi == NULL) {
	    cprintf("Error in LowLevel initialization code...\n");
	    sti();
	    exit(-1);
    }
    sti();
    cprintf("LowLevel started...\n");
    th1 = ll_context_create(thread1, &stack1[STACK_SIZE], NULL,killer, 0);
    thm = FROM();
    cprintf("Thread 1 created\n");
    cprintf("Switching to it...\n");
    TO(th1);
    cprintf("Returned to main\n");
    cprintf("Let's try if the push func works...\n");
    cprintf("It returned %lu\n", ll_push_func(alienfunc));
    cprintf("And now, to thread 1 again!!!\n");
    TO(th1);
    cprintf("Main another time...\n");
    TO(th1);
    cprintf("OK, now I finish...\n");
    cli();
    ll_end();
    sp2 = get_SP();
    cprintf("End reached!\n");
    cprintf("Actual stack : %lx - ", sp2);
    cprintf("Begin stack : %lx\n", sp1);
    cprintf("Check if same : %s\n",sp1 == sp2 ? "Ok :-)" : "No :-(");
    return 1;
}
