/*
 * Copyright (c) 2003-2013 Objective Systems, Inc.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by Objective Systems, Inc.
 *
 * PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 *****************************************************************************/
/**
 * @file rtxBitString.h - Contains utility functions for setting, clearing,
 * and testing bits at any position in an arbitrarily sized array of bytes.
 */
#ifndef _RTXBITSTRING_H_
#define _RTXBITSTRING_H_

#include "rtxsrc/rtxContext.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup bitstrhelpers Bit String Functions
 * @{
 * Bit string functions allow bits to be set, cleared, or tested in
 * arbitrarily sized byte arrays.
 */
/**
 * This macro is used to calculate the byte array size required to hold
 * the given number of bits.
 */
#define OSRTBYTEARRAYSIZE(numbits) ((numbits+7)/8)

/**
 * This function returns the minimum size of the bit field required to
 * hold the given integer value.
 *
 * @param value Integer value
 * @return Minimum size of the the field in bits required to hold value.
 */
EXTERNRT OSUINT32 rtxGetBitCount (OSUINT32 value);

/**
 * This function sets the specified zero-counted bit in the bit string.
 *
 * @param pBits        Pointer to octets of bit string.
 * @param numbits      Number of bits in the bit string.
 * @param bitIndex     Index of bit to be set. The bit with index 0 is a most
 *                       significant bit in the octet with index 0.
 * @return             If successful, returns the previous state of bit. If bit
 *                       was set the return value is positive, if bit was not
 *                       set the return value is zero. Otherwise, return value
 *                       is an error code:
 *                       - RTERR_OUTOFBND = bitIndex is out of bounds
 */
EXTERNRT int rtxSetBit (OSOCTET* pBits, OSSIZE numbits, OSSIZE bitIndex);

/**
 * This function sets one or more bits to TRUE or FALSE in a 32-bit unsigned
 * bit flag set.
 *
 * @param flags        Flags to which mask will be applied.
 * @param mask         Mask with one or more bits set that will be applied
 *                       to pBitMask.
 * @param action       Boolean action indicating if bits in flags should be
 *                       set (TRUE) or cleared (FALSE).
 * @return             Updated flags after mask is applied.
 */
EXTERNRT OSUINT32 rtxSetBitFlags
(OSUINT32 flags, OSUINT32 mask, OSBOOL action);

/**
 * This function clears the specified zero-counted bit in the bit string.
 *
 * @param pBits        Pointer to octets of bit string.
 * @param numbits      Number of bits in the bit string.
 * @param bitIndex     Index of bit to be cleared. The bit with index 0 is a
 *                       most significant bit in the octet with index 0.
 * @return             If successful, returns the previous state of bit. If bit
 *                       was set the return value is positive, if bit was not
 *                       set the return value is zero. Otherwise, return value
 *                       is an error code:
 *                       - RTERR_OUTOFBND = bitIndex is out of bounds
 */
EXTERNRT int rtxClearBit
(OSOCTET* pBits, OSSIZE numbits, OSSIZE bitIndex);

/**
 * This function tests the specified zero-counted bit in the bit string.
 *
 * @param pBits        Pointer to octets of bit string.
 * @param numbits      Number of bits in the bit string.
 * @param bitIndex     Index of bit to be tested. The bit with index 0 is a
 *                       most significant bit in the octet with index 0.
 * @return             True if bit set or false if not set or array index is
 *                       beyond range of number of bits in the string.
 */
EXTERNRT OSBOOL rtxTestBit
(const OSOCTET* pBits, OSSIZE numbits, OSSIZE bitIndex);

/**
 * This function returns the zero-counted index of the last bit set in a bit
 * string.
 *
 * @param pBits       Pointer to the octets of the bit string.
 * @param numbits     The number of bits in the bit string.
 *
 * @return            Index of the last bit set in the bit string.
 */
EXTERNRT OSSIZE rtxLastBitSet (const OSOCTET *pBits, OSSIZE numbits);

/**
 * Check whether the given bit string is large enough, and expand it if
 * necessary.
 *
 * @param pctxt The context to use should memory need to be allocated.
 * @param ppBits *ppBits is a pointer to the bit string, or NULL if one has not
 *    been created.  If the string is expanded, *ppBits receives a pointer to
 *    the new bit string.
 * @param pNumocts pNumocts points to the current size of the bit string in
 *    octets.  If the bit string is expanded, *pNumocts receives the new size.
 * @param minRequiredBits The minimum number of bits needed in the bit string.
 *    On return, pBits will point to a bit string with at least this many bits.
 * @param preferredLimitBits The number of bits over which we prefer not to go.
 *    If nonzero, no more bytes will be allocated than necessary for this many
 *    bits, unless explicitly required by minRequiredBits.
 * @return  If successful, 0.  Otherwise, an error code.
 */
EXTERNRT int rtxCheckBitBounds(OSCTXT* pctxt, OSOCTET** ppBits,
   OSSIZE* pNumocts, OSSIZE minRequiredBits, OSSIZE preferredLimitBits);

/**
 * @}bitstrhelpers
 */
#ifdef __cplusplus
}
#endif

#endif
