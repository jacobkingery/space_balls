#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <stdint.h>
#include "pin.h"
#include "ui.h"
#include "state.h"

typedef enum{
    START,
    SHOOT,
    HIT,
    LOSE
} Sound;

typedef enum{
    SEND_SOUND
} Request;

void init_audio(_PIN *request_pin);
void VendorRequests(void);
void VendorRequestsIn(void);
void VendorRequestsOut(void);
void trigger_audio(Sound play);

typedef struct {
    _PIN *request_pin;
    Sound play;
} Audio;

extern Audio audio;

#endif
