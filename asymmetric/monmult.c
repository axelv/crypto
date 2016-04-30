/***********************************************************************
* FILENAME :        monmult.c
*
* DESCRIPTION :
*       Performs Montgomery Multiplication of 2 multiple precision integers.
*
* PUBLIC FUNCTIONS :
*       int     monmult(  )
*
* NOTES : Using the Finely Integrated Product Scanning (FIPS) algorithm.
*
* AUTHOR :    mraes
*
*
*****/
#include <stdint.h>
#include <stdio.h>
#include "monmult.h"

MONWORD nprime_0 = 0; // To store the nprime_0 state across monmult calls. mod_inverse should only be called when a new modulus is defined.


// Computes x**-1 mod b for x odd and b being 2**(MON_WORDSIZE)
// x being ODD is REQUIRED and assumed in this function.
// For its normal use (inverting the least significant word of the modulus, this is normally fulfilled because the modulus has to be odd.
// Based on algorithm found in "A Cryptographic Library for the Motorola DSP56000"
MONWORD mod_inverse(MONWORD x)
{
	MONWORD y[MON_WORDSIZE+1] = {0,1,};
	uint32_t bitmask = 0;
	uint32_t i;
	for(i=2;i<MON_WORDSIZE+1;i++){
		bitmask = 0xFFFFFFFF;      //1111111111111111
		bitmask = bitmask << i;//1111111111111100
		bitmask = ~bitmask;    //0000000000000011 to select the last 2 bits, which is the same as doing modulo 2**2
		//fprintf(stdout,"Bitmask is 0x%02X\n", bitmask);
			if((x*y[i-1] & bitmask) < (0x00000001) << (i-1) ){
				y[i] = y[i-1];
			}else{
				y[i] = y[i-1] + ((0x01) << (i-1));
			}
		//fprintf(stdout,"\n[MODINV] y[%0d] is %0d\n", i, y[i]);
	}
	return y[MON_WORDSIZE];
}

void setup_monmult(MONWORD *n){
	MONWORD min_n0 = (-n[0]) & 0xFFFF;
	//fprintf(stdout,"\n[MODINV] min_n0 is 0x%02X, in decimal (unsigned) : %0d\n", min_n0,min_n0);
	nprime_0 = mod_inverse(min_n0); // nprime_0 is static/global!
	#if MONT_DEBUG
	fprintf(stdout,"\n[MODINV] nprime_0 is 0x%0X, in decimal (unsigned) : %0d\n", nprime_0,nprime_0);
	#endif
	}


