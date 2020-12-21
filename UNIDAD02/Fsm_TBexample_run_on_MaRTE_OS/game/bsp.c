/*************************************************************************
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
***************************************************************************
* Product: "Fly'n'Shoot" game, BSP for 80x86, Vanilla/DOS, Open Watcom
* Last Updated for Version: 4.5.00
* Date of the Last Update:  May 18, 2012
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2012 Quantum Leaps, LLC. All rights reserved.
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
**************************************************************************/
#include "qp_port.h"
#include "game.h"
#include "bsp.h"
#include "video.h"

#include <stdlib.h>                                     /* for _exit() */

extern volatile int thread_signal;

Q_DEFINE_THIS_FILE

/* Local-scope objects -----------------------------------------------------*/
// static void interrupt (*l_dosTmrISR)();
// static void interrupt (*l_dosKbdISR)();

//#ifndef Q_SPY
//#error Q_SPY NO ESTA DEFINIDO!!!!!!!!!!!!!!!!!!!!!!
//#endif 

#ifdef Q_SPY
    static uint16_t l_uart_base;    /* QS data uplink UART base address */
    QSTimeCtr l_tickTime;                    /* keeps timetsamp at tick */
    static uint8_t   l_tmr;
    static uint8_t   l_kbd;

    #define UART_TXFIFO_DEPTH 16
#endif

#define TMR_VECTOR      0x08
#define KBD_VECTOR      0x09

/*.....................................................................*/
void timer_event(void) {
    static QEvt const tickEvt = { TIME_TICK_SIG, 0, 0 };

    QF_TICK(&l_tmr);     /* call QF_tick() outside of critical section */
    QF_PUBLISH(&tickEvt, &l_tmr);            /* publish the tick event */
}

/*....................................................................*/
bool keyboard_support(void) {
    static uint8_t ship_pos = GAME_SHIP_Y;
    static bool result = 0;

    // check keyboard and flush buffer (flushing so works in raw mode)
    char key = getchar();
    while (getchar() != -1 );

    // primera ejecución de handler de teclado
    if (!result) {
        Video_printNumAt(24, 24, VIDEO_FGND_YELLOW, ship_pos);
        result = true;
    }

    switch (key) {
        case 'w':                                         /* Up-arrow */
        case 's': {                                     /* Down-arrow */
            ObjectPosEvt *ope = Q_NEW(ObjectPosEvt, PLAYER_SHIP_MOVE_SIG);
            if ((key == 'w') && (ship_pos > 0x00)) {
                --ship_pos;
            }
            else if ((key == 's')
                     && (ship_pos < (GAME_SCREEN_HEIGHT - 3))) {
                ++ship_pos;
            }
            ope->x = (uint8_t)GAME_SHIP_X;       /* x-position is fixed */
            ope->y = (uint8_t)ship_pos;
#ifdef SHIP
            QACTIVE_POST(AO_Ship, (QEvt *)ope, &l_kbd);  /* to the ship */
#endif /*SHIP*/

            Video_printNumAt(24, 24, VIDEO_FGND_YELLOW, ship_pos);
            break;
        }
        case 'e': {                                           /* Space */
            static uint16_t ntrig = 0;
            static QEvt const fireEvt = { PLAYER_TRIGGER_SIG, 0, 0 };
            QF_PUBLISH(&fireEvt, &l_kbd);

            Video_printNumAt(47, 24, VIDEO_FGND_YELLOW, ++ntrig);
            break;
        }                                                       /* Esc */
        case 'x': {
            static QEvt const quitEvt = { PLAYER_QUIT_SIG, 0, 0 };
            QF_PUBLISH(&quitEvt, &l_kbd);
            result = false;
            thread_signal = 1; // signal thread termination
            break;
        }
        case 'u': { /*2020.12.11*/
            //static QEvt const upEvt = { PLAYER_UP_SIG,0,0 };
            //QF_PUBLISH(&upEvt,&l_kbd);
            BombEvt *be = Q_NEW(BombEvt,UP_SIG);
            QACTIVE_POST(AO_Tunnel,(QEvt *)be,&l_kbd);/* to the tunnel */
            //Video_printNumAt(27, 24, VIDEO_FGND_YELLOW, ship_pos);
            break;
        }
        case 'd': {
            BombEvt *be = Q_NEW(BombEvt,DOWN_SIG);
            QACTIVE_POST(AO_Tunnel,(QEvt *)be,&l_kbd);
            break;
        }
        case 'a': {
            BombEvt *be = Q_NEW(BombEvt,ARM_SIG);
            QACTIVE_POST(AO_Tunnel,(QEvt *)be,&l_kbd);
            break;
        }
    }/*end switch()*/
    return result;
}/*keyboard_support()*/

