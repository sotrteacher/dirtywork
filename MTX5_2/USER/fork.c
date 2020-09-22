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
/************************ ps.c *****************************/
#include "ucode.c"

main(int argc, char *argv[])
{
  int pid, status;

  pid = fork();
  if (pid){
    printf("parent %d waits for child %d\n", getpid(), pid);
    pid = wait(&status);
  }
  else{
    printf("child %d running;  ", getpid());
    printf("child %d exit(123)\n", getpid());
    exit(123);
  }

}
