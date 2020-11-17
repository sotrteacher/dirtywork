/* producer_consumer_semaphores.c: producer-consumer by threads 
   with semaphores */
/**
 * REF: Systems Programming in Unix/Linux, K.C. Wang, editorial 
 * Springer Nature, 2018. Chapter 4 Concurrent Programming.
 * ISBN: 978-3-319-92428-1 ISBN: 978-3-319-92429-8 (eBook) and 
 * Sistemas Operativos Diseño e Implementación, Andrew S. Tanenbaum, 
 * Albert S. Whoodhull, editorial Prentice Hall, 1998. Chapter 2 
 * Procesos, page 66.
 *   gcc prod_cons_with_semaphores.c -pthread
 * O bien, gcc -lpthread -lrt prod_cons_with_semaphores.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
//#define NDEBUG
#include <assert.h>

#define NBUF	5
#define N	10

// shared global variables
int buf[NBUF];                    // circular buffers
int head, tail;                   // indices
int data;                         // number of full buffers
sem_t sem_mutex,sem_empty,sem_full; // semaphores


int init()
{
  head = tail = data = 0;
  sem_init(&sem_mutex,0,1);
  sem_init(&sem_empty,0,NBUF);
  sem_init(&sem_full,0,0);
}

void *producer()
{
  int i;
  pthread_t me = pthread_self();
  for (i=0; i<N; i++){              // try to put N items into buf[ ]
    if (data == NBUF){
      printf ("producer %lu: all bufs FULL: wait\n", me);
    }
    sem_wait(&sem_empty);
    sem_wait(&sem_mutex);
    buf[head++] = i+1;              // item = 1,2,..,N
    head %= NBUF;                   // circular bufs
    data++;                         // inc data by 1
    printf("producer %lu: data=%d value=%d\n", me, data, i+1);
    sem_post(&sem_mutex);
    sem_post(&sem_full);
  }
  printf("producer %lu: exit\n", me);
}/*end producer()*/

void *consumer()
{
  int i, c;
  pthread_t me = pthread_self();
  for (i=0; i<N; i++) {
    if (data == 0) {
      printf ("consumer %lu: all bufs EMPTY: wait\n", me);
    }
    sem_wait(&sem_full);
    sem_wait(&sem_mutex);
    c = buf[tail++];                       // get an item
    tail %= NBUF;
    data--;                                // dec data by 1
    printf("consumer %lu: value=%d\n", me, c);
    sem_post(&sem_mutex);
    sem_post(&sem_empty);
  }
  printf("consumer %lu: exit\n", me);
}/*end consumer()*/

int main ()
{
  pthread_t pro, con;
  init();
  printf("main: create producer and consumer threads\n");
  pthread_create(&pro, NULL, producer, NULL);
  pthread_create(&con, NULL, consumer, NULL);
  printf("main: join with threads\n");
  pthread_join(pro, NULL);
  pthread_join(con, NULL);
  sem_destroy(&sem_mutex);
  sem_destroy(&sem_empty);
  sem_destroy(&sem_full);
  printf("main: exit\n");
}/*end main()*/





