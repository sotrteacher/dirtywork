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

int console, s0, s1, cdrom;

void login()
{
  exec("login /dev/tty0");
}

void logins0()
{
  exec("login /dev/ttyS0");
}

void logins1()
{
  exec("login /dev/ttyS1");
}

void another(int pid)
{
   printf("-----------------------------------------------------\n");
   printf("KCINIT: Oh my God! My beloved login child %d has died\n", pid);
   printf("KCINIT: Oh well, I'll just fork another one!\n");
   printf("-----------------------------------------------------\n");
}
        
void parent()
{
  int pid, status;
  //printf("KCINIT : waiting .....\n");
  while(1){
    pid = wait(&status);
    if (pid==console){
       another(pid);
       console = fork();
       if (console)
	 continue;
       else
         login();
    }
    else if (pid==s0){
       another(pid);
       s0 = fork();
       if (s0)
	 continue;
       else
         logins0();
    }
    else if (pid==s1){
       another(pid);
       s1 = fork();
       if (s1)
	 continue;
       else
         logins1();
    }
    printf("KCINIT : I just buried an orphan child task %d\n", pid);
  }
}

void cdserver()
{
  exec("cdserver");
  exit(1);
}

int main(int argc, char *argv[ ])
{
  int in, out;

  in  = open("/dev/tty0", O_RDONLY);
  out = open("/dev/tty0", O_WRONLY);
  printf("KCINIT: fork CDSERVER\n");

  cdrom = fork();
  if (cdrom){
    printf("KCINIT: fork login on serial port 0\n");
    s0 = fork();
    if (s0){
       printf("KCINIT: fork login on serial port 1\n");
       s1 = fork();
       if (s1){
         printf("KCINIT: fork login on console\n");
	 console = fork();
         if (console)
             parent();
         else
	     login();
      }
       else
         logins1();
    }
    else
      logins0();
  }
  else
    cdserver();
}


