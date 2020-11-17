/** simple_periodic_thread_1.c
 * Create a periodic thread using 'clock_nanosleep()'.
 */
#include <time.h>
#include <pthread.h>
#include <misc/timespec_operations.h>
#include <misc/error_checks.h>        /* CHK() */ 

/* Periodic thread */
void * periodic1 (void *arg)
{
  int activation_count = 0;
  struct timespec my_period, next_activation;
  struct timespec ahora;

  my_period.tv_sec = * (int*)arg;
  my_period.tv_nsec = 0; // You could also specify a number of nanoseconds
  
  if (clock_gettime(CLOCK_MONOTONIC, &next_activation))
    printf ("Error in clock_gettime\n");

  // Do "useful" work and wait until next period
  while (1) {
    incr_timespec (&next_activation, &my_period);
    if (clock_gettime(CLOCK_MONOTONIC, &ahora))
      printf ("Error in clock_gettime\n");
    printf ("\n-----------------------------------------------------\n");
    printf ("[Time:%2.1fs] ",ahora.tv_sec+ahora.tv_nsec/1000000000.0);
    printf ("Thread 1 with period %ds %d ns, activation %d\n",
	    my_period.tv_sec, my_period.tv_nsec, activation_count++);

    if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
			&next_activation, NULL))
      printf("Error in clock_nanosleep");
  }
}/*end periodic1()*/

/* Periodic thread */
void * periodic2 (void *arg)
{
  int activation_count = 0;
  struct timespec my_period, next_activation;
  struct timespec ahora;

  my_period.tv_sec = * (int*)arg;
  my_period.tv_nsec = 0; // You could also specify a number of nanoseconds
  
  if (clock_gettime(CLOCK_MONOTONIC, &next_activation))
    printf ("Error in clock_gettime\n");

  // Do "useful" work and wait until next period
  while (1) {
    incr_timespec (&next_activation, &my_period);
    if (clock_gettime(CLOCK_MONOTONIC, &ahora))
      printf ("Error in clock_gettime\n");
    printf ("\n-----------------------------------------------------\n");
    printf ("[Time:%2.1fs] ",ahora.tv_sec+ahora.tv_nsec/1000000000.0);
    printf ("Thread 2 with period %ds %d ns, activation %d\n",
	    my_period.tv_sec, my_period.tv_nsec, activation_count++);

    if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
			&next_activation, NULL))
      printf("Error in clock_nanosleep");
  }
}/*end periodic2()*/

// Main thread. It creates a periodic thread
int main ()
{
  pthread_t th1,th2;
  pthread_attr_t attr1,attr2;
  /*int *periods;*/
  int period1,period2;
  int th_number, i;
  struct timespec next_activ_dummy,my_lag;

  my_lag.tv_sec = 0;
  my_lag.tv_nsec = 500000000;  /* 0.5 second */

  period1 = 1;
  // Create thread with default scheduling parameters (SCHED_FIFO)
  CHK( pthread_attr_init (&attr1) );
  CHK( pthread_create (&th1, &attr1, periodic1, &period1) );

  if (clock_gettime(CLOCK_MONOTONIC, &next_activ_dummy))
    printf ("Error in clock_gettime\n");
  incr_timespec (&next_activ_dummy, &my_lag);
  if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
      		&next_activ_dummy, NULL))
    printf("Error in clock_nanosleep");
  period2 = 3; /* 3 seconds */
  // Create thread with default scheduling parameters (SCHED_FIFO)
  CHK( pthread_attr_init (&attr2) );
  CHK( pthread_create (&th2, &attr2, periodic2, &period2) );

  // Allows thread to execute for a while
  sleep (32);
  printf ("Main thread finishing after 32 seconds\n"); 
  return 0;
}/*end main()*/

