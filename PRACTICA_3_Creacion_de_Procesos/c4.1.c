/**** C4.1.c file: compute matrix sum by threads ***/
/**
 * REF: Systems Programming in Unix/Linux, K.C. Wang, editorial 
 * Springer Nature, 2018. Chapter 4 Concurrent Programming, page 147.
 * ISBN: 978-3-319-92428-1 ISBN: 978-3-319-92429-8 (eBook).
 *
 *   gcc c4.1.c -pthread
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
//#define NDEBUG
#include <assert.h>
#define N 4
int A[N][N], sum[N];

void *func(void *arg) // threads function
{
#ifndef NDEBUG
  int j;
  long row;
#else
  int j, row;
#endif /*NDEBUG*/
  pthread_t tid = pthread_self();// get thread ID number
#ifndef NDEBUG
  row=(long)arg;
#else
  row = (int)arg;                // get row number from arg
#endif /*NDEBUG*/
  printf("Thread %d [%lu] computes sum of row %d\n", row, tid, row);
  for (j=0; j<N; j++)            // compute sum of A[row]in global sum[row]
    sum[row] += A[row][j];
  printf("Thread %d [%lu] done: sum[%d] = %d\n",
          row, tid, row, sum[row]);
  pthread_exit((void*)0);        // thread exit: 0=normal termination
}/*end func()*/

int main (int argc, char *argv[])
{
  pthread_t thread[N]; // thread IDs
#ifndef NDEBUG
  long i;
  int j, r, total = 0;
#else
  int i, j, r, total = 0;
#endif /*NDEBUG*/
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
  printf("Main: compute and print total sum: ");
  for (i=0; i<N; i++)
    total += sum[i];
  printf("total = %d\n", total);
  pthread_exit(NULL);
}/*end main()*/




