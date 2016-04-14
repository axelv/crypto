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


/************** SYNTHESIS **************/

/*
 * Perform synthesis for all subbands in the leftSub and rightSub buffers,
 * and put the result in buffer.
 */
void synthesis(short buffer[BUFFERSIZE], struct decChunk *leftChunk, struct decChunk *rightChunk)
{
	// Perform synthesis for all subbands of left channel
	synthFilt(g4,LENGTH4,buffer,BUFFERSIZE/16,
			DELAY4_LO,DELAY4_HI,leftChunk->lo4,leftChunk->hi4);
	synthFilt(g3,LENGTH3,buffer+BUFFERSIZE/2,BUFFERSIZE/8,
			DELAY3_LO,DELAY3_HI,leftChunk->lo3,leftChunk->hi3);
	synthFilt(g2,LENGTH2,buffer,BUFFERSIZE/8,
			DELAY2_LO,DELAY2_HI,leftChunk->lo2,leftChunk->hi2);
	synthFilt(g1,LENGTH1,buffer,BUFFERSIZE/4,
			DELAY1_LO,DELAY1_HI,leftChunk->lo1,leftChunk->hi1);

	// Perform synthesis for all subbands of left channel
	synthFilt(g4,LENGTH4,buffer+1,BUFFERSIZE/16,
			DELAY4_LO,DELAY4_HI,rightChunk->lo4,rightChunk->hi4);
	synthFilt(g3,LENGTH3,buffer+1+BUFFERSIZE/2,BUFFERSIZE/8,
			DELAY3_LO,DELAY3_HI,rightChunk->lo3,rightChunk->hi3);
	synthFilt(g2,LENGTH2,buffer+1,BUFFERSIZE/8,
			DELAY2_LO,DELAY2_HI,rightChunk->lo2,rightChunk->hi2);
	synthFilt(g1,LENGTH1,buffer+1,BUFFERSIZE/4,
			DELAY1_LO,DELAY1_HI,rightChunk->lo1,rightChunk->hi1);
}


/*
 * Perform single synthesis operation, i.e., merge two subbands signals. The 'signal'
 * input is a pointer to the first element of the cascaded subband signal array. The
 * 'signalLength' input equals the number of samples in each of those subbands signals.
 * The results are stored in signal[2*n], n=0...2*signalLength-1.
 */
