/* C4.4.c: producer-consumer by threads with condition variables */
/**
 * REF: Systems Programming in Unix/Linux, K.C. Wang, editorial 
 * Springer Nature, 2018. Chapter 4 Concurrent Programming, page 158.
 * ISBN: 978-3-319-92428-1 ISBN: 978-3-319-92429-8 (eBook).
 *
 *   gcc c4.4.c -pthread
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NBUF	5
#define N	10

// shared global variables
int buf[NBUF];                    // circular buffers
int head, tail;                   // indices
int data;                         // number of full buffers
pthread_mutex_t mutex;            // mutex lock
pthread_cond_t emptyBuf, fullBuf; // condition variables

int init()
{
  head = tail = data = 0;
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&fullBuf, NULL);
  pthread_cond_init(&emptyBuf, NULL);
}

void *producer()
{
  int i;
  pthread_t me = pthread_self();
  for (i=0; i<N; i++){              // try to put N items into buf[ ]
    pthread_mutex_lock(&mutex);     // lock mutex
    if (data == NBUF){
      printf ("producer %lu: all bufs FULL: wait\n", me);
      pthread_cond_wait(&emptyBuf, &mutex); // wait
    }
    buf[head++] = i+1;              // item = 1,2,..,N
    head %= NBUF;                   // circular bufs
    data++;                         // inc data by 1
    printf("producer %lu: data=%d value=%d\n", me, data, i+1);
    pthread_mutex_unlock(&mutex);   // unlock mutex
    pthread_cond_signal(&fullBuf);  // unblock a consumer, if any
  }
  printf("producer %lu: exit\n", me);
}/*end producer()*/

void *consumer()
{
  int i, c;
  pthread_t me = pthread_self();
  for (i=0; i<N; i++) {
    pthread_mutex_lock(&mutex);            // lock mutex
    if (data == 0) {
      printf ("consumer %lu: all bufs EMPTY: wait\n", me);
      pthread_cond_wait(&fullBuf, &mutex); // wait
    }
    c = buf[tail++];                       // get an item
    tail %= NBUF;
    data--;                                // dec data by 1
    printf("consumer %lu: value=%d\n", me, c);
    pthread_mutex_unlock(&mutex);          // unlock mutex
    pthread_cond_signal(&emptyBuf);        // unblock a producer, if any
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
  printf("main: exit\n");
}/*end main()*/





