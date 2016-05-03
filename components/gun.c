#include <p24FJ128GB206.h>
#include "gun.h"
#include "common.h"
#include "pin.h"
#include "timer.h"
#include "ui.h"
#include "audio.h"

Gun gun;

void shoot_gun(void) {
    if (gun.state != gun.last_state) {  // if we are entering the state, do initialization stuff
        gun.last_state = gun.state;
    }

    // Perform state tasks
    if (pin_read(gun.camera)){
        gun.hit_flag = 1;
        trigger_audio(HIT);
    } else {
        gun.hit_flag = 0;
        trigger_audio(SHOOT);
    }

    gun.state = rest_gun;
}

void rest_gun(void) {
    if (gun.state != gun.last_state) {  // if we are entering the state, do intitialization stuff
        gun.last_state = gun.state;
        gun.hit_flag = 0;
    }
    // Check for state transitions
    if (timer_flag(gun.rof_timer)){
        if ((pin_read(gun.l_trigger) || pin_read(gun.r_trigger))) {
            timer_lower(gun.rof_timer);
            gun.state = shoot_gun;
        }
    }

    if (!gun.level){
        gun.state = over_gun;
    }
    /*if (state_gun != last_state_gun) {  // if we are leaving the state, do clean up stuff
      }*/
}

void over_gun(void) {
    if ((pin_read(gun.l_trigger) || pin_read(gun.r_trigger))){
        gun.hit_flag = 1;
    } else {
        gun.hit_flag = 0;
    }
    if (gun.level) {
        gun.state = rest_gun;
    }
}

void init_gun(_PIN *camera, _PIN *l_trigger, _PIN *r_trigger, _TIMER *rof_timer) {
    gun.camera = camera;
    gun.l_trigger = l_trigger;
    gun.r_trigger = r_trigger;

    gun.rof_timer = rof_timer;
    timer_setPeriod(gun.rof_timer, .2);
    timer_start(gun.rof_timer);

    gun.state = over_gun;
    gun.last_state = (STATE_HANDLER_T)NULL;
}

uint8_t run_gun(uint8_t level) {
    gun.level = level;
    gun.state();
    return gun.hit_flag;
}
