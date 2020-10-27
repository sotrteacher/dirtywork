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
/*************** Unix cp -r **************************************
  cp -r DIR dir : dir not existing ==> mkdir dir; cp contents of DIR into dir
                  dir existing     ==>  dir
                                      ... DIR
****************************************************************/
#include "ucode.c"

int fd, gd, n, total;
char buf[1024];
// cp f1 f2 ... fn 

int S_ISDIR(m) u16 m;
{
  if ((m & 0040000)==0040000)
    return 1;
  return 0;
}

int S_ISREG(m) u16 m;
{
  if ((m & 0100000)==0100000)
    return 1;
  return 0;
}

int S_ISLNK(m) u16 m;
{
  if ((m & 0120000)==0120000)
    return 1;
  return 0;
}
   
char *dirname(s) char *s;
{
 char *p;
 p = s;
 while (*p) p++;
 while (*(p) != '/' && p != s) p--;
 if (p == s){
     if (*s == '/') 
         return('/');
     else
         return('.');
 }
 *p = 0;
 return(s);
}      

char *basename(s) char *s;
{
 char *p;
 p = s;

 while (*p) p++;
 while (*p != '/' && p != s) 
       p--;
 if (p == s){
    if (*s == '/') 
       return(p+1);
    else
       return(p);
 }
 return(p+1);
}

int samefile(f1, f2) char *f1, *f2;
{
  STAT st1, st2;
  if (stat(f1, &st1) < 0)
    return 0;
  if (stat(f2, &st2) < 0)
    return 0;
  if ((st1.st_dev == st2.st_dev) && (st1.st_ino == st2.st_ino))
    return 1;
  return 0;
}

// cp f1 f2 f2 ... fn
int main(argc, argv) int argc; char *argv[ ];
{  
  STAT st1, st2;
  int  i, n, r;
  char stemp[64], dtemp[64];

  printf("\nargc = %d  argv = ", argc);

  for (n=0; n<argc; n++)
    printf("%s ", argv[n]);
  printf("\n");

  if (argc<3){
    printf("usage : cp f1 f2\n");
    exit(1);
  }

  // n = 2 case : cp src dst
  if (argc == 3){
    if (stat(argv[1], &st1) < 0){
      printf("bad src %s\n", argv[1]);
      exit(1);
    }
    if (stat(argv[2], &st2) < 0){  // f2 does not exist
      if (S_ISREG(st1.st_mode))
	cpf2f(argv[1], argv[2]);
      else{
	mkdir(argv[2]);
        cpd2d(argv[1], argv[2]);
      }
      exit(0);
    }

    // f2 exists
    stat(argv[2], &st2);

    if (S_ISREG(st1.st_mode)){  
    
       if ( S_ISREG(st2.st_mode) )
           
	   cpf2f(argv[1], argv[2]);
  
       if (S_ISDIR(st2.st_mode))
         cpf2d(argv[1], argv[2]);

       exit(0);
    }

    // f2 exists & f1 is DIR
    if (!S_ISDIR(st2.st_mode)){
      printf("can't cp DIR %s to file %s\n", argv[1], argv[2]);
      exit(1);
    }

    // cp dir into existing dir
    if (samefile(argv[1], argv[2])){
      printf("can't cp DIR %s into itself\n", argv[1]);
      exit(1);
    }
    strcpy(stemp, argv[1]); strcpy(dtemp, argv[2]); 
    strcat(dtemp, "/"); strcat(dtemp, basename(stemp));
    printf("mkdir %s\n", dtemp);
    mkdir(dtemp);
    cpd2d(argv[1], dtemp);
    exit(0);

  }
 
  /************* n > 2 ==> fn must be a DIR ***********/    
  if (stat(argv[argc-1], &st2) < 0){
      printf("no such IDR %s\n", argv[argc-1]);
      exit(1);
  }
  if (!S_ISDIR(st2.st_mode)){
      printf("%s is not a DIR\n", argv[argc-1]);
      exit(2);
  }
  // now do it for each argv[ ]
  for (i=1; i<argc-1; i++){
      if (stat(argv[i], &st1) < 0)
	continue;
      if (S_ISREG(st1.st_mode))
	cpf2d(argv[i], argv[argc-1]);
      if (S_ISDIR(st1.st_mode))
        cpd2d(argv[i], argv[argc-1]);
  }
}

