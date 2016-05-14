#ifndef __SHA2_WRAPPER_H__
#define __SHA2_WRAPPER_H__
#include <stdint.h>
#include "sha2.h"
void compute_SHA256(uint8_t *hashResult, uint8_t *to_be_hashed, size_t len_to_be_hashed);
#endif
