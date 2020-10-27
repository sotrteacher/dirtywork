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

// #define FD 2

kmount(char *y, char *z)
{
  int i, ino, fd, dev, spdev, mode, devnum;
  MOUNT *mp;
  SUPER *sp;
  MINODE *ip;
  char temp[32];
  char filesys[32], mountdir[32], *u, c;
  struct buf *bp;

  filesys[0] = mountdir[0] = 0;
  
  // get filsys and mountdir from Umode
  if (y){
    u = filesys;
    c = get_byte(running->uss, y);
    while (c){
      *u = c;
      u++; y++;
      c = get_byte(running->uss, y);
    }
    *u = 0;
  }
  if (z){
    u = mountdir;
    c = get_byte(running->uss, z);
    while (c){
      *u = c;
      u++; z++;
      c = get_byte(running->uss, z);
    }
    *u = 0;
  }

  //printf("mount %s %s\n", filesys, mountdir);

  if (filesys[0]==0){  /* mount ==> show mounted devices */
      for (mp=&mounttab[0]; mp<&mounttab[NMOUNT]; mp++){
           if (mp->busy){
              ip = mp->mounted_inode;
              printf("%s mounted on %s\n", mp->name, mp->mount_name); 
           }
      }
      return 0;
  }

  if (running->res->uid != 0){
     printf("not SUPER user\n");
     return(-1);
  }


  if (filesys[0] != 0 && mountdir[0] == 0){
     printf("Usage : mount  filesys  dir\n");
     return(-1);
  }

  /* check whether special is already mounted */
  for (mp=&mounttab[0]; mp<&mounttab[NMOUNT]; mp++){
      if (strcmp(mp->name, filesys) == 0 && mp->busy){
          printf("%s already mounted\n", filesys);
             return(-1);
      }
  }

  for (mp=&mounttab[0]; mp<&mounttab[NMOUNT]; mp++){
       if (mp->busy == 0) break;
  }
  if (mp > &mounttab[NMOUNT]){
      printf("no more mount table entry\n");
      return(-1);
  }

  // start with /dev/fd0 only

  spdev = myopen(filesys, 0);   

  if (spdev < 0){
       printf("can't open special file %s\n", filesys);
       return(-1);
  }

  mode = running->res->fd[spdev]->inodeptr->INODE.i_mode;
  devnum = running->res->fd[spdev]->inodeptr->INODE.i_block[0];

  //  printf("spdev=%d  mode=%x devnum=%x\n", spdev, mode, devnum);
  myclose(spdev);

  if ( (mode & 0170000) != 0060000 ){
    printf("%s is not a block special file\n");
    return -1;
  }

  // As of now, assume can only mount /dev/fd0 whose dev# = 2
  // In order to mount HD's other partitions, MUST change these HERE

  if (devnum != 0x0200){
    printf("expecting /dev/fd0\n");
    return -1;
  }    

  // ASSUME it's /dev/fd0; FD's dev#=FD = 2
  /****************************************
  fdrw(0xE6, 1, pbuf);
  sp = (SUPER *)pbuf;
  
  if (sp->s_magic != 0xEF53){
    printf("magic=%x expecting 0xEF53; wrong EXT2 magic\n");
    brelse(bp);
    return -1;
  }
  **************************************/

  bp = bread(FD, 1);
  sp = (SUPER *)bp->buf;
  
  if (sp->s_magic != 0xEF53){
    printf("magic=%x expecting 0xEF53; wrong EXT2 magic\n");
    brelse(bp);
    return -1;
  }

  brelse(bp);
 

  // check mountdir
  if (mountdir[0] == '/')  
      dev = root->dev;
  else 
      dev = running->res->cwd->dev;

  strcpy(temp, mountdir);

  /* getino() may change dev due to mounted inodes */
  ino = getino(&dev, temp);

  if (!ino){
      printf("mount point does not exist\n");
      brelse(bp);
      return(-1);
  }

  ip = iget(dev, ino);             /* get mount point dir's inode */

  if ((ip->INODE.i_mode & 0170000) != 0040000){
     printf("%s is not a directory\n", mountdir);
     iput(ip); 
     return(-1);
  }

  if (ip->mounted || ip->ino == 2){
     printf("%s is already mounted on\n", mountdir);
     
     return(-1);
  }

  bp = bread(FD, 1); //to avoid brelse(bp) many times, was brelse()_ed before

  /* get super block of special */
  mp->ninodes = sp->s_inodes_count;
  mp->nblocks = sp->s_blocks_count;

  ip->mounted = 1;                 /* inode has been mounted on */
  ip->mountptr = mp;               /* inode points at mounttab[i] */ 
  mp->mounted_inode = ip;          /* mounttab points back at inode */
  strcpy(mp->name, filesys);
  strcpy(mp->mount_name, mountdir);
  
  mp->dev = FD;      // for NOW: mount FD only, fd's dev=FD = 2        
  mp->busy = 1;

  brelse(bp);

  // read device's GD to record its BMAP, IMAP and IBLOCK in mouttab[dev]
  bp = bread(FD, 2);  // Assume all FS 1KB BLOCK_SIZE, GD is in block# 2
  gp = (GD *)bp->buf;
  mp->BMAP   = (u16)gp->bg_block_bitmap;
  mp->IMAP   = (u16)gp->bg_inode_bitmap;
  mp->IBLOCK = (u16)gp->bg_inode_table;
  printf("bmap=%d  imap=%d  iblock=%d\n", mp->BMAP, mp->IMAP, mp->IBLOCK);
  brelse(bp);

  // must unlock mounted directory
  // V(&ip->lock);
  iunlock(ip);

  printf("mount : mounted %s on %s\n", filesys, mountdir);
  return(0);
}

