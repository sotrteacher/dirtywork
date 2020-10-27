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

int krmdir(char *y, int z)
{
 MINODE *ip, *pip;  
 DIR *dp;
 char *cp;
 u16 i, j, r, ino, dev, count, done;
 u16 me, myparent;
 u16 size, remain, non_empty;
 char pathname[32], child[16];
 struct buf *bp;

 get_param(y, pathname);

 /* first, get its ino and inode */
 if (pathname[0] == '/') 
     dev = root->dev;
 else
     dev = running->res->cwd->dev;

 ino = getino(&dev, pathname);

 if (!ino){
   //printf2u("invalid pathname\n");
    return(-1);
 }

 if (ino == 2){
   //printf2u("mouted filesys; umount first\n");
    return(-1);
 }
 
 ip = iget(dev, ino);

 /* check for DIR type */
 if ((ip->INODE.i_mode & 0040000) != 0040000){
   //printf2u("not a direcory\n");
    iput(ip);  return(-1);
 }
 
 /* check ownership */

 if (running->res->uid != 0 && running->res->uid != ip->INODE.i_uid){
    printf2u("not owner\n"); iput(ip);  return(-1);
 }

 /****** MUST CHECK FOR MOUNTING AS WELL AS cwd */
 if (ip == running->res->cwd){
    printf2u("can't rmdir cwd\n"); 
    iput(ip); return(-1);
 }

 if (ip->refCount > 1){
    printf2u("dir busy\n");
    iput(ip); return(-1);
 }  

 /* then check whether it's empty */
 // link > 2 OR do a search

 if (ip->INODE.i_links_count > 2){
   printf2u("DIR not empty\n");
   iput(ip); return -1;
 }

 size = ip->INODE.i_size;  
 count = non_empty = 0;
 for (i=0; i<12; i++){
      if (ip->INODE.i_block[i]==0)
	break;
      //get_block(ip->dev, (u16)ip->INODE.i_block[i], buf);
      bp = bread(ip->dev, (u16)ip->INODE.i_block[i]);
      dp = (DIR *)bp->buf;
      cp = bp->buf;

      while (count < ip->INODE.i_size && cp < &bp->buf[1024]){
        if (dp->inode)
	  non_empty++;
        if (non_empty > 2) break;

        count += dp->rec_len;
        cp += dp->rec_len;
        dp = (DIR *)cp;
      }

      brelse(bp);

      if (non_empty > 2) break;
 }

 if (non_empty > 2){
     printf2u("dir not empty\n");
     iput(ip); return(-1);
 }

 /* dir is empty, ok to remove */

 /* get parent's ino and inode */
 myparent = findino(ip, &me);
 pip = iget(ip->dev, myparent);

 strcpy(child, basename(pathname));

 /* remove child from parent directory */
 rm_child(pip, me, child);


 /* deallocate its block and inode */
 for (i=0; i<12; i++){
   if (ip->INODE.i_block[i]==0)
     continue;
   zdalloc(ip->dev, (u16)ip->INODE.i_block[i]);
 }
 idalloc(ip->dev, (u16)ip->ino);
 //ip->refCount = 0;
 iput(ip);

 pip->INODE.i_links_count--;   // dec parent's link count by 1
 pip->dirty = 1;   // parent dirty
 iput(pip);

 return(0);
}

/********** Code using old delete DIR entry algorithm ************
int rm_child(MINODE *parent, u16 me, char *child)
{
  int i, j, done, count;
  DIR  *dp, *pp;
  char *cp, c;
  MINODE *mip = parent;
  struct buf *bp;

  // remove child name from parent directory
  done = 0;
  count = 0;

  for (i=0; i<12; i++){
      if (parent->INODE.i_block[i]==0)
          break;
      //get_block(parent->dev, (u16)parent->INODE.i_block[i], buf);
      bp = bread(parent->dev, (u16)parent->INODE.i_block[i]);
      pp = dp = (DIR *)bp->buf;
      cp = bp->buf;

      while (count < mip->INODE.i_size && cp < &bp->buf[1024]){
 
       //***** KCW: when unlink, must go by name, not by ino ********
        c = dp->name[dp->name_len];      // get char at end of name
        dp->name[dp->name_len] = 0;
        
        if (strcmp(dp->name, child)==0){ // found child name
           dp->inode = 0;
	   // if not the first entry absorb its rec_len to previous entry
           if (pp != dp)
              pp->rec_len += dp->rec_len;
           dp->name[dp->name_len] = c;   // restore that char
           bwrite(bp);                   // bp is modified; must wirte back 
           return 0;  // OK
        }
        dp->name[dp->name_len] = c;      // restore that char
        pp = dp;                         // pp follows dp
        cp += dp->rec_len;               // advance cp;
        dp = (DIR *)cp;
      }

      brelse(bp);
  }

  // put_block(parent->dev, (u16)parent->INODE.i_block[i], buf);
  //bwrite(bp);

  return -1;  // BAD
}
***************************************************************/