/*.....................................................................*/
void BSP_init(int argc, char *argv[]) {
    char const *com = "COM1";

    if (argc > 1) {
        com = argv[1];
    }
    if (!QS_INIT(com)) {                               /* initialize QS */
        Q_ERROR();
    }

    QS_OBJ_DICTIONARY(&l_tmr);
    QS_OBJ_DICTIONARY(&l_kbd);

#ifdef LMC_9000
    Video_clearScreen(VIDEO_BGND_LIGHT_GRAY);
    Video_clearRect( 0,  0, 80,   1, VIDEO_BGND_RED | VIDEO_BLINK);
    Video_clearRect( 0,  8, 80,  24, VIDEO_BGND_BLACK | VIDEO_FGND_WHITE);
    Video_clearRect( 0,  7, 80,   8, VIDEO_BGND_BLUE);
    Video_clearRect( 0, 24, 80,  25, VIDEO_BGND_BLUE);

    Video_clearRect(24, 24, 28,  25, VIDEO_BGND_RED | VIDEO_BLINK);
    Video_clearRect(24, 24, 28,  25, VIDEO_BGND_RED | VIDEO_BLINK);

    Video_printStrAt(35,  0, VIDEO_FGND_WHITE, "FLY 'n' SHOOT");
    Video_printStrAt(15,  2, VIDEO_FGND_BLACK,
                     "Press W to move the space ship up");
    Video_printStrAt(15,  3, VIDEO_FGND_BLACK,
                     "Press S to move the space ship down");
    Video_printStrAt(15,  4, VIDEO_FGND_BLACK,
                     "Press E to fire the missile");
    Video_printStrAt(15,  5, VIDEO_FGND_BLACK,
                     "Press X to quit the game");
    Video_printStrAt( 8, 24, VIDEO_FGND_WHITE, "Ship Position:");
    Video_printStrAt(37, 24, VIDEO_FGND_WHITE, "Triggers:");
    Video_printStrAt(61, 24, VIDEO_FGND_WHITE, "Score:");

    Video_clearRect(24, 24, 28,  25, VIDEO_BGND_RED);
    Video_clearRect(47, 24, 51,  25, VIDEO_BGND_RED);
    Video_clearRect(68, 24, 72,  25, VIDEO_BGND_RED);
    Video_printNumAt(24, 24, VIDEO_FGND_YELLOW, 0);
    Video_printNumAt(47, 24, VIDEO_FGND_YELLOW, 0);
    Video_printNumAt(68, 24, VIDEO_FGND_YELLOW, 0);
#endif /*LMC_9000*/
    (void)com;             /* avoid compiler warning if QS is not used */
}/*end bsp_init()*/
/*.....................................................................*/
void BSP_drawBitmap(uint8_t const *bitmap, uint8_t width, uint8_t height) {
    Video_drawBitmapAt(0, 8, bitmap, width, height);
}
/*.....................................................................*/
void BSP_drawNString(uint8_t x, uint8_t y, char const *str) {
    Video_drawStringAt(x, 8 + y*8, str);
}
/*....................................................................*/
void BSP_updateScore(uint16_t score) {
    if (score == 0) {
        Video_clearRect(68, 24, 72,  25, VIDEO_BGND_RED);
    }
    Video_printNumAt(68, 24, VIDEO_FGND_YELLOW, score);
}
/*....................................................................*/
void BSP_display(uint8_t num) {
  //Video_clearRect(74,24,78,25,VIDEO_BGND_RED);
  Video_printNumAt(74,24,VIDEO_FGND_YELLOW,num);
}
/*....................................................................*/
void BSP_display1(uint8_t partent,uint8_t partdec) {
  Video_clearRect(74,24,78,25,VIDEO_BGND_RED);
  Video_printNumAt(71,24,VIDEO_FGND_YELLOW,partent);
  Video_printNumAt(76,24,VIDEO_FGND_YELLOW,partdec);
}
/*....................................................................*/
void BSP_display2(uint8_t partent) {
  //Video_clearRect(74,24,78,25,VIDEO_BGND_RED);
  Video_printNumAt(72,24,VIDEO_FGND_YELLOW,partent);
}
/*....................................................................*/
void BSP_display3(uint8_t partdec) {
  //Video_clearRect(74,24,78,25,VIDEO_BGND_RED);
  Video_printNumAt(76,24,VIDEO_FGND_YELLOW,partdec);
}
/*....................................................................*/
void BSP_display4(uint8_t timeout) {
  Video_printNumAt(28, 24, VIDEO_FGND_YELLOW, timeout);
}
/*....................................................................*/
void BSP_display5(uint8_t code) {
  Video_printNumAt(76, 7, VIDEO_FGND_YELLOW, code);
}
/*....................................................................*/
void BSP_boom(void) {
//  Video_clearRect(64,24,68,25,VIDEO_BGND_RED);
  Video_printStrAt(56,24,VIDEO_FGND_WHITE,"BOOM");
}
/*....................................................................*/
void BSP_unboom(void) {
  Video_printStrAt(56,24,VIDEO_FGND_WHITE,"    ");
}
/*....................................................................*/
void BSP_displayOn(void) {
}
/*..........................................................................*/
void BSP_displayOff(void) {
    Video_clearRect( 0,  8, 80,  24, VIDEO_BGND_BLACK | VIDEO_FGND_WHITE);
}
/*..........................................................................*/
void QF_onStartup(void) {
}
/*..........................................................................*/
void QF_onCleanup(void) {
}
/*..........................................................................*/
void QF_onIdle(void) {
}
/*--------------------------------------------------------------------------*/
void Q_onAssert(char const Q_ROM * const file, int line) {
    // assertion message moved to line 7 for easier debugging
    Video_clearRect ( 0, 7, 80, 8, VIDEO_BGND_RED);
    Video_printStrAt( 0, 7, VIDEO_FGND_WHITE, "ASSERTION FAILED in file:");
    Video_printStrAt(26, 7, VIDEO_FGND_YELLOW, file);
    Video_printStrAt(57, 7, VIDEO_FGND_WHITE, "line:");
    Video_printNumAt(62, 7, VIDEO_FGND_YELLOW, line);

    QF_stop();                                       /* stop QF and cleanup */
}

