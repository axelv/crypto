/*
 * bbs.c
 *
 * @version 0.1 (March 2016)
 *
 * Implementation of the Blum-Blum-Shub algorithm to generate cryptographicly safe pseudorandom bits
 *
 * @author Axel Vanraes <axel.vanraes@student.kuleuven.be>
 * @author Micha묠Raes <michael.raes@student.kuleuven.be>
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Choose two primes p & q so that each congruent to 3 mod 4, then n = p*q. */
	
#define n (1049*1061) // = 2109
// The maximum possible length (in bits) for the random bitstream.
#define MAX_l 256
/* ------------------------------------------------------------------------- */

void random_byte(unsigned char *byte, unsigned *seed){
	unsigned i;
	for (i=0; i<8;i++){
		*seed = (*seed)*(*seed) % n; // the new seed: s(n) = s(n-1)² mod n
		*byte +=((*seed & 1) << i); // Select the LSB of seed en put in the bit with index i
	}
}

/* ------------------------------------------------------------------------- */

int generate_number(uint8_t *result, uint8_t len_bits, uint8_t seed) {
	
	unsigned l = max(MAX_l/8, len_bits/8); //Number of random bytes needed
	unsigned i;
	// The 'seed' has to be a value between 1 & n-1 such that gcd(s,n) = 1
	// TODO: check when the seed = n-2
	seed = (seed % n-1)+1;	
	
	for(i=0; i < l; i++){
		random_byte(result+i,&seed);
	}	
}



