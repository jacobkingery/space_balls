#ifndef _GAME_H_
#define _GAME_H_
#include <stdint.h>
#include "display.h"
#include "pin.h"
#include "ui.h"
#include "timer.h"
#include "state.h"

void init_game(_TIMER *level_timer, _TIMER *decay_timer, _PIN *coin_op, Display *score_display, Display *level_display);
uint8_t run_game(uint8_t hit_flag);
void over_game();
void rest_game(void);
void level_game(void);
void game_over(void);

typedef struct {
    _PIN *coin_op;
    Display *score_display;
    Display *level_display;
    _TIMER *level_timer;
    uint8_t level_ticks;
    uint8_t level_limit;
    uint8_t level;
    _TIMER *decay_timer;
    uint8_t decay_ticks;
    uint8_t decay_limit;
    uint8_t life;
    uint16_t score;
    uint8_t hit_flag;
    STATE_HANDLER_T state;
    STATE_HANDLER_T last_state;
} Game;

extern Game game;

#endif