/*--------------------------------------------------------------------------*/
#ifdef Q_SPY

/*..........................................................................*/
static uint8_t UART_config(char const *comName, uint32_t baud) {
    switch (comName[3]) {          /* Set the base address of the COMx port */
        case '1': l_uart_base = (uint16_t)0x03F8; break;            /* COM1 */
        case '2': l_uart_base = (uint16_t)0x02F8; break;            /* COM2 */
        case '3': l_uart_base = (uint16_t)0x03E8; break;            /* COM3 */
        case '4': l_uart_base = (uint16_t)0x02E8; break;            /* COM4 */
        default: return (uint8_t)0;        /* COM port out of range failure */
    }
    baud = (uint16_t)(115200UL / baud);            /* divisor for baud rate */
    outp(l_uart_base + 3, (1 << 7));       /* Set divisor access bit (DLAB) */
    outp(l_uart_base + 0, (uint8_t)baud);                   /* Load divisor */
    outp(l_uart_base + 1, (uint8_t)(baud >> 8));
    outp(l_uart_base + 3, (1 << 1) | (1 << 0));    /* LCR:8-bits,no p,1stop */
    outp(l_uart_base + 4, (1 << 3) | (1 << 1) | (1 << 0));  /* DTR,RTS,Out2 */
    outp(l_uart_base + 1, 0);        /* Put UART into the polling FIFO mode */
    outp(l_uart_base + 2, (1 << 2) | (1 << 0));    /* FCR: enable, TX clear */

    return (uint8_t)1;                                           /* success */
}
/*..........................................................................*/
uint8_t QS_onStartup(void const *arg) {
    static uint8_t qsBuf[1*1024];                 /* buffer for Quantum Spy */
    QS_initBuf(qsBuf, sizeof(qsBuf));

                                                 /* setup the QS filters... */
    QS_FILTER_ON(QS_ALL_RECORDS);

//    QS_FILTER_OFF(QS_QEP_STATE_ENTRY);
//    QS_FILTER_OFF(QS_QEP_STATE_EXIT);
//    QS_FILTER_OFF(QS_QEP_STATE_INIT);
//    QS_FILTER_OFF(QS_QEP_TRAN_HIST);
//    QS_FILTER_OFF(QS_QEP_INTERN_TRAN);
//    QS_FILTER_OFF(QS_QEP_TRAN);
//    QS_FILTER_OFF(QS_QEP_IGNORED);

    QS_FILTER_OFF(QS_QF_ACTIVE_ADD);
    QS_FILTER_OFF(QS_QF_ACTIVE_REMOVE);
    QS_FILTER_OFF(QS_QF_ACTIVE_SUBSCRIBE);
    QS_FILTER_OFF(QS_QF_ACTIVE_UNSUBSCRIBE);
    QS_FILTER_OFF(QS_QF_ACTIVE_POST_FIFO);
    QS_FILTER_OFF(QS_QF_ACTIVE_POST_LIFO);
    QS_FILTER_OFF(QS_QF_ACTIVE_GET);
    QS_FILTER_OFF(QS_QF_ACTIVE_GET_LAST);
    QS_FILTER_OFF(QS_QF_EQUEUE_INIT);
    QS_FILTER_OFF(QS_QF_EQUEUE_POST_FIFO);
    QS_FILTER_OFF(QS_QF_EQUEUE_POST_LIFO);
    QS_FILTER_OFF(QS_QF_EQUEUE_GET);
    QS_FILTER_OFF(QS_QF_EQUEUE_GET_LAST);
    QS_FILTER_OFF(QS_QF_MPOOL_INIT);
    QS_FILTER_OFF(QS_QF_MPOOL_GET);
    QS_FILTER_OFF(QS_QF_MPOOL_PUT);
    QS_FILTER_OFF(QS_QF_PUBLISH);
    QS_FILTER_OFF(QS_QF_NEW);
    QS_FILTER_OFF(QS_QF_GC_ATTEMPT);
    QS_FILTER_OFF(QS_QF_GC);
//    QS_FILTER_OFF(QS_QF_TICK);
    QS_FILTER_OFF(QS_QF_TIMEEVT_ARM);
    QS_FILTER_OFF(QS_QF_TIMEEVT_AUTO_DISARM);
    QS_FILTER_OFF(QS_QF_TIMEEVT_DISARM_ATTEMPT);
    QS_FILTER_OFF(QS_QF_TIMEEVT_DISARM);
    QS_FILTER_OFF(QS_QF_TIMEEVT_REARM);
    QS_FILTER_OFF(QS_QF_TIMEEVT_POST);
    QS_FILTER_OFF(QS_QF_CRIT_ENTRY);
    QS_FILTER_OFF(QS_QF_CRIT_EXIT);
    QS_FILTER_OFF(QS_QF_ISR_ENTRY);
    QS_FILTER_OFF(QS_QF_ISR_EXIT);

    return UART_config((char const *)arg, 115200UL);
}
/*..........................................................................*/
void QS_onCleanup(void) {
}
/*..........................................................................*/
QSTimeCtr QS_onGetTime(void) {            /* invoked with interrupts locked */
    static uint32_t l_lastTime;
    uint32_t now;
    uint16_t count16;                         /* 16-bit count from the 8254 */

    outp(0x43, 0);                      /* latch the 8254's counter-0 count */
    count16 = (uint16_t)inp(0x40);        /* read the low byte of counter-0 */
    count16 += ((uint16_t)inp(0x40) << 8);            /* add on the hi byte */

    now = l_tickTime + (0x10000 - count16);

    if (l_lastTime > now) {                 /* are we going "back" in time? */
        now += 0x10000;               /* assume that there was one rollover */
    }
    l_lastTime = now;

    return (QSTimeCtr)now;
}
/*..........................................................................*/
void QS_onFlush(void) {
    uint16_t b;
    while ((b = QS_getByte()) != QS_EOD) { /* next QS trace byte available? */
        while ((inp(l_uart_base + 5) & (1 << 5)) == 0) { /* not empty? */
        }
        outp(l_uart_base + 0, (uint8_t)b);  /* put the byte to TX FIFO */
    }
}
#endif                                                             /* Q_SPY */
/*---------------------------------------------------------------------*/
