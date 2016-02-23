#include <p24FJ128GB206.h>
#include <stdint.h>
#include "config.h"
#include "ui.h"
#include "pin.h"
#include "usb.h"

// USB communication encoding
#define GET_DISTANCE    1

WORD DISTANCE = (WORD) 0;

void get_distance() {
    /*
    Get IR sensor distance reading
    */
    // Read current pin and zero-center
    DISTANCE.w = pin_read(&A[0]);
}

void VendorRequests(void) {
    /*
    Handle USB vendor requests
    */
    switch (USB_setup.bRequest) {
        case GET_DISTANCE:
            BD[EP0IN].address[0] = DISTANCE.b[0];
            BD[EP0IN].address[1] = DISTANCE.b[1];
            BD[EP0IN].bytecount = 2;
            BD[EP0IN].status = 0xC8;
            break;
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void VendorRequestsIn(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void VendorRequestsOut(void) {
}

int16_t main(void) {
    init_clock();
    init_ui();
    init_pin();

    // Distance measurement pin
    pin_analogIn(&A[0]);

    // USB setup
    InitUSB();
    while (USB_USWSTAT!=CONFIG_STATE) {
        ServiceUSB();
    }

    // Main loop
    while (1) {
        get_distance();
        ServiceUSB();
    }
}
