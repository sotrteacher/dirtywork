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

main(argc, argv) int argc; char *argv[];
{
    prints("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    printf("        This is KC's man pages in action\n");
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

   printf("* ===================    USAGE:  ========================== *\n");
   printf("* MTX can boot and run from any x86-based PC or simulated   *\n");
   printf("* (real-mode) PC environment, which include the following:  *\n");
   printf("* 1. PC with (optional) 2 serial terminals and a printer    *\n");
   printf("* 2. DOSEMU : xdosemu -A with /dev/pts/0-1 as serial terms  *\n");
   printf("* 3. BOCHS  : bochs   -q with /dev/pts/0-1 as serial terms  *\n");
   printf("* Supports multiple user logins with (0.3 sec) time-sharing *\n");
   printf("* On the PC console, text are in color by uid (0G 1Cy 2Y)   *\n");
   printf("* ===================  COMMANDS: ========================== *\n");
   printf("* login/logout, ls, cd, pwd, ps, whoami, su, lpr, Crtl-C :  *\n");
   printf("*                   same as in Unix.                        *\n");
   printf("* mkdir (rmdir rm cat more) filename     e.g. mkdir /a/b/c  *\n");
   printf("* chmod 0644 filename;      chown uid filename;             *\n");
   printf("* cp (mv) src dest                                          *\n");
   printf("* grep pattern filename         e.g.  grep printf /a/b/c    *\n");
   printf("* kill -sig# pid                (sig#=9 if kill pid)        *\n");
   printf("* 10 command history : (by up|down arrow keys)              *\n");
   printf("* file protection: owner permission only                    *\n");
   printf("* I/O redirection and pipe : <, >, >>, | : as in Unix       *\n");
   printf("* ===================  SYSCALLS: ========================== *\n");
   printf("* fork, exec, wait, open, close, lseek, read, write, creat, *\n");
   printf("* pipe, dup, dup2, signal, kill, catcher : same as in Unix  *\n");
   printf("* EXCEPT only have signals 1-9 with SIGALRM=4, SIGTERM=5    *\n");
   printf("* ===================  MISC TESTS: ======================== *\n");
   printf("* sleep, timer, ttimer (in seconds) : test timer requests.  *\n");
   printf("* NOTE : ttimer can be tested on REAL PC but not on dosemu  *\n");
   printf("* divide, trap : test umode exceptions and signal handling  *\n");
   printf("* map: display inode bitmap (test RW special file /dev/fd0  *\n");
   printf("*************************************************************\n");
   exit(0);  
}

             
