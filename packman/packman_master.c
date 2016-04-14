#include "packman.h"

static void create_data_packet(uint8_t packet[MAX_PACK_LENGTH], uint8_t *data, uint16_t length){
	uint8_t a[AUTHBYTES] = {DATA,0,}; 				
	uint8_t p[MAX_DATA_LENGTH];						//kaas: decyphered plaintext 
	uint8_t c[MAX_DATA_LENGTH+TAGBYTES] = {0,};		//kaas: cyphertext
	uint8_t nonce[256] = {0,};
	memcpy(a+IDBYTES, &m_seq, SEQBYTES);
	memcpy(a+IDBYTES+SEQBYTES, &length, LENBYTES);
	SHA256_Data(&m_seq, SEQBYTES, nonce);
	
	ocb_encrypt(c, key, nonce, a, AUTHBYTES, data, length);
	
	//authenticated data
	packet[0] = DATA;
	memcpy(packet+IDBYTES, &m_seq,SEQBYTES);
	memcpy(packet+IDBYTES+SEQBYTES, &length, LENBYTES);
	//encrypted data
	memcpy(packet+AUTHBYTES,c,length+TAGBYTES);
}
static int validate_data_packet(uint8_t data[MAX_DATA_LENGTH], uint8_t *packet){
	uint8_t length[LENBYTES];
	uint8_t nonce[256];
	SHA256_Data(packet+IDBYTES, SEQBYTES, nonce);
	memcpy(length, packet+IDBYTES+SEQBYTES, LENBYTES);
	uint8_t result_tag = ocb_decrypt(data, key, nonce, packet, AUTHBYTES ,packet+AUTHBYTES , *length+TAGBYTES);
	return result_tag;
}
void m_create_packet(uint8_t packet[MAX_PACK_LENGTH], uint8_t *data, uint8_t type, uint16_t length){
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
	m_seq = m_seq + 1;
}

int m_validate_packet(uint8_t *data, uint8_t *packet){
	int is_valid = -1;
	uint8_t type = packet[0];
	if(memcmp(packet+IDBYTES, &m_seq, SEQBYTES)>1){
		
		m_seq = (uint32_t )packet[IDBYTES]+ 1;
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