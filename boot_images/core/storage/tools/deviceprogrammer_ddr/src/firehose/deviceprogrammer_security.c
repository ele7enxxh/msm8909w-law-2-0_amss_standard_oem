/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       S E C U R I T Y   S E R V I C E S

                       H A S H I N G   F U N C T I O N S

                   ( A R M   I M P L E M E N T A T I O N S )

GENERAL DESCRIPTION
  This sofware module contains the source code for the Security Services
  Crypto-Interface layer for message digest specific to the ARM.

EXTERNAL FUNCTIONS

  sechsh_create_digest() - Creates the message digest

Copyright (c) 2013-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/deviceprogrammer_ddr/src/firehose/deviceprogrammer_security.c#1 $ 
  $DateTime: 2015/09/01 00:30:35 $ 
  $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2015-05-11   sb      Use crypto sha for ddr version only
2015-04-01   sb      Use crypto bam for hash calculation
2013-06-03   ah      Added legal header
2013-05-31   ab      Initial checkin
 
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdio.h>
#include <string.h>
//#include <basetypes.h>
//#include <basetypes_qcom.h>
//#include <memory.h>
#include <comdef.h>
#include "deviceprogrammer_security.h"
#include "stringl.h"

#include "boot_util.h"
//#include "boot_sbl_if.h"
#include "secboot_hw.h"
#include "secboot.h"
#include BOOT_PBL_H

#define SECAPI_HSH_SHA_BLOCK_BYTE_LEN      64
#define SECAPI_HSH_SHA_DIGEST_SIZE         20

void logMessage(const char* format, ...);

/* ------------------------------------------------------------ */
/* Constants and definitions for SHA-1                          */
/* ------------------------------------------------------------ */
typedef union
{
    unsigned long l[16];
    unsigned char c[64];

} CHAR64LONG16;

void our_memset(void *ptr, uint8 val, uint32 size)
{
   uint32 count=0;
   uint8 * tmp_ptr= (uint8*)ptr;
   for (count =0; count < size; count ++)
   {
      *tmp_ptr=val;
      tmp_ptr++;
   }
}
/* Optimizations */
unsigned long rol30( unsigned long value)
{
  return (value << 30) | ((value >> 2) & 0x3fffffff);
}

unsigned long rol24( unsigned long value)
{
  return (value << 24) | ((value >> 8) & 0x00ffffff);
}

unsigned long rol8( unsigned long value)
{
  return (value << 8) | ((value >> 24) & 0x000000ff);
}

unsigned long rol5( unsigned long value)
{
  return (value << 5) | ((value >> 27) & 0x0000001f);
}

unsigned long rol1( unsigned long value)
{
  return (value << 1) | ((value >> 31) & 0x00000001);
}


/* blk0() and blk() perform the initial expand */

//#if defined(__BIG_ENDIAN)
//#define blk0(block, i)      (block->l[i])
//#else
#define blk0(block, i) (block->l[i]        = (rol24(block->l[i]) & 0xff00ff00) | (rol8(block->l[i]) & 0x00ff00ff))
//#endif //
#define blk(block, i)  (block->l[i & 0x0f] = rol1(block->l[(i + 13) & 0x0f] ^ block->l[(i +  8) & 0x0f] ^ block->l[(i +  2) & 0x0f] ^ block->l[(i +  0) & 0x0f]))

/* R0, R1, R2, R3, R4 are the different operations used in SHA1 */

#define R0(block, work, v, w, x, y, z, i) \
    (work[z] += ((work[w]&(work[x]^work[y]))^work[y])           + blk0(block, i)    + 0x5a827999 + rol5(work[v]), work[w] = rol30(work[w]))

#define R1(block, work, v, w, x, y, z, i) \
    (work[z] += ((work[w]&(work[x]^work[y]))^work[y])           + blk(block, i)     + 0x5a827999 + rol5(work[v]), work[w] = rol30(work[w]))

#define R2(block, work, v, w, x, y, z, i) \
    (work[z] += (work[w]^work[x]^work[y])                       + blk(block, i)     + 0x6ed9eba1 + rol5(work[v]), work[w] = rol30(work[w]))

#define R3(block, work, v, w, x, y, z, i) \
    (work[z] += (((work[w]|work[x])&work[y])|(work[w]&work[x])) + blk(block, i)     + 0x8f1bbcdc + rol5(work[v]), work[w] = rol30(work[w]))

#define R4(block, work, v, w, x, y, z, i) \
    (work[z] += (work[w]^work[x]^work[y])                       + blk(block, i)     + 0xca62c1d6 + rol5(work[v]), work[w] = rol30(work[w]))

/* Forward declarations of the internal working functions */
void sechsharm_sha1_init(   struct __sechsh_ctx_s* );
void sechsharm_sha1_update( struct __sechsh_ctx_s*, uint8*, uint32*, uint8*, uint32 );
void sechsharm_sha1_final(  struct __sechsh_ctx_s*, uint8*, uint32*, uint8* );
void sechsharm_sha1_transform( uint32*, unsigned char* );


/* ------------------------------------------------------------ */
/* Constants and definitions for SHA-256                          */
/* ------------------------------------------------------------ */
#define HSHSHA_COUNTER_SIZE 8

// Try and optimize this thing
#define SHA2_UNROLL_TRANSFORM

/*** SHA-256 Various Length Definitions ***********************/
#define SHA256_BLOCK_LENGTH    64
#define SHA256_SHORT_BLOCK_LENGTH	(SHA256_BLOCK_LENGTH - 8)

/*** ENDIAN SPECIFIC COPY MACROS **************************************/
#define BE_8_TO_32(dst, cp) do {				\
      (dst) = (uint32)(cp)[3] | ((uint32)(cp)[2] << 8) |	\
	 ((uint32)(cp)[1] << 16) | ((uint32)(cp)[0] << 24);	\
   } while(0)

