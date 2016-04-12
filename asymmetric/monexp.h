/***********************************************************************
* FILENAME :        monexp.h       VERSION: 1.0
*
* DESCRIPTION :
*       Header file of monexp.c, exposing the function and defining
*	some constants.
*
*
* NOTES : 
* 
* AUTHOR :    mraes        START DATE :    25 Mar 16
*
*
*****/
#ifndef __MONEXP_H__
#define __MONEXP_H__
#define EXPONENT_LENGTH 2 //in bytes
#define DEXPONENT_LENGTH 128 // decryption exponent

void montgomery_exponentiation(uint8_t *res, uint8_t *base, uint8_t *exponent,uint8_t exponent_length, uint8_t *n);
#endif

