/* nettle, low-level cryptographics library
 *
 * Copyright (C) 2001, 2003, 2006 Niels Möller
 *  
 * The nettle library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 * 
 * The nettle library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with the nettle library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02111-1301, USA.
 */

#include <stdint.h>
#include "digest-info.h"

/* From RFC 3447, Public-Key Cryptography Standards (PKCS) #1: RSA
 * Cryptography Specifications Version 2.1.
 *
 *     id-sha256    OBJECT IDENTIFIER ::=
 *       {joint-iso-itu-t(2) country(16) us(840) organization(1)
 *         gov(101) csor(3) nistalgorithm(4) hashalgs(2) 1}
 */
const uint8_t
sha256_prefix[] =
{
  /* 19 octets prefix, 32 octets hash, total 51 */
  0x30,      49, /* SEQUENCE */
    0x30,    13, /* SEQUENCE */
      0x06,   9, /* OBJECT IDENTIFIER */
        0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01,
      0x05,   0, /* NULL */
    0x04,    32  /* OCTET STRING */
      /* Here comes the raw hash value */
};

unsigned int sha256_prefix_size = sizeof(sha256_prefix);

/* From pkcs-1v2
 *
 *   id-sha1 OBJECT IDENTIFIER ::=
 *     {iso(1) identified-organization(3) oiw(14) secsig(3)
 *   	 algorithms(2) 26}
 *   
 *   The default hash function is SHA-1: 
 *   sha1Identifier ::= AlgorithmIdentifier {id-sha1, NULL}
 */
const uint8_t
sha1_prefix[] =
{
  /* 15 octets prefix, 20 octets hash, total 35 */
  0x30,       33, /* SEQUENCE */
    0x30,      9, /* SEQUENCE */
      0x06,    5, /* OBJECT IDENTIFIER */
  	  0x2b, 0x0e, 0x03, 0x02, 0x1a,
      0x05,    0, /* NULL */
    0x04,     20  /* OCTET STRING */
      /* Here comes the raw hash value */
};

unsigned int sha1_prefix_size = sizeof(sha1_prefix);
