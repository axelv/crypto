Instructions to use realtime audio interfacing project
================================================================


1. Unzip and copy directory 2016_realtime_audio into your workspace area

2. In CCS, go to Project > Import Existing CCS Project

3. Browse to 2016_realtime_audio directory in your workspace

4. Uncheck "Copy projects into workspace"

5. Copy your own code (.c & .h files) into "2016_realtime_audio/src_speech_crypto" directory

6. You have to edit following files in "2016_realtime_audio/src_audio" directory
	a. process_audio.c
	b. process_audio.h

7. Changes in process_audio.h file
	- Set BUFLEN (see below)

8. Changes in process_audio.c file

	There are two functions in this file.
		- init_audio()
			init_audio() is called once and does all the initializations. 
			Typically all the initializations that you do in your main function 
			must be done here. The variables, arrays, etc which you have declared 
			in your main must be made global in this C file.

		- process_audio
			process_audio() will be called repeatedly. This function receives pointers 
			for input buffer and output buffer, and size of buffers in bytes.

			The input buffer contains audio samples received from line in. You can pass it 
			to your functions performing encoding, encryption, decryption and decoding. Your 
			output must be contained in the output buffer.

			Currently, there is a dummy copy loop in this function. This loop simply copies 
			the input buffer to the output buffer. Once you have placed your code, you must 
			comment out this loop.


9. Remove your main(). Your main() gets split into init_audio() and process_audio(), as discussed above.
	You can disable the entire content of your main.c as follows:
		
		#ifdef 0
			// contents of main.c file
		#endif
	
10. On the board, connect the audio input from PC to the line-in port and your earphones to the line-out port. 



11. Buffer Sizes:

	No. of samples processed in encode()/decode() = BUFFERSIZE [to be set in your globals.h]
	Each sample = 16 bits 

	No. of received audio samples = BUFLEN  [To be set in process_audio.h]
	Each sample = 32 bits, where upper 16 bits are for left channel and lower 16 bits for right channel. 

	Min. value of BUFLEN = (BUFFERSIZE * 16)/ 32
	Possible values of BUFLEN = Multiples of {(BUFFERSIZE * 16)/ 32}


12. Data Format of the Audio Input:

	Each of the received audio sample = 32 bits, where upper 16 bits are for left channel and lower 16 bits for right channel. 
	Therefore, you have to separate each 32bit sample into two 16bit values, one for each channel. 


13. Cycle Budget for Realtime Performance:

	We are using a sampling rate of 8KHz, i.e. 8000 samples per sec. 
	At 8KHz, you have a cycle budget of approximately 100,000 cycles to get a real time performance and clear audio output.
	This means that your entire code (speech + crypto) has to be less than 100,000 cycles for real time performance.


14. For speech and crypto interfacing:

	BUFLEN must be set appropriately depending on the size of the input buffer required by encrypt(). 

	Let, encrypt() require an input buffer of X bits.

	Required value of BUFLEN = ceiling{X/(BUFFERSIZE*16)} * {BUFFERSIZE * (16/32)}


	No. of bits received = BUFLEN * 32bits
	For BUFLEN = 20, the input buffer received is 640 bits. 

	If crypto needs larger input buffer, you will have to change BUFLEN (as discussed in #11 above).

	For BUFLEN > (BUFFERSIZE*16)/32, you will have to perform encode/decode more than one time (=iter).
	The process_audio() function has two 'iter' loops for it.

	A few example values:
		For BUFLEN = 20: input buffer = 640 bits : 1 encode, encrypt, decrypt, 1 decode
		For BUFLEN = 40: input buffer = 1280 bits: 2 encode, encrypt, decrypt, 2 decode
		For BUFLEN = 60: input buffer = 2560 bits: 3 encode, encrypt, decrypt, 3 decode
		:
		And so on.




