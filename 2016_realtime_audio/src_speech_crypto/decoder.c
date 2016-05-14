#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "codec.h"
#include "utils.h"
#include "decoder.h"

/*
 * Initialize chunk variable
 */
void initDecChunk(struct decChunk *chunk){
	// loop variable
	int k =0;

	// Set previous samples to zero (zero padding)
	memset(chunk->lo1,0,sizeof(chunk->lo1));
	memset(chunk->hi1,0,sizeof(chunk->hi1));
	memset(chunk->lo2,0,sizeof(chunk->lo2));
	memset(chunk->hi2,0,sizeof(chunk->hi2));
	memset(chunk->lo3,0,sizeof(chunk->lo3));
	memset(chunk->hi3,0,sizeof(chunk->hi3));
	memset(chunk->lo4,0,sizeof(chunk->lo4));
	memset(chunk->hi4,0,sizeof(chunk->hi4));

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


/************** SYNTHESIS **************/

/*
 * Perform synthesis for all subbands in the leftSub and rightSub buffers,
 * and put the result in buffer.
 */
void synthesis(short buffer[BUFFERSIZE], struct decChunk *leftChunk, struct decChunk *rightChunk)
{
	// Perform synthesis for all subbands of left channel and right channel
	synthFilt(g4,LENGTH4,buffer,BUFFERSIZE/16,
			DELAY4_LO,DELAY4_HI,leftChunk->lo4,leftChunk->hi4,rightChunk->lo4,rightChunk->hi4);
	synthFilt(g3,LENGTH3,buffer+BUFFERSIZE/2,BUFFERSIZE/8,
			DELAY3_LO,DELAY3_HI,leftChunk->lo3,leftChunk->hi3,rightChunk->lo3,rightChunk->hi3);
	synthFilt(g2,LENGTH2,buffer,BUFFERSIZE/8,
			DELAY2_LO,DELAY2_HI,leftChunk->lo2,leftChunk->hi2,rightChunk->lo2,rightChunk->hi2);
	synthFilt(g1,LENGTH1,buffer,BUFFERSIZE/4,
			DELAY1_LO,DELAY1_HI,leftChunk->lo1,leftChunk->hi1,rightChunk->lo1,rightChunk->hi1);
}


/*
 * Perform single synthesis operation, i.e., merge two subbands signals. The 'signal'
 * input is a pointer to the first element of the cascaded subband signal array. The
 * 'signalLength' input equals the number of samples in each of those subbands signals.
 * The results are stored in signal[2*n], n=0...2*signalLength-1.
 */
void synthFilt(const short* restrict g, unsigned char filterLength, short* restrict signal,
		unsigned char signalLength, unsigned char loDelay, unsigned char hiDelay,
		short* restrict loChunkL, short* restrict hiChunkL, short* restrict loChunkR, short* restrict hiChunkR)
{
	int k,n;									// Loop counters
	short* restrict lo = signal;				// Low-frequency subband signal
	short* restrict hi = signal+2*signalLength;	// High-frequency subband signal
	int evenTermL, oddTermL, evenTermR, oddTermR;

	/*
	unsigned g3_g2, g1_g0;		// packed 16-bit values
	unsigned lo1_lo0, hi1_hi0;	// packed 16-bit values
	long long sum_diff;			// packed 16-bit values
	*/

	// Put the input samples in the chunk variable
	// #pragma MUST_ITERATE(5,20,5);
	for (n=0; n<signalLength; n++){
		loChunkL[n+filterLength/2-1+loDelay] = *(lo+2*n);
		loChunkR[n+filterLength/2-1+loDelay] = *(lo+2*n+1);
		hiChunkL[n+filterLength/2-1+hiDelay] = *(hi+2*n);
		hiChunkR[n+filterLength/2-1+hiDelay] = *(hi+2*n+1);
	}

	// Perform convolution on the chunk variable and store the result in buffer
	// #pragma MUST_ITERATE(5,20,5);
	for (n=0; n<signalLength; n++){
		evenTermL = oddTermL = evenTermR = oddTermR = 0;
		// _nassert((int) g % 8 == 0);		// g is aligned to 8-byte boundary
		// #pragma MUST_ITERATE(4,16,4);
		// #pragma UNROLL(2);				// balances the use of resources
		for (k=0; k<filterLength/2; k+=2){
			evenTermL += g[2*k] *
					(loChunkL[n-k+filterLength/2-2] - hiChunkL[n-k+filterLength/2-2]);
			evenTermL += g[2*k+1] *
					(loChunkL[n-k+filterLength/2-1] - hiChunkL[n-k+filterLength/2-1]);
			oddTermL += g[2*k+2] *
					(loChunkL[n-k+filterLength/2-2] + hiChunkL[n-k+filterLength/2-2]);
			oddTermL += g[2*k+3] *
					(loChunkL[n-k+filterLength/2-1] + hiChunkL[n-k+filterLength/2-1]);
			evenTermR += g[2*k] *
					(loChunkR[n-k+filterLength/2-2] - hiChunkR[n-k+filterLength/2-2]);
			evenTermR += g[2*k+1] *
					(loChunkR[n-k+filterLength/2-1] - hiChunkR[n-k+filterLength/2-1]);
			oddTermR += g[2*k+2] *
					(loChunkR[n-k+filterLength/2-2] + hiChunkR[n-k+filterLength/2-2]);
			oddTermR += g[2*k+3] *
					(loChunkR[n-k+filterLength/2-1] + hiChunkR[n-k+filterLength/2-1]);

			/*
			g3_g2 = _hi(_amemd8_const(&g[2*k]));
			g1_g0 = _lo(_amemd8_const(&g[2*k]));

			// Left channel
			lo1_lo0 = _mem4(&loChunkL[n-k+filterLength/2-2]);
			hi1_hi0 = _mem4(&hiChunkL[n-k+filterLength/2-2]);
			sum_diff = _addsub2(lo1_lo0, hi1_hi0);
			evenTermL += _ldotp2(g1_g0, _loll(sum_diff));
			oddTermL += _ldotp2(g3_g2, _hill(sum_diff));

			// Right channel
			lo1_lo0 = _mem4(&loChunkR[n-k+filterLength/2-2]);
			hi1_hi0 = _mem4(&hiChunkR[n-k+filterLength/2-2]);
			sum_diff = _addsub2(lo1_lo0, hi1_hi0);
			evenTermR += _dotp2(g1_g0, _loll(sum_diff));
			oddTermR += _dotp2(g3_g2, _hill(sum_diff));
			*/
		}
		*(lo+4*n) = longToShort(evenTermL);
		*(lo+4*n+1) = longToShort(evenTermR);
		*(lo+4*n+2) = longToShort(oddTermL);
		*(lo+4*n+3) = longToShort(oddTermR);

	}

	// Shift chunks to the left
	// #pragma MUST_ITERATE(7,31);
	for (n=0; n<filterLength/2-1+loDelay; n++){
		loChunkL[n] = loChunkL[n+signalLength];
		loChunkR[n] = loChunkR[n+signalLength];
	}
	// #pragma MUST_ITERATE(7,47);
	for (n=0; n<filterLength/2-1+hiDelay; n++){
		hiChunkL[n] = hiChunkL[n+signalLength];
		hiChunkR[n] = hiChunkR[n+signalLength];
	}
}


/************** DEQUANTIZATION **************/

/* 	quantises a buffer of the split in subbands input signal
 *	INPUTS:
 *		encodedSigBuffer[NBYTESENCODEDSIGNAL]: the encoded signal
 *		dequantChunkL: the dequantisation chunk for the left channel
 *		dequantChunkR: the dequantisation chunk for the right channel
 *
 *	OUTPUTS:
 *		buffer[BUFFERSIZE]:	the dequantised audio signal, still split in subbands
 */
void dequantise (unsigned char* restrict encodedSigBuffer,
		struct decChunk* restrict dequantChunkL, struct decChunk* restrict dequantChunkR,
		short* restrict buffer){

	// decompress bitstream into an array where every sample is a byte
	int currentByteNeedle = 0;
	int bitsLeft = 8;
	char deCompressedEncodedSig[10];
	int numSubband;
	int k;
	int writeBufferNeedle = 0;

	// ------------
	// LEFT CHANNEL
	// ------------

	// loop over subbands
	// #pragma MUST_ITERATE(4,4,4);
	for(numSubband=0; numSubband<NSUBBANDS-1; numSubband++)
	{
		// decompress the encoded squized signal
		decompressEncodedSignal(numSubband, &currentByteNeedle,
				&bitsLeft, encodedSigBuffer, deCompressedEncodedSig);

		// call quantisesubband, quantChunk updating also happens here
		dequantiseSubband (numSubband,deCompressedEncodedSig,
						(dequantChunkL->stepSize[numSubband]), (dequantChunkL->prediction[numSubband]), (dequantChunkL->power[numSubband]),
						buffer,
						&(dequantChunkL->stepSize[numSubband]), &(dequantChunkL->prediction[numSubband]), &(dequantChunkL->power[numSubband]), writeBufferNeedle, 0);

		writeBufferNeedle += numSamplesSubband[numSubband];
	}

	// -------------
	// RIGHT CHANNEL
	// -------------

	writeBufferNeedle = 0;

	// loop over subbands
	// #pragma MUST_ITERATE(4,4,4);
	for(numSubband=0; numSubband<NSUBBANDS-1; numSubband++)
	{
		// decompress the encoded squized signal
		decompressEncodedSignal(numSubband,	&currentByteNeedle,
				&bitsLeft, encodedSigBuffer, deCompressedEncodedSig);

		// call quantisesubband, quantChunk updating also happens here
		dequantiseSubband (numSubband,deCompressedEncodedSig,
							(dequantChunkR->stepSize[numSubband]), (dequantChunkR->prediction[numSubband]), (dequantChunkR->power[numSubband]),
							buffer,
							&(dequantChunkR->stepSize[numSubband]), &(dequantChunkR->prediction[numSubband]), &(dequantChunkR->power[numSubband]), writeBufferNeedle, 1);

		writeBufferNeedle += numSamplesSubband[numSubband];
	}


	/*
	 * 	set output to zero for the last subband for both left and right channel
	 */
	// #pragma MUST_ITERATE(5,10,5);
	for(k=0; k<numSamplesSubband[numSubband];k++)
	{
		buffer[2*(k+writeBufferNeedle)] = 0;
		buffer[2*(k+writeBufferNeedle)+1] = 0;
	}
}

/* 	decompresses the encoded signal of a subband
 * 	INPUTS:
 *		numSubband : the index of the subband counting from the lowest frequency to the highest
 *		encodedSigBuffer : the compressed encoded signal of all subbands
 *		currentByteNeedle : indicates at what byte of the compressed encoded signal needs to be read
 *		bitsLeft : indicates the number of bits left to read in the current byte (currentBytNeedle)
 *	OUTPUTS
 *		currentByteNeedle : is updated during the execution of this method
 *		bitsLeft : is updated during the execution of this method
 *		deCompressedEncodedSig : the decompressed encoded signal. every encoded sample uses one char of memory space
 */
void decompressEncodedSignal(int numSubband, int* restrict currentByteNeedle,
		int* restrict bitsLeft, unsigned char encodedSigBuffer[NBYTESENCODEDSIGNAL],
		char deCompressedEncodedSig[10]) {

	int totalSamplesCounter = -1;
	signed char byteToHandle;
	signed char sampleValue;
	int shortage;
	int bitsRemoved;
	int k;

	// reset deCompressedEncodedSig
	for(k=0; k<10; k++)
	{
		deCompressedEncodedSig[k] = 0;
	}

	// loop over samples in subband
	// #pragma MUST_ITERATE(5,10,5);
	for(k=0; k<numSamplesSubband[numSubband]; k++)
	{
		totalSamplesCounter++;

		byteToHandle = encodedSigBuffer[*currentByteNeedle];

		// shift to the right to remove bits that are not part of this sample
		sampleValue = byteToHandle >> (8-numQuanBits[numSubband]);

		// calculate shortage of bits in the current byte
		shortage = numQuanBits[numSubband] - *bitsLeft;

		// calculate the amount of bits left in the current byte that still needs to be processed
		*bitsLeft -= numQuanBits[numSubband];

		if(shortage <= 0)
		{
			bitsRemoved = numQuanBits[numSubband];
		} else
		{
			// go to next byte, reset bitsLeft
			*bitsLeft = 8;
			*currentByteNeedle = *currentByteNeedle + 1;

			// get next byte
			byteToHandle = encodedSigBuffer[*currentByteNeedle];

			// shift byte, but this time with leading zeros
			byteToHandle = ((unsigned char) byteToHandle) >> (8-shortage);

			// add piece of shifted byte to sample value
			sampleValue += byteToHandle;

			// reset byte to handle
			byteToHandle = encodedSigBuffer[*currentByteNeedle];

			// update bitsLeft
			*bitsLeft -= shortage;

			bitsRemoved = shortage;
		}

		// update the byte with the extracted sample removed
		encodedSigBuffer[*currentByteNeedle] = byteToHandle << bitsRemoved;

		// set sample to decoded
		deCompressedEncodedSig[totalSamplesCounter] = sampleValue;

		if(*bitsLeft == 0)
		{
			// go to next byte, reset bitsLeft
			*bitsLeft = 8;
			*currentByteNeedle = *currentByteNeedle + 1;
		}
	}

}

/*	dequantises an encoded subband signal
 * 	INPUTS:
 * 		numSubband : the index of the subband counting from the lowest frequency to the highest
 * 		encodedSubbandSig : the encoded subband signal
 * 		prevStepSize : the last stepSize calculated in the dequantising of the previous time segment of this subband signal
 * 		prevPrediction : the last prediction calculated in the previous segment
 *		prevPower : the last power calculated in the previous segment
 *	OUTPUTS:
 *		subbandSig : the decoded subband signal
 *		nextStepSize : the last calculated step size, to be used in the next segment of this subband signal
 *		nextPrediction : the last calculated prediction
 *		nextPower : the last calculated power
 */
void dequantiseSubband (int numSubband, char* restrict encodedSubbandSig,
		unsigned short prevStepSize, short prevPrediction,unsigned short prevPower,
		short* restrict buffer, unsigned short* restrict nextStepSize, short* restrict nextPrediction,
		unsigned short* restrict nextPower, int writeBufferNeedle, int offset){

	// initialiseren
	int prediction = prevPrediction;
	int stepSize = prevStepSize;
	int power = prevPower;
	int diffSigEst;
	int subSigEst;

	// the number of samples that need to be processed (loop boundary)
	unsigned char numSamples = numSamplesSubband[numSubband];

	// counter
	int i;
	// #pragma MUST_ITERATE(5,10,5);
	for(i=0; i<numSamples; i++)
	{
		diffSigEst = encodedSubbandSig[i]*stepSize;
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

		// clip subSigEst
		subSigEst = CLAMPINT(subSigEst);

		// copy to output
		buffer[2*(i+writeBufferNeedle)+offset] = subSigEst;
	}

	// assign outputs
	*nextPrediction = (short) CLAMPINT(prediction);
	*nextStepSize = (stepSize > 32767) ? 32767 : (unsigned short) stepSize;
	*nextPower = (power > 32767) ? 32767 : (unsigned short) power;
}

