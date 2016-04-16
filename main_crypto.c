#include "crypto_wrapper.h"
#define BUFFERSIZE 30

int main(){
	/***************************************/
	/*            CRYPTOSHIZLE             */
	/***************************************/
	short buffer[BUFFERSIZE] = {0x89, 0x4c, 0xcf, 0xa1, 0xad, 0x58, 0x4a, 0xb2, 0xed, 0x58, 0xe3, 0x35, 0xad, 0xd7, 0x98, 
								0x1e, 0x28, 0x0a, 0x37, 0x73, 0x71, 0x2b, 0x7e, 0xe4, 0x19, 0x4f, 0x24, 0x67, 0x64, 0x19};
	uint8_t packet[MAX_PACK_LENGTH];
	uint8_t data_in[MAX_DATA_LENGTH];
	uint8_t data_out[MAX_DATA_LENGTH];
	memcpy(data_in, buffer, BUFFERSIZE);
	 
	encrypt_data(packet, data_in, BUFFERSIZE);
	decrypt_data(data_out, packet);
	if(memcmp(data_in, data_out,BUFFERSIZE)==0){
		printf("Received data is correct! \n");
		memcpy(buffer, data_out, BUFFERSIZE);	  	
	}
}