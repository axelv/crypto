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
	// Set previous samples to zero (zero padding)
	memset(chunk->filtFork1,0,sizeof(chunk->filtFork1));
	memset(chunk->filtFork2,0,sizeof(chunk->filtFork2));
	memset(chunk->filtFork3,0,sizeof(chunk->filtFork3));
	memset(chunk->filtFork4,0,sizeof(chunk->filtFork4));

	// set prediction to zero
	memset(chunk->prediction,0,sizeof(chunk->prediction));

	// set power to minimum power
	for(unsigned char k=0; k<NSUBBANDS; k++)
	{
		chunk->power[k] = MINPOWER;
	}

	// set stepsize
	for(unsigned char k=0; k<NSUBBANDS; k++)
	{
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
	unsigned char L = BUFFERSIZE/2;

	// Perform analysis filtering for all subbands in left channel
	analFilt(h1,LENGTH1,buffer,L,leftChunk->filtFork1);
	analFilt(h2,LENGTH2,buffer,L/2,leftChunk->filtFork2);
	analFilt(h3,LENGTH3,buffer+L,L/2,leftChunk->filtFork3);
	analFilt(h4,LENGTH4,buffer,L/4,leftChunk->filtFork4);

	// Perform analysis filtering for all subbands in right channel
	analFilt(h1,LENGTH1,buffer+1,L,rightChunk->filtFork1);
	analFilt(h2,LENGTH2,buffer+1,L/2,rightChunk->filtFork2);
	analFilt(h3,LENGTH3,buffer+1+L,L/2,rightChunk->filtFork3);
	analFilt(h4,LENGTH4,buffer+1,L/4,rightChunk->filtFork4);
}

/* Perform single analysis filtering operation on 1 (sub)band signal.
 * The low-pass signal is stored in the first half of the input signal array.
 * The high-pass signal is stored in the second half of the input signal array.
 */
void analFilt(const short* h, unsigned char filterLength,
		short* signal, unsigned char signalLength, short* chunk)
{
	unsigned char n,k;	// Loop counters

	// Put the input signal samples in the chunk variable
	for (n=0; n<signalLength; n++){
		chunk[filterLength-1+n] = *(signal+2*n);
	}

	// Perform convolution on the chunk variable and store the result in the input array
	for (n=0; n<signalLength; n+=2){
		int evenTerm = 0;
		int oddTerm = 0;
		for (k=0; k<filterLength; k+=2){
			evenTerm += h[k] * chunk[filterLength-1+n-k];
			oddTerm  += h[k+1] * chunk[filterLength-2+n-k];
		}
		*(signal+n) = intToShort(evenTerm+oddTerm);
		*(signal+n+signalLength) = intToShort(evenTerm-oddTerm);
	}

	// Shift the chunk to the left
	for (n=0; n<filterLength-1; n++){
		chunk[n] = chunk[n+signalLength];
	}

}

/*
 * For debugging
 */
