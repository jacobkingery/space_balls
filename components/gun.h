#ifndef _GUN_H_
#define _GUN_H_
#include <stdint.h>
#include "pin.h"
#include "ui.h"
#include "state.h"

void init_gun(void);
void run_gun(void);
void shoot_gun(void);
void rest_gun(void);

typedef struct {
    _LED *shoot_led;
    _PIN *camera;
    STATE_HANDLER_T *state;
    STATE_HANDLER_T *last_state;
} Gun;

extern Gun gun;

#endif
