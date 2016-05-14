#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "codec.h"
#include "encoder.h"
#include "utils.h"

/*
 * Initialize chunk variable
 */
void initEncChunk(struct encChunk *chunk)
{
	// loop variable
	int k;

	// Set previous samples to zero (zero padding)
	memset(chunk->filtFork1,0,sizeof(chunk->filtFork1));
	memset(chunk->filtFork2,0,sizeof(chunk->filtFork2));
	memset(chunk->filtFork3,0,sizeof(chunk->filtFork3));
	memset(chunk->filtFork4,0,sizeof(chunk->filtFork4));

	// set prediction to zero
	memset(chunk->prediction,0,sizeof(chunk->prediction));

	// set power to minimum power
	// set stepsize
	for(k=0; k<NSUBBANDS; k++)
	{
		chunk->power[k] = MINPOWER;
		chunk->stepSize[k] = 1 << (15-numQuanBits[k]+1);
	}
}

/************** ANALYSIS **************/

/*
 * Perform analysis into all subbands for signal in 1 buffer.
 */
void analysis(short buffer[BUFFERSIZE],
		struct encChunk *leftChunk, struct encChunk *rightChunk)
{
	const int L = BUFFERSIZE/2;

	// Perform analysis filtering for all subbands in left channel and right channel
	analFilt(h1,LENGTH1,buffer,L,leftChunk->filtFork1,rightChunk->filtFork1);
	analFilt(h2,LENGTH2,buffer,L/2,leftChunk->filtFork2,rightChunk->filtFork2);
	analFilt(h3,LENGTH3,buffer+L,L/2,leftChunk->filtFork3,rightChunk->filtFork3);
	analFilt(h4,LENGTH4,buffer,L/4,leftChunk->filtFork4,rightChunk->filtFork4);
}

/* Perform single analysis filtering operation on 1 (sub)band signal.
 * The low-pass signal is stored in the first half of the input signal array.
 * The high-pass signal is stored in the second half of the input signal array.
 */
void analFilt(const short* restrict h, unsigned char filterLength, short* restrict signal,
		unsigned char signalLength, short* restrict chunkL, short* restrict chunkR)
{
	int n,k;	// Loop counters
	int evenTermL, oddTermL, evenTermR, oddTermR;

	/*
	unsigned h3_h2, h1_h0;					// packed 16-bit values
	unsigned chunk3_chunk2, chunk1_chunk0;	// packed 16-bit values
	double prod3_prod2, prod1_prod0;		// packed 16-bit values
	*/

	// Put the input signal samples in the chunk variable
	//#pragma MUST_ITERATE(10,40,10);
	for (n=0; n<signalLength; n++){
		chunkL[filterLength-1+n] = *(signal+2*n);
		chunkR[filterLength-1+n] = *(signal+2*n+1);
	}

	// Perform convolution on the chunk variable and store the result in the input array
	//#pragma MUST_ITERATE(5,20,5);
	for (n=0; n<signalLength; n+=2){
		evenTermL = oddTermL = evenTermR = oddTermR = 0;
		//_nassert((int) h % 8 == 0);			// h is aligned to 8-byte boundary
		//#pragma MUST_ITERATE(4,16,4);
		//#pragma UNROLL(2);					// balances the use of resources
		for (k=0; k<filterLength; k+=4){
			evenTermL += h[k+3] * chunkL[filterLength-1+n-k];
			oddTermL  += h[k+2] * chunkL[filterLength-2+n-k];
			evenTermL += h[k+1] * chunkL[filterLength-3+n-k];
			oddTermL  += h[k] * chunkL[filterLength-4+n-k];

			evenTermR += h[k+3] * chunkR[filterLength-1+n-k];
			oddTermR  += h[k+2] * chunkR[filterLength-2+n-k];
			evenTermR += h[k+1] * chunkR[filterLength-3+n-k];
			oddTermR  += h[k] * chunkR[filterLength-4+n-k];

			/*
			h3_h2 = _hi(_amemd8_const(&h[k]));
			h1_h0 = _lo(_amemd8_const(&h[k]));

			// Left channel
			chunk3_chunk2 = _hi(_memd8(&chunkL[filterLength-4+n-k]));
			chunk1_chunk0 = _lo(_memd8(&chunkL[filterLength-4+n-k]));
			prod3_prod2 = _mpy2(h3_h2,chunk3_chunk2);
			prod1_prod0 = _mpy2(h1_h0,chunk1_chunk0);
			evenTermL += _hi(prod3_prod2) + _hi(prod1_prod0);
			oddTermL += _lo(prod3_prod2) + _lo(prod1_prod0);

			// Right channel
			chunk3_chunk2 = _hi(_memd8(&chunkR[filterLength-4+n-k]));
			chunk1_chunk0 = _lo(_memd8(&chunkR[filterLength-4+n-k]));
			prod3_prod2 = _mpy2(h3_h2,chunk3_chunk2);
			prod1_prod0 = _mpy2(h1_h0,chunk1_chunk0);
			evenTermR += _hi(prod3_prod2) + _hi(prod1_prod0);
			oddTermR += _lo(prod3_prod2) + _lo(prod1_prod0);
			*/
		}
		*(signal+n) = intToShort(evenTermL+oddTermL);
		*(signal+n+1) = intToShort(evenTermR+oddTermR);
		*(signal+n+signalLength) = intToShort(evenTermL-oddTermL);
		*(signal+n+signalLength+1) = intToShort(evenTermR-oddTermR);
	}

	// Shift the chunk to the left
	//#pragma MUST_ITERATE(15,63);
	for (n=0; n<filterLength-1; n++){
		chunkL[n] = chunkL[n+signalLength];
		chunkR[n] = chunkR[n+signalLength];
	}
}