void synthFilt(const short* g, unsigned char filterLength, short* signal, unsigned char signalLength,
		unsigned char loDelay, unsigned char hiDelay, short* loChunk, short* hiChunk)
{
	unsigned char k,n;					// Loop counters
	short* lo = signal;					// Low-frequency subband signal
	short* hi = signal+2*signalLength;	// High-frequency subband signal

	// Put the input samples in the chunk variable
	for (n=0; n<signalLength; n++){
		loChunk[n+filterLength/2-1+loDelay] = *(lo+2*n);
		hiChunk[n+filterLength/2-1+hiDelay] = *(hi+2*n);
	}

	// Perform convolution on the chunk variable and store the result in buffer
	for (n=0; n<signalLength; n++){
		long evenTerm=0;
		long oddTerm=0;
		for (k=0; k<filterLength/2; k++){
			evenTerm += g[2*k] *
					(loChunk[n-k+filterLength/2-1] - hiChunk[n-k+filterLength/2-1]);
			oddTerm += g[2*k+1] *
					(loChunk[n-k+filterLength/2-1] + hiChunk[n-k+filterLength/2-1]);
		}
		*(lo+4*n) = longToShortWithClamping(evenTerm);
		*(lo+4*n+2) = longToShortWithClamping(oddTerm);
	}

	// Shift chunks to the left
	for (n=0; n<filterLength/2-1+loDelay; n++){
		loChunk[n] = loChunk[n+signalLength];
	}
	for (n=0; n<filterLength/2-1+hiDelay; n++){
		hiChunk[n] = hiChunk[n+signalLength];
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
void dequantise (unsigned char encodedSigBuffer[NBYTESENCODEDSIGNAL],
		struct decChunk *dequantChunkL, struct decChunk *dequantChunkR,
		short buffer[BUFFERSIZE]){

	// decompress bitstream into an array where every sample is a byte
	unsigned char currentByteNeedle = 0;
	signed char bitsLeft = 8;
	char deCompressedEncodedSig[10];
	short decodedSigSub[10];
	unsigned char numSubband;
	unsigned char k;
	unsigned char writeBufferNeedle = 0;

	// ------------
	// LEFT CHANNEL
	// ------------

	// loop over subbands
	for(numSubband=0; numSubband<NSUBBANDS; numSubband++)
	{
		// if no quantisation bits are assigned to subband, no encoded signal is present
		if(numQuanBits[numSubband] > 0)
		{
			// decompress the encoded squized signal
			decompressEncodedSignal(numSubband, &currentByteNeedle,
					&bitsLeft, encodedSigBuffer, deCompressedEncodedSig);

			// call quantisesubband, quantChunk updating also happens here
			dequantiseSubband (numSubband,deCompressedEncodedSig,
							(dequantChunkL->stepSize[numSubband]), (dequantChunkL->prediction[numSubband]), (dequantChunkL->power[numSubband]),
							decodedSigSub,
							&(dequantChunkL->stepSize[numSubband]), &(dequantChunkL->prediction[numSubband]), &(dequantChunkL->power[numSubband]));

			// copy result to output
			for(k=0; k<numSamplesSubband[numSubband];k++)
			{
				buffer[2*(k+writeBufferNeedle)] = decodedSigSub[k];
			}
			writeBufferNeedle += numSamplesSubband[numSubband];

		}
		else // quanBits = 0
		{
			// set output to zero
			for(k=0; k<numSamplesSubband[numSubband];k++)
			{
				buffer[2*(k+writeBufferNeedle)] = 0;
			}
			writeBufferNeedle += numSamplesSubband[numSubband];
		}
	}

	// -------------
	// RIGHT CHANNEL
	// -------------

	writeBufferNeedle = 0;

	// loop over subbands
	for(numSubband=0; numSubband<NSUBBANDS; numSubband++)
	{
		// if no quantisation bits are assigned to subband, no encoded signal is present
		if(numQuanBits[numSubband] > 0)
		{
			// decompress the encoded squized signal
			decompressEncodedSignal(numSubband,	&currentByteNeedle,
					&bitsLeft, encodedSigBuffer, deCompressedEncodedSig);

			// call quantisesubband, quantChunk updating also happens here
			dequantiseSubband (numSubband,deCompressedEncodedSig,
								(dequantChunkR->stepSize[numSubband]), (dequantChunkR->prediction[numSubband]), (dequantChunkR->power[numSubband]),
								decodedSigSub,
								&(dequantChunkR->stepSize[numSubband]), &(dequantChunkR->prediction[numSubband]), &(dequantChunkR->power[numSubband]));

			// copy result to output
			for(k=0; k<numSamplesSubband[numSubband];k++)
			{
				buffer[2*(k+writeBufferNeedle)+1] = decodedSigSub[k];
			}
			writeBufferNeedle += numSamplesSubband[numSubband];

		}
		else // quanBits = 0
		{
			// set output to zero
			for(k=0; k<numSamplesSubband[numSubband];k++)
			{
				buffer[2*(k+writeBufferNeedle)+1] = 0;
			}
			writeBufferNeedle += numSamplesSubband[numSubband];
		}
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
void decompressEncodedSignal(unsigned char numSubband, unsigned char *currentByteNeedle,
		signed char *bitsLeft, unsigned char encodedSigBuffer[NBYTESENCODEDSIGNAL],
		char deCompressedEncodedSig[10]) {

	signed char totalSamplesCounter = -1;
	signed char byteToHandle;
	signed char sampleValue;
	signed char shortage;
	signed char bitsRemoved;
	unsigned char k;

	// reset deCompressedEncodedSig
	for(k=0; k<10; k++)
	{
		deCompressedEncodedSig[k] = 0;
	}

	// loop over samples in subband
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
		}

		if(shortage > 0)
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
void dequantiseSubband (unsigned char numSubband, char encodedSubbandSig[10],
		unsigned short prevStepSize, short prevPrediction,unsigned short prevPower,
		short subbandSig[10], unsigned short *nextStepSize, short *nextPrediction,
		unsigned short *nextPower){

	// initialiseren
	int prediction = prevPrediction;
	int stepSize = prevStepSize;
	int power = prevPower;
	int diffSigEst;
	int subSigEst;

	// the number of samples that need to be processed (loop boundary)
	unsigned char numSamples = numSamplesSubband[numSubband];

	// counter
	unsigned char i;
	for(i=0; i<numSamples; i++)
	{
		diffSigEst = encodedSubbandSig[i]*stepSize;

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

		// clip subSigEst
		if(subSigEst > SHORTMAX)
		{
			subSigEst = SHORTMAX;
		}
		else if(subSigEst < SHORTMIN)
		{
			subSigEst = SHORTMIN;
		}

		// copy to output
		subbandSig[i] = subSigEst;
	}

	// assign outputs
	*nextPrediction = (short) limitIntToShort(prediction);
	*nextStepSize = (unsigned short) limitIntToShort(stepSize);
	*nextPower = (unsigned short) limitIntToShort(power);
}

