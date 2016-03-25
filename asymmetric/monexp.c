/***********************************************************************
* FILENAME :        monexp.c       VERSION: 1.0
*
* DESCRIPTION : Computes a**e mod n in an efficient way using Montgomery
*		exponentiation.
*
* PUBLIC FUNCTIONS: montgomery_exponentiation()
*
* NOTES : Based on the algorithm described in the
* 		  Handbook of Applied Cryptography.
* 
* AUTHOR :    mraes        START DATE :    25 Mar 16
*
*
*****/

#include <stdint.h>
#include <stdio.h>
#include "monmult.h"
#include "monexp.h"

void montgomery_exponentiation(uint8_t *res, uint8_t *base, uint8_t *exponent, uint8_t *n)
{
	// calc rmodn
	// calc r2modn
	uint8_t rmodn[SIZE] = {0x01,};
	uint8_t r2modn[SIZE] = {0x01,};
	uint8_t xtilde[SIZE] = {0x01,};
	uint8_t A[SIZE];
	uint8_t one[SIZE] = {0x01,0,};
	signed int i;
	signed int j;
	
	// 1
	montgomery_multiplication(xtilde, base, r2modn,n);
	for(i=0;i<SIZE;i++){ // Copy rmodn into A
		A[i] = rmodn[i];
	}
	
	//2
	printf("Entering step 2 of algo:\n");
	for(i=EXPONENT_LENGTH-1;i>=0; i--){
		for(j=7;j>=0;j--){
			montgomery_multiplication(A,A,A,n);
			if((exponent[i]>>j) & 0x01){
				printf("Bit %d of the exponent is: 1\n",(i*8)+j);
				montgomery_multiplication(A,A,xtilde,n);
			}else{printf("Bit %d of the exponent is: 0\n",(i*8)+j);}
			
		}
	}
	montgomery_multiplication(A,A,one,n);
	for(i=0;i<SIZE;i++){ // Copy A into res
		res[i] = A[i];
	}
}
