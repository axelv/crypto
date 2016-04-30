/***********************************************************************
* FILENAME :        monmult.h     
*
* DESCRIPTION :
*       Header file of monmult.c, exposing the function and defining
*	some constants.
*
*
* NOTES : 
* 
* AUTHOR :    mraes
*
*
*****/
#ifndef __MONMULT_H__
#define __MONMULT_H__
#define SIZE 156 // size of modulus in BYTES
#define MWORDSIZE 8 // Montgomery word size in bits
#define MONT_DEBUG 0

void montgomery_multiplication(uint8_t *res, uint8_t *in1, uint8_t *in2, uint8_t *n);
#endif

