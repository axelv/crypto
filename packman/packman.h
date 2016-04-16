#ifndef __PACKMAN_H__
#define __PACKMAN_H__
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../constants.h"
#include "../ocb/ocb.h"
#include "../ciphers/sha2/sha2_wrapper.h"
#include "../tools/tools.h"
#define SEQINIT  0x32323232 //for SEQBYTES = 4

static uint8_t key[KEYBYTES] = "3LLEBBLAP3EVUIOP";  //random testkey: 128bit


#endif