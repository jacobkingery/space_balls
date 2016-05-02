#ifndef _LAUNCHER_H_
#define _LAUNCHER_H_
#include <stdint.h>
#include "pin.h"
#include "ui.h"
#include "oc.h"
#include "timer.h"
#include "state.h"

void init_launcher(_PIN *load_sensor, _PIN *launch_sensor, _PIN *launch_motor, _PIN *elevator_motor);
void run_launcher(void);
void over_launcher(void);
void rest_launcher(void);

typedef struct {
    _PIN *load_sensor;
    _PIN *launch_sensor;
    _PIN *launch_motor;
    _PIN *elevator_motor;
    uint8_t over;
    uint8_t launch;
    uint8_t loaded;
    STATE_HANDLER_T state;
    STATE_HANDLER_T last_state;
} Launcher;

extern Launcher launcher;

#endif
