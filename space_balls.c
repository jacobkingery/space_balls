#include <p24FJ128GB206.h>
#include <stdint.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "pin.h"
#include "timer.h"

#include "game.h"
#include "gun.h"
#include "launcher.h"
#include "display.h"

int16_t main(void) {
    init_clock();
    init_ui();
    init_pin();
    init_timer();
    init_game(&led2, &timer5);
    init_gun(&led1, &D[0], &D[1], &D[2]);
    init_launcher(&D[3], &D[4], &D[5]);
    init_display(&spi1, &D[6], &D[7], &D[8]);

    int level;
    while (1) {
        set_display(1, 5555);
        // level = run_game();
        // run_gun(level);
        // run_launcher(level);
    }
}
