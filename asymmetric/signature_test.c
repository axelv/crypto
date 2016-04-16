/***********************************************************************
* FILENAME :        signature_test.c       VERSION: 1.0
*
* DESCRIPTION :
*       Testing of Signature Generation and Validation
*
*
* NOTES : 
* 
* AUTHOR :    mraes 
*
*
*****/

#include <stdio.h>
#include <stdint.h>
#include "signatures.h"
#include "keys.c" // Holds keys in Big Endian and functions to get them in Little Endian
#include "monmult.h" // for the SIZE parameter (will be moved)
#include "monexp.h" // for the EXPONENT_LENGTH parameter (will be moved)

void main(){
     unsigned int i;
	// TEST SIGNING the msg "abc" (0x61,0x62,0x63 in hex)
	uint8_t master_privkey[SIZE];
	uint8_t master_pubkey[3];
	uint8_t master_mod[SIZE];
	uint8_t slave_privkey[SIZE];
	uint8_t slave_pubkey[3];
	uint8_t slave_mod[SIZE];
	
	// These functions are needed for transformation to Little Endian, they get the keys from keys.c
	get_master_modulus(master_mod);
     printf("\n[RSA SETUP] Master Modulus\n");
     for(i=0;i<SIZE;i++){
          printf("0x%02X,",master_mod[i]);
     }
     printf("\n");
	 
	get_slave_modulus(slave_mod);
     printf("\n[RSA SETUP] Slave Modulus\n");
     for(i=0;i<SIZE;i++){
          printf("0x%02X,",slave_mod[i]);
     }
     printf("\n");	 
	 
	get_master_privkey(master_privkey);
     printf("\n[RSA SETUP] Master Private Key\n");
     for(i=0;i<SIZE;i++){
          printf("0x%02X,",master_privkey[i]);
     }
     printf("\n");

	get_slave_privkey(slave_privkey);
     printf("\n[RSA SETUP] Slave Private Key\n");
     for(i=0;i<SIZE;i++){
          printf("0x%02X,",slave_privkey[i]);
     }
     printf("\n");

	get_master_pubkey(master_pubkey);
     printf("\n[RSA SETUP] Master Public Key\n");
     for(i=0;i<3;i++){
          printf("0x%02X,",master_pubkey[i]);
     }
     printf("\n");

	get_slave_pubkey(slave_pubkey);
     printf("\n[RSA SETUP] Slave Public Key\n");
     for(i=0;i<3;i++){
          printf("0x%02X,",slave_pubkey[i]);
     }
     printf("\n");
	
	
	
	uint8_t msg[] = "The quick brown fox jumps over the lazy dog";
    unsigned int msg_length = sizeof(msg);
	uint8_t msg_signature[SIZE];

     // Generate Signature for MASTER
    RSASSA_PKCS1_V1_5_SIGN(msg_signature,master_privkey,SIZE,master_mod, rmodn_master, r2modn_master, msg, msg_length );
     // VERIFY MASTER SIGNATURE
	uint8_t master_sigValid = RSASSA_PKCS1V1_5_VERIFY(msg, msg_length, msg_signature,master_pubkey, 3, master_mod, rmodn_master, r2modn_master);
	printf("\nMASTER Signature Valid: %d\n",master_sigValid);
	printf("\n ------------------------------------------------------------------------------------------------------\n");
     // Generate Signature for SLAVE
     RSASSA_PKCS1_V1_5_SIGN(msg_signature,slave_privkey,SIZE,slave_mod,rmodn_slave, r2modn_slave, msg, msg_length);
     // VERIFY SLAVE SIGNATURE
	uint8_t slave_sigValid = RSASSA_PKCS1V1_5_VERIFY(msg, msg_length, msg_signature,slave_pubkey, 3,slave_mod,rmodn_slave, r2modn_slave);
	printf("\nSLAVESignature Valid: %d\n",slave_sigValid);
}
