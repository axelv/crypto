CC=gcc
LDFLAGS=-lm 	# add -ggdb flag for debugging with gdb in Eclipse on Mac OSX
CFLAGS=-Wall -ansi -pedantic-errors -g -O0 -std=c99

rebuild: clean all

all: integration

clean: 
	$(RM) $^ integration main.o crypto_wrapper.o packman/packman_master.o packman/packman_slave.o  ciphers/sha2/sha2_wrapper.o ciphers/sha2/sha2.o ocb/ocb.o ocb/aes.o tools/tools.o
	
crypto: main_crypto.o crypto_wrapper.o packman/packman_master.o packman/packman_slave.o ciphers/sha2/sha2_wrapper.o ciphers/sha2/sha2.o ocb/ocb.o ocb/aes.o tools/tools.o
	$(LINK.o) $^ -o $@
	
integration: main.o SBC/wavpcm_io.o SBC/encoder.o SBC/decoder.o crypto_wrapper.o packman/packman_master.o packman/packman_slave.o ciphers/sha2/sha2_wrapper.o ciphers/sha2/sha2.o ocb/ocb.o ocb/aes.o tools/tools.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
	
SBC: SBC/main.o SBC/wavpcm_io.o SBC/encoder.o SBC/decoder.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@