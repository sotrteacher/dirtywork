#include <intr.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <misc/error_checks.h>
#include <sys/pio.h>
#define FUENTE_INTERRUPCION	SOFT_HWINTERRUPT

// datos compartidos entre el manejador y el thread
typedef struct {
 sem_t sem;
 unsigned char dato;
} area_manejador_t;

area_manejador_t area_manejador;

// manejador de interrupci\'on
int manejador_intr (void * area,intr_t intr) {
 printc("En el manejador\n");
 (void)intr;

 // lee el dato del dispositivo
 ((area_manejador_t *)area)->dato = inb(0x378);

 //Se\~naliza el sem\'aforo
 CHKE( sem_post(&((area_manejador_t *)area)->sem ));

 return POSIX_INTR_HANDLED_DO_NOT_NOTIFY;
}

// lee dato (funci\'on llamada por los threads)
// (la operaci\'on read de un driver se comportar\'ia de forma similar)
unsigned char lee_dato() {
 // Si no hay datos, los threads se encolan en el sem\'aforo
 CHKE( sem_wait(&area_manejador.sem) );

 // lee el dato
 // cuando el dato a leer no es at\'omico es necesario mantener 
 // la interrupci\'on deshabilitada (no en este caso, aun as\'i 
 // se hace para que sirva de ejemplo)
 CHKE( posix_intr_lock(FUENTE_INTERRUPCION) );
 unsigned char d = area_manejador.dato;
 CHKE ( posix_intr_unlock(FUENTE_INTERRUPCION));

 return d;
}

int main ()
{
 unsigned char dato;

 // inicializa el sem\'aforo
 CHKE( sem_init(&area_manejador.sem, 0, 0) );

 // instala el manejador de interrupci\'on
 CHKE( posix_intr_associate(FUENTE_INTERRUPCION,manejador_intr,
                            &area_manejador,sizeof(area_manejador_t)));

// habilita la interrupci\'on
 CHKE( posix_intr_unlock(FUENTE_INTERRUPCION)  ); 

 // lee datos del dispositivo
 while (1) {
  // espera un nuevo dato
  dato = lee_dato();
  printf("Le\\'ido %c\n",dato);
 }
 return 0;
}/*end main()*/









