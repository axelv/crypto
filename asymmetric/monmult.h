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

// Montgomery Multiplication parameters

#define MON_MODULUS_LENGTH_IN_BITS 1024
#define MON_WORDSIZE 16 // Montgomery word size in bits
#define SIZE MON_MODULUS_LENGTH_IN_BITS/MON_WORDSIZE // size of modulus in BYTES


#define MONT_DEBUG 1



// Public Montgomery Multiplication functions
#if MON_WORDSIZE == 8
	#define MONWORD uint8_t
	void setup_monmult(MONWORD *n);
	void montgomery_multiplication(MONWORD *res, MONWORD *in1, MONWORD *in2, MONWORD *n);
#elif MON_WORDSIZE == 16
	#define MONWORD uint16_t
	void setup_monmult(MONWORD *n);
	void montgomery_multiplication(MONWORD *res, MONWORD *in1, MONWORD *in2, MONWORD *n);
#else
	#error "MWORDSIZE (monmult.h) is not 8 or 16"
#endif

#endif

