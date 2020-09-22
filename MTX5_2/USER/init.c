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
#include "../type.h"
#include "ucode.c"

main(int argc, char *argv[])
{
  int child, pid, status;
  
  while(1){
    printf("INIT %d forks a sh process\n", getpid());

     child = fork();
     if (child){
       printf("INIT %d waits\n", getpid()); 
           pid = wait(&status);
           printf("INIT %d found a dead child %d\n", getpid(), pid); 
           if (pid==child){
               continue;
           }
           printf("INIT : I just buried an orphan process %d\n", pid);
     }
     else{
          printf("proc %d exec to sh\n", getpid());
          exec("sh run sh program");
     }
  }
}

              
