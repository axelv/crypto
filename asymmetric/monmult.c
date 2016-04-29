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

uint8_t nprime_0 = 0; // To store the nprime_0 state across monmult calls. mod_inverse should only be called when a new modulus is defined.


// Computes x**-1 mod b for x odd and b being 2**(8)
// x begin ODD is REQUIRED and assumed in this function.
// For its normal use (inverting the least significant word of the modulus, this is normally fulfilled because the modulus has to be odd.
// Based on algorithm found in "A Cryptographic Library for the Motorola DSP56000"
uint8_t mod_inverse(uint8_t x)
{
	uint8_t y[MWORDSIZE+1] = {0,1,};
	uint16_t bitmask = 0;
	unsigned char i;
	for(i=2;i<MWORDSIZE+1;i++){
		bitmask = 0xFFFF;      //1111111111111111
		bitmask = bitmask << i;//1111111111111100
		bitmask = ~bitmask;    //0000000000000011 to select the last 2 bits, which is the same as doing modulo 2**2
		//fprintf(stdout,"Bitmask is 0x%02X\n", bitmask);
			if((x*y[i-1] & bitmask) < (0x0001) << (i-1) ){ 
				
				y[i] = y[i-1];
			}else{
				y[i] = y[i-1] + ((0x01) << (i-1));
			}
		//fprintf(stdout,"\n[MODINV]y[%0d] is %0d\n", i, y[i]);
	}
	
	return y[MWORDSIZE];
}

void setup_monmult(uint8_t *n){
	uint8_t min_n0 = (-n[0]) & 0xFF;
	//fprintf(stdout,"\n[MODINV] min_n0 is 0x%02X, in decimal (unsigned) : %0d\n", min_n0,min_n0);
	nprime_0 = mod_inverse(min_n0);
}

void montgomery_multiplication(uint8_t *res, uint8_t *in1, uint8_t *in2, uint8_t *n)
{
		signed int it;
		#if MONT_DEBUG
		
		printf("\n[MULT]\n in1:\n");
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
		#endif
	// STEP 1: t = a.b & STEP 2 integrated
	
		// loop variables
		unsigned int k;
		unsigned int j;

		unsigned char i;
		// temporary sum used in former ADD function
		unsigned short temp_sum = 0;

		// variables used in former mp_substraction function
		unsigned short s1;
		unsigned short s2;

		signed short r = 0;
		signed short c = 0;

		// resetting t to zeros array TODO needed? 
		// NEED opmerking onderaan pagina 8 over dimensie van t!
		uint8_t t[3]={0x0,0x0,0x0};
		// variable for storing m as well as u
		uint8_t m[SIZE];
		// sum in short (C,S)
		unsigned int S_short = 0;
		// sum S
		uint8_t S = 0;
		// carry C
		uint8_t C = 0;

		//uint8_t min_n0 = (-n[0]) & 0xFF;
		//fprintf(stdout,"\n[MODINV] min_n0 is 0x%02X, in decimal (unsigned) : %0d\n", min_n0,min_n0);
		//uint8_t nprime_0 = mod_inverse(min_n0);
		#if MONT_DEBUG
		fprintf(stdout,"\n[MODINV] nprime_0 is 0x%02X, in decimal (unsigned) : %0d\n", nprime_0 ,nprime_0);
		#endif
#pragma MUST_ITERATE(SIZE,SIZE,1)
		for(k=0; k<SIZE; k++)
		{
#pragma MUST_ITERATE(0,SIZE,1)
#pragma UNROLL(4)
			for(j=0; j<k; j++) //deze loop wordt bij de eerste iteratie van i niet uitgevoerd
			{
				// (C,S) = t[0] + a[j]*b[i-j]
					// multiplication of two chars results in short
					S_short = t[0] + in1[j]*in2[k-j];
					// S_short can be split in first char=C, second char=S
					C = (S_short >> (8)) & 0xff;
					S = S_short & 0xff; // TODO moet hier cast (char)?

				// add carry C to t[1] and propagate carry if needed
					// carry is second char of short
					t[2] += (t[1] + C) >> 8;
					// actual sum is first char of short
					t[1] = t[1] + C;

				// (C,S) = S + m[j]*n[i-j]
					// multiplication of two chars results in short
					S_short = S + m[j]*n[k-j];
					// S_short can be split in first char=C, second char=S
					C = (S_short >> (8)) & 0xff;
					S = S_short & 0xff; // TODO moet hier cast (char)?

				t[0] = S;

				// add carry C to t[1] and propagate carry if needed
					// carry is second char of short
					t[2] += (t[1] + C) >> 8;
					// actual sum is first char of short
					t[1] = t[1] + C;
			}

			// (C,S) = t[0] + a[i]*b[0]
					// multiplication of two chars results in short
					S_short = t[0] + in1[k]*in2[0];
					// S_short can be split in first char=C, second char=S
					C = (S_short >> 8) & 0xff;
					S = S_short & 0xff; // TODO moet hier cast (char)?

			// add carry C to t[1] and propagate carry if needed
				// carry is second char of short
				t[2] += (t[1] + C) >> 8;
				// actual sum is first char of short
				t[1] = t[1] + C;

			// m[i] = S*nprime[0] mod W
			// W = 2**w(ordsize) =256 als w=8
			// W is te interpreteren als de nieuwe radix van deze mp bewerking
			// w = 8 bit 
			// module 2**8 is het zelfde als shiften met 8 als de input een 16 bit getal is (short=char*char)
			//  & 0xff is niet nodig, maar weglaten speed code niet up
			// initiele code m[i] = ((S*nprime[0]) >> 8) & 0xff;
			// ik denk dat shiften niet nodig is want je wil de LSB houden
			m[k] = (S*nprime_0) & 0xff;

			// (C,S) = S + m[i]*n[0]
					// multiplication of two chars results in short
					S_short = S + m[k]*n[0];
					// S_short can be split in first char=C, second char=S
					C = (S_short >> 8) & 0xff;
					S = S_short & 0xff; // TODO moet hier cast (char)?

			// add carry C to t[1] and propagate carry if needed
				// carry is second char of short
				t[2] += (t[1] + C) >> 8;
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
					C = (S_short >> 8) & 0xff;
					S = S_short & 0xff; // TODO moet hier cast (char)?

				// add carry C to t[1] and propagate carry if needed
					// carry is second char of short
					t[2] += (t[1] + C) >> 8;
					// actual sum is first char of short
					t[1] = t[1] + C;

				// (C,S) = S + m[j]*n[i-j]
					// multiplication of two chars results in short
					S_short = S + m[j]*n[k-j];
					// S_short can be split in first char=C, second char=S
					C = (S_short >> 8) & 0xff;
					S = S_short & 0xff; // TODO moet hier cast (char)?

				t[0] = S;
				
				// add carry C to t[1] and propagate carry if needed
					// carry is second char of short
					t[2] += (t[1] + C) >> 8;
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
			printf("\n t[0] is %02X : u>n, subtraction needed.",t[0]);
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
					printf("subtraction needed.");
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
	printf("\n res:\n");
	for(it=SIZE-1; it>=0; it--)
	{
		printf("%02X", res[it]);
	}
	/*printf("\n res (LITTLE ENDIAN):\n");
	for(it=0; it<SIZE; it++)
	{
		printf("0x%02X,", res[it]);
	}*/
	#endif
}

