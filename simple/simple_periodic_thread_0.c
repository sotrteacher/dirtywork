/** simple_periodic_thread_0.c
 * Create periodic threads using 'clock_nanosleep()'.
 */
#include <time.h>   /*clock_nanosleep()*/
#include <pthread.h>
#include <misc/timespec_operations.h>
#include <misc/error_checks.h>        /* CHK() */ 

/* Periodic thread */
void * periodic (void *arg)
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
    printf ("[Time:%2.1fs] ",ahora.tv_sec+ahora.tv_nsec/1000000000.0);
    printf ("Thread with period %ds activation %d\n",
	    my_period.tv_sec, activation_count++);

    if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
			&next_activation, NULL))
      printf("Error in clock_nanosleep");
  }
}/*end periodic()*/

// Main thread. It creates two periodic threads
int main ()
{
  pthread_t th;
  pthread_attr_t attr;
  /*int *periods;*/
  int period1,period2;
  struct timespec next_activ_dummy,my_lag;

  my_lag.tv_sec = 0;
  my_lag.tv_nsec = 500000000;  /* 0.5 second */

  period1 = 1; /* 1 second */
  // Create thread with default scheduling parameters (SCHED_FIFO)
  CHK( pthread_attr_init (&attr) );
  CHK( pthread_create (&th, &attr, periodic, &period1) );

  if (clock_gettime(CLOCK_MONOTONIC, &next_activ_dummy))
    printf ("Error in clock_gettime\n");
  incr_timespec (&next_activ_dummy, &my_lag);
  if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME,
      		&next_activ_dummy, NULL))
    printf("Error in clock_nanosleep");
  period2 = 2; /* 2 seconds */
  // Create thread with default scheduling parameters (SCHED_FIFO)
  CHK( pthread_attr_init (&attr) );
  CHK( pthread_create (&th, &attr, periodic, &period2) );

  // Allows threads to execute for a while
  sleep (22);
  printf ("Main thread finishing after 22 seconds\n"); 
  return 0;
}/*end main()*/



