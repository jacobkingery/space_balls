#include <p24FJ128GB206.h>
#include <stdint.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "pin.h"
#include "timer.h"

typedef void (*STATE_HANDLER_T)(void);

void shoot(void);
void rest(void);

STATE_HANDLER_T state, last_state;
uint16_t counter;

_LED *green_led;
_PIN *camera_pin;
_PIN *l_trigger;
_PIN *r_trigger;

void shoot(void) {
    if (state != last_state) {  // if we are entering the state, do initialization stuff
        last_state = state;
    }

    // Perform state tasks
    if (pin_read(camera_pin)){
        led_on(green_led);
    } else {
        led_off(green_led);
    }
    // Check for state transitions
    if (pin_read(l_trigger) || pin_read(r_trigger)) {
        state = rest;
    }

    if (state != last_state) {  // if we are leaving the state, do clean up stuff
        led_off(green_led);
    }
}

void rest(void) {
    if (state != last_state) {  // if we are entering the state, do intitialization stuff
        last_state = state;
    }

    // Check for state transitions
    if (!pin_read(l_trigger) || !pin_read(r_trigger)) {
        state = shoot;
    }

    /*if (state != last_state) {  // if we are leaving the state, do clean up stuff
    }*/
}

int16_t main(void) {
    init_clock();
    init_ui();
    init_pin();

    green_led = &led2;
    camera_pin = &D[13];
    l_trigger = &D[12];
    r_trigger = &D[11];

    state = rest;
    last_state = (STATE_HANDLER_T)NULL;

    while (1) {
        state();
    }
}
