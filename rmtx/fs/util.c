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

/****************** token() ************************************
  split pathname into tokens, each point by a name[i],
  return number of token strings
******************************************************************/

MINODE *mialloc();
MOUNT  *getmountp();

int token(char *pathname, char *name[ ])
{
 int i;
 char *p, *q;

 p = pathname;
 i = 0;
 while (*p == ' ') p++;  /* skip over blanks */

 while (*p){
     while (*p == '/'){
            *p = 0; p++;  /* zero out leading /s */
     }
     name[i] = p;
     i++;
     while (*p != '/' && *p != 0) 
             p++;                      /* take chars */
     if (*p == 0) break;               /* end of pathname */
     *p = 0; p++;                      /* set this / to 0 */
 }

 return i;

}

char gname[64];

char *dirname(char *s)
{
 char *p;
 strcpy(gname, s);
 p = gname;

 while (*p) p++;
 while (*(p) != '/' && p != gname) p--;
 if (p == gname){
     if (*p == '/') 
         return(slash);
     else
         return(dot);
 }
 *p = 0;
 return(gname);
}      


char *basename(char *s)
{
 char *p;
 strcpy(gname, s);
 p = gname;

 while (*p) p++;
 while (*p != '/' && p != gname) 
       p--;
 if (p == gname){
    if (*p == '/') 
       return(p+1);
    else
       return(p);
 }
 return(p+1);
}

MOUNT *getmountp(int dev)
{
  int i;
  for (i=0; i<NMOUNT; i++)
      if (dev == mounttab[i].dev)
         return(&mounttab[i]);
  return(0);
}

int ilock(MINODE *mip)
{
  P(&mip->lock);
}

int iunlock(MINODE *mip)
{
  V(&mip->lock);
}
  
MINODE *iget(int dev, int ino)
{
 MINODE *mip;
 MOUNT  *mp;
 INODE  *ip;
 u16    i, block, offset;
 char   *cp, *cq;
 struct buf *bp;

 /********************************** 
  if (minode[0].lock.value > 1){
       printf("PANIC: root lock > 1");
  }
 **********************************/

  /* serach in-core minodes first */
  for (i=0; i<NMINODES; i++){
   mip = &minode[i];
   if (mip->refCount > 0){
     if (mip->dev==dev && mip->ino==ino){
        mip->refCount++;   // inc refCount means "reserved" 
        ilock(mip);        // caller may "wait" in lock.queue 
        return(mip);
     }
   }
 }

 mip = (MINODE *)mialloc();       // refCount = 1 by mialloc()

 ilock(mip);                      //  try to lock minode; caller may pause

 // mip->lock.value = 1; mip->lock.queue = 0; 
 mip->dev = dev; mip->ino = ino;

 mp = (MOUNT *)getmountp(dev);

 block = mp->IBLOCK + ((ino-1) / 8);

 offset= (ino-1) % 8;
 
 //get_block(dev, block, buf);   

 bp = bread(dev, block); // caller may "pause" ==> another proc may run
                         // but minode is marked as (dev,ino) ==> no dups
                         // minode is locked ==> no one else can use it until
                         // this caller "unlocks"
  
 /* copy inode to minode */

 ip = (INODE *)bp->buf + offset;

 mip->INODE = *ip;

 /*  10-29-8:
     KCW must NOT rseet refCount to 1 because while loading
     INODE by bread(), other proc may find THIS mip and inc 
     refCount to > 1 already. Rest to 1 ==> disaster 
 */
 /**********************************
 mip->dev = dev; mip->ino = ino;
 mip->refCount = 1;
 mip->mounted = 0;
 mip->dirty = 0;
 mip->mountptr = 0;
 **********************************/
 brelse(bp);

 return(mip);
}

int valid(MINODE *mip)
{
  if (mip->refCount < 0){
     printf("PANIC %d [%d%d]refCount<0\n", running->pid,mip->dev,mip->ino);
     kgetc();
  }
}

