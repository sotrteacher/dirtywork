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
/**************  6-3 : KCW *************************
5-2-06 : Re-wrote the kbd.c completely. The only thing left from MINIX's kbd.c
is part of the scan code to ASCII translation table. Instead of mapping the 
special keys to weired octal values, they are handled directly.
***********************************************************************/
#ifndef MK
 #include "../type.h"
#endif

#include "../extern.h"

#define KEYBD	0x60	/* I/O port for keyboard data */
#define PORT_B  0x61    /* port_B of 8255 */
#define KBIT	0x80	/* bit used to ack characters to keyboard */
/*
#define N_SCAN           105	 Number of scan codes 
*/
#define N_SCAN           64	/* Number of scan codes */
#define F1		 0x3B
#define F2		 0x3C
#define F3		 0x3D
#define F4		 0x3E
#define F5		 0x3F

#define CAPSLOCK         0x3A
#define LSHIFT           0x2A
#define RSHIFT           0x36
#define CONTROL          0x1D
#define ALT              0x38
#define DEL              0x53

/* Scan codes to ASCII for unshifted keys; unused keys are left out */
char unsh[N_SCAN] = {
 0,033,'1','2','3','4','5','6',        '7','8','9','0', '-','=','\b','\t',
 'q','w','e','r','t','y','u','i',      'o','p','[',']', '\r', 0,'a','s',
 'd','f','g','h','j','k','l',';',       0,  0,  0,  0,  'z','x','c','v',
 'b','n','m',',','.','/', 0,'*',        0, ' '
};

/* Scan codes to ASCII for shifted keys; unused keys are left out */
char sh[N_SCAN] = {
 0,033,'!','@','#','$','%','^',        '&','*','(',')', '_', '+','\b','\t',
 'Q','W','E','R','T','Y','U','I',      'O','P','{','}', '\r', 0, 'A', 'S',
 'D','F','G','H','J','K','L',':',       0, '~', 0, '|', 'Z', 'X','C', 'V',
 'B','N','M','<','>','?',0,'*',         0, ' '
};

#define KBN       128

char kbbuf[KBN];
int kbhead, kbtail;
struct semaphore kbData;
struct semaphore kbline;

extern struct semaphore fdsem; // defined in fd.c

int color;

int alt;		/* alt key state */
int capslock;		/* caps lock key state */
int esc;		/* escape scan code detected? */
int control;		/* control key state */

int shift;		/* left and right shift key state */
int escKey;             // keys that come as escape sequence E0 xx

int kbinit()
{
  printf("kbinit, ");

  shift=alt=capslock=esc=control=0;

  kbhead = kbtail = 0;

  kbData.value = 0;   kbData.queue = 0;
  kbline.value = 0;   kbline.queue = 0;

  //enable_irq(1);	
  //out_byte(0x20, 0x20);
  //printf("kbinit done\n\r");
}
/************************************************************************
 kbhandler() is the kbd interrupt handler. The kbd generates 2 interrupts
 for each key typed; one when the key is pressed and another one when the
 key is released. Each key generates a scan code. The scan code of a key
 release is 0x80 + the scan code of key pressed. When the kbd interrupts,
 the scan code is in the data port (0x60) of the KBD interface. First, 
 read the scan code from the data port. Then ack the key input by strobing 
 its PORT_B at 0x61.
 Some special keys generate ESC key sequences,e.g. arrow keys
                   Then process the scan code:
1. Normal key releases are ignored except for the spcecial keys of 
   0xE0, CAPLOCK, CONTROL, ALT, SHIFTs, which are used to set or clear
   the status variables  esc, control, alt, shift  
2. For normal keys: translate into ASCII, depending on shifted or not.
3. ASCII keys are entered into a circular input buffer. 
4. Sync between upper routines and lower routines is by P/V on semaphores
   kbData, kbline
5. The input buffer contains RAW keys (may have \b). kgets()/ugets() cooks
   the keys to weed out specail keys such as \b. So far only \b is handled.
   Arrow keys are used only by sh for history. Each arrow key is immediately
   made into a line to allow sh to get a line of inputs.   
**************************************************************************/
struct proc *fdtask;
extern struct floppy floppy[ ];
extern int recalibrate();
extern MINODE minode[NMINODES];

