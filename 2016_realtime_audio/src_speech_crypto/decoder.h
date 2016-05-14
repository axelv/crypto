#ifndef DECODER_H
#define DECODER_H

#include "globals.h"
#include "codec.h"
#include "utils.h"

/*
 * Chunk variable.
 * The lo and hi fields enables the convolution operation for the synthesis filtering
 * to access samples from the previous buffer contents.
 * The power, stepSize, and prediction fields provide continuity of the encoder settings
 * throughout the frames.
 */
struct decChunk{
	short lo1[LENGTH1/2-1+BUFFERSIZE/4+DELAY1_LO];
	short hi1[LENGTH1/2-1+BUFFERSIZE/4+DELAY1_HI];
	short lo2[LENGTH2/2-1+BUFFERSIZE/8+DELAY2_LO];
	short hi2[LENGTH2/2-1+BUFFERSIZE/8+DELAY2_HI];
	short lo3[LENGTH3/2-1+BUFFERSIZE/8+DELAY3_LO];
	short hi3[LENGTH3/2-1+BUFFERSIZE/8+DELAY3_HI];
	short lo4[LENGTH4/2-1+BUFFERSIZE/16+DELAY4_LO];
	short hi4[LENGTH4/2-1+BUFFERSIZE/16+DELAY4_HI];

	unsigned short power[NSUBBANDS];
	unsigned short stepSize[NSUBBANDS];
	short prediction[NSUBBANDS];
};

/*
 * Initialize chunk variable
 */
void initDecChunk(struct decChunk *chunk);


/************** SYNTHESIS **************/

/*
 * Perform synthesis for all subbands in the leftSub and rightSub buffers,
 * and put the result in buffer.
 */
void synthesis(short buffer[BUFFERSIZE], struct decChunk *leftChunk, struct decChunk *rightChunk);

/*
 * Perform single synthesis operation, i.e., merge two subbands signals. The 'signal'
 * input is a pointer to the first element of the cascaded subband signal array. The
 * 'signalLength' input equals the number of samples in  each of those subbands signals.
 * The results are stored in signal[2*n], n=0...2*signalLength-1.
 */
void synthFilt(const short* restrict g, unsigned char filterLength, short* restrict signal,
		unsigned char signalLength, unsigned char loDelay, unsigned char hiDelay,
		short* restrict loChunkL, short* restrict hiChunkL, short* restrict loChunkR, short* restrict hiChunkR);


/************** DEQUANTIZATION **************/

void dequantise (unsigned char* restrict encodedSigBuffer,
		struct decChunk* restrict dequantChunkL, struct decChunk* restrict dequantChunkR,
		short* restrict buffer);

void decompressEncodedSignal(int numSubband, int* restrict currentByteNeedle,
		int* restrict bitsLeft, unsigned char encodedSigBuffer[NBYTESENCODEDSIGNAL],
		char deCompressedEncodedSig[10]);


void dequantiseSubband (int numSubband, char* restrict encodedSubbandSig,
		unsigned short prevStepSize, short prevPrediction,unsigned short prevPower,
		short* restrict buffer, unsigned short* restrict nextStepSize, short* restrict nextPrediction,
		unsigned short* restrict nextPower, int writeBufferNeedle, int offset);

#endif /* DECODER_H */
