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

#ifdef MK
 #include "../type.h"
 #include "global.c"
 #include "buffer.c"
 #include "util.c"
 #include "mount_root.c"
 #include "alloc_dealloc.c"
 #include "mkdir_creat.c"
 #include "cd_pwd.c"
 #include "rmdir.c"
 #include "link_unlink.c"
 #include "stat.c"
 #include "open_close.c"
 #include "dev.c"
 #include "read.c"
 #include "write.c"
 #include "mount.c"
 #include "touch.c"
#endif

#include "../extern.h"

fs_init(int pno)
{

  int i,j;
  MINODE *rootip, *mip;

  for (i=0; i<NMINODES; i++){
    mip = &minode[i];
    mip->refCount = 0;
    mip->lock.value = 1;
    mip->lock.queue = 0;
  }
 
  for (i=0; i<NMOUNT; i++)
      mounttab[i].busy = 0;

  for (i=0; i<NOFT; i++)
      oft[i].refCount = 0;

  printf("mounting root : ");
    mountroot(pno);
  // printf("ok\n");
}
