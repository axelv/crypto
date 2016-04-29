/***********************************************************************
* FILENAME :        signatures.h
*
* DESCRIPTION :
*       Header file for signatures.c
*
*		Exposes Signing and Verification methods.
*
* AUTHOR :    mraes
*
*
*****/
#ifndef __SIGNATURES_H__
#define __SIGNATURES_H__

#include <stdint.h>

// S is the signature of M (a message with length M_length), using privkey (length privkey_length) and modulus in the signing operation.
void RSASSA_PKCS1_V1_5_SIGN(uint8_t *S,uint8_t *privkey,unsigned int privkey_length, uint8_t *modulus,uint8_t *rmodn, uint8_t *r2modn, uint8_t *M, unsigned int M_length);

// This returns 1 if the signature S is a correct signature (verified using pubkey) for the message M.
uint8_t RSASSA_PKCS1V1_5_VERIFY(uint8_t *M, unsigned int M_length, uint8_t *S,uint8_t *pubkey, unsigned int pubkey_length, uint8_t *modulus,uint8_t *rmodn, uint8_t *r2modn);


#endif
