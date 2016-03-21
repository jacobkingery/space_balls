#include <p24FJ128GB206.h>
#include "launcher.h"
#include "common.h"
#include "pin.h"
#include "ui.h"

Launcher launcher;

void launch_launcher(void) {
    if (launcher.state != launcher.last_state) {  // if we are entering the state_launcher, do initialization stuff
        launcher.last_state =  launcher.state;
        pin_write(launcher.launch_motor, 1);
    }

    // Check for state transitions
    if (!sw_read(launcher.launch_sensor)) {
        launcher.state = rest_launcher;
    }

    if (launcher.state != launcher.last_state) {  // if we are leaving the state, do clean up stuff
        pin_write(launcher.launch_motor, 0);
    }
}

void load_launcher(void) {
    if (launcher.state != launcher.last_state) {  // if we are entering the state, do intitialization stuff
        launcher.last_state = launcher.state;
        pin_write(launcher.load_motor, 1);
    }

    // Check for state transitions
    if (sw_read(launcher.load_sensor) == 0) {
        launcher.state = launch_launcher;
    }

    if (launcher.state != launcher.last_state) {  // if we are leaving the state, do clean up stuff
        pin_write(launcher.load_motor, 0);
    }
}

void rest_launcher(void) {
    if (launcher.state != launcher.last_state) {  // if we are entering the state, do intitialization stuff
        launcher.last_state = launcher.state;
    }

    // Check for state transitions
    if (1) {
        launcher.state = load_launcher;
    }

    /*if (state_launcher != last_state_launcher) {  // if we are leaving the state, do clean up stuff
      }*/
}

void init_launcher(_PIN *load_sensor, _PIN *launch_sensor, _PIN *load_motor, _Pin *launch_motor) {

    pin_digitalOut(load_motor);
    pin_digitalOut(launch_motor);

    launcher.load_sensor = load_sensor;
    launcher.launch_sensor = launch_sensor;
    launcher.load_motor = load_motor;
    launcher.launch_motor = launch_motor;

    launcher.state = load_launcher;
    launcher.last_state = (STATE_HANDLER_T)NULL;
}

void run_launcher(void) {
    launcher.state();
}
