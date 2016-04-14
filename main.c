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
	
	m_create_packet(packet, message, 3, length);
	bool result_tag = s_validate_packet(output,packet);
	printf("Result TAG: %d \n",result_tag);
	if(result_tag  == false){
		for (i=0; i<length; i++) printf("%c", output[i]);
		printf("\n");
	}
}