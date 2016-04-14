#include "packman.h"

static uint8_t s_seq = 0;

static void create_data_packet(uint8_t packet[MAX_PACK_LENGTH], uint8_t *data, uint8_t length){
	uint8_t a[AUTHBYTES] = {DATA, s_seq, length};
	uint8_t p[length];						//kaas: decyphered plaintext 
	uint8_t c[MAX_PACK_LENGTH] = {0,};		//kaas: cyphertext
	ocb_encrypt(c, key, n, a, AUTHBYTES, data, length);
	packet[0] = DATA;
	packet[1] = s_seq;
	packet[2] = length;
	memcpy(packet+AUTHBYTES,c,length+TAGBYTES);
}
static bool validate_data_packet(uint8_t data[MAX_DATA_LENGTH], uint8_t *packet){
	uint8_t length = packet[2];
	uint8_t result_tag = ocb_decrypt(data, key, n, packet, AUTHBYTES ,packet+3 , length+TAGBYTES);
	return result_tag;
}
void s_create_packet(uint8_t packet[MAX_PACK_LENGTH], uint8_t *data, uint8_t type, uint8_t length){
	if(type == EST1){
		
	}
	if(type == EST2){
		
	}
	if(type == EST3){
		
	}
	if(type == DATA){
		create_data_packet(packet, data, length);
	}
	if(type == EOT){
		
	}
	s_seq = s_seq + 1;
}

bool s_validate_packet(uint8_t *data, uint8_t *packet){
	bool is_valid = false;
	uint8_t type = packet[0];
	uint8_t received_seq = packet[1];
	printf("seq: %d \n",s_seq);
	printf("received_seq: %d \n",received_seq);
	if(received_seq >= s_seq){
		printf("received_seq >= seq \n");
		
		s_seq = received_seq + 1;
		if(type == (uint8_t) EST1){
			
		}
		if(type == (uint8_t) EST2){
			
		}
		if(type == (uint8_t) EST3){
			
		}
		if(type == (uint8_t) DATA){
			is_valid = validate_data_packet(data, packet);	
		}
		if(type == (uint8_t) EOT){
			
		}
	}
	return is_valid;
}