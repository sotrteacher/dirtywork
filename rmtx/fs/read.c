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

int kread(int fd, char *buf, int nbytes)
{
 int i, n, r, t, device, mode, major, dev; 
 char line[64], *cp;
 OFT *oftp;
 u32 size;
 u16 blk, *up;

 if (nbytes < 0){
    printf("bad nbytes\n");
    return(-1);
 }

 if (fd < 0 || fd >= NFD){
    printf("bad fd number %d\n", fd);
    return(-1);
 }
 oftp = running->res->fd[fd];
 if (oftp == 0){
    printf("bad fd number %d\n", fd);
    return(-1);
 }     

 if (oftp->mode != 0 && oftp->mode != 4){
    printf("fd %d is not for READ\n", fd);
    return(-1);
 }

 if (oftp->mode == READ_PIPE){
    return read_pipe(fd, buf, nbytes);
 }

 /********************** 12-2010 *******************************************
  kread() eventually access INODE in memory. The minode was unlocked after
  open(). To guarantee read/write INODE one at a time, must lock the minode.
  However, read/write special files does NOT need minode locking because the
  device driver will enforce one at a time. So, do minode locking only for 
  non-special files.
 *************************************************************************/

 /****** READ special files ******/
 mode = oftp->inodeptr->INODE.i_mode;
 if ((mode & 0170000)==0020000){ // char special file
    device = oftp->inodeptr->INODE.i_block[0];
    major = (device & 0xFF00) >> 8;
    return (*devsw[major].dev_read)(device, buf, nbytes);
 }
 if ((mode & 0170000)==0060000){ // block special file
    device = oftp->inodeptr->INODE.i_block[0];
    major = (device & 0xFF00) >> 8;
    dev   = (device & 0x00FF);

    blk = oftp->offset / 1024;

    if (blk > devtab[dev].size/BLOCK_SIZE){
      printf("blk exceeds size\n");
      return -1;
    }
    oftp->offset += 1024;
    
    return (*devsw[major].dev_read)(device, blk, buf);
 }   

 // non-special file: lock the minode first
 ilock(oftp->inodeptr);
   n = myread(fd, buf, nbytes, 0);
 iunlock(oftp->inodeptr);

 return n;
}

int MIN(int a, int b)
{
  return (a <= b)?  a : b;
}
    
int myread(int fd, char *buf, int nbytes, int space)
{
  int dev, i, n, nn, start, remain, t;
  MINODE *ip;   OFT *oftp;
  char  *cp, *cq;
  u16 ino, blk, lbk;
  u32 size, dblk;
  int bad, j;
  struct buf *bp;
  u32 *up;

  oftp = running->res->fd[fd];
  ip = oftp->inodeptr;
  /* compute logical block */

  cq = buf;

  oftp = running->res->fd[fd];
  ip = oftp->inodeptr;
  size = ip->INODE.i_size - oftp->offset;;

  n = MIN(nbytes, size); // can read at most size bytes
  t = 0;

  while (n > 0){
     lbk = oftp->offset / BLOCK_SIZE;
     start = oftp->offset % BLOCK_SIZE;

     if (lbk < 12){              /* direct block */
        blk = ip->INODE.i_block[lbk];
     }

     else if (lbk >= 12 && lbk < 256 + 12) {  /* indirect blocks */
         //get_block(ip->dev, (u16)ip->INODE.i_block[12], buf12);
         bp = bread(ip->dev, (u16)ip->INODE.i_block[12]);
         up = (u32 *)bp->buf;

         blk = up[lbk-12];   /* get the blk number */
         brelse(bp);
     }

     else{ /* double indirect blocks */
         lbk -= (12 + 256);
  
         //get_block(ip->dev, (u16)ip->INODE.i_block[13], buf12);
         bp = bread(ip->dev, (u16)ip->INODE.i_block[13]);
         up = (u32 *)bp->buf;

         dblk = up[lbk/256];
         brelse(bp);

         //get_block(ip->dev, (u16)dblk, buf12);   /* get dindirect */
         bp = bread(ip->dev, (u16)dblk);   /* get dindirect */
         up = (u32 *)bp->buf;

         blk = up[lbk % 256];
         brelse(bp);
     }

     bp = bread(ip->dev, (u16)blk);

     /* copy bytes from start to buf[], at most remain bytes */
     cp = bp->buf + start;
     remain = BLOCK_SIZE - start;

     nn = MIN(n, remain);

     oftp->offset += nn; 
     t += nn;
     n -= nn;

     while (nn > 0){
       if (space)            // space=1 for K space
	    *cq = *cp;
       else                  // space=0 for U space
            put_ubyte(*cp, cq);
       cp++; cq++;
       nn--;
     }
     brelse(bp);
  } 
  return(t);
}