#define BE_8_TO_64(dst, cp) do {					\
      (dst) = (uint64_t)(cp)[7] | ((uint64_t)(cp)[6] << 8) |		\
	 ((uint64_t)(cp)[5] << 16) | ((uint64_t)(cp)[4] << 24) |	\
	 ((uint64_t)(cp)[3] << 32) | ((uint64_t)(cp)[2] << 40) |	\
	 ((uint64_t)(cp)[1] << 48) | ((uint64_t)(cp)[0] << 56);		\
   } while (0)

#define BE_64_TO_8(cp, src) do {		\
      (cp)[0] = (uint8)((src) >> 56);		\
      (cp)[1] = (uint8)((src) >> 48);		\
      (cp)[2] = (uint8)((src) >> 40);		\
      (cp)[3] = (uint8)((src) >> 32);		\
      (cp)[4] = (uint8)((src) >> 24);		\
      (cp)[5] = (uint8)((src) >> 16);		\
      (cp)[6] = (uint8)((src) >> 8);		\
      (cp)[7] = (uint8)((src));			\
   } while (0)

#define BE_32_TO_8(cp, src) do {		\
      (cp)[0] = (uint8)((src) >> 24);		\
      (cp)[1] = (uint8)((src) >> 16);		\
      (cp)[2] = (uint8)((src) >> 8);		\
      (cp)[3] = (uint8)((src));			\
   } while (0)

/*
 * Macro for incrementally adding the unsigned 64-bit integer n to the
 * unsigned 128-bit integer (represented using a two-element array of
 * 64-bit words):
 */
#define ADDINC128(w,n) do {			\
      (w)[0] += (uint64_t)(n);			\
      if ((w)[0] < (n)) {			\
	 (w)[1]++;				\
      }						\
   } while (0)

/** THE SIX LOGICAL FUNCTIONS ****************************************/
/*
 * Bit shifting and rotation (used by the six SHA-XYZ logical functions:
 *
 *   NOTE:  The naming of R and S appears backwards here (R is a SHIFT and
 *   S is a ROTATION) because the SHA-256/384/512 description document
 *   (see http://csrc.nist.gov/cryptval/shs/sha256-384-512.pdf) uses this
 *   same "backwards" definition.
 */
/* Shift-right (used in SHA-256, SHA-384, and SHA-512): */
#define R(b,x) 		((x) >> (b))
/* 32-bit Rotate-right (used in SHA-256): */
#define SHA_S32(b,x)	(((x) >> (b)) | ((x) << (32 - (b))))
/* 64-bit Rotate-right (used in SHA-384 and SHA-512): */
#define SHA_S64(b,x)	(((x) >> (b)) | ((x) << (64 - (b))))

/* Two of six logical functions used in SHA-256, SHA-384, and SHA-512: */
#define Ch(x,y,z)	(((x) & (y)) ^ ((~(x)) & (z)))
#define Maj(x,y,z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

/* Four of six logical functions used in SHA-256: */
#define Sigma0_256(x)	(SHA_S32(2,  (x)) ^ SHA_S32(13, (x)) ^ SHA_S32(22, (x)))
#define Sigma1_256(x)	(SHA_S32(6,  (x)) ^ SHA_S32(11, (x)) ^ SHA_S32(25, (x)))
#define sigma0_256(x)	(SHA_S32(7,  (x)) ^ SHA_S32(18, (x)) ^ R(3 ,   (x)))
#define sigma1_256(x)	(SHA_S32(17, (x)) ^ SHA_S32(19, (x)) ^ R(10,   (x)))

