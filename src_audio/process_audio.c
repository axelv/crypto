#include "process_audio.h"

/* Include your header files here */
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

#include "codec.h"
#include "globals.h"
#include "encoder.h"
#include "decoder.h"


#include "crypto_wrapper.h"
#include "constants.h"

/* Declare your variables globally here */

short buffer[BUFFERSIZE];
unsigned char encoded[NBYTESENCODEDSIGNAL];
short reconstructedBuffer[BUFFERSIZE];

// crypto global variables
uint8_t packet[MAX_PACK_LENGTH];
uint8_t data_in[MAX_DATA_LENGTH];
uint8_t data_out[MAX_DATA_LENGTH];

/* Declare chunk variables */
struct encChunk leftEncChunk;
struct encChunk rightEncChunk;
struct decChunk leftDecChunk;
struct decChunk rightDecChunk;

/****************************
 * Define filter parameters *
 ****************************/

// Analysis filters
#pragma DATA_ALIGN(h1,8);
const short h1[LENGTH1] = H1;
#pragma DATA_ALIGN(h2,8);
const short h2[LENGTH2] = H2;
#pragma DATA_ALIGN(h3,8);
const short h3[LENGTH3] = H3;
#pragma DATA_ALIGN(h4,8);
const short h4[LENGTH4] = H4;

// Synthesis filters
#pragma DATA_ALIGN(g1,8);
const short g1[LENGTH1] = G1;
#pragma DATA_ALIGN(g2,8);
const short g2[LENGTH2] = G2;
#pragma DATA_ALIGN(g3,8);
const short g3[LENGTH3] = G3;
#pragma DATA_ALIGN(g4,8);
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

/* Include all your initialization code. This function is called once only */
void init_audio(){

	/**************************
	 *  Crpyto initialisation *
	 **************************/

	if(initialize_connection()==0)
	{
		printf("\n crypto initialisaion failed \n");
	}

	/***************************************
	 *  Initialize chunk variables to zero *
	 ***************************************/

	initEncChunk(&leftEncChunk);
	initEncChunk(&rightEncChunk);
	initDecChunk(&leftDecChunk);
	initDecChunk(&rightDecChunk);

	printf("\n Init done");
}


// Multi-iteration version

void process_audio(int* input_buf, int* output_buf, int nbytes)
{
   int i=0, iter=0;
   short *pshort;
   int nr_elements = BUFFERSIZE;
   short input_rot[BUFLEN*2], output_rot[BUFLEN*2];	/* Factor of 2 is because of sizeof(int)/sizeof(short) = 2*/
   int niters = (BUFLEN*2)/BUFFERSIZE;
   int stepsize = BUFFERSIZE/2;

   /* Dummy copy code below. Insert your encode, encrypt, decrypt, decode functions below and comment out the copy loop */

      /* Dummy copy code below. Insert your encode, encrypt, decrypt, decode functions below and comment out the copy loop */
//      for (i = 0; i < nbytes/4; i++) { // no._int is 1/4 of nbytes
//   	   output_buf[i] = input_buf[i];
//      }

   for (iter=0; iter<niters; iter++){
   		// Extract upper and lower 16 bits from input_buf[i]
   		pshort = (short*)(&input_buf[iter*stepsize]);
   		for (i = 0; i < nr_elements; i++) {
   			input_rot[(iter*BUFFERSIZE)+i] = pshort[i];
   		}

   		// Step 1: perform signal analysis
		analysis(input_rot,&leftEncChunk,&rightEncChunk);

		// Step 2: perform signal encoding
		quantise(input_rot,&leftEncChunk,&rightEncChunk,encoded);

		// Step 3: encryption
		memcpy(data_in, encoded, CRYPTO_BUFFERSIZE);
		encrypt_data(packet, data_in, CRYPTO_BUFFERSIZE);
   	}

   for (iter=0; iter<niters; iter++){


		//for(i=0; i<BUFFERSIZE; i++) {
	    //	output_rot[i] = input_rot[i];
	    //}


	   	// Step 0: decryption
	   	decrypt_data(data_out, packet);
	   	memcpy(encoded, data_out, CRYPTO_BUFFERSIZE);

	   	if(memcmp(data_in, data_out,CRYPTO_BUFFERSIZE)!=0){
			printf("\n decrypted packet was not equal to original packet . \n");
		}

	    // Step 1: perform signal decoding
		dequantise(encoded, &leftDecChunk, &rightDecChunk, output_rot);

	    // Step 2: perform signal synthesis
		synthesis(output_rot,&leftDecChunk,&rightDecChunk);

		pshort = (short*)(&output_buf[iter*stepsize]);
		for (i = 0; i < nr_elements; i++) {
			pshort[i] = output_rot[(iter*BUFFERSIZE)+i];
		}
   }

   /*for (iter=0; iter<niters; iter++){
		// Extract upper and lower 16 bits from input_buf[i]
		pshort = (short*)(&input_buf[iter*stepsize]);
		for (i = 0; i < nr_elements; i++) {
			input_rot[(iter*BUFFERSIZE)+i] = pshort[i];
		}

		// Step 1: perform signal analysis
		analysis(input_rot,&leftEncChunk,&rightEncChunk);

		// Step 2: perform signal encoding
		quantise(input_rot,&leftEncChunk,&rightEncChunk,encoded);
	}

   // encrypt and decrypt

	for (iter=0; iter<niters; iter++){
		// Step 1: perform signal decoding
		dequantise(encoded, &leftDecChunk, &rightDecChunk, output_rot);

		// Step 2: perform signal synthesis
		synthesis(output_rot,&leftDecChunk,&rightDecChunk);

		pshort = (short*)(&output_buf[iter*stepsize]);
		for (i = 0; i < nr_elements; i++) {
			pshort[i] = output_rot[(iter*BUFFERSIZE)+i];
		}
	}*/
}
