#include <p24FJ128GB206.h>
#include <stdint.h>
#include "common.h"
#include "i2c.h"
#include "display.h"

Display disp1, disp2;

uint8_t numbertable[] = {
    0x3F, /* 0 */
    0x06, /* 1 */
    0x5B, /* 2 */
    0x4F, /* 3 */
    0x66, /* 4 */
    0x6D, /* 5 */
    0x7D, /* 6 */
    0x07, /* 7 */
    0x7F, /* 8 */
    0x6F, /* 9 */
    0x77, /* a */
    0x7C, /* b */
    0x39, /* C */
    0x5E, /* d */
    0x79, /* E */
    0x71, /* F */
};

void command_display(Display *disp, uint8_t cmd) {
    uint8_t addr = (disp->addr << 1) | 0;     // Write mode
    i2c_start(disp->i2c);
    i2c_putc(disp->i2c, addr);
    i2c_putc(disp->i2c, cmd);
    i2c_stop(disp->i2c);
}

void blink_display(Display *disp, uint8_t blink) {
    if (blink) {
        command_display(disp, 0x83);
    } else {
        command_display(disp, 0x81);
    }
}

void write_display(Display *disp, uint16_t value, uint8_t colon) {
    uint8_t addr = (disp->addr << 1) | 0;     // Write mode
    i2c_start(disp->i2c);
    i2c_putc(disp->i2c, addr);
    i2c_putc(disp->i2c, 0x00);      // Set data address pointer to 0

    i2c_putc(disp->i2c, numbertable[value / 1000]);         // Thousands
    i2c_putc(disp->i2c, 0x00);
    i2c_putc(disp->i2c, numbertable[(value / 100) % 10]);   // Hundreds
    i2c_putc(disp->i2c, 0x00);
    i2c_putc(disp->i2c, 0x02 & (colon << 1));               // Colon
    i2c_putc(disp->i2c, 0x00);
    i2c_putc(disp->i2c, numbertable[(value / 10) % 10]);    // Tens
    i2c_putc(disp->i2c, 0x00);
    i2c_putc(disp->i2c, numbertable[value % 10]);           // Ones
    i2c_putc(disp->i2c, 0x00);

    i2c_stop(disp->i2c);
}

void init_display(_I2C *i2c, uint8_t addr1, uint8_t addr2) {
    i2c_open(i2c, 3e5);
    disp1.i2c = i2c;
    disp2.i2c = i2c;
    disp1.addr = addr1;
    disp2.addr = addr2;

    // Turn on system oscillator
    command_display(&disp1, 0x21);
    // Set blinking to off, display to on
    command_display(&disp1, 0x81);
    // Set brightness to max
    command_display(&disp1, 0xEF);

    // Turn on system oscillator
    command_display(&disp2, 0x21);
    // Set blinking to off, display to on
    command_display(&disp2, 0x81);
    // Set brightness to max
    command_display(&disp2, 0xEF);
}
