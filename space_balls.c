#include <p24FJ128GB206.h>
#include <stdint.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "pin.h"
#include "timer.h"
#include "i2c.h"

#include "display.h"
#include "game.h"
#include "gun.h"
#include "launcher.h"
#include "audio.h"

int16_t main(void) {
    init_clock();
    init_ui();
    init_pin();
    init_timer();
    init_i2c();
    //init_display(&i2c1, 0x70, 0x71);
    init_game(&led2, &timer5);
    init_gun(&led1, &D[0], &D[1], &D[2]);
    init_launcher(&D[3], &D[4], &D[5]);
    init_audio(&D[6]);

    int level;
    while (1) {
        trigger_audio(SHOOT);
        /* level = run_game(); */
        /* run_gun(level); */
        /* run_launcher(level); */
    }
}
