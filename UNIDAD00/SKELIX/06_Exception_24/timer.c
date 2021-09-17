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
	outb(inb(0x21)&0xfe, 0x21);
}

volatile unsigned int timer_ticks = 0;

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
