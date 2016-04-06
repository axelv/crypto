#include "../asymmetric/monexp.c"
#include "../ciphers/sha2/sha2.c"
#include "../digest-info/digest-info.c"
#include "../bbs/bbs.c"
#include "../ciphers/sha2/sha2.c"

#define GSIZE (128/8)  /*size of the generator*/
#define PSIZE (2048/8) /*modulus and size of the message g^y & g^x */
#define XYSIZE (256/8) /*size of the secrete private number*/

uint8_t g[GSIZE] = {0x50, 0x24, 0x6E, 0x0E, 0x44, 0xDD, 0xE3, 0x57, 0xEB, 0xB9, 0xEF, 0xCE, 0xB4, 0x10, 0x8D, 0xBF}