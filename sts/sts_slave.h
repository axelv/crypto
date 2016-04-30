#include <stdint.h>

//Shared key for AES
static uint8_t K[PSIZE] =  {0,};

void reply_master(uint8_t message[PSIZE+HASHSIZE], uint8_t received_msg[PSIZE], uint8_t *seqnr);
uint8_t validate_master(uint8_t *message, uint8_t *seqnr);
