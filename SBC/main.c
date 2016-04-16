#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wavpcm_io.h"
#include "globals.h"
#include "codec.h"
#include "encoder.h"
#include "decoder.h"

/****************************
 * Define filter parameters *
 ****************************/

// Analysis filters
const short h1[LENGTH1] = H1;
const short h2[LENGTH2] = H2;
const short h3[LENGTH3] = H3;
const short h4[LENGTH4] = H4;

// Synthesis filters
const short g1[LENGTH1] = G1;
const short g2[LENGTH2] = G2;
const short g3[LENGTH3] = G3;
const short g4[LENGTH4] = G4;


/**********************************
 * Define quantization parameters *
 *********************************/

// Linear prediction coefficients
const signed short mu[NSUBBANDS] = MU;

// Step size adaptation coefficients
const unsigned short phi[NSUBBANDS] = PHI;

// Number of quantization bits per subband
const unsigned short numQuanBits[NSUBBANDS] = NUMQUANBITS;

// Number of subband samples to process per frame
const unsigned short numSamplesSubband[NSUBBANDS] =
		{BUFFERSIZE/16,BUFFERSIZE/16,BUFFERSIZE/8,BUFFERSIZE/8,BUFFERSIZE/8};

// Upper and lower limits for encoded signal
const signed short maxEncodedSig[NSUBBANDS] = MAXENCODEDSIG;
const signed short minEncodedSig[NSUBBANDS] = MINENCODEDSIG;

// Initial step size values
const unsigned short defaultStepSize[NSUBBANDS] = DEFAULTSTEPSIZE;


/****************************************
 * Perform signal encoding and decoding *
 ***************************************/

/* This is the function that is called when the program starts. */
int main (int argc, char *argv[])
{
  /* Variable declarations. */
  struct wavpcm_input input;
  struct wavpcm_output output;
  short buffer[BUFFERSIZE]; 
  unsigned char encodedSigBuffer[NBYTESENCODEDSIGNAL];
  int bufPos, read;

  memset(&input, 0, sizeof(struct wavpcm_input));
  input.resource=INPUTWAVFILE;
  memset(&output, 0, sizeof(struct wavpcm_output));
  output.resource=OUTPUTWAVFILE;

  /* First open input file and parse header, */
  wavpcm_input_open (&input);
  /* and then use this same header configuration for the output file */
  wavpcm_output_copy_settings(&input, &output);
  wavpcm_output_open(&output);
  
  /* Initialize chunk variables with zeros */
  struct encChunk leftEncChunk;  initEncChunk(&leftEncChunk);
  struct encChunk rightEncChunk; initEncChunk(&rightEncChunk);
  struct decChunk leftDecChunk;  initDecChunk(&leftDecChunk);
  struct decChunk rightDecChunk; initDecChunk(&rightDecChunk);

  /*bufPos expressed in temporal samples*/
  for (bufPos=0; bufPos<input.samplesAvailable; bufPos+=(BUFFERSIZE/2)){
	  /* Try to read BUFFERSIZE samples (16 bits, pairwise identical if input is mono, interleaved if input is stereo)  */
	  /* into buffer, with read the actual amount read (expressed in bytes! =  (2*read)/(channels * bitDepth/8) array elements)*/
	  if (bufPos==43000){
		  //breakpoint
	  }
	  read = wavpcm_input_read (&input, buffer);

	  /* transform buffer */

	  // Step 1: perform signal analysis
	  analysis(buffer,&leftEncChunk,&rightEncChunk);

	  // Step 2: perform signal encoding
	  quantise(buffer,&leftEncChunk,&rightEncChunk,encodedSigBuffer);

	  /* if required, dump compressed output */
		
	  /* inverse transform buffer */

	  // Step 1: perform signal decoding
	  dequantise(encodedSigBuffer, &leftDecChunk, &rightDecChunk, buffer);

	  // Step 2: perform signal synthesis
	  synthesis(buffer,&leftDecChunk,&rightDecChunk);

	  /* dump reconstructed output */
	  wavpcm_output_write (&output, buffer, read);
  }

  /* finalize output (write header) and close */
  wavpcm_output_close (&output);  

  /* Return successful exit code. */
  return 0;
}
