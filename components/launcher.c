#include <p24FJ128GB206.h>
#include <stdlib.h>
#include <math.h>
#include "launcher.h"
#include "common.h"
#include "pin.h"
#include "ui.h"

Launcher launcher;

void launch_launcher(void) {
    if (launcher.state != launcher.last_state) {  // if we are entering the state, do initialization stuff
        launcher.last_state =  launcher.state;
        pin_write(launcher.launch_motor, 1);
    }

    // Check for state transitions
    if (!pin_read(launcher.launch_sensor)) {
        launcher.state = rest_launcher;
    }

    if (launcher.state != launcher.last_state) {  // if we are leaving the state, do clean up stuff
        pin_write(launcher.launch_motor, 0);
    }
}

void level_launcher(void) {
    if (launcher.state != launcher.last_state) {  // if we are entering the state, do initialization stuff
        launcher.last_state =  launcher.state;
        laucher.rol_limit = laucher.rol_limit - floor(launcher.level^1.5);
        chaos_limit = chaos_limit - floor(launcher.level^1.5);
    }

    // Check for state transitions
    if (!pin_read(launcher.launch_sensor)) {
        launcher.state = rest_launcher;
    }

    if (launcher.state != launcher.last_state) {  // if we are leaving the state, do clean up stuff
        pin_write(launcher.launch_motor, 0);
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
        chaos = rand()%100;
        if (chaos > chaos_limit){
            laucher.rol_ticks++;
        }
        if(launcher.rol_ticks >= launcher.rol_limit){
            if (!pin_read(launcher.load_sensor)) {
                launcher.state = launch_launcher;
            }
        }
    }

    /*if (state_launcher != last_state_launcher) {  // if we are leaving the state, do clean up stuff
      }*/
}

void init_launcher(_PIN *load_sensor, _PIN *launch_sensor, _PIN *launch_motor, _TIMER *rol_timer) {
    pin_digitalIn(load_sensor);
    pin_digitalIn(launch_sensor);
    pin_digitalOut(launch_motor);

    laucher.rol_timer = rol_timer;
    launcher.rol_limit = 100;
    launcher.rol_ticks = 0;
    timer_setPeriod(launcher.rol_timer, .01);
    timer_start(launcher.rol_timer);

    launcher.load_sensor = load_sensor;
    launcher.launch_sensor = launch_sensor;
    launcher.launch_motor = launch_motor;

    launcher.state = rest_launcher;
    launcher.last_state = (STATE_HANDLER_T)NULL;
}

void run_launcher(uint8_t level) {
    if (launcher.level != level){
        launcher.last_state = launcher.state;
        launcher.state = level_launcher;
    }
    launcher.level = level;
    launcher.state();
}
