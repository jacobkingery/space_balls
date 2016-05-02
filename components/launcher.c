#include <p24FJ128GB206.h>
#include <stdlib.h>
#include "launcher.h"
#include "common.h"
#include "pin.h"
#include "ui.h"
#include "oc.h"

Launcher launcher;

void launch_launcher(void) {
    if (launcher.state != launcher.last_state) {  // if we are entering the state, do initialization stuff
        launcher.last_state =  launcher.state;
    }

    if (!launcher.load_sensor){
        launcher.loaded = 1;
        led_on(&led1);
        pin_clear(launcher.elevator_motor);
    }

    if(launcher.loaded){
        pin_set(launcher.launch_motor);
        if (!pin_read(launcher.launch_sensor)) {
            launcher.state = rest_launcher;
        }
    }

    if (launcher.over){
        launcher.state = over_launcher;
    }

    if (launcher.state != launcher.last_state) {  // if we are leaving the state, do clean up stuff
        pin_clear(launcher.launch_motor);
        pin_set(launcher.elevator_motor);
        launcher.launch = 0;
        launcher.loaded = 0;
        led_off(&led1);
    }
}

void rest_launcher(void) {
    if (launcher.state != launcher.last_state) {  // if we are entering the state, do intitialization stuff
        launcher.last_state = launcher.state;
    }

    if (!launcher.load_sensor){
        launcher.loaded = 1;
        led_on(&led1);
        pin_clear(launcher.elevator_motor);
    }

    if (launcher.over){
        launcher.state = over_launcher;
    }

    if (launcher.launch){
        launcher.state = launch_launcher;
    }

    /*if (state_launcher != last_state_launcher) {  // if we are leaving the state, do clean up stuff
      }*/
}

void over_launcher(void) {
    if (launcher.state != launcher.last_state) {  // if we are entering the state, do intitialization stuff
        launcher.last_state = launcher.state;
        pin_clear(launcher.elevator_motor);
        if (pin_read(launcher.launch_sensor)) {
            pin_set(launcher.launch_motor);
        }
    }

    if (!pin_read(launcher.launch_sensor)){
        pin_clear(launcher.launch_motor);
    }

    // Check for state transitions
    if(!launcher.over){
        launcher.state = rest_launcher;
    }
    if (launcher.state != launcher.last_state) {  // if we are leaving the state, do clean up stuff
        led_off(&led1);
        pin_set(launcher.elevator_motor);
    }
}

void init_launcher(_PIN *load_sensor, _PIN *launch_sensor, _PIN *launch_motor, _PIN *elevator_motor) {
    pin_digitalIn(load_sensor);
    pin_digitalIn(launch_sensor);
    pin_digitalOut(launch_motor);
    pin_digitalOut(elevator_motor);

    launcher.load_sensor = load_sensor;
    launcher.launch_sensor = launch_sensor;
    launcher.launch_motor = launch_motor;
    launcher.elevator_motor = elevator_motor;
    launcher.over = 1;
    launcher.launch = 0;
    launcher.loaded = 0;

    launcher.state = over_launcher;
    launcher.last_state = (STATE_HANDLER_T)NULL;
}

void run_launcher() {
    launcher.state();
}
