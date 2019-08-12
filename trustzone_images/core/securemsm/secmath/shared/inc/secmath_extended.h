#ifndef SECMATH_EXTENDED_H
#define SECMATH_EXTENDED_H

/**
  @file secmath_extended.h

  @brief Allows access to additional internal secmath functions that not accessible through secmath.
*/

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/secmath/shared/inc/secmath_extended.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
10/07/12    bd      Initial Revision

===========================================================================*/

#if defined(__CC_ARM) || defined(_MSC_VER)
  #define restrict 
#endif

#include "secmath.h"

/* Montgomery mutliplication z = a * b * R^-1 mod m, where R = r^N and r = 2^32 */
/* a and b can overlap.  z will be overwritten                                  */
/* m0_1 = - m[0]^(-1) mod r                                                     */
/* This function implements Algorithm 14.36, Handbook of Applied Cryptography   */
/* this code meant to be simple, have small data and code foot print and yet be */
/* efficient for RSA signature veriffication, in particular exp=3               */
int montmul_ref(uint32 * z, const uint32 *a, const uint32 *b, const uint32 *m, const size_t N, uint32 m0_1);

//---------------------------------------------------------------------------------------------
// Montgomery multiply a by one, return result in z. montmul_ref() may be used as well
// but for exp=3 signature verification, this will be much faster 
// This function is used to get out of the Montgomery domain.
int montmul_one_ref(uint32 * restrict z, const uint32 *a, const uint32 *m, size_t N, uint32 m0_1);

// x must be odd
// Bijan should put more detailed explanation 
uint32 inverse32(uint32 x);

//computes a*r mod m, where r=2^32 and a is a big integer of length N
//a and z can overlap. that is, mult and reduction can be done inplace
int mult_base_mod_m(uint32 *z, const uint32 *m, const uint32 *a, uint32 qi, size_t N);

//R = base ^N and base=2^32  
//WARNING: i- a must be smaller than m, otherwise this functions returns wrong result
//         ii- msb of m must be one, if it is zero then the next bit must be one, or the function returns wrong result
// caller must make sure a<m, or this function wont work
int mult_R_mod_m(uint32 *z, const uint32 *a, const uint32 *m, const size_t N);

/* mod_exp does modular exponentiation.  base, exponent, modulus, and
 * result are all big nums with the bytes in native order within words
 * and the words in little-endian order.  N is the number of 32-bit
 * words in base, modulus and result. expN is the number of 32-bit
 * words in the exponent.  work is array with pointers to 4 N-word
 * bignums used for temporary storage of values.  The result and the
 * values in work must not alias any other arguments.
 *
 * base^0 is defined to be 1.
 * 
 * Return value: Returns 0 on success.  Returns -1 on any detected
 * error.  
 */
int mod_exp(uint32 *restrict result, const uint32 *base, 
            const uint32 *exponent, const uint32 *modulus,
            const size_t N, const size_t expN, uint32 *restrict work[4]);

int32 addition(uint32 *z, const uint32 *a, const uint32 *b, size_t N);

#endif
