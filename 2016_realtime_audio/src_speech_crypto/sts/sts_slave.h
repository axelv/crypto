#include <stdint.h>
#ifndef STS_SLAVE_H
#define STS_SLAVE_H
//Shared key for AES
extern uint8_t SKEY[PSIZE];

void reply_master(uint8_t message[PSIZE+HASHSIZE], uint8_t received_msg[PSIZE], uint8_t *seqnr);
uint8_t validate_master(uint8_t *message, uint8_t *seqnr);
#endif
