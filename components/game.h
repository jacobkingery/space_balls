#ifndef _GAME_H_
#define _GAME_H_
#include <stdint.h>
#include "pin.h"
#include "ui.h"
#include "timer.h"
#include "state.h"

void init_game(_LED *level_led, _TIMER *timer);
uint8_t run_game(void);
void rest_game(void);
void level_game(void);
void game_over(void);

typedef struct {
    _LED *level_led;
    _TIMER *timer;
    uint8_t counter;
    uint8_t level;
    STATE_HANDLER_T state;
    STATE_HANDLER_T last_state;
} Game;

extern Game game;

#endif
