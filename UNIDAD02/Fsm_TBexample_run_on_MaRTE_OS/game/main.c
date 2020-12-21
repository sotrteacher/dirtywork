//#define LMC_1000
//#define LMC_2000 
//#define LMC_3000 
//#define LMC_4000
//#define LMC_5000
//#define LMC_6000
//#define LMC_7000
//#define LMC_8000
//#define LMC_9000
//#define LMC_10000
/***************************************************************************
*                     INSTITUTO POLITÉCNICO NACIONAL
* UNIDAD PROFESIONAL INTERDISCIPLINARIA EN INGENIERÍA Y TECNOLOGÍAS AVANZADAS
*
*                   SISTEMAS OPERATIVOS EN TIEMPO REAL.
*                             Periodo 20/1
*
* Port de juego Fly'n'Shoot (Quantum Leaps) para plataforma MaRTE OS
*
* Autores de port:
* José Fausto Romero Lujambio
* Pastor Alan Rodríguez Echeverría
*
******************************************************************************
* Product: "Fly'n'Shoot" game example
* Last Updated for Version: 4.5.05
* Date of the Last Update:  Mar 22, 2013
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2013 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* Quantum Leaps Web sites: http://www.quantum-leaps.com
*                          http://www.state-machine.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "qp_port.h"
#include "bsp.h"
#include "game.h"
#include "video.h"

#include <stdlib.h>
#include <pthread.h>

/* Port de MaRTE OS */
extern int screenWidth,screenHeight;
extern unsigned char *backBuffer;

volatile int thread_signal;
static pthread_t event_thread, kernel_thread;

void print_exit_screen(void);

/* Local-scope objects -----------------------------------------------------*/
static QEvt const * l_missileQueueSto[2];
static QEvt const * l_shipQueueSto[3];
static QEvt const * l_tunnelQueueSto[GAME_MINES_MAX + 5];

static union SmallEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(QEvt)];
    /* ... other event types to go into this pool */
} l_smlPoolSto[10];                     /* storage for the small event pool */

static union MediumEvents {
    void   *e0;                                       /* minimum event size */
    uint8_t e1[sizeof(ObjectPosEvt)];
    uint8_t e2[sizeof(ObjectImageEvt)];
    uint8_t e3[sizeof(MineEvt)];
    uint8_t e4[sizeof(ScoreEvt)];
    /* ... other event types to go into this pool */
} l_medPoolSto[2*GAME_MINES_MAX + 8];  /* storage for the medium event pool */

static QSubscrList    l_subscrSto[MAX_PUB_SIG];

