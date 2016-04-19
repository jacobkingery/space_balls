#include <p24FJ128GB206.h>
#include "audio.h"
#include "common.h"
#include "pin.h"
#include "usb.h"

void VendorRequests(void) {
    switch (USB_setup.bRequest) {
        case SEND_SOUND:
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0
            BD[EP0IN].status = audio.play;    // send packet as DATA1, set UOWN bit
            pin_clear(audio.request_pin);
            break;
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void trigger_audio(sound play){
    audio.play = play;
    pin_set(audio.request_pin);
}

void init_audio(_PIN *request_pin) {
    audio.play = start;
    pin_digitalOut(request_pin);
    audio.request_pin = request_pin;

    pin_clear(audio.request_pin);
}
