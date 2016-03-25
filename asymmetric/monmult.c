/***********************************************************************
* FILENAME :        monmult.c          VERSION: 1.0
*
* DESCRIPTION :
*       Performs Montgomery Multiplication of 2 multiple precision integers.
*
* PUBLIC FUNCTIONS :
*       int     monmult(  )
*
* NOTES : Using the Finely Integrated Product Scanning (FIPS) algorithm.
* 
* AUTHOR :    mraes        START DATE :    10 Mar 16
*
*
*****/
#include <stdint.h>
#include <stdio.h>
#include "monmult.h"
/*
// NEED TO CHANGE SHIFTING AND MASKING (from >>8 to >>16 for example and from && xff to &&xffff)!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
uint16_t a16[SIZE] = {0x71A3,0x179,0xF2FC,0xC793,0x691C,0x672D,0x2B38,0x1A7C,0x8840,0x9BBA,0x5310,0xED65,0x39FE,0x77B4,0xC83B,0xB29B,0xB1E4,0xC9A3,0xC8CD,0x69F2,0x9101,0x6787,0x6EAC,0xF156,0xBF60,0xF1E,0xA853,0x40AF,0x76DB,0xEAB2,0xDD90,0x40FF,0xD20C,0xDC4D,0xB35E,0xFAE7,0x53B4,0x755E,0x910B,0x1492,0xD87E,0xA7B8,0x9505,0x55D0,0x3097,0x4178,0xEF81,0x64C6,0x4A9F,0x4A9C,0xB3A,0xD8CA,0x1A9C,0xA764,0x6EA9,0xAC8,0xCEE5,0x3BE2,0x8A73,0xF0EF,0xA6E4,0x2640,0xD9F3,0x1F50};

uint16_t b16[SIZE] = {0xAABD,0x4C18,0x5CCD,0x3595,0xA25B,0xC09D,0xE2EB,0xC68A,0xDFBF,0x88A1,0x8C1B,0xE358,0xD8B0,0xC6F8,0x1B6,0xE296,0x52E0,0xB01,0xFF3A,0x4865,0x67E0,0x9ECD,0x6A1E,0xFCA4,0xC504,0x1C7B,0x2BF0,0x4ED1,0xFB62,0x6836,0x70B9,0x98C8,0x8A0C,0x3C65,0x15D1,0x132B,0x5ACA,0xBAB7,0x15FC,0x1137,0xF36C,0x8593,0x1E9B,0x28E8,0x4B44,0x6AFB,0xECFD,0xCBC9,0x7C9,0x2B28,0xE096,0xA5F9,0x358C,0xCC62,0xB9AB,0xA2F3,0x4DA9,0x724,0x8716,0x31F4,0xBCBB,0xA6D,0xE588,0x7D47};

uint16_t c16[SIZE] = {0xEC0C,0xE699,0x5E55,0x1218,0xA17C,0x34AF,0x53A7,0x951A,0x9CED,0xF870,0x709F,0xE0B7,0xF279,0x8BAE,0xD288,0xB8C4,0x6FB7,0xCC0C,0xC92E,0xB85C,0xE82,0x177B,0xC3E1,0x320A,0xDB35,0xBE5C,0x35AC,0xBD3,0xE5EE,0x778A,0xC089,0x4DBC,0x3ED3,0x8E5B,0xD5A8,0xE06,0x7A17,0x1D5E,0x1CD2,0xE4C5,0x5519,0x1A1D,0x574A,0xAFAC,0xF00F,0xA2DD,0xF3FE,0x8B7E,0x15AA,0xA3B5,0x4A2A,0xCB70,0x960,0xBAD3,0x4722,0xBC68,0x1796,0xB8BD,0xCA39,0x57A5,0x1216,0x8995,0x809F,0x8C22};

uint16_t n16[SIZE] = {0xD50D,0x260F,0x958F,0x59AD,0xA024,0x8FDA,0x27AE,0xD49F,0xE77A,0xBF7E,0x2DDF,0xDA12,0x1443,0x200F,0x90FA,0xD546,0xF139,0x90AA,0xE402,0xA3DC,0xA49C,0xFFF2,0xC130,0x2214,0x117E,0x77E3,0x1A00,0x50AD,0x7BE1,0x4A91,0x892,0x2065,0x6875,0xB374,0xB065,0xC60C,0xC98B,0xEF10,0x4EC4,0xAD45,0xD2FA,0x64F,0x29EC,0xF35C,0x97AA,0x7082,0x44B8,0x8606,0x67E0,0x70FC,0xA479,0xB3F0,0x480B,0xF0CD,0x6F6C,0x8B0B,0x6725,0x8F46,0xA4EC,0xE3C1,0x476A,0xEB0D,0x81A3,0xBAB5};

uint16_t nprime16[SIZE] = {0xFE3B,0x4913,0x1C55,0x1095,0x6074,0xE6FA,0x4B25,0x8A97,0x9234,0x7237,0x1BAE,0xCE7D,0x5F9D,0x6CEC,0xE13,0xC7FE,0xD4B3,0x3B4,0x8B4C,0xDAE,0xDC86,0x665,0xBBC4,0x155D,0x76BE,0x826,0x6057,0x83D8,0x3876,0xDE1C,0x163B,0x2D94,0xA5CC,0x458B,0x322D,0x1287,0x9918,0x4CDE,0x162B,0xFA24,0x692A,0x9C04,0xF3E8,0x82B,0xA879,0xEDC6,0xD8B8,0x451A,0xC57,0xD740,0xA637,0xCD19,0x565A,0xF6DE,0xDB29,0x5A15,0x480B,0x3643,0xFDED,0x90BD,0x8135,0xA33B,0x5418,0xD7B1};
*/

