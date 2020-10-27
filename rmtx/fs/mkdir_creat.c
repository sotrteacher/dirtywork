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
/*** mkdir_creat.c ****/
/*********************** 11-10-2010 ************************************
 did not set ctime, atime, mtime in INODES. Try to incoporate time intp
 MTX's inodes. If using 32-bit seconds since 1970 as in Unix/Linux, then
 need a ctime() program to convert time into calendar form, which is tedious 
 to develop/port. Instead, decided to use BIOS get date and get time calls
 to get the real-timer setting when MTX starts.
 Use BIOS time calls to set time fields in file's INODEs.

 Need 8 bytes to store a complete time infor.

 Will ignore atime and dtime ==> leave them blank for now
  
   ctime = |centry|year|month|day|; in BCD form
   mtime = |bhr   |bmin|bsec |0  |; in BCD form

typedef struct ext2_inode {
	u16	i_mode;	
	u16	i_uid;	
	u32	i_size;	
	u32	i_atime;	// Access time
	u32	i_ctime;	// Creation time
	u32	i_mtime;	// Modification time
	u32	i_dtime;	// Deletion Time
	u16	i_gid;		
	u16	i_links_count;	
	u32	i_blocks;	
	u32	i_flags;	
        u32     dummy;
	u32	i_block[15];    
        u32     pad[7];         // pad to 128-byte inode size
} INODE;
************************************************************************/
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h"

int get_param(char *y, char *z)
{
    char c;
    c = get_ubyte(y);

    while (c){
          *z = c;
           z++; y++;
           c = get_ubyte(y);
    }
    *z = c;
}

// return 0 if pathname == multiple / or . or .. 
int check_path(char *pathname)
{
  char *cp;
  int found;

  /** can handle . and .. by search for name
  if (!strcmp(pathname, ".") || !strcmp(pathname, ".."))
    return 0;
  **/

  found = 0;
  cp = pathname;
  while(*cp){
    if (*cp != '/')
      found++;
    cp++;
  }
  return found;
}

int kmkdir(char *y, int z)
{
 MINODE *mip;
 int r, dev;
 u32 ino;
 char pathname[32], parent[16], child[16];

 get_param(y, pathname);

 if (pathname[0] == '/') 
     dev = root->dev;
 else
     dev = running->res->cwd->dev;

 strcpy(child, basename(pathname));
 strcpy(parent, dirname(pathname));

 // printf("parent=%s child=%s\n", parent, child);
 if (child[0]==0){
    printf("can't mkdir /\n");
    return -1;
 }
 if (strcmp(child, "/")==0){
    return -1;
 }

 ino = getino(&dev, parent);
 if (!ino){
    return(-1);
 }
 mip = iget(dev,ino);

 if ((mip->INODE.i_mode & 0170000) != 0040000){
    printf2u("not DIR\n");
    iput(mip);
    return(-1);
 }

 r = mymkdir(mip, child);

 mip->INODE.i_links_count++;	/* inc parent's links count */
 iput(mip);
 return(r);
} 


