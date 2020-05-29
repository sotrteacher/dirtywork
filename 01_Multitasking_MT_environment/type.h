/*********** type.h file ************/
#define NPROC	   9       // number of PROCs
#define SSIZE	1024       // stack size = 1KB

// PROC status
#define FREE   0
#define READY  1
#define SLEEP  2
#define ZOMBIE 3

typedef struct proc{
  struct proc *next; // next proc pointer
  int *ksp;          // saved stack pointer
  int pid;           // pid = 0 to NPROC-1
  int ppid;          // parent pid
  int status;        // PROC status
  int priority;      // scheduling priority
  int kstack[SSIZE]; // process stack
}PROC;


