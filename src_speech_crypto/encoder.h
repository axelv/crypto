#ifndef ENCODER_H
#define ENCODER_H

#include "globals.h"
#include "codec.h"

/*
 * Chunk variable.
 * The filtfork fields enable the convolution operation for the analysis filtering
 * to access samples from the previous buffer contents.
 * The power, stepSize, and prediction fields provide continuity of the encoder settings
 * throughout the frames.
 */
struct encChunk{
	short filtFork1[LENGTH1-1+BUFFERSIZE/2];
	short filtFork2[LENGTH2-1+BUFFERSIZE/4];
	short filtFork3[LENGTH3-1+BUFFERSIZE/4];
	short filtFork4[LENGTH4-1+BUFFERSIZE/8];
	unsigned short power[NSUBBANDS];
	unsigned short stepSize[NSUBBANDS];
	short prediction[NSUBBANDS];
};

/*
 * Initialize chunk variable
 */
void initEncChunk(struct encChunk *chunk);


/************** ANALYSIS **************/

/*
 * Perform analysis into all subbands for signal in 1 buffer.
 */
void analysis(short buffer[BUFFERSIZE],
		struct  encChunk *leftChunk, struct encChunk *rightChunk);

/* Perform single analysis filtering operation on 1 (sub)band signal.
 * The low-pass signal is stored in the first half of the input signal array.
 * The high-pass signal is stored in the second half of the input signal array.
 */
void analFilt(const short* restrict h, unsigned char filterLength, short* restrict signal,
		unsigned char signalLength, short* restrict chunkL, short* restrict chunkR);

/*
 * For debugging
 */
void writeBufferToTextFile(short buffer[BUFFERSIZE], char* fileName);


/************** QUANTIZATION **************/

void quantise (short* restrict buffer, struct encChunk* restrict quantChunkL,
		struct encChunk* restrict quantChunkR, unsigned char* restrict encodedSigTotal);

void quantiseSubband (int numSubband, short* restrict buffer, int readBufferNeedle, unsigned short prevStepSize,
		short prevPrediction, unsigned short prevPower, char* restrict encodedSigSub,
		unsigned short* restrict nextStepSize, short* restrict nextPrediction, unsigned short* restrict nextPower, int offset);

void compressEncodedSignal (int numSubband, char* restrict encodedSigSub,
		int* restrict currentBitNeedle, unsigned char* restrict encodedSigTotal,
		int* restrict currentByteNeedle);

#endif /* ENCODER_H */
