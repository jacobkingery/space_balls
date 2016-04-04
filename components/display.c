#include <p24FJ128GB206.h>
#include "display.h"
#include "common.h"
#include "pin.h"
#include "spi.h"

Display display;

void set_display(uint8_t chunk, uint16_t value) {
    uint8_t values[] = {0, 0, 0, 0};
    uint8_t i = 3;
    while ((value > 0) && (i >= 0)) {
        values[i] = value % 10;
        value /= 10;
        i--;
    }

    uint8_t digit_reg;
    if (chunk == 0) {
        digit_reg = 0x63;
    } else {
        digit_reg = 0x67;
    }

    for (i=0; i<4; i++) {
        pin_clear(display.ncs);
        spi_transfer(display.spi, digit_reg);
        spi_transfer(display.spi, values[i]);
        pin_set(display.ncs);
        digit_reg--;
    }
}

void init_display(_SPI *spi, _PIN *ncs, _PIN *din, _PIN *clk) {
    display.spi = spi;
    display.ncs = ncs;
    display.din = din;
    display.clk = clk;

    pin_digitalOut(display.ncs);
    pin_digitalOut(display.din);
    pin_digitalOut(display.clk);
    pin_set(display.ncs);
    spi_open(display.spi, NULL, display.din, display.clk, 2e6, 1);
}
