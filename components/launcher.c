#include <p24FJ128GB206.h>
#include <stdlib.h>
#include <math.h>
#include "launcher.h"
#include "common.h"
#include "pin.h"
#include "ui.h"
#include "oc.h"

Launcher launcher;

void launch_launcher(void) {
    if (launcher.state != launcher.last_state) {  // if we are entering the state, do initialization stuff
        launcher.last_state =  launcher.state;
        pin_set(launcher.launch_motor);
    }

    // Check for state transitions
    if (!pin_read(launcher.launch_sensor)) {
        launcher.state = rest_launcher;
    }

    if (launcher.state != launcher.last_state) {  // if we are leaving the state, do clean up stuff
        pin_clear(launcher.launch_motor);
    }

    if (!launcher.level){
        launcher.state = over_launcher;
    }
}

void rest_launcher(void) {
    if (launcher.state != launcher.last_state) {  // if we are entering the state, do intitialization stuff
        launcher.last_state = launcher.state;
    }

    // Check for state transitions
    if (timer_flag(launcher.rol_timer)) {
        timer_lower(launcher.rol_timer);
        launcher.rol_ticks++;
        uint8_t chaos = rand()%100;
        if (chaos > launcher.chaos_limit){
            launcher.rol_ticks++;
        }
        if(launcher.rol_ticks >= launcher.rol_limit){
            if (!pin_read(launcher.load_sensor)) {
                launcher.state = launch_launcher;
            }
        }
    }

    if (!launcher.level){
        launcher.state = over_launcher;
    }

    /*if (state_launcher != last_state_launcher) {  // if we are leaving the state, do clean up stuff
      }*/
}

void over_launcher(void) {
    if (launcher.state != launcher.last_state) {  // if we are entering the state, do intitialization stuff
        launcher.last_state = launcher.state;

        launcher.rol_limit = 100;
        launcher.rol_ticks = 0;
        timer_lower(launcher.rol_timer);
        timer_stop(launcher.rol_timer);
        pin_clear(launcher.elevator_motor);
        pin_write(launcher.sort_motor, 0);
        led_on(&led1);
    }

    // Check for state transitions
    if(launcher.level){
        launcher.state = rest_launcher;
    }
    if (launcher.state != launcher.last_state) {  // if we are leaving the state, do clean up stuff
        led_off(&led1);
        timer_start(launcher.rol_timer);
        pin_set(launcher.elevator_motor);
        pin_write(launcher.sort_motor, 0xffff);
    }
}

void init_launcher(_PIN *load_sensor, _PIN *launch_sensor, _PIN *launch_motor, _PIN *elevator_motor, _PIN *sort_motor, _OC *sort_oc, _TIMER *rol_timer) {
    pin_digitalIn(load_sensor);
    pin_digitalIn(launch_sensor);
    pin_digitalOut(launch_motor);
    pin_digitalOut(elevator_motor);

    launcher.rol_timer = rol_timer;
    launcher.rol_limit = 100;
    launcher.rol_ticks = 0;
    timer_setPeriod(launcher.rol_timer, .01);
    timer_start(launcher.rol_timer);

    launcher.load_sensor = load_sensor;
    launcher.launch_sensor = launch_sensor;
    launcher.launch_motor = launch_motor;
    launcher.elevator_motor = elevator_motor;
    launcher.sort_motor = sort_motor;

    launcher.sort_oc = sort_oc;

    oc_pwm(sort_oc, launcher.sort_motor, NULL, 10e3, 0x0);

    launcher.state = over_launcher;
    launcher.last_state = (STATE_HANDLER_T)NULL;
}

void run_launcher(uint8_t level) {
    launcher.state();
    if (launcher.level != level){
        launcher.level = level;
        launcher.rol_limit = launcher.rol_limit - (uint8_t)(pow((double)launcher.level, 1.25));
        launcher.chaos_limit = launcher.chaos_limit - (uint8_t)(pow((double)launcher.level, 1.25));
    }
}
