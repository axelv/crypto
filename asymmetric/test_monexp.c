/***********************************************************************
* FILENAME :        test_monexp.c       VERSION: 1.0
*
* DESCRIPTION :
*       Testing of monexp.c
*
*
* NOTES : 
* 
* AUTHOR :    mraes        START DATE :    25 Mar 16
*
*
*****/
#include <stdint.h>
#include <stdio.h>
#include "monexp.h"

// TESTVECTORS (LITTLE ENDIAN)
uint8_t b[SIZE]; // base
uint8_t e[SIZE]; // exponent
uint8_t n[SIZE]; // modulus
// output
uint8_t result_mon_exp[SIZE];

int main() 
{
	signed int i;
	montgomery_exponentiation(result_mon_mul,a,b,n);
	printf("\nResult of monexp is:\n");
	for(i=0; i<SIZE; i++)
	{
		fprintf(stdout,"0x%02X,", result_mon_exp[i]);
	}
	printf("\n");
	
	return 0;
}
