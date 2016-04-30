#include <p24FJ128GB206.h>
#include <stdlib.h>
#include "shooter.h"
#include "common.h"
#include "pin.h"
#include "ui.h"
#include "oc.h"

Shooter shooter;

void shoot_shooter(void) {
    if (shooter.state != shooter.last_state) {  // if we are entering the state, do initialization stuff
        shooter.last_state =  shooter.state;
        pin_set(shooter.push_motor);
    }

    // Check for state transitions
    if (!pin_read(shooter.push_sensor)) {
        shooter.state = rest_shooter;
    }

    if (shooter.over){
        shooter.state = over_shooter;
    }

    if (shooter.state != shooter.last_state) {  // if we are leaving the state, do clean up stuff
        pin_clear(shooter.push_motor);
        shooter.shoot = 0;
    }
}

void rest_shooter(void) {
    if (shooter.state != shooter.last_state) {  // if we are entering the state, do intitialization stuff
        shooter.last_state = shooter.state;
    }

    if (shooter.over){
        shooter.state = over_shooter;
    }

    if (shooter.shoot){
        shooter.state = over_shooter;
    }
}

void over_shooter(void) {
    if (shooter.state != shooter.last_state) {  // if we are entering the state, do intitialization stuff
        shooter.last_state = shooter.state;

        pin_clear(shooter.elevator_motor);
        led_on(&led1);
    }

    // Check for state transitions
    if(!shooter.over){
        shooter.state = rest_shooter;
    }
    if (shooter.state != shooter.last_state) {  // if we are leaving the state, do clean up stuff
        led_off(&led1);
        pin_set(shooter.elevator_motor);
    }
}

void init_shooter(_PIN *load_sensor, _PIN *push_sensor, _PIN *push_motor, _PIN *top_spin_motor, _PIN *back_spin_motor, _PIN *elevator_motor) {
    pin_digitalIn(load_sensor);
    pin_digitalIn(push_sensor);

    pin_digitalOut(push_motor);
    pin_digitalOut(elevator_motor);
    pin_digitalOut(top_spin_motor);
    pin_digitalOut(back_spin_motor);

    shooter.load_sensor = load_sensor;
    shooter.push_sensor = push_sensor;
    shooter.push_motor = push_motor;
    shooter.top_spin_motor = top_spin_motor;
    shooter.back_spin_motor = back_spin_motor;
    shooter.elevator_motor = elevator_motor;
    shooter.over = 1;
    shooter.shoot = 0;

    shooter.state = over_shooter;
    shooter.last_state = (STATE_HANDLER_T)NULL;
}

void run_shooter() {
    shooter.state();
}
