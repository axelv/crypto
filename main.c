#include    <stdint.h>
#include 	<stdio.h>
#include 	<stdbool.h>
#include    "constants.h"

int main(){
	uint8_t message[] = "Hallo dit is een test om te zien of het werkt. nog iets bij";
	uint8_t length = 59;
	uint8_t packet[MAX_PACK_LENGTH];
	uint8_t output[MAX_PACK_LENGTH];
	int i;

	//master creates packet
	m_create_packet(packet, message, 3, length);
	printf("ID: ");
	for (i=0; i<IDBYTES; i++) printf("%02X", packet[i]);
	printf("\nSEQ: ");
	for (i=IDBYTES; i<IDBYTES+SEQBYTES; i++) printf("%02X", packet[i]);
	printf("\n");

	//slave validates packet
	int result_tag = s_validate_packet(output,packet);
	printf("Result TAG: %d \n",result_tag);
	
	if(result_tag  == 0){
		printf("Valid packet: \n");
		for (i=0; i<length; i++) printf("%c", output[i]);
		printf("\n");
	}else{
		printf("False packet! \n");
	}
}