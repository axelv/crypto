#include <stdint.h>

//Shared key for AES
static uint8_t K[PSIZE] =  {0,};

void init_connection(uint8_t message[PSIZE]);
uint8_t validate_slave(uint8_t *message, uint8_t *seqnr);
void reply_slave(uint8_t message[HASHSIZE], uint8_t *seqnr);
