/***********************************************************************
* FILENAME :        main.c
*
* DESCRIPTION : Main file to show the crypto code in action. Encrypts/decrypts one buffer of data.
*
*****/

#include <string.h>
#include "crypto_wrapper.h"
#include "constants.h"
#define BUFFERSIZE 30

int main(){
	
	if(initialize_connection()==0)return -1; // Do STS handshake to establish shared session key
		
	short buffer[BUFFERSIZE] = {0x89, 0x4c, 0xcf, 0xa1, 0xad, 0x58, 0x4a, 0xb2, 0xed, 0x58, 0xe3, 0x35, 0xad, 0xd7, 0x98, 
								0x1e, 0x28, 0x0a, 0x37, 0x73, 0x71, 0x2b, 0x7e, 0xe4, 0x19, 0x4f, 0x24, 0x67, 0x64, 0x19}; // Just a buffer with random data.
	uint8_t packet[MAX_PACK_LENGTH];
	uint8_t data_in[MAX_DATA_LENGTH];
	uint8_t data_out[MAX_DATA_LENGTH];
	memcpy(data_in, buffer, BUFFERSIZE); // Copy buffer to data_in
#ifdef PRINT
	printf("\nStart sending data. \n");
#endif
	encrypt_data(packet, data_in, BUFFERSIZE); // Encrypt data_in, placer result in packet
	decrypt_data(data_out, packet); // Decrypt packet, place result in data_out
	if(memcmp(data_in, data_out,BUFFERSIZE)==0){ 
#ifdef PRINT
		printf("Received data is correct! \n"); // if data_in == data_out all is well.
#endif
		//memcpy(buffer, data_out, BUFFERSIZE);
	}
	finalize_connection(data_out); // Close connection by sending EOT packet.
}
