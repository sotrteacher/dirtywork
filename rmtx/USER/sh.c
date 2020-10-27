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

extern char *name[16], components[64];
extern int nk;

int me; 
char cline[64];


char buf[1024];
int loguid, myuid, uid;
int fd, n,i;
char *cp, *cq, *cpp,*cqq, pline[64], *pname[8];

u16 boot[512];
char any[64];
int nowait;

int menu()
{
   printf("#################################################\n");
   printf("# ls     cd     pwd    cat   cp      mv    ps   #\n");
   printf("# mkdir  rmdir  creat  rm    chmod   more  grep #\n");
   printf("# lpr    (I/O and Pipe) :  >   >>    <     |    #\n");
   printf("#################################################\n");
}

int pscan(s) char *s;
{ 
  int count = 0;
  while(*s){
    if (*s == '|')
      count++;
    s++;
  }
  return count;
}

int su()
{
  int euid;
  euid = getuid(); // get uid
  if (euid == 0){
    printf("already root user\n");
    return 0;
  } 
  printf("enter root password : ");
  gets(buf);
  if (strcmp(buf, "12345")==0)
    setuid(0, 0);     // change gid, uid to 0
  else
    printf("denied\n");
}  

int exitsh()
{
  int euid;
  euid = getuid();

  if (euid == loguid)
    exit(0);

  if (euid != loguid)
    setuid(loguid);
}
int clr(char *buf)
{
  int i;
  for (i=0; i<1024; i++)
    buf[i] = ' ';
}
main(int argc, char *argv[])
{
  int  i, j, pid, status, pcount, cpcount;
  char *head, *next;
  char buf[64], tbuf[64], *cp, *cq;
  int index;  //must be unsigned for % to work on negative valuess 

  loguid = getuid();  // mgetuid() 
  //printi(loguid);
  me = getpid();

  signal(2, 1);  // ignore Control-C
  strcpy(cline, "/bin/");

  printf("enter ? for help menu\n");

  while(1){

    if (getuid()==0)
        printf("kcsh # : ");
    else
        printf("kcsh % : ");

  get:
    clr(buf);
     if(gets(buf)==0)
       exit(0);
     //printf("buf=%s\n", buf);
  ha:
     /* condition input string */
     //     printf("input=%s\n", buf);
     cp = buf;
     while(*cp == ' ')   // skip leading blanks 
       cp++;

     cq = cp;
     while (*cq)         // zero our trailing blanks
       cq++;
     cq--;
     while(*cq == ' '){
        *cq = 0;
        cq--;
     }

     //printf("input=%s\n", buf);

     if (strcmp(cp, "")==0)  // if nothing or a bunch of spaces 
       continue;             //    repeat the while loop

     if (*cp == 4){ // this is the Control-D key on SERIAL terminals ==>
       exit(0);    // if input buffer contains 04 and got by sh, it will die
     }


     strcpy(tbuf, cp);
     strcpy(buf, tbuf);
     //printf("input=%s\n", buf);

     strcpy(tbuf, buf);

     eatpat(tbuf, components, name, &nk);

     nowait = 0;
     if (nk > 1){
       if (strcmp(name[nk-1], "&")==0){
         // must delete & from buf[ ]
         cp = buf;
         while (*cp)
	   cp++;       // cp to end to buf
         while (*cp != ' ')
	   cp--;       // cp back to space
         *cp = 0;       // delete & from buf[ ] end

	 nk--;
         nowait = 1;
       }
     }
 
     if (strcmp(name[0], "cd")==0){
       if (name[1]==0)
         chdir("/"); 
       else
         chdir(name[1]);
       continue;
     }

     if (strcmp(name[0], "pwd")==0){
         pwd(); continue;
     }

     if (strcmp(name[0], "echo")==0){
       for (i=1; i<nk; i++){
	 printf("%s ", name[i]);
       }
       printf("\n");         
       continue;
     }

     if (strcmp(name[0], "?")==0 || strcmp(name[0], "help")==0){
        menu(); continue;
     }
     if (strcmp(name[0], "logout")==0){
       printf("##########################################\n");
       printf("Hey You\007!, I am your process! Don't leave me!\n");
       printf("Hamm .... The guy just walked out on me!\n");
       printf("Oh well, I'll commit suicide! So long!\n");
       printf("##########################################\n");
       chdir("/");       
       exit(0);
     }
     if (strcmp(name[0], "su")==0){
       su(); continue;
     }
     if (strcmp(name[0], "exit")==0){
       exitsh(); continue;
     }
     if (strcmp(name[0], "reboot")==0){
       if (getuid() != 0){
          printf("not root user\n");
          continue;
       }
       sync();
       continue;
     }

     // assume cmd ... & ; last token is &, then no wait 
     printf("Parent sh %d fork a child  ", getpid());

     pid = fork(); /* sh forks child */

     if (pid){ /* parent sh */
       if (nowait){
	  printf("parent sh %d no wait for child\n", getpid());
          nowait = 0;
          tswitch();
          continue;
       }
       else{   
           printf("parent sh %d waits\n", getpid());
           pid = wait(&status);
           continue;
       }
     }
     else{
        print2f("child  sh %d running\n", getpid());
        fisan(buf, 0);  /* child sh */
     }
  }
}

