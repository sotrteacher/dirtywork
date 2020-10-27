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

int mountroot(int pno)
{
  int dev;
  MOUNT *mp;
  SUPER *sp;
  MINODE *ip;
  struct buf *bp;
  char rootdev[16];
  
  dev = pno;  
  strcpy(rootdev, "/dev/hda");
  rootdev[strlen(rootdev)+1] = 0;
  rootdev[strlen(rootdev)] = pno+'0'; // assume pno <= 9
  //printf("dev=%d  rootdev=%s\n", dev, rootdev); 

  // get super block of rootdev 
  bp = bread(dev, 1);
  
  sp = (SUPER *)bp->buf;

  mp = &mounttab[0];

  /* copy super block info into mounttab[0] */
  mp->ninodes = sp->s_inodes_count;
  mp->nblocks = sp->s_blocks_count;
  
  sp->s_free_blocks_count;
  sp->s_free_inodes_count;

  mp->dev = dev;        // with dev as index, this is redundant but keep it  
  mp->busy = 1;

  strcpy(mp->name, rootdev);
  strcpy(mp->mount_name, "/");
  brelse(bp);

  // read root_dev's GD to record its BMAP, IMAP and IBLOCK in mouttab[0]
  bp = bread(dev, 2);  // Assume all FS 1KB BLOCK_SIZE, GD is in block# 2
  gp = (GD *)bp->buf;
  mp->BMAP   = (u16)gp->bg_block_bitmap;
  mp->IMAP   = (u16)gp->bg_inode_bitmap;
  mp->IBLOCK = (u16)gp->bg_inode_table;
  printf("bmap=%d  imap=%d  iblock=%d\n", mp->BMAP, mp->IMAP, mp->IBLOCK);
  brelse(bp);

  /***** call iget(), which inc the Minode's refCount ****/
  root = iget(dev, 2);          // get root inode
  mp->mounted_inode = root;
  root->mountptr = mp;

  // MUST unlock root->lock
  //printf("mount_root  : root->lock=%d ", root->lock.value);
    V(&root->lock);
  //printf("After unlock: root->lock=%d\n", root->lock.value);
  printf("mount : /dev/hda%d mounted on / OK\n", dev);
 
  return(0);
} 

// compile FS/fs.c separately, must repeat buf tyep here

extern struct buf buffer[ ];

int ksync()
{ 
  MINODE *mip; INODE *ip;
  int i, ref, count;
  struct buf *bp;

  printf("KERNEL: sync file system ....\n");
  printf("Active minodes;\n");
  for (i=0; i<NMINODES; i++){
      mip = &minode[i];
      ip = &mip->INODE;
      if (mip->refCount){
         printf("[%d%d] ref=%d lock=%d\n",
               mip->dev, mip->ino, mip->refCount, mip->lock.value);
      }
  }

  count = 0;
  for (i=0; i<NMINODES; i++){
      mip = &minode[i];
      ip = &mip->INODE;
      if (mip->refCount > 0 && mip->dirty){
        printf("[%d%d] ref=%d lock=%d\n",
                mip->dev, mip->ino, mip->refCount, mip->lock.value);
	ref = mip->refCount;
        mip->refCount = 1;

         P(&mip->lock);
          iput(mip); /* force out */
        mip->dirty = 0;
        mip->refCount = ref;
        count++;
      }
  }
  printf("%d %s ", count, "minodes +");

  // must flush block buffers also
  count = 0;
  for (i=0; i<NBUF; i++){
    bp = &buffer[i];
    if (bp->valid && bp->dirty){
       bp = getblk(bp->dev, bp->blk);  // must lock the buffer
       awrite(bp);
       bp->dirty = 0;
      count++;
    }
  }
  printf("%d buffers flushed\n", count);

  return 0;
}
