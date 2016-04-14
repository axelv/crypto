#ifndef __PACKMAN_H__
#define __PACKMAN_H__
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../constants.h"

static uint8_t key[KEYBYTES] = "3LLEBBLAP3EVUIOP";  //random testkey: 128bit
static uint32_t s_seq = 0x32323232; //for SEQBYTES = 4
static uint32_t m_seq = 0x32323232; //for SEQBYTES = 4
#endif