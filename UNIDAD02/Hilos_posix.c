#include <pthread.h>

int phread_create (pthread_t *thread, const pthread_attr_t *attr,
                  void *(*start_routine)(void*), void *arg);
pthread_t pthread_self (void);
int pthread_detach (pthread_t thread);
void pthread_exit (void *value_ptr);
int pthread_join (pthread_t thread, void **value_ptr);
int pthread_equal (pthread_t t1, pthread_t t2);


$ gcc hilos.c -lpthread -o test_hilos 

#include <string.h>
char *strerror (int errnum);

  hilos.c:38: ERROR: pthread_join - Invalid argument

  hilos.c:38: ERROR: pthread_join - No such process
   _exit(-1) called; rebooting...

#include <pthread.h>
int pthread_attr_init (pthread_attr_t *attr);
int pthread_attr_destroy (pthread_attr_t *attr);

#include <pthread.h>
int pthread_attr_getdetachstate (pthread_attr_t *attr, int *detachstate);
int pthread_attr_setdetachstate (pthread_attr_t *attr, int *detachstate);







