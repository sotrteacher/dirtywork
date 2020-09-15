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

/*******************************************************
*                  @bc.c file                          *
*******************************************************/
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

int prints(char *s)
{
   while(*s)
     putc(*s++);
}

int gets(char *s)
{ 
    while ( (*s=getc()) != '\r')
      putc(*s++);
    *s = 0;
}


u16 NSEC = 2;
char buf1[BLK], buf2[BLK];

u16 getblk(u16 blk, char *buf)
{
    readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}

u16 search(INODE *ip, char *name)
{
   int i; char c;
   DIR  *dp; 
   
   for (i=0; i<12; i++){
       if ( (u16)ip->i_block[i] ){
          getblk((u16)ip->i_block[i], buf2);
          dp = (DIR *)buf2;

          while ((char *)dp < &buf2[1024]){
              c = dp->name[dp->name_len];  // save last byte

              dp->name[dp->name_len] = 0;   
	      prints(dp->name); putc(' ');

              if ( strcmp(dp->name, name) == 0 ){
                 prints("\n\r"); 
                 return((u16)dp->inode);
              }
              dp->name[dp->name_len] = c; // restore that last byte

              dp = (char *)dp + dp->rec_len;
	}
     }
   }
   error();
}

main()
{ 
  u16    i;
  u16    me, blk, iblk;
  char   *cp, *name[2], filename[64];
  u32    *up;
  GD     *gp;
  INODE  *ip;
  DIR    *dp;

  name[0] = "boot";
  name[1] = filename;

  prints("boot (mtx) : ");  

  gets(filename);

  if (filename[0]==0)
     name[1]="mtx";

  /* read blk#2 to get group descriptor 0 */
  getblk(2, buf1);
  gp = (GD *)buf1;
  iblk = (u16)gp->bg_inode_table;
  getblk(iblk, buf1);       // read first inode block block
  ip = (INODE *)buf1 + 1;   // ip->root inode #2

  /* serach for system name */
  for (i=0; i<2; i++){
      me = search(ip, name[i]) - 1;
      if (me < 0) 
         error();
      getblk(iblk+(me/8), buf1);    // read block inode of me
      ip = (INODE *)buf1 + (me % 8);
  }

  /* read indirect block into b2 */
  if ((u16)ip->i_block[12])         // only if has indirect blocks 
     getblk((u16)ip->i_block[12], buf2);

  setes(0x1000);
  for (i=0; i<12; i++){
      getblk((u16)ip->i_block[i], 0);
      putc('*');
      inces();
  }
  if ((u16)ip->i_block[12]){ // only if file has indirect blocks
     up = (u32 *)buf2;      
     while(*up){
        getblk((u16)*up, 0); putc('.');
        inces();
        up++;
     }
  }
  prints("ready to go?"); getc();
}  
