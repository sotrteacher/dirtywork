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
/***************************************************************
    KCW: make ext2 filesystem on 1.44MB floppy disk (11-4-97)
***************************************************************/
#include "ucode.c"

struct ext2_super_block *sp;
struct ext2_group_desc  *gp;
struct ext2_inode       *ip;
struct ext2_dir_entry_2 *dp;

#define BLK 1024

char buf[BLK], *cp;
int  fd, i, j, n, r;
int  nblocks, ninodes, ngroups, ngblocks;
int  bmap, imap, inodes_start, inodes_blocks, root_block;
int  free_blocks, free_inodes;
int  used_blocks, used_inodes;


main(int argc, char *argv[])
{
  if (argc < 3){
      printf("Usage mkfs DISKIMAGE  block_number\n");
      exit(1);
  } 
  fd = open(argv[1], O_WRONLY|O_CREAT|O_TRUNC);

  if (fd < 0){
      printf("open %s error\n", argv[1]); 
      exit(1);
  }

  lseek(fd, (long)(BLK*(nblocks-1)), 0);   // seek to last block
  r = write(fd, buf, BLK);                 // write a block of 0
  if (r<0){
    printf("write to block %d failed\n", nblocks);
    exit(3);
  }


  nblocks = atoi(argv[2]);            /* number of blocks */
  if (nblocks > 1440){
      printf("too many blocks for %s\n", argv[1]);
      exit(1);
  }
  ngroups = (nblocks + 8191)/8192;    /* number of groups       (1) */
  ngblocks= (ngroups*32 + 1023)/1024; /* number of group blocks (1) */

  if (argc>3)
    ninodes = atoi(argv[3]);
  else
    ninodes = nblocks/4;                /* default # of inodes */
  ninodes = 8*(ninodes/8);

  inodes_blocks = (ninodes+7)/8;      /* number of inodes blocks */

  bmap = 2 + ngblocks;                /* blocks bitmap after group (3) */ 
  imap = bmap + 1;                    /* inodes bitmap after bmap  (4) */
  inodes_start = imap + 1;            /* inodes blocks after imap  (5) */

  root_block = inodes_start + inodes_blocks;   /* data block of root */

  used_blocks = root_block + 1;       /* number of used blocks */
  used_inodes = 10;                   /* inodes 1 to 10 are reserved */

  free_blocks = nblocks - used_blocks;        /* number of free blocks */
  free_inodes = ninodes - 10;                 /* number of free inodes */

  printf("\n************  mkfs on %s  *************\n", argv[1]);
  printf("nblocks      = %d   ninodes     = %d\n", nblocks, ninodes);
  printf("inodes_start = %d   root_block  = %d\n", inodes_start, root_block);
  printf("free_blocks  = %d   free_inodes = %d\n", free_blocks, free_inodes);
  printf("********************************************\n");

  /* make super block */
  make_super();

  /* make group table */
  make_group();

  printf("making blocks bit map ......\n");
  //make_bitmap(nblocks, used_blocks, bmap);
   make_bmap();

  printf("making inode bit map ......\n");
  //make_bitmap(ninodes, used_inodes, imap);
  make_imap();

  clear_inodes();
  make_root_dir();
  make_root_block();

  close(fd);

  fd = open(argv[1], O_RDONLY);
  print_fs();
  printf("**************  All Done  ******************\n");
}


int make_super()
{
  printf("making super block ......\n");
  memset(buf, 0, BLK);
  sp = (struct ext2_super_block *)buf;

  sp->s_inodes_count = ninodes;                 /* Number of inodes     */
  sp->s_blocks_count = nblocks;                 /* Number of blocks     */
  sp->s_r_blocks_count = 0;                     /* No reserved blocks   */
  sp->s_free_blocks_count = free_blocks;	/* Free blocks count    */
  sp->s_free_inodes_count = free_inodes;	/* Free inodes count    */
  sp->s_first_data_block = 1;   	        /* First Data Block     */
  sp->s_log_block_size = 0;                     /* Block size           */
  sp->s_log_frag_size  = 0;      	        /* Fragment size        */
  sp->s_blocks_per_group = 8192;	        /* Blocks per group     */
  sp->s_frags_per_group  = 8192;                /* Fragments per group  */
  sp->s_inodes_per_group = ninodes;	        /* Inodes per group     */
  sp->s_max_mnt_count = 20;     	        /* Maximal mount count  */
  sp->s_magic = 0xEF53;                         /* ext2 magic signature */  
  put_block(1, buf);
}


int make_group()
{
  printf("making group descriptors ......\n");
  memset(buf, 0, BLK);
  gp = (struct ext2_group_desc *)buf;

  gp->bg_block_bitmap = 3;		/* Blocks bitmap block */
  gp->bg_inode_bitmap = 4;		/* Inodes bitmap block */
  gp->bg_inode_table  = 5;		/* Inodes table block */
  gp->bg_free_blocks_count = free_blocks; /* Free blocks count */
  gp->bg_free_inodes_count = free_inodes; /* Free inodes count */
  gp->bg_used_dirs_count = 1;   	/* Directories count */
  put_block(2, buf);
}

