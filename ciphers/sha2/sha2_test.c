 /***********************************************************************
* FILENAME :        sha2_test.c 
*
* DESCRIPTION :
*       SHA256 wraper
*
* 
* AUTHOR :    mraes 
*
*
*****/

#include "sha2_wrapper.h"
#include <stdio.h>
#include <stdint.h>


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
}


