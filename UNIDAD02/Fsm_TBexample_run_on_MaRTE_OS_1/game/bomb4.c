//#define LMC_15000
#define LMC_16000
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
  uint8_t fine_time;      /* 1/10's of second */
  uint8_t code;           /* currently entered code to disarm the bomb */
  uint8_t defuse;             /* secret defuse code to disarm the bomb */
} Bomb4;

static QState Bomb4_initial(Bomb4 *me, QEvent const *e);
static QState Bomb4_setting(Bomb4 *me, QEvent const *e);
static QState Bomb4_timing (Bomb4 *me, QEvent const *e);
static QState Bomb4_unused (Bomb4 *me, QEvent const *e);
//static Bomb4 l_bomb4;      /* the sole instance of Bomb4 active object */
static Bomb4 l_bomb4[GAME_MINES_MAX];           /* a pool of time bomb */
                        /* helper macro to provide the ID of this bomb */
#define BOMB_ID(me_)    ((me_) - l_bomb4)

/*....................................................................*/
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
  me->code = 0;
  return (QFsm *)me;
}/*end Bomb4_ctor()*/
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
    //QS_OBJ_DICTIONARY(&l_bomb4[1]);
    //QS_OBJ_DICTIONARY(&l_bomb4[2]);
    //QS_OBJ_DICTIONARY(&l_bomb4[3]);
    //QS_OBJ_DICTIONARY(&l_bomb4[4]);
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
  //QS_SIG_DICTIONARY(BOMB_SIG,          me);
  QS_SIG_DICTIONARY(UP_SIG,            me);
  QS_SIG_DICTIONARY(DOWN_SIG,          me);
  QS_SIG_DICTIONARY(ARM_SIG,           me);

#ifdef LMC_16000
  clrscr();       /*Clear the screen*/
  disable_echo(); /*Input characters are not echoed*/
  set_cursorxy(0,0); 
  printf("[State Bomb4_setting]                        \n");
  printf("Press U to move the timeout up               \n");
  printf("Press D to move the timeout down             \n");
  printf("Press A to arm the bomb and transition to Bomb4_timing state\n");
#endif /*LMC_16000*/
  me->timeout = INIT_TIMEOUT;
  me->fine_time = 0;
  return Q_TRAN(&Bomb4_setting);
  /**Tambi\'en se puede hacer transici\'on directo al estado timing*/
  //return Q_TRAN(&Bomb4_timing);  
  /** return Q_TRAN(&Bomb4_unused);
   *  La transici\'on de estado usada en las primeras pruebas
   */
  return Q_TRAN(&Bomb4_unused);
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
  switch (e->sig) {
    case UP_SIG: {
      if (me->timeout < 60) {
        ++me->timeout;
#ifdef LMC_16000
        set_cursorxy(0,5);
        printf("timeout = %2d\n",me->timeout);
#else
        BSP_display4(me->timeout);
#endif /*LMC_16000*/
      }
      return Q_HANDLED();
    }
    case DOWN_SIG: {
      if (me->timeout > 1) {
        --me->timeout;
#ifdef LMC_16000
        set_cursorxy(0,5);
        printf("timeout = %2d\n",me->timeout);
#else
        BSP_display4(me->timeout);
#endif /*LMC_16000*/
      }
      return Q_HANDLED();
    }
    case ARM_SIG: {
#ifdef LMC_16000
      clrscr();
#else
       BSP_unboom();
#endif /*LMC_16000*/
       return Q_TRAN(&Bomb4_timing);        /* transition to "timing" */
    }
  }/*end switch()*/
//while(1) { printf("CCCC\n"); }
  return Q_IGNORED();
}/*end Bomb4_setting()*/
/*....................................................................*/
QState Bomb4_timing(Bomb4 *me, QEvent const *e) {
  static uint8_t num_ticks = 0;
  switch (e->sig) {
    case Q_ENTRY_SIG: {
      me->code = 0;                          /* clear the defuse code */
      //TickEvt *tev;
      //tev = Q_NEW(TickEvt,BOMB_SIG);
      //tev->fine_time = 5;//<----------------------------
      //QACTIVE_POST(AO_Tunnel,(QEvt *)tev,me);
      //tev->fine_time = 2;//<----------------------------
      //BSP_display(tev->fine_time);
#ifdef LMC_16000
      set_cursorxy(0,0); 
      printf("Insert defuse code, then press key A!\n");
#endif /*LMC_16000*/
      return Q_HANDLED();
    }
    case UP_SIG: {
      me->code <<= 1;
      me->code |=1;  /*If only UP button is pressed: 1,3,7,15, etc. */
      //BSP_display5(me->code);
      return Q_HANDLED();
    }
    case DOWN_SIG: {
      me->code <<= 1;
      //BSP_display5(me->code);
      return Q_HANDLED();
    }
    case ARM_SIG: {
      if (me->code == me->defuse) {
#ifdef LMC_16000
       set_cursorxy(0,0); 
       printf("[State Bomb4_setting]                          \n");
       printf("Press U to move the timeout up                 \n");
       printf("Press D to move the timeout down               \n");
       printf("Press A to arm the bomb and transition to Bomb4_timing state\n");
#endif /*LMC_16000*/
        return Q_TRAN(&Bomb4_setting);
      }
      return Q_HANDLED();
    }
    /*case TICK_SIG: {*/
    case TIME_TICK_SIG: {
      num_ticks++;
      /* ?`ya ha transcurrido 1/10 segundo? */
      if ((num_ticks % (BSP_TICKS_PER_SEC/10)) == 0) {
        if (me->fine_time) {/* d\'ecimas > 0 */
          --me->fine_time;
        }else if (!(me->timeout)) {/* d\'ecimas = 0 y segundos = 0 */
          goto boom;
        }else{/* d\'ecimas = 0 y segundos > 0 */
          --me->timeout;
          me->fine_time = 9;
        }
#ifdef LMC_16000
        set_cursorxy(0,1);
        printf("[State Bomb4_timing] countdown = %2d.%d\n",
               me->timeout,me->fine_time);
#else
        BSP_display2(me->timeout);
        BSP_display3(me->fine_time);
#endif /*LMC_16000*/
      }
      return Q_HANDLED();
boom:
#ifdef LMC_16000
        set_cursorxy(0,5);
        printf("BOOM!!!!\n");
#else
      BSP_boom();
#endif /*LMC_16000*/
      num_ticks = 0;
#ifdef LMC_16000
  set_cursorxy(0,0); 
  printf("[State Boomb4_setting]                         \n");
  printf("Press U to move the timeout up                 \n");
  printf("Press D to move the timeout down               \n");
  printf("Press A to arm the bomb and transition to Bomb4_timing state\n");
#endif /*LMC_16000*/
      return Q_TRAN(&Bomb4_setting);
    }
  }/*end switch()*/
  return Q_IGNORED();
}/*end Bomb4_timing()*/