// scan breaks up buf = "head | tail" into  "head"  "tail" 

int scan(buf, tail) char *buf; char **tail;
{ 
  char *p, *q;

  p = buf; *tail = 0;

  while(*p)         // scan to buf end line
    p++;    

  while (p != buf && *p != '|') // scan backward until |
    p--;

  if (p == buf)     // did not see any |, so head=buf
    return 0;

  *p = 0;           // change | to NULL 
  p++;              // move p right by 1
  while(*p == ' ')  // skip over any blanks
    p++;

  *tail = p;        // change tail pointer to p

  return 1;       // head points at buf; return head
}

int fisan(buf, rpd) char *buf; int *rpd;
{ 
  int hasPipe, pid; 
  char *tail;
  int lpd[2];
  
  /*******************************************************************
   cmdline = a < infile | b |c | d > outfile 
   1. only FIRST and LAST can have < or > for I/O redirect
   2. divide cmdline into  head=a|b|c; tail=d;
   3. pipe(pd); 
      fork();
      parent: do tail; as pipe reader by close(rpd[1)}; close(0); dup(rpd[0]);
           then do > redirect;
           then exec(tail);

      child : fisan(buf, rpd)
              if (rpd):  as rpd writer by close(rpd[0]); close(1); dup(rpd[1]);
  **************************************************************************/
  if (rpd){
    // as writer on RIGHT side pipe
    close(rpd[0]);
    close(1);
    dup(rpd[1]);
    close(rpd[1]);
  }

  /****************
  print2f("proc %d : buf=%s ", getpid(), buf);
  if (rpd) print2f("rpd=[%d %d]  ", rpd[0], rpd[1]);
  print2f("\n");
  *****************/

  hasPipe = scan(buf, &tail);
  //print2f("after scan: buf=%s  tail=%s\n", buf, tail);

  if (hasPipe){   // buf=head; tail->tailString 
     if (pipe(lpd) < 0){ // create LEFT side pipe
         printf("proc %d pipe call failed\n", getpid());
         exit(1);
      }

      pid = fork();

      if (pid<0){
	printf("proc %d fork failed\n", getpid());
        exit(1);
      }

      if (pid){   // parent as reader on LEFT side pipe
          close(lpd[1]); 
          close(0); 
          dup(lpd[0]);
          close(lpd[0]);
          //print2f("proc %d exec %s\n", getpid(), tail);
          command(tail);
      }
      else{ // child gets LEFT pipe as its RIGHT pipe
	  fisan(buf, lpd);
      }
 }
 else{ // no pipe symbol in string must be the leftmost cmd
   command(buf);
 }
}

int command(s) char *s;
{
    char *name[16], components[64];
    int i,j, I; int flag;
    char cmdline[64];
    
    eatpat(s, components, name, &nk);
    I = nk;

    for (i=0; i<nk; i++){
        if (strcmp(name[i], "<")==0){
	  print2f("proc %d redirect input from %s\n", getpid(), name[i+1]);
	    if (I > i)  I = i;         // I = index of first < or > or >>
            if (name[i+1]==0){
                print2f("invalid < in command line\n\r");
                exit(1);
            }
            close(0);
            if(open(name[i+1], 0) < 0){
               print2f("no such input file\n");
               exit(2);
            }
            break;
	 }
    }
    for (i=0; i<nk; i++){
         if (strcmp(name[i], ">")==0){
	   print2f("proc %d redirect outout to %s\n", getpid(), name[i+1]);
	    if (I > i) I = i;          // I = index of first > or < or >>
            if (name[i+1]==0){
               print2f("invalid > in command line\n\r");
               exit(3);
            }
            close(1);
            open(name[i+1], O_WRONLY|O_CREAT);
            break;
         } 
    }
    for (i=0; i<nk; i++){
         if (strcmp(name[i], ">>")==0){
           print2f("proc %d append output to %s\n",getpid(), name[i+1]);
	   if (I > i) I = i;
            if (name[i+1]==0){
               print2f("invalid >> in command line\n\r");
               exit(4);
            }
	    /********** 
            flag = O_RDWR|O_CREAT|O_APPEND;
            printf("flag=%x  ", flag);
	    ****/
            close(1);
            open(name[i+1], O_WRONLY|O_CREAT|O_APPEND);
            break;
         }
    }

    strcpy(cmdline, name[0]);
    for (j=1; j<I; j++){
      strcat(cmdline, " ");
      strcat(cmdline, name[j]);
    }

    //print2f("cmdline = %s  ", cmdline);
    print2f("proc %d exec %s\n", getpid(), name[0]);

    exec(cmdline);
}

