/***********************************************************************
* FILENAME :        monexp.c
*
* DESCRIPTION : Computes a**e mod n in an efficient way using Montgomery
*		exponentiation.
*
* PUBLIC FUNCTIONS: montgomery_exponentiation()
*
* NOTES : Based on the algorithm described in the
* 		  Handbook of Applied Cryptography.
*
* AUTHOR :    mraes
*
*
*****/

#include <stdint.h>
#include <stdio.h>
#include "monmult.h"
#include "monexp.h"
#include "../tools/tools.h" // for the TEMPORARY 8 to 16 bit conversion


void montgomery_exponentiation(uint8_t *res, uint8_t *base, uint8_t *exponent,uint8_t exponent_length, uint8_t *n, uint8_t *rmodn, uint8_t *r2modn)
{
	signed int i;
	signed int j;

	#if MON_WORDSIZE == 16 // for internal conversion to 16 bit numbers. This is a temporary measure until higher layer code (eg. the key storage format) is adapted as well.
	uint16_t res16[SIZE];
	uint16_t base16[SIZE];
	uint16_t n16[SIZE];
	uint16_t rmodn16[SIZE];
	uint16_t r2modn16[SIZE];
	convert_8_to_16(base16,base);
	convert_8_to_16(n16,n);
	convert_8_to_16(rmodn16,rmodn);
	convert_8_to_16(r2modn16,r2modn);
	#endif


	MONWORD xtilde[SIZE];
	MONWORD A[SIZE];
	MONWORD one[SIZE] = {0x01,0,};
	uint8_t bln_exp_start = 0; // For lack of a boolean type, this meant to be a boolean.

	#if MON_WORDSIZE == 16
		setup_monmult(n16); // Setting up the montgomery multiplication
		// 1
		montgomery_multiplication(xtilde, base16, r2modn16,n16);
		for(i=0;i<SIZE;i++){ // Copy rmodn into A
			A[i] = rmodn16[i];
		}
	#else
		setup_monmult(n); // Setting up the montgomery multiplication
		// 1
		montgomery_multiplication(xtilde, base, r2modn,n);
		for(i=0;i<SIZE;i++){ // Copy rmodn into A
			A[i] = rmodn[i];
		}
	#endif


	//2
	#if MONT_DEBUG
	printf("\n[EXP] Step 2\n");
	#endif
	for(i=exponent_length-1;i>=0; i--){
		for(j=7;j>=0;j--){
			if(bln_exp_start){

				#if MON_WORDSIZE == 16
					montgomery_multiplication(A,A,A,n16);
				#else
					montgomery_multiplication(A,A,A,n);
				#endif

				if((exponent[i]>>j) & 0x01){
					#if MONT_DEBUG
					printf("\n[EXP] Bit %d of the exponent is: 1\n",(i*8)+j);
					#endif

					#if MON_WORDSIZE == 16
						montgomery_multiplication(A,A,xtilde,n16);
					#else
						montgomery_multiplication(A,A,xtilde,n);
					#endif

				}else{
					#if MONT_DEBUG
					printf("\n[EXP] Bit %d of the exponent is: 0\n",(i*8)+j);
					#endif
					}
			}else{
				if((exponent[i]>>j) & 0x01){
					#if MONT_DEBUG
					printf("\n[EXP] Bit %d of the exponent is the first nonzero bit\n",(i*8)+j);
					#endif
					bln_exp_start=1;

					#if MON_WORDSIZE == 16
					montgomery_multiplication(A,A,A,n16);
					montgomery_multiplication(A,A,xtilde,n16);
					#else
					montgomery_multiplication(A,A,A,n);
					montgomery_multiplication(A,A,xtilde,n);
					#endif

					}
			}
		}
	}
	#if MONT_DEBUG
	printf("\n[EXP] Step 3:\n");
	#endif

	#if MON_WORDSIZE == 16
		montgomery_multiplication(A,A,one,n16);
	#else
		montgomery_multiplication(A,A,one,n);
	#endif

	#if MONT_DEBUG
	printf("\n[EXP] Step 4:\n");
	#endif
	for(i=0;i<SIZE;i++){ // Copy A into res

		#if MON_WORDSIZE == 16
			res16[i] = A[i];
		#else
			res[i] = A[i];
		#endif

	}

#if MON_WORDSIZE == 16
	convert_16_to_8(res,res16);
#else
#endif

}
