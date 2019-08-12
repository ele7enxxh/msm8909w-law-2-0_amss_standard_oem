#ifndef SECMATH_H
#define SECMATH_H

/**
  @file secmath.h

  @brief This header file defines the interface to all crypto math
         operations.

   The approach to crypto math operations can be broken into two types: provide
   big integer operations and let the caller construct the operations they need
   from such primitives, or make a crypto math library that is tuned for the type
   of operations that the crypto library is performing. This library is performing
   the latter
*/

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secmath/api/secmath.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who      what, where, why
--------   ---      ------------------------------------
05/11/11   vg             Initial Revision

===========================================================================*/

#include "comdef.h"


/**
 * @addtogroup SecMath
 * @{
 */
#ifdef __cplusplus
extern "C"
{
#endif

/* version */
#define E_SECMATH   0x0001
#define S_SECMATH   "0.01"

/* error codes */
typedef enum
{
  E_SECMATH_SUCCESS = 0,         ///< Result OK
  E_SECMATH_FAILURE,             ///< Generic Error */
  E_SECMATH_NOP,                 ///< Not a failure but no operation was performed */
  E_SECMATH_FAIL_TESTVECTOR,     ///< Algorithm failed test vectors */
  E_SECMATH_BUFFER_OVERFLOW,     ///< Not enough space for output */
  E_SECMATH_MEM,                 ///< Out of memory */
  E_SECMATH_INVALID_ARG,         ///< Generic invalid argument */
  E_SECMATH_INVALID_PRIME_SIZE,  ///< Invalid size of prime requested */
  E_SECMATH_NOT_SUPPORTED        ///< Operation not supported
} SECMATH_ERRNO_ET;

/** Common input/output type */
typedef struct common_in_out_s
{
  uint8 *ptr;
  uint32 len;
} common_in_out_t;


typedef uint32 BLONG;
#define MAX_KEY_SIZE_IN_BITS 4096                  ///< Maximum key size in bits
#define BLONG_SIZE sizeof(BLONG)                   ///< Bytes per digit
#define BN_BITS_PER_WORD (BLONG_SIZE * 8)          ///< Bit per digit
#define BLONGS_PER_KEY  (MAX_KEY_SIZE_IN_BITS + 8*BLONG_SIZE - 1)/(8*BLONG_SIZE)  ///< Digits per key
#define BYTES_TO_BLONG_SHIFT 2                     ///< bytes/sizeof(BLONG)

#define BLONG_MAX_ELEMENTS BLONGS_PER_KEY

typedef struct
{
  BLONG a[BLONGS_PER_KEY];             ///< Make room for multiplication
  int n;                               ///< Current length of a
} BigInt;


/**
 * @brief
 *  Read an unsigned buffer of bytes into a big integer
 */
SECMATH_ERRNO_ET secmath_BIGINT_read_unsigned_bin(BigInt * a, const uint8 * buf,
                                                  uint32 len);

 /**
 * @brief
 *  Read a zero terminated string into a big integer
 */
SECMATH_ERRNO_ET secmath_BIGINT_read_radix(BigInt * a, const char *str,
                                           uint32 radix);

/**
 * @brief
 *  Write a big integer into NULL terminate string
 */
SECMATH_ERRNO_ET secmath_BIGINT_to_radix(const BigInt * a, char *str, uint32 *len,
                                         uint32 radix);

/**
 * @brief
 *  Write a big integer into a buffer
 */
SECMATH_ERRNO_ET secmath_BIGINT_to_unsigned_bin(const BigInt * a, uint8 * buf,
                                                uint32 size);

/**
 * @brief
 *  r = b^e mod m
 */
SECMATH_ERRNO_ET secmath_BIGINT_modexp(BigInt * r, BigInt * b,
                                       BigInt * e, BigInt * m);

#ifdef __cplusplus
}
#endif

/// @}

#endif
