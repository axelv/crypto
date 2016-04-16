#include "crypto_wrapper.h"

static int valid_packets;
static int invalid_packets;

void encrypt_data(uint8_t output[MAX_PACK_LENGTH], uint8_t *input, uint8_t length){
	m_create_packet(output, input, DATA, length);
}
int decrypt_data(uint8_t output[MAX_DATA_LENGTH], uint8_t *input){
	uint8_t result_tag = s_validate_packet(output,input);

	if(result_tag  == 0){
		valid_packets = valid_packets + 1;
		return 1;
	}else{
		invalid_packets = invalid_packets + 1;
		return 0;
	}
}
void finalize_connection(){
	printf("CRYPTO RESULTS: \n");
	printf("Number of valid packets: %d \n", valid_packets);
	printf("Number of invalid packets: %d \n", invalid_packets);	
}