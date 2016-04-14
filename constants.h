#define AUTHBYTES  3

#define EST1 0
#define EST2 1
#define EST3 2
#define DATA 3
#define EOT 4

#define MAX_PACK_LENGTH 2048
#define MAX_DATA_LENGTH 1024

#define KEYBYTES   (128/8)  // = 128 bit = 16 byte 
#define NONCEBYTES (96/8)
#define TAGBYTES   (128/8)
#define BLOCKBYTES (128/8)

#if !(KEYBYTES==16 || KEYBYTES==24 || KEYBYTES==32) ||  \
     (NONCEBYTES > 15 || NONCEBYTES < 0) ||             \
     (TAGBYTES > 16 || TAGBYTES < 1)
#error -- KEYBYTES, NONCEBYTES, or TAGBYTES is an illegal value
#endif