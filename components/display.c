#include <p24FJ128GB206.h>
#include "display.c.h"
#include "common.h"
#include "pin.h"
#include "spi.h"

Display display;

void set_display(uint8_t chunk, uint16_t value){
    int values = [0, 0, 0, 0];
    int i = 3;
    while((value > 0) && ( i>= 0)){
        values[i] = value % 10;
        value /= 10;
        i--;
    }
    if(chunk = 0){
        uint8_t digit_reg = 0x63;
    } else {
        uint8_t digit_reg = 0x67;
    }

    for(i=0; i<4, i++){
        pin_clear(display.cs);
        spi_transfer(display.spi, digit_reg);
        spi_transfer(display.spi, values[i]);
        pin_set(display.cs);
        digit_reg--;
    }
}

void init_display(_SPI *spi, _PIN *cs, _PIN *din, _PIN *clk){
    display.spi = spi;
    display.cs = cs;
    display.din = din;
    display.clk = clk;

    pin_digitalOut(display.cs);
    pin_set(display.cs);
    spi_open(display.spi, NULL, display.din, display.clk, 5e6, 1);
}
