#define CASOUNO
extern int X;
extern char* data_start;
void notmain(){
  X=X+1;
#ifdef CASOUNO
  unsigned int y=(unsigned int)&data_start;/*en objdump_notmain1 vemos que data_start=0x64*/
#endif
  /*terminamos la funcion ordenadamente*/
  asm volatile("mov $1,%%eax;\n\t"      /*se coloca 1 en eax*/
               "mov %0,%%ebx;\n\t"      /*se coloca el valor de X en ebx*/
               "int $0x80\n\t"          /*se hace una syscall (system call)*/
               : : "r"(X) : "%eax" );   /*se avisa al compilador que eax esta clobbered*/
}/*end motmain()*/