/*** SHA-XYZ INITIAL HASH VALUES AND CONSTANTS ************************/
/* Hash constant words K for SHA-256: */
static const uint32 K256[64] = {
   0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
   0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
   0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
   0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
   0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
   0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
   0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
   0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
   0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
   0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
   0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
   0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
   0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
   0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
   0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
   0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

/* Initial hash value H for SHA-256: */
static const uint32 sha256_initial_hash_value[8] = {
   0x6a09e667UL,
   0xbb67ae85UL,
   0x3c6ef372UL,
   0xa54ff53aUL,
   0x510e527fUL,
   0x9b05688cUL,
   0x1f83d9abUL,
   0x5be0cd19UL
};

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                    EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                  INTERNAL FUNCTION DEFINTIONS FOR SHA-1
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION SECHSHARM_SHA1_TRANSFORM()

DESCRIPTION
  Transform the input block into a digest update.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void sechsharm_sha1_transform
(
  uint32*  iv, 
  unsigned char*  buf
)
{
  unsigned long bufi[16];                     /* internal buffer */
  unsigned long work[5];                      /* internal work registers */
  
  CHAR64LONG16*  block = (CHAR64LONG16*) bufi;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Copy operating buffer to buf */
  memscpy( bufi, sizeof(bufi), buf, sizeof( bufi ) );
  
  /* Copy context->iv[] to working vars */
  work[0] = iv[0];                 /* a */
  work[1] = iv[1];                 /* b */
  work[2] = iv[2];                 /* c */
  work[3] = iv[3];                 /* d */
  work[4] = iv[4];                 /* e */
  
  /* 4 rounds of 20 operations each. Loop unrolled */
  /* Round 1 */
  R0( block, work, 0, 1, 2, 3, 4,  0 );
  R0( block, work, 4, 0, 1, 2, 3,  1 );
  R0( block, work, 3, 4, 0, 1, 2,  2 );
  R0( block, work, 2, 3, 4, 0, 1,  3 );
  R0( block, work, 1, 2, 3, 4, 0,  4 );
  R0( block, work, 0, 1, 2, 3, 4,  5 );
  R0( block, work, 4, 0, 1, 2, 3,  6 );
  R0( block, work, 3, 4, 0, 1, 2,  7 );
  R0( block, work, 2, 3, 4, 0, 1,  8 );
  R0( block, work, 1, 2, 3, 4, 0,  9 );
  R0( block, work, 0, 1, 2, 3, 4, 10 );
  R0( block, work, 4, 0, 1, 2, 3, 11 );
  R0( block, work, 3, 4, 0, 1, 2, 12 );
  R0( block, work, 2, 3, 4, 0, 1, 13 );
  R0( block, work, 1, 2, 3, 4, 0, 14 );
  R0( block, work, 0, 1, 2, 3, 4, 15 );
  
  R1( block, work, 4, 0, 1, 2, 3, 16 );
  R1( block, work, 3, 4, 0, 1, 2, 17 );
  R1( block, work, 2, 3, 4, 0, 1, 18 );
  R1( block, work, 1, 2, 3, 4, 0, 19 );
  
  /* Round 2 */
  R2( block, work, 0, 1, 2, 3, 4, 20 );
  R2( block, work, 4, 0, 1, 2, 3, 21 );
  R2( block, work, 3, 4, 0, 1, 2, 22 );
  R2( block, work, 2, 3, 4, 0, 1, 23 );
  R2( block, work, 1, 2, 3, 4, 0, 24 );
  R2( block, work, 0, 1, 2, 3, 4, 25 );
  R2( block, work, 4, 0, 1, 2, 3, 26 );
  R2( block, work, 3, 4, 0, 1, 2, 27 );
  R2( block, work, 2, 3, 4, 0, 1, 28 );
  R2( block, work, 1, 2, 3, 4, 0, 29 );
  R2( block, work, 0, 1, 2, 3, 4, 30 );
  R2( block, work, 4, 0, 1, 2, 3, 31 );
  R2( block, work, 3, 4, 0, 1, 2, 32 );
  R2( block, work, 2, 3, 4, 0, 1, 33 );
  R2( block, work, 1, 2, 3, 4, 0, 34 );
  R2( block, work, 0, 1, 2, 3, 4, 35 );
  R2( block, work, 4, 0, 1, 2, 3, 36 );
  R2( block, work, 3, 4, 0, 1, 2, 37 );
  R2( block, work, 2, 3, 4, 0, 1, 38 );
  R2( block, work, 1, 2, 3, 4, 0, 39 );
  
  /* Round 3 */
  R3( block, work, 0, 1, 2, 3, 4, 40 );
  R3( block, work, 4, 0, 1, 2, 3, 41 );
  R3( block, work, 3, 4, 0, 1, 2, 42 );
  R3( block, work, 2, 3, 4, 0, 1, 43 );
  R3( block, work, 1, 2, 3, 4, 0, 44 );
  R3( block, work, 0, 1, 2, 3, 4, 45 );
  R3( block, work, 4, 0, 1, 2, 3, 46 );
  R3( block, work, 3, 4, 0, 1, 2, 47 );
  R3( block, work, 2, 3, 4, 0, 1, 48 );
  R3( block, work, 1, 2, 3, 4, 0, 49 );
  R3( block, work, 0, 1, 2, 3, 4, 50 );
  R3( block, work, 4, 0, 1, 2, 3, 51 );
  R3( block, work, 3, 4, 0, 1, 2, 52 );
  R3( block, work, 2, 3, 4, 0, 1, 53 );
  R3( block, work, 1, 2, 3, 4, 0, 54 );
  R3( block, work, 0, 1, 2, 3, 4, 55 );
  R3( block, work, 4, 0, 1, 2, 3, 56 );
  R3( block, work, 3, 4, 0, 1, 2, 57 );
  R3( block, work, 2, 3, 4, 0, 1, 58 );
  R3( block, work, 1, 2, 3, 4, 0, 59 );
  
  /* Round 4 */
  R4( block, work, 0, 1, 2, 3, 4, 60 );
  R4( block, work, 4, 0, 1, 2, 3, 61 );
  R4( block, work, 3, 4, 0, 1, 2, 62 );
  R4( block, work, 2, 3, 4, 0, 1, 63 );
  R4( block, work, 1, 2, 3, 4, 0, 64 );
  R4( block, work, 0, 1, 2, 3, 4, 65 );
  R4( block, work, 4, 0, 1, 2, 3, 66 );
  R4( block, work, 3, 4, 0, 1, 2, 67 );
  R4( block, work, 2, 3, 4, 0, 1, 68 );
  R4( block, work, 1, 2, 3, 4, 0, 69 );
  R4( block, work, 0, 1, 2, 3, 4, 70 );
  R4( block, work, 4, 0, 1, 2, 3, 71 );
  R4( block, work, 3, 4, 0, 1, 2, 72 );
  R4( block, work, 2, 3, 4, 0, 1, 73 );
  R4( block, work, 1, 2, 3, 4, 0, 74 );
  R4( block, work, 0, 1, 2, 3, 4, 75 );
  R4( block, work, 4, 0, 1, 2, 3, 76 );
  R4( block, work, 3, 4, 0, 1, 2, 77 );
  R4( block, work, 2, 3, 4, 0, 1, 78 );
  R4( block, work, 1, 2, 3, 4, 0, 79 );
  
  /* Add the working vars back into context.iv[] */
  iv[0] += work[0];
  iv[1] += work[1];
  iv[2] += work[2];
  iv[3] += work[3];
  iv[4] += work[4];

} /* sechsharm_sha1_transform() */


/*===========================================================================

FUNCTION SECHSHARM_SHA1_INIT()

DESCRIPTION
  Initialize the hash context to prepare for message digest operations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void sechsharm_sha1_init
(
  struct __sechsh_ctx_s* context             /* context */
)
{
  our_memset( context, 0, sizeof( struct __sechsh_ctx_s) );

  /* SHA1 initialization constants */
  context->iv[0] = 0x67452301;
  context->iv[1] = 0xefcdab89;
  context->iv[2] = 0x98badcfe;
  context->iv[3] = 0x10325476;
  context->iv[4] = 0xc3d2e1f0;
  
  context->counter[0] = context->counter[1] = 0;

} /* sechsharm_sha1_init() */


