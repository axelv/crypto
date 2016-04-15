#include    <stdint.h>
#include 	<stdio.h>
#include 	<stdbool.h>
#include    "tools/tools.h"
#include    "constants.h"
#include	"packman/packman_master.h"
#include	"packman/packman_slave.h"

void encrypt_data(uint8_t output[MAX_PACK_LENGTH], uint8_t *input, uint8_t length);
void decrypt_data(uint8_t output[MAX_DATA_LENGTH], uint8_t *input);