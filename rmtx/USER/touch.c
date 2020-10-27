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

char *rootdir[ ]={"/", "/bin", "/boot", "/dev", "/etc", "/user", 0};

main(argc,argv) int argc; char *argv[];
{

  int fd, i;
  char buf[1024], *cp, *np;
  DIR *dp;
  char name[64];

  /*
  print2f("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
  print2f("      This is KC's touchall in action\n");
  print2f("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
  */

  //showarg(argc, argv);

  if (argc > 1){
     for (i=1; i<argc; i++){
         printf("touching %s\n", argv[i]);
         if (strcmp(argv[i], "*")==0)
	   argv[i]="/";
         touch(argv[i]);
     }
     exit(0);
  }

  //  printf("touch ALL\n");
  touch("/");
  chdir("/");
  i = 0;
  np = rootdir[i];
  while(np){
    //printf("name=%s\n", np);
    chdir(np);  
    fd = open(np, O_RDONLY);
    //printf("fd=%d\n", fd);
    read(fd, buf, 1024);
    dp = (DIR *)buf;
    cp = buf;
    while(cp < buf+1024){
        strncpy(name, dp->name, dp->name_len);
        name[dp->name_len] = 0;
        if (!strcmp(name, ".") || !strcmp(name, "..")){
	     cp += dp->rec_len;
             dp = (DIR *)cp;
	     continue;
        }
        //printf("%s  ", name);      
        touch(name);
        cp += dp->rec_len;
        dp = (DIR *)cp;
    }
    //printf("\n");
    close(fd);  
    chdir("/");
    i++;
    np = rootdir[i];
  }
}

