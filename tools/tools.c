#include "tools.h"
#include "../constants.h"

void int32_to_int8(uint8_t output[4], uint32_t input){
	output[0] = input;   
	output[1] = input >> 8; 
	output[2] = input >> 16;
	output[3] = input >> 24; 
}
void print_packet(uint8_t *packet, uint32_t type){
	
	int i;
	
	printf("\nERROR PACKET: \n");
	
	printf("|ID:");
	for(i=0; i<IDBYTES; i++){
		printf("%d",packet[i]);
	}
	printf("|SEQ:");
	for(i=IDBYTES; i<IDBYTES+SEQBYTES; i++){
		printf("%d",packet[i]);
	}
	printf("|LEN:");
	for(i=IDBYTES+SEQBYTES; i<AUTHBYTES; i++){
		printf("%d",packet[i]);
	}
	if(type == EST1){
		printf("|\n|GX:");
		for(i=AUTHBYTES; i<AUTHBYTES+PSIZE; i++){
			printf("%02x",packet[i]);
		}
		printf("|\n");
	}
	if(type == EST2){
		printf("|\n|GY:");
		for(i=AUTHBYTES; i<AUTHBYTES+PSIZE; i++){
			printf("%02x",packet[i]);
		}
		printf("|\n|S:");
		for(i=AUTHBYTES+PSIZE; i<AUTHBYTES+PSIZE+CIPHSIZE; i++){
			printf("%02x",packet[i]);
		}
		printf("|\n\n");
	}
	if(type == EST3){
		printf("|\n|S:");
		for(i=AUTHBYTES; i<AUTHBYTES+CIPHSIZE; i++){
			printf("%02x",packet[i]);
		}
		printf("|\n\n");
	}
}