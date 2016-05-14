#include <stdint.h>
#ifndef STS_MASTER_H
#define STS_MASTER_H
//Shared key for AES
extern uint8_t MKEY[PSIZE];

void init_connection(uint8_t message[PSIZE]);
uint8_t validate_slave(uint8_t *message, uint8_t *seqnr);
void reply_slave(uint8_t message[HASHSIZE], uint8_t *seqnr);
#endif
