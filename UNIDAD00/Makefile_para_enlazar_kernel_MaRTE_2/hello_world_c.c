/*----------------------------------------------------------------------------
 *-------------------------      M a R T E   O S      ------------------------
 *----------------------------------------------------------------------------
 *                                                             V2.0 2017-02-22
 *
 *                            H e l l o    W o r l d
 *
 *                                    C
 *
 * File 'hello_world.c'                                              By MAR.
 *
 * 
 * A simple "hello world" program.
 *
 *---------------------------------------------------------------------------*/

#include <stdio.h>
//#include <debug_marte.h> // For Debugging


int main()
{
  // For Debugging
  //init_serial_communication_with_gdb (SERIAL_PORT_1);
  //set_break_point_here;

  printf("\nHello, I'm a C program running on MaRTE OS.\n\n");

  return 0;
}
