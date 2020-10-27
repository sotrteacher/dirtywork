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

main(int argc, char *argv[])
{
  int pid, status;
  char msg[256];

    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    printf("        This is KC's %s in action\n", argv[0]);
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    pid = fork();
    if (pid){
      printf("parent %d waits for a sessage from %d\n", getpid(), pid);
      recv(msg);
      printf("%d received msg=%s from %d\n", getpid(), msg, pid);

      printf("parent %d sending message to %d\n", getpid(), pid);
      printf("enter msg : "); gets(msg); printf("\n");
      printf("%d sending msg=%s to %d\n", getpid(), msg, pid);
      send(msg, pid);
      wait(&status);
    }
    else{
      printf("child %d sending message to %d\n", getpid(), getppid());
      printf("enter msg : "); gets(msg);
      printf("\n");
      printf("sending msg=%s to %d\n", msg, getppid());
      send(msg, getppid());

      printf("child %d receiving message from %d\n", getpid(), getppid());
      recv(msg);
      printf("%d received msg=%s from %d\n", getpid(), msg, getppid());
    }
}

             
