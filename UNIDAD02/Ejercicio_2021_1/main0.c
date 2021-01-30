 /*---------------------------------------------------------------------
 * File 'main0.c'                               By LMC.
 *
 * 
 * A concurrent program.
 *
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <debug_marte.h> // For Debugging
#include <stdbool.h> /*for bool type name*/
#include <stdint.h>  /*for uint8_t type name*/
#include <misc/console_management.h>  /*reset_blocking_mode,set_raw_mode*/
#include <pthread.h>
#include <misc/timespec_operations.h> /*double_to_timespec()*/
#include <stdlib.h> /*exit()*/
static pthread_t thread1,thread2;
volatile int thread_signal;

#define SETTING	1
#define TIMING	2
uint8_t estado;

typedef struct time_bomb {
  uint8_t timeout;
  uint8_t fine_time;
  uint8_t code;
  uint8_t defuse;
} TimeBomb;
TimeBomb TB;
TimeBomb *tb = &TB;

char keyboard_input(void) {
  //static bool result = 0;
  // check keyboard and flush buffer (flushing so works in raw mode)
  char key = getchar();
  while (getchar() != -1 );

  return key;
}/*keyboard_input()*/

void *thread1_handler (void *arg) {
  double thread1_interval = 0.1;   /* 0.1 second */
  struct timespec thread1_ts;
  //int *thread1_status = (int *)status;
  (void)arg; /*to avoid the unused parameter compiler warning*/
  double_to_timespec(thread1_interval,&thread1_ts);
  for (;;) {
    if (estado == TIMING) {
      if (tb->fine_time) {/* d\'ecimas > 0 */
        --tb->fine_time;
      }else if (!(tb->timeout)) {/* d\'ecimas = 0 y segundos = 0 */
        goto boom;
      }else{/* d\'ecimas = 0 y segundos > 0 */
        --tb->timeout;
        tb->fine_time = 9;
      }
      set_cursorxy(0,0); 
      printf("Insert defuse code, then press key A!\n");
      set_cursorxy(0,1);
      printf("[State Bomb4_timing] countdown = %2d.%d\n",
              tb->timeout,tb->fine_time);
    }
    nanosleep(&thread1_ts,NULL);
  } /*end for()*/
boom:
  set_cursorxy(0,5);
  printf("BOOM!!!!\n");
  exit(0);
  return NULL;
}/*end thread1_handler()*/

void *thread2_handler (void *arg) {
  double thread2_interval = 0.01; /* 0.01 second */
  struct timespec thread2_ts;
  char c;
  (void)arg; /*to avoid the unused parameter compiler warning*/
  double_to_timespec(thread2_interval,&thread2_ts);
  for (;;) {
    c = keyboard_input();
    switch (c) {
      case 'a':{
        if (estado == SETTING) {
          estado = TIMING; 
          clrscr();
        }else if (estado == TIMING) {
          if (tb->code == tb->defuse) {
            estado = SETTING;
            tb->code = 0;
            tb->fine_time = 0;
            set_cursorxy(0,0); 
            printf("[State Bomb4_setting]                        \n");
            printf("Press U to move the timeout up               \n");
            printf("Press D to move the timeout down             \n");
            printf("Press A to arm the bomb and transition to Bomb4_timing state\n");
          }
        }
        break;
      }
      case 'd':{
        if (estado == SETTING) {
          if (tb->timeout > 1) {
            --tb->timeout;
            set_cursorxy(0,5);
            printf("timeout = %2d\n",tb->timeout);
          }
        }else if (estado == TIMING) {
          tb->code <<= 1;
        }
        break;
      }
      case 'u':{
        if (estado == SETTING) {
          if (tb->timeout < 60) {
            ++tb->timeout;
            set_cursorxy(0,5);
            printf("timeout = %2d\n",tb->timeout);
          }
        }else if (estado == TIMING) {
          tb->code <<= 1;
          tb->code |=1; /* If only UP button is pressed: 1,3,7,15, etc. */
        }
        break;
      }
    }/*---------------switch()*/
    nanosleep(&thread2_ts,NULL);
  }/*--------for()*/
  return NULL;
}/*end thread2_handler()*/

int main()
{
  // For Debugging
  //init_serial_communication_with_gdb (SERIAL_PORT_1);
  //set_break_point_here;

  //make getchar() non-blocking: 'getchar' returns -1 inmediately 
  //when there is no characters available at the moment of the call. 
  //x86_arch/include/misc/console_management.h
  reset_blocking_mode();
  set_raw_mode();// Every character is made available to the 
                 // calling program as soon as it is typed, so
                 // no line editing is available in this mode.
  disable_echo();// Input characters are not echoed
  clrscr();

  tb->timeout = 10;
  tb->code = 0;
  tb->defuse = 7;
  //estado = TIMING;
  estado = SETTING;
  set_cursorxy(0,0); 
  printf("[State Bomb4_setting]                        \n");
  printf("Press U to move the timeout up               \n");
  printf("Press D to move the timeout down             \n");
  printf("Press A to arm the bomb and transition to Bomb4_timing state\n");

  pthread_create(&thread1,NULL,thread1_handler,(void*)thread_signal);
  pthread_create(&thread2,NULL,thread2_handler,(void*)thread_signal);
  pthread_join(thread1,NULL);
  pthread_join(thread2,NULL);
  return 0;
}/*end main()*/


