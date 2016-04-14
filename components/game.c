#include <p24FJ128GB206.h>
#include <math.h>
#include "game.h"
#include "common.h"
#include "pin.h"
#include "ui.h"
#include "timer.h"
#include "audio.h"
#include "usb.h"

Game game;
void level_game(void) {
    if (game.state != game.last_state) {  // if we are entering the state_game, do initialization stuff
        game.last_state = game.state;
        game.level++;
        game.life = 15;;
        game.decay_limit = 50 - floor(game.level^1.25);
        led_toggle(game.level_led);
    }

    // Check for state transitions
    if (1) {
        game.state = rest_game;
    }

    /* if (game.state != game.last_state) {  // if we are leaving the state, do clean up stuff */
    /* } */
}

void decay_game(void) {
    if (game.state != game.last_state) {  // if we are entering the state_game, do initialization stuff
        game.last_state = game.state;
        game.life--;
        led_toggle(game.level_led);
    }

    // Check for state transitions
    if (1) {
        game.state = rest_game;
    }

    /* if (game.state != game.last_state) {  // if we are leaving the state, do clean up stuff */
    /* } */
}

void rest_game(void) {
    if (game.state != game.last_state) {  // if we are entering the state, do intitialization stuff
        game.last_state = game.state;
        game.counter = 0;
    }

    //run state logic

    // Check for state transitions
    if (timer_flag(game.level_timer)) {
        timer_lower(game.level_timer);
        game.level_ticks++;
        if(game.level_ticks == game.level_limit){
            game.state = level_game;
        }
    }

    if (timer_flag(game.decay_timer)) {
        timer_lower(game.decay_timer);
        game.decay_ticks++;
        if(game.decay_ticks == game.decay_limit){
            game.state = decay_game(void);
        }
    }

    if (game.state != game.last_state) {  // if we are leaving the state, do clean up stuff
    }
}

void init_game(_LED *level_led, _TIMER *timer, _TIMER *decay_timer) {
    game.level_led = level_led;

    game.hit_flag = 0;

    game.level_timer = level_timer;
    game.level_ticks = 0;
    game.level_limit = 100;
    game.level = 0;
    timer_setPeriod(game.level_timer, .1);
    timer_start(game.level_timer);

    game.decay_timer = decay_timer;
    game.decay_ticks = 0;
    game.decay_limit = 50;
    game.life = 15;
    timer_setPeriod(game.decay_timer, .1);
    timer_start(game.decay_timer);

    game.state = rest_game;
    game.last_state = (STATE_HANDLER_T)NULL;
}

uint8_t run_game(uint8_t hit_flag) {
    ServiceUSB();                       // service any pending USB requests
    game.hit_flag = hit_flag;
    game.state();
    return game.level;
}
