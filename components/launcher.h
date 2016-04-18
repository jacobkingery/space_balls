#ifndef _LAUNCHER_H_
#define _LAUNCHER_H_
#include <stdint.h>
#include "pin.h"
#include "ui.h"
#include "oc.h"
#include "timer.h"
#include "state.h"

void init_launcher(_PIN *load_sensor, _PIN *launch_sensor, _PIN *launch_motor, _PIN *elevator_motor, _PIN *sort_motor, _OC *sort_oc, _TIMER *rol_timer);
void run_launcher(uint8_t level);
void over_launcher(void);
void level_launcher(void);
void rest_launcher(void);

typedef struct {
    _PIN *load_sensor;
    _PIN *launch_sensor;
    _PIN *launch_motor;
    _PIN *elevator_motor;
    _PIN *sort_motor;
    _OC *sort_oc;
    _TIMER *rol_timer;
    uint8_t rol_ticks;
    uint8_t rol_limit;
    uint8_t chaos;
    uint8_t chaos_limit;
    uint8_t level;
    STATE_HANDLER_T state;
    STATE_HANDLER_T last_state;
} Launcher;

extern Launcher launcher;

#endif