void writeBufferToTextFile(short buffer[BUFFERSIZE], char* fileName)
{
    FILE *fp = fopen(fileName, "a");
    for (unsigned char k=0; k<BUFFERSIZE; k++){
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
void quantise (short buffer[BUFFERSIZE], struct encChunk *quantChunkL,
		struct encChunk *quantChunkR, unsigned char encodedSigTotal[NBYTESENCODEDSIGNAL]){

	// needles to keep track of where in the encoded total signal writing stopped
	unsigned char currentByteNeedle = 0;
	unsigned char currentBitNeedle = 0;

	// needle to keep track of where in the buffer reading stopped
	unsigned char readBufferNeedle = 0;

	// inputs for quantiseSubband
	short subbandSig[10];
	char encodedSigSub[10];

	// counters & limits
	unsigned char numSubband;
	unsigned char k;

	// total encoded signal of all subbands, L & R
	for(k=0; k<NBYTESENCODEDSIGNAL; k++)
	{
		encodedSigTotal[k] = 0;
	}

	// ------------
	// LEFT CHANNEL
	// ------------

	// loop over all subbands
	for(numSubband=0; numSubband<NSUBBANDS; numSubband++)
	{
		// copy signal from buffer to subbandSig
		for(k=0; k<numSamplesSubband[numSubband];k++)
		{
			subbandSig[k] = buffer[2*(readBufferNeedle+k)];
		}
		readBufferNeedle += numSamplesSubband[numSubband];

		// check if quantisation is really necessary
		if(numQuanBits[numSubband] > 0)
		{

			// call quantisesubband, quantChunk updating also happens here
			quantiseSubband (numSubband,subbandSig,
							(quantChunkL->stepSize[numSubband]), (quantChunkL->prediction[numSubband]), (quantChunkL->power[numSubband]),
							encodedSigSub,
							&(quantChunkL->stepSize[numSubband]), &(quantChunkL->prediction[numSubband]), &(quantChunkL->power[numSubband]));

			// compress encoded signal
			compressEncodedSignal (numSubband, encodedSigSub,
					&currentBitNeedle, encodedSigTotal,
					&currentByteNeedle);
		}
	}

	// -------------
	// RIGHT CHANNEL
	// -------------

	// reset readNeedle
	readBufferNeedle = 0;

	// loop over all subbands
	for(numSubband=0; numSubband<NSUBBANDS; numSubband++)
	{
		// copy signal from buffer to subbandSig
		for(k=0; k<numSamplesSubband[numSubband];k++)
		{
			subbandSig[k] = buffer[2*(readBufferNeedle+k)+1];
		}
		readBufferNeedle += numSamplesSubband[numSubband];

		// check if quantisation is really necessary
		if(numQuanBits[numSubband] > 0)
		{
			// call quantisesubband, quantChunk updating also happens here
			quantiseSubband (numSubband,subbandSig,
							(quantChunkR->stepSize[numSubband]), (quantChunkR->prediction[numSubband]), (quantChunkR->power[numSubband]),
							encodedSigSub,
							&(quantChunkR->stepSize[numSubband]), &(quantChunkR->prediction[numSubband]), &(quantChunkR->power[numSubband]));

			// compress encoded signal
			compressEncodedSignal (numSubband, encodedSigSub,
					&currentBitNeedle, encodedSigTotal,
					&currentByteNeedle);
		}
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
void compressEncodedSignal (unsigned char numSubband, char encodedSigSub[10],
		unsigned char *currentBitNeedle, unsigned char encodedSigTotal[NBYTESENCODEDSIGNAL],
		unsigned char *currentByteNeedle) {

	signed char sampleToHandle;
	unsigned char uSampleToHandle;
	signed char excessBits;

	// counters & limits
	unsigned char k;

	// reorganisation of encodedSignal
	// loop over samples in encoded signal of subband
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
void quantiseSubband (unsigned char numSubband, short subbandSig[10], unsigned short prevStepSize,
		short prevPrediction,unsigned short prevPower, char encodedSigSub[10],
		unsigned short *nextStepSize, short *nextPrediction, unsigned short *nextPower){
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
	unsigned char i;
	for(i=0; i<numSamples; i++)
	{
		diffSig = (int) subbandSig[i] - (int) prediction;

		// + sign(diffSig)*stepSize/2 to do rounding
		encodedSigTemp = (diffSig+sign(diffSig)*stepSize/2)/stepSize;

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

		if(diffSigEst > SHORTMAX)
		{
			diffSigEst = SHORTMAX;
		}
		else if(diffSigEst < SHORTMIN)
		{
			diffSigEst = SHORTMIN;
		}

		// calculate power, +512 is to do rounding instead of truncating
		// no danger for overflow if previous power and diffSigEst are below short limit
		// shift by 10 is division by 1024
		power = divByPow2(ALFA*power + (1024-ALFA)*abs(diffSigEst),10);

		// set power to minPower if too low
		if(power < MINPOWER)
		{
			power = MINPOWER;
		}

		stepSize = divByPow2(phi[numSubband]*power,(numQuanBits[numSubband] + 4));

		if(stepSize == 0)
		{
			stepSize = 1;
		}

		subSigEst = prediction + diffSigEst;

		// shift by 5 is division by 32
		prediction = divByPow2(subSigEst*mu[numSubband],5);

	}

	// assign outputs
	*nextPrediction = (short) limitIntToShort(prediction);
	*nextStepSize = (unsigned short) limitIntToShort(stepSize);
	*nextPower = (unsigned short) limitIntToShort(power);
}

