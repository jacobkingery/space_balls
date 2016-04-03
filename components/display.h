#ifndef _DISPLAY_H_
#define _DISPLAY_H_
#include <stdint.h>
#include "pin.h"
#include "spi.h"

void init_display(_SPI *spi, _PIN *cs, _PIN *din, _PIN *clk);
void set_display(uint8_t start, uint8_t value);

typedef struct {
    _SPI spi;
    _PIN cs;
    _PIN din;
    _PIN clk;
} Display;

extern Display display
#endif
