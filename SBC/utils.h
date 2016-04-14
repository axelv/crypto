#ifndef UTILS_H
#define UTILS_H

/*
 * Signum function
 */
static inline int sign(int x){
	return ((x>0) - (x<0));
}

/*
 * Divides an integer by 2^n and rounds the result to the nearest integer value
 */
static inline int divByPow2(int x, int n){
	return (((x-(x<0))>>(n-1))+1)>>1;
}

/*
 * Scales an int to a short by dividing by 2^15 and rounding to the nearest integer value
 */
static inline short intToShort(int x){
	// return (short) ((sign(x)+x/16384)/2);
	return (short) ((((x-(x<0))>>14)+1)>>1);
}

/*
 * Limits an int to values of a short
 */
static inline int limitIntToShort(int x){
	return (x>SHORTMAX ? SHORTMAX : (x<SHORTMIN ? SHORTMIN : x));
}

/*
 * Scales and clamps a long int to the range of a short (i.e., -32768 to 32767)
 */
static inline short longToShortWithClamping(long x){
	// return (short) ( (x=(sign(x)+x/16384)/2) < -32768 ? -32768 : (x > 32767 ? 32767 : x) );
	return (short) ( (x=(((x-(x<0))>>14)+1)>>1) < -32768 ? -32768 : (x > 32767 ? 32767 : x) );
}

#endif /* UTILS_H */
