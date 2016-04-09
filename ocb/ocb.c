/*------------------------------------------------------------------------
/ OCB Version 3 Reference Code (Unoptimized C)   Last modified 12-JUN-2013
/-------------------------------------------------------------------------
/ Copyright (c) 2013 Ted Krovetz.
/
/ Permission to use, copy, modify, and/or distribute this software for any
/ purpose with or without fee is hereby granted, provided that the above
/ copyright notice and this permission notice appear in all copies.
/
/ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
/ WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
/ MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
/ ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
/ WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
/ ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
/ OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
/
/ Phillip Rogaway holds patents relevant to OCB. See the following for
/ his free patent grant: http://www.cs.ucdavis.edu/~rogaway/ocb/grant.htm
/
/ Comments are welcome: Ted Krovetz <ted@krovetz.net>
/------------------------------------------------------------------------- */

/* This implementation is not optimized and is suceptible to timing attack.
/  It mirrors the OCB RFC to aid in understanding and should not be used
/  for any other purpose. This implementation manipulates data as bytes
/  rather than machine words, which avoids endian issues entirely.
/  To compile: gcc -lcrypto ocb_ref.c                                      */

#include <string.h>
#include "aes.h"
#include <stdio.h>
#include <stdlib.h> // MAG WEG??? 


#define KEYBYTES   (128/8)  // = 128 bit = 16 byte 
#define NONCEBYTES (96/8)
#define TAGBYTES   (128/8)
#define BLOCKBYTES (128/8)


#if !(KEYBYTES==16 || KEYBYTES==24 || KEYBYTES==32) ||  \
     (NONCEBYTES > 15 || NONCEBYTES < 0) ||             \
     (TAGBYTES > 16 || TAGBYTES < 1)
#error -- KEYBYTES, NONCEBYTES, or TAGBYTES is an illegal value
#endif

typedef unsigned char block[16];

/* ------------------------------------------------------------------------- */

static void xor_block(block d, block s1, block s2) {
    unsigned i;
    for (i=0; i<16; i++)
        d[i] = s1[i] ^ s2[i];
}

/* ------------------------------------------------------------------------- */

static void double_block(block d, block s) {
    unsigned i;
    unsigned char tmp = s[0];
    for (i=0; i<15; i++)
        d[i] = (s[i] << 1) | (s[i+1] >> 7);
    d[15] = (s[15] << 1) ^ ((tmp >> 7) * 135);
}

/* ------------------------------------------------------------------------- */

static void calc_L_i(block l, block ldollar, unsigned i) {
    double_block(l, ldollar);         /* l is now L_0               */
    for ( ; (i&1)==0 ; i>>=1)
        double_block(l,l);            /* double for each trailing 0 */
}

/* ------------------------------------------------------------------------- */

static void hash(block result, unsigned char *k,
                 unsigned char *a, unsigned abytes) {
    struct aes_key_st aes_key;
    block lstar, ldollar, offset, sum, tmp;
    unsigned i;
    
    /* Key-dependent variables */
    
    /* L_* = ENCIPHER(K, zeros(128)) */
    AES_set_encrypt_key(k, KEYBYTES*8, &aes_key);
    memset(tmp, 0, 16);
    AES_encrypt(tmp, lstar, &aes_key);
    /* L_$ = double(L_*) */
    double_block(ldollar, lstar); 
    
    /* Process any whole blocks */
    
    /* Sum_0 = zeros(128) */
    memset(sum, 0, 16);
    /* Offset_0 = zeros(128) */
    memset(offset, 0, 16);
    for (i=1; i<=abytes/16; i++, a = a + 16) {
        /* Offset_i = Offset_{i-1} xor L_{ntz(i)} */
        calc_L_i(tmp, ldollar, i);
        xor_block(offset, offset, tmp);
        /* Sum_i = Sum_{i-1} xor ENCIPHER(K, A_i xor Offset_i) */
        xor_block(tmp, offset, a);
        AES_encrypt(tmp, tmp, &aes_key);
        xor_block(sum, sum, tmp);
    }

    /* Process any final partial block; compute final hash value */

    abytes = abytes % 16;  /* Bytes in final block */
    if (abytes > 0) {
        /* Offset_* = Offset_m xor L_* */
        xor_block(offset, offset, lstar);
        /* tmp = (A_* || 1 || zeros(127-bitlen(A_*))) xor Offset_* */
        memset(tmp, 0, 16);
        memcpy(tmp, a, abytes);
        tmp[abytes] = 0x80;
        xor_block(tmp, offset, tmp);
        /* Sum = Sum_m xor ENCIPHER(K, tmp) */
        AES_encrypt(tmp, tmp, &aes_key);
        xor_block(sum, tmp, sum);
    }
    
    memcpy(result, sum, 16);
}

