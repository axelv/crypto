#crypto
Repository for P&D: Crypto Team 1.


#TODO:
- [ ] **Key Establishment (STS + RSA + SHA256)**

**WE SHOULD REPLACE BBS WITH AES-CTR!!!**

To be implemented: STS (Diffie Hellman) using Mont Exponentiation

AV: Add check for g^y = 1 or 0 or modulus-1

- [ ] **Data Encryption (OCB)**

Is deze code ANSI-C en voldoet het aan de specs in de opgave (volgorde van argumenten etc) of moet er herschreven worden?
- [ ] **Finite State Machine for protocol flow (for both Master and Slave)**

Should be fairly straightforward

AV: Add case for EOT that redirects the slave/master to initlialisation
- [ ] **Make everything ANSI-C compliant!!!**

- [ ] **Cryptographically Secure Pseudo Random Number Generator (CSPRNG)** (eg. Bum Bum Chup) using several entropy sources. (Needed for STS!)

AV: Check if Montgomery squaring is faster than standard squaring operation of c.

AV: Change the typdef 'unsigned char' to uint8_t etc.

- [ ] **Create 'make'-file** (http://www.gnu.org/software/make/manual/make.html#Introduction)

Done. Run 'make rebuild' to recompile the full project, the executable will be stored in 'integration'.

##Notes:
- *Do not use malloc( )!!*
- The order of a function’s arguments is important! If a function has arguments that specify
input and output parameters, then the output parameters should be specified before the
input parameters
- Use header files to centralize all constants and types you use in your *.c-files

##Possible further optimisations (for DSP phase):
- Montgomery Mult:change 8 bit operands to 32 bit in the future (might give boost to efficiency). [Very difficult]
- Exponentiation: We mostly need squaring so maybe implement a more efficient MontSq(x) instead of MontMult(x,x)
- General restructuring of crypto code might give little boost?
