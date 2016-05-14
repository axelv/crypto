#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../constants.h"
#include "../ocb/ocb.h"
#include "../ciphers/sha2/sha2_wrapper.h"
#include "../tools/tools.h"
#include "packman_slave.h"
#include "../sts/sts_slave.h"


static uint32_t s_seq = SEQINIT; //for SEQBYTES = 4
uint8_t SKEY[PSIZE] = {0,};
static void create_data_packet(uint8_t packet[MAX_PACK_LENGTH], uint8_t *data, uint8_t length){
	uint8_t a[AUTHBYTES] = {DATA, 0,}; 				//veranderen door memcpy
	uint8_t c[MAX_DATA_LENGTH+TAGBYTES] = {0,};		//kaas: cyphertext
	uint8_t nonce[32] = {0,};
	uint8_t seq8[4] = {0,};
	
	memcpy(a+IDBYTES, &s_seq, SEQBYTES);
	memcpy(a+IDBYTES+SEQBYTES, &length, LENBYTES);
	int32_to_int8(seq8, s_seq);
	compute_SHA256(nonce, seq8, SEQBYTES);

	ocb_encrypt(c, SKEY, nonce, a, AUTHBYTES, data, length);
	
	//authenticated data
	memcpy(packet+IDBYTES, &s_seq,SEQBYTES);
	memcpy(packet+IDBYTES+SEQBYTES, &length, LENBYTES);
	//encrypted data
	memcpy(packet+AUTHBYTES,c,length+TAGBYTES);
}
static int validate_data_packet(uint8_t data[MAX_DATA_LENGTH], uint8_t *packet){
	uint8_t length[LENBYTES];
	uint8_t nonce[32] = {0,};
	compute_SHA256(nonce, packet+IDBYTES, SEQBYTES);
	memcpy(length, packet+IDBYTES+SEQBYTES, LENBYTES);
	uint8_t result_tag = ocb_decrypt(data, SKEY, nonce, packet, AUTHBYTES ,packet+AUTHBYTES , *length+TAGBYTES);
	return result_tag;
}
void s_create_packet(uint8_t packet[MAX_PACK_LENGTH], uint8_t *data, uint8_t type, uint8_t length){
	uint8_t seq8[4] = {0,};
	int32_to_int8(seq8, s_seq);
	
	if(type == EST2){
		memcpy(packet, &type, IDBYTES);
		memcpy(packet+IDBYTES, &s_seq, SEQBYTES);
		memcpy(packet+IDBYTES+SEQBYTES, &length, LENBYTES);
		reply_master(packet+AUTHBYTES, data, seq8);
	}
	if(type == DATA){
		memcpy(packet, &type, IDBYTES);
		create_data_packet(packet, data, length);
	}
	s_seq = s_seq + 1;
}

uint8_t s_validate_packet(uint8_t *data, uint8_t *packet){
	uint8_t is_valid = 0;
	uint8_t type;//TODO make array and use memcmp
	memcpy(&type, packet, IDBYTES);

	if(memcmp(packet+IDBYTES, &s_seq, SEQBYTES)>=0){	
		
		
		if(type == EST1){
			is_valid=1;
			memcpy(data,packet+AUTHBYTES,PSIZE);
		}
		if(type == EST3){
			is_valid  = validate_master(packet + AUTHBYTES, packet+IDBYTES);
			data = packet+AUTHBYTES;
		}
		if(type == DATA){
			is_valid = validate_data_packet(data, packet);	
		}
		if(type == EOT){
#if PRINT
			printf("\nEnd of Transmission \n");
#endif
			memset(SKEY,0, KEYBYTES);
			memset(data, 0, MAX_DATA_LENGTH);
		}
		//s_seq = (uint32_t )packet[IDBYTES]+ 1; TODO
		s_seq = s_seq + 1;
	}else{
		printf("Invalid seq nr. \n");
	}
	return is_valid;
}
