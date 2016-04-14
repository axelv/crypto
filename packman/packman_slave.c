#include "packman_slave.h"

static void create_data_packet(uint8_t packet[MAX_PACK_LENGTH], uint8_t *data, uint8_t length){
	uint8_t a[AUTHBYTES] = {DATA, 0,}; 				//veranderen door memcpy
	uint8_t p[MAX_DATA_LENGTH];						//kaas: decyphered plaintext 
	uint8_t c[MAX_DATA_LENGTH+TAGBYTES] = {0,};		//kaas: cyphertext
	uint8_t nonce[32] = {0,};
	uint8_t seq8[4] = {0,};
	
	memcpy(a+IDBYTES, &s_seq, SEQBYTES);
	memcpy(a+IDBYTES+SEQBYTES, &length, LENBYTES);
	int32_to_int8(seq8, s_seq);
	compute_SHA256(nonce, seq8, SEQBYTES);

	ocb_encrypt(c, key, nonce, a, AUTHBYTES, data, length);
	
	//authenticated data
	packet[0] = DATA;
	memcpy(packet+IDBYTES, &s_seq,SEQBYTES);
	memcpy(packet+IDBYTES+SEQBYTES, &length, LENBYTES);
	//encrypted data
	memcpy(packet+AUTHBYTES,c,length+TAGBYTES);
}
static int validate_data_packet(uint8_t data[MAX_DATA_LENGTH], uint8_t *packet){
	uint8_t length[LENBYTES];
	uint8_t nonce[100] = {0,};
	compute_SHA256(nonce, packet+IDBYTES, SEQBYTES);
	memcpy(length, packet+IDBYTES+SEQBYTES, LENBYTES);
	uint8_t result_tag = ocb_decrypt(data, key, nonce, packet, AUTHBYTES ,packet+AUTHBYTES , *length+TAGBYTES);
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

int s_validate_packet(uint8_t *data, uint8_t *packet){
	int is_valid = -1;
	uint8_t type = packet[0];

	if(memcmp(packet+IDBYTES, &s_seq, SEQBYTES)>=0){
		
		s_seq = (uint32_t )packet[IDBYTES]+ 1;
		if(type == EST1){
			
		}
		if(type == EST2){
			
		}
		if(type == EST3){
			
		}
		if(type == DATA){
			printf("Validate data. \n");
			is_valid = validate_data_packet(data, packet);	
		}
		if(type == EOT){
			
		}
	}
	return is_valid;
}