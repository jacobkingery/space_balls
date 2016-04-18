#ifndef _BALLER_H_
#define _BALLER_H_
#include <stdint.h>
#include "pin.h"
#include "ui.h"
#include "oc.h"
#include "timer.h"
#include "state.h"
#include "launcher.h"
#include "shooter.h"

void init_baller(_PIN *sort_motor, _OC *sort_oc, _TIMER *rol_timer, Shooter *shooter, Launcher *launcher);
void run_baller(uint8_t level);
void over_baller(void);
void rest_baller(void);

typedef struct {
    _PIN *sort_motor;
    _OC *sort_oc;
    _TIMER *rol_timer;
    uint8_t rol_ticks;
    uint16_t sorter_ticks;
    uint8_t rol_limit;
    uint8_t chaos;
    uint8_t chaos_limit;
    uint8_t level;
    Shooter *shooter;
    Launcher *launcher;
    STATE_HANDLER_T state;
    STATE_HANDLER_T last_state;
} Baller;

extern Baller baller;

#endif