/*
 * For debugging
 */
void writeBufferToTextFile(short buffer[BUFFERSIZE], char* fileName)
{
    FILE *fp = fopen(fileName, "a");
    int k;
    for (k=0; k<BUFFERSIZE; k++){
    	fprintf(fp,"%hd ",buffer[k]);
    }
    fprintf(fp,"\n");
    fclose(fp);
}


/************** QUANTIZATION **************/

/* 	quantises a buffer of subband signals
 *	INPUTS:
 *		buffer[BUFFERSIZE]: the input signal, split in subbands
 *		quantChunkL: the quantisation chunk for the left channel
 *		quantChunkR: the quantisation chunk for the right channel
 *
 *	OUTPUTS:
 *		encodedSigBuffer:	the encoded quantised signal with following format:
 *							[15 bytes L, 15 bytes R], each of the channels using following format:
 *							25 bits sub1, 25 bits sub2, 40 bits sub3, 30 bits sub4, 0 bits sub5
 */
void quantise (short* restrict buffer, struct encChunk* restrict quantChunkL,
		struct encChunk* restrict quantChunkR, unsigned char* restrict encodedSigTotal){

	// needles to keep track of where in the encoded total signal writing stopped
	int currentByteNeedle = 0;
	int currentBitNeedle = 0;

	// needle to keep track of where in the buffer reading stopped
	int readBufferNeedleLeft = 0;
	int readBufferNeedleRight = 0;

	// inputs for quantiseSubband
	char encodedSigSub[10];

	// counters & limits
	int numSubband;

	// reset total encoded signal of all subbands, L & R
	memset(encodedSigTotal,0,NBYTESENCODEDSIGNAL);

	// ------------
	// LEFT CHANNEL
	// ------------

	// loop over all subbands but the last subband (zero quantisation bits)
	// #pragma UNROLL(4);
	for(numSubband=0; numSubband<NSUBBANDS-1; numSubband++)
	{
		// call quantisesubband, quantChunk updating also happens here
		quantiseSubband (numSubband,buffer,readBufferNeedleLeft,
						(quantChunkL->stepSize[numSubband]), (quantChunkL->prediction[numSubband]), (quantChunkL->power[numSubband]),
						encodedSigSub,
						&(quantChunkL->stepSize[numSubband]), &(quantChunkL->prediction[numSubband]), &(quantChunkL->power[numSubband]), 0);

		// compress encoded signal
		compressEncodedSignal (numSubband, encodedSigSub,
				&currentBitNeedle, encodedSigTotal,
				&currentByteNeedle);

		readBufferNeedleLeft += numSamplesSubband[numSubband];
	}

	// -------------
	// RIGHT CHANNEL
	// -------------

	// loop over all subbands but the last subband (zero quantisation bits)
	// #pragma UNROLL(4);
	for(numSubband=0; numSubband<NSUBBANDS-1; numSubband++)
	{
		// call quantisesubband, quantChunk updating also happens here
		quantiseSubband (numSubband,buffer,readBufferNeedleRight,
						(quantChunkR->stepSize[numSubband]), (quantChunkR->prediction[numSubband]), (quantChunkR->power[numSubband]),
						encodedSigSub,
						&(quantChunkR->stepSize[numSubband]), &(quantChunkR->prediction[numSubband]), &(quantChunkR->power[numSubband]), 1);

		// compress encoded signal
		compressEncodedSignal (numSubband, encodedSigSub,
				&currentBitNeedle, encodedSigTotal,
				&currentByteNeedle);

		readBufferNeedleRight += numSamplesSubband[numSubband];
	}
}