char *format="%d [%d%d] ref=%d lock=%d";
int doF1()
{
    kps(); printf("running=%d\n", running->pid);
}
int doF2()
{
    printQueue(readyQueue);
}
int doF3()
{
  int i;
    for (i=0; i<NBUF; i++){
      printf("\nbuf%d = [%d %d]  ", i, buffer[i].dev, buffer[i].blk);
    }
    prints("\n\r");

    for (i=0; i<NBUF; i++){
      printf("buf%d.lock at %x= [%d %x] -> ", 
               i, &buffer[i].lock, buffer[i].lock.value, buffer[i].lock.queue);
      if (buffer[i].lock.queue)
         printf("%d", buffer[i].lock.queue->pid);
      printf("\n");
    }
   
   printf("freebuf at %x= [%d %x] -> ", &freebuf, freebuf.value, freebuf.queue);
    if (freebuf.queue)
      printf("%d", freebuf.queue->pid);
    printf("\n");
}

int doF4()
{
  int i;
    printf("semaphores:\n");
    
    printf("kbData at %x= [%d %x] -> ", &kbData, kbData.value, kbData.queue);
    if (kbData.queue)
      printf("%d", kbData.queue->pid);
    printf("\n");

    printf("fdsem at %x= [%d %x] -> ", &fdsem, fdsem.value, fdsem.queue);

    fdtask = 0;
    if (fdsem.queue){
      printf("%d", fdsem.queue->pid);
      fdtask = fdsem.queue;
    }
    printf("\n");
     
    for (i=0; i<NPROC; i++){
      if (proc[i].status==BLOCK){
	printf("task %d blocked on sem %x\n", proc[i].pid, proc[i].sem);
      }
    }
    if (fdtask){
      fdtask->res->signal |= (1 << 5);
       wV(fdtask);
       binit();
       kbinit();
       fd_init();
       recalibrate(&floppy[0]);

    }
}

int doF5()
{
  int me;
  MINODE *mip; PROC *p;

   printf("\nActive minodes :\n");
  
    for (me=0; me<NMINODES; me++){
      mip=&minode[me];

      if (mip->refCount != 0){
	printf(format, me, mip->dev,mip->ino,mip->refCount,mip->lock.value);
 
        if (mip->lock.value < 0){
          p = mip->lock.queue;
          printf(" queue=%x : ",p); 
          while(p != 0){
            printf("%x @ %d ->", p, p->pid);
            p = p->next;
           }
	}
	printf("\n");
      }
    }
}