/*===========================================================================

FUNCTION SECHSHARM_SHA1_UPDATE()

DESCRIPTION
  Continue the SHA1 digest operation, process input in block-sized chunks,
  save the leftover input, update the context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void sechsharm_sha1_update
(
  struct __sechsh_ctx_s* context,            /* context */
  uint8*                 leftover,           /* leftover input in context */
  uint32*                leftover_size,
  uint8*                 data, 
  uint32                 len
)
{
  int i, j;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Compute number of bytes mod 64 */
  j = (context->counter[0] >> 3) & 63;
  
  /* Update number of bits */
  context->counter[0] += (len << 3);
  if ( context->counter[0] < (len << 3) )
  {
    context->counter[1]++;
  }
  context->counter[1] += (len >> 29);
  
  /* Transform as many times as possible. */
  if ( (j + len) > SECAPI_HSH_SHA_BLOCK_BYTE_LEN-1 )
  {
    i = SECAPI_HSH_SHA_BLOCK_BYTE_LEN - j;
    
    memscpy( &leftover[j], CONTEXT_LEFTOVER_FIELD_SIZE - j, data, i );
    
    j = 0;
    
    sechsharm_sha1_transform( context->iv, /*context->*/leftover );
    
    while (i + SECAPI_HSH_SHA_BLOCK_BYTE_LEN-1 < len)
    {
      sechsharm_sha1_transform( context->iv, &data[i] );
      i += SECAPI_HSH_SHA_BLOCK_BYTE_LEN;
    }
  }
  else
  {
    i = 0;
  }
  
  /* Buffer remaining input */
  memscpy( &leftover[j], CONTEXT_LEFTOVER_FIELD_SIZE - j, &data[i], (len - i) );

  /* Update context var for fallback */
  *leftover_size = (uint32)(j + len - i);
} /* sechsharm_sha1_update() */


/*===========================================================================

FUNCTION SECHSHARM_SHA1_FINAL()

DESCRIPTION
  Do the final steps of a SHA1 hash, output the digest value, 
  clear the context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void sechsharm_sha1_final
(
  struct __sechsh_ctx_s* context,            /* context */
  uint8*                 leftover,           /* leftover input in context */
  uint32*                leftover_size,
  uint8*                 digest
)
{
  unsigned long i;
  unsigned char finalcount[8];
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  for ( i = 0; i < 8; i++ )
  {
    /* Note: This is endianess-independent */
    finalcount[i] = (unsigned char) ((context->counter[(i >= 4 ? 0 : 1)]
                                      >> ((3-(i & 3)) * 8) ) & 255);
  }
  
  {
    unsigned char data[] = { 0x80};

    sechsharm_sha1_update( context, leftover, leftover_size, data, sizeof( data ) );
  }
  
  while ( (context->counter[0] & 0x01f8) != 0x01c0 )
  {
    unsigned char data[] = { 0x00};
    
    sechsharm_sha1_update( context, leftover, leftover_size, data, sizeof( data ) );
  }
  
  sechsharm_sha1_update( context, leftover, leftover_size, finalcount, 8 );
  
  for ( i = 0; i < SECAPI_HSH_SHA_DIGEST_SIZE; i++ )
  {
    digest[i] = (unsigned char)
      ((context->iv[i>>2] >> ((3-(i & 3)) * 8) ) & 255);
  }
  
  /* Wipe variables */
  i = 0;
  
  //BM: ?????????????
  //our_memset( context, 0, sizeof( struct __sechsh_ctx_s) );


  our_memset( &finalcount, 0, sizeof( finalcount ) );
  
  sechsharm_sha1_transform( context->iv, /*context->*/leftover );

} /* sechsharm_sha1_final() */

#ifdef SHA2_UNROLL_TRANSFORM

/* Unrolled SHA-256 round macros: */

#define ROUND256_0_TO_15(a,b,c,d,e,f,g,h) do {				\
      BE_8_TO_32(W256[j], data);					\
      data += 4;							\
      T1 = (h) + Sigma1_256((e)) + Ch((e), (f), (g)) + K256[j] + W256[j]; \
      (d) += T1;							\
      (h) = T1 + Sigma0_256((a)) + Maj((a), (b), (c));			\
      j++;								\
   } while(0)

#define ROUND256(a,b,c,d,e,f,g,h) do {					\
      s0 = W256[(j+1)&0x0f];						\
      s0 = sigma0_256(s0);						\
      s1 = W256[(j+14)&0x0f];						\
      s1 = sigma1_256(s1);						\
      T1 = (h) + Sigma1_256((e)) + Ch((e), (f), (g)) + K256[j] +	\
	 (W256[j&0x0f] += s1 + W256[(j+9)&0x0f] + s0);			\
      (d) += T1;							\
      (h) = T1 + Sigma0_256((a)) + Maj((a), (b), (c));			\
      j++;								\
   } while(0)

static void sechsharm_sha256_transform
(
  uint32 state[8], 
  const uint8 data[SHA256_BLOCK_LENGTH]
)
{
   uint32	a, b, c, d, e, f, g, h, s0, s1;
   uint32	T1, W256[16];
   int		j;
   
   /* Initialize registers with the prev. intermediate value */
   a = state[0];
   b = state[1];
   c = state[2];
   d = state[3];
   e = state[4];
   f = state[5];
   g = state[6];
   h = state[7];
   
   j = 0;
   /* Klocwork: Filter. For the second iteration of the loop j 
    * is 15 for ROUND256_0_TO_15(b,c,d,e,f,g,h,a) and is then 
    * incremented to 16 at which time it breaks out of the loop.
    */
   do {
      /* Rounds 0 to 15 (unrolled): */
      ROUND256_0_TO_15(a,b,c,d,e,f,g,h);
      ROUND256_0_TO_15(h,a,b,c,d,e,f,g);
      ROUND256_0_TO_15(g,h,a,b,c,d,e,f);
      ROUND256_0_TO_15(f,g,h,a,b,c,d,e);
      ROUND256_0_TO_15(e,f,g,h,a,b,c,d);
      ROUND256_0_TO_15(d,e,f,g,h,a,b,c);
      ROUND256_0_TO_15(c,d,e,f,g,h,a,b);
      ROUND256_0_TO_15(b,c,d,e,f,g,h,a);
   } while (j < 16);
   
   /* Now for the remaining rounds up to 63: */
   /* Klocwork: Filter. The first time through the loop 
    * j is 16 for ROUND256(a,b,c,d,e,f,g,h) and 23 
    * for ROUND256(b,c,d,e,f,g,h,a) and the last time 
    * through the loop j is 63 for ROUND256(b,c,d,e,f,g,h,a) 
    * which will not overflow the array.  
    */
   do {
      ROUND256(a,b,c,d,e,f,g,h);
      ROUND256(h,a,b,c,d,e,f,g);
      ROUND256(g,h,a,b,c,d,e,f);
      ROUND256(f,g,h,a,b,c,d,e);
      ROUND256(e,f,g,h,a,b,c,d);
      ROUND256(d,e,f,g,h,a,b,c);
      ROUND256(c,d,e,f,g,h,a,b);
      ROUND256(b,c,d,e,f,g,h,a);
   } while (j < 64);
   
   /* Compute the current intermediate hash value */
   state[0] += a;
   state[1] += b;
   state[2] += c;
   state[3] += d;
   state[4] += e;
   state[5] += f;
   state[6] += g;
   state[7] += h;
   
   /* Clean up */
   a = b = c = d = e = f = g = h = T1 = 0;
}

