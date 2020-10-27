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

int ktouch(char *y)
{ 
  int  dev, ino;
  char pathname[32], filename[32];
  MINODE *mip;

 // y points to pathname string in Uspace
  get_param(y, pathname);

  //  printf("ktouch %s\n", pathname);

   if (pathname[0] == '/'){
     dev = root->dev;
     strcpy(filename, pathname);
  }
  else{
     dev = running->res->cwd->dev;
     upwd(running->res->cwd);               // need cwd name string 

     strcpy(filename, cwdname);  
     strcat(filename, pathname);
  }

  ino = getino(&dev, filename);
  if (ino==0)
    return -1;

  mip = iget(dev,ino);  

  // change time fields of mip->INODE;
  set_time(mip);
  mip->dirty = 1;
  iput(mip);
  return 0;
}

int set_time(MINODE *mip)
{
  biostime(); // BIOS 0x1A to set |centry,year,month,day,bhr,bmin,bsec,bzero| 

  memcpy(&(mip->INODE.i_date), btime, 4);   // INODE.i_date=date
  memcpy(&(mip->INODE.i_time), btime+4, 4); // INODE.i_time=hh:mm:sec

  //pmdate(&mip->INODE.i_date);  pmtime(&mip->INODE.i_time);
}

int pmdate(u8 t[4])
{
   printf("date=%c%c%c%c-%c%c-%c%c  ",

          (t[0]>>4)+'0', (t[0]&0x0F)+'0',
          (t[1]>>4)+'0', (t[1]&0x0F)+'0', 
          (t[2]>>4)+'0', (t[2]&0x0F)+'0',
          (t[3]>>4)+'0', (t[3]&0x0F)+'0'
	  /*******
          (t[3]>>4)+'0', (t[3]&0x0F)+'0',
          (t[2]>>4)+'0', (t[2]&0x0F)+'0', 
          (t[1]>>4)+'0', (t[1]&0x0F)+'0',
          (t[0]>>4)+'0', (t[0]&0x0F)+'0'
	  ***************/
         );
}
     
int pmtime(u8 t[4])
{
   printf("time=%c%c:%c%c:%c%c\n", 
          (t[0]>>4)+'0', (t[0]&0x0F)+'0',
          (t[1]>>4)+'0', (t[1]&0x0F)+'0', 
          (t[2]>>4)+'0', (t[2]&0x0F)+'0'
	  /*************
          (t[3]>>4)+'0', (t[3]&0x0F)+'0',
          (t[2]>>4)+'0', (t[2]&0x0F)+'0', 
          (t[1]>>4)+'0', (t[1]&0x0F)+'0'
	  **************/
         );
}

int ptime(u8 t[8])
{
   printf("date=%c%c%c%c-%c%c-%c%c  ",
          (t[0]>>4)+'0', (t[0]&0x0F)+'0',
          (t[1]>>4)+'0', (t[1]&0x0F)+'0', 
          (t[2]>>4)+'0', (t[2]&0x0F)+'0',
          (t[3]>>4)+'0', (t[3]&0x0F)+'0'
	  /***********
          (t[7]>>4)+'0', (t[7]&0x0F)+'0',
          (t[6]>>4)+'0', (t[6]&0x0F)+'0', 
          (t[5]>>4)+'0', (t[5]&0x0F)+'0',
          (t[4]>>4)+'0', (t[4]&0x0F)+'0'
	  ****************/
         );
   printf("time=%c%c:%c%c:%c%c\n", 
          (t[4]>>4)+'0', (t[4]&0x0F)+'0',
          (t[5]>>4)+'0', (t[5]&0x0F)+'0', 
          (t[6]>>4)+'0', (t[6]&0x0F)+'0'
	  /**********
          (t[3]>>4)+'0', (t[3]&0x0F)+'0',
          (t[2]>>4)+'0', (t[2]&0x0F)+'0', 
          (t[1]>>4)+'0', (t[1]&0x0F)+'0'
	  ************/
         );
}
