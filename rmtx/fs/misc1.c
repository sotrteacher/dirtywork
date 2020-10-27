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

int kchmod(char *ufile, int mode)
{

   int i, ino, dev;
   MINODE *mip;
   char pathname[32];

   get_param(ufile, pathname);
  

   if (pathname[0] == '/')
      dev = root->dev;
   else
      dev = running->res->cwd->dev;
   ino = getino(&dev, pathname);

   if (!ino){
     //printf("no such file\n");
      return(-1);
   }

   mip = iget(dev,ino);
   /*********
   if (ip==0){
       printf("no such file\n");
       return(-1);
   }  
   ************/

   if (running->res->uid != 0 && running->res->uid != mip->INODE.i_uid){
      printf("not owner\n");
      iput(mip);
      return(-1);
   }
   // mode comes in as int converted from 0xyz
   mode = mode & 0777;        /* retain only 9 bits */
   mip->INODE.i_mode &= 00177000;   // zero out 9 bits permissions
   mip->INODE.i_mode |= mode;       // replace permission bits
   settime(mip);
   mip->dirty = 1;             /* modified */
   printf("mip->mode = %o\n", mip->INODE.i_mode);
   
   iput(mip);
   return(0);

}


int kchown(char *ufile, int uid)
{
   int i, ino, dev;
   MINODE *mip;
   char pathname[32];

   get_param(ufile, pathname);

   if (pathname[0] == '/')
      dev = root->dev;
   else
      dev = running->res->cwd->dev;
   ino = getino(&dev, pathname);

   if (!ino){
     //printf("no such file\n");
      return(-1);
   }

   mip = iget(dev,ino);

   if (running->res->uid != 0 && running->res->uid != mip->INODE.i_uid){
      printf("not owner\n");
      iput(mip);
      return(-1);
   }

   mip->INODE.i_uid = uid;
   settime(mip);
   mip->dirty = 1;             /* modified */

   printf("mip->uid = %d\n", mip->INODE.i_uid);
   
   iput(mip);
   return(1);
}