/* 	compresses the encoded signal of a subband
 * 	INPUTS:
 * 		numSubband : the index of the subband counting from the lowest frequency to the highest
 *		encodedSigSub : an array containing the encoded, uncompressed subband signal
 *						all encoded samples use one char, but multiple bits are unused
 *		currentByteNeedle : indicates at what byte of the total encoded and compressed signal needs to be written
 *		currentBitNeedle : indicates a what bit in the current byte (currentByteNeedle) needs to be written
 *	OUTPUTS
 *		currentByteNeedle : is updated during the execution of this method
 *		currentBitNeedle : is updated during the execution of this method
 *		encodedSigTotal : 	is updated during the execution of this method
 *							this array contains the total compressed and encoded signal of a whole buffer
 */
void compressEncodedSignal (int numSubband, char* restrict encodedSigSub,
		int* restrict currentBitNeedle, unsigned char* restrict encodedSigTotal,
		int* restrict currentByteNeedle) {

	signed char sampleToHandle;
	unsigned char uSampleToHandle;
	signed char excessBits;

	// counters & limits
	int k;

	// reorganisation of encodedSignal
	// loop over samples in encoded signal of subband
	// #pragma MUST_ITERATE(5,10,5);
	for(k=0; k<numSamplesSubband[numSubband]; k++)
	{
		sampleToHandle = encodedSigSub[k];

		// shift left to remove all bits that are not part of sample e.g. 5 bit sample does not need 3 MSB bits of a byte
		sampleToHandle = sampleToHandle << (8-numQuanBits[numSubband]);

		// convert to unsigned so that leading zeros will appear instead of ones in case of negative number
		// e.g. signed 		1001 >> 2 => 1110
		// 		unsigned 	1001 >> 2 => 0010
		uSampleToHandle = (unsigned char) sampleToHandle;

		// shift to the right until currentBitNeedle
		uSampleToHandle = uSampleToHandle >> *currentBitNeedle;

		// if there has been more shifting to the right then to the left, bits are lost and they need to be put in the next byte
		excessBits = *currentBitNeedle - (8-numQuanBits[numSubband]);

		// first store the first part of the result in the correct byte by doing an OR operation
		encodedSigTotal[*currentByteNeedle] = encodedSigTotal[*currentByteNeedle] | uSampleToHandle;

		// then increase the bit needle
		*currentBitNeedle += numQuanBits[numSubband];

		// handle excess bits
		if(excessBits > 0)
		{
			// set needles to next byte
			*currentByteNeedle = *currentByteNeedle +1;
			*currentBitNeedle = 0;

			// retake editing of the sample from the start
			sampleToHandle = encodedSigSub[k];
			uSampleToHandle = (unsigned char) sampleToHandle;

			// shift until only excess bits are left
			uSampleToHandle = uSampleToHandle << (8-excessBits);

			// add to the current byte using an OR operation
			encodedSigTotal[*currentByteNeedle] = encodedSigTotal[*currentByteNeedle] | uSampleToHandle;

			// increase needle
			*currentBitNeedle += excessBits;
		}
	}
}


