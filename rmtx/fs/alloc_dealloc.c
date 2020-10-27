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

MINODE *mialloc()
{ 
   int i;
   for (i=0; i<100; i++){
     if (minode[i].refCount == 0){
        minode[i].refCount = 1; 
        minode[i].dirty = minode[i].mounted = 0;
        minode[i].mountptr = 0; 
        return(&minode[i]);
     }
   }
   return(0);
}


midalloc(MINODE *mip)
{
  mip->refCount = 0;
}

int incFreeInodes(int dev)
{
  struct buf *bp,*bp2;

  // inc free inodes count in SUPER and GD
  //  get_block(dev, 1, buf);
  bp = bread(dev, 1);

  sp = (SUPER *)bp->buf;
  sp->s_free_inodes_count++;
  //put_block(dev, 1, buf);

  bwrite(bp);

  //get_block(dev, 2, buf);
  bp2 = bread(dev, 2);

  gp = (GD *)bp2->buf;
  gp->bg_free_inodes_count++;

  //put_block(dev, 2, buf);
  bwrite(bp2);
}

int decFreeInodes(int dev)
{
  struct buf *bp, *bp2;

  // dec free inodes count in SUPER and GD
  //get_block(dev, 1, buf);
  bp = bread(dev, 1);
  sp = (SUPER *)bp->buf;

  sp->s_free_inodes_count--;

  //put_block(dev, 1, buf);
  bwrite(bp);

  //get_block(dev, 2, buf);
  bp2 = bread(dev, 2);
  gp = (GD *)bp2->buf;

  gp->bg_free_inodes_count--;
  //put_block(dev, 2, buf);

  bwrite(bp2);
}

int incFreeBlocks(int dev)
{
  struct buf *bp, *bp2;

  // inc free block count in SUPER and GD
  //get_block(dev, 1, buf);
  bp = bread(dev, 1);
  sp = (SUPER *)bp->buf;

  sp->s_free_blocks_count++;

  //put_block(dev, 1, buf);
  bwrite(bp);

  //get_block(dev, 2, buf);
  bp2 = bread(dev, 2);
  gp = (GD *)bp2->buf;

  gp->bg_free_blocks_count++;

  //put_block(dev, 2, buf);
  bwrite(bp2);
}

int decFreeBlocks(int dev)
{
  struct buf *bp, *bp2;
  // dec free block count in SUPER and GD
  //get_block(dev, 1, buf);

  bp = bread(dev, 1);
  sp = (SUPER *)bp->buf;

  sp->s_free_blocks_count--;

  //put_block(dev, 1, buf);
  bwrite(bp);

  //get_block(dev, 2, buf);
  bp2 = bread(dev, 2);
  gp = (GD *)bp2->buf;

  gp->bg_free_blocks_count--;

  //put_block(dev, 2, buf);
  bwrite(bp2);
}

int tst_bit(char *buf, int bit)
{
  int i, j;

  i = bit / 8;
  j = bit % 8;
  if (buf[i] & (1 << j)){
    return 1;
  }
  return 0;
}

int clr_bit(char *buf, int bit)
{
  int i, j;
  i = bit / 8;
  j = bit % 8;
  buf[i] &= ~(1 << j);
  return 0;
}


int set_bit(char *buf, int bit)
{
  int i, j;
  i = bit / 8;
  j = bit % 8;
  buf[i] |= (1 << j);
  return 0;
}

ushort ialloc(int dev)
{
  int i, imap;
 MOUNT *mp;
 struct buf *bp;

 mp = (MOUNT *)getmountp(dev);
 imap = mp->IMAP;

 //printf("ialloc dev=%d\n", dev);

 // get inode Bitmap into buf
 //get_block(dev, 4, buf);
 bp = bread(dev, imap);
 
 for (i=0; i<mp->ninodes; i++){
   if (tst_bit(bp->buf, i)==0){
     set_bit(bp->buf, i);

     //put_block(dev, 4, buf);
     bwrite(bp);

     // update free inode count in SUPER and GD
     decFreeInodes(dev);
     return (i+1);
   }
 }
 return 0;
} 

idalloc(int dev, int ino)
{
  int i;  
  MOUNT *mp;
  struct buf *bp;

  mp = (MOUNT *)getmountp(dev);
  if (ino > mp->ninodes){
    //printf2u("inumber out of range\n");
    return;
  }
  
  // get inode bitmap block
  //get_block(dev, 4, buf);

  bp = bread(dev, mp->IMAP);

  clr_bit(bp->buf, ino-1);

  // write buf back
  //put_block(dev, 4, buf);
  bwrite(bp);

  // update free inode count in SUPER and GD
  incFreeInodes(dev);
}

u16 zalloc(int dev)
{
 int i;
 MOUNT *mp;
 struct buf *bp;

 mp = (MOUNT *)getmountp(dev);

 // get blocks bitmap
 //get_block(dev, 3, buf);

 bp = bread(dev, mp->BMAP);

 for (i=0; i<mp->nblocks; i++){
   if (tst_bit(bp->buf, i)==0){
     set_bit(bp->buf, i);
     // write buf back
     //put_block(dev, 3, buf);
     bwrite(bp);

     // update free block count in SUPER and GD
     decFreeBlocks(dev); 

     return(i+1);
   }
 }
 return 0;
}


int zdalloc(int dev, int bit)
{
  int i;
  MOUNT *mp;
  struct buf *bp;
 
  mp = (MOUNT *)getmountp(dev);
  if (bit > mp->nblocks){
    //printf2u("data block out of range\n");
    return;
  }

  // get blocks bitmap
  //get_block(dev, 3, buf);
  bp = bread(dev, mp->BMAP);

  clr_bit(bp->buf, bit-1);

  // write buf back
  //put_block(dev, 3, buf);
  bwrite(bp);

  // update free block count in SUPER and GD
  incFreeBlocks(dev);
}


OFT *falloc()
{
 int i;
 for (i=0; i<NOFT; i++){
     if (oft[i].refCount == 0)
        break;
 }
 if (i>=NOFT){
   //printf2u("no more OFT entry\n");
    return(0);
 }
 return(&oft[i]);
}

fdalloc(oftptr) OFT *oftptr;
{
 oftptr->refCount = 0;
}
     
/** flash inode back to disk */

// why is this needed? especially in myclose()?

int putinode(MINODE *mip)
{
  u16  i, block, offset;
  char *cp, *cq;
  MOUNT *mp;
  struct buf *bp;

  mp = (MOUNT *)getmountp(mip->dev);

  block =  ((mip->ino - 1) / 8) + mp->IBLOCK;
  offset = (mip->ino - 1) % 8;

  /* first get the block containing this inode */
  //get_block(mip->dev, (u16)block, buf);
  bp = bread(mip->dev, (u16)block);

  cp = (char *)mip; 
  cq = (char *)&bp->buf[offset*128];
  for (i=0; i<128; i++)
     *cq++ = *cp++;
  //put_block(mip->dev, (u16)block, buf);
  bwrite(bp);
}
