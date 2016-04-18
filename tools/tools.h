#ifndef __TOOLS_H_
#define __TOOLS_H_
#include <stdint.h>
#include <stdio.h>
#include "../constants.h"
void int32_to_int8(uint8_t output[4], uint32_t input);
void print_packet(uint8_t *packet, uint32_t length);

#endif