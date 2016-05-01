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

	// these are the rmodn's for 128 byte modulus (Dit is voor met de vectoren ooit gegeven in DDP (MAGMA))
	//uint8_t rmodn[SIZE] =  {0xCF, 0x90, 0x85, 0x19, 0x1A, 0x43, 0x88, 0x59, 0x07, 0x99, 0x61, 0xC4, 0xFD, 0x0B, 0x20, 0xCD, 0x37, 0x58, 0x46, 0xD5, 0xE1, 0x43, 0xF7, 0x41, 0x79, 0xEE, 0x21, 0xD5, 0x30, 0x17, 0x77, 0x71, 0xC4, 0xA3, 0x95, 0x8E, 0xB8, 0xC3, 0xBA, 0xFA, 0xB9, 0x1B, 0x52, 0x6C, 0xBD, 0xE0, 0x88, 0x23, 0xDA, 0x68, 0x8F, 0x1E, 0x4A, 0x14, 0x76, 0x80, 0x56, 0xE1, 0xAF, 0xE2, 0xCC, 0x66, 0xC3, 0xF6, 0x97, 0x1B, 0xE5, 0x30, 0x4F, 0xE1, 0x45, 0xA9, 0xDB, 0x83, 0x71, 0xCA, 0x7B, 0x1F, 0x55, 0xD2, 0x13, 0xA7, 0x5F, 0xB2, 0x76, 0x75, 0xFD, 0xD8, 0xC7, 0xAB, 0xD1, 0xB0, 0x4D, 0x69, 0x54, 0xD6, 0xFE, 0xDE, 0xF8, 0x06, 0x7A, 0xFE, 0x66, 0x18, 0x01, 0x07, 0x65, 0xE6, 0x92, 0x6D, 0x78, 0x97, 0xF1, 0xBA, 0x4F, 0x30, 0x26, 0xC2, 0x5A, 0xC9, 0xD3, 0x67, 0x2C, 0xF1, 0x29, 0x4B, 0x2C, 0x2E};
	//uint8_t r2modn[SIZE] = {0xC8, 0xF6, 0x32, 0x9D, 0xE1, 0x61, 0xBD, 0x2F, 0xDA, 0xA1, 0x1F, 0x69, 0xC7, 0x0B, 0x56, 0x7E, 0x26, 0x8B, 0x2D, 0xB8, 0xF3, 0x71, 0xBE, 0xDE, 0x22, 0x34, 0x38, 0x6B, 0x8B, 0xB5, 0x71, 0x69, 0x39, 0x08, 0x03, 0x05, 0x94, 0xB2, 0x91, 0x3F, 0x96, 0xC7, 0xB3, 0xE4, 0x48, 0x25, 0xF5, 0xF7, 0xB5, 0xD5, 0xD1, 0xF5, 0x1A, 0x06, 0x13, 0xE5, 0x9A, 0x81, 0xD6, 0xAD, 0x4C, 0xD4, 0x6D, 0x2C, 0x71, 0x1B, 0x4E, 0x5D, 0x4F, 0xE0, 0x7D, 0xEA, 0xE9, 0x63, 0x06, 0x34, 0x4C, 0xA6, 0xA9, 0xA9, 0x2A, 0x3F, 0x66, 0x1B, 0x51, 0x3B, 0x73, 0x44, 0x18, 0xFF, 0xFE, 0x30, 0x8B, 0xEF, 0x94, 0x4A, 0x7D, 0x3F, 0x92, 0x8F, 0x23, 0x3D, 0x22, 0xCF, 0x07, 0x1A, 0x85, 0xB7, 0x76, 0x5F, 0x03, 0xBA, 0x4B, 0x56, 0xA1, 0x42, 0x29, 0x21, 0x50, 0x98, 0xC9, 0xED, 0xD9, 0xEA, 0x25, 0x35, 0xAF, 0x24};

	// These are the rmodn's for the 156 byte modulus for the MASTER RSA Signatures
	//uint8_t rmodn[SIZE] = {0x7D,0x20,0xAA,0xFA,0x79,0x0E,0xB3,0xA0,0x54,0xB1,0x58,0xBE,0x9A,0xB9,0xA2,0xAA,0xF6,0x70,0xA4,0x57,0x6E,0xAF,0xE3,0x2B,0xB4,0x0F,0xAE,0xDD,0x4F,0x4F,0x3E,0x4B,0x9C,0xEA,0x70,0x0A,0xC4,0x41,0x1C,0x78,0x0D,0xDF,0xAE,0xB8,0x36,0x55,0xD0,0x32,0x25,0xC3,0x5A,0x49,0x4D,0xB5,0x25,0x6E,0x88,0x8C,0x4B,0x09,0x52,0xEE,0x4A,0x66,0xFD,0xC1,0x55,0x3E,0x26,0x95,0xEA,0x99,0x6E,0xDB,0x49,0xFF,0x73,0xFB,0xA2,0x2D,0x1E,0x88,0xA3,0x55,0x7C,0xA7,0xFF,0xF3,0xC2,0xA7,0x60,0xE7,0x78,0x63,0x4E,0x06,0xBB,0x3E,0x8B,0x79,0xC7,0x61,0x9E,0x43,0xD9,0xC3,0x1A,0x59,0xCE,0xD2,0xAC,0x11,0xB1,0xAC,0xD4,0xFE,0x73,0x03,0xAD,0x25,0x58,0xE6,0x50,0x10,0x34,0xCE,0x61,0xF6,0xDE,0xE4,0xEA,0xA1,0x3D,0xF8,0xEC,0xBD,0xC3,0x16,0x50,0xFC,0xFC,0xDA,0x4A,0xD3,0x7F,0x9A,0x5C,0xA6,0x66,0xD0,0x16,0xFC,0x6E,0x6E,0xDF,0x4D};
	//uint8_t r2modn[SIZE]= {0x63,0x93,0xBE,0x6C,0x04,0xE5,0xD6,0xE4,0x27,0xB7,0x53,0x91,0xFA,0x16,0xAA,0x75,0x8A,0x3C,0x2E,0xA9,0xEE,0x91,0x70,0x90,0xBD,0xF8,0xA3,0x2B,0x30,0x83,0xED,0xAE,0xA8,0x6C,0x99,0x16,0xC8,0x9B,0x13,0x13,0xD1,0x57,0xB3,0x6A,0x8F,0x0D,0x7E,0x3C,0xC9,0xE4,0x7F,0x3D,0x46,0x51,0x76,0x09,0x82,0x3B,0xE7,0x08,0x1F,0x54,0x9E,0xA0,0xE5,0xBA,0x8B,0x1E,0x64,0x0A,0x60,0x30,0xBB,0xE7,0xB8,0x91,0xCB,0xF4,0x53,0xA1,0x8D,0x48,0xCC,0x0B,0xC3,0x47,0xC4,0x0A,0x5E,0x7A,0xD3,0xC4,0xE8,0xFA,0x36,0x62,0x33,0x0D,0x86,0x24,0x61,0xE7,0xF3,0xB4,0xFD,0xDC,0x87,0x3E,0x9C,0xDF,0xCC,0x55,0x77,0x08,0xC0,0xBF,0xF4,0x5D,0x9C,0xA1,0xFB,0x1B,0xF0,0x66,0xF6,0xEF,0x4C,0x3D,0x07,0x36,0x50,0xCC,0xB6,0xB7,0x57,0x76,0x22,0x71,0x55,0x54,0xBA,0xA4,0x1D,0x0C,0xFA,0x00,0x00,0x82,0x95,0x35,0xAB,0x4D,0x06,0x0C,0xB0,0x9E};

	// These are the rmodn's for the 156 byte modulus for the SLAVE RSA Signatures
	//uint8_t rmodn[SIZE] = {0x09,0x79,0x5B,0xF8,0xAC,0xD4,0x14,0x98,0xB0,0xA0,0xF9,0xF3,0xC4,0x07,0x95,0x24,0x8D,0x15,0x3A,0xF6,0x38,0x2B,0xF5,0xBD,0xAE,0x2D,0x1B,0x56,0xC3,0xBF,0x48,0x9C,0xBB,0x00,0xFF,0xE2,0x69,0xEE,0xE6,0x18,0x30,0x13,0xF5,0x6B,0xA8,0xE6,0xF6,0x09,0x6B,0xEC,0x03,0x70,0xA5,0x91,0xCB,0xBB,0xDB,0x71,0x00,0x3F,0x85,0x00,0xDF,0x16,0x7E,0x8E,0xC3,0x3C,0xE5,0x36,0x1D,0x5C,0xC5,0x0E,0xF7,0xA7,0xFB,0x9A,0xBD,0x32,0x3F,0xB9,0xED,0x9F,0x70,0x1B,0xB1,0x49,0x1A,0x89,0xB5,0x72,0x93,0x0B,0x3C,0x50,0x79,0x52,0x00,0xEA,0xCF,0x59,0x8C,0xE9,0x91,0x46,0x36,0xCA,0x1A,0x50,0x22,0x92,0x90,0xAE,0xF6,0xC9,0xCA,0xCF,0x18,0x5F,0xF0,0x3C,0xCE,0xCC,0xAB,0xE0,0x73,0x85,0x95,0x9C,0x45,0x4D,0xBE,0x90,0xFA,0xFE,0x72,0x45,0x55,0xD3,0x87,0xB7,0xA9,0x90,0xCE,0x99,0xF6,0xEC,0x69,0x62,0xF7,0x1B,0x70,0x0B,0x86,0x5D};
	//uint8_t r2modn[SIZE]= {0x0A,0xD4,0x11,0xA8,0xA6,0x4B,0x97,0xA8,0x54,0x3D,0xB8,0xDC,0x54,0x23,0x5B,0xAB,0x84,0x05,0x54,0x63,0x9D,0x85,0xD9,0x52,0x4C,0x7B,0xA4,0xBA,0x24,0xDF,0x05,0xF0,0xC3,0x04,0x50,0xC2,0x83,0x03,0x55,0x43,0xBF,0x92,0x09,0x14,0x3E,0x7C,0xE8,0x99,0xCE,0x3A,0x83,0x5C,0x6B,0x3C,0x8C,0xA8,0x21,0xA1,0xC9,0xB2,0x0A,0x78,0x21,0x00,0xFE,0x4A,0x84,0xFE,0x8A,0x18,0xE7,0x72,0xCE,0x86,0x85,0x90,0x4E,0x6D,0xB0,0xF4,0xDB,0xE4,0x31,0xD0,0xD4,0xB8,0x71,0xA1,0x64,0xDE,0x81,0xDC,0x52,0x52,0xFE,0x4D,0x15,0xCB,0x91,0x30,0x1B,0x42,0x6E,0x7F,0xC7,0x44,0xAE,0xA4,0x67,0x3E,0xD1,0x18,0x65,0x55,0x93,0xA7,0x0C,0xE8,0x4F,0x12,0xEA,0xAA,0x1E,0xF4,0x96,0xA3,0x3F,0x48,0x7C,0x14,0xF6,0xDD,0x10,0xB5,0xA6,0x5B,0x4B,0x18,0x28,0x0B,0xBE,0x48,0x2C,0x15,0x39,0xAA,0xB8,0xB4,0x26,0x55,0xB3,0x37,0x10,0x18,0x8F,0x3B};


	// these are the rmodn's for the 156 byte modulus (Voor de Diffie Hellman modulus gegeven op toledo)
	//uint8_t rmodn[SIZE] = {0xBD,0x53,0x67,0x38,0xFC,0x94,0xC0,0x63,0x1C,0x54,0x51,0x76,0x15,0x2D,0x03,0x56,0x94,0xE3,0x24,0x5A,0xD5,0xAE,0xE7,0x34,0x54,0xF1,0x56,0x86,0xB7,0x2F,0x33,0x6F,0xFA,0x3E,0x17,0x43,0xFC,0xC1,0x7F,0x89,0x49,0xB8,0xB9,0x55,0x50,0x16,0x80,0x1A,0x53,0xBD,0x1F,0xA3,0x55,0x7D,0xAE,0x3A,0x60,0x3E,0xE9,0x55,0xE8,0x82,0xFF,0xDB,0x6F,0xF8,0xBF,0x16,0xDF,0x0C,0x21,0x1E,0x79,0xA0,0xD8,0xBB,0xD7,0x19,0x03,0xB1,0x60,0x48,0x6E,0xAA,0x30,0x8B,0x87,0x6A,0x33,0xC0,0x5E,0x6A,0x11,0xE0,0x12,0x08,0x39,0xED,0x6F,0xED,0xB2,0x04,0xD4,0x1B,0x15,0xB7,0x8E,0x33,0x05,0x73,0x34,0xDF,0xA9,0xAD,0xAB,0x08,0x34,0xDE,0x52,0x24,0xC4,0x10,0xAE,0xBC,0x39,0xB9,0xCD,0x5C,0xED,0x08,0xCA,0x96,0xB2,0x86,0xF6,0x75,0x64,0x91,0xBE,0x20,0x47,0x99,0x33,0xDB,0x6D,0xCB,0x06,0xEF,0xC2,0xA0,0x5C,0xFB,0x7F,0x8C,0x57,0x2F};
	//uint8_t r2modn[SIZE] ={0x1A,0x1C,0x3E,0xFD,0x47,0xBE,0x7F,0x6E,0x0C,0xC4,0x25,0xCE,0xEA,0x24,0x25,0x72,0xC5,0x68,0x14,0x23,0x08,0x93,0x2C,0x0C,0x9A,0x84,0x61,0xC0,0x18,0x16,0x27,0x64,0xF0,0x66,0xF7,0x1E,0x23,0xC2,0xEF,0xD3,0x87,0x66,0xB1,0x98,0x82,0x58,0x0C,0x46,0xCB,0x87,0xA7,0x40,0x92,0x94,0x0F,0x76,0x0F,0xA9,0xC6,0x70,0x3B,0x48,0x2B,0x27,0xE6,0xBB,0xA7,0x8E,0xD8,0x5E,0xC2,0x36,0x19,0x3E,0xDF,0x42,0x50,0x78,0x2C,0x2A,0x20,0x27,0x4E,0x60,0x6E,0x9C,0xF9,0x11,0x5E,0xD9,0x1B,0x83,0x56,0x46,0xE4,0x1D,0x93,0xE6,0x5B,0x0C,0xCE,0x4A,0x2B,0xC5,0xAA,0x0F,0xC0,0xED,0x30,0x58,0x77,0xD3,0x45,0xEC,0x71,0xC1,0x32,0xF4,0xEF,0x13,0xD3,0xB6,0x00,0x26,0xE5,0x71,0x4C,0x7D,0x76,0xBB,0x1F,0x0D,0xC6,0x45,0x2F,0x7E,0x07,0xA8,0x49,0xC2,0x20,0x35,0x3B,0x52,0xE7,0xE8,0x51,0x47,0x7A,0xDA,0x1B,0xFB,0xA8,0xE8,0x7C,0x14};


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
