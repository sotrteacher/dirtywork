/*actobj.c - active object*/
#include "qp_port.h"
#include "game.h"
#include "bsp.h"

Q_DEFINE_THIS_FILE

typedef struct ActObjTag {
  QActive super;
  //QTimeEvt blinkTimeEvt;    /* time event for blinking */
  QFsm *bomb;               /*active bomb*/
  QFsm *bomb_pool;          /*pool for Bomb4*/
} ActObj;                   /* the ActObj active object*/

static QState ActObj_initial          (ActObj *me, QEvt const *e);
static QState ActObj_active           (ActObj *me, QEvt const *e);
static QState ActObj_playing          (ActObj *me, QEvt const *e);

/* Helper functions ..................................................*/
static void ActObj_dispatchToAllBombs(ActObj *me, QEvt const *e); 

static ActObj l_actobj;/* the sole instance of the ActObj active object */

/* global objects -----------------------------------------------------*/
QActive * const AO_ActObj = (QActive *)&l_actobj;/*opaque pointer*/

/*....................................................................*/
void ActObj_ctor(void) {
  ActObj *me = &l_actobj;
  QActive_ctor(&me->super, (QStateHandler)&ActObj_initial);
  //QTimeEvt_ctor(&me->blinkTimeEvt,  BLINK_TIMEOUT_SIG);
  me->bomb_pool = Bomb4_ctor(0,7);/* instantiate Bomb4 in the pool */
  me->bomb = (QFsm *)0;           /* bomb is initially unused */
}/*end ActObj_ctor()*/

/*.....................................................................*/
QState ActObj_initial(ActObj *me, QEvt const *e) {
  (void)e;               /* avoid the "unreferenced parameter" warning */
  QFsm_init(me->bomb_pool,(QEvt *)0);            /* initial tran. */
  QActive_subscribe((QActive *)me, TIME_TICK_SIG);
  QS_OBJ_DICTIONARY(&l_actobj);        /* object dictionary for ActObj */
  //QS_OBJ_DICTIONARY(&l_actobj.blinkTimeEvt);
  QS_FUN_DICTIONARY(&ActObj_initial);/* fun. dictionary for ActObj HSM */
  QS_FUN_DICTIONARY(&ActObj_active);
  QS_FUN_DICTIONARY(&ActObj_playing);
  //QS_SIG_DICTIONARY(BLINK_TIMEOUT_SIG,   &l_actobj); /* local signals */
  //QS_SIG_DICTIONARY(MINE_DISABLED_SIG,  &l_actobj);
  return Q_TRAN(&ActObj_playing);
}/*end ActObj_initial()*/

/*.....................................................................*/
QState ActObj_active(ActObj *me, QEvt const *e) {
  (void)me;(void)e;
  //switch(e->sig){
  //  case MINE_DISABLED_SIG: {
  //    return Q_HANDLED();
  //  }
  //}
  return Q_SUPER(&QHsm_top);
}/*end ActObj_active()*/
/*.....................................................................*/
QState ActObj_playing(ActObj *me, QEvt const *e) {
  switch (e->sig) {
    case Q_ENTRY_SIG: {
      //static QEvt const takeoff = { TAKE_OFF_SIG, 0, 0 };
      //QACTIVE_POST(AO_ActObj, &takeoff, me);/* post the TAKEOFF sig */
      return Q_HANDLED();
    }
    case Q_EXIT_SIG: {
      return Q_HANDLED();
    } 
    case TIME_TICK_SIG: {
      if (me->bomb == (QFsm *)0) {
        me->bomb = me->bomb_pool;
        QFsm_dispatch(me->bomb,(QEvt *)e); /* direct dispatch */
      }
      ActObj_dispatchToAllBombs(me, e);
      return Q_HANDLED();
    }
    case UP_SIG:  /*2020.12.11*/
    case DOWN_SIG: 
    case ARM_SIG: {
        ActObj_dispatchToAllBombs(me,e);
        return Q_HANDLED();
    }
  }/*end switch()*/
  return Q_SUPER(&ActObj_active);
}/*end ActObj_playing()*/

/*....................................................................*/
void ActObj_dispatchToAllBombs(ActObj *me, QEvt const *e) {
  if (me->bomb != (QFsm *)0) {
    QFsm_dispatch(me->bomb, e);
  }
  (void)me;                     /* avoid the compiler warning */
}/*Tunnel_dispatchToAllBombs()*/






