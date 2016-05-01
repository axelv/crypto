#include "tools.h"
#include "../constants.h"


// Convert a an array with 8 bit words to an array with 16 bit words.
void convert_8_to_16(uint16_t *res, uint8_t *in){
        int i;
        for(i=0;i<156/2;i++){
                res[i] = in[2*i] + (in[(2*i)+1]<<8);
        }
}
// Convert a an array with 16 bit words to an array with 8 bit words.
void convert_16_to_8(uint8_t *res, uint16_t *in){
        int i;
        for(i=0;i<156/2;i++){
                res[2*i] = (uint8_t) in[i];
                res[(2*i)+1] = (uint8_t) (in[i]>>8);
        }
}

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
