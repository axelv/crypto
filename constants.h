//HEADER
#define IDBYTES    1
#define LENBYTES   3
#define	SEQBYTES   4
#define AUTHBYTES  IDBYTES + LENBYTES + SEQBYTES //{ID, SEQ, LEN}

//used in ocb.c
#define KEYBYTES   (128/8)  // = 128 bit = 16 byte 
#define NONCEBYTES (96/8)
#define TAGBYTES   (128/8)
#define BLOCKBYTES (128/8)

#if !(KEYBYTES==16 || KEYBYTES==24 || KEYBYTES==32) ||  \
     (NONCEBYTES > 15 || NONCEBYTES < 0) ||             \
     (TAGBYTES > 16 || TAGBYTES < 1)
#error -- KEYBYTES, NONCEBYTES, or TAGBYTES is an illegal value
#endif

//PACKET IDENTIFIERS
#define EST1 0
#define EST2 1
#define EST3 2
#define DATA 3
#define EOT 4

//Maximus size of data and packet
#define MAX_DATA_LENGTH 1024 //maximum length of data in bytes
#define MAX_PACK_LENGTH MAX_DATA_LENGTH + AUTHBYTES + TAGBYTES


