#include <p24FJ128GB206.h>
#include "gun.h"
#include "common.h"
#include "pin.h"
#include "ui.h"

Gun gun;
void shoot_gun(void) {
    if (gun.state != gun.last_state) {  // if we are entering the state_gun, do initialization stuff
        gun.last_state = gun.state;
    }

    // Perform state tasks
    if (pin_read(gun.camera)){
        led_on(gun.shoot_led);
    } else {
        led_off(gun.shoot_led);
    }
    // Check for state transitions
    if (sw_read(&sw1) == 1) {
        gun.state = rest_gun;
    }

    if (gun.state != gun.last_state) {  // if we are leaving the state, do clean up stuff
        led_off(gun.shoot_led);
    }
}

void rest_gun(void) {
    if (gun.state != gun.last_state) {  // if we are entering the state, do intitialization stuff
        gun.last_state = gun.state;
    }

    // Check for state transitions
    if (pin_read(gun.l_trigger) && pin_read(gun.r_trigger)) {
        gun.state = shoot_gun;
    }

    /*if (state_gun != last_state_gun) {  // if we are leaving the state, do clean up stuff
      }*/
}

void init_gun(_LED *shoot_led, _PIN *camera, _PIN *l_trigger, _PIN *r_trigger) {
    gun.shoot_led = shoot_led;
    gun.camera = camera;
    gun.l_trigger = gun.l_trigger;
    gun.r_trigger = gun.r_trigger;

    gun.state = rest_gun;
    gun.last_state = (STATE_HANDLER_T)NULL;
}

void run_gun(uint8_t level) {
    gun.level = level;
    gun.state();
}