/* 	quantises a subband signal
 * 	INPUTS:
 * 		numSubband : the index of the subband counting from the lowest frequency to the highest
 * 		subbandSig : a segment of the subband signal (10 samples)
 * 		prevStepSize : the last stepSize calculated in the quantising of the previous time segment of this subband signal
 * 		prevPrediction : the last prediction calculated in the previous segment
 *		prevPower : the last power calculated in the previous segment
 *	OUTPUTS:
 *		encodedSigSub : the encoded subband signal
 *		nextStepSize : the last calculated step size, to be used in the next segment of this subband signal
 *		nextPrediction : the last calculated prediction
 *		nextPower : the last calculated power
 */
void quantiseSubband (int numSubband, short* restrict buffer, int readBufferNeedle, unsigned short prevStepSize,
		short prevPrediction, unsigned short prevPower, char* restrict encodedSigSub,
		unsigned short* restrict nextStepSize, short* restrict nextPrediction, unsigned short* restrict nextPower, int offset){
	// initialiseren
	int diffSig;
	int prediction = prevPrediction;
	int stepSize = prevStepSize;
	int power = prevPower;
	int diffSigEst;
	int subSigEst;
	int encodedSigTemp;

	// the number of samples that need to be processed (loop boundary)
	unsigned char numSamples = numSamplesSubband[numSubband];

	// max en min value of encodedSignal
	short maxEnc = maxEncodedSig[numSubband];
	short minEnc = minEncodedSig[numSubband];

	// counter
	int i;
	for(i=0; i<numSamples; i++)
	{
		diffSig = (int) buffer[2*(readBufferNeedle+i)+offset] - (int) prediction;

		// + sign(diffSig)*stepSize/2 to do rounding
		encodedSigTemp = (diffSig+SIGN(diffSig)*stepSize/2)/stepSize;

		// clip if needed
		if(encodedSigTemp > maxEnc)
		{
			encodedSigTemp = maxEnc;
		}
		else if(encodedSigTemp < minEnc)
		{
			encodedSigTemp = minEnc;
		}

		// assign to output
		encodedSigSub[i] = (char) encodedSigTemp;

		diffSigEst = encodedSigTemp*stepSize;
		diffSigEst = CLAMPINT(diffSigEst);

		// calculate power, +512 is to do rounding instead of truncating
		// no danger for overflow if previous power and diffSigEst are below short limit
		// shift by 10 is division by 1024
		power = ALFA*power + (1024-ALFA)*abs(diffSigEst);
		power = DIVBYPOW2(power,10);

		// set power to minPower if too low
		power = (power < MINPOWER) ? MINPOWER : power;

		stepSize = phi[numSubband]*power;
		stepSize = DIVBYPOW2(stepSize,numQuanBits[numSubband] + 4);

		if(stepSize == 0)
		{
			stepSize = 1;
		}

		subSigEst = prediction + diffSigEst;

		// shift by 5 is division by 32
		prediction = subSigEst*mu[numSubband];
		prediction = DIVBYPOW2(prediction,5);

	}

	// assign outputs
	*nextPrediction = (short) CLAMPINT(prediction);
	*nextStepSize = (stepSize > 32767) ? 32767 : (unsigned short) stepSize;
	*nextPower = (power > 32767) ? 32767 : (unsigned short) power;
}