int mymkdir(MINODE *pinode, char *name)
{
  int i, j; 
  u16 inumber, znumber, ino, blk;

  MINODE *mip; 
  DIR  *dp;
  char *cp;
  int dev;  ushort pz;


  struct buf *bp;

  /* first check whether name already exists in parent */
  if (ksearch(pinode, name)){
      printf2u("%s already exists\n", name);
      return(-1);
  } 

  dev = pinode->dev;
  
  inumber = ialloc(dev);
  znumber = zalloc(dev);

  if (inumber == 0 || znumber == 0){
    //printf2u("no more inode or disk block\n");
    return(-1);
  }

  mip = iget(dev, inumber);

  mip->INODE.i_mode = 0x41ED;		/* DIR and permissions */
  mip->INODE.i_uid  = running->res->uid;		/* Owner Uid */
  mip->INODE.i_gid =  running->res->gid;		/* Group Id */
  mip->INODE.i_size = 1024 ;		/* Size in bytes */

  mip->INODE.i_links_count = 2;	/* Links count */

  // set ctime mtime fields 8 bytes to |centry|year|month|day|bhr|bmin|bsec|0|
  set_time(mip);
  mip->INODE.i_atime=mip->INODE.i_dtime = 0;

  mip->INODE.i_blocks = 2;     	/* Blocks count in 512-byte blocks */
  mip->dirty = 1;                /* mark dirty */
  for (i=0; i<15; i++)
    mip->INODE.i_block[i] = 0;
  mip->INODE.i_block[0] = znumber; 

  iput(mip);

  //get_block(dev, (u16)znumber, buf);

  bp = bread(dev, (u16)znumber);

  dp = (DIR *)bp->buf;
  cp = bp->buf; 

  dp->inode = inumber;		/* Inode number */
  strcpy(dp->name, ".");        /* File name */
  dp->name_len = 1;		/* Name length */
  dp->rec_len = 12;		/* Directory entry length */

  /*********************************************************************** 
    ext2 dir entries are variable length (min=12 bytes). Last entry's 
    rec_len must be set to the remaining length of the block. 
  ************************************************************************/

  cp += dp->rec_len;            /* advance by rec_len */
  dp = (DIR *)cp;

  dp->inode = pinode->ino;      /* Inode number */
  dp->name_len = 2;		/* Name length */
  strcpy(dp->name, "..");       /* File name */
  dp->rec_len = BLOCK_SIZE - 12;/* last DIR entry length */

  //put_block(dev, (u16)znumber, buf);
  bwrite(bp);

  /* Finally, enter name into parent directory */

  enter_name(pinode, inumber, name);
}

int ocreat(char *pathname)
{
   MINODE *mip;
   int r, dev;
   u32 ino;
   char temp[32], parent[16], child[16];

   if (pathname[0] == '/') 
     dev = root->dev;
   else
     dev = running->res->cwd->dev;

   strcpy(child, basename(pathname));
   strcpy(parent, dirname(pathname));

   if (strcmp(child, "/")==0){
      //printf("can't creat\n");
      return -1;
    }

    ino = getino(&dev, parent);

    if (!ino){
       //printf("invalid dir pathname\n");
       return(-1);
    }

    mip = iget(dev,ino);     
    if ((mip->INODE.i_mode & 0040000) != 0040000){
       //printf("not a dir\n");
       iput(mip);
       return(-1);
    }

    r = mycreat(mip, child);
    iput(mip);
    return(r);
}


int kcreat(char *y, int z)
{
   char pathname[32];

   get_param(y, pathname);

   return ocreat(pathname);
}


int mycreat(MINODE *parent, char *name)
{
  int i, j, b, dev;
  MINODE *mip; 
  DIR    *dp;
  u32 ino, inumber;

  /* first check whether name already exists in parent */
  if (ksearch(parent, name)){
    //prints("creat : already exists in dir\n\r");
      return(-1);
  } 

  dev = parent->dev;
  
  inumber = ialloc(dev);
  if (inumber == 0){
    return(-1);
  }

  mip = iget(dev, inumber);

  mip->INODE.i_mode = FILE_MODE;
  mip->INODE.i_uid  = running->res->uid;
  mip->INODE.i_gid  = running->res->gid;
  mip->INODE.i_size = (long)0;
  // set [ctime|mtime] to date and time
  set_time(mip);  
  mip->INODE.i_atime = mip->INODE.i_dtime = 0;

  mip->INODE.i_blocks = 0;
  mip->INODE.i_links_count = 1;
  for (i=0; i<15; i++) 
       mip->INODE.i_block[i] = 0;
  mip->dirty = 1;  /* mark dirty */

  iput(mip);

  /* Finally, enter name into parent directory */

  return(enter_name(parent, (u16)inumber, name));

}  

