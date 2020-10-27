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

int kwrite(int fd, char *buf, int nbytes)
{
  char c, *cp;
  OFT *oftp;
  ushort blk, i, n, device, mode, major, dev;
 
  if (fd < 0 || fd >= NFD){
     printf("bad fd number\n", fd);
     return(-1);
  }
  oftp = running->res->fd[fd];
  if (oftp == 0){
      printf("bad fd number\n",fd);
      return(-1);
  }
  if (oftp->mode == 0){
      printf("fd = %d is NOT FOR WRITE", fd);
      return(-1);
  }

  if (oftp->mode == WRITE_PIPE){
     return write_pipe(fd, buf, nbytes);
  }

  /** write special files tty0, ttyS0, ttyS1, lp0, fd0 */
  mode = oftp->inodeptr->INODE.i_mode;

  /************************************************************************** 
  if ((mode & 0170000)==0020000 || (mode & 0170000)==0060000){ // C|B special
    device = oftp->inodeptr->INODE.i_block[0];
    major = (device & 0xFF00) >> 8;

    blk = oftp->offset / 1024;
    oftp->offset += 1024;
    return (*devsw[major].dev_write)(device, blk, buf, nbytes);
  }
  *************************************************************************/

  if ((mode & 0170000)==0020000){ // char special
    device = oftp->inodeptr->INODE.i_block[0];
    major = (device & 0xFF00) >> 8;
    return (*devsw[major].dev_write)(device, buf, nbytes);
  }
  if ((mode & 0170000)==0060000){ // block special
    device = oftp->inodeptr->INODE.i_block[0];
    major = (device & 0xFF00) >> 8;
    dev   = (device & 0x00FF);

    blk = oftp->offset / 1024;
 
    if (blk > devtab[dev].size/BLOCK_SIZE){
       printf("blk exceeds size\n");
       return -1;
    }
    oftp->offset += 1024;
    
    return (*devsw[major].dev_write)(device, blk, buf);
  } 

  // non-special file: lock minode first
  ilock(oftp->inodeptr);
    n = mywrite(fd, buf, nbytes);
  iunlock(oftp->inodeptr);

  return n;
}

int mywrite(int fd, char *buf, int nbytes)
{
  int dev, i, j, count, start, remain;
  long size, position;
  MINODE *ip;   OFT *oftp;
  char *cp, *cq;
  ushort ino, blk, lbk, b12, b13;
  u32 dblk;
  struct buf *bp, *dbp;
  u32 *up, *dup;

  oftp = running->res->fd[fd];
  ip = oftp->inodeptr;

  /* compute logical block */

  cq = buf;           /* source buffer */
  count = 0;

  while (nbytes > 0 ){
     lbk = oftp->offset / BLOCK_SIZE;
     start = oftp->offset % BLOCK_SIZE;

     if (lbk < 12){
        blk = ip->INODE.i_block[lbk];
        if (blk == 0){
            blk = ip->INODE.i_block[lbk] = zalloc(ip->dev);
	    if (blk==0){
	       // prints("no more data blocks\n\r");
               return 0;
	    } 
	    ip->INODE.i_blocks += 2;
        }
     }
     else if (lbk >= 12 && lbk < 256 + 12){  /* indirect blocks */
          if (ip->INODE.i_block[12] == 0){
             b12 = ip->INODE.i_block[12] = zalloc(ip->dev);
	     if (b12==0){
	       //prints("no more data blocks\n\r");
                return 0;
	     } 
             bp = getblk(ip->dev,(u16)ip->INODE.i_block[12]);
             up = (u32 *)bp->buf;

             for (i=0; i<256; i++)       /* zero out the indirect block */
                  up[i] = 0;
	    
             //put_block(ip->dev, (u16)ip->INODE.i_block[12], buf12);
             bwrite(bp);
          }
 
	  //get_block(ip->dev, (u16)ip->INODE.i_block[12], buf12);
          bp = bread(ip->dev, (u16)ip->INODE.i_block[12]);
          up = (u32 *)bp->buf;
          
          blk = up[lbk-12];   /* get the blk number */
          if (blk == 0){
             blk = up[lbk - 12] = zalloc(ip->dev);
	     if (blk==0){
	       //prints("no more data blocks\n\r");
               return 0;
	     }
             ip->INODE.i_blocks += 2;
	  }
	  //put_block(ip->dev, (u16)ip->INODE.i_block[12], buf12); 
          bwrite(bp);
     }
     else{                      /* double indirect blocks */
         lbk -= (12 + 256);
         if (ip->INODE.i_block[13] == 0){
	     b13 = ip->INODE.i_block[13] = zalloc(ip->dev); // new block[13]
	     if (b13==0){
	       //prints("no more data blocks\n\r");
               return 0;
	     }
             bp = getblk(ip->dev, b13);
             up = (u32 *)bp->buf;

             for (i=0; i<256; i++)     /* zero out zone[13] block */
                 up[i] = 0;
             //put_block(ip->dev, (u16)ip->INODE.i_block[13], buf12);
             bwrite(bp);
         } 
         //get_block(ip->dev, (u16)ip->INODE.i_block[13], buf12);
         dbp = bread(ip->dev, (u16)ip->INODE.i_block[13]);
         dup = (u32 *)dbp->buf;

         /* get the double indirect blk# */     
         dblk = dup[lbk/256];       /* compute the dblk number */

         if (dblk == 0){
             dblk = dup[lbk/256] = zalloc(ip->dev);
	     if (dblk==0){
	       //prints("no more data blocks\n\r");
               return 0;
             ip->INODE.i_blocks += 2; 
	     }
             bp = getblk(ip->dev, dblk);
             up = (u32 *)bp->buf;
 
             for (i=0; i<256; i++)
	       up[i] = 0;  
 
            //put_block(ip->dev, (u16)dblk, buf);   /* zero out the d_block */
             bwrite(bp);

             //put_block(ip->dev, (u16)ip->INODE.i_block[13], buf12); 
             bwrite(dbp);
         }

         //get_block(ip->dev, (u16)dblk, buf12);
         bp = bread(ip->dev, (u16)dblk);
         up = (u32 *)bp->buf;

         if (up[lbk % 256] == 0){
             blk = up[lbk % 256] = zalloc(ip->dev);
	     if (blk==0){
	       //prints("no more data blocks\n\r");
               return 0;
	     }
	     ip->INODE.i_blocks += 2;
             //put_block(ip->dev, (u16)dblk, buf12); /* update the d_block */ 
             bwrite(bp);
         }
     }

     /* all cases come to here : write to the data block blk */
     bp = bread(ip->dev, (u16)blk);      
     
     cp = bp->buf + start;
     remain = BLOCK_SIZE - start;

     while (remain > 0){
           *cp = get_ubyte(cq);
           cp++; cq++;
           nbytes--; remain--;
           oftp->offset++; count++;
           if (oftp->offset > ip->INODE.i_size)
              ip->INODE.i_size++;
           if (nbytes <= 0) break;
     }
     bwrite(bp);
  }

  ip->dirty = 1;       /* mark ip dirty for iput() */

  return(count);
}
