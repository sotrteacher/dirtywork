/** C4.3.c: matrix sum by threads with mutex lock **/
/**
 * REF: Systems Programming in Unix/Linux, K.C. Wang, editorial 
 * Springer Nature, 2018. Chapter 4 Concurrent Programming, page 152.
 * ISBN: 978-3-319-92428-1 ISBN: 978-3-319-92429-8 (eBook).
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
//#define NDEBUG
#include <assert.h>

#define N 2
//int A[N][N];
//int total = 0;        // global total
pthread_mutex_t *m0;   // mutex pointer
pthread_mutex_t *m1;   // mutex pointer
void *func(void *arg) // working thread function
{
  int i;
  long row;
  pthread_t tid = pthread_self(); // get thread ID number
  row=(long)arg;
switch(row)
{
 case 0:
 {
  printf("Thread %d [%lu] trata de adquirir el candado %d\n", row, tid, row);
  pthread_mutex_lock(m0);
  printf("Thread %d [%lu] trata de adquirir el candado %d\n", row, tid, row+1);
  pthread_mutex_lock(m1);
  printf("HEMOS LLEGADO AQUI %d\n",row);
  pthread_mutex_unlock(m1);
  pthread_mutex_unlock(m0);
  break;
 }
 case 1:
 {
  printf("Thread %d [%lu] trata de adquirir el candado %d\n", row, tid, row);
  pthread_mutex_lock(m1);
  printf("Thread %d [%lu] trata de adquirir el candado %d\n", row, tid, row-1);
  pthread_mutex_lock(m0);
  printf("HEMOS LLEGADO AQUI %d\n",row);
  pthread_mutex_unlock(m0);
  pthread_mutex_unlock(m1);
  break;
 }
 default:
  break;
}
  printf("Thread %d finalizando\n", row);
}/*end func()*/

int main (int argc, char *argv[])
{
  pthread_t thread[N];
  long i;
  int j, r;
  void *status;
  // create mutexes m0 and m1
  m0 = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  m1 = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(m0, NULL); // initialize mutex m0
  pthread_mutex_init(m1, NULL); // initialize mutex m1
  printf("Main: create %d threads\n", N);
  for(i=0; i<N; i++) {
    pthread_create(&thread[i], NULL, func, (void *)i);
  }
  printf("Main: try to join with threads\n");
  for(i=0; i<N; i++) {
    pthread_join(thread[i], &status);
    printf("Main: joined with %d [%lu]: status=%lu\n",
           i, thread[i], (long)status);
  }
  printf("Main: NO HUBO DEADLOCK!!\n");
  pthread_mutex_destroy(m0); // destroy mutex m0
  pthread_mutex_destroy(m1); // destroy mutex m1
  pthread_exit(NULL);
}/*end main()*/