#else /* SHA2_UNROLL_TRANSFORM */

static void sechsharm_sha256_transform
(
  uint32 state[8], 
  const uint8 data[SHA256_BLOCK_LENGTH]
)
{
   uint32	a, b, c, d, e, f, g, h, s0, s1;
   uint32	T1, T2, W256[16];
   int		j;
   
   /* Initialize registers with the prev. intermediate value */
   a = state[0];
   b = state[1];
   c = state[2];
   d = state[3];
   e = state[4];
   f = state[5];
   g = state[6];
   h = state[7];
   
   j = 0;
   do {
      BE_8_TO_32(W256[j], data);
      data += 4;
      /* Apply the SHA-256 compression function to update a..h */
      T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] + W256[j];
      T2 = Sigma0_256(a) + Maj(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + T1;
      d = c;
      c = b;
      b = a;
      a = T1 + T2;
      
      j++;
   } while (j < 16);
   
   do {
      /* Part of the message block expansion: */
      s0 = W256[(j+1)&0x0f];
      s0 = sigma0_256(s0);
      s1 = W256[(j+14)&0x0f];	
      s1 = sigma1_256(s1);
      
      /* Apply the SHA-256 compression function to update a..h */
      T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] + 
	 (W256[j&0x0f] += s1 + W256[(j+9)&0x0f] + s0);
      T2 = Sigma0_256(a) + Maj(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + T1;
      d = c;
      c = b;
      b = a;
      a = T1 + T2;
      
      j++;
   } while (j < 64);
   
   /* Compute the current intermediate hash value */
   state[0] += a;
   state[1] += b;
   state[2] += c;
   state[3] += d;
   state[4] += e;
   state[5] += f;
   state[6] += g;
   state[7] += h;
   
   /* Clean up */
   a = b = c = d = e = f = g = h = T1 = T2 = 0;
}

#endif // /* SHA2_UNROLL_TRANSFORM */

void sechsharm_sha256_init
(
  struct __sechsh_ctx_s* context             /* context */
)
{
   if (NULL == context)
      return;

  our_memset( context, 0, sizeof( struct __sechsh_ctx_s) );

  /* SHA256 initialization constants */   
   memscpy((uint8*)context->iv, sizeof(sha256_initial_hash_value), (uint8*)sha256_initial_hash_value, sizeof(sha256_initial_hash_value));
   
   context->counter[0] = 0;
   context->counter[1] = 0;
}

void sechsharm_sha256_update
(
  struct __sechsh_ctx_s* context,            /* context */
  uint8*                 leftover,           /* leftover input in context */
  uint32*                leftover_size,
  uint8*                 data, 
  uint32                 len
)
{
   uint32	freespace, usedspace;
   uint64    bitcounter;
   
   /* Calling with no data is valid (we do nothing) */
   if (len == 0)
      return;
   
   memscpy(&bitcounter, sizeof(bitcounter), context->counter, HSHSHA_COUNTER_SIZE);
   usedspace = ((uint32)bitcounter >> 3) % SHA256_BLOCK_LENGTH;
   if (usedspace > 0) {
      /* Calculate how much free space is available in the buffer */
      freespace = SHA256_BLOCK_LENGTH - usedspace;
      
      if (len >= freespace) {
	 /* Fill the buffer completely and process it */
	 memscpy((uint8*)&leftover[usedspace], CONTEXT_LEFTOVER_FIELD_SIZE - usedspace, data, freespace);
	 bitcounter += (uint32)(freespace << 3);
	 len -= freespace;
	 data += freespace;
	 sechsharm_sha256_transform(context->iv, leftover);
      } else {
	 /* The buffer is not yet full */
	 memscpy((uint8*)&leftover[usedspace], CONTEXT_LEFTOVER_FIELD_SIZE - usedspace, data, len);
	 bitcounter += (uint32)(len << 3);
	 /* Clean up: */
	 usedspace = freespace = 0;
         memscpy(context->counter, HSHSHA_COUNTER_SIZE, &bitcounter, HSHSHA_COUNTER_SIZE);
	 return;
      }
   }
   while (len >= SHA256_BLOCK_LENGTH) {
      /* Process as many complete blocks as we can */
      sechsharm_sha256_transform(context->iv, data);
      bitcounter += SHA256_BLOCK_LENGTH << 3;
      len -= SHA256_BLOCK_LENGTH;
      data += SHA256_BLOCK_LENGTH;
   }
   if (len > 0) {
      /* There's left-overs, so save 'em */
      memscpy((uint8*)leftover, CONTEXT_LEFTOVER_FIELD_SIZE, data, len);
      bitcounter += (uint32)(len << 3);
   }
   /* Clean up: */
   usedspace = freespace = 0;
   memscpy(context->counter, HSHSHA_COUNTER_SIZE, &bitcounter, HSHSHA_COUNTER_SIZE);
}


