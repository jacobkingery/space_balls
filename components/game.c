#include <p24FJ128GB206.h>
#include <math.h>
#include "usb.h"
#include "common.h"
#include "pin.h"
#include "ui.h"
#include "timer.h"
#include "game.h"
#include "display.h"
#include "audio.h"
#include "pix.h"

Game game;
void level_game(void) {
    if (game.state != game.last_state) {  // if we are entering the state_game, do initialization stuff
        game.last_state = game.state;
        game.level++;
        game.life = 15;
        game.decay_limit = 50 - floor(pow((double)game.level, 1.25));
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
        game.level_ticks = 0;
    }

    //run state logic

    // Check for state transitions
    if (timer_flag(game.decay_timer)) {
        timer_lower(game.decay_timer);
        game.decay_ticks++;
        if(game.decay_ticks == game.decay_limit){
            game.life--;
            game.decay_ticks = 0;
        }
        float life_percent = (float)game.life/100.0+(float)game.decay_ticks/game.decay_limit;
        pix_fill_frac_c(life_percent, &RED, NULL);
    }

    if (timer_flag(game.level_timer)) {
        timer_lower(game.level_timer);
        game.level_ticks++;
        if(game.level_ticks == game.level_limit){
            game.state = level_game;
        }
    }

    if (game.hit_flag){
        game.score += 100;
        write_display(game.score_display, game.score, 0);
    }

    if (!game.life){
        trigger_audio(LOSE);
        game.state = over_game;
    }
    if (game.state != game.last_state) {  // if we are leaving the state, do clean up stuff
    }
}

void over_game(void) {
    if (game.state != game.last_state) {  // if we are entering the state, do intitialization stuff
        game.last_state = game.state;

        game.level_ticks = 0;
        game.level_limit = 100;
        game.level = 0;

        game.decay_ticks = 0;
        game.decay_limit = 50;
        game.life = 100;
        pix_fill_frac_c(0, &RED, NULL);
        write_display(game.score_display, game.score, 0);
        /* blink_display(game.score_display, 1); */
        timer_lower(game.level_timer);
        timer_lower(game.decay_timer);
        timer_stop(game.level_timer);
        timer_stop(game.decay_timer);
        led_on(&led1);
        led_on(&led2);
        led_on(&led3);
    }

    //run state logic
    if (pin_read(game.coin_op)) {
        game.state = rest_game;
    }

    if (game.state != game.last_state) {  // if we are leaving the state, do clean up stuff
        led_off(&led1);
        led_off(&led2);
        led_off(&led3);
        /* blink_display(game.score_display, 0); */
        timer_start(game.level_timer);
        timer_start(game.decay_timer);
        game.level = 1;
        game.score = 0;
        trigger_audio(START);
    }
}

void init_game(_LED *level_led, _TIMER *level_timer, _TIMER *decay_timer, _PIN *coin_op, Display *score_display) {
    game.level_led = level_led;

    game.hit_flag = 0;
    game.score = 0;

    game.coin_op = coin_op;

    game.level_timer = level_timer;
    game.level_ticks = 0;
    game.level_limit = 100;
    game.level = 0;
    timer_setPeriod(game.level_timer, .1);
    timer_start(game.level_timer);

    game.decay_timer = decay_timer;
    game.decay_ticks = 0;
    game.decay_limit = 50;
    game.life = 30;
    timer_setPeriod(game.decay_timer, .1);
    timer_start(game.decay_timer);

    game.score_display = score_display;
    game.state = over_game;
    game.last_state = (STATE_HANDLER_T)NULL;
}

uint8_t run_game(uint8_t hit_flag) {
    ServiceUSB();                       // service any pending USB requests
    game.hit_flag = hit_flag;
    game.state();
    return game.level;
}
