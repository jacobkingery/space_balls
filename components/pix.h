#ifndef _PIX_H_
#define _PIX_H_

#include <p24FJ128GB206.h>
#include "common.h"
#include "ui.h"
#include "oc.h"
#include "timer.h"
#define MAX_PIXELS 200

typedef struct {
    uint8_t count;
    uint8_t data[3 * MAX_PIXELS];
    _PIN *pin;
    // _OC *oc; // Note: Only programmed for oc1.  Any others will screw up
    _TIMER *timer;
    volatile uint16_t bit_position;
    volatile uint8_t is_dirty;
} Pixels;
extern Pixels pix;

// Color struct.  These can be used for any of the color_ functions
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Color;

// Predefined colors that can be used anywhere.
extern Color RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN, BLACK, WHITE, __fill, __empty;

void __attribute__((interrupt, auto_psv)) _OC1Interrupt(void);
void __redraw_pixels(_TIMER *timer);

void init_pix(_PIN *pin, _TIMER *timer, uint8_t count, float fps);

void pix_write(uint8_t pixel, uint8_t red, uint8_t green, uint8_t blue);
void pix_write_c(uint8_t pixel, Color *color);

void pix_get(uint8_t dst[3], uint8_t pos);
void pix_get_c(Color *dst, uint8_t pos);

void pix_write_all(uint8_t red, uint8_t green, uint8_t blue);
void pix_write_all_c(Color *color);

void pix_fill_range(uint8_t start, uint8_t stop, uint8_t red, uint8_t green, uint8_t blue);
void pix_fill_range_c(uint8_t start, uint8_t stop, Color *color);

void pix_fill_frac(float frac, uint8_t red, uint8_t green, uint8_t blue);
void pix_fill_frac_c(float frac, Color *fill_color, Color *empty_color);
void update_bar_pix(float frac, Color *fill_color, Color *empty_color);

void __test_pixels_1(_TIMER *timer);
void __test_pixels_2(_TIMER *timer);

// Color specific functions
// First argument is always destination color.
// These MUST HANDLE thresholding.
void color_copy(Color *dst, Color *src);
void color_shift(Color *dst, Color *src, int rshift);
void color_shift_half(Color *dst, Color *src, int halfshifts);
void color_shift_fourth(Color *dst, Color *src, int fourthshifts);
void color_mix(Color *dst, Color *src1, Color *src2, float frac_src1);
void color_add(Color *dst, Color *src1, Color *src2); // src1 + src2
void color_sub(Color *dst, Color *src1, Color *src2); // src1 - src2
void color_gray(Color *dst, Color *src);
void color_thresh_f(Color *dst, float red, float green, float blue);
void color_thresh_i(Color *dst, int red, int green, int blue);

#endif
