#include "packman.h"

static uint32_t m_seq = SEQINIT; //for SEQBYTES = 4

void m_create_packet(uint8_t packet[MAX_PACK_LENGTH], uint8_t *data, uint8_t type, uint8_t length);
int m_validate_packet(uint8_t *data, uint8_t *packet);