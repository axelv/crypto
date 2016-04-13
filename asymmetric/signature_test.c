#include <stdin.h>
#include <stdint.h>

uint8_t rsa_privkey[SIZE];
uint8_t rsa_pubkey[SIZE];
uint8_t msg[];
uint8_t msg_hash[];

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


void EMSA-PKCS1-V1_5-ENCODE(uint8_t *EM, uint8_t *M, int k){
/*Applying the EMSA-PKVS1-V1_5-ENCODE encoding operation
to the message M to produce an encoded message EM of length k octets
*/
}

void RSASSA-PKCS1-V1_5-SIGN(uint8_t *S,uint8_t *privkey, uint8_t *M){
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

// Step 1: EM = EMSA-PKCS1-V1_5-ENCODE (M, k)
// Step 2: S = Sign EM using rsa_privkey
// Step 3: output S
}

int RSASSA-PKCS1-V1_5-VERIFY(uint8_t *pubkey, uint8_t *M, uint8_t *S){
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


// if signature valid:
	// return 1
// else 
	// return 0
}