iput(MINODE *mip)
{
 u16  i, block, offset;
 int dev,ino; 
 char *cp, *cq;
 MOUNT *mp;
 INODE *ip;
 struct buf *bp;

 if (mip==0) 
     return;

 if (mip->refCount == 0){
    printf("PANIC %d [%d%d]refCount==0 lock=%d\n", 
       running->pid,mip->dev,mip->ino, mip->lock.value);
    mip->refCount = 1;    
 } 

 if (mip->refCount > 1){  // cannot dec refCount first because if refCount=0
   goto out;              // another proc may think this minode if FREE 
 }
 // come to here : refCount==1 
 if (mip->dirty == 0)   goto out;

 /*********************** KCW : 10-31-08 ******************************
  Even with mip lock semaphore, race condition still occured when 
       cat f | grep print
  ==> one of the proc will fail due to an minode's refCount < 0 ===>
  occur only in pipe operation, not when child sh is only one proc to
  exec.==> use a mutex in exec cured the race but not a true solution.
  
  Traced to this race: when iput(mip), if dec refCount first to 0 and dirty
  must write back IONDE. while bread(), another proc sees this minode already
  FREE, proceeds to use it to load a different INODE.
 **********************************************************************/

 // invalid minode to prevent from being found by other proc during write back
 // keep refCount > 0 until write inode back to disk
 dev = mip->dev; 
 ino = mip->ino;

 //mip->dev = mip->ino = -1;

 /* write back */
 
 mp = (MOUNT *)getmountp(mip->dev);

 block =  ((ino - 1) / 8) + mp->IBLOCK;
 offset =  (ino - 1) % 8;

 /* first get the block containing this inode */
 // get_block(mip->dev, block, buf);

 bp = bread(dev, block); // this is a big time gap many things can happen
                         // while blocked for data valid
                         /** but minode is still locked and refCount > 0 
                             other procs needing this INODE will find it as
                             existing and P(lock) to wait in lock.queue ==>
                             other procs will not load the same INODE again but
                             waited for the writing out done ********/
  
 ip = (INODE *)bp->buf + offset;
 *ip = mip->INODE;

 //put_block(mip->dev, block, buf);

 bwrite(bp);
 
 //mip->lock.value = 1; mip->lock.queue = 0;

 // printf("write back [%d%d]\n", dev, ino);
 
 out:
   mip->refCount--; 
   iunlock(mip);
   /*
   if (mip==root){
     printf("%d iput:root ref=%d lock=%d\n", 
            running->pid, mip->refCount,mip->lock.value);
   }
   */
} 

u16 ksearch(MINODE *mip, char *name)
{
   int i, count, nlen; 
   char *cp;
   DIR *dp; 
   struct buf *bp;

   count = 0;
   nlen = strlen(name);

   /**********  search for a file name ***************/
   for (i=0; i<12; i++){ /* search direct blocks only */
        if (mip->INODE.i_block[i] == 0) 
           return 0;
        bp = bread(mip->dev, (u16)mip->INODE.i_block[i]);
        dp = (DIR *)bp->buf;
        cp = bp->buf;

        while (count < mip->INODE.i_size && cp < &bp->buf[1024]){
          if (dp->name_len == nlen){  // only if same length  
	    if (strncmp(dp->name, name, nlen)==0){
	        brelse(bp);
                return (u16)dp->inode;
            }
	  }

          cp += dp->rec_len;
          dp = (DIR *)cp;
        }
        brelse(bp);
        count += 1024;    /* should be BLOCK_SIZE */
   }
   return(0);
}


