#ifndef DIGEST_INFO_H
# define DIGEST_INFO_H

/* This file provides the prefixes to generate a PKCS #1 1.5
 * DigestInfo structure for various algorithms. By appending
 * the hash value to that prefix the DigestInfo structure
 * is formed.
 */

extern const uint8_t sha256_prefix[];
extern unsigned int sha256_prefix_size;

extern const uint8_t sha1_prefix[];
extern unsigned int sha1_prefix_size;

#endif