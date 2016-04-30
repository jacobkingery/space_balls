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

#define MAX_DECAY 100
#define MAX_LIFE  100
#define MAX_LEVEL 100

Game game;
float life_percent;
Color cmap;

Color start_color = {0, 50,0};
Color end_color = {255, 0, 0};

void level_game(void) {
    if (game.state != game.last_state) {  // if we are entering the state_game, do initialization stuff
        game.last_state = game.state;
        game.level++;
        game.decay_ticks = 0;
        game.level_ticks = 0;
        game.decay_limit = MAX_DECAY - floor(pow((double)game.level, 2));
        write_display(game.level_display, game.level, 0);
    }

    game.state = rest_game;
}

void rest_game(void) {
    if (game.state != game.last_state) {  // if we are entering the state, do intitialization stuff
        game.last_state = game.state;
        game.level_ticks = 0;
    }

    //run state logic
    life_percent = (float)game.life/MAX_LIFE-((float)game.decay_ticks/game.decay_limit)/100.0;
    color_mix(&cmap, &start_color, &end_color, life_percent);
    pix_fill_frac_c(life_percent, &cmap, NULL);

    // Check for state transitions
    if (timer_flag(game.decay_timer)) {
        timer_lower(game.decay_timer);
        game.decay_ticks++;
        if(game.decay_ticks == game.decay_limit){
            game.life--;
            game.decay_ticks = 0;
        }
    }

    if (timer_flag(game.level_timer)) {
        timer_lower(game.level_timer);
        game.level_ticks++;
        if(game.level_ticks == game.level_limit){
            game.state = level_game;
        }
    }

    if (game.hit_flag){
        game.score += 25;
        game.life += 10;
        if (game.life > MAX_LIFE){
            game.life = MAX_LIFE;
        }
        write_display(game.score_display, game.score, 0);
    }

    if (!game.life){
        trigger_audio(LOSE);
        game.state = over_game;
    }
}

void over_game(void) {
    if (game.state != game.last_state) {  // if we are entering the state, do intitialization stuff
        game.last_state = game.state;

        game.level_ticks = 0;
        game.level_limit = MAX_LEVEL;
        game.level = 0;

        game.decay_ticks = 0;
        game.decay_limit = MAX_DECAY;
        game.life = MAX_LIFE;
        pix_fill_frac_c(0, &RED, NULL);
        write_display(game.score_display, game.score, 0);
        write_display(game.level_display, game.level, 0);
        blink_display(game.score_display, 1);
        timer_lower(game.level_timer);
        timer_lower(game.decay_timer);
        timer_stop(game.level_timer);
        timer_stop(game.decay_timer);
        led_off(&led3);
    }

    //run state logic
    if (pin_read(game.coin_op)) {
        game.state = rest_game;
    }

    if (game.state != game.last_state) {  // if we are leaving the state, do clean up stuff
        led_on(&led3);
        game.level = 1;
        game.score = 0;
        blink_display(game.score_display, 0);
        write_display(game.level_display, game.level, 0);
        write_display(game.score_display, game.score, 0);
        timer_start(game.level_timer);
        timer_start(game.decay_timer);
        trigger_audio(START);
    }
}

void init_game(_TIMER *level_timer, _TIMER *decay_timer, _PIN *coin_op, Display *score_display, Display *level_display) {

    game.hit_flag = 0;
    game.score = 0;

    game.coin_op = coin_op;

    game.level_timer = level_timer;
    game.level_ticks = 0;
    game.level_limit = MAX_LEVEL;
    game.level = 0;
    timer_setPeriod(game.level_timer, .2);
    timer_start(game.level_timer);

    game.decay_timer = decay_timer;
    game.decay_ticks = 0;
    game.decay_limit = MAX_DECAY;
    game.life = 0;
    timer_setPeriod(game.decay_timer, .001);
    timer_start(game.decay_timer);

    game.score_display = score_display;
    game.level_display = level_display;
    game.state = over_game;
    game.last_state = (STATE_HANDLER_T)NULL;
}

uint8_t run_game(uint8_t hit_flag) {
    ServiceUSB();                       // service any pending USB requests
    game.hit_flag = hit_flag;
    game.state();
    return game.level;
}
