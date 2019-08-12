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

#ifndef _RTXUTIL_H_
#define _RTXUTIL_H_

#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/rtxExternDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Utility functions */

EXTERNRT OSUINT32 rtxGetIdentByteCount (OSUINT32 ident);

EXTERNRT OSUINT32 rtxIntByteCount (OSINT32 val);

EXTERNRT OSUINT32 rtxOctetBitLen (OSOCTET w);

EXTERNRT OSUINT32 rtxUInt32BitLen (OSUINT32 w);

/**
 * This function retrieves the binary logarithm of the given value
 * (by excess).
 *
 * @param w     Word value for which to get binary logarithm.
 * @return      The smallest x such as w <= 2^x
 */
EXTERNRT OSUINT32 rtxLog2Ceil (OSUINT32 w);

/**
 * This function retrieves the binary logarithm of the given value
 * (by default).
 *
 * @param w     Word value for which to get binary logarithm.
 * @return      The smallest x such as 2^x <= w < 2^(x+1)
 */
EXTERNRT OSUINT32 rtxLog2Floor (OSUINT32 w);

#if !defined(_NO_INT64_SUPPORT)
EXTERNRT OSUINT32 rtxGetIdent64ByteCount (OSUINT64 ident);
EXTERNRT OSUINT32 rtxUInt64BitLen (OSUINT64 w);
#endif

#ifdef __cplusplus
}
#endif

#endif