int kbhandler()
{
  int code, val, c, i,me;
  MINODE *mip; PROC *p;

  /* Fetch the character from the keyboard hardware and acknowledge it. */
  code = in_byte(KEYBD);	/* get the scan code of the key struck */
  val  = in_byte(PORT_B);	/* strobe the keyboard to ack the char */
  out_byte(PORT_B, val | KBIT);	/* first, strobe the bit high */
  out_byte(PORT_B, val);	/* then strobe it low */

  //printx(code);

  // must catch 0xE0 for keys that generate escape (scan) code sequences:
  // Arrow keys: up=E048; down=E050; left=E04B; right=E04D
  // Rcontrol=E01D        Delete=E053/ RAlt=E038

  if (code == 0xE0) 
     esc++;              // inc esc bount by 1

  if (esc && esc < 2)    // only the first 0xE0, wait for next code
    goto out;

  if (esc == 2){ // two E0 means escape sequence key release
    if (code == 0xE0) // this is the 2nd E0 itself => real code will come next
       goto out;

     // with esc==2, this must be the actual scan code, so handle it 

     code &= 0x7F;         // leading bit off 

     if (code == 0x53){   // Delete key ONLY USE is control-alt-del
       if (control && alt) // this is NOT the Del key, so very unlikely
	 kreboot();        // to reboot by Cntl-Alt-Delete 
       goto out;
     }

     if (code == 0x38){  // Right Alt
       alt = 0;
       goto out;
     }

     if (code == 0x1D){   // Right Control release
       control = 0;
       goto out;
     }

     if (code == 0x48)   // up arrow
       escKey = 0x0B;
     if (code == 0x50)   // down arrow
       escKey = 0x0C;
     if (code == 0x4B)   // left arrow
       escKey = 0x0D;
     if (code == 0x4D)   // right arrow
       escKey = 0x0E;

     kbbuf[kbhead++] = escKey;
     kbhead %= KBN;
     V(&kbData); 

     kbbuf[kbhead++] = '\n';
     kbhead %= KBN;

     V(&kbData);
     V(&kbline);
     esc = 0;
     goto out;
  }

  if (code & 0x80){ // key release: ONLY catch release of shift, control

    code &= 0x7F;  // mask out bit 7
    if (code == LSHIFT || code == RSHIFT)
      shift = 0;    // released the shift key
    if (code == CONTROL)
      control = 0;
    if (code == ALT)
      alt = 0;
    goto out;
  }

  // from here on, must be key press 
  if (code == LSHIFT || code == RSHIFT){
    shift = 1;
    goto out;
  }
  if (code == ALT){
    alt = 1;
    goto out;
  }
  if (code == CONTROL){
    control = 1;
    goto out;
  }

  if (code == 0x3A){
    capslock = 1 - capslock;    // capslock key acts like a toggle
    goto out;
  }

  if (control && alt && code == DEL){
      kreboot();
      goto out;
  }
  /************ 5-06-****** add F keys for debuggin *************/
  if (code == F1){
     doF1(); goto out;
  }
  if (code == F2){
     doF2(); goto out;
  }
  if (code == F3){
     doF3(); goto out;
  }

  if (code == F4){
     doF4(); goto out;
  }

  if (code==F5){
     doF5();goto out;
  }      

  c = (shift ? sh[code] : unsh[code]);

  if (capslock){
    if (c >= 'A' && c <= 'Z')
	c += 'a' - 'A';
    else if (c >= 'a' && c <= 'z')
	c -= 'a' - 'A';
  }

  if (control && c=='c'){  // Control-C keys on PC, these are 2 keys
    printf("Control_C : send signal#2 to procs ");
    for (i=1; i<NPROC; i++){  // give signal#2 to ALL on this terminal
      if (proc[i].status != FREE && strcmp(proc[i].res->tty, "/dev/tty0")==0){
	proc[i].res->signal |= (1 << 2); // sh IGNore, so only children die
        //printf("%d ", proc[i].pid);
        //kkill(2, proc[i].pid);  // kill unblocks process
        }   
    }
    printf("\n");
    /***************
    kbbuf[kbhead++] = '\n';
    kbhead %= KBN; 
    V(&kbData);
    goto out;
    ****************/
    c = '\n';             // feed a \n to input buffer
  }

  if (control && (c=='d'|| c=='D')){  // Control-D, these are 2 keys
    c = 4;   // Control-D
  }

  /*****************
  if (c =='\r'){
    //c = '\n';
      putuc(c);
      putuc('\r');
  }
  else{
    if (c!=4)
      putuc(c);         // this is an ASCII key 
  }
  *******************/

  /* Store the character in kbbuf[] for task to get */
  if (kbData.value == KBN)
     goto out;

  kbbuf[kbhead++] = c;
  kbhead %= KBN;

  V(&kbData); 

 out:
   out_byte(0x20, 0x20); 
}

/********************** upper half rotuine ***********************/ 

int kgetc()
{
  int c;
  P(&kbData);
  c = kbbuf[kbtail++] & 0x7F;
  kbtail %= KBN;
  return c;
}

int ugetc()
{
  int c;
  P(&kbData);
  c = kbbuf[kbtail++] & 0x7F;
  kbtail %= KBN;
  return c;
}

