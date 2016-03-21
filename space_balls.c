#include <p24FJ128GB206.h>
#include <stdint.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "pin.h"
#include "timer.h"

#include "gun.h"
#include "launcher.h"

int16_t main(void) {
    init_clock();
    init_ui();
    init_pin();
    init_timer();
    init_gun(&led1, &D[0], &D[1], &D[2]);
    init_launcher(&D[3], &D[4], &D[5], &D[6]);

    while (1) {
        run_gun();
        run_launcher();
    }
}
