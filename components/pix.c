#include "pix.h"
#include <p24FJ128GB206.h>
#include "common.h"
#include "ui.h"
#include "oc.h"
#include "timer.h"

// #define PIXEL_COUNT 29
// #define PIXEL_PIN 9
#define PIXEL_FREQ 1.5e5 // 0.15 MHz pwm freq.  6.667 microsecond period
#define DUTY_0 0x0001 // Changes oc1's duty cycle to a 0 bit
#define DUTY_1 0x000F // Changes oc1's duty cycle to a 1 bit
#define BIT_SHIFT 7

Pixels pix;
Color RED     = {255,   0,   0};
Color GREEN   = {  0, 255,   0};
Color BLUE    = {  0,   0, 255};
Color YELLOW  = {255, 255,   0};
Color MAGENTA = {255,   0, 255};
Color CYAN    = {  0, 255, 255};
Color BLACK   = {  0,   0,   0};
Color WHITE   = {255, 255, 255};

// Sourced: https://github.com/amuthaOlin/projectElecanisms/blob/master/lib/leds.c lines 45-58
void __attribute__((interrupt, auto_psv)) _OC1Interrupt(void) {
    disable_interrupts();

    bitclear(&IFS0, 2); // Reset interrupt, I think?
    // Set duty cycle depending on bit.
    OC1R = bitread(&pix.data[(uint16_t)(pix.bit_position/8)], 7-(pix.bit_position%8)) ? DUTY_1 : DUTY_0;

    pix.bit_position++;

    // Finished with loop. Exiting.
    if (pix.bit_position == BIT_SHIFT) {
        bitclear(&IEC0, 2); // disable OC1 interrupt
        oc_free(&oc1);
    }
    // Reset (Account for barrel shift)
    if (pix.bit_position == 24 * (pix.count)) {
        pix.bit_position = 0;
    }
    enable_interrupts();
}

void __redraw_pixels(_TIMER *timer){
    // Only redraw pixels if a change has been marked.  Setting this flag will queue a change.
    // This is to save processor time for
    if(pix.is_dirty){
        pix.is_dirty = 0;
        oc_pwm(&oc1, pix.pin, NULL, PIXEL_FREQ, 0);
        bitset(&IEC0, 2); // enable OC1 interrupt.
    }
}

void init_pix(_PIN *pin, _TIMER *timer, uint8_t count, float fps) {
    // Argument setups.
    pix.timer = timer;
    pix.pin = pin;
    pix.count = count;

    // Generic setup
    pix.bit_position = BIT_SHIFT;
    pix.is_dirty = 1;

    timer_every(timer, fps, __redraw_pixels); // Refresh rate of 20 FPS
    IPC0 |= 0x0300; // OC1 interrupt low priority
    pix_write_all_c(&BLACK);
}


void pix_write(uint8_t pixel, uint8_t red, uint8_t green, uint8_t blue) {
    // Green pixel only has the two most significant bits defined it seems.
    // Not only that, they're reversed and put into a really weird position in the bits.
    // val > binary    > Pixel Green
    // 128 > 1000 0000 > 0000 0010
    //  64 > 0100 0000 > 0000 0100

    if(pixel == 0){
        pix.data[0] = ((green & 128) ? 2 : 0) 
                    + ((green &  64) ? 4 : 0);
    }
    else {
        pix.data[3 * pixel] = green;
    }
    pix.data[3 * pixel + 1] = red;
    pix.data[3 * pixel + 2] = blue;
    pix.is_dirty = 1;
}

void pix_write_c(uint8_t pixel, Color *color){
    pix_write(pixel, color->red, color->green, color->blue);
}

void pix_get(uint8_t dst[3], uint8_t pos){
    dst[0] = pix.data[3 * pos + 1];
    dst[1] = pix.data[3 * pos    ];
    dst[2] = pix.data[3 * pos + 2];
}

void pix_get_c(Color *dst, uint8_t pos){
    dst->red   = pix.data[3 * pos + 1];
    dst->green = pix.data[3 * pos    ];
    dst->blue  = pix.data[3 * pos + 2];
}

void pix_write_all(uint8_t red, uint8_t green, uint8_t blue) {
    uint8_t i;
    for(i = 0; i < pix.count; ++i){
        pix_write(i, red, green, blue);
    }
    pix.is_dirty = 1;
}

void pix_write_all_c(Color *color){
    pix_write_all(color->red, color->green, color->blue);
}

void pix_fill_range(uint8_t start, uint8_t stop, uint8_t red, uint8_t green, uint8_t blue){
    start = start > 0 ? start : 0;
    stop = stop < pix.count ? stop : pix.count;

    uint8_t i;
    for(i = start; i < stop; ++i){
        pix_write(i, red, green, blue);
    }
}

void pix_fill_range_c(uint8_t start, uint8_t stop, Color *color){
    pix_fill_range(start, stop, color->red, color->green, color->blue);
}

void pix_fill_frac(float frac, uint8_t red, uint8_t green, uint8_t blue){
    uint8_t cutoff = (uint8_t)(frac * pix.count + 0.5);
    pix_fill_range(0, cutoff, red, green, blue);
    pix_fill_range(cutoff, pix.count, 0, 0, 0);
    pix.is_dirty = 1;
}

