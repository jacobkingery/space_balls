#include <p24FJ128GB206.h>
#include "game.h"
#include "common.h"
#include "pin.h"
#include "ui.h"
#include "timer.h"

Game game;
void level_game(void) {
    if (game.state != game.last_state) {  // if we are entering the state_game, do initialization stuff
        game.last_state = game.state;
        game.level++;
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
    if (timer_flag(game.timer)) {
        timer_lower(game.timer);
        game.counter++;
        if(game.counter == 20){
            game.state = level_game;
        }
    }

    if (game.state != game.last_state) {  // if we are leaving the state, do clean up stuff
    }
}

void init_game(_LED *level_led, _TIMER *timer) {
    game.level_led = level_led;
    game.timer = timer;
    game.level = 0;
    game.counter = 0;
    game.state = rest_game;
    game.last_state = (STATE_HANDLER_T)NULL;

    timer_setPeriod(game.timer, .5);
    timer_start(game.timer);
}

uint8_t run_game() {
    game.state();
    return game.level;
}