static void sechsharm_sha256_pad
(
  struct __sechsh_ctx_s* context,
  uint8*                leftover           /* leftover input in context */
)
{
   uint32 usedspace;
   uint64 bitcounter;
   
   memscpy(&bitcounter, sizeof(bitcounter), context->counter, 8);
   usedspace = (context->counter[0] >> 3) % SHA256_BLOCK_LENGTH;
   if (usedspace > 0) {
      /* Begin padding with a 1 bit: */
      leftover[usedspace++] = 0x80;
      
      if (usedspace <= SHA256_SHORT_BLOCK_LENGTH) {
	 /* Set-up for the last transform: */
	 our_memset(&leftover[usedspace], 0,
		SHA256_SHORT_BLOCK_LENGTH - usedspace);
      } else {
	 if (usedspace < SHA256_BLOCK_LENGTH) {
	    our_memset(&leftover[usedspace], 0,
		   SHA256_BLOCK_LENGTH - usedspace);
	 }
	 /* Do second-to-last transform: */
	 sechsharm_sha256_transform(context->iv, leftover);
	 
	 /* Prepare for last transform: */
	 our_memset(leftover, 0, SHA256_SHORT_BLOCK_LENGTH);
      }
   } else {
      /* Set-up for the last transform: */
      our_memset(leftover, 0, SHA256_SHORT_BLOCK_LENGTH);
      
      /* Begin padding with a 1 bit: */
      *leftover = 0x80;
   }
   /* Store the length of input data (in bits) in big endian format: */
    BE_64_TO_8(&leftover[SHA256_SHORT_BLOCK_LENGTH], bitcounter);
   
   /* Final transform: */
   sechsharm_sha256_transform(context->iv, leftover);
   
   memscpy(context->counter, HSHSHA_COUNTER_SIZE, &bitcounter, HSHSHA_COUNTER_SIZE);
   /* Clean up: */
   usedspace = 0;
}


void sechsharm_sha256_final
(
  struct __sechsh_ctx_s* context,            /* context */
  uint8*                 leftover,           /* leftover input in context */
  uint32*                leftover_size,
  uint8*                 digest
)
{
   sechsharm_sha256_pad(context, leftover);
   
   /* If no digest buffer is passed, we don't bother doing this: */
   if (digest != NULL) {
     int	i;
      
     /* Convert TO host byte order */
     for (i = 0; i < 8; i++)
	 BE_32_TO_8(digest + i * 4, context->iv[i]);

     /* zeroize context structure */
     //BM: ???
     //our_memset((uint8*)context, 0, sizeof(*context));

     sechsharm_sha256_transform( context->iv, /*context->*/leftover );
   }
}

#define HASH_DIGEST_BLOCK_SIZE 64
#define HASH_DIGEST_SIZE_SHA256 32

uint32 
sechsharmhmacSha256 (uint8                * keyPtr, 
                     uint32               keyLen, 
                     uint8*               inputPtr,
					 uint32               inputLen,
                     uint8*               outputPtr,
					 uint32               outputLen)
{
  struct __sechsh_ctx_s   context;     
  uint8            ipad[HASH_DIGEST_BLOCK_SIZE];
  uint8            opad[HASH_DIGEST_BLOCK_SIZE];
  uint8            key[HASH_DIGEST_BLOCK_SIZE];
  uint32           key_len = HASH_DIGEST_BLOCK_SIZE;
  uint32           hmac[HASH_DIGEST_SIZE_SHA256];
  uint32           hash_size = HASH_DIGEST_SIZE_SHA256;
  uint32           i = 0;

  if ((keyPtr == NULL) || (keyLen == 0) || (inputPtr == NULL) || (outputPtr == NULL) || (outputLen < HASH_DIGEST_SIZE_SHA256))
	  return 1;

  if (keyLen > HASH_DIGEST_BLOCK_SIZE)
  {

    sechsharm_sha256_init(&context);
    sechsharm_sha256_update( &context, context.leftover, &(context.leftover_size), keyPtr, (uint32) keyLen);
    sechsharm_sha256_final( &context, context.leftover, &(context.leftover_size), key);

    memset(key+hash_size, 0, (key_len-hash_size));
  }
  else
  {
    memscpy(key, sizeof(key), keyPtr, keyLen);
    if (key_len > keyLen)
        memset(key+keyLen, 0, (key_len-keyLen)); 
  }

  memset(ipad, 0, HASH_DIGEST_BLOCK_SIZE);
  memset(opad, 0, HASH_DIGEST_BLOCK_SIZE);
  memscpy(ipad, sizeof(ipad), key, key_len);
  memscpy(opad, sizeof(opad), key, key_len);

  for (i=0; i < HASH_DIGEST_BLOCK_SIZE; i++) 
  {
    ipad[i] ^= 0x36;
    opad[i] ^= 0x5C;
  }

  /* Inner Hash */
  sechsharm_sha256_init(&context);
  sechsharm_sha256_update( &context, context.leftover, &(context.leftover_size), ipad, (uint32) HASH_DIGEST_BLOCK_SIZE);
  sechsharm_sha256_update( &context, context.leftover, &(context.leftover_size), inputPtr, (uint32) inputLen);
  sechsharm_sha256_final( &context, context.leftover, &(context.leftover_size), (uint8*) hmac);

  /* Outer Hash */
  sechsharm_sha256_init(&context);
  sechsharm_sha256_update( &context, context.leftover, &(context.leftover_size), opad, (uint32) HASH_DIGEST_BLOCK_SIZE);
  sechsharm_sha256_update( &context, context.leftover, &(context.leftover_size), (uint8*)hmac, (uint32) hash_size);
  sechsharm_sha256_final( &context, context.leftover, &(context.leftover_size), (uint8*) hmac);

  memscpy(outputPtr, outputLen, hmac, HASH_DIGEST_SIZE_SHA256);

  return 0;
}