/******************** old kcreat code ******************************
int kcreat(y,z) char *y; int z;
{
 MINODE *mip;
 int r, dev;
 u32 ino;
 char pathname[32],temp[32],saveParent[16],parent[16],child[16];
 struct buf *bp;

 get_param(y, pathname);

 
 // printf("kcreat %s", pathname);
 if (pathname[0] == '/') 
     dev = root->dev;
 else
     dev = running->cwd->dev;

 strcpy(temp, pathname);
 strcpy(child, basename(temp));
 strcpy(temp, pathname);
 strcpy(parent, dirname(temp));

 if (strcmp(child, "/")==0){
   //printf2u("can't creat /\n");
   return -1;
 }

 strcpy(saveParent, parent);
 ino = getino(&dev, parent);

 if (!ino){
   //printf2u("invalid pathname\n");
    return(-1);
 }

 mip = iget(dev,ino);     
 if ((mip->INODE.i_mode & 0170000) != 0040000){
   //printf2u("not a dir\n");
    iput(mip);
    return(-1);
 }

 r = mycreat(mip, child);
 iput(mip);
 return(r);
} 


int mycreat(pinode,name) MINODE *pinode; char *name;
{
  int i, j, b, dev;
  MINODE *ip; 
  DIR    *dp;
  u32 ino, inumber;
  struct buf *bp;

  // first check whether name already exists in parent 
  if (ksearch(pinode, name)){
    //printf2u("creat : %s already exists in dir\n", name);
      return(-1);
  } 
  dev = pinode->dev;

  inumber = ialloc(dev);
  if (inumber == 0){
    return(-1);
  }

  ip = iget(dev, inumber);

  ip->INODE.i_mode = FILE_MODE;
  ip->INODE.i_uid  = running->uid;
  ip->INODE.i_gid  = running->gid;
  ip->INODE.i_size = (long)0;
  ip->INODE.i_atime=ip->INODE.i_ctime=ip->INODE.i_mtime = 0;
  ip->INODE.i_blocks = 0;
  ip->INODE.i_links_count = 1;
  for (i=0; i<15; i++) ip->INODE.i_block[i] = 0;
  ip->dirty = 1;  // mark dirty

  iput(ip);

  // Finally, enter name into parent directory 

  return(enter_name(pinode, (u16)inumber, name));

}  
***********************************************************************/



/*********** code using old insert/delete DIR entry algorithms *********
// EXT2 DIR entries: Each DIR entry has name_len and rec_len.
// (1). rec_len = name_len+8 raised to the next multiple of 4 bytes.
//      So,       ideal_len = [(name_len + 8 + 3)/4] * 4
// (2). When delete an entry, its rec_len is added to the previous entry's
//      rec_len ==> making the previous rec_len longer.
// (3). (2) ==> when enter a name into DIR, look for an entry whose
//      rec_len - ideal_len >= new entry's ideal_len. Then, reduce this entry's
//      rec_len to its ideal_len and add the new entry in the remaining space.
//      The new entry's rec_len = remain = this entry's (rec_len - ideal_len).
// (4). When scan a DIR block, must check for block ending; allocate new
//      data block if needed.

int enter_name(MINODE *pinode, u16 inumber, char *name)
{
  u16 i, j, size, found, count, blk;
  u16 ideal_len, need, remain;
  DIR  *dp; 
  char *cp;
  MINODE *mip = pinode;
  struct buf *bp;

  size = pinode->INODE.i_size;
  found = 0;
  count = 0;
  need = ((strlen(name)+8+3)/4)*4;

  // parent dir has up to 12 direct blocks
  for (i=0; i<12; i++){
    blk = pinode->INODE.i_block[i];
    if (blk==0){
        blk = pinode->INODE.i_block[i] = zalloc(pinode->dev);
        
        pinode->INODE.i_size += BLOCK_SIZE;
        //memset(buf, 0, 1024);
        bp = getblk(pinode->dev, (u16)pinode->INODE.i_block[i]);
        dp = (DIR *)bp->buf;
        dp->inode = inumber;
        dp->name_len = strlen(name);
        dp->rec_len = BLOCK_SIZE;
        strncpy(dp->name, name, dp->name_len);
        break;
    }
   
    //get_block(pinode->dev, (u16)pinode->INODE.i_block[i], buf);

    bp = bread(pinode->dev, (u16)pinode->INODE.i_block[i]);
    dp = (DIR *)bp->buf;
    cp = bp->buf;

    while (count < mip->INODE.i_size && cp < &bp->buf[1024]){

        ideal_len = ((dp->name_len + 8 + 3)/4)*4;

	// i_block[1] ,... may have a leading entry with ino==0
        if (dp->inode == 0 && dp->rec_len >= need){

            dp->inode = inumber;
	    strncpy(dp->name, name, strlen(name));
 	    dp->name_len = strlen(name);
            found = 1;
            break;
	}

        if (dp->rec_len != ideal_len){
          remain = dp->rec_len - ideal_len;
          if (remain >= need){
	      dp->rec_len = ideal_len;  // trim this entry's len
            cp += dp->rec_len;          // advance cp;

            dp = (DIR *)cp;
            dp->inode = inumber;
            dp->name_len = strlen(name);
            dp->rec_len = remain;
            strncpy(dp->name, name, strlen(name));
            found = 1;
            break; 
	  }
	}
        cp += dp->rec_len;
        count += dp->rec_len;
        dp = (DIR *)cp;
    }
    if (found) 
	break;
    brelse(bp);
  }

  //put_block(pinode->dev, (u16)pinode->INODE.i_block[i], buf);
  bwrite(bp);

  pinode->dirty = 1;
  return(0);
}
********************************************************************/


 /***** code using new insert/delete DIR entry algorithms *******/

