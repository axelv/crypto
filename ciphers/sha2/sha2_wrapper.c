 /***********************************************************************
* FILENAME :        sha2_test.c 
*
* DESCRIPTION :
*       SHA256 wrapper
*
* 
* AUTHOR :    mraes 
*
*
*****/

#include "sha2.h"
//#include <stdio.h>
#include <stdint.h>
//#include <string.h>

void compute_SHA256(uint8_t *hashResult, uint8_t *to_be_hashed, size_t len_to_be_hashed){
	SHA256_CTX ctx;
     SHA256_Init(&ctx);
     SHA256_Update(&ctx, to_be_hashed, len_to_be_hashed);
     SHA256_Final(hashResult, &ctx);

}
/*
// FOR TESTING PURPOSES, SHOULD PRINT 0xba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
void main(){
     u_int8_t results[SHA256_DIGEST_LENGTH];
     uint8_t buf[3] = {0x61,0x62,0x63};
        

     compute_SHA256(results, buf, 3);
     printf("0x");
     unsigned int n;
     for (n = 0; n < SHA256_DIGEST_LENGTH; n++)
         printf("%02x", results[n]);
     putchar('\n');	
}*/


