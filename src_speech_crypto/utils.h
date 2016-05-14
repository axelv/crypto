#ifndef UTILS_H
#define UTILS_H

/*
 * Signum function
 */
#define SIGN(x) (((x)>0) - ((x)<0))

/*
 * Clamps an int to values of a short
 */
#define CLAMPINT(x) ((x)>32767 ? 32767 : ((x)<-32768 ? -32768 : (x)))

/*
 * Devides an integer by 2^n and rounds the result to the nearest integer value
 */
#define DIVBYPOW2(x,n) (((((x)-((x)<0))>>((n)-1))+1)>>1)

/*
 * Scales and clamps an int to a short by dividing by 2^15 and rounding
 * to the nearest integer value
 */
//define TOSHORT(x) ((x)<-1073758207 ? -32768 : ((x)>1073725439 ? 32767 : (short) ((((x)-((x)<0))>>14)+1)>>1));

/*
 * Scales and clamps int to a short by dividing by 2^15 and rounding to the nearest integer value
 */
static inline short intToShort(int x){
	// return (short) ( (x=(sign(x)+x/16384)/2) < -32768 ? -32768 : (x > 32767 ? 32767 : x) );
	return (short) ( (x=(((x-(x<0))>>14)+1)>>1) < -32768 ? -32768 : (x > 32767 ? 32767 : x) );
}

/*
 * Scales and clamps a long int to a short by dividing by 2^15 and rounding to the nearest integer value
 */
static inline short longToShort(long x){
	// return (short) ( (x=(sign(x)+x/16384)/2) < -32768 ? -32768 : (x > 32767 ? 32767 : x) );
	return (short) ( (x=(((x-(x<0))>>14)+1)>>1) < -32768 ? -32768 : (x > 32767 ? 32767 : x) );
}

#endif /* UTILS_H */
