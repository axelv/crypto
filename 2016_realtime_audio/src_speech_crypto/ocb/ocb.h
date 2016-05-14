#define OCB_ENCRYPT 1
#define OCB_DECRYPT 0

void ocb_encrypt(unsigned char *c, unsigned char *k, unsigned char *n,
                 unsigned char *a, unsigned abytes,
                 unsigned char *p, unsigned pbytes);

/* ------------------------------------------------------------------------- */

int ocb_decrypt(unsigned char *p, unsigned char *k, unsigned char *n,
                unsigned char *a, unsigned abytes,
                unsigned char *c, unsigned cbytes);
