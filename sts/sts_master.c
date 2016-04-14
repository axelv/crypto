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
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "sts.h"

static uint8_t p[PSIZE] = {0x50, 0x24, 0x6E, 0x0E, 0x44, 0xDD, 0xE3, 0x57, 0xEB, 0xB9, 0xEF, 0xCE, 0xB4, 0x10, 0x8D, 0xBF, 
                    0xC8, 0xEA, 0xC3, 0x00, 0x0E, 0xCF, 0x52, 0x2D, 0x25, 0x9D, 0x87, 0xA6, 0x1D, 0x4A, 0xB5, 0x4D, 
                    0x67, 0xBC, 0x85, 0xBA, 0x70, 0x43, 0xC2, 0x81, 0xBD, 0x2F, 0x64, 0x53, 0xED, 0x32, 0x3C, 0x2D, 
                    0x31, 0x23, 0xCF, 0xD8, 0x0F, 0xE9, 0x13, 0x51, 0x2D, 0xF8, 0xAC, 0x36, 0xA6, 0x5D, 0x8F, 0x70, 
                    0x22, 0x60, 0x82, 0x88, 0x8C, 0xE3, 0x43, 0x4A, 0x9F, 0x87, 0x05, 0xED, 0xC9, 0x13, 0x0A, 0xBE, 
                    0xCF, 0x6F, 0x07, 0x41, 0x89, 0x7A, 0xE8, 0xBD, 0xF1, 0xDD, 0x33, 0x70, 0x80, 0xD0, 0x10, 0xA3, 
                    0xD2, 0x25, 0xB3, 0xCC, 0x4C, 0x62, 0x19, 0xBC, 0x7A, 0x92, 0x8B, 0x46, 0xBC, 0x7A, 0xF4, 0xA7, 
                    0xC6, 0xF7, 0x1A, 0x76, 0xEE, 0x6E, 0xAC, 0x03, 0x4D, 0xAA, 0x3F, 0xC9, 0x3E, 0xE5, 0x92, 0xC8,
					0x50, 0x24, 0x6E, 0x0E, 0x44, 0xDD, 0xE3, 0x57, 0xEB, 0xB9, 0xEF, 0xCE, 0xB4, 0x10, 0x8D, 0xBF, 
                    0xC8, 0xEA, 0xC3, 0x00, 0x0E, 0xCF, 0x52, 0x2D, 0x25, 0x9D, 0x87, 0xA6, 0x1D, 0x4A, 0xB5, 0x4D, 
                    0x67, 0xBC, 0x85, 0xBA, 0x70, 0x43, 0xC2, 0x81, 0xBD, 0x2F, 0x64, 0x53, 0xED, 0x32, 0x3C, 0x2D, 
                    0x31, 0x23, 0xCF, 0xD8, 0x0F, 0xE9, 0x13, 0x51, 0x2D, 0xF8, 0xAC, 0x36, 0xA6, 0x5D, 0x8F, 0x70, 
                    0x22, 0x60, 0x82, 0x88, 0x8C, 0xE3, 0x43, 0x4A, 0x9F, 0x87, 0x05, 0xED, 0xC9, 0x13, 0x0A, 0xBE, 
                    0xCF, 0x6F, 0x07, 0x41, 0x89, 0x7A, 0xE8, 0xBD, 0xF1, 0xDD, 0x33, 0x70, 0x80, 0xD0, 0x10, 0xA3, 
                    0xD2, 0x25, 0xB3, 0xCC, 0x4C, 0x62, 0x19, 0xBC, 0x7A, 0x92, 0x8B, 0x46, 0xBC, 0x7A, 0xF4, 0xA7, 
                    0xC6, 0xF7, 0x1A, 0x76, 0xEE, 0x6E, 0xAC, 0x03, 0x4D, 0xAA, 0x3F, 0xC9, 0x3E, 0xE5, 0x92, 0xC8};
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

	
//Shared key for AES
static uint8_t K[PSIZE] =  {0,};

/************************/
/*   MASTER == Alice    */
/************************/

/* ------------------------------------------------------------------------- */
/* The master has to start the protocol by sending the message g^y. 
   The generator 'g' is a predetermined number. y is a secret random number. */
void init_connection(uint8_t *message){
	//TODO: seed has to come from random inputs
	generate_number(x, XYSIZE, 8);
	montgomery_exponentiation(gx, g, x, p);
	*message = *gx;
}

/* ------------------------------------------------------------------------- */
/* Validate the message received from the slave */
bool validate_slave(uint8_t *message){
	uint8_t signed_msg[PSIZE];
	uint8_t encrypted_msg[PSIZE];
	//copy g^y to the first and the last PSIZE bytes of gyx
	memcpy(gy, message, PSIZE);
	memcpy(gyx+2*PSIZE, message, PSIZE);
	
	//RSA SIGNING
	
	//calcuate the shared key
	montgomery_exponentiation(K, gy, x, p);
	//TODO:encrypt (OCB) the hash with the shared key
	ocb_decrypt(unsigned char *p, unsigned char *k, unsigned char *n,
                unsigned char *a, unsigned abytes,
                unsigned char *c, unsigned cbytes);	
	//Check signature of slave
	if(memcmp(encrypted_msg, message+PSIZE, PSIZE) == 0){
		return true;
	}else{
		return false;
	}
}
/* ------------------------------------------------------------------------- */
/* Send an encrypted and signed message to the slave */
void reply_slave(uint8_t message[PSIZE]){
	uint8_t *signed_msg;
	//RSA SIGNING
	//calculate hash S(g^y,g^x)
	 SHA256_Data(gxy, 2*PSIZE, signed_msg);
	//encrypt (RSA) the hash with the shared key
	montgomery_exponentiation(message, signed_msg, K, p);	
}