void pix_fill_frac_c(float frac, Color *fill_color, Color *empty_color){
    uint8_t cutoff = (uint8_t)(frac * pix.count);
    Color mixed;
    float diff = frac * pix.count - cutoff;

    pix_fill_range_c(0, cutoff, fill_color);
    if(empty_color){
        // Handle cutoff color
        color_mix(&mixed, fill_color, empty_color, diff);
        pix_fill_range_c(cutoff + 1, pix.count, empty_color);
    }
    else{
        diff = 32 - diff * 32;
        color_shift_fourth(&mixed, fill_color, (int)diff);
        pix_fill_range_c(cutoff + 1, pix.count, &BLACK);
    }

    pix_write_c(cutoff, &mixed);
    pix.is_dirty = 1;
}

// Approximately caclulates a quarter bit shift
void color_shift_fourth(Color *dst, Color *src, int fourthshifts){
    if(fourthshifts % 2 == 0){
        color_shift_half(dst, src, fourthshifts / 2);
    }

    Color mostly;
    color_shift_half(&mostly, src, fourthshifts / 2);
    dst->red   = mostly.red   * 0.8409;
    dst->green = mostly.green * 0.8409;
    dst->blue  = mostly.blue  * 0.8409;    
}

// Approximately calculates a half bit shift.
void color_shift_half(Color *dst, Color *src, int halfshifts){
    if(halfshifts % 2 == 0){
        color_shift(dst, src, halfshifts / 2);
        return;
    }

    Color mostly;
    color_shift(&mostly, src, halfshifts / 2);
    dst->red   = mostly.red   * 0.7071;
    dst->green = mostly.green * 0.7071;
    dst->blue  = mostly.blue  * 0.7071;
}

void color_thresh_f(Color *dst, float red, float green, float blue){
    red   = red   >   0 ? red   :   0;
    red   = red   < 255 ? red   : 255;
    green = green >   0 ? green :   0;
    green = green < 255 ? green : 255;
    blue  = blue  >   0 ? blue  :   0;
    blue  = blue  < 255 ? blue  : 255;
    dst->red   = (uint8_t)red;
    dst->green = (uint8_t)green;
    dst->blue  = (uint8_t)blue;    
}

void color_thresh_i(Color *dst, int red, int green, int blue){
    red   = red   >   0 ? red   :   0;
    red   = red   < 255 ? red   : 255;
    green = green >   0 ? green :   0;
    green = green < 255 ? green : 255;
    blue  = blue  >   0 ? blue  :   0;
    blue  = blue  < 255 ? blue  : 255;
    dst->red   = (uint8_t)red;
    dst->green = (uint8_t)green;
    dst->blue  = (uint8_t)blue;
}

void color_shift(Color *dst, Color *src, int rshift){
    int red   = src->red   >> rshift;
    int green = src->green >> rshift;
    int blue  = src->blue  >> rshift;

    color_thresh_i(dst, red, green, blue);
}

void color_copy(Color *dst, Color *src){
    dst->red   = src->red;
    dst->green = src->green;
    dst->blue  = src->blue;
}

// Linear mixing.  Works best with colors of similar saturation
void color_mix(Color *dst, Color *src1, Color *src2, float frac_src1){
    float frac_src2 = 1.0 - frac_src1;
    float red   = (src1->red   * frac_src1) + (src2->red   * frac_src2);
    float green = (src1->green * frac_src1) + (src2->green * frac_src2);
    float blue  = (src1->blue  * frac_src1) + (src2->blue  * frac_src2);
    color_thresh_f(dst, red, green, blue);
}

void color_add(Color *dst, Color *src1, Color *src2){
    int red   = src1->red   + src2->red;
    int green = src1->green + src2->green;
    int blue  = src1->blue  + src2->blue;
    color_thresh_i(dst, red, green, blue);
}

void color_sub(Color *dst, Color *src1, Color *src2){
    int red   = src1->red   - src2->red;
    int green = src1->green - src2->green;
    int blue  = src1->blue  - src2->blue;
    color_thresh_i(dst, red, green, blue);    
}

// Luminosity calculated from http://www.johndcook.com/blog/2009/08/24/algorithms-convert-color-grayscale/
void color_gray(Color *dst, Color *src){
    float luminosity = 
        0.21 * src->red   + 
        0.72 * src->green +
        0.07 * src->blue;
    color_thresh_f(dst, luminosity, luminosity, luminosity);
}

// Health bar simulator
// Starts off full green.  Ticks down and fills with red.
// Changes to yellow when halfway.  Continues ticking down.
float __test_frac = 1.00;
Color __fill, __empty;

void __test_pixels_1(_TIMER *timer){
    color_mix(&__fill, &GREEN, &RED, __test_frac);
    pix_fill_frac_c(__test_frac, &__fill, NULL);
    __test_frac -= 0.001;
}

// RGB rotation.  Iterates through Red, Green, Blue and sets new colors
uint8_t __test_state = 0;
uint8_t __test_pixel_idx = 0;
void __test_pixels_2(_TIMER *timer) {
    switch(__test_state){
        case 0:
            pix_write_c(__test_pixel_idx, &RED);
            break;
        case 1:
            pix_write_c(__test_pixel_idx, &GREEN);
            break;
        case 2:
            pix_write_c(__test_pixel_idx, &BLUE);
            break;
    }
    __test_pixel_idx = (__test_pixel_idx + 1) % pix.count;
    __test_state = (__test_state + 1) % 3;
    pix.is_dirty = 1;
}