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
 * @file rtxStreamBuffered.h
 */
#ifndef _RTXSTREAMBUFFERED_H_
#define _RTXSTREAMBUFFERED_H_

#ifndef _NO_STREAM

#include "rtxsrc/rtxStream.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OSRTSTRMCM_RESTORE_UNDERLAYING_AFTER_RESET 0x0001

EXTERNRT int rtxStreamBufferedCreate (OSCTXT* pctxt, OSUINT32 mode);

EXTERNRT int rtxStreamBufferedRelease (OSCTXT* pctxt);

EXTERNRT int rtxStreamBufferedSetPreReadBuf
   (OSCTXT* pctxt, const OSOCTET* pdata, size_t datalen);

EXTERNRT int rtxStreamBufferedPrependReadBuf
   (OSCTXT* pctxt, const OSOCTET* pdata, size_t datalen);

#ifdef __cplusplus
}
#endif

#endif /* _NO_STREAM */
#endif /* _RTXSTREAMBUFFERED_H_ */

