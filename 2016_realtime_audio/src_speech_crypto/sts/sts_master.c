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

//TODO in header plaatsen
#include <stdio.h>
#include "sts.h"
#include "sts_master.h"
#include "master_keys.c"
#include "dh.c"

static uint8_t x[XYSIZE] = {0,};

// DIFFERENT ALIASSES TO PREVENT CONFUSION
// Structure in memory:
// 	gy, gyx ->	g^y g^y g^y g^y g^y g^y g^y g^y
// 							g^y g^y g^y g^y g^y g^y g^y g^y
// 							g^y g^y g^y g^y g^y g^y g^y g^y
// 							g^y g^y g^y g^y g^y g^y g^y g^y
// 	gx, gxy	->	g^x g^x g^x g^x g^x g^x g^x g^x
// 							g^x g^x g^x g^x g^x g^x g^x g^x
// 							g^x g^x g^x g^x g^x g^x g^x g^x
// 							g^x g^x g^x g^x g^x g^x g^x g^x
// 							g^y g^y g^y g^y g^y g^y g^y g^y
// 							g^y g^y g^y g^y g^y g^y g^y g^y
// 							g^y g^y g^y g^y g^y g^y g^y g^y
// 							g^y g^y g^y g^y g^y g^y g^y g^y

//The second PSIZE bytes contain g^x the first and last PSIZE bytes contain g^y
static uint8_t gyx[3*PSIZE] = {0,};
//The first PSIZE bytes contain g^x the last PSIZE bytes contain g^y
static uint8_t *gxy = (uint8_t *)(gyx+PSIZE);

uint8_t *gy = (uint8_t *)(gyx);

uint8_t *gx = (uint8_t *)(gyx+PSIZE);


/************************/
/*   MASTER == Alice    */
/************************/

/* ------------------------------------------------------------------------- */
/* The master has to start the protocol by sending the message g^y. 
   The generator 'g' is a predetermined number. y is a secret random number. */
   
void init_connection(uint8_t message[PSIZE]){
	uint8_t g[GSIZE];
	uint8_t p[PSIZE];
	
	//generator
	get_generator(g);
	
	//modulus
	get_prime(p);
	
	//TODO: seed has to come from random inputs
	generate_number(x, XYSIZE, 8);
	montgomery_exponentiation(gx, g, x, XYSIZE, p, rmodn_dh, r2modn_dh);

	memcpy(message,gx,PSIZE); //TODO: gewoon adres toekennen aan message?
}

/* ------------------------------------------------------------------------- */
/* Validate the message received from the slave */
uint8_t validate_slave(uint8_t message[PSIZE+CIPHSIZE], uint8_t seqnr[SEQBYTES]){
	uint8_t signed_msg[SIGNSIZE];
	uint8_t p[PSIZE];
	uint8_t nonce[HASHSIZE];
	
	//generate nonce
	compute_SHA256(nonce, seqnr, SEQBYTES);
	
	//copy g^y to the first and the last PSIZE bytes of gyx
	memcpy(gy, message, PSIZE);
	memcpy(gyx+2*PSIZE, message, PSIZE);
	
	//modulus p & n
	get_prime(p);
	
	//calcuate the shared key
	montgomery_exponentiation(MKEY, gy, x, XYSIZE, p, rmodn_dh, r2modn_dh);
	
	//OCB-AES decrypt
   	int i = 0;
   	printf("\nKEY [sts_master]: \n");
   	for(i=0;i<KEYBYTES;i++) printf("%02x",MKEY[i]);
	ocb_decrypt(signed_msg, MKEY, nonce, NULL, 0, message+PSIZE, CIPHSIZE);
	//memcpy(signed_msg, message+PSIZE, SIGNSIZE);
	
	//RSA VERIFY
	if(RSASSA_PKCS1V1_5_VERIFY(gyx, 2*PSIZE, signed_msg, slave_public_exponent, PUB_KEY, slave_modulus, rmodn_slave, r2modn_slave)){
		return 1;
	}else{
		return 0;
	}
	
}
/* ------------------------------------------------------------------------- */
/* Send an encrypted and signed message to the slave */
void reply_slave(uint8_t message[CIPHSIZE], uint8_t seqnr[SEQBYTES]){
	uint8_t signed_msg[SIGNSIZE];
	uint8_t p[PSIZE];
	uint8_t nonce[HASHSIZE];
	
	//generate nonce
	compute_SHA256(nonce, seqnr, SEQBYTES);
	
	//modulus
	get_prime(p);

	//RSA SIGNING
	RSASSA_PKCS1_V1_5_SIGN(signed_msg, master_private_exponent, PRIV_KEY, master_modulus, rmodn_master, r2modn_master, gxy, 2*PSIZE);
	//OCB-AES encrypt
	ocb_encrypt(message, MKEY, nonce, NULL, 0, signed_msg, SIGNSIZE);
}



