#crypto
Repository for P&D: Crypto Team 1.


#TODO:
**FUNCTIONALITY**

- REPLACE BBS WITH **AES-CTR**!!!
- AV: Add check for g^y = 1 or 0 or modulus-1 to **STS**
- AV: Add case for EOT that redirects the slave/master to initlialisation


AV: Change the typdef 'unsigned char' to uint8_t etc.

**OPTIMISATIONS**
- M: Montgomery Mult:change 8 bit operands to 32 bit in the future (might give boost to efficiency). [Very difficult]
- Exponentiation: We mostly need squaring so maybe implement a more efficient MontSq(x) instead of MontMult(x,x)
- General restructuring of crypto code might give little boost?
- M: Remove the constant calling of mod_inverse (will save about 0.5M cycles)


##Notes:
- *Do not use malloc( )!!*