extern struct buf buffer[ ];  

kumount(char *y)
{
  MOUNT *mp; 
  MINODE *ip;
  struct buf *bp;

  int i, use_count;
  char *u, c, filesys[32];

  // get filsys and mountdir from Umode
  filesys[0] = 0;
  if (y){
    u = filesys;
    c = get_byte(running->uss, y);
    while (c){
      *u = c;
      u++; y++;
      c = get_byte(running->uss, y);
    }
    *u = 0;
  }
  printf("umount %s\n", filesys);

  if (filesys[0]==0){
    printf("Usage: umount filesys\n");
    return -1;
  }

  if (running->res->uid != 0){
     printf("not SUPER user\n");
     return(-1);
  }

  for (i=0; i<NMOUNT; i++){
      mp = &mounttab[i];
      if ( !strcmp(mp->name, filesys) || !strcmp(mp->mount_name, filesys) )
         break;
  }
  if (i>=NMOUNT){
     printf("%s is not mounted\n",filesys);
     return(-1);
  }

  mp = &mounttab[i];  

  ip = mp->mounted_inode;

  /********** check mounted device BUSY **********/
  if (ip->refCount > 1){
     printf("[%d %d] %d device is busy\n", ip->dev, ip->ino, ip->refCount);
     return(-1);
  }

  use_count = 0;
  for (i=0; i<NMINODES; i++){

    if (minode[i].refCount && minode[i].dev==mp->dev){ 
	printf("[%d %d].ref=%d\n", minode[i].dev, minode[i].ino,
	       minode[i].refCount);
        use_count++;
    }
  }

  if (use_count>0){
      printf("mounted file system %s busy\n", filesys);
      return(-1);
  }

  for (i=0; i<NBUF; i++){
    bp = &buffer[i];
    if (bp->dev==mp->dev && bp->dirty && bp->valid){
      awrite(bp);
      bp->dirty = 0;  // for sure
    }
  }

  ip = mp->mounted_inode;
  ip->mounted = 0;
  ip->mountptr = 0;

  // mounted dir was NOT locked ==> to imput() must pre-dec its lock.value
  //ip->lock.value--;
  ilock(ip);
  iput(ip);

  mp->busy = 0;
  printf("%s umounted\n", filesys);
  return(0);
}  
  
