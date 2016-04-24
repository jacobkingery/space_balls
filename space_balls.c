#include <p24FJ128GB206.h>
#include <stdint.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "pin.h"
#include "timer.h"
#include "i2c.h"
#include "audio.h"
#include "usb.h"
#include "oc.h"

#include "display.h"
#include "game.h"
#include "gun.h"
#include "launcher.h"
#include "pix.h"

int16_t main(void) {
    init_clock();
    init_ui();
    init_pin();
    init_timer();
    init_i2c();
    InitUSB();                              // initialize the USB registers and serial interface engine
    init_oc();

    led_on(&led1);
    init_display(&i2c3, 0x70, 0x71);
    led_on(&led2);
    init_game(&led2, &timer1, &timer2, &D[0], &disp1);
    init_gun(&led1, &D[1], &D[2], &D[3], &timer3);
    init_launcher(&D[4], &D[5], &D[6], &D[8], &D[9], &oc2, &timer4);
    init_audio(&D[10]);
    init_pix(&D[11], &timer5, 30, 0.05);


    uint8_t level = 0;
    uint8_t hit_flag = 0;
    led_on(&led3);
    while (1) {
        level = run_game(hit_flag);
        hit_flag = run_gun(level);
        run_launcher(level);
    }
}