int setbit(char *buf, int bit) // set bit_th bit in char buf[1024] to 1
{
  int i,j;
  i = bit / 8; 
  j = bit % 8;
  buf[i] |= (1 << j);
  return 1;
}  

int clearbit(char *buf, int bit) // clear bit_th bit in char buf[1024] to 0
{
  int i,j;
  i = bit / 8; 
  j = bit % 8;
  buf[i] &= ~(1 << j);
  return 1;
}  

int make_bmap()
{
  // nblocks, used_blocks ==> bmap block
  char buf[BLK];
  int i;
  memset(buf, 0xFF, BLK);   // set all bits to 1

  for (i=used_blocks-1; i<nblocks-1; i++)
    clearbit(buf, i);
  put_block(bmap, buf);
}
  
int make_imap()
{
  // inodes, used_inodes ==> imap block
  char buf[1024];
  int i;
  printf("bmap=%d imap=%d\n", bmap, imap);

  memset(buf, 0xFF, BLK);   // set all bits to 1

  for (i=used_inodes; i<ninodes; i++)
    clearbit(buf, i);
  put_block(imap, buf);
}
  

int make_bitmap(int nbits, int nused, int whereto)
{
  memset(buf, 0xFF, BLK);            /* set all bits to 1 */

  for (i=0; i<nbits/8; i++)           /* clear nbits/8 bytes to 0 */
       buf[i] = 0;
  /*** additional bits to clear = nbits % 8 ****/
  for (j=0; j<(nbits % 8); j++)
       buf[i] &= ~(1 << j);

  for (i=0; i<nused/8; i++)           /* set nused/8 bytes to 1 */
       buf[i] = 0xFF;
  /** additional bits to set = nused % 8 **/
  for (j=0; j<(nused % 8); j++)
       buf[i] |= (1 << j);

  put_block(whereto, buf);
}


int clear_inodes()
{
  int i; char buf[BLK];
  memset(buf, 0, BLK);
  for (i=inodes_start; i<inodes_start+inodes_blocks; i++){
       put_block(i, buf);
  }
}


int make_root_dir()
{
  printf("making root directory ......\n");
  memset(buf, 0, BLK);
  ip = (struct ext2_inode *)buf; 
  ip++;

  ip->i_mode = 0x41ED;		/* File mode */
  ip->i_uid  = 0;		/* Owner Uid */
  ip->i_size = 1024 ;		/* Size in bytes */
  ip->i_gid =  0;		/* Group Id */
  ip->i_links_count = 2;	/* Links count */
  ip->i_blocks = 2;     	/* Blocks count */
  ip->i_block[0] = root_block;  /* Pointers to blocks */
  put_block(inodes_start, buf);
}


int make_root_block()
{ 
  printf("making root data block ......\n");
  memset(buf, 0, BLK);

  dp = (struct ext2_dir_entry_2 *)buf;   
  dp->inode = 2;		/* Inode number */
  strcpy(dp->name, ".");        /* File name */
  dp->name_len = 1;		/* Name length */
  dp->rec_len = 12;		/* Directory entry length */

  /*********************************************************************** 
    ext2 dir entries are variable length (min=12 bytes). Last entry's 
    rec_len must be set to the remaining length of the block. 
  ************************************************************************/
  cp = buf; 
  cp += dp->rec_len;            /* advance by rec_len */
  dp = (struct ext2_dir_entry_2 *)cp;
  dp->inode = 2;		/* Inode number */
  dp->name_len = 2;		/* Name length */
  strcpy(dp->name, "..");       /* File name */
  dp->rec_len = BLK-12;		/* Directory entry length */
  put_block(root_block, buf);
}


int print_bits(c) char c;
{
  char s;
  int i;

  s = 1;
  for (i = 0; i < 8; i++){
      if (c & s) putc('1');
      else       putc('0');
      s = s << 1;
  }
  putc(' ');
}


int print_fs()
{
  int i,j,k;
  int bcount, icount;
  bcount = nblocks;
  icount = ninodes;

  printf("******** Blocks Bitmap ********\n");
  get_block(3, buf);

  for (j=0; j<BLK; j++){
      if (j && j % 8 == 0) 
          printf("\n");
          print_bits(buf[j]);
          bcount -= 8;
          if (bcount < 0) 
             break;
  }

  printf("\n********** Inodes Bitmap ***********\n");
  get_block(4, buf);

  for (j = 0; j < BLK; j++){
      if (j && j % 8 == 0) 
         printf("\n");
      print_bits(buf[j]);
      icount -= 8;
      if (icount < 0) 
         break;
  }
  printf("\n");

}

int get_block(int block, char *buf)
{
   lseek(fd,(long)BLK*block,0);
    read(fd,buf,BLK);
}	

int put_block(int block, char *buf)
{
   lseek(fd, (long)BLK*block,0);
   write(fd, buf, BLK);
}
