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
#include <mtx_type.h>

char buf[1024];
int myuid, uid;
int fd, n,i;
char *cp, *cq, *cpp,*cqq, pline[64], *pname[8];

main(int argc, char *argv[ ])
{   
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    printf("        This is KC's %s in action\n", argv[0]);
    printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
    myuid = getuid();

    fd = open("/etc/passwd", 0); 
    if (fd < 0){
       printf("no passwd file\n");
       exit(1);
    }

    n = read(fd, buf, 1024);
    buf[n] = 0;
    cp = cq = buf;
    while(cp < &buf[n]){   
       while (*cq != '\n'){
         if (*cq == ' ') *cq = '-';
         if (*cq == ':') *cq = ' ';
         cq++;
       }
       *cq = 0;
       strcpy(pline, cp);
       cpp = cqq = pline;
       i = 0;
       while (*cqq){
         if (*cqq == ' '){
	   *cqq = 0;
           pname[i] = cpp;
           i++;
           cqq++; cpp = cqq;
           continue;
         }
         cqq++; 
       }

       uid = atoi(pname[3]);

       if (myuid == uid){
	  printf("%s\n", pname[0]);
          exit(0);
       }
       cq++; cp = cq;
    }
    exit(0);   
}


             
