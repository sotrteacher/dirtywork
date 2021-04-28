/** C4.5.c: Gauss Elimination with Partial Pivoting **/
/**
 * REF: Systems Programming in Unix/Linux, K.C. Wang, editorial 
 * Springer Nature, 2018. Chapter 4 Concurrent Programming, page 162.
 * ISBN: 978-3-319-92428-1 ISBN: 978-3-319-92429-8 (eBook).
 *
 *   gcc c4.5.c -pthread
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
//#define NDEBUG
#include <assert.h>

#define N	4
double A[N][N+1];
pthread_barrier_t barrier;

int print_matrix()
{
  int i, j;
  printf("------------------------------------\n");
  for(i=0; i<N; i++){
    for(j=0;j<N+1;j++)
      printf("%6.2f ", A[i][j]);
    printf("\n");
  }
}/*end print_matrix()*/

void *ge(void *arg) // threads function: Gauss elimination
{
  int i, j, prow;
  int k;
#ifndef NDEBUG
  long myid=(long)arg;
#else
  int myid = (int)arg;
#endif /*NDEBUG*/
  double temp, factor;
  for(i=0; i<N-1; i++){
    if (i == myid){
      printf("partial pivoting by thread %d on row %d: ", myid, i);
      temp = 0.0; prow = i;
      for (j=i; j<=N; j++){
        if (fabs(A[j][i]) > temp){
          temp = fabs(A[j][i]);
          prow = j;
        }
      }
      printf("pivot_row=%d pivot=%6.2f\n", prow, A[prow][i]);
      if (prow != i){ // swap rows
        for (j=i; j<N+1; j++){
          temp = A[i][j];
          A[i][j] = A[prow][j];
          A[prow][j] = temp;
        }
      }
    }/*if(i==myid)*/
    // wait for partial pivoting done
    pthread_barrier_wait(&barrier);
    for(j=i+1; j<N; j++){
      if (j == myid){
        printf("thread %d do row %d\n", myid, j);
        factor = A[j][i]/A[i][i];
        for (k=i+1; k<=N; k++)
          A[j][k] -= A[i][k]*factor;
        A[j][i] = 0.0;
      }
    }
    // wait for current row reductions to finish
    pthread_barrier_wait(&barrier);
    if (i == myid)
      print_matrix();
  }/*end for(i=0;)*/
}/*end ge()*/

int main(int argc, char *argv[])
{
#ifndef NDEBUG
  long i;
  int j;
#else
  int i, j;
#endif /*NDEBUG*/
  double sum;
  pthread_t threads[N];

  printf("main: initialize matrix A[N][N+1] as [A|B]\n");
  for (i=0; i<N; i++)
    for (j=0; j<N; j++)
      A[i][j] = 1.0;
  for (i=0; i<N; i++)
    A[i][N-i-1] = 1.0*N;
  for (i=0; i<N; i++){
    A[i][N] = 2.0*N - 1;
  }
  print_matrix(); // show initial matrix [A|B]

  pthread_barrier_init(&barrier, NULL, N); // set up barrier

  printf("main: create N=%d working threads\n", N);
  for (i=0; i<N; i++){
    pthread_create(&threads[i], NULL, ge, (void *)i);
  }
  printf("main: wait for all %d working threads to join\n", N);
  for (i=0; i<N; i++){
    pthread_join(threads[i], NULL);
  }
  printf("main: back substitution : ");
  for (i=N-1; i>=0; i--){
    sum = 0.0;
    for (j=i+1; j<N; j++)
      sum += A[i][j]*A[j][N];
    A[i][N] = (A[i][N]- sum)/A[i][i];
  }
  // print solution
  printf("The solution is :\n");
  for(i=0; i<N; i++){
    printf("%6.2f ", A[i][N]);
  }
  printf("\n");
}/*end main()*/



