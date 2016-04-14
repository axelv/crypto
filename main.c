#include    <stdint.h>
#include 	<stdio.h>
#include 	<stdbool.h>
#include    "tools/tools.c"
#include    "constants.h"
//#include	"message.c"

int main(){
	uint8_t message[] = "Hallo dit is een test om te zien of het werkt. nog iets bij";
	uint8_t length = 59;
	uint8_t packet[MAX_PACK_LENGTH];
	uint8_t output[MAX_PACK_LENGTH];
	int i;

	//master creates packet
	m_create_packet(packet, message, DATA, length);
	printf("ID: ");
	for (i=0; i<IDBYTES; i++) printf("%02X", packet[i]);
	printf("\nSEQ: ");
	for (i=IDBYTES; i<IDBYTES+SEQBYTES; i++) printf("%02X", packet[i]);
	printf("\n\n");

	//slave validates packet
	int result_tag = s_validate_packet(output,packet);
	
	if(result_tag  == 0){
		printf("Valid packet! Received data: \n");
		for (i=0; i<length; i++) printf("%c", output[i]);
		printf("\n\n");
	}else{
		printf("False packet! \n");
	}
	int cmp = memcmp(output, message, length);
	printf("Result comparator: %d \n",cmp);
}