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
// ucode.c file

char *cmd[]={"getpid", "ps", "chname", "kmode", "switch", "wait", "exit", 
             "fork", "exec", "itimer", 0};

int show_menu()
{
   printf("********************** Menu *************************\n");
   printf("* ps chname kmode switch wait exit fork exec itimer *\n");
   /*         1   2      3      4    5     6   7    8     9     10    11    */
   printf("*****************************************************\n");
}

int find_cmd(char *name)
{
   int i;   char *p;
   i = 0;   p = cmd[0];
   while (p){
         if (strcmp(p, name)==0)
            return i;
         i++;  p = cmd[i];
   } 
   return(-1);
}


int getpid()
{
   return syscall(0,0,0);
}

int ps()
{
   syscall(1, 0, 0);
}

int chname()
{
    char s[64];
    printf("input new name : ");
    gets(s);
    syscall(2, s, 0);
}

int kmode()
{
    printf("kmode : enter Kmode via INT 80\n");
    printf("proc %d going K mode ....\n", getpid());
        syscall(3, 0, 0);
    printf("proc %d back from Kernel\n", getpid());
}    

int kswitch()
{
    printf("proc %d enter Kernel to switch proc\n", getpid());
        syscall(4,0,0);
    printf("proc %d back from Kernel\n", getpid());
}

int wait(int *status)
{
  return syscall(5, status);
}


int do_wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n");
    return child; 
} 

int exit(int value)
{
  syscall(6,0,0);
}

int do_exit()
{
   char exitValue;
   printf("enter an exitValue (0-9) : ");
   exitValue=(getc()&0x7F) - '0';
   printf("enter kernel to die with exitValue=%d\n",exitValue);
   _kexit(exitValue);
}

int _kexit(int exitValue)
{
  syscall(6,exitValue,0);
}

int fork()
{
  return syscall(7,0,0,0);
}

int do_fork()
{
  int pid;
  pid = fork();
  if (pid)
    printf("parent return child %d\n", pid);
  else
    printf("child return 0\n");
}


int exec(char *filename)
{
  return syscall(8, filename, 0);
}
   
int do_exec()
{
  int r;
  char filename[32];
  printf("enter exec filename : ");
  gets(filename);
  r = exec(filename);
  printf("exec failed\n");
}

int do_itimer()
{
  char s[16];
  int t;
  printf("input a timer value : ");
  gets(s);
  t = atoi(s);
  return syscall(10, t);
}

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}
