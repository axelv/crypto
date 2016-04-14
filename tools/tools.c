#ifndef __TOOLS_H_
#define __TOOLS_H_
#include <stdint.h>

void int32_to_int8(uint8_t output[4], uint32_t input){
	output[0] = input;   
	output[1] = input >> 8; 
	output[2] = input >> 16;
	output[3] = input >> 24; 
}

#endif