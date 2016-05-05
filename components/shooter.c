#include <p24FJ128GB206.h>
#include <stdlib.h>
#include <math.h>
#include "shooter.h"
#include "common.h"
#include "pin.h"
#include "ui.h"
#include "oc.h"

Shooter shooter;

#define E 2.71
#define MAX_SPEED (uint16_t)0xffff*.5

uint16_t speed_range = .75 * MAX_SPEED /(1.0 + pow(E,5.0));
uint16_t top_speed = 0;
uint16_t back_speed = 0;

uint32_t push_counter = 0;
void shoot_shooter(void) {
    if (shooter.state != shooter.last_state) {  // if we are entering the state, do initialization stuff
        shooter.last_state =  shooter.state;
        //Sigmoid function to define range of spinner variation
        //https://www.wolframalpha.com/input/?i=(1%2F(1%2B2.7%5E(-x%2B5)))%2F1.5+from+0+to+10
        speed_range = .75 * MAX_SPEED /(1.0 + pow(E,(-shooter.level+5)));
        top_speed = MAX_SPEED - (rand()%speed_range);
        back_speed = MAX_SPEED - (rand()%speed_range);
    }

    if (!pin_read(shooter.load_sensor)){
        shooter.loaded = 1;
        pin_clear(shooter.elevator_motor);
        led_on(&led2);
    }

    if(shooter.loaded){
        push_counter++;
        pin_set(shooter.push_motor);
        pin_write(shooter.top_spin_motor, top_speed);
        pin_write(shooter.back_spin_motor, back_speed);
        if (!pin_read(shooter.push_sensor) && push_counter>100) {
            shooter.state = rest_shooter;
        }
    }

    if (shooter.over){
        shooter.state = over_shooter;
    }

    if (shooter.state != shooter.last_state) {  // if we are leaving the state, do clean up stuff
        pin_clear(shooter.push_motor);
        pin_set(shooter.elevator_motor);
        shooter.shoot = 0;
        shooter.loaded = 0;
        led_off(&led2);
        push_counter = 0;
    }
}

void rest_shooter(void) {
    if (shooter.state != shooter.last_state) {  // if we are entering the state, do intitialization stuff
        shooter.last_state = shooter.state;
    }

    if (!pin_read(shooter.load_sensor)){
        shooter.loaded = 1;
        pin_clear(shooter.elevator_motor);
        led_on(&led2);
    }
    if (shooter.over){
        shooter.state = over_shooter;
    }

    if (shooter.shoot){
        shooter.state = shoot_shooter;
        led_on(&led2);
    }
}

void over_shooter(void) {
    if (shooter.state != shooter.last_state) {  // if we are entering the state, do intitialization stuff
        shooter.last_state = shooter.state;

        pin_clear(shooter.elevator_motor);
        pin_write(shooter.top_spin_motor, 0);
        pin_write(shooter.back_spin_motor, 0);
        shooter.level = 0;
    }

    // Check for state transitions
    if(!shooter.over){
        shooter.state = rest_shooter;
    }

    if (shooter.state != shooter.last_state) {  // if we are leaving the state, do clean up stuff
        pin_set(shooter.elevator_motor);
        pin_write(shooter.top_spin_motor, MAX_SPEED*.75);
        pin_write(shooter.back_spin_motor, MAX_SPEED*.75);
    }
}

void init_shooter(_PIN *load_sensor, _PIN *push_sensor, _PIN *push_motor, _PIN *elevator_motor, _PIN *top_spin_motor, _PIN *back_spin_motor, _OC *top_spin_oc, _OC *back_spin_oc) {
    srand(0);
    pin_digitalIn(load_sensor);
    pin_digitalIn(push_sensor);

    pin_digitalOut(push_motor);
    pin_digitalOut(elevator_motor);

    oc_pwm(top_spin_oc, top_spin_motor, NULL, 10e3, 0x0);
    oc_pwm(back_spin_oc, back_spin_motor, NULL, 10e3, 0x0);

    shooter.load_sensor = load_sensor;
    shooter.push_sensor = push_sensor;
    shooter.push_motor = push_motor;
    shooter.elevator_motor = elevator_motor;

    shooter.top_spin_motor = top_spin_motor;
    shooter.back_spin_motor = back_spin_motor;

    shooter.over = 1;
    shooter.shoot = 0;
    shooter.loaded = 0;
    shooter.level = 0;

    shooter.state = over_shooter;
    shooter.last_state = (STATE_HANDLER_T)NULL;
}

void run_shooter() {
    shooter.state();
}
