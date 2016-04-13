 /***********************************************************************
* FILENAME :        dh_test.c 
*
* DESCRIPTION :
*       Testing the Diffie-Hellman mathematical procedures.
*       It tests if (g^x)^y  == (g^y)^x
*
* 
* AUTHOR :    mraes 
*
*
*****/
 
 
 /* p=2wq+1
  * the generator is of a subgroup of q-1 elements, and q is 160 bits,
  * i.e., recommended key length: 20 bytes 
  */
 #include <stdint.h>
#include "monmult.h"
#include "monexp.h"
#include <stdio.h>

const unsigned char generator[156] = // generator, Gegeven op toledo
	"\x82\xc1\x57\x1c\xf6\x8d\x59\xaa\xc1\x93\x67\xc7\xde\x23\x4b"
	"\xe8\xc3\xb9\x20\xb8\x36\x2d\x21\xf5\x3e\x3c\x6b\xc8\x4e\xaa"
	"\x5c\x54\x8d\x84\x88\x73\x3a\xc3\x27\x8b\xcf\x66\xe6\x35\xbe"
	"\xbd\x20\x73\x4b\x4b\xea\xaa\x48\x46\xf1\xbc\xf4\x8d\xfa\x0d"
	"\x91\xa3\xbb\xd2\x97\x27\x21\x84\x25\x66\x78\x03\x6d\xd5\x66"
	"\x80\xc5\xa1\x63\x6b\x25\x36\xd7\xe2\x3e\xf6\x6a\xac\x1d\x62"
	"\xb7\xdb\x3c\x6f\xbe\x05\x75\x1b\x1b\x64\x2f\x7c\x1a\xba\x7c"
	"\x07\x4f\x48\x8e\x34\x7b\xf4\xd7\xff\x25\x5f\x2d\x13\x4d\x87"
	"\x4b\x06\x54\x19\x84\xe0\x3d\x61\x14\x4e\x7a\xfb\x12\x38\x34"
	"\x62\x5e\xc0\x33\x7d\xea\xe4\x70\xa6\x79\x9c\x05\x87\xbb\xed"
	"\x9a\xcc\xf1\x0d\xf0\x8b";

const unsigned char prime[156] = // de modulus, gegeven op toledo
	"\xd0\xa8\x73\x80\x04\xa3\x5f\x3d\x10\xf9\x34\x92\x24\xcc\x66"
	"\xb8\xdf\x41\x6e\x9b\x8a\x09\x79\x4d\x69\x35\xf7\x12\xa3\x32"
	"\x46\xc6\x43\x51\xef\x3b\xdb\xad\x21\xcb\xf7\x54\x52\x56\x20"
	"\xcb\x8c\xfa\xcc\x71\x48\xea\xe4\x2b\xfb\x4d\x12\x90\x12\xc6"
	"\xf7\xed\x1f\xee\x95\xa1\x3f\xcc\x95\x78\x74\xcf\x55\x91\xb7"
	"\x9f\x4e\xfc\xe6\x28\x44\x27\x5f\x86\xe1\xde\xf3\x20\xe9\x40"
	"\x07\x90\x24\x00\x7d\x17\xaa\x16\xc1\x9f\xc5\x51\x82\xaa\x5c"
	"\xe0\x42\xac\xe5\x7f\xe9\xaf\xaa\x46\x47\xb6\x76\x80\x3e\x03"
	"\xbc\xe8\xc1\x05\x90\xcc\xd0\x48\x79\xa9\x0e\xab\xcb\x18\x51"
	"\x2a\xa5\xdb\x1c\x6b\xa9\xfc\xd2\xea\x89\xae\xab\xe3\x9c\x3f"
	"\x6b\x03\xc7\x98\xac\x43";

	
	int main(){
	signed int i;
	
	uint8_t g[156]; // for storing generator in little endian
	uint8_t p[156]; // for storing prime in little endian
	printf("\n [DH] Generator:\n");
	for(i=155;i>=0;i--){
		g[155-i] = generator[i];
		printf("0x%02X,",g[155-i]);
	}
	printf("\n [DH] Prime:\n");
	for(i=155;i>=0;i--){
		p[155-i] = prime[i];
		printf("0x%02X,",p[155-i]);
	}
	printf("\nPrime IN MAPLE FORMAT:\n");
	for(i=155;i>=0;i--){
		printf("%02X",p[i]);
	}
	
	uint8_t e1[EXPONENT_LENGTH]= {0x01,0x03}; // exponent1 (random number)
	uint8_t e2[EXPONENT_LENGTH]= {0x05,0x01}; // exponent2 (random number)
	uint8_t res1[156];
	uint8_t final1[156];
	montgomery_exponentiation(res1,g,e1,EXPONENT_LENGTH,p); // now res1 contains g to the power e1
	montgomery_exponentiation(final1,res1,e2,EXPONENT_LENGTH,p); // now final1 contains g to the power e1 to the power e2
	printf("\n[DH] FINAL1\n");
	for(i=0;i<156;i++){
		printf("0x%02X,",final1[i]);
	}
	uint8_t res2[156];
	uint8_t final2[156];
	montgomery_exponentiation(res2,g,e2,EXPONENT_LENGTH,p); // now res1 contains g to the power e2
	montgomery_exponentiation(final2,res2,e1,EXPONENT_LENGTH,p); // now final2 contains g to the power e2 to the power e1
	printf("\n[DH] FINAL2\n");
	for(i=0;i<156;i++){
		printf("0x%02X,",final2[i]);
	}
	
	for(i=0;i<156;i++){
		if(final1[i] != final2[i]){
			printf("\n FINAL1 NOT EQUAL TO FINAL2\n"); // If the two computations are not equal, this will print.
			break;
		}
	}
	
	printf("\n");
	return 0;
	}