int findmyname(MINODE *parent, u16 myino, char *myname)
{
 int i,j, count;
 unsigned long size;
 char   *cp;  
 DIR    *dp;
 MINODE *mip = parent;
 struct buf *bp;

 size = parent->INODE.i_size;
 count = 0;

 /**********  search for a file name ***************/
 for (i=0; i<12; i++){ /* search direct blocks only */

     if (mip->INODE.i_block[i] == 0) 
           return -1;

     //get_block(mip->dev, (u16)mip->INODE.i_block[i], buf);

     bp = bread(mip->dev, (u16)mip->INODE.i_block[i]);
     dp = (DIR *)bp->buf;
     cp = bp->buf;

     while (count < mip->INODE.i_size && cp < &bp->buf[1024]){
       if ((u16)dp->inode == myino){
          strncpy(myname, dp->name, dp->name_len);
          myname[dp->name_len] = 0;
          brelse(bp);
          return 0;
       }
       count += dp->rec_len;
       cp += dp->rec_len;
       dp = (DIR *)cp;
     }
     brelse(bp);
 }
 return(-1);
}

ushort findino(MINODE *mip, u16 *myino)
{
  char *cp;   
  DIR  *dp;
  struct buf *bp;
  u16  parentino;

  //get_block(mip->dev, (u16)mip->INODE.i_block[0], buf);
  bp = bread(mip->dev, (u16)mip->INODE.i_block[0]);
  cp = bp->buf; 
  dp = (DIR *)bp->buf;

  *myino = (u16)dp->inode;
  cp += dp->rec_len;
  dp = (DIR *)cp;
  parentino = (u16)dp->inode;

  brelse(bp);

  return parentino;  // no parent ino
}


u16 getino(int *dev, char *path)
{
  u16 i, ino, me, myparent, olddev, newdev, acc;
  MINODE *mip, *newip;  MOUNT *mp;
  char pathname[64];
  int nnames;
  char *name[16];   // assume path has at most 16 components
  struct buf *bp;

  strcpy(pathname, path);  // use local pathname[ ]; do NOT destroy path[ ]

  if (strcmp(pathname, "/")==0){
     *dev = root->dev;
     return(2);
  }

  if (pathname[0] == '/')
      mip = root; 
  else
      mip = running->res->cwd;

  mip->refCount++;              /* in order to iput() */
 
  // cwd and root are NOT locked
  /*******
  if (mip->ino == 2)
    printf("getino: cwd or root %d %d lock=%d\n",
            mip->dev, mip->ino, mip->lock.value);
  ***********/
  P(&mip->lock);

  nnames = token(pathname, name);  

  if (name[0][0] == 0){
      iput(mip);
      return(mip->ino);
  }

  for (i=0; i<nnames; i++){

      /* check for DIR type AND x permission */

      if ((mip->INODE.i_mode & 0170000) != 0040000){
	  printf("%s is not a DIR\n", name[i-1]);
          iput(mip);   return(0);
      }

      acc = 0;
      if (running->res->uid != 0){ // not super user, check x permission
	if (running->res->uid == mip->INODE.i_uid){ // owner of file
	  if ((mip->INODE.i_mode & 0000100))
	    acc = 1;
	}
        else if ((mip->INODE.i_mode & 0000011))
              acc = 1;

        if (acc==0){
	  printf("access denied\n");
          iput(mip);  return(0);
	}
      }

      if (strcmp(name[i], "..")==0 && mip->dev != root->dev && mip->ino == 2){
         mp = getmountp(mip->dev);

         newip = mp->mounted_inode;

         myparent = findino(newip, &me);
         iput(mip);
         mip = iget(newip->dev, myparent);
         *dev = newip->dev;
         ino = myparent;
         continue;
      }

      ino = ksearch(mip, name[i]);

      if (!ino){
	 //printf2u("%s : no such name\n", name[i]);
         iput(mip);   return(0);
      }

      olddev = mip->dev;

      iput(mip);

      newip = iget(olddev, ino);

      mip = newip;                   /* switch to new inode   */

      /* special processing for mounted inode */
      if (mip->mounted){  /* downward direction */
	mp = mip->mountptr;         // mount table
	newdev = mp->dev;           // new dev from mount table
          *dev = newdev;
          iput(mip);
          mip = iget(mp->dev, 2);   /* switch to mounting root */
          ino = 2;
      }
  }
  iput(mip);
  return(ino);
}   

