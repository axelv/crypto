CC=gcc

all: crypto

crypto: 
	$(CC) main.c crypto_wrapper.c packman/packman_slave.c packman/packman_master.c ocb/ocb.c ciphers/sha2/sha2.c ciphers/sha2/sha2_wrapper.c tools/tools.c -o test
