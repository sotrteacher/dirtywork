extern PROC proc[ ], *running;
extern PROC *freeList, *tfreeList, *sleepList, *readyQueue;
extern int nproc;
extern int hits, requests; 
extern int color;
extern int HD, org;
extern int goUmode(), SEP;

extern MINODE *root; 
extern SUPER  *sp;
extern GD     *gp;
extern INODE  *ip;
extern DIR    *dp;
extern MINODE minode[ ];
extern MOUNT  mounttab[ ];
extern OFT    oft[ ];

extern int P(), V();
extern struct semaphore fdio;

extern struct devsw devsw[ ]; 
extern struct devtab devtab[ ];
extern int boot_dev;
extern int hd_dev;
extern struct hd hda[];

extern struct buf buffer[ ];
extern struct semaphore freebuf;

extern int BMAP, IMAP;    // block_bitmap, inode_bimap block number
extern u8 btime[8];
extern char cwdname[ ];
extern char  *slash, *dot;

extern struct semaphore kbData;
extern struct stty stty[ ]; 

