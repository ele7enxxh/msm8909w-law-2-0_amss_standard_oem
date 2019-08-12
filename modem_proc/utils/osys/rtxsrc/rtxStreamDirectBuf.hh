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
/*
 * @file rtxStreamDirectBuf.h
 */
#ifndef _RTXSTREAMDIRECTBUF_H_
#define _RTXSTREAMDIRECTBUF_H_

#ifndef _NO_STREAM

#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/rtxExternDefs.h"
#include "rtxsrc/rtxStream.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This function initializes a direct-buffered stream. This function should be called
 * first before any operation with a buffered stream.
 *
 * @param pctxt        Pointer to context structure variable.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxStreamDirectBufCreate (OSCTXT* pctxt);

/*
 * This function pre-reads a 'size' number of octets of data from the input
 * stream into the internal stream buffer. This functions blocks until all
 * necessary data is read, end of file is detected, or another error
 * occurs. This function guarantees that the internal buffer will contain at
 * least the 'size' number of octets. Thus, it is possible to obtain direct
 * access to the memory buffer with pre-read data by using the 'buffer' field
 * of the OSCTXT context block.
 *
 * @param pctxt        Pointer to a context structure variable that has
 *                       been initialized for stream operations.
 * @param size         Number of octets to be pre-read.
 * @return             Completion status of operation: 0 (0) = success,
 *                       negative return value is error.
 */
EXTERNRT int rtxStreamDirectBufPreRead (OSCTXT* pctxt, size_t size);

/*
 * This function checks is the stream already direct-buffered or not.
 * If not, the direct-buffered stream will be created above the existing stream.
 * If stream is not opened at all, error code is returned.
 */
EXTERNRT int rtxStreamDirectBufCheck (OSCTXT* pctxt);

#ifdef __cplusplus
}
#endif

#endif /* _NO_STREAM */
#endif /* _RTXSTREAMDIRECTBUF_H_ */

