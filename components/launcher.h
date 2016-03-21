#ifndef _LAUNCHER_H_
#define _LAUNCHER_H_
#include <stdint.h>
#include "pin.h"
#include "ui.h"
#include "state.h"

void init_launcher(void);
void run_launcher(void);
void load_launcher(void);
void launch_launcher(void);
void rest_launcher(void);

typedef struct {
    _SW *load_sensor;
    _SW *launch_sensor;
    _PIN *load_motor;
    _PIN *launch_motor;
    STATE_HANDLER_T *state;
    STATE_HANDLER_T *last_state;
} Launcher;

extern Launcher launcher;

#endif