struct buf *getblk();

// KCW: enter_name: always enter as last entry in a block
int enter_name(MINODE *pinode, u16 inumber, char *name)
{
  u16 i, j, blk;
  u16 ideal, need, remain;
  DIR  *dp; 
  char *cp, c;
  MINODE *mip = pinode;
  struct buf *bp;

  //printf("enter_name by NEW algorithm\n");

  need = 4*((strlen(name)+8+3)/4);
  /* parent dir has upto 12 direct blocks */
  for (i=0; i<12; i++){  // assume no non-empty blocks
     blk = pinode->INODE.i_block[i];
     //printf("INSERT: %d%d ", i, blk);
     if (blk==0){
        blk = pinode->INODE.i_block[i] = zalloc(pinode->dev);
        pinode->INODE.i_size += BLKSIZE;
        printf("allocated new DIR block [%d%d]\n", i, blk);

        // new block, no need to bread() it in; just get a buffer
        bp = getblk(pinode->dev, (u16)pinode->INODE.i_block[i]);

        dp = (DIR *)bp->buf;
        dp->inode = inumber;
        dp->name_len = strlen(name);
        dp->rec_len = BLKSIZE;
        strncpy(dp->name, name, dp->name_len);
        bwrite(bp);     // bp modified, must write back
        pinode->dirty = 1;
	printf("inserted first entry %s%d\n", dp->name, dp->rec_len);
        return 1;
     }
     // block exist: read block into bp->buf 
     bp = bread(pinode->dev, (u16)pinode->INODE.i_block[i]);

     dp = (DIR *)bp->buf;
     cp = bp->buf;

     while (cp + dp->rec_len < &bp->buf[BLKSIZE]){// step to last entry in block
        c = dp->name[dp->name_len];
        dp->name[dp->name_len] = 0;
        //printf("%s ", dp->name);
        dp->name[dp->name_len] = c;

        cp += dp->rec_len;
        dp = (DIR *)cp; 
     }

     // dp points at last entry in block
     ideal = 4*((dp->name_len + 8 + 3)/4);
     remain = dp->rec_len - ideal;
     if (remain >= need){
        dp->rec_len = ideal;
        cp += ideal;
        dp = (DIR *)cp;
        dp->inode = inumber;
        dp->name_len = strlen(name);
        dp->rec_len = remain;
        strncpy(dp->name, name, strlen(name));
	//printf("\nremLen=%d\n", dp->rec_len - need);
        bwrite(bp);            // bp modified, must write back
        return 1;
     }
     brelse(bp);               // bp not modified, just release it         
  }
  return 1;
}




