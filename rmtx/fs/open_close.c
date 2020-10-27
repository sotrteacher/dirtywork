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

int truncate(MINODE *mip)
{
   int i, j; 
   u32 *up, *dup;
   struct buf *bp, *dbp;

   for (i=0; i<12; i++){
      if (mip->INODE.i_block[i] == 0)
         break;
      zdalloc(mip->dev, (u16)mip->INODE.i_block[i]);
      mip->INODE.i_block[i] = 0;
   }

   /* deallocate indirect data blocks */
   if (mip->INODE.i_block[12]){
     //get_block(mip->dev, (u16)mip->INODE.i_block[12], buf12);
      bp = bread(mip->dev, (u16)mip->INODE.i_block[12]);
      up = (u32 *)bp->buf;

      for (i=0; i<256; i++){
          if (up[i] == 0)
	     break;
          zdalloc(mip->dev, (u16)up[i]);
      }
      brelse(bp);

      zdalloc(mip->dev, (u16)mip->INODE.i_block[12]);
      mip->INODE.i_block[12] = 0;
   }

   // deallocate double-indirect blocks
   if (mip->INODE.i_block[13]){
      //get_block(mip->dev, (u16)mip->INODE.i_block[13], buf12);

      dbp = bread(mip->dev, (u16)mip->INODE.i_block[13]);
      dup = (u32 *)dbp->buf;
 
      for (i=0; i<256; i++){
	  if (dup[i] == 0)
             break;
	  //get_block(mip->dev, (u16)buf12[i], buf);
          bp = bread(mip->dev, (u16)dup[i]);
          up = (u32 *)bp->buf; 

          for (j=0; j<256; j++){
              if (up[j] == 0)
		 break;
              zdalloc(mip->dev, (u16)up[j]);
          }
          brelse(bp);  
          zdalloc(mip->dev, (u16)dup[i]);
      }
      brelse(dbp);

      zdalloc(mip->dev, (u16)mip->INODE.i_block[13]);
      mip->INODE.i_block[13] = 0;
   }
   mip->INODE.i_blocks = 0; 
   mip->INODE.i_mtime = mip->INODE.i_atime = 0;
   mip->INODE.i_size = 0;
   mip->dirty = 1;
}


int kopen(char *file, int flag)
{
  int mode, r;
  char pathname[32];

  get_param(file, pathname);

  r = myopen(pathname, flag);
  /*********
  if (r>2)
     printf(" r=%d refcount=%d\n", r, running->fd[r]->inodeptr->refCount);
  ***********/
  return r;
}

int myopen(char *pathname, int flag)
{
  int i, ino, dev;
  MINODE *mip;
  char filename[32];
  OFT *oftp;
   
  if (pathname[0] == '/'){
     dev = root->dev;
     strcpy(filename, pathname);
  }
  else{
     dev = running->res->cwd->dev;
     upwd(running->res->cwd);               /* need cwd name string */

     strcpy(filename, cwdname);  /******************** KCW ************/
     strcat(filename, pathname);
  }

  // printf("kopen:%d %s flag=%x  ",running->pid, filename, flag);

  ino = getino(&dev, filename);

  if (!ino){
    if (flag==0x0041 || flag==0x0241 || flag==0x441){ // WR|CREAT|APPEND
      //printf("no file, creat %s flag=%x\n", pathname, flag);
      if( ocreat(filename) < 0)
        return -1;
      //printf("creat OK\n");
      ino = getino(&dev, filename);
    }
    else{  // NOT O_CREAT
      return -1;
    }
  }
  
  mip = iget(dev,ino);  

  // check permission by running->uid and file's rwx rwx rwx
  // need access(mip, flag) function
  if (running->res->uid != 0){
    if (!access(mip, flag)){
        printf("kopen: psermission denied\n");
        iput(mip);
        return -1;
    }
  }

  // check for incompatible open modes: for non-special files only
  if ( !((mip->INODE.i_mode & 0170000)==0020000) && !((mip->INODE.i_mode & 0170000)==0060000) ){
     for (i=0; i<NOFT; i++){
        if (oft[i].refCount > 0 && (oft[i].inodeptr == mip)){
	  if (flag > 0 || oft[i].mode > 0){ // compatible: must be both READ
	     printf("%s already opened with incompatible mode\n", pathname);
             iput(mip);
             return -1;
           }
        }
     }
  }

  //strcpy(mip->name, filename);

  oftp = falloc();
  oftp->mode = flag;  // mode=flag AS IS
  oftp->refCount = 1;
  oftp->inodeptr = mip;

/***********
flag=0x000 RD
flag=0x001 WR                  0000 0001
flag=0x002 RDWR
flag=0x041 WR|CREAT            0100 0001
flag=0x241 WR|CREAT|TRUNC 0010 0100 0001
flag=0x401 WR|APPEND      0100 0000 0001
flag=0x441 WR|CREAT|APPEND

O_RDONLY	     00
O_WRONLY	     01
O_RDWR		     02
APPEND               03
O_CREAT		   0100	
O_TRUNC		  01000	
O_APPEND	  02000
**********/

  switch (flag){
     case O_RDONLY : oftp->offset = 0; // RD
                      break;
     case O_WRONLY : // WR
     case O_WRONLY|O_CREAT : // WR|CREAT 
     case O_WRONLY|O_CREAT|O_TRUNC : // WR|CREAT|TRUNC
                   if (mip->INODE.i_size)
                       truncate(mip);
                   oftp->offset = 0;
                   break;
     case O_RDWR   : oftp->offset = 0;   //RDWR
                     break;

     case O_WRONLY|O_APPEND :  
       //case 0_WRONLY|O_CREAT|O_APPEND :
       //case 02001:
     case 02002:
     case 02101:
     case 02102:      
                   oftp->offset =  mip->INODE.i_size;
                   break;
  default: printf("invalid flag\n", flag);
	          iput(mip);
                  return(-1);
  }

  for (i = 0; i<NFD; i++){
      if (running->res->fd[i] == 0)
         break;
  }

  if (i>=NFD){
    //prints("no more fd entry\n\r");
     fdalloc(oftp);
     iput(mip);
     return(-1);
  }

  running->res->fd[i] = oftp;
  //  mip->dirty = 1; 
  iunlock(mip);  // unlock minode  
  return(i);
}     
      
