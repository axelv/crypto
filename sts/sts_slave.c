/*
 * sts.h
 *
 * @version 0.1 (March 2016)
 *
 * Implementation of the Station-to-Station protocol to establish a secure connection with a symmetric key known by the master & slave
 *
 * @author Axel Vanraes <axel.vanraes@student.kuleuven.be>
 * @author Micha묠Raes <michael.raes@student.kuleuven.be>
 */
#include "sts.h"
#include "sts_slave.h"
#include "slave_keys.c"
#include "dh.c"
#include <stdio.h>

static uint8_t y[XYSIZE] = {0,};

// DIFFERENT ALIASSES TO PREVENT CONFUSION
// Structure in memory:
// 	gx, gxy ->				g^x g^x g^x g^x g^x g^x g^x g^x
// 							g^x g^x g^x g^x g^x g^x g^x g^x
// 							g^x g^x g^x g^x g^x g^x g^x g^x
// 							g^x g^x g^x g^x g^x g^x g^x g^x
// 	gy, gyx	->				g^y g^y g^y g^y g^y g^y g^y g^y
// 							g^y g^y g^y g^y g^y g^y g^y g^y
// 							g^y g^y g^y g^y g^y g^y g^y g^y
// 							g^y g^y g^y g^y g^y g^y g^y g^y
// 							g^x g^x g^x g^x g^x g^x g^x g^x
// 							g^x g^x g^x g^x g^x g^x g^x g^x
// 							g^x g^x g^x g^x g^x g^x g^x g^x
// 							g^x g^x g^x g^x g^x g^x g^x g^x

//The second PSIZE bytes contain g^x the first and last PSIZE bytes contain g^y
static uint8_t gxy[3*PSIZE] = {0,};
//The first PSIZE bytes contain g^x the last PSIZE bytes contain g^y
static uint8_t *gyx = (uint8_t *)(gxy+PSIZE);

static uint8_t *gx = (uint8_t *)(gxy);

static uint8_t *gy = (uint8_t *)(gxy+PSIZE);

/************************/
/*   Slave == Bob       */
/************************/

/* ------------------------------------------------------------------------- */
/* Send an encrypted and signed message to the slave */
void reply_master(uint8_t message[PSIZE+CIPHSIZE], uint8_t received_msg[PSIZE], uint8_t seqnr[SEQBYTES]){
	uint8_t signed_msg[SIGNSIZE];
	uint8_t private_key[PRIV_KEY];
	uint8_t public_key[PUB_KEY];
	uint8_t p[PSIZE];
	uint8_t n[NSIZE];
	uint8_t g[GSIZE];
	uint8_t nonce[HASHSIZE];

	//generate nonce
	compute_SHA256(nonce, seqnr, SEQBYTES);
	
	//copy received g^x to memory
	memcpy(gx, received_msg, PSIZE);
	memcpy(gx+2*PSIZE, received_msg, PSIZE);	
	
	///modulus
	get_slave_modulus(n);
	get_prime(p);
	
	//private key
	get_slave_privkey(private_key); 
	get_slave_pubkey(public_key);
	
	//generator
	get_generator(g);
	
	//First part of message: g^y
	//TODO: seed has to come from random inputs
	generate_number(y, XYSIZE, 9);
	montgomery_exponentiation(gy, g, y, XYSIZE, p, rmodn_dh, r2modn_dh);
	memcpy(message,gy,PSIZE);//TODO: gewoon adres toekennen aan message?
	
	//RSA SIGNING
	RSASSA_PKCS1_V1_5_SIGN(signed_msg, private_key, PRIV_KEY, n, rmodn_slave, r2modn_slave, gyx, 2*PSIZE);
	
	
	//calcuate the shared key
	montgomery_exponentiation(K, gx, y, XYSIZE, p, rmodn_dh, r2modn_dh);
	
	//OCB-AES encrypt
	
	memcpy(message+PSIZE, signed_msg, SIGNSIZE);
	//ocb_encrypt(message+PSIZE, K, nonce, NULL, 0, signed_msg, SIGNSIZE);
	
}

/* ------------------------------------------------------------------------- */
/* Validate the message received from the master */
uint8_t validate_master(uint8_t message[SIGNSIZE], uint8_t seqnr[SEQBYTES]){
	uint8_t signed_msg[SIGNSIZE];
	uint8_t public_key[PUB_KEY];
	uint8_t p[PSIZE];
	uint8_t n[NSIZE];
	uint8_t nonce[HASHSIZE];
	
	//modulus
	get_master_modulus(n);
	get_prime(p);
	
	//public key
	get_master_pubkey(public_key);
	
	//calcuate the shared key
	montgomery_exponentiation(K, gx, y, XYSIZE, p, rmodn_dh, r2modn_dh);
	
	//generate nonce
	compute_SHA256(nonce, seqnr, SEQBYTES);
	
	//OCB-AES decrypt
	//ocb_decrypt(signed_msg, K, nonce, NULL, 0, message, SIGNSIZE);
	memcpy(signed_msg, message, SIGNSIZE);
	
	/*printf("\nrMODn: \n"); int i;
	for(i=0;i<NSIZE;i++)printf("%02x",rmodn_master[i]);
	printf("\n");
	printf("\nr2RMODn: \n");
	for(i=0;i<NSIZE;i++)printf("%02x",r2modn_master[i]);
	printf("\n");
	printf("\nPUB_KEY: \n");
	for(i=0;i<PUB_KEY;i++)printf("%02x",public_key[i]);
	printf("\n");
	printf("\nModulus: \n");
	for(i=0;i<NSIZE;i++)printf("%02x",n[i]);
	printf("\n");*/
	
	//RSA VERIFY
	if(RSASSA_PKCS1V1_5_VERIFY(gxy, 2*PSIZE, signed_msg, public_key, PUB_KEY, n, rmodn_master, r2modn_master)){
		return 1;
	}else{
		return 0;
	}
}