/* ------------------------------------------------------------------------- */

static int ocb_crypt(unsigned char *out, unsigned char *k, unsigned char *n,
                     unsigned char *a, unsigned abytes,
                     unsigned char *in, unsigned inbytes, int encrypting) {
    aes_key aes_encrypt_key, aes_decrypt_key;
    block lstar, ldollar, sum, offset, ktop, pad, nonce, tag, tmp;
    unsigned char stretch[24];
    unsigned bottom, byteshift, bitshift, i;
    
    /* Setup AES and strip ciphertext of its tag */
    if ( ! encrypting ) {
         if (inbytes < TAGBYTES) return -1;
         inbytes -= TAGBYTES;  				//kaas: de #messagebytes = #ciphertextbytes - #tagbytes
         AES_set_decrypt_key(k, KEYBYTES*8, &aes_decrypt_key);
    }
    AES_set_encrypt_key(k, KEYBYTES*8, &aes_encrypt_key);
     
    /* Key-dependent variables */

    /* L_* = ENCIPHER(K, zeros(128)) */
    memset(tmp, 0, 16);
    AES_encrypt(tmp, lstar, &aes_encrypt_key);
    /* L_$ = double(L_*) */
    double_block(ldollar, lstar); 

    /* Nonce-dependent and per-encryption variables */

    /* Nonce = zeros(127-bitlen(N)) || 1 || N */
    memset(nonce,0,16);
    memcpy(&nonce[16-NONCEBYTES],n,NONCEBYTES);
    nonce[0] = (unsigned char)(((TAGBYTES * 8) % 128) << 1);
    nonce[16-NONCEBYTES-1] |= 0x01;
    /* bottom = str2num(Nonce[123..128]) */
    bottom = nonce[15] & 0x3F;
    /* Ktop = ENCIPHER(K, Nonce[1..122] || zeros(6)) */
    nonce[15] &= 0xC0;
    AES_encrypt(nonce, ktop, &aes_encrypt_key);
    /* Stretch = Ktop || (Ktop[1..64] xor Ktop[9..72]) */
    memcpy(stretch, ktop, 16);
    memcpy(tmp, &ktop[1], 8);
    xor_block(tmp, tmp, ktop);
    memcpy(&stretch[16],tmp,8);
    /* Offset_0 = Stretch[1+bottom..128+bottom] */
    byteshift = bottom/8;
    bitshift  = bottom%8;
    if (bitshift != 0)
        for (i=0; i<16; i++)
            offset[i] = (stretch[i+byteshift] << bitshift) |
                        (stretch[i+byteshift+1] >> (8-bitshift));
    else
        for (i=0; i<16; i++)
            offset[i] = stretch[i+byteshift];
    /* Checksum_0 = zeros(128) */
    memset(sum, 0, 16);

    /* Process any whole blocks */
	//kaas: elke blok bevat 128 bit = 16 byte
    for (i=1; i<=inbytes/16; i++, in=in+16, out=out+16) {
        /* Offset_i = Offset_{i-1} xor L_{ntz(i)} */
        calc_L_i(tmp, ldollar, i);
        xor_block(offset, offset, tmp);
        
        xor_block(tmp, offset, in);
        if (encrypting) {
            /* P_i = Offset_i xor DECIPHER(K, C_i xor Offset_i) */
            AES_encrypt(tmp, tmp, &aes_encrypt_key);
            xor_block(out, offset, tmp);
            /* Checksum_i = Checksum_{i-1} xor P_i */
            xor_block(sum, in, sum);
        } else {
            /* P_i = Offset_i xor DECIPHER(K, C_i xor Offset_i) */
            AES_decrypt(tmp, tmp, &aes_decrypt_key);
            xor_block(out, offset, tmp);
            /* Checksum_i = Checksum_{i-1} xor P_i */
            xor_block(sum, out, sum);
        }
		
    }

    /* Process any final partial block and compute raw tag */

    inbytes = inbytes % 16;  /* Bytes in final block */
    if (inbytes > 0) {
        /* Offset_* = Offset_m xor L_* */
        xor_block(offset, offset, lstar);
        /* Pad = ENCIPHER(K, Offset_*) */
        AES_encrypt(offset, pad, &aes_encrypt_key);
        
        if (encrypting) {
            /* Checksum_* = Checksum_m xor (P_* || 1 || zeros(127-bitlen(P_*))) */
            memset(tmp, 0, 16);
            memcpy(tmp, in, inbytes);
            tmp[inbytes] = 0x80;
            xor_block(sum, tmp, sum);
            /* C_* = P_* xor Pad[1..bitlen(P_*)] */
            xor_block(pad, tmp, pad);
            memcpy(out, pad, inbytes);
            out = out + inbytes;
        } else {
            /* P_* = C_* xor Pad[1..bitlen(C_*)] */
            memcpy(tmp, pad, 16);
            memcpy(tmp, in, inbytes);
            xor_block(tmp, pad, tmp);
            tmp[inbytes] = 0x80;     /* tmp == P_* || 1 || zeros(127-bitlen(P_*)) */
            memcpy(out, tmp, inbytes);
            /* Checksum_* = Checksum_m xor (P_* || 1 || zeros(127-bitlen(P_*))) */
            xor_block(sum, tmp, sum);
            in = in + inbytes;
        }
    }
    
    /* Tag = ENCIPHER(K, Checksum xor Offset xor L_$) xor HASH(K,A) */
    xor_block(tmp, sum, offset);
    xor_block(tmp, tmp, ldollar);
    AES_encrypt(tmp, tag, &aes_encrypt_key);
    hash(tmp, k, a, abytes);
    xor_block(tag, tmp, tag);
    
    if (encrypting) {
        memcpy(out, tag, TAGBYTES);
        return 0;
    } else
        return (memcmp(in,tag,TAGBYTES) ? -1 : 0);     /* Check for validity */
}

