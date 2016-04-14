#include "packman.h"

static uint32_t s_seq = SEQINIT; //for SEQBYTES = 4

void s_create_packet(uint8_t packet[MAX_PACK_LENGTH], uint8_t *data, uint8_t type, uint8_t length);
int s_validate_packet(uint8_t *data, uint8_t *packet);