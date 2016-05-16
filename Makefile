CC=gcc
LDFLAGS=-lm 	# add -ggdb flag for debugging with gdb in Eclipse on Mac OSX
CFLAGS=-Wall -ansi -pedantic-errors -g -O0 -std=c99

rebuild: clean all

all: integration
 
clean: 
	$(RM) $^ integration main_integration.o main.o SBC/wavpcm_io.o SBC/encoder.o SBC/decoder.o crypto_wrapper.o sts/sts_master.o sts/sts_slave.o sts/sts.o packman/packman_master.o packman/packman_slave.o asymmetric/monexp.o asymmetric/monmult.o asymmetric/signatures.o ciphers/sha2/sha2_wrapper.o ciphers/sha2/sha2.o ocb/ocb.o ocb/aes.o tools/tools.o bbs/bbs.o
	
clean_crypto:
	$(RM) $^ crypto main_crypto.o crypto_wrapper.o sts/sts_master.o sts/sts_slave.o sts/sts.o packman/packman_master.o packman/packman_slave.o asymmetric/monexp.o asymmetric/monmult.o asymmetric/signatures.o ciphers/sha2/sha2_wrapper.o ciphers/sha2/sha2.o ocb/ocb.o ocb/aes.o tools/tools.o bbs/bbs.o

crypto: main.o crypto_wrapper.o sts/sts_master.o sts/sts_slave.o packman/packman_master.o packman/packman_slave.o asymmetric/monexp.o asymmetric/monmult.o asymmetric/signatures.o ciphers/sha2/sha2_wrapper.o ciphers/sha2/sha2.o ocb/ocb.o ocb/aes.o tools/tools.o bbs/bbs.o
	$(LINK.o) $^ -o $@
	
integration: main_integration.o SBC/wavpcm_io.o SBC/encoder.o SBC/decoder.o crypto_wrapper.o sts/sts_master.o sts/sts_slave.o packman/packman_master.o packman/packman_slave.o asymmetric/monexp.o asymmetric/monmult.o asymmetric/signatures.o ciphers/sha2/sha2_wrapper.o ciphers/sha2/sha2.o ocb/ocb.o ocb/aes.o tools/tools.o bbs/bbs.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
	
SBC: SBC/main.o SBC/wavpcm_io.o SBC/encoder.o SBC/decoder.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
