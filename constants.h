//PRINT
#define PRINT 1

//HEADER
#define IDBYTES    1
#define LENBYTES   1
#define	SEQBYTES   4
#define AUTHBYTES  IDBYTES + LENBYTES + SEQBYTES //{ID, SEQ, LEN}

//used in ocb.c
#define KEYBYTES   (128/8)  // = 128 bit = 16 byte 
#define NONCEBYTES (96/8)
#define TAGBYTES   (128/8)
#define BLOCKBYTES (128/8)

//used in sts.h
//RSA
#define PRIV_KEY 156
#define PUB_KEY 3
#define NSIZE 157
#define SIGNSIZE 156 //NEED: waarom is dit anders?
#define CIPHSIZE SIGNSIZE+TAGBYTES
#define HASHSIZE 256/8 /*size of the signed message*/

//DH
#define GSIZE (156)  /*size of the generator*/
#define PSIZE (156) /*modulus and size of the message g^y & g^x */
#define XYSIZE (256/8) /*size of the secrete private number*/

#if !(KEYBYTES==16 || KEYBYTES==24 || KEYBYTES==32) ||  \
     (NONCEBYTES > 15 || NONCEBYTES < 0) ||             \
     (TAGBYTES > 16 || TAGBYTES < 1)
#error -- KEYBYTES, NONCEBYTES, or TAGBYTES is an illegal value
#endif

//used in packman.h
#define SEQINIT  0x32323232 //for SEQBYTES = 4

//used in bbs.c
#define MAX_l 256 //maximum length of random bits NEED: NOT SAFE CURRENTLY

//PACKET IDENTIFIERS
#define EST1 0
#define EST2 1
#define EST3 2
#define DATA 3
#define EOT 4

//Maximus size of data and packet
#define MAX_DATA_LENGTH 1024 //maximum length of data in bytes
#define MAX_PACK_LENGTH MAX_DATA_LENGTH + AUTHBYTES + TAGBYTES