void montgomery_multiplication(MONWORD *res, MONWORD *in1, MONWORD *in2, MONWORD *n)
{
		signed int it;
		#if MONT_DEBUG
			#if MON_WORDSIZE == 8
				printf("\n[MONMULT INPUTS]\n in1:\n");
				for(it=SIZE-1; it>=0; it--)
				{
					printf("%02X", in1[it]);
				}
				printf("\n in2:\n");
				for(it=SIZE-1; it>=0; it--)
				{
					printf("%02X", in2[it]);
				}
				printf("\n modulus:\n");
				for(it=SIZE-1; it>=0; it--)
				{
					printf("%02X", n[it]);
				}
			#elif MON_WORDSIZE == 16
				printf("\n[MONMULT INPUTS]\n in1:\n");
				for(it=SIZE-1; it>=0; it--)
				{
					printf("%04X", in1[it]);
				}
				printf("\n in2:\n");
				for(it=SIZE-1; it>=0; it--)
				{
					printf("%04X", in2[it]);
				}
				printf("\n modulus:\n");
				for(it=SIZE-1; it>=0; it--)
				{
					printf("%04X", n[it]);
				}
			#else
			#endif
		#endif
	// STEP 1: t = a.b & STEP 2 integrated

		// loop variables
		uint32_t k;
		uint32_t j;

		signed short r = 0;
		signed short c = 0;

		// NEED opmerking onderaan pagina 8 over dimensie van t!
		MONWORD t[3]={0x0,0x0,0x0};
		// variable for storing m as well as u
		MONWORD m[SIZE];
		// sum in short (C,S)
		uint64_t S_short = 0;
		// sum S
		MONWORD S = 0;
		// carry C
		MONWORD C = 0;

#pragma MUST_ITERATE(SIZE,SIZE,1)
		for(k=0; k<SIZE; k++)
		{
#pragma MUST_ITERATE(0,SIZE,1)
#pragma UNROLL(4)
			for(j=0; j<k; j++)
			{
				// (C,S) = t[0] + a[j]*b[i-j]
					// multiplication of two chars results in short
					S_short = t[0] + in1[j]*in2[k-j];
					// S_short can be split in first char=C, second char=S
					C = (MONWORD) (S_short >> (MON_WORDSIZE));
					S = (MONWORD) S_short;

				// add carry C to t[1] and propagate carry if needed
					// carry is second char of short
					t[2] += (t[1] + C) >> MON_WORDSIZE;
					// actual sum is first char of short
					t[1] = t[1] + C;

				// (C,S) = S + m[j]*n[i-j]
					// multiplication of two chars results in short
					S_short = S + m[j]*n[k-j];
					// S_short can be split in first char=C, second char=S
					C = (MONWORD) (S_short >> MON_WORDSIZE);
					S = (MONWORD) S_short;

				t[0] = S;

				// add carry C to t[1] and propagate carry if needed
					// carry is second char of short
					t[2] += (t[1] + C) >> MON_WORDSIZE;
					// actual sum is first char of short
					t[1] = t[1] + C;
			}

			// (C,S) = t[0] + a[i]*b[0]
					// multiplication of two chars results in short
					S_short = t[0] + in1[k]*in2[0];
					// S_short can be split in first char=C, second char=S
					C = (MONWORD) (S_short >> MON_WORDSIZE);
					S = (MONWORD) S_short; // TODO moet hier cast (char)?

			// add carry C to t[1] and propagate carry if needed
				// carry is second char of short
				t[2] += (t[1] + C) >> MON_WORDSIZE;
				// actual sum is first char of short
				t[1] = t[1] + C;

			// m[i] = S*nprime[0] mod W
			// W = 2**w(ordsize) =256 als w=8
			m[k] = (MONWORD) (S*nprime_0);

			// (C,S) = S + m[i]*n[0]
					// multiplication of two chars results in short
					S_short = S + m[k]*n[0];
					// S_short can be split in first char=C, second char=S
					C = (MONWORD) (S_short >> MON_WORDSIZE);
					S = (MONWORD) S_short; // TODO moet hier cast (char)?

			// add carry C to t[1] and propagate carry if needed
				// carry is second char of short
				t[2] += (t[1] + C) >> MON_WORDSIZE;
				// actual sum is first char of short
				t[1] = t[1] + C;

			// een soort t shift
			t[0] = t[1];
			t[1] = t[2];
			t[2] = 0;
		}

	// STEP 2 finishing touch
#pragma MUST_ITERATE(2*SIZE,2*SIZE,1)
		for(k=SIZE; k<2*SIZE; k++) // NEED warning: comparison is always true due to limited range of data type. warning is er niet meer omdat i nu short is ipv char
		{
#pragma MUST_ITERATE(0,SIZE,1)
#pragma UNROLL(4)
			for(j=k-SIZE+1; j<SIZE; j++)
			{

				// (C,S) = t[0] + a[j]*b[i-j]
					// multiplication of two chars results in short
					S_short = t[0] + in1[j]*in2[k-j];
					// S_short can be split in first char=C, second char=S
					C = (MONWORD) (S_short >> MON_WORDSIZE);
					S = (MONWORD) S_short; // TODO moet hier cast (char)?

				// add carry C to t[1] and propagate carry if needed
					// carry is second char of short
					t[2] += (t[1] + C) >> MON_WORDSIZE;
					// actual sum is first char of short
					t[1] = t[1] + C;

				// (C,S) = S + m[j]*n[i-j]
					// multiplication of two chars results in short
					S_short = S + m[j]*n[k-j];
					// S_short can be split in first char=C, second char=S
					C = (MONWORD) (S_short >> MON_WORDSIZE);
					S = (MONWORD) S_short; // TODO moet hier cast (char)?

				t[0] = S;

				// add carry C to t[1] and propagate carry if needed
					// carry is second char of short
					t[2] += (t[1] + C) >> MON_WORDSIZE;
					// actual sum is first char of short
					t[1] = t[1] + C;
			}

			m[k-SIZE] = t[0];

			// een soort t shift
			t[0] = t[1];
			t[1] = t[2];
			t[2] = 0;
		}
		// STEP 3: subtraction if needed
		// MSB of u is t[0], the rest of u is contained in m
		// if u>=n then return u-n else return u
		if(t[0]!=0){
			// u>n, subtraction needed
			#if MONT_DEBUG
			printf("\n[MONMULT STEP3] t[0] is 0x%0X : u>n, subtraction needed.",t[0]);
			#endif
			c=0;
			for(it=0;it<SIZE;it++){
				r = (m[it] - n[it] + c);
				if(r>=0){
					c = 0;
				}else{
					c = -1;
				}
				m[it] = r;

			}


		}else{
			for(it=SIZE-1; it>=0;it--){
				if(m[it]>n[it]){
					#if MONT_DEBUG
					printf("[MONMULT STEP3] subtraction needed.");
					#endif
					// u>n, subtraction needed
					c=0;
					for(it=0;it<SIZE;it++){
						r = (m[it] - n[it] + c);
						if(r>=0){
							c = 0;
						}else{
							c = -1;
						}
						m[it] = r;

					}
					break;
				}else if(m[it]<n[it]){
					// // u<n, no subtraction needed, m contains the result
					break;
				}else {
					// the two compared bytes are equal, go to the next byte to keep comparing
				}
			}

		}
		// if subtraction was needed, it is already done here. Start copying answer to res
	for(k=0; k<SIZE; k++)
	{
		res[k]=m[k];	
	}
	#if MONT_DEBUG
		#ifdef MON_WORDSIZE
			#if MON_WORDSIZE == 8
				printf("\n[MONMULT RESULT (Big Endian)]:\n");
				for(it=SIZE-1; it>=0; it--)
				{
					printf("%02X", res[it]);
				}
			#elif MON_WORDSIZE == 16
				printf("\n[MONMULT RESULT (Big Endian)]:\n");
				for(it=SIZE-1; it>=0; it--)
				{
					printf("%02X", res[it]);
				}
			#else 
			#endif
		#endif
	#endif
}

