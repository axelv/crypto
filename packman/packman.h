#ifndef __PACKMAN_H__
#define __PACKMAN_H__
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../constants.h"
#include "../ciphers/sha2/sha2_wrapper.h"
static uint8_t key[KEYBYTES] = "3LLEBBLAP3EVUIOP";  //random testkey: 128bit
static unsigned int s_seq = 0x32; //for SEQBYTES = 4
static unsigned int m_seq = 0x32; //for SEQBYTES = 4

#endif