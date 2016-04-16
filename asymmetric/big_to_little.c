 /***********************************************************************
* FILENAME :        big_to_little.c
*
* DESCRIPTION :
*       Converting big endian strings (from Maple, MAGMA, toledo)
*       into little-endian, which is what our system needs.
* 
* AUTHOR :    mraes 
*
*
*****/

#define SIZE 156
#include <stdio.h>
#include <stdint.h>
int main(){
//uint8_t big[SIZE] = {0x5D,0x86,0x0B,0x70,0x1B,0xF7,0x62,0x69,0xEC,0xF6,0x99,0xCE,0x90,0xA9,0xB7,0x87,0xD3,0x55,0x45,0x72,0xFE,0xFA,0x90,0xBE,0x4D,0x45,0x9C,0x95,0x85,0x73,0xE0,0xAB,0xCC,0xCE,0x3C,0xF0,0x5F,0x18,0xCF,0xCA,0xC9,0xF6,0xAE,0x90,0x92,0x22,0x50,0x1A,0xCA,0x36,0x46,0x91,0xE9,0x8C,0x59,0xCF,0xEA,0x00,0x52,0x79,0x50,0x3C,0x0B,0x93,0x72,0xB5,0x89,0x1A,0x49,0xB1,0x1B,0x70,0x9F,0xED,0xB9,0x3F,0x32,0xBD,0x9A,0xFB,0xA7,0xF7,0x0E,0xC5,0x5C,0x1D,0x36,0xE5,0x3C,0xC3,0x8E,0x7E,0x16,0xDF,0x00,0x85,0x3F,0x00,0x71,0xDB,0xBB,0xCB,0x91,0xA5,0x70,0x03,0xEC,0x6B,0x09,0xF6,0xE6,0xA8,0x6B,0xF5,0x13,0x30,0x18,0xE6,0xEE,0x69,0xE2,0xFF,0x00,0xBB,0x9C,0x48,0xBF,0xC3,0x56,0x1B,0x2D,0xAE,0xBD,0xF5,0x2B,0x38,0xF6,0x3A,0x15,0x8D,0x24,0x95,0x07,0xC4,0xF3,0xF9,0xA0,0xB0,0x98,0x14,0xD4,0xAC,0xF8,0x5B,0x79,0x09};
uint8_t big[SIZE] = {0x3B,0x8F,0x18,0x10,0x37,0xB3,0x55,0x26,0xB4,0xB8,0xAA,0x39,0x15,0x2C,0x48,0xBE,0x0B,0x28,0x18,0x4B,0x5B,0xA6,0xB5,0x10,0xDD,0xF6,0x14,0x7C,0x48,0x3F,0xA3,0x96,0xF4,0x1E,0xAA,0xEA,0x12,0x4F,0xE8,0x0C,0xA7,0x93,0x55,0x65,0x18,0xD1,0x3E,0x67,0xA4,0xAE,0x44,0xC7,0x7F,0x6E,0x42,0x1B,0x30,0x91,0xCB,0x15,0x4D,0xFE,0x52,0x52,0xDC,0x81,0xDE,0x64,0xA1,0x71,0xB8,0xD4,0xD0,0x31,0xE4,0xDB,0xF4,0xB0,0x6D,0x4E,0x90,0x85,0x86,0xCE,0x72,0xE7,0x18,0x8A,0xFE,0x84,0x4A,0xFE,0x00,0x21,0x78,0x0A,0xB2,0xC9,0xA1,0x21,0xA8,0x8C,0x3C,0x6B,0x5C,0x83,0x3A,0xCE,0x99,0xE8,0x7C,0x3E,0x14,0x09,0x92,0xBF,0x43,0x55,0x03,0x83,0xC2,0x50,0x04,0xC3,0xF0,0x05,0xDF,0x24,0xBA,0xA4,0x7B,0x4C,0x52,0xD9,0x85,0x9D,0x63,0x54,0x05,0x84,0xAB,0x5B,0x23,0x54,0xDC,0xB8,0x3D,0x54,0xA8,0x97,0x4B,0xA6,0xA8,0x11,0xD4,0x0A};
uint8_t little[SIZE];
signed int i;
for(i=0;i<SIZE;i++){
	little[SIZE-1-i] = big[i];
}
printf("\nCONVERTED TO LITTLE ENDIAN:\n");
for(i=0;i<SIZE;i++){
	printf("0x%02X,",little[i]);
}
printf("\n");
return 0;
}