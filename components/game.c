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
#define MAX_LOSE 20
#define MAX_LIFE  100
#define MAX_LEVEL 100
#define E 2.71

Game game;
float life_percent;

Color start_color = {0, 50,0};
Color end_color = {255, 0, 0};

void level_game(void) {
    if (game.state != game.last_state) {  // if we are entering the state_game, do initialization stuff
        game.last_state = game.state;
        game.level++;
        game.decay_ticks = 0;
        game.level_ticks = 0;
        //https://www.wolframalpha.com/input/?i=.9*100*+1%2F(1%2Be%5E-x)+from+0+to+10
        game.decay_limit = MAX_DECAY - (.9*MAX_DECAY/(1.0+pow(E, -game.level+5)));
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
    update_bar_pix(life_percent, &start_color, &end_color);

    if (timer_flag(game.decay_timer)) {
        timer_lower(game.decay_timer);
        game.decay_ticks++;
        if (game.decay_ticks == game.decay_limit) {
            game.life--;
            game.decay_ticks = 0;
        }
    }

    if (timer_flag(game.level_timer)) {
        timer_lower(game.level_timer);
        game.level_ticks++;
        if (game.level_ticks == game.level_limit) {
            game.state = level_game;
        }
    }

    if (game.hit_flag) {
        game.score += 25;
        game.hit_flag = 0;
        game.life += 10;
        if (game.life > MAX_LIFE) {
            game.life = MAX_LIFE;
        }
        write_display(game.score_display, game.score, 0);
    }

    // Check for state transitions
    if (!game.life) {
        game.state = over_game;
        game.lose = 1;
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

        timer_lower(game.level_timer);
        timer_lower(game.decay_timer);
        timer_stop(game.level_timer);
        timer_stop(game.decay_timer);
        update_bar_pix(0, &start_color, &end_color);
        if (game.score > game.high) {
            game.high = game.score;
        }
        write_display(game.high_display, game.high, 0);
        write_display(game.score_display, game.score, 0);
        blink_display(game.score_display, 1);
        game.hit_flag = 0;
    }

    if (game.lose) {
        if (timer_flag(game.level_timer)) {
            timer_lower(game.level_timer);
            game.lose_ticks++;
            if (game.lose_ticks == MAX_LOSE) {
                trigger_audio(LOSE);
                game.lose = 0;
            }
        }
    }

    //run state logic
    if (pin_read(game.coin_op)) {
        game.coin_flag = 1;
        trigger_audio(START);
    }

    if (game.hit_flag && game.coin_flag) {
        game.state = rest_game;
        game.hit_flag = 0;
        game.coin_flag = 0;
    }

    if (game.state != game.last_state) {  // if we are leaving the state, do clean up stuff
        game.level = 1;
        game.life = MAX_LIFE;
        game.score = 0;
        game.lose = 0;
        blink_display(game.score_display, 0);
        write_display(game.score_display, game.score, 0);
        timer_start(game.level_timer);
        timer_start(game.decay_timer);
    }
}

void init_game(_TIMER *level_timer, _TIMER *decay_timer, _PIN *coin_op, Display *score_display, Display *high_display) {
    game.hit_flag = 0;
    game.coin_flag = 0;
    game.score = 0;
    game.high = 0;

    pin_digitalIn(coin_op);
    game.coin_op = coin_op;

    game.level_timer = level_timer;
    game.level_ticks = 0;
    game.level_limit = MAX_LEVEL;
    game.level = 0;

    game.lose = 0;
    game.lose_ticks = 0;

    timer_setPeriod(game.level_timer, 0.2);
    timer_start(game.level_timer);

    game.decay_timer = decay_timer;
    game.decay_ticks = 0;
    game.decay_limit = MAX_DECAY;
    game.life = MAX_LIFE;
    timer_setPeriod(game.decay_timer, 0.002);
    timer_start(game.decay_timer);

    game.score_display = score_display;
    game.high_display = high_display;
    write_display(game.high_display, game.high, 0);
    write_display(game.score_display, game.score, 0);

    game.state = over_game;
    game.last_state = (STATE_HANDLER_T)NULL;
}

uint8_t run_game(uint8_t hit_flag) {
    game.hit_flag = hit_flag;
    if (game.hit_flag) {
        led_on(&led3);
    } else {
        led_off(&led3);
    }
    game.state();
    return game.level;
}
