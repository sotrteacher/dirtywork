#include "type.h"

#define KCODE	        0x08
#define KDATA	        0x10

#define USER_CODE	0x07
#define USER_DATA	0x0f

#define TSS_SEL		0x18
#define	LDT_SEL		0x20

#define DEFAULT_LDT_CODE 0x00c0fa000000fffffULL
#define DEFAULT_LDT_DATA 0x00c0f2000000fffffULL

/*** UCS selector=0111=index=0, LDT=1, RPL=11; UDS=1111: index=1,LDT,RPL=11 */
#define UCS   0x07
#define UDS   0x0F
#define USS   0x07
#define UFLAG 0x3202

extern u64 *gdt;
extern u64 *ldt;

void default_trap(void);

#define printf printk
int printk(char *fmt,...);
#define printf printk
#define SYS_CALL 0x80

extern PROC proc[ ];
extern PROC *running;
extern PROC *freeList;
extern PROC *tfreeList;
extern PROC *readyQueue;
extern PROC *sleepList;

extern int (*intEntry[ ])();
extern int (*intHandler[ ])();

extern int int80h();
extern int hdinth(), kbinth(), tinth(), kcinth(), fdinth(), s0inth(), s1inth();
extern int printh(), cdinth();

extern u8 btime[8];  // centry, year, month, day, bhr, bmin, bsec, bpad;
extern u32 hr, min, sec;

extern struct buf *bread(), *getblk();

extern MINODE *root;
extern struct semaphore loadsem;
extern struct semaphore kbData;
extern OFT oft[ ];
extern void loader();
extern int ntasks;

extern int goUmode();
extern int HD;
extern int hd_rw();

extern PROC *readyQueue;
extern PROC *dequeue();

extern MINODE minode[NMINODES];

extern struct devtab devtab[NDEV];
extern struct buf buffer[ ];

extern int int_off(), int_on();
extern int in_word(), out_word();
extern int boot_dev;

extern struct floppy floppy[ ];
extern int recalibrate();
extern int motor_status, fd_motor_off;   // in floppy disk driver fd.c
extern int tout, fd_timeout;
extern int need_reset, fdreset, IFLAG;   // FD need reset flag

extern struct semaphore freebuf;
extern struct semaphore fdsem;
extern struct semaphore fdsem; // defined in fd.c

extern int requests, hits, ratio;
extern int org;
extern char *strcpy(), *strcat();

extern int color;
#define RED  0x0C
#define CYAN 0x0A
