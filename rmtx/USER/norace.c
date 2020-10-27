/********************************************************************
Copyright 2010-2015 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
/*********************************************************************
           demonstrate threads sync by mutex.
  each thread :  read current total;
                 update it
      =======> tswitch()
                 write total back
  would create race conditions ==> total will be over-written ==> incorrect
  With mutex:

With mutex:  each thread : 
                 mutex_lock(mutex);
                     read current total;
                     update it
            =======> tswitch()
                     write total back
                 mutex_unlock(mutex);

 execute the update of total in a CR ==> result is correct despite tswitch()

MTX kernel: threads.c implements mutex_creat(), which returns an int mutex
            mutex is initially unlocked and has no owner.
The first thread that does  mutx_lock(mutex) locks mutex and is the OWNER
of the mutex; only owner can unlock a locked mutex.
While mutex is locked, another threads trying lock_mutex() would BLOCK in
mutex.queue;
            mutex_unlock(): by owner, allows one waiter to become owner
with mutex still locked.  mutex becomes unlocked and no owner when the
owner unlocks it.

Actually, a counting semaphore works better than mutex.
*************************************************************************/
#include "ucode.c"

int matrix[4][5]={ {1,2,3,4,0}, {5,6,7,8,0},{9,10,11,12,0},{13,14,15,16,0}}; 
int total = 0;

int mutex;

int rbody(int *ptr)
{
  char c; 
  int pid, row, j, sum, mytotal;
  row = *ptr - 1;
  pid = getpid();
  
  printf("-------------------------------\n");
  printf("this is thread %d: ", pid);
  printf("my segment=%x my parent=%d ", getcs(), getppid());
  printf("parameter=%d\n", *ptr);
  printf("thread %d : my job is to compute sum of row[%d]\n", pid, row);
        sum = 0;
        for (j=0; j<4; j++)
	  sum += matrix[row][j];
        printf("thread %d : row[%d] sum = %d\n", pid, row, sum);

	mutex_lock(mutex);

          mytotal = total;
          mytotal += sum;
	   tswitch();       
	  total = mytotal;
	mutex_unlock(mutex);

        //printf("thread %d exits : enter a key : \n", pid);
        //getc();      
        texit(pid);
}

#define TMAX 8

int thid[TMAX] = {1,2,3,4};
int thstack[TMAX][1024];

int main(int argc, char *argv[ ])
{
  int pid, i, nthreads, status, mypid;
  int *ptr, *stack;
  /**************
  printf("argc=%d  argv = ", argc);
  for (i=0; i<argc; i++)
    printf("%s   ", argv[i]);
  printf("\n");
  ****************/
  thinit();

  nthreads = 4;
  if (argc > 1)
    nthreads = argv[1][0] - '0';
  total = 0;
  printf("Number of threads = %d\n", nthreads);
  // create mutex for threads to share
  mutex = mutex_creat();

    mypid = getpid();
  
    for (i=0; i<nthreads; i++){
       stack = (int *)thstack[i+1];
       printf("proc %d creates a thread using stack %x\n", mypid, stack);
       ptr = &thid[i]; 
       pid = thread(rbody, stack, 0, ptr);

      printf("proc %d created a thread %d\n", getpid(), pid);
    }

    printf("main thread %d waits in thread_join()\n", getpid());
    for (i=0; i<nthreads; i++){
        pid = wait(&status);
        printf("thread %d died status=%x\n", pid, status);
    }
    printf("main thread compute total : ");

    printf("total = %d\n", total);
    printf("proc %d exit\n", mypid);
    //getc();
    exit(0);
}
