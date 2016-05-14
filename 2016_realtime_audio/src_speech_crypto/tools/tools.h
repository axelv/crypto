#ifndef __TOOLS_H_
#define __TOOLS_H_
#include <stdint.h>
#include <stdio.h>
#include "../constants.h"
void int32_to_int8(uint8_t output[4], uint32_t input);
void print_packet(uint8_t *packet, uint32_t length);
void convert_8_to_16(uint16_t *res, uint8_t *in);
void convert_16_to_8(uint8_t *res, uint16_t *in);
#endif