#ifdef FEATURE_BOOT_EXTERN_DEVICEPROGRAMMER_DDR_COMPLETED
void PerformSHA256HW(uint8* inputPtr, uint32 inputLen, uint8* outputPtr)
{
    CeMLCntxHandle*     cntx = NULL;
    CEMLIovecListType   ioVecIn;
    CEMLIovecListType   ioVecOut;
    CEMLIovecType       IovecIn;
    CEMLIovecType       IovecOut;
    uint32              digest_len = CEML_HASH_DIGEST_SIZE_SHA256;

    /* Input IOVEC */
    ioVecIn.size = 1;
    ioVecIn.iov = &IovecIn;
    ioVecIn.iov[0].dwLen = inputLen; //msg_len;
    ioVecIn.iov[0].pvBase = inputPtr; //msg; 

    /* Output IOVEC */
    ioVecOut.size = 1;
    ioVecOut.iov = &IovecOut;
    ioVecOut.iov[0].dwLen = digest_len;
    ioVecOut.iov[0].pvBase = outputPtr; //result_digest;


    CeMLInit();
    CeMLHashInit(&cntx, CEML_HASH_ALGO_SHA256);
    CeMLHashUpdate(cntx, ioVecIn);

    CeMLHashFinal(cntx, &ioVecOut);

    CeMLHashDeInit(&cntx);

    CeMLDeInit();
}
#else            
void PerformSHA256(uint8* inputPtr, uint32 inputLen, uint8* outputPtr)
{
  struct __sechsh_ctx_s   context;

  sechsharm_sha256_init  (&context);
  sechsharm_sha256_update(&context,
                        context.leftover,
                        &(context.leftover_size),
                        inputPtr,
                        inputLen);

  sechsharm_sha256_final (&context,
                        context.leftover,
                        &(context.leftover_size),
                        outputPtr);

}
#endif

extern boot_pbl_shared_data_type *pbl_shared_global;

__align(32)  secboot_image_info_type     image_info;
__align(32)  secboot_verified_info_type  verified_info;

boolean isAuthenticationEnabled(void)
{
    uint32 auth_enabled = 1;

    secboot_hw_ftbl_type *pbl_secboot_hw_ftbl_ptr = 
        &(pbl_shared_global->secboot_shared_data->pbl_secboot_hw_ftbl);
  
    secboot_ftbl_type *pbl_secboot_ftbl_ptr = 
        &(pbl_shared_global->secboot_shared_data->pbl_secboot_ftbl);
     
    if(pbl_secboot_hw_ftbl_ptr == NULL || pbl_secboot_ftbl_ptr == NULL) {
		logMessage("Returning FALSE, pbl_secboot_hw_ftbl_ptr == NULL || pbl_secboot_ftbl_ptr == NULL.");
        return FALSE;
    }

    if (E_SECBOOT_HW_SUCCESS ==
            pbl_secboot_hw_ftbl_ptr->secboot_hw_is_auth_enabled(
                SECBOOT_HW_APPS_CODE_SEGMENT, &auth_enabled)) {
        if (auth_enabled) {
            return TRUE;
        }
    }
    return FALSE;
}

boolean AuthenticateImage(byte* src_ptr)
{
    secboot_error_type sec_status = E_SECBOOT_SUCCESS;

    secboot_handle_type  secboot_handle;

    secboot_hw_ftbl_type *pbl_secboot_hw_ftbl_ptr = 
        &(pbl_shared_global->secboot_shared_data->pbl_secboot_hw_ftbl);
  
    secboot_ftbl_type *pbl_secboot_ftbl_ptr = 
        &(pbl_shared_global->secboot_shared_data->pbl_secboot_ftbl);
     
    if(pbl_secboot_hw_ftbl_ptr == NULL || pbl_secboot_ftbl_ptr == NULL) {
		logMessage("AuthenticateImage() Returning FALSE, pbl_secboot_hw_ftbl_ptr == NULL || pbl_secboot_ftbl_ptr == NULL.");
        return FALSE;
    }

    if (FALSE == isAuthenticationEnabled()) {
        // To be here means Secure Boot is not enabled on the HW
        // so no need to check the Digital Signature on this digest table
        // This is okay, this can be used to test VIP
		logMessage("AuthenticateImage() Authentication is NOT enabled, returning TRUE");
        return TRUE;
    }
 
    // To be this far means secure boot is enabled, and this
    // programmer must be signed. Therefore check the signature
    // of the digest table as well
  
    //memset(&verified_info, 0, sizeof(secboot_verified_info_type)); // change below for klocwork fix
    memset(&verified_info, 0, sizeof(verified_info));

    // Initializes the secboot handle and Crypto HW engine
    sec_status = pbl_secboot_ftbl_ptr->secboot_init(NULL, &secboot_handle); 
    if (E_SECBOOT_SUCCESS != sec_status)
	{
		logMessage("AuthenticateImage() Returning FALSE, pbl_secboot_ftbl_ptr->secboot_init(NULL, &secboot_handle) FAILED");
        return FALSE;
	}

	logMessage("Calling pbl_secboot_ftbl_ptr->secboot_authenticate with image_info.signature_len = 0x%X",image_info.signature_len);
	logMessage("Calling pbl_secboot_ftbl_ptr->secboot_authenticate with image_info.sw_type = 0x%X",image_info.sw_type);
	logMessage("Calling pbl_secboot_ftbl_ptr->secboot_authenticate with image_info.sw_version = 0x%X",image_info.sw_version);

    sec_status = pbl_secboot_ftbl_ptr->secboot_authenticate(&secboot_handle,
                                                    SECBOOT_HW_APPS_CODE_SEGMENT,
                                                    &image_info,
                                                    &verified_info);
    if (E_SECBOOT_SUCCESS != sec_status) {
		logMessage("AuthenticateImage() Returning FALSE, secboot_authenticate() returned 0x%X (not E_SECBOOT_SUCCESS=0x%X)",sec_status,E_SECBOOT_SUCCESS);
        pbl_secboot_ftbl_ptr->secboot_deinit(&secboot_handle);
        return FALSE;
    }

    // Deinitializes the secboot handle and Crypto HW engine
    pbl_secboot_ftbl_ptr->secboot_deinit(&secboot_handle);

    return TRUE;  
}

