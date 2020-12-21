#ifndef BOMB_H
#define BOMB_H
enum BombSignals {                     /* all signals for the Bomb FSM */
  UP_SIG = Q_USER_SIG,
  DOWN_SIG,
  ARM_SIG,
  /*TICK_SIG*/
  TIME_TICK_SIG,
  /* insert other published signals ... */
  MAX_PUB_SIG                             /* the last published signal */
};

typedef struct TickEvtTag {
  QEvent super;                    /* derive from the QEvent structure */
  uint8_t fine_time;                        /* the fine 1/10 s counter */
} TickEvt;

typedef struct UpEvtTag {
  QEvent super;                    /* derive from the QEvent structure */
  uint8_t p;    /*priority*/
} UpEvt;

void Bomb4_ctor(void);

extern QActive * const AO_Bomb4;

/*---------------------------------------------------------------------*/
                                   /* the initial value of the timeout */
#define INIT_TIMEOUT 10

#endif /*BOMB_H*/
