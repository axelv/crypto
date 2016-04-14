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
#include "sha2_wrapper.h"
#include <string.h>
#include "monexp.h"
#include "monmult.h"

#define SIGNATURE_DEBUG 1

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
	
	#ifdef SIGNATURE_DEBUG
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
	#ifdef SIGNATURE_DEBUG
	printf("\n[EMSA]_PKCS1_V1.5_ENCODING OUTPUT (%d bytes):\n", emLen);
	for(i=0;i<emLen;i++){
		printf("%02x",EM[i]);
	}
	printf("\n");
	#endif
}

void RSASSA_PKCS1_V1_5_SIGN(uint8_t *S,uint8_t *privkey,unsigned int privkey_length, uint8_t *modulus, uint8_t *M, unsigned int M_length){
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
	uint8_t encoded_msg[SIZE];
	unsigned int i;
// Step 1: EM = EMSA-PKCS1-V1_5-ENCODE (M, k)
	EMSA_PKCS1_V1_5_ENCODE(encoded_msg, M, M_length,SIZE);
// Step 2: S = Sign EM using rsa_privkey
	montgomery_exponentiation(S,encoded_msg,privkey,privkey_length,modulus);

// Step 3: output S
	#ifdef SIGNATURE_DEBUG
	printf("\n[RSASSA]_PKCS1_V1_5_SIGN\n");
	for(i=0;i<SIZE;i++){
		printf("%02x",S[i]);
	}
	printf("\n");
	#endif
}

uint8_t RSASSA_PKCS1V1_5_VERIFY(uint8_t *M, unsigned int M_length, uint8_t *S,uint8_t *pubkey, unsigned int pubkey_length, uint8_t *modulus){
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
	uint8_t encoded_msg[SIZE];
	uint8_t msg_to_be_compared_to_encoded[SIZE];
	unsigned int i;
// Step 1: Verify signature using rsa_pubkey
	montgomery_exponentiation(msg_to_be_compared_to_encoded,S,pubkey,pubkey_length,modulus);
	#ifdef SIGNATURE_DEBUG
	printf("\n[RSASSA]_PKCS1_V1_5_VERIFY: msg_to_be_compared_to_encoded\n");
	for(i=0;i<SIZE;i++){
		printf("%02x",msg_to_be_compared_to_encoded[i]);
	}
	printf("\n");
	#endif
// Step 2: EM' = EMSA-PKCS1-V1_5-ENCODE (M, k)

	EMSA_PKCS1_V1_5_ENCODE(encoded_msg, M, M_length,SIZE);
// Step 3: if EM' == EM: Signature VALID
	int Signature_Valid = !memcmp(encoded_msg,msg_to_be_compared_to_encoded,SIZE); // 0 if equal
	#ifdef SIGNATURE_DEBUG
	if(Signature_Valid){
		printf("\n[RSASSA]_PKCS1_V1_5_VERIFY : SIGNATURE IS VALID\n");
	}else{
		printf("\n[RSASSA]_PKCS1_V1_5_VERIFY : SIGNATURE INVALID\n");
	}
	#endif
	return Signature_Valid;
}

void main(){
	//TEST ENCODING
	/*
	uint8_t msg[3] = {0x61,0x62,0x63};
	uint8_t encodedmsg[SIZE];
	int modsizetest = SIZE;
	EMSA_PKCS1_V1_5_ENCODE(encodedmsg,msg,sizeof(msg),modsizetest);
	*/
	
	// TEST SIGNING
	
	uint8_t msg[3] = {0x61,0x62,0x63};
	uint8_t msg_signature[SIZE];
	uint8_t private_key[EXPONENT_LENGTH]= {0x01,0x01}; // exponent
	uint8_t public_key[SIZE] =      {0x01, 0x0F, 0xEC, 0x4A, 0xDD, 0x26, 0x57, 0x7D, 0x77, 0x31, 0x93, 0xE1, 0xCF, 0x05, 0xC5, 0x06, 0xE4, 0x0F, 0x04, 0xF9, 0x08, 0x57, 0x0C, 0x73, 0x21, 0xA9, 0x73, 0xFB, 0xA8, 0x3F, 0x53, 0x43, 0x1F, 0x7F, 0x4C, 0xC7, 0xD8, 0xB2, 0xFE, 0x6E, 0xC1, 0x72, 0x40, 0xF5, 0x6C, 0xA8, 0x68, 0x13, 0x87, 0x8B, 0x36, 0x21, 0x74, 0x59, 0x44, 0x66, 0xBF, 0xC7, 0x49, 0x24, 0x96, 0xC4, 0xFA, 0x0C, 0x8A, 0xF3, 0x84, 0xCE, 0xDC, 0xD0, 0x5C, 0x61, 0x9B, 0x2E, 0x9C, 0x51, 0xE9, 0x5D, 0xFA, 0x84, 0x65, 0x77, 0x4D, 0xF1, 0x90, 0x02, 0x21, 0x01, 0x11, 0x8A, 0xFE, 0x55, 0x6A, 0xD3, 0x8E, 0xB8, 0x57, 0x76, 0xED, 0xB0, 0xA2, 0x72, 0xEB, 0xBE, 0x31, 0x6C, 0x0D, 0x59, 0x9E, 0x63, 0x06, 0xB1, 0x18, 0xAE, 0xF5, 0x65, 0x83, 0xDE, 0x2A, 0xD2, 0x98, 0xB9, 0xD8, 0xF6, 0xBD, 0x25, 0x6E, 0x0B};
	uint8_t n[SIZE] =      {0x31, 0x6F, 0x7A, 0xE6, 0xE5, 0xBC, 0x77, 0xA6, 0xF8, 0x66, 0x9E, 0x3B, 0x02, 0xF4, 0xDF, 0x32, 0xC8, 0xA7, 0xB9, 0x2A, 0x1E, 0xBC, 0x08, 0xBE, 0x86, 0x11, 0xDE, 0x2A, 0xCF, 0xE8, 0x88, 0x8E, 0x3B, 0x5C, 0x6A, 0x71, 0x47, 0x3C, 0x45, 0x05, 0x46, 0xE4, 0xAD, 0x93, 0x42, 0x1F, 0x77, 0xDC, 0x25, 0x97, 0x70, 0xE1, 0xB5, 0xEB, 0x89, 0x7F, 0xA9, 0x1E, 0x50, 0x1D, 0x33, 0x99, 0x3C, 0x09, 0x68, 0xE4, 0x1A, 0xCF, 0xB0, 0x1E, 0xBA, 0x56, 0x24, 0x7C, 0x8E, 0x35, 0x84, 0xE0, 0xAA, 0x2D, 0xEC, 0x58, 0xA0, 0x4D, 0x89, 0x8A, 0x02, 0x27, 0x38, 0x54, 0x2E, 0x4F, 0xB2, 0x96, 0xAB, 0x29, 0x01, 0x21, 0x07, 0xF9, 0x85, 0x01, 0x99, 0xE7, 0xFE, 0xF8, 0x9A, 0x19, 0x6D, 0x92, 0x87, 0x68, 0x0E, 0x45, 0xB0, 0xCF, 0xD9, 0x3D, 0xA5, 0x36, 0x2C, 0x98, 0xD3, 0x0E, 0xD6, 0xB4, 0xD3, 0xD1}; // modulus
	RSASSA_PKCS1_V1_5_SIGN(msg_signature,private_key,EXPONENT_LENGTH,n, msg, 3);
	uint8_t oy = RSASSA_PKCS1V1_5_VERIFY(msg, 3, msg_signature,public_key, SIZE,n);

}
