/*
 * bbs.h
 *
 * @version 0.1 (March 2016)
 *
 * Implementation of the Blum-Blum-Shub algorithm to generate cryptographicly safe pseudorandom bits
 *
 * @author Axel Vanraes <axel.vanraes@student.kuleuven.be>
 * @author Michaël Raes <michael.raes@student.kuleuven.be>
 */
#include <stdint.h>
#include <stdio.h>

/* Choose two primes p & q so that each congruent to 3 mod 4, then n = p*q. */
	
#define n (1049*1061) // = 2109
// The maximum possible length for the random bitstream.
#define MAX_l 128
/* ------------------------------------------------------------------------- */

void random_byte(unsigned char *byte, unsigned *seed){
	unsigned i;
	for (i=0; i<8;i++){
		*seed = (*seed)*(*seed) % n;
		*byte +=((*seed & 1) << i); // Select the LSB of seed en put in the bit with index i
	}
}

/* ------------------------------------------------------------------------- */

int main() {
	
	// The 'seed' has to be a value between 1 & n-1 such that gcd(s,n) = 1
	unsigned s;
	unsigned char x[MAX_l/8] = {0,};
	unsigned l = MAX_l/8; //Number of random bytes needed
	unsigned i;
	printf("Give a seed: \n" );
	scanf("%d", &s);
	s = (s % n-1)+1;	
	
	for(i=0; i < l; i++){
		random_byte(x+i,&s);
		printf("%d: %02X \n",i,x[i]);
	}
	printf("\n Done \n");
	
}



