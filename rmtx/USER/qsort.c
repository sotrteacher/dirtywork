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
/*****************************************************
           KCW's quicksort by threads
threads may die and be created again. MUST keep track of the
ustack used 
*****************************************************/
#define NULL 0
#include "ucode.c"

typedef struct
{
	int  upperbound;
	int  lowerbound;
}PARM;          

int a[10] = {5,8,9,1,6,4,7,2,0,3};
int arraySize = 10;

#define TMAX 12

int sid[TMAX] = {1,2,3,4,5,6,7,8};
int sstack[TMAX][256];
/*******************
int tjoin(n) int n;
{ 
  int i, pid, status;
  for (i=0; i<n; i++){
    pid = wait(&status);
    printf("thread %d died status=%x\n", pid, status);
  }
}
*****************/
int qs(aptr) int *aptr;
{
     PARM *ap, aleft, aright;
     int pivot, pivotIndex, left, right, tmp;
     int upperbound, lowerbound;
     int leftThread, rightThread;

     ap = (PARM *)aptr;
     upperbound = ap->upperbound;
     lowerbound = ap->lowerbound;
     //printf("%d in qs\n", getpid()); 
     printf("lowerbound=%d   upperbound=%d\n", lowerbound, upperbound);
     //     getc();
     pivot = a[upperbound];         // pick low pivot value
     left = lowerbound - 1;         // scan index from left side
     right = upperbound;            // scan index from right side

     //printf("pivot=%d bound=[%d %d]\n", pivot, left,right); getc();

     if (lowerbound >= upperbound){
       printf("%d texit\n", getpid());
       texit(getpid());
     }
     while (left < right) {             // partition loop
          do { left++;} while (a[left] < pivot);
          do { right--;} while (a[right] > pivot);

          if (left < right ) {          
               tmp = a[left];
               a[left] = a[right];
               a[right] = tmp;
          }
     }
     printf("pivoted : "); print();
     //getc();
     pivotIndex = left;                 // put the  pivot back
     tmp = a[pivotIndex];
     a[pivotIndex] = pivot;
     a[upperbound] = tmp;
 
     // start the "recursive threads"
     aleft.upperbound = pivotIndex - 1;
     aleft.lowerbound = lowerbound;
     aright.upperbound = upperbound;
     aright.lowerbound = pivotIndex + 1;
 
     printf("left thread : ");
     leftThread = thread(qs, sstack[TMAX], 1, &aleft);
     printf("right thread: ");
     rightThread = thread(qs, sstack[TMAX], 1, &aright); 
     printf("waiting for left & right threads to join\n");
     tjoin(2);
     
     print();//getc();
     printf("%d texit\n", getpid());
     texit(getpid());
}
//---------------------------------------------------------------//
//  main program 
//---------------------------------------------------------------//

int main(argc, argv) int argc; char *argv[ ];
{
     PARM arg;
     int  pid; 
     thinit();
     printf("unsorted array  = "); print(); 
     printf("enter a key to continue : "); getc();
     arg.upperbound = arraySize-1;
     arg.lowerbound = 0;

     printf("main creates thread\n");
     pid = thread(qs, sstack[TMAX], 1, &arg);
     printf("main : join with %d\n", pid);     
     tjoin(1);
     printf("main resumes : sorted array  = ");
     print();
}

int print()
{
  int i;
  printf("[ ");
  for (i=0; i<arraySize; i++)
    printf("%d ", a[i]);
  printf("]\n");
}

