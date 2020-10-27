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

char *pStatus[] = {"FREE ","READY","SLEEP","ZOMBIE","BLOCK",0};

PS ups[NPROC+NTHREAD], *p;

main(int argc, char *argv[ ])
{
  int i;
  ps(ups);  // to kernel, passing & of ps[ ];

  printf("%s\n", "Proc  ppid  uid   status   file");
  printf("%s\n", "----  ----  ---   ------  ------");


  //  for (i=0; i<NPROC+NTHREAD; i++){
  for (i=0; i<NPROC; i++){
      p = &ups[i];

      if (p->pid <10)
         printf(" "); 
      printf("%d     ", p->pid);

      if (p->status)
	printf("%d   %d    ", p->ppid, p->uid);
      else
          printf("-    -    ");

      if (p->status==0)
	 printf(" ");
      printf("%s  %s\n", pStatus[p->status], p->pgm);

      /**********
      if (i==NPROC-1){
	printf("enter a key "); getc();
        printf("\n");
      }
      ***********/
  }
}


            
