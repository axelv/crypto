#ifndef _PROCESS_AUDIO_H_
#define _PROCESS_AUDIO_H_

#include "globals.h"

#define BUFLEN 40  /* number of samples in the frame */

void init_audio();
void process_audio(int*, int*, int);

#endif