// Global internal RAM (chosen by the compiler by default)
unsigned char i;

// Computes x**-1 mod b for x odd and b being 2**(8)
// x begin ODD is REQUIRED and assumed in this function.
// For its normal use (inverting the least significant word of the modulus, this is normally fulfilled because the modulus has to be odd.
// Based on algorithm found in "A Cryptographic Library for the Motorola DSP56000"
uint8_t mod_inverse(uint8_t x)
{
	uint8_t y[MWORDSIZE+1] = {0,1,};
	uint16_t bitmask = 0;
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
		//fprintf(stdout,"y[%0d] is %0d\n", i, y[i]);
	}
	
	return y[MWORDSIZE];
}
void montgomery_multiplication(uint8_t *res, uint8_t *in1, uint8_t *in2, uint8_t *n)
{

	// STEP 1: t = a.b & STEP 2 integrated
	
		// loop variables
		unsigned short k;
		unsigned char j;

		// temporary sum used in former ADD function
		unsigned short temp_sum = 0;

		// variables used in former mp_substraction function
		unsigned short s1;
		unsigned short s2;
		signed short r = 0;
		unsigned short c = 0;

		// resetting t to zeros array TODO needed? 
		// NEED opmerking onderaan pagina 8 over dimensie van t!
		uint8_t t[3]={0x0,0x0,0x0};
		// variable for storing m as well as u
		uint8_t m[SIZE];
		// sum in short (C,S)
		unsigned short S_short = 0;
		// sum S
		uint8_t S = 0;
		// carry C
		uint8_t C = 0;

		uint8_t min_n0 = (-n[0]) & 0xFF;
		fprintf(stdout,"min_n0 is 0x%02X, in decimal (unsigned) : %0d\n", min_n0,min_n0);
		uint8_t nprime_0 = mod_inverse(min_n0);
		fprintf(stdout,"nprime_0 is 0x%02X, in decimal (unsigned) : %0d\n", nprime_0 ,nprime_0);

		for(k=0; k<SIZE; k++)
		{
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

		for(k=SIZE; k<2*SIZE; k++) // NEED warning: comparison is always true due to limited range of data type. warning is er niet meer omdat i nu short is ipv char
		{
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

	// STEP 3: substraction if needed
	// if u>=n then return u-n else return u
	// m = u!

		// kaascode
		// GEEN NIEUWE VARS DECLAREREN MIDDEN IN FUNCTIE! hier geen unsigned char k=0; meer declareren!
		k=0;
		// TODO for met break ipv while?
		while(m[SIZE-1-k] == n[SIZE-k-i]) //Search for the first two words that are different, starting from MSB
		{ 
			k++;
		}
		
		if(m[SIZE-1-k] >= n[SIZE-1-k])
		{
			// mp_substraction(m,m,n)
			for(j=0; j<SIZE; j++)
			{
				s1 = (short) m[j];
				s2 = (short) n[j];
				r = s1 - s2 - c;
				if(r < 0)
				{ 
					c = 1;
					r = r + (SIZE + SIZE); // r mod(2*SIZE) (always: |r|<2*SIZE)
				}
				else
				{
					c = 0;		
				}
				m[j] = (unsigned char) r&0xff;
			}
			m[j] = (unsigned char) (SIZE-c); // modulo NEED kaas wat is dit? en klopt het zoals ik het nu doe? 
		}

	// assign output
	// memcpy is niet sneller, maar werkt wel!
	// copy zelfs helemaal niet nodig als m op de plaats van res in het geheugen wordt genomen
	// als je dit echter verwijderd, verslechtert de tijd! (compiler doet rare dingen, houdt m lokaal of zo?)
	for(k=0; k<SIZE; k++)
	{
		res[k]=m[k];	
	}

}

