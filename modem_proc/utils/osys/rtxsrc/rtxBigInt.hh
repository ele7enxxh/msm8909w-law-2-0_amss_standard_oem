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
 * @file rtxBigInt.hh
 */

#ifndef _RTXBIGINT_HH_
#define _RTXBIGINT_HH_

#include <stdlib.h>
#include "rtxsrc/rtxBigInt.h"
#include "rtxsrc/rtxErrCodes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UNIT_MASK 0xFF

#define ADDRESS_BITS       3 /* as far as char has 8 bits (2^3) */

#define BITS_PER_UNIT (1 << ADDRESS_BITS)
#define BIT_INDEX_MASK  (BITS_PER_UNIT - 1)

#define bitsLeftOf(x) \
   (x==0 ? UNIT_MASK : (UNIT_MASK << (BITS_PER_UNIT - x)))

#define LONG_MASK 0xFF

// extern unsigned short bitsPerDigit[];

// extern short digitsPerByte[];

// extern short byteRadix[];

/* BigInt functions */
void rtxBigIntDestructiveMulAdd(OSOCTET* x, int len, int y, int z);
void rtxBigIntRemoveLeadingZeroBytes (OSBigInt* pInt);
int rtxBigIntCopyAndInverse
   (OSCTXT* pCtxt, const OSBigInt* pSrc, OSBigInt* pDst);
int rtxBigIntDivideByInt (OSCTXT* pCtxt, OSBigInt* pDivident,
   int divisor, OSBigInt* pQuotient, int* reminder);
int rtxBigIntNegate (OSBigInt* pInt);

#ifdef __cplusplus
}
#endif

#endif /* RTXBIGINT */

