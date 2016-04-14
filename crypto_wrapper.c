#include "crypto_wrapper.h"

void encrypt_data(uint8_t output[MAX_PACK_LENGTH], uint8_t *input, uint8_t length, uint8_t key[KEYBYTES){
	m_create_packet(packet, message, DATA, length);
}
void decrypt_data(uint8_t output[MAX_DATA_LENGTH], uint8_t length){
	result_tag = s_validate_packet(output,packet);
	
	if(result_tag  == 0){
		printf("Valid packet! Received data: \n");
		for (i=0; i<length; i++) printf("%c", output[i]);
		printf("\n\n");

	}else{
		printf("False packet! \n");
	}
	
}