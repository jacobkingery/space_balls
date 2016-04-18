#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>
#include "i2c.h"

typedef struct {
    _I2C *i2c;
    uint8_t addr;
} Display;

void command_display(Display *disp, uint8_t cmd);
void init_display(_I2C *i2c, uint8_t addr1, uint8_t addr2);
void write_display(Display *disp, uint16_t value, uint8_t colon);
void blink_display(Display *disp, uint8_t blink);

extern Display disp1, disp2;

#endif
