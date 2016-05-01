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
#define MONT_DEBUG 0

#define MON_MODULUS_LENGTH_IN_BITS 1248 // 1024 for testing with DDP vectors, 1248 for normal operation for PenO

#define MON_WORDSIZE 8 // Montgomery word size in bits



// The rest of the parameters are calculated from these first two.
#define SIZE MON_MODULUS_LENGTH_IN_BITS/MON_WORDSIZE // size of modulus in BYTES

// Some definitions for the MON_WORD_PRINTSTRING
#if MON_WORDSIZE == 8
#define MON_WORD_PRINTSTRING "%02X"
#elif MON_WORDSIZE == 16
#define MON_WORD_PRINTSTRING "%04X"
#else // for other cases, add here
#endif


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
	#error "MWORDSIZE (monmult.h) is not 8 or 16" // If you want add for example 32 bit, here is the spot.
#endif

#endif