/* ------------------------------------------------------------------------- */

#define OCB_ENCRYPT 1
#define OCB_DECRYPT 0

void ocb_encrypt(unsigned char *c, unsigned char *k, unsigned char *n,
                 unsigned char *a, unsigned abytes,
                 unsigned char *p, unsigned pbytes) {
    ocb_crypt(c, k, n, a, abytes, p, pbytes, OCB_ENCRYPT);
}

/* ------------------------------------------------------------------------- */

int ocb_decrypt(unsigned char *p, unsigned char *k, unsigned char *n,
                unsigned char *a, unsigned abytes,
                unsigned char *c, unsigned cbytes) {
    return ocb_crypt(p, k, n, a, abytes, c, cbytes, OCB_DECRYPT);
}

/* ------------------------------------------------------------------------- */


int main() {
    unsigned char m[BLOCKBYTES] = "HALLO_IKBEN_KAAS";	//kaas: plaintext - 128 bit  array (16 bytes)
	unsigned char a[BLOCKBYTES] = "_GEHEIM_BERICHT_";   //kaas: authenticated data
    unsigned char k[KEYBYTES] = "3LLEBBLAP3EVUIOP";		//kaas: symmetric key - 96 bit  array (12 bytes)
	unsigned char n[NONCEBYTES] = "27X63WKO3X8F";		//kaas: noncebytes to create L-table
    unsigned char p[BLOCKBYTES];						//kaas: decyphered plaintext 
	unsigned char c[BLOCKBYTES+TAGBYTES] = {0,};		//kaas: cyphertext
    int result_msg;										//kaas: boolean to check if the initial plaintext = received plaintext
	int result_tag;										//kaas: boolean to check if the AUTH TAG is valid
	int i;
	
	
	ocb_encrypt(c, k, n, a, BLOCKBYTES, m, BLOCKBYTES);

	printf("Plaintext (CHAR): \n");
	for (i=0; i<BLOCKBYTES; i++) printf("%c", m[i]);
	printf("\n");
	printf("\n");
	
	printf("Authenticated Data (CHAR): \n");
	for (i=0; i<BLOCKBYTES; i++) printf("%c", a[i]);
	printf("\n");
	printf("\n");
	
	printf("Encryptionkey (HEX): \n");
	for (i=0; i<KEYBYTES; i++) printf("%02X", k[i]);
	printf("\n");
	printf("\n");
	
	printf("Ciphertext (HEX) & Tag (HEX): \n");
	for (i=0; i<BLOCKBYTES; i++) printf("%02X", c[i]);
    printf("\n");
	for (i=BLOCKBYTES; i<BLOCKBYTES+TAGBYTES; i++) printf("%02X", c[i]);
    printf("\n");
	printf("\n");
	result_tag = ocb_decrypt(p, k, n, a, BLOCKBYTES ,c , BLOCKBYTES+TAGBYTES);
	
	printf("Output plaintext (CHAR): \n");
	for (i=0; i<BLOCKBYTES; i++) printf("%c", p[i]);
    printf("\n");
	printf("\n");
		
	result_msg = memcmp(p,  m, BLOCKBYTES);
	if(result_msg || result_tag){
		printf("FAIL \n");
		printf("Result Tag: %d \n", result_tag);
		printf("Result message: %d \n", result_msg);
		
	}else{
		printf("Well done! \n");
		printf("Result Tag: %d \n", result_tag);
		printf("Result message: %d \n \n", result_msg);
	}
}
