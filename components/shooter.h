#ifndef _SHOOTER_H_
#define _SHOOTER_H_
#include <stdint.h>
#include "pin.h"
#include "ui.h"
#include "oc.h"
#include "timer.h"
#include "state.h"

void init_shooter(_PIN *load_sensor, _PIN *push_sensor, _PIN *push_motor, _PIN *top_spin_motor, _PIN *back_spin_motor, _PIN *elevator_motor);
void run_shooter(void);
void over_shooter(void);
void rest_shooter(void);

typedef struct {
    _PIN *load_sensor;
    _PIN *push_sensor;
    _PIN *push_motor;
    _PIN *top_spin_motor;
    _PIN *back_spin_motor;
    _PIN *elevator_motor;
    uint8_t over;
    uint8_t shoot;
    STATE_HANDLER_T state;
    STATE_HANDLER_T last_state;
} Shooter;

extern Shooter shooter;

#endif
