/*********** type.h file *************/
#define NPROC	9          // number of PROCs
#define SSIZE	1024       // stack size = 1KB

// PROC status
#define FREE 	0
#define READY 	1
#define SLEEP 	2
#define BLOCK 	3
#define ZOMBIE 	4

typedef struct proc{
  struct proc *next;    // next proc pointer
  int ksp;              // saved stack pointer
  int pid;              // proc PID
  int priority;         // proc scheduling priority
  int status;           // current status: FREE|READY, etc.
  int event;            // for sleep/wakeup
  int exitStatus;       // exit status
  int joinPid;          // join target pid
  struct proc *joinPtr; // join target PROC pointer
  int stack[SSIZE];     // proc 4KB stack area
}PROC;