int ugets(char *uline)
{
  int n;
  char c, *tp, *cp;
  char temp[64], temp2[64];
  tp = temp;

   while (1){
     c = kgetc();
     if (c == '\n')
        break;
     *tp = c;
     tp++;
   }
   *tp = 0;
   //printi(strlen(temp));

   // cook the temp[] line
   tp = temp; cp = temp2;

   while (*tp){
     if (*tp == '\b'){
       cp--; tp++;
       continue;
     }
     if (*cp == '\n')
       break;
     *cp = *tp;
     cp++; tp++;
   }
   *cp = 0;
   //printi(strlen(temp2));

   n = 0;
   tp = temp2;
   while(*tp){
     put_ubyte(*tp, uline);
     tp++; uline++; n++;
   }
   put_ubyte(0, uline);
   return n;
}


/************************* KBD scan code info *****************************

US 104-key keyboard, set 1 scancodes

"Make" code is generated when key is pressed.
"Break" code is generated when key is released.
Hex value of make code for each key is shown.

Most keys:
	one-byte make code	= nn
	one-byte repeat code	= nn
	one-byte break code	= 80h + nn

"Gray" keys (not on original 84-key keyboard):
	two-byte make code	= E0nn
	two-byte repeat code	= E0nn
	two-byte break code	= E0 followed by 80h + nn

"Gray" keys noted by [1] are NumLock-sensitive.
When the keyboard's internal NumLock is active:
	four-byte make code	= E02AE0nn
	two-byte repeat code	= E0nn
	four-byte break code	= E0 followed by 80h + nn followed by E0AA

 ____    ___________________    ___________________    ___________________
|    |  |    |    |    |    |  |    |    |    |    |  |    |    |    |    |
|Esc |  |F1  |F2  |F3  |F4  |  |F5  |F6  |F7  |F8  |  |F9  |F10 |F11 |F12 |
|    |  |    |    |    |    |  |    |    |    |    |  |    |    |    |    |
|  01|  |  3B|  3C|  3D|  3E|  |  3F|  40|  41|  42|  |  43|  44|  57|  58|
|____|  |____|____|____|____|  |____|____|____|____|  |____|____|____|____|

 __________________________________________________________________________
|    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
|~   |!   |@   |#   |$   |%   |^   |&   |*   |(   |)   |_   |+   ||   |bksp|
|`   |1   |2   |3   |4   |5   |6   |7   |8   |9   |0   |-   |=   |\   |    |
|  29|  02|  03|  04|  05|  06|  07|  08|  09|  0A|  0B|  0C|  0D|  2B|  0E|
|____|____|____|____|____|____|____|____|____|____|____|____|____|____|____|
|    |    |    |    |    |    |    |    |    |    |    |    |    |         |
|Tab |Q   |W   |E   |R   |T   |Y   |U   |I   |O   |P   |{   |}   |         |
|    |    |    |    |    |    |    |    |    |    |    |[   |]   |         |
|  0F|  10|  11|  12|  13|  14|  15|  16|  17|  18|  19|  1A|  1B|         |
|____|____|____|____|____|____|____|____|____|____|____|____|____|         |
|    |    |    |    |    |    |    |    |    |    |    |    |              |
|Caps|A   |S   |D   |F   |G   |H   |J   |K   |L   |:   |"   |     Enter    |
|    |    |    |    |    |    |    |    |    |    |;   |'   |              |
|  3A|  1E|  1F|  20|  21|  22|  23|  24|  25|  26|  27|  28|            1C|
|____|____|____|____|____|____|____|____|____|____|____|____|______________|
|         |    |    |    |    |    |    |    |    |    |    |              |
| L Shift |Z   |X   |C   |V   |B   |N   |M   |<   |>   |?   |   R Shift    |
|         |    |    |    |    |    |    |    |,   |.   |/   |              |
|       2A|  2C|  2D|  2E|  2F|  30|  31|  32|  33|  34|  35|            36|
|_________|____|____|____|____|____|____|____|____|____|____|______________|
|       |       |       |                  |       |       |       |       |
|L Ctrl | L win | L Alt |       space      | R Alt | R win | menu  |R Ctrl |
|       |[1]    |       |                  |       |[1]    |[1]    |       |
|     1D|   E05B|     38|                39|   E038|   E05C|   E05D|   E01D|
|_______|_______|_______|__________________|_______|_______|_______|_______|


[2] For PrintScreen/SysRq key: make code = E02AE037,
repeat code = E037, break code = E0B7E0AA

[3] The Pause/Break key does not repeat, and it does not
generate a break code. Its make code is E11D45E19DC5

 ____ ____ ____
|    |    |    |
|Prt |Scrl|Paus|
|Scrn|Lock|Brk |
| [2]|  46| [3]|
|____|____|____|

 ____ ____ ____   ____ ____ ____ ____
|    |    |    | |    |    |    |    |
|Ins |Home|PgUp| |Num |/   |*   |-   |
|[1] |[1] |[1] | |Lock|    |    |    |
|E052|E047|E049| |  45|E035|  37|  4A|
|____|____|____| |____|____|____|____|
|    |    |    | |    |    |    |    |
|Del |End |PgDn| |7   |8   |9   |    |
|[1] |[1] |[1] | |Home|(U) |PgUp|    |
|E053|E04F|E051| |  47|  48|  49|    |
|____|____|____| |____|____|____|    |
		 |    |    |    |+   |
		 |4   |5   |6   |    |
		 |(L) |    |(R) |    |
		 |  4B|  4C|  4D|  4E|
      ____       |____|____|____|____|
     |    |      |    |    |    |    |
     |(U) |      |1   |2   |3   |    |
     |[1] |      |End |(D) |PgDn|    |
     |E048|      |  4F|  50|  51|Ent |
 ____|____|____  |____|____|____|    |
|    |    |    | |         |    |    |
|(L) |(D) |(R) | |0        |.   |    |
|[1] |[1] |[1] | |Ins      |Del |    |
|E04B|E050|E04D| |       52|  53|E01C|
|____|____|____| |_________|____|____|


code key        code key        code key        code key
---- ---        ---- ---        ---- ---        ---- ---
01   Esc        0F   Tab        1D   L Ctrl     2B   \|
02   1!         10   Q          1E   A          2C   Z
03   2"         11   W          1F   S          2D   X
04   3#         12   E          20   D          2E   C
05   4$         13   R          21   F          2F   V
06   5%         14   T          22   G          30   B
07   6^         15   Y          23   H          31   N
08   7&         16   U          24   J          32   M
09   8*         17   I          25   K          33   ,<
0A   9(         18   O          26   L          34   .>
0B   0)         19   P          27   ;:         35   /?
0C   -_         1A   [{         28   '"         36   R Shift
0D   =+         1B   ]}         29   `~         37   *
0E   BackSpace  1C   Enter      2A   L Shift    38   L Alt

code key        code key        code key        code key
---- ---        ---- ---        ---- ---        ---- ---
39   Space      41   F7         49   PageUp 9   51   PageDown 3
3A   CapsLock   42   F8         4A   -          52   Insert 0
3B   F1         43   F9         4B   (left) 4   53   Del .
3C   F2         44   F10        4C   5
3D   F3         45   NumLock    4D   (right) 6  57   F11
3E   F4         46   ScrollLock 4E   +          58   F12
3F   F5         47   Home  7    4F   End 1
40   F6         48   (up) 8     50   (down) 2


code            key
----            ---
E01C            Enter (on numeric keypad)
E01D            R Ctrl
E02A            make code prefix for keyboard internal numlock
E02AE037        PrintScreen make code
E035            /
E037            PrintScreen repeat code
E038            R Alt
E047            Home
E048            (up)
E049            PageUp
E04B            (left)
E04D            (right)
E04F            End
E050            (down)
E051            PageDown
E052            Insert
E053            Del
E05B            L Win
E05C            R Win
E05D            Menu
E0AA            break code suffix for keyboard internal numlock
E0B7E0AA        PrintScreen break code
E11D45E19DC5    Pause
*****************************************************************************/