int kclose(int fd)
{
  if (fd < 0 || fd >= NFD){
    //printf("invalid file descriptor\n");
      return(-1);
  }

  if (running->res->fd[fd] == 0){
    //printf("invalid fd\n");
     return(-1);
  }
 
  return(myclose(fd));

}


int myclose(int fd)
{
  int mode; 
  OFT *oftp; 
  MINODE *mip;
  
  oftp = running->res->fd[fd];
  mode = oftp->mode;

  if (mode==4 || mode==5){ // KCW:mode was 0,1,2,3,4,5
     return(close_pipe(fd));
  }

  running->res->fd[fd] = 0;
  oftp->refCount--;
  if (oftp->refCount > 0)
     return(0);

  mip = oftp->inodeptr;

  ilock(mip);
  iput(mip);
  
  //putinode(mip);
  
  fdalloc(oftp);

  return(0);
}

/** lseek to at most begin/end of file **/

int klseek(int y, int z, int w, int ww)
{
  u32 position;
  position = w;    
  position = position << 16;
  position += z;
  return mylseek(y,position,ww); 
}

int mylseek(int fd, long position, int whence)
{
  ushort mode, old;
  OFT *oftp; MINODE *ip;

  oftp = running->res->fd[fd];
  ip = oftp->inodeptr;
  mode = oftp->inodeptr->INODE.i_mode;
  if (mode & 0100000 == 0100000 || mode & 0040000 == 0040000){ 
      if (position > ip->INODE.i_size) 
          position = ip->INODE.i_size;
  }
  if (whence==0)
      oftp->offset = position;
  if (whence==1)
    oftp->offset += position;
  if (whence==2)
    oftp->offset = ip->INODE.i_size + position;
  /*
  if (fd==3)
    printli(position);
  */
  return (u16)oftp->offset;
}
       
int kdup(int y, int z)
{
   int i;
   for (i=0; i<NFD; i++){
       if (running->res->fd[i]==0){
	 //printf("dup : %d by %d\n",i, y);
           running->res->fd[i] = running->res->fd[y];
           running->res->fd[y]->refCount++;
           return(i); 
      }
  }
  return(-1);  
}

int kdup2(int y, int z)
{
   if (running->res->fd[z])
       myclose(z);
   running->res->fd[z] = running->res->fd[y];
   running->res->fd[y]->refCount++;
   return(0); 
}

int kchmod(char *ufile, u16 mode)
{
   int i, ino, dev;
   MINODE *mip;  INODE *ip;
   char pathname[32];

   printf("mode=%x ", mode);
   get_param(ufile, pathname);
   
   printf("kchmod %s %x\n", pathname, mode);
 
   if (pathname[0] == '/')
      dev = root->dev;
   else
      dev = running->res->cwd->dev;
   ino = getino(&dev, pathname);

   if (!ino){
      return(-1);
   }

   mip = iget(dev,ino);
 
   if (running->res->uid != 0 && running->res->uid != mip->INODE.i_uid){
      prints("not owner\n\r");
      iput(mip);
      return(-1);
   }

   mode = mode & 0777;          /* retain only 9 bits */
   mip->INODE.i_mode &= 0177000;
   mip->INODE.i_mode |= mode;
   set_time(mip);
   mip->dirty = 1;             /* modified */
   
   iput(mip);
   //putinode(mip);
   return(0);
}


int kchown(char *ufile, int uid)
{
   int i, ino, dev, r;
   MINODE *mip; INODE *ip;
   char pathname[32];

   printf("kchown %d\n", uid);

   get_param(ufile, pathname);

   if (pathname[0] == '/')
      dev = root->dev;
   else
      dev = running->res->cwd->dev;

   ino = getino(&dev, pathname);

   if (!ino){
      return(-1);
   }

   mip = iget(dev,ino);

   if (running->res->uid != 0 && running->res->uid != mip->INODE.i_uid){
      prints("not owner\n\r");
      iput(mip);
      return(-1);
   }

   mip->INODE.i_uid = uid;
   set_time(mip);
   mip->dirty = 1;             /* modified */

   iput(mip);
   //   putinode(mip);
   return(0);
}

#define OWNER  000700
#define GROUP  000070
#define OTHER  000007

int access(MINODE *mip, int mode)
{   
  ushort uid, gid, fmode;
  int r;

  uid = running->res->uid;
  gid = running->res->gid;

  mode = mode & 0x00F; // mode = 0,1,2

  if (uid == mip->INODE.i_uid) 
     fmode = (mip->INODE.i_mode & OWNER) >> 6;
  else{
       if (gid == mip->INODE.i_gid)
          fmode = (mip->INODE.i_mode & GROUP) >> 3;
       else
          fmode = mip->INODE.i_mode & OTHER;
  }
  
  r = 0;
  switch(mode){
     case 0:  if (fmode & 4)         /* READ access */
                  r = 1;              break;
     case 1:  if (fmode & 2)         /* WRITE access */        
                  r = 1;              break;

     case 2:  if ((fmode & 2) && (fmode & 4))     /* RW access */
                 r = 1;
              break;
  }    
  return r;
}

