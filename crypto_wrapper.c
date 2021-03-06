#include "crypto_wrapper.h"
#include "tools/tools.h"

static int valid_packets;
static int invalid_packets;

int initialize_connection(){
	uint8_t packet[MAX_PACK_LENGTH];
	uint8_t data[MAX_DATA_LENGTH];
	
	printf("\n------------------------------------------\n");
	printf("| Alice is the master | Bob is the slave |\n");
	printf("------------------------------------------\n");
	
	
	// Alice -> Bob: g^x
	m_create_packet(packet, NULL, EST1, 0);
	printf("Alice sended EST1 packet. \n");
	//print_packet(packet, AUTHBYTES+PSIZE);
	
	// 1. Bob validates g^x
	// 2. Bob -> Alice: g^y,Ek(S(g^y,g^x);
	if(s_validate_packet(data, packet)){
		printf("EST1 packet from Bob is valid! \n");
		s_create_packet(packet, data, EST2, 0);
		printf("Bob sended EST2 packet. \n");
	}else{
		printf("EST1 packet from Bob is not valid. \nEST1 packet: \n");
		print_packet(packet, EST1);
		return 0;
	}	
	// 1. Alice validates g^y and Bob's signature
	// 2. Alice -> Bob: Ek(S(g^x,g^y);
	if(m_validate_packet(data,packet)){
		printf("EST2 packet and signature from Alice are valid. \n");
		m_create_packet(packet, NULL, EST3, 0);
		printf("Alice sended EST3 packet. \n");
	}else{
		printf("EST2 packet and/or signature from Alice are not valid. \nEST2 packet: \n");
		print_packet(packet, EST2);
		return 0;
	}
	
	// Bob validates Alice's signature
	if(s_validate_packet(data,packet)){
		printf("EST3 packet from Bob is valid.\n");
		return 1;
	}else{
		printf("EST3 packet from Bob is not valid. \nEST3 packet:  \n");
		print_packet(packet, EST3);
		return 0;
	}
}

void encrypt_data(uint8_t output[MAX_PACK_LENGTH], uint8_t *input, uint8_t length){
	m_create_packet(output, input, DATA, length);
}

int decrypt_data(uint8_t output[MAX_DATA_LENGTH], uint8_t *input){
	uint8_t result_tag = s_validate_packet(output,input);

	if(result_tag  == 1){
		valid_packets = valid_packets + 1;
		return 1;
	}else{
		invalid_packets = invalid_packets + 1;
		return 0;
	}
}
void finalize_connection(){
	printf("\n----------------");
	printf("\nCRYPTO RESULTS: \n");
	printf("Number of valid packets: %d \n", valid_packets);
	printf("Number of invalid packets: %d \n", invalid_packets);	
}