typedef struct
{
  uint32  codeword;            /* Codeword/magic number defining flash type
                                  information. */
  uint32  magic;               /* Magic number */
  uint32  RESERVED_0;          /* RESERVED */
  uint32  RESERVED_1;          /* RESERVED */
  uint32  RESERVED_2;          /* RESERVED */
  uint32  image_src;             /* Location of RPM_SBL in flash or e-hostdl in RAM. This is given in
                                  byte offset from beginning of flash/RAM.  */
  uint32 image_dest_ptr;        /* Pointer to location to store RPM_SBL/e-hostdl in RAM.
                                  Also, entry point at which execution begins.
                                  */
  uint32  image_size;      /* Size of RPM_SBL image in bytes */
  uint32  code_size;       /* Size of code region in RPM_SBL image in bytes */
  uint32  signature_ptr;         /* Pointer to images attestation signature */
  uint32  signature_size;        /* Size of the attestation signature in
                                  bytes */
  uint32  cert_chain_ptr;  /* Pointer to the chain of attestation
                                  certificates associated with the image. */
  uint32  cert_chain_size; /* Size of the attestation chain in bytes */

  uint32  oem_root_cert_sel;  /* Root certificate to use for authentication.
                                 Only used if SECURE_BOOT1 table_sel fuse is
                                 OEM_TABLE. 1 indicates the first root
                                 certificate in the chain should be used, etc */
  uint32  oem_num_root_certs; /* Number of root certificates in image.
                                 Only used if SECURE_BOOT1 table_sel fuse is
                                 OEM_TABLE. Denotes the number of certificates
                                 OEM has provisioned                          */
          
  uint32  RESERVED_5;          /* RESERVED */
  uint32  RESERVED_6;          /* RESERVED */
  uint32  RESERVED_7;          /* RESERVED */
  uint32  RESERVED_8;          /* RESERVED */
  uint32  RESERVED_9;          /* RESERVED */
} eighty_byte_header_type;

typedef struct
{
  uint32 image_id;       /* Identifies the type of image this header
                                 represents (OEM SBL, AMSS, Apps boot loader,
                                 etc.). */
  uint32 header_vsn_num;     /* Header version number. */
  uint32 image_src;          /* Location of image in flash: Address of
                                 image in NOR or page/sector offset to image
                                 from page/sector 0 in NAND/SUPERAND. */
  uint32 image_dest_ptr;     /* Pointer to location to store image in RAM.
                                 Also, entry point at which image execution
                                 begins. */
  uint32 image_size;         /* Size of complete image in bytes */
  uint32 code_size;          /* Size of code region of image in bytes */
  uint32 signature_ptr;      /* Pointer to images attestation signature */
  uint32 signature_size;     /* Size of the attestation signature in
                                 bytes */
  uint32 cert_chain_ptr;     /* Pointer to the chain of attestation
                                 certificates associated with the image. */
  uint32 cert_chain_size;    /* Size of the attestation chain in bytes */

} forty_byte_header_type;

boolean authenticateSignedHash(byte *buffer,
                               SIZE_T length,
                               byte** hash_position_address,
                               SIZE_T *signed_payload_size) {

    boolean retval;

    //uint32 sec_cert_chain_size = 6144;
    uint32 eighty_byte_header_magic = 0x73D71034;

    forty_byte_header_type *forty_byte_header_ptr = (forty_byte_header_type *) buffer;
    eighty_byte_header_type *eighty_byte_header_ptr = (eighty_byte_header_type *) buffer;


    if (length < sizeof(eighty_byte_header_type) || hash_position_address == NULL) {
		logMessage("ERROR: length < sizeof(eighty_byte_header_type) || hash_position_address == NULL");
        return FALSE;
    }

    memset(&image_info, 0, sizeof(secboot_image_info_type));
#ifdef FEATURE_QDST_SECBOOT
    image_info.sw_type = 0;
    image_info.sw_version = 0;
#endif
    //image_info.sw_version      = NumOfOneBitsIn(TZ_ANTI_REPLAY_FUSES);
    image_info.sw_type         = DEVICEPROGRAMMER_SW_ID; //Type identifier for image during signing
    image_info.header_ptr_1    = buffer;

    if (eighty_byte_header_ptr->magic == eighty_byte_header_magic) {
        image_info.header_len_1    = sizeof(eighty_byte_header_type);
        image_info.code_ptr_1      = &buffer[eighty_byte_header_ptr->image_src];
        image_info.code_len_1      = eighty_byte_header_ptr->code_size;
        image_info.x509_chain_ptr  = &buffer[eighty_byte_header_ptr->image_src + eighty_byte_header_ptr->cert_chain_ptr];
        image_info.x509_chain_len  = eighty_byte_header_ptr->cert_chain_size;
        image_info.signature_ptr   = &buffer[eighty_byte_header_ptr->image_src + eighty_byte_header_ptr->signature_ptr];
        image_info.signature_len   = eighty_byte_header_ptr->signature_size;
    }
    else {
        image_info.header_len_1    = sizeof(forty_byte_header_type);
        image_info.code_ptr_1      = &buffer[forty_byte_header_ptr->image_src];
        image_info.code_len_1      = forty_byte_header_ptr->code_size;
        image_info.x509_chain_ptr  = &buffer[forty_byte_header_ptr->image_src + forty_byte_header_ptr->cert_chain_ptr];
        image_info.x509_chain_len  = forty_byte_header_ptr->cert_chain_size;
        image_info.signature_ptr   = &buffer[forty_byte_header_ptr->image_src + forty_byte_header_ptr->signature_ptr];
        image_info.signature_len   = forty_byte_header_ptr->signature_size;
    }
 
	logMessage("Calling AuthenticateImage");
    retval = AuthenticateImage(buffer);
	logMessage("Back from AuthenticateImage, retval=0x%X",retval);
    if (retval == TRUE) {
        *hash_position_address = (byte *) image_info.code_ptr_1;
        *signed_payload_size = image_info.code_len_1;
    }
    return retval;
}

