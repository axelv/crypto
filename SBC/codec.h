#ifndef CODEC_H
#define CODEC_H

/************************************************
* Analysis and synthesis filter bank parameters *
************************************************/

#define NSUBBANDS 5		// Number of subbands
#define NFILTERS 4		// Number of filters (= NSUBBANDS-1)

// Filter lengths (must be even!)
#define LENGTH1	64
#define LENGTH2 32
#define LENGTH3 16
#define LENGTH4 16

extern const short h1[LENGTH1];		// Low-pass analysis filter for stage 1
extern const short h2[LENGTH2]; 	// Low-pass analysis filter for stage 2
extern const short h3[LENGTH3];		// Low-pass analysis filter for stage 3
extern const short h4[LENGTH4];		// Low-pass analysis filter for stage 4

extern const short g1[LENGTH1];		// Low-pass synthesis filter for stage 1
extern const short g2[LENGTH2];		// Low-pass synthesis filter for stage 2
extern const short g3[LENGTH3];		// Low-pass synthesis filter for stage 3
extern const short g4[LENGTH4];		// Low-pass synthesis filter for stage 4

// Low-pass analysis filter coefficients
#define H1 {6,-12,2,11,1,-17,-6,31,12,-55,-17,92,18,-146,-11,222,\
	-10,-323,51,458,-126,-635,252,876,-464,-1226,843,1816,-1647,-3211,4511,15089,\
	15089,4511,-3211,-1647,1816,843,-1226,-464,876,252,-635,-126,458,51,-323,-10,\
	222,-11,-146,18,92,-17,-55,12,31,-6,-17,1,11,2,-12,6}
#define H2 {27,-55,-27,128,34,-284,-3,554,-122,-990,447,1714,-1251,-3270,4177,15303,\
	15303,4177,-3270,-1251,1714,447,-990,-122,554,-3,-284,34,128,-27,-55,27}
#define H3 {28,-145,-81,849,-278,-2922,3141,15786,15786,3141,-2922,-278,849,-81,-145,28}
#define H4 {14,-105,-93,789,-228,-2885,3077,15810,15810,3077,-2885,-228,789,-93,-105,14}

// High-pass analysis filter coefficients
#define G1 {13,-24,4,21,2,-34,-13,62,25,-110,-34,185,35,-293,-21,443,\
	-19,-646,102,915,-251,-1270,503,1752,-928,-2451,1686,3631,-3294,-6422,9021,30179,\
	30179,9021,-6422,-3294,3631,1686,-2451,-928,1752,503,-1270,-251,915,102,-646,-19,\
	443,-21,-293,35,185,-34,-110,25,62,-13,-34,2,21,4,-24,13}
#define G2 {55,-110,-54,257,68,-568,-6,1109,-245,-1980,895,3428,-2502,-6539,8355,30606,\
	30606,8355,-6539,-2502,3428,895,-1980,-245,1109,-6,-568,68,257,-54,-110,55}
#define G3 {55,-290,-161,1698,-556,-5844,6282,31572,31572,6282,-5844,-556,1698,-161,-290,55}
#define G4 {28,-210,-186,1577,-456,-5769,6154,31620,31620,6154,-5769,-456,1577,-186,-210,28}

// Delays for aligning signals during synthesis
// (cf. MATLAB: flip(loDelay) and flip(hiDelay) in synthesis.m)
#define DELAY1_LO 0		// Delay to add to lo-freq input of 1st filter stage (4th merge)
#define DELAY1_HI 16	// Delay to add to hi-freq input of 1st filter stage (4th merge)
#define DELAY2_LO 0		// Delay to add to lo-freq input of 2nd filter stage (3rd merge)
#define DELAY2_HI 15	// Delay to add to hi-freq input of 2nd filter stage (3rd merge)
#define DELAY3_LO 15	// Delay to add to lo-freq input of 3rd filter stage (2nd merge)
#define DELAY3_HI 15	// Delay to add to hi-freq input of 3rd filter stage (2nd merge)
#define DELAY4_LO 0		// Delay to add to lo-freq input of 4th filter stage (1st merge)
#define DELAY4_HI 0		// Delay to add to hi-freq input of 4th filter stage (1st merge)


/********************************************
* Quantisation en dequantisation parameters *
*********************************************/

// Linear prediction coefficients
#define MU {0,-16,-5,-6,2} // factor 32 too large, original: [0,-0.5,-0.15,-0.2,0.06]
extern signed const short mu[NSUBBANDS];

// Step size adaptation coefficients
#define PHI {117,160,144,107,0} // factor 32 too large, original: [3.65,5,4.5,3.35,0]
extern unsigned const short phi[NSUBBANDS];

// Number of quantization bits per subband
#define NUMQUANBITS {5,5,4,3,0}
extern unsigned const short numQuanBits[NSUBBANDS];

// Number of subband samples to process per frame
extern const unsigned short numSamplesSubband[NSUBBANDS];

// the number of bytes in the encoded signal: 15 L channel, 15 R channel
#define NBYTESENCODEDSIGNAL 30

// Upper and lower limits for encoded signal
#define MAXENCODEDSIG {15,15,7,3,0} // 2^(NUMQUANBITS-1) -1
#define MINENCODEDSIG {-16,-16,-8,-4,0} // - 2^(NUMQUANBITS-1)
extern signed const short minEncodedSig[NSUBBANDS];
extern signed const short maxEncodedSig[NSUBBANDS];

// Initial step size values
#define DEFAULTSTEPSIZE {2185,2185,4681,10923,1}
extern unsigned const short defaultStepSize[NSUBBANDS];

// Minimally allowed signal power to prevent step size from becoming too low
#define MINPOWER 60

// Forgetting factor for exponential weighting of signal power
#define ALFA 819

#endif	/* CODEC_H */