// cp file to file
int cpf2f(src, dst) char *src, *dst;
{
  int fd, gd, n, r, mode;
  char buf[1024];
  struct stat st;

  if (samefile(src, dst)){
    printf("can't cp %s to itself\n", src);
    return -1;
  }

  r = stat(src, &st);

  if (S_ISLNK(st.st_mode)){
    printf("%s is a symlink file ==> ", src);
    n = readlink(src, buf, 1024);  
    buf[n] = 0;
    printf("%s\n", buf);
    getc();
    // make a symbolic file
    symlink(buf, dst);
    return 0;
  }
 
  fd = open(src, O_RDONLY); 
  gd = open(dst, O_WRONLY|O_CREAT|O_TRUNC);

  while( (n=read(fd, buf, 1024)) )
    write(gd, buf, n);

  close(fd); close(gd);

  return 0;
}

int cpf2d(f1, f2) char *f1, *f2;
{
  int  n, size;
  char buf[1024], name[128], f3[128];
  DIR *gd;
  struct stat st, st3;
  struct DIR *dp;

  printf("\ncpf2d : cp %s into %s\n", f1, f2);

  n = stat(f2, &st);
  if (n<0 || S_ISDIR(st.st_mode)==0){
    printf("no such dir %s\n", f2);
    return -1;
  }

  strcpy(name, (char *)basename(f1));
  strcpy(f3, f2); strcat(f3, "/"); strcat(f3, name);
  return cpf2f(f1, f3);
}


// recursively cp dir into dir 
int cpd2d(f1,f2) char *f1, *f2;
{
  int  n, r, found;
  char buf[1024], name[16], dname[128];
  char src[128], dst[128];
  char *cp, temp[32];
  int  fd, gd;
  DIR *dp;
  STAT st;

  printf("entering cpd2d : %s  %s\n",f1,f2);
  getc();
  if (samefile(f1, f2)){
    printf("can't cp DIR %s to itself\n", f1);
    return -1;
  }

  // KCW : should also check path
 
  n = stat(f1, &st);
  if (n<0 || S_ISDIR(st.st_mode)==0){
    printf("%s is not a dir\n", f1);
    return -1;
  }

  n = stat(f2, &st);
  if (n<0 || S_ISDIR(st.st_mode)==0){
    printf("%s is not a dir\n", f2);
    return -1;
  }

  fd = open(f1, O_RDONLY);

  // step through DIR entries of f1

  while (n=read(fd, buf, 1024)){

    cp = buf; dp = (DIR *)buf;

    while (cp < &buf[1024]){
      strncpy(temp, dp->name, dp->name_len);
      temp[dp->name_len] = 0;
      printf("%s   ", temp);
      cp += dp->rec_len;
      dp = (DIR *)cp;
    }
    printf("\n");

    cp = buf; dp = (DIR *)buf;

    while (cp < &buf[1024]){
      strncpy(temp, dp->name, dp->name_len);
      temp[dp->name_len] = 0;
      printf("%s   ", temp);

      if (strcmp(temp, ".")==0 || strcmp(temp, "..")==0){
	cp += dp->rec_len;
        dp = (DIR *)cp;
        continue;
      }
      
      strcpy(src, f1); strcat(src, "/"); 
      strcat(src, temp);

      strcpy(dst, f2); strcat(dst, "/"); strcat(dst, temp); 
      printf("src=%s   dst=%s\n", src, dst); getc();

      
      r = stat(src, &st);

      if (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)){
	  printf("\ncp file: %s to %s\n", src, dst);
	  cpf2f(src, dst);
      }

      if (S_ISDIR(st.st_mode)){
          printf("recursive cp dir %s to %s\n", src, dst);
          mkdir(dst); 
          cpd2d(src, dst);
      }
      cp += dp->rec_len;
      dp = (DIR *)cp;
    }
  }
  close(fd);
}

