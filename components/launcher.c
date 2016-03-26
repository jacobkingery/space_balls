#include <p24FJ128GB206.h>
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

void rest_launcher(void) {
    if (launcher.state != launcher.last_state) {  // if we are entering the state, do intitialization stuff
        launcher.last_state = launcher.state;
    }

    // Check for state transitions
    if (!pin_read(launcher.load_sensor)) {
        launcher.state = launch_launcher;
    }

    /*if (state_launcher != last_state_launcher) {  // if we are leaving the state, do clean up stuff
      }*/
}

void init_launcher(_PIN *load_sensor, _PIN *launch_sensor, _PIN *launch_motor) {
    pin_digitalIn(load_sensor);
    pin_digitalIn(launch_sensor);
    pin_digitalOut(launch_motor);

    launcher.load_sensor = load_sensor;
    launcher.launch_sensor = launch_sensor;
    launcher.launch_motor = launch_motor;

    launcher.state = rest_launcher;
    launcher.last_state = (STATE_HANDLER_T)NULL;
}

void run_launcher(uint8_t level) {
    launcher.level = level;
    launcher.state();
}
