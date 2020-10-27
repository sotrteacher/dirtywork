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

#define OWNER  000700
#define GROUP  000070
#define OTHER  000007

int okToX(ip)
    MINODE *ip;
{
  ushort uid, gid, fmode;
  uid = running->res->uid; 
  gid = running->res->gid;

  if (uid == SUPER_USER)  
     return(1);

  if (uid == ip->INODE.i_uid) 
     fmode = (ip->INODE.i_mode & OWNER) >> 6;
  else{
       if (gid == ip->INODE.i_gid)
          fmode = (ip->INODE.i_mode & GROUP) >> 3;
       else
          fmode = ip->INODE.i_mode & OTHER;
  }

  if (fmode & 1)
      return(1);
  return(0);
}

int kchdir(char *y, int z)
{
  DIR *dp; MINODE *ip, *newip;
  int dev; 
  u16 ino;
  char c;
  char pathname[32],temp[32];

  get_param(y, pathname);

symlink:  
  if (strcmp(pathname, "/")==0){

    ilock(running->res->cwd);
    iput(running->res->cwd);

    running->res->cwd = iget(root->dev, 2);
    iunlock(root);
    
    return 0;
  }
        
  if (pathname[0] == '/')  
     dev = root->dev;
  else                     
     dev = running->res->cwd->dev;

  strcpy(temp, pathname);

  ino = getino(&dev, temp);

  if (!ino){
    //prints("cd : no such directory\n\r");
     return(-1);
  }

  //printf("kcd : %s ino=%d ", pathname, ino);

  newip = iget(dev, ino);    /* get inode of this ino */

  if (newip == running->res->cwd){ // same DIR; must iput() to unlock it
    iput(newip);
    return 0;
  }

  if ( (newip->INODE.i_mode & 0120000) == 0120000){
    printf("%s is a symlink -> ", pathname);
    strcpy(pathname, (char *)newip->INODE.i_block);
    printf("%s\n", pathname);
    iput(newip);
    goto symlink;
  }

  if ((newip->INODE.i_mode & 0170000) != 0040000){  
     printf("not a DIR\n");
     iput(newip);
     return(-1);
  }

  if ( !okToX(newip) ){
     printf("cd: access denied\n");
     iput(newip);
     return(-1);
  }
  
  ilock(running->res->cwd);
  iput(running->res->cwd);

  running->res->cwd = newip;
  iunlock(newip);        // unlock new cwd
}

/*********************************
kpwd(wd) MINODE *wd;
{ 
  char myname[16];
  MINODE *parent, *ip;
  u16 dev, myino, parentino; 
  DIR *dp;
  MOUNT *mp; 

  if (wd == root){
      prints(" /");
      return;
  }

  findino(wd, &myino, &parentino);

  if (myino == 2){
      mp = getmountp(wd->dev);
      ip = mp->mounted_inode;
      findino(ip, &myino, &parentino);
      wd = ip;
  }

  parent = iget(wd->dev, parentino);
  findmyname(parent, myino, myname);

  kpwd(parent);

  iput(parent);

  printf("%s/",myname);

}
***********************************************/

char cwdname[64];

int kgetcwd(char *y, char *z)
{
    char *p, *q;
    p = cwdname; *p = 0;

    upwd(running->res->cwd);

    p = cwdname;
    cps2u(cwdname, y);
    return strlen(cwdname);
}

upwd(MINODE *wd)
{ 
  char myname[16];
  MINODE *parent, *ip;
  int dev, myino, parentino; DIR *dp;
  MOUNT *mp; 

  if (wd == root){
      strcpy(cwdname, "/");
      return;
  }

  parentino = findino(wd, &myino);

  if (myino == 2){
      mp = getmountp(wd->dev);
      ip = mp->mounted_inode;
      parentino = findino(ip, &myino);
      wd = ip;
  }

  parent = iget(wd->dev, parentino);

  findmyname(parent, myino, myname);

  upwd(parent);

  iput(parent);

  strcat(cwdname, myname); strcat(cwdname,"/");
}

