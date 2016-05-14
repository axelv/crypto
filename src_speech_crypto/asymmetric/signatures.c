/***********************************************************************
* FILENAME :        signatures.c
*
* DESCRIPTION :
*       Signature Generation and Validation functions
*
*
* AUTHOR :    mraes
*
*
*****/

#include <stdio.h>
#include <stdint.h>
#include "../ciphers/sha2/sha2_wrapper.h"
#include <string.h> // for memcpy and memcmp
#include "monexp.h"
#include "monmult.h"
#include "signatures.h"
#include "../constants.h"

#define OLDSIZE 156

#define SIGPRINT 0 // enable debug statements for this module

/* From RFC 3447, Public-Key Cryptography Standards (PKCS) #1: RSA
 * Cryptography Specifications Version 2.1.
 *
 *     id-sha256    OBJECT IDENTIFIER ::=
 *       {joint-iso-itu-t(2) country(16) us(840) organization(1)
 *         gov(101) csor(3) nistalgorithm(4) hashalgs(2) 1}
 */
const uint8_t
sha256_prefix[] =
{
  /* 19 octets prefix, 32 octets hash, total 51 */
  0x30,      49, /* SEQUENCE */
    0x30,    13, /* SEQUENCE */
      0x06,   9, /* OBJECT IDENTIFIER */
        0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01,
      0x05,   0, /* NULL */
    0x04,    32  /* OCTET STRING */
      /* Here comes the raw hash value */
};
unsigned int sha256_prefix_size = sizeof(sha256_prefix);

void EMSA_PKCS1_V1_5_ENCODE(uint8_t *EM, uint8_t *M,unsigned int M_length, unsigned int emLen){
/*Applying the EMSA-PKCS1-V1_5-ENCODE encoding operation
to the message M to produce an encoded message EM of length k octets
*/
	unsigned int i;

//Step 1: Apply the hash function to the message M.
	uint8_t hashedM[SHA256_DIGEST_LENGTH];
    compute_SHA256(hashedM,M,M_length);

	#if SIGPRINT
	printf("\n[EMSA] Output of SHA256:\n0x");
	for(i=0;i<SHA256_DIGEST_LENGTH;i++){
		printf("%02x",hashedM[i]);
	}
	printf("\n");
	printf("\n[EMSA] STANDARD PREFIX:\n0x");
	for(i=0;i<sha256_prefix_size;i++){
		printf("%02x",sha256_prefix[i]);
	}
	printf("\n");
	#endif

	memcpy(EM+emLen-SHA256_DIGEST_LENGTH,hashedM,SHA256_DIGEST_LENGTH);

// Step 2: Put the prefix in front, total is T with length tLen
	unsigned int tLen = sha256_prefix_size + SHA256_DIGEST_LENGTH; // should be 51
	memcpy(EM+emLen-tLen,sha256_prefix,sha256_prefix_size);

// Step 3: Generate octet string PS consisting of emLen - tLen -3 octets 0xff.
	for(i=0;i<emLen-tLen-3;i++){
		EM[2+i] = 0xFF; // 2+i because the first two bytes HAVE TO BE 0x00 and 0x01 as per specifications
	}
	EM[2+emLen-tLen-3] = 0x00;

// Step 4: Concatenate EM = 0x00 / 0x01 / PS / 0x00 / T
	EM[0] = 0x00;
	EM[1] = 0x01;

// Output EM, should be of length n
// Operations were performed on EM, nothing has to be returned here.
	#if SIGPRINT
	printf("\n[EMSA]_PKCS1_V1.5_ENCODING OUTPUT (%d bytes):\n", emLen);
	for(i=0;i<emLen;i++){
		printf("%02x",EM[i]);
	}
	printf("\n");
	#endif
}

void RSASSA_PKCS1_V1_5_SIGN(uint8_t *S,uint8_t *privkey,unsigned int privkey_length, uint8_t *modulus,uint8_t *rmodn, uint8_t *r2modn, uint8_t *M, unsigned int M_length){
/*
   RSASSA-PKCS1-V1_5-SIGN (K, M)

   Input:
   K        signer's RSA private key 
   M        message to be signed, an octet string

   Output:
   S        signature, an octet string of length k, where k is the
            length in octets of the RSA modulus n

   Errors: "message too long"; "RSA modulus too short"
*/
	uint8_t encoded_msg[OLDSIZE] = {0,};
	unsigned int i;
// Step 1: EM = EMSA-PKCS1-V1_5-ENCODE (M, k)
	EMSA_PKCS1_V1_5_ENCODE(encoded_msg, M, M_length,OLDSIZE);
// Step 2: S = Sign EM using rsa_privkey

	// Let's try and change the encoded msg endianness
	uint8_t encoded_msg_LE[OLDSIZE] = {0,};
	for(i=0;i<OLDSIZE;i++){
		encoded_msg_LE[i] = encoded_msg[OLDSIZE-1-i];
	}
	montgomery_exponentiation(S,encoded_msg_LE,privkey,privkey_length,modulus,rmodn, r2modn);

// Step 3: output S
	#if SIGPRINT
	printf("\n[RSASSA]_PKCS1_V1_5_SIGN\n");
	for(i=0;i<OLDSIZE;i++){
		printf("%02x",S[i]);
	}
	printf("\n");
	#endif
}

uint8_t RSASSA_PKCS1V1_5_VERIFY(uint8_t *M, unsigned int M_length, uint8_t *S,uint8_t *pubkey, unsigned int pubkey_length, uint8_t *modulus, uint8_t *rmodn, uint8_t *r2modn){
/*
   RSASSA-PKCS1-V1_5-VERIFY ((n, e), M, S)

   Input:
   (n, e)   signer's RSA public key
   M        message whose signature is to be verified, an octet string
   S        signature to be verified, an octet string of length k, where
            k is the length in octets of the RSA modulus n

   Output:
   "valid signature" or "invalid signature"

   Errors: "message too long"; "RSA modulus too short"
*/
	uint8_t encoded_msg[OLDSIZE];
	uint8_t msg_to_be_compared_to_encoded[OLDSIZE];
	unsigned int i;
// Step 1: Verify signature using rsa_pubkey
	montgomery_exponentiation(msg_to_be_compared_to_encoded,S,pubkey,pubkey_length,modulus,rmodn,r2modn);
	#if SIGPRINT
	printf("\n[RSASSA]_PKCS1_V1_5_VERIFY: msg_to_be_compared_to_encoded\n");
	for(i=0;i<OLDSIZE;i++){
		printf("%02x",msg_to_be_compared_to_encoded[i]);
	}
	printf("\n");
	#endif
// Step 2: EM' = EMSA-PKCS1-V1_5-ENCODE (M, k)

	EMSA_PKCS1_V1_5_ENCODE(encoded_msg, M, M_length,OLDSIZE);
	// change encoded msg endianness
	uint8_t encoded_msg_LE[OLDSIZE];
	for(i=0;i<OLDSIZE;i++){
		encoded_msg_LE[i] = encoded_msg[OLDSIZE-1-i];
	}
// Step 3: if EM' == EM: Signature VALID
	int Signature_Valid = !memcmp(encoded_msg_LE,msg_to_be_compared_to_encoded,OLDSIZE); // 0 if equal
	#if SIGPRINT
	if(Signature_Valid){
		printf("\n[RSASSA]_PKCS1_V1_5_VERIFY : SIGNATURE IS VALID\n");
	}else{
		printf("\n[RSASSA]_PKCS1_V1_5_VERIFY : SIGNATURE INVALID\n");
	}
	#endif
	return Signature_Valid;
}
