#ifndef GLOBALS_H
#define GLOBALS_H

/* represents buffer array elements: equivalent to BUFFERSIZE/2 temporal sample positions */
#define BUFFERSIZE 80	// MUST be <= 255 and a multiple of 2^ceil(log2(NSUBBANDS))

#define INPUTWAVFILE  "SBC/input.wav"
#define OUTPUTWAVFILE "SBC/output.wav"

#define VERBOSE

#define SHORTMAX 32767
#define SHORTMIN -32768

#endif /* GLOBALS_H */
