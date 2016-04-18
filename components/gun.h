#ifndef _GUN_H_
#define _GUN_H_

#include <stdint.h>
#include "pin.h"
#include "ui.h"
#include "timer.h"
#include "state.h"

void init_gun(_PIN *camera, _PIN *l_trigger, _PIN *r_trigger, _TIMER *rof_timer);
uint8_t run_gun(uint8_t level);
void over_gun(void);
void shoot_gun(void);
void rest_gun(void);

typedef struct {
    _PIN *camera;
    _PIN *l_trigger;
    _PIN *r_trigger;
    _TIMER *rof_timer;
    uint8_t level;
    uint8_t hit_flag;
    STATE_HANDLER_T state;
    STATE_HANDLER_T last_state;
} Gun;

extern Gun gun;

#endif
