#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <stdint.h>
#include "pin.h"
#include "ui.h"
#include "state.h"

enum sound{start, shoot, hit, lose};

void init_audio(_PIN *request_pin);
void VendorRequests(void);
void trigger_audio(sound play);

typedef struct {
    _PIN *request_pin;
    sound play;
} Audio;

extern Audio audio;

#endif