/*..........................................................................*/
void *kernel_thread_handler(void *status) {
    double kernel_interval;
    struct timespec kernel_ts;
    int *thread_status = (int *) status;

    /** este intervalo es crítico para el funcionamiento del kernel **/
    /* el intervalo del kernel debe ser significativamente menor al intervalo
     * del hilo de eventos, pero existir (no puede ser omitido o ser 0). */
    kernel_interval = 0.1e-3;
    double_to_timespec(kernel_interval, &kernel_ts);

    for (;;) {
        if (*thread_status == 1)
            break;

        // ejecutar bucle de eventos del kernel QP
        QF_run_event_loop();

        // ESTA LÍNEA ES CRÍTICA PARA EL FUNCIONAMIENTO CORRECTO DEL KERNEL
        nanosleep(&kernel_ts, NULL);
    }
    return NULL;
}
/*..........................................................................*/
void *event_thread_handler(void *status) {
    double event_interval;
    struct timespec event_ts;
    int *thread_status = (int *) status;

    event_interval = 1.0 / BSP_TICKS_PER_SEC;
    double_to_timespec(event_interval, &event_ts);

    for (;;) {
        // procesar señales de control
        if (*thread_status == 1)
            break;

#ifndef LMC_6000
        // procesar entrada de teclado
        if (!keyboard_support())
            break;
#endif /*LMC_6000*/

#ifndef LMC_7000
        // evento de reloj
        timer_event();
#endif /*LMC_7000*/

        // dibujar pantalla
#ifdef LMC_10000
        Video_render();
#endif /*LMC_10000*/
        nanosleep(&event_ts, NULL);
    }
    print_exit_screen();
    return NULL;
}
/*..........................................................................*/
void print_exit_screen(void) {
    // Pantalla de salida de aplicación
    Video_clearScreen(VIDEO_BGND_BLACK);
    Video_printStrAt(2, 19, VIDEO_FGND_WHITE, "Port realizado por: 1 2 3 4 5");
    Video_printStrAt(2, 20, VIDEO_FGND_WHITE, "Jose Fausto Romero Lujambio x y z");
    Video_printStrAt(2, 21, VIDEO_FGND_WHITE, "Pastor Alan Rodriguez Echeverria");
    Video_printStrAt(2, 22, VIDEO_FGND_YELLOW, "LMC Juego terminado. Presione cualquier tecla para salir...");
    Video_render();
}
/*..........................................................................*/
int main(int argc, char *argv[]) {
    /* Inicializar MaRTE OS */
    //make getchar() non-blocking: 'getchar' returns -1 inmediately 
    //when there is no characters available at the moment of the call. 
    //x86_arch/include/misc/console_management.h
    reset_blocking_mode();
    set_raw_mode();// Every character is made available to the 
                   // calling program as soon as it is typed, so
                   // no line editing is available in this mode.

    /* inicializar vga y emulador de terminal */
//    screenWidth=320;
//    screenHeight=200;
//    int ret = init_vga(G320x200x256, VGA, PCI_DEVICE_ID_S3_TRIO64V2);
//    assert (ret == 0);
//    (void) ret;

    //virtual canvas for blitting
//    backBuffer = (unsigned char *)malloc(screenWidth*screenHeight);
//    memset(backBuffer, 0, screenWidth*screenHeight);
//    init_terminal_buffer();

//    initPalette(); //fill palette's colors
    /* fin de inicialización de MaRTE OS */
#ifdef LMC_3000
    while(1){ printf("AAAA\n");}
#endif /*LMC_3000*/

    /* explicitly invoke the active objects' ctors... */
#ifndef LMC_1000
#ifdef MISSILE
    Missile_ctor();
#endif /*MISSILE*/
#ifdef SHIP
    Ship_ctor();
#endif /*SHIP*/
    Tunnel_ctor();
#endif /*LMC_1000*/
#ifdef LMC_3000
    while(1){ printf("AAAA\n");}
#endif /*LMC_3000*/

    QF_init();/* initialize the framework and the underlying RT kernel */

                                      /* initialize the event pools... */
    QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
    QF_poolInit(l_medPoolSto, sizeof(l_medPoolSto), sizeof(l_medPoolSto[0]));

    QF_psInit(l_subscrSto, Q_DIM(l_subscrSto));/* init publish-subscribe */

                        /* send object dictionaries for event pools... */
    QS_OBJ_DICTIONARY(l_smlPoolSto);
    QS_OBJ_DICTIONARY(l_medPoolSto);

          /* send signal dictionaries for globally published events... */
    QS_SIG_DICTIONARY(TIME_TICK_SIG,      0);
    QS_SIG_DICTIONARY(PLAYER_TRIGGER_SIG, 0);
    QS_SIG_DICTIONARY(PLAYER_QUIT_SIG,    0);
    QS_SIG_DICTIONARY(GAME_OVER_SIG,      0);

                                        /* start the active objects... */
#ifdef MISSILE
    QActive_start(AO_Missile,
                  1,                                       /* priority */
                  l_missileQueueSto, Q_DIM(l_missileQueueSto), /* evt queue */
                  (void *)0, 0,                 /* no per-thread stack */
                  (QEvt *)0);               /* no initialization event */
#endif /*MISSILE*/
#ifdef LMC_9000
    QActive_start(AO_Ship,
                  2,                                       /* priority */
                  l_shipQueueSto,    Q_DIM(l_shipQueueSto),/* evt queue */
                  (void *)0, 0,                  /* no per-thread stack */
                  (QEvt *)0);                /* no initialization event */
#endif /*LMC_9000*/
    QActive_start(AO_Tunnel,
                  3,                                        /* priority */
                  l_tunnelQueueSto,  Q_DIM(l_tunnelQueueSto),/*evt queue*/
                  (void *)0, 0,                  /* no per-thread stack */
                  (QEvt *)0);                /* no initialization event */

    BSP_init(argc, argv);       /* initialize the Board Support Package */

    /* hilos de programa */
#ifndef LMC_2000
    pthread_create(&kernel_thread, NULL, kernel_thread_handler, (void *)&thread_signal);
#ifndef LMC_4000
    pthread_create(&event_thread, NULL, event_thread_handler, (void *)&thread_signal);
#endif /*LMC_4000*/
#ifdef LMC_10000
    int b = 0;
    while(b<5){ printf("BBBB\n");b++;}
#endif /*LMC_10000*/
    pthread_join(kernel_thread, NULL);
#ifndef LMC_5000
    pthread_join(event_thread, NULL);
#endif /*LMC_5000*/
#endif /*LMC_2000*/

    return 0;
}/*end main()*/


