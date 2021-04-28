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

#define N 4
int A[N][N];
int total = 0;        // global total
pthread_mutex_t *m;   // mutex pointer
void *func(void *arg) // working thread function
{
#ifndef NDEBUG
  int i,sum=0;
  long row;
#else
  int i, row, sum = 0;
#endif /*NDEBUG*/
  pthread_t tid = pthread_self(); // get thread ID number
#ifndef NDEBUG
  row=(long)arg;
#else
  row = (int)arg;                 // get row number from arg
#endif /*NDEBUG*/
  printf("Thread %d [%lu] computes sum of row %d\n", row, tid, row);
  for (i=0; i<N; i++)             // compute partial sum of A[row]in
    sum += A[row][i];
  printf("Thread %d [%lu] update total with %d : ", row, tid, sum);
  pthread_mutex_lock(m);
  total += sum;                   // update global total inside a CR
  pthread_mutex_unlock(m);
  printf("total = %d\n", total);
}

int main (int argc, char *argv[])
{
  pthread_t thread[N];
#ifndef NDEBUG
  long i;
  int j, r;
#else
  int i, j, r;
#endif /*NDEBUG*/
  int sum[N];
  void *status;
  printf("Main: initialize A matrix\n");
  for (i=0; i<N; i++){
    sum[i] = 0;
    for (j=0; j<N; j++){
      A[i][j] = i*N + j + 1;
      printf("%4d ", A[i][j]);
    }
    printf("\n");
  }
  // create a mutex m
  m = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(m, NULL); // initialize mutex m
  printf("Main: create %d threads\n", N);
  for(i=0; i<N; i++) {
    pthread_create(&thread[i], NULL, func, (void *)i);
  }
  printf("Main: try to join with threads\n");
  for(i=0; i<N; i++) {
    pthread_join(thread[i], &status);
#ifndef NDEBUG
    printf("Main: joined with %d [%lu]: status=%lu\n",
           i, thread[i], (long)status);
#else
    printf("Main: joined with %d [%lu]: status=%d\n",
           i, thread[i], (int)status);
#endif /*NDEBUG*/
  }
  printf("Main: total = %d\n", total);
  pthread_mutex_destroy(m); // destroy mutex m
  pthread_exit(NULL);
}/*end main()*/





