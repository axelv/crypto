CC=gcc
LDFLAGS=-lm 	# add -ggdb flag for debugging with gdb in Eclipse on Mac OSX
CFLAGS=-Wall -ansi -pedantic-errors -g -O0 -std=c99

rebuild: clean all	# first do clean, then compile SBC

clean:
	rm -f *.o SBC checkAnalSynth crypto
	
all: SBC checkAnalSynth

integration: crypto SBC

SBC: main.o wavpcm_io.o encoder.o decoder.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
	
checkAnalSynth: checkAnalSynth.o wavpcm_io.o encoder.o decoder.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
	
crypto: main.o wavpcm_io.o encoder.o decoder.o ../crypto_wrapper.o ../packman_slave.o ../packman_master.o ../ocb.o ../sha2.o ../sha2_wrapper.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@