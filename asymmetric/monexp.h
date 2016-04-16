/***********************************************************************
* FILENAME :        monexp.h   
*
* DESCRIPTION :
*       Header file of monexp.c, exposing the function and defining
*	some constants.
*
*
* NOTES : 
* 
* AUTHOR :    mraes     
*
*
*****/
#ifndef __MONEXP_H__
#define __MONEXP_H__

void montgomery_exponentiation(uint8_t *res, uint8_t *base, uint8_t *exponent,uint8_t exponent_length, uint8_t *n, uint8_t *rmodn, uint8_t *r2modn);
#endif

