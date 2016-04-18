#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../constants.h"
#include "../ocb/ocb.h"
#include "../ciphers/sha2/sha2_wrapper.h"
#include "../tools/tools.h"
#include "packman_master.h"
#include "../sts/sts_master.h"

static uint32_t m_seq = SEQINIT; //for SEQBYTES = 4

static void create_data_packet(uint8_t packet[MAX_PACK_LENGTH], uint8_t *data, uint8_t length){
	uint8_t a[AUTHBYTES] = {DATA,0,}; 				
	uint8_t c[MAX_DATA_LENGTH+TAGBYTES] = {0,};		//kaas: cyphertext
	uint8_t nonce[32] = {0,};
	uint8_t seq8[4] = {0,};
	
	memcpy(a+IDBYTES, &m_seq, SEQBYTES);
	memcpy(a+IDBYTES+SEQBYTES, &length, LENBYTES);
	int32_to_int8(seq8, m_seq);
	compute_SHA256(nonce, seq8, SEQBYTES);
		
	ocb_encrypt(c, K, nonce, a, AUTHBYTES, data, length);
	
	//authenticated data
	memcpy(packet+IDBYTES, &m_seq,SEQBYTES);
	memcpy(packet+IDBYTES+SEQBYTES, &length, LENBYTES);
	//encrypted data
	memcpy(packet+AUTHBYTES,c,length+TAGBYTES);
}
static int validate_data_packet(uint8_t data[MAX_DATA_LENGTH], uint8_t *packet){
	uint8_t length[LENBYTES];
	uint8_t nonce[32];
	compute_SHA256(nonce, packet+IDBYTES, SEQBYTES);
	memcpy(length, packet+IDBYTES+SEQBYTES, LENBYTES);
	uint8_t result_tag = ocb_decrypt(data, K, nonce, packet, AUTHBYTES ,packet+AUTHBYTES , *length+TAGBYTES);
	return result_tag;
}
void m_create_packet(uint8_t packet[MAX_PACK_LENGTH], uint8_t *data, uint8_t type, uint8_t length){
	uint8_t seq8[4] = {0,};
	
	int32_to_int8(seq8, m_seq);
	
	if(type == EST1){
		memcpy(packet, &type, IDBYTES);
		memcpy(packet+IDBYTES, &m_seq, SEQBYTES);
		memcpy(packet+IDBYTES+SEQBYTES, &length, LENBYTES);
		init_connection(packet+AUTHBYTES);
	}
	if(type == EST3){
		memcpy(packet, &type, IDBYTES);
		memcpy(packet+IDBYTES, &m_seq, SEQBYTES);
		memcpy(packet+IDBYTES+SEQBYTES, &length, LENBYTES);
		reply_slave(packet+AUTHBYTES, seq8);
	}
	if(type == DATA){
		memcpy(packet, &type, IDBYTES);
		create_data_packet(packet, data, length);
	}
	if(type == EOT){
		
	}
	m_seq = m_seq + 1;
}
uint8_t m_validate_packet(uint8_t *data, uint8_t *packet){
	uint8_t is_valid = 0;
	uint8_t type = packet[0];
	//uint32_t *s_seq = (uint32_t *) packet+IDBYTES;
	
	if(memcmp(packet+IDBYTES, &m_seq, SEQBYTES)>=0){

		if(type == EST2){
			is_valid  = validate_slave(packet + AUTHBYTES, packet+IDBYTES);
			data = packet+AUTHBYTES;
		}
		if(type == DATA){
			is_valid = validate_data_packet(data, packet);	
		}
		if(type == EOT){
			
		}
		//m_seq = (uint32_t )packet[IDBYTES]+ 1; TODO
		m_seq = m_seq + 1;
	}else{
		printf("Invalid seq nr. \n");
	}
	return is_valid;
}