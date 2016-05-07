#include <p24FJ128GB206.h>
#include <stdint.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "pin.h"
#include "timer.h"
#include "i2c.h"
#include "usb.h"
#include "oc.h"

#include "display.h"
#include "game.h"
#include "gun.h"
#include "shooter.h"
#include "launcher.h"
#include "baller.h"
#include "pix.h"
#include "audio.h"

int16_t main(void) {
    init_clock();
    init_ui();
    init_pin();
    init_timer();
    init_i2c();
    InitUSB();                              // initialize the USB registers and serial interface engine
    init_oc();

    init_display(&i2c3, 0x70, 0x71);
    init_game(&timer1, &timer2, &A[0], &disp1, &disp2);
    init_gun(&D[13], &A[1], &A[2], &timer3);
    init_launcher(&D[5], &D[8], &D[3], &D[4]);
    init_shooter(&D[6], &D[9], &D[1], &D[2], &D[7], &D[0], &oc3, &oc4);
    init_baller(&D[10], &oc2, &timer4, &shooter, &launcher);
    init_pix(&D[11], &timer5, 30, 0.05);
    init_audio(&D[12]);

    uint8_t level = 0;
    uint8_t hit_flag = 0;
    while (1) {
        ServiceUSB();
        level = run_game(hit_flag);
        ServiceUSB();
        hit_flag = run_gun(level);
        ServiceUSB();
        run_baller(level);
        ServiceUSB();
    }
}
