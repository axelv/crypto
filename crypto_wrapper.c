#include "crypto_wrapper.h"

void encrypt_data(uint8_t output[MAX_PACK_LENGTH], uint8_t *input, uint8_t length){
	m_create_packet(output, input, DATA, length);
}
void decrypt_data(uint8_t output[MAX_DATA_LENGTH], uint8_t *input){
	uint8_t result_tag = s_validate_packet(output,input);
	
	if(result_tag  == 0){
		printf("Valid packet! Received data: \n");
	}else{
		printf("False packet! \n");
	}
	
}