/********** Code using new delete DIR entry algorithm ********/

// remove child name from parent directory 
int rm_child(MINODE *parent, u16 me, char *child)
{
  int i, j, del_len; 
  DIR  *dp, *pp;
  char *cp, *cq, c; 
  MINODE *mip = parent;
  struct buf *bp;
  //printf("rmchild by NEW algorithm\n");

  for (i=0; i<12; i++){   // ASSUME: parent DIR has only 12 direct blocks
      //printf("rm: i=%d%d\n", i, (u16)parent->INODE.i_block[i]);
      if (parent->INODE.i_block[i]==0)
          break;

      bp = bread(parent->dev, (int)parent->INODE.i_block[i]);
      pp = dp = (DIR *)bp->buf;
      cp = bp->buf;
      
      //showentries(cp);
      
      while (cp < &bp->buf[BLKSIZE]){
        c = dp->name[dp->name_len];      // get char at end of name
        dp->name[dp->name_len] = 0;
        
        if (strcmp(dp->name, child)==0){ // found child name
           dp->name[dp->name_len] = c;   // restore that char
	   del_len = dp->rec_len;

           // if first and only entry in block
           if (pp == dp && dp->rec_len == BLKSIZE){ // only entry in block
               printf("ONLY entry in block: dp rec_len=%d\n", dp->rec_len);
               printf("deallocate %d:blk=%d\n", i,parent->INODE.i_block[i]);
               zdalloc(parent->dev, parent->INODE.i_block[i]);
               parent->INODE.i_size -= BLKSIZE;
               parent->dirty = 1;
               // move trailing blocks forward
               for (j=i+1; j<12; j++)
                   parent->INODE.i_block[j-1] = parent->INODE.i_block[j];
               brelse(bp);   // bp not modified, just release it
               return 1;
	   }
           // if last entry in block: absorb rec_len into predecessor
           cq = (char *)dp;
           if (cq + dp->rec_len >= &bp->buf[BLKSIZE]){
  
	     //printf("LAST entry in block [%d %s] ", dp->rec_len, dp->name);
	     //printf("pp is [%d %s]\n", pp->rec_len, pp->name);
	      pp->rec_len += dp->rec_len;
          }
	  else{  // entry in middle of block
             if (dp->rec_len != BLKSIZE){  // must have trailing entries
	        last_entry(bp->buf, del_len);
                memcpy(cp, cp+dp->rec_len, (&bp->buf[BLKSIZE]-cp));
	     }
	  }
	  bwrite(bp);                    // bp modified; must wirte back
          return 1;                      // OK
	}
        dp->name[dp->name_len] = c;      // restore that char
        pp = dp;                         // pp follows dp
        cp += dp->rec_len;               // advance cp;
        dp = (DIR *)cp;
      }
      brelse(bp);                        // bp not modified, just release it
  }
  return -1;                             // BAD
}

// step to last entry, add del_len to rec_len
int last_entry(char *buf, int del_len)
{
  char *cp = buf;
  DIR  *dp = (DIR *)cp;

  //printf("last_entry del_len=%d\n", del_len);
  while (cp + dp->rec_len < &buf[BLKSIZE]){
    //printf("%d %d %d %s\n", (u16)dp->inode,dp->rec_len,dp->name_len,dp->name);
     cp += dp->rec_len;
     dp = (DIR *)cp;
  }
  //printf("last entry=[%d%d] ",  (u16)dp->inode, dp->rec_len);
  dp->rec_len += del_len;
  //printf("last entry=[%d%d]\n", (u16)dp->inode, dp->rec_len);
  return 1;
} 

int showentries(char *buf)
{
  char *cp = buf;
  DIR *dp = (DIR *)buf;

  while(cp < &buf[BLKSIZE]){
     printf("[%d%d%d %s] ", (u16)dp->inode,dp->rec_len,dp->name_len,dp->name);
     cp += dp->rec_len;
     dp = (DIR *)cp;
  }
  return 1;
} 
