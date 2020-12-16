/** bomb4.c - FSM Time Bomb. */
//#include "qep_port.h"           /* the port of the QEP event processor */
#include "qp_port.h"           
#include "bsp.h"                              /* board support package */
#include "game.h"

Q_DEFINE_THIS_FILE
/* Local objects ------------------------------------------------------*/
typedef struct Bomb4Tag {
  QFsm super;                                      /* derive from QFsm */
//  QHsm super;                                      /* derive from QHsm */
//  QActive super;                                /* derive from QActive */
  uint8_t timeout;                 /* number of seconds till explosion */
  uint8_t code;           /* currently entered code to disarm the bomb */
  uint8_t defuse;             /* secret defuse code to disarm the bomb */
} Bomb4;

static QState Bomb4_initial(Bomb4 *me, QEvent const *e);
static QState Bomb4_setting(Bomb4 *me, QEvent const *e);
static QState Bomb4_timing (Bomb4 *me, QEvent const *e);
static QState Bomb4_unused (Bomb4 *me, QEvent const *e);
//static Bomb4 l_bomb4;      /* the sole instance of Bomb4 active object*/
static Bomb4 l_bomb4[GAME_MINES_MAX];        /* a pool of time bomb */
                        /* helper macro to provide the ID of this bomb */
#define BOMB_ID(me_)    ((me_) - l_bomb4)

/*.....................................................................*/
/* global objects ----------------------------------------------------*/
/* opaque pointer to Bomb4 AO */
//QActive * const AO_Bomb4 = (QActive *)&l_bomb4; 
/*....................................................................*/
//void Bomb4_ctor(Bomb4 *me, uint8_t defuse) {
QFsm *Bomb4_ctor(uint8_t id,uint8_t defused) {
  Bomb4 *me;
  Q_REQUIRE(id < GAME_MINES_MAX);
  me = &l_bomb4[id];
  //QHsm_ctor(&me->super, (QStateHandler)&Bomb4_initial);
  QFsm_ctor(&me->super, (QStateHandler)&Bomb4_initial);
  me->defuse = defused;/*the defuse code is hardcoded at instantiation*/
  return (QFsm *)me;
}
/*....................................................................*/
//QState Bomb4_initial(Bomb4 *me, QEvent const *e) {
QState Bomb4_initial(Bomb4 *me, QEvt const *e) {
  static  uint8_t dict_sent;
  (void)e;  /* avoid the "unreferenced parameter" warning */
  //QHsm_init((QHsm *)&me->super,(QEvt *)0);
  //QActive_subscribe((QActive *)me,TIME_TICK_SIG);
  //QActive_subscribe((QActive *)me,UP_SIG);
  //QActive_subscribe((QActive *)me,DOWN_SIG);
  //QActive_subscribe((QActive *)me,ARM_SIG);
  if (!dict_sent) {
    QS_OBJ_DICTIONARY(&l_bomb4[0]);/* object dictionary for Bomb4 pool */
    QS_OBJ_DICTIONARY(&l_bomb4[1]);
    QS_OBJ_DICTIONARY(&l_bomb4[2]);
    QS_OBJ_DICTIONARY(&l_bomb4[3]);
    QS_OBJ_DICTIONARY(&l_bomb4[4]);
    QS_FUN_DICTIONARY(&Bomb4_initial);/*function dictionary for Bomb4 FSM*/
    QS_FUN_DICTIONARY(&Bomb4_setting);
    QS_FUN_DICTIONARY(&Bomb4_timing);
    QS_FUN_DICTIONARY(&Bomb4_unused);
    dict_sent = 1;
  }
                                                     /* local signals */
//  QS_SIG_DICTIONARY(MINE_DISABLED_SIG, me);
  QS_SIG_DICTIONARY(MINE_PLANT_SIG, me);
  QS_SIG_DICTIONARY(SHIP_IMG_SIG,      me);
  QS_SIG_DICTIONARY(BOMB_SIG,          me);

  me->timeout = INIT_TIMEOUT;
  //return Q_TRAN(&Bomb4_setting);
  return Q_TRAN(&Bomb4_unused);
  //return Q_TRAN(&Bomb4_timing);
}/*end Bomb4_initial()*/
/*....................................................................*/
QState Bomb4_unused(Bomb4 *me, QEvt const *e) {
    switch (e->sig) {
        case MINE_PLANT_SIG: {
            return Q_TRAN(&Bomb4_timing);
        }
    }
  //return Q_SUPER(&QFsm_top);
  return Q_IGNORED();
}
/*....................................................................*/
QState Bomb4_setting(Bomb4 *me, QEvent const *e) {
  //return Q_TRAN(&Bomb4_timing);
  switch (e->sig) {
    case UP_SIG: {
      if (me->timeout < 60) {
        ++me->timeout;
        BSP_display(me->timeout);
      }
      return Q_HANDLED();
    }
    case DOWN_SIG: {
      if (me->timeout > 1) {
        --me->timeout;
        BSP_display(me->timeout);
      }
      return Q_HANDLED();
    }
    case ARM_SIG: {
       return Q_TRAN(&Bomb4_timing);        /* transition to "timing" */
    }
  }/*end switch()*/
  return Q_IGNORED();
}/*end Bomb4_setting()*/
/*....................................................................*/
QState Bomb4_timing(Bomb4 *me, QEvent const *e) {
  switch (e->sig) {
    case Q_ENTRY_SIG: {
      me->code = 0;                          /* clear the defuse code */
      TickEvt *tev;
      tev = Q_NEW(TickEvt,BOMB_SIG);
      tev->fine_time = 5;//<----------------------------
      QACTIVE_POST(AO_Tunnel,(QEvt *)tev,me);
      return Q_HANDLED();
    }
    case UP_SIG: {
      me->code <<= 1;
      me->code |=1;
      return Q_HANDLED();
    }
    case DOWN_SIG: {
      me->code <<= 1;
      return Q_HANDLED();
    }
    case ARM_SIG: {
      if (me->code == me->defuse) {
        return Q_TRAN(&Bomb4_setting);
      }
      return Q_HANDLED();
    }
    /*case TICK_SIG: {*/
    case TIME_TICK_SIG: {
      if (((TickEvt const *)e)->fine_time == 0) {
        --me->timeout;
        BSP_display(me->timeout);
        if (me->timeout == 0) {
          BSP_boom(); /* destroy the bomb */
        }
      }
      return Q_HANDLED();
    }
  }
  return Q_IGNORED();
}/*end Bomb4_timing()*/





