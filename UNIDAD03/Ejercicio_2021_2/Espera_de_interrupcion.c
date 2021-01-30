#include <intr.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <misc/error_checks.h>
#include <misc/timespec_operations.h>
#define FUENTE_ITERRUPCION	RTC_HWINTERRUPT

// Interrupt handler
int interrupt_handler(void * area,intr_t intr)
{
 (void)area;
 printc("In interrupt handler (%d)\n",intr);
 return POSIX_INTR_HANDLED_NOTIFY;
}

int main()
{
 const struct timespec rel_timeout = {5, 0};
 struct timespec timeout;
 intr_t intr;
 int (*handler)(void * area,intr_t intr);

 // Install interrupt handler
 CHK( posix_intr_associate(FUENTE_ITERRUPCION,interrupt_handler,
                           NULL,0));

 // Enable the interrupt in the computer
 CHK( posix_intr_unlock(FUENTE_ITERRUPCION));

 // Read initial time and wait interrupts in a loop
 CHKE( clock_gettime(CLOCK_REALTIME,&timeout));
 while(1) {
  // timeout calculation
  add_timespec(&timeout,&timeout,&rel_timeout);

  // wait for interrupt
  CHK( posix_intr_timedwait(0,&timeout,&intr,&handler));
  printf("After interrupt\n");
 }
}/*end main()*/


