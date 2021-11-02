/* Skelix by Xiaoming Mo (skelixos@gmail.com)
 * Licence: GPLv2 */
#include <asm.h>
#include <task.h>
#include <scr.h>
#include <kprintf.h>

void timer_install(int hz) {
	unsigned int divisor = 1193180/hz;
	outb(0x36, 0x43);
	outb(divisor&0xff, 0x40);
	outb(divisor>>8, 0x40);
	outb(inb(0x21)&0xfe, 0x21);       /** unmask the timer interrupt */
}

volatile unsigned int timer_ticks = 0;
unsigned int *timer_ticks_Pt = &timer_ticks;

void do_timer(void) {
	struct TASK_STRUCT *v = &TASK0;
	int x, y;
	++timer_ticks;
	get_cursor(&x, &y);
	set_cursor(71, 24);
	kprintf(KPL_DUMP, "%x", timer_ticks);
	set_cursor(x, y);
	outb(0x20, 0x20);
	cli();
	for (; v; v=v->next) {
		if (v->state == TS_RUNNING) {
			if ((v->priority+=30) <= 0)
				v->priority = 0xffffffff;
		} else
			v->priority -= 10;
	}
	if (! (timer_ticks%1))
		scheduler();
	sti();
}

#if 1 //LMC 2021.10.21
extern volatile int *x_Pt;
extern volatile int *y_Pt;
int arr[2];
#endif
#if 0 //LMC 2021.10.26
void my_do_timer(void) {
	//struct TASK_STRUCT *v = &TASK0;
	int x=40, y=12;
	
	++timer_ticks;
	//get_cursor(&x, &y);
	get_cursor(x_Pt, y_Pt);
//goto out_my_do_timer;	
	//get_cursor(arr,arr+1);
	set_cursor(71, 24);
//#if 1 //2021.10.20
#if 0 //2021.10.20
	while(!0){}
#endif	
    kprintf(KPL_DUMP, "%x", timer_ticks);
	//set_cursor(x, y);
	set_cursor(*x_Pt, *y_Pt);
	//set_cursor(arr[0],arr[1]);
//  #if 0 //LMC 2021.10.19	
//	outb(0x20, 0x20);
//	cli();
//	for (; v; v=v->next) {
//		if (v->state == TS_RUNNING) {
//			if ((v->priority+=30) <= 0)
//				v->priority = 0xffffffff;
//		} else
//			v->priority -= 10;
//	}
//	if (! (timer_ticks%1))
//		scheduler();
//	sti();
//  #endif	

out_my_do_timer:  
  return;
}/*end my_do_timer()*/
#endif

