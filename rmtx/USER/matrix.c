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
#include "ucode.c"

#define N 4
int thid[N] = {0,1,2,3};
int thstack[N][512];
int tpid[N];

int color;

int matrix[N][N]={ {1,2,3,4}, {5,6,7,8},{9,10,11,12},{13,14,15,16}}; 
int sum[N];

int thbody(int *ptr)
{
  char c; 
  int pid, i, j, rsum;;
  i = *ptr;
  pid = getpid();
  printf("-------------------------------\n");
  printf("this is thread %d: ", pid);
  printf("my segment=%x my parent=%d ", getcs(), getppid());
  printf("parameter=%d\n", *ptr);
  printf("thread %d : my job is to compute sum of row[%d]\n", pid, i);
        rsum = 0;
        for (j=0; j<4; j++)
	  rsum += matrix[i][j];
        printf("thread %d : row[%d] sum = %d\n", pid, i, rsum);
        sum[i] = rsum;
        //printf("thread %d exits : enter a key : \n", pid);
        //getc();      
        texit(pid);
}


int main(int argc, char *argv[ ])
{
  int pid, i, total, nthreads, status, mypid;
  int *ptr, *stack;
  /*****************
  printf("argc=%d ", argc); 
  for (i=0; i<argc; i++)
    printf("%s  ", argv[i]);
  printf("\n");
  *****************/
  thinit();

  nthreads = 4;
  if (argc > 1)
    nthreads = argv[1][0] - '0';
  color = 0x000C;
  printf("Number of threads = %d\n", nthreads); 

    mypid = getpid();
  
    for (i=0; i<nthreads; i++){
       stack = (int *)thstack[i+1];
       printf("proc %d creates a thread using stack %x\n", mypid, stack);
       ptr = &thid[i]; 
       tpid[i] = thread(thbody, stack, 0, ptr);

      printf("proc %d created a thread %d\n", getpid(), tpid[i]);
    }

    printf("main thread %d waits in thread_join()\n", getpid());
    //getc();
    tjoin(nthreads);

    printf("main thread compute total : ");
    total = 0;
    for (i=0; i<nthreads; i++)
        total += sum[i];
    printf("total = %d\n", total);
    printf("proc %d exit\n", mypid);
    exit(0);

}



