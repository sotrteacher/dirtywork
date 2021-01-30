/*
 * Keyboard Interrupt.
 *
 * Example of use of the Interrupt Control API provided by MaRTE OS.
 *
 * By L. M. C.  based on the example 
 * hardware_interrupts/printer_port_interrupts_c.c by
 * Mario Aldea Rivas.
 */

#include <stdio.h>
#include <sys/pio.h>
#include <intr.h>

///*
// * Printer Port related stuff
// */
//   
//// Printer Port Registers
//#define PP_BASE_REG    0x378
//#define PP_DATA_REG    0     // Data port offset
//#define PP_STATUS_REG  1     // Status port offset
//#define PP_CONTROL_REG 2     // Control port offset
//
// Control bits
//#define PP_IENABLE 0x10  // Enable interrupt (disabled by default).

// stored data
volatile char pp_data;

// printer port interrupt handler
int printer_port_interrupt_handler(void * area, intr_t intr)
{
  printf ("In printer_port_interrupt_handler (intr:%d)\n", intr);
  pp_data = inb_p (PP_BASE_REG + PP_DATA_REG);
  return POSIX_INTR_HANDLED_NOTIFY;
}

// printer_port_configure
void printer_port_configure ()
{
  printf ("Configuring PP\n");
  // Install interrupt handler
  if (posix_intr_associate (PARALLEL1_HWINTERRUPT,
			    printer_port_interrupt_handler,
			    NULL, 0))
    perror ("posix_intr_associate");
  //  Enable printer port interrupt
  outb_p (PP_BASE_REG + PP_CONTROL_REG, PP_IENABLE);
  //  Enable the interrupt in the computer
  if (posix_intr_unlock (PARALLEL1_HWINTERRUPT))
    perror ("posix_intr_unlock");
}


/*
 * main
 */
int main ()
{
  int i;
  char data;
  struct timespec timeout = {5, 0};
  intr_t intr;
  int (*handler) (void * area, intr_t intr);

  printer_port_configure ();

  for (i=0; i<10; i++) {
    // Wait for interrupt
    if (clock_gettime (CLOCK_REALTIME, &timeout))
      perror ("clock_gettime");
    timeout.tv_sec += 5;
    if (posix_intr_timedwait (0, &timeout, &intr, &handler)) 
      perror ("hwinterrupts_wait");

    // Read data (with interrupt disabled)
    if (posix_intr_lock (PARALLEL1_HWINTERRUPT))
      perror ("posix_intr_lock");

    data = pp_data;

    if (posix_intr_unlock (PARALLEL1_HWINTERRUPT))
      perror ("posix_intr_unlock");

    printf ("Data read:%d\n", data);
  }

  return 0;
}/*end main()*/


