#crypto
Repository for P&D: Crypto Team 1.


#TODO:
- [ ] **Key Establishment (STS + RSA + SHA256)**

AV: Replace the predefined secret random numbers with a call to bbs.c
AV: Reconsider the use of 2PSIZE for gyx/gxy

Semi Implemented: Montgomery Multiplication (asymmetric/monmult.c) works, but 8bit operands should be changed to 32 bit in the future (for efficiency). ALSO: Maybe change this to more efficient Montgomery Square algorithm. (We only need squaring)

To be implemented: Mont. Exponentiation using Mont. Mult., algorithm is 14.94 in Handbook of Applied Crypto

To be implemented: RSA signing using Mont Exponentiation. Need also generate: PKCS #1 1.5 DigestInfo structures for SHA2-256. (See Toledo)

To be implemented: STS (Diffie Hellman) using Mont Exponentiation
- [ ] **Data Encryption (OCB)**

Is deze code ANSI-C en voldoet het aan de specs in de opgave (volgorde van argumenten etc) of moet er herschreven worden?
- [ ] **Finite State Machine for protocol flow (for both Master and Slave)**

Should be fairly straightforward
- [ ] **Make everything ANSI-C compliant!!!**

- [ ] **Cryptographically Secure Pseudo Random Number Generator (CSPRNG)** (eg. Bum Bum Chup) using several entropy sources. (Needed for STS!)

AV: Check if Montgomery squaring is faster than standard squaring operation of c.

AV: Change the typdef 'unsigned char' to uint8_t etc.

##Notes:
- To test ocb, compile using: ***gcc aes.c ocb.c -o test***
- *Do not use malloc( )!!*
- The order of a function’s arguments is important! If a function has arguments that specify
input and output parameters, then the output parameters should be specified before the
input parameters
- Use header files to centralize all constants and types you use in your *.c-files
