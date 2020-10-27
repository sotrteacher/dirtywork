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
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h"

int fdtype(int fd)
{  
   if (fd >=0 && fd < NFD){
      if (running->res->fd[fd]){
         return running->res->fd[fd]->mode;
      }
   }
}

STAT kst;

int ustat(MINODE *mip, char *z)
{
  INODE *ip = &(mip->INODE);
  STAT *sp = &kst;
  char *cp;
  int i;

  // copy minode into kstat structure first
  sp->st_dev=mip->dev; sp->st_ino=mip->ino;
  sp->st_mode = ip->i_mode;
  sp->st_nlink = ip->i_links_count;
  sp->st_uid = ip->i_uid;  sp->st_gid = ip->i_gid;
  sp->st_size = ip->i_size;

  sp->st_atime = ip->i_atime;
  sp->st_ctime = ip->i_ctime;
  sp->st_mtime = ip->i_mtime;
  sp->st_dtime = ip->i_dtime;
  sp->st_date  = ip->i_date;
  sp->st_time  = ip->i_time;

  cp = (char *)&kst;

  for (i=0; i<sizeof(STAT); i++){
      put_ubyte(*cp, z);
      cp++; z++;
  }
}


kstat(char *y, char *z)
{
  ushort dev, ino;
  MINODE *mip; 
  INODE *ip;
  STAT kstat, *sp; 
  char *cp; int i;
  char pathname[32];

  get_param(y, pathname);

  if (strcmp(pathname, "./")==0 || strcmp(pathname, ".")==0){
          mip = running->res->cwd; 
          mip->refCount++;
          P(&mip->lock);
  }
  else{     
          if (pathname[0] == '/')
              dev = root->dev;
          else 
              dev = running->res->cwd->dev;
  
          ino = getino(&dev, pathname);
          if (!ino){
	    //printf("kstat: BAD\n");
              return(-1);
          }
          mip = iget(dev, ino);
  }  

  ustat(mip, z);

  iput(mip);
  return(0);
}


int kfstat(int fd, char *z)
{
  MINODE *mip;

  if (running->res->fd[fd]==0)
    return -1;
  mip = running->res->fd[fd]->inodeptr;
  ustat(mip,z);
  return 0;
}
    
