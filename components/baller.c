#include <p24FJ128GB206.h>
#include <stdlib.h>
#include <math.h>
#include "launcher.h"
#include "shooter.h"
#include "baller.h"
#include "common.h"
#include "pin.h"
#include "ui.h"
#include "oc.h"

Baller baller;

#define E 2.71
#define ROL_LIMIT_MAX 100
#define CHAOS_LIMIT_MAX 100

void rest_baller(void) {
    if (baller.state != baller.last_state) {  // if we are entering the state, do intitialization stuff
        baller.last_state = baller.state;
    }

    // Check for state transitions
    if (timer_flag(baller.rol_timer)) {
        timer_lower(baller.rol_timer);
        baller.rol_ticks++;
        uint8_t chaos = rand()%100;
        if (chaos > baller.chaos_limit){
            baller.rol_ticks++;
        }
        if(baller.rol_ticks >= baller.rol_limit){
            if(rand()%2){
                if(!baller.shooter->shoot){
                    baller.shooter->shoot = 1;
                } else {
                    baller.launcher->launch = 1;
                }
            } else {
                if(!baller.launcher->launch){
                    baller.launcher->launch = 1;
                } else {
                    baller.shooter->shoot = 1;
                }
            }
        }
    }

    if (!baller.level){
        baller.state = over_baller;
    }

    /*if (state_baller != last_state_baller) {  // if we are leaving the state, do clean up stuff
      }*/
}

void over_baller(void) {
    if (baller.state != baller.last_state) {  // if we are entering the state, do intitialization stuff
        baller.last_state = baller.state;

        baller.rol_limit = 100;
        baller.rol_ticks = 0;
        timer_lower(baller.rol_timer);
        timer_stop(baller.rol_timer);
        pin_write(baller.sort_motor, 0);
        baller.launcher->over = 1;
        baller.shooter->over = 1;
        baller.level = 0;
    }

    // Check for state transitions
    if(baller.level){
        baller.state = rest_baller;
    }

    if (baller.state != baller.last_state) {  // if we are leaving the state, do clean up stuff
        timer_start(baller.rol_timer);
        pin_write(baller.sort_motor, 0xcccc);
        baller.launcher->over = 0;
        baller.shooter->over = 0;
    }
}

void init_baller(_PIN *sort_motor, _OC *sort_oc, _TIMER *rol_timer, Shooter *shooter, Launcher *launcher) {
    baller.rol_timer = rol_timer;
    baller.rol_limit = 100;
    baller.rol_ticks = 0;
    timer_setPeriod(baller.rol_timer, .01);
    timer_start(baller.rol_timer);

    baller.sort_motor = sort_motor;
    baller.sort_oc = sort_oc;
    oc_pwm(sort_oc, baller.sort_motor, NULL, 10e3, 0x0);

    //initialize random number generator
    srand(0);

    baller.shooter = shooter;
    baller.launcher = launcher;

    baller.level = 0;
    baller.state = over_baller;
    baller.last_state = (STATE_HANDLER_T)NULL;
}

void run_baller(uint8_t level) {
    baller.state();
    run_launcher();
    run_shooter();

    if (baller.level != level){
        baller.level = level;
        baller.shooter->level = level;
        baller.rol_limit = ROL_LIMIT_MAX - (.9*ROL_LIMIT_MAX/(1.0+pow(E, -baller.level+5)));
        baller.chaos_limit = .9*CHAOS_LIMIT_MAX/(1.0+pow(E, -baller.level+5));
    }
}
