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
 * @file rtxBuffer.h
 * Common runtime functions for reading from or writing to the message
 * buffer defined within the context structure.
 */
#ifndef _RTXBUFFER_H_
#define _RTXBUFFER_H_

#include "rtxsrc/rtxContext.h"

/* reserve octets to enable short backoffs */
#define MIN_STREAM_BACKOFF 1

#ifdef __cplusplus
extern "C" {
#endif

EXTERNRT int rtxCheckBuffer (OSCTXT* pctxt, size_t nbytes);

EXTERNRT int rtxCheckOutputBuffer (OSCTXT* pctxt, size_t nbytes);

EXTERNRT int rtxCheckInputBuffer (OSCTXT* pctxt, size_t nbytes);

EXTERNRT int rtxCopyAsciiText (OSCTXT* pctxt, const char* text);

EXTERNRT int rtxCopyUTF8Text (OSCTXT* pctxt, const OSUTF8CHAR* text);

EXTERNRT int rtxCopyUnicodeText (OSCTXT* pctxt, const OSUNICHAR* text);

/**
 * This function safely reads bytes from the currently open stream or
 * memory buffer into the given static buffer.  This function is preferred
 * over \c rtxReadBytes because it will detect buffer overflow.
 *
 * @param pctxt         Pointer to a context structure.
 * @param buffer        Static buffer into which bytes are to be read.
 * @param bufsize       Size of the static buffer.
 * @param nocts         Number of bytes (octets) to read.
 * @return              Status of the operation: 0 if success,
 *                        negative value if error.
 */
EXTERNRT int rtxReadBytesSafe
(OSCTXT* pctxt, OSOCTET* buffer, size_t bufsize, size_t nocts);

/**
 * This function reads bytes from the currently open stream or memory buffer.
 *
 * @param pctxt         Pointer to a context structure.
 * @param pdata         Pointer to byte array where bytes are to be copied.
 * @param nocts         Number of bytes (octets) to read.
 * @return              Status of the operation: 0 if success,
 *                        negative value if error.
 */
EXTERNRT int rtxReadBytes (OSCTXT* pctxt, OSOCTET* pdata, size_t nocts);

/**
 * This function reads bytes from the currently open stream or memory buffer.
 * In this case the function MAY allocate memory to hold the read bytes.  It
 * will only do this if the requested number of bytes will not fit in the
 * context buffer; othwerwise, a pointer to a location in the context buffer
 * is returned.  If memory was allocated, it should be freed using
 * rtxMemFreePtr.
 *
 * @param pctxt         Pointer to a context structure.
 * @param ppdata        Pointer to byte buffer pointer.
 * @param nocts         Number of bytes (octets) to read.
 * @param pMemAlloc     Pointer to boolean value which is set to true if
 *                        memory was allocated to hold requested bytes.
 * @return              Status of the operation: 0 if success,
 *                        negative value if error.
 */
EXTERNRT int rtxReadBytesDynamic
(OSCTXT* pctxt, OSOCTET** ppdata, size_t nocts, OSBOOL* pMemAlloc);

/**
 * This function writes bytes to the currently open stream or memory buffer.
 *
 * @param pctxt         Pointer to a context structure.
 * @param pdata         Pointer to location where bytes are to be copied.
 * @param nocts         Number of bytes to read.
 * @return              I/O byte count.
 */
EXTERNRT int rtxWriteBytes
(OSCTXT* pctxt, const OSOCTET* pdata, size_t nocts);

/* Macros */

#define OSRTPUTCHAR(pctxt,ch) rtxWriteBytes (pctxt, (OSOCTET*)&ch, 1)

#define OSRTPUTCHARREV(pctxt,ch) \
(pctxt)->buffer.data[--(pctxt)->buffer.byteIndex]=(OSOCTET)ch;

#define OSRTZTERM(pctxt) \
(pctxt)->buffer.data[(pctxt)->buffer.byteIndex]=(OSOCTET)0;

#define OSRTSAFEZTERM(pctxt) \
do { \
if (rtxCheckOutputBuffer (pctxt, 1) == 0) \
(pctxt)->buffer.data[(pctxt)->buffer.byteIndex]=(OSOCTET)0; \
else return LOG_RTERRNEW (pctxt, RTERR_BUFOVFLW); \
} while (0)

#define OSRTSAFEPUTCHAR(pctxt,ch) \
do { \
if (rtxCheckOutputBuffer (pctxt, 1) == 0) \
(pctxt)->lastChar= \
(pctxt)->buffer.data[(pctxt)->buffer.byteIndex++]=(OSOCTET)ch; \
else return LOG_RTERRNEW (pctxt, RTERR_BUFOVFLW); \
} while (0)

#define OSRTSAFEPUTCHAR1(pctxt,ch,stat) \
do { \
OSOCTET b = (OSOCTET)ch; \
rtxWriteBytes (pctxt, &b, 1); \
} while (0)

#if 0
#define OSRTSAFEPUTCHAR2(pctxt,ch,prealloc) \
do { \
if (rtxCheckOutputBuffer (pctxt, ((prealloc > 1)?prealloc:1)) == 0) \
(pctxt)->lastChar= \
(pctxt)->buffer.data[(pctxt)->buffer.byteIndex++]=(OSOCTET)ch; \
else return LOG_RTERRNEW (pctxt, RTERR_BUFOVFLW); \
} while (0)
#endif

#define OSRTMEMCPY(pctxt,bdata,len) \
do { \
OSCRTLMEMCPY (&(pctxt)->buffer.data[(pctxt)->buffer.byteIndex], bdata, len); \
(pctxt)->buffer.byteIndex += len; \
(pctxt)->lastChar = (pctxt)->buffer.data[(pctxt)->buffer.byteIndex-1]; \
} while (0)

#define OSRTMEMCPYREV(pctxt,bdata,len) \
do { \
(pctxt)->buffer.byteIndex -= len; \
OSCRTLMEMCPY (&(pctxt)->buffer.data[(pctxt)->buffer.byteIndex], bdata, len); \
} while (0)

#define OSRTSAFEMEMCPY(pctxt,bdata,len) \
do { \
if (rtxCheckOutputBuffer (pctxt, len) == 0) { \
OSCRTLMEMCPY (&(pctxt)->buffer.data[(pctxt)->buffer.byteIndex], bdata, len); \
(pctxt)->buffer.byteIndex += len; \
(pctxt)->lastChar = (pctxt)->buffer.data[(pctxt)->buffer.byteIndex-1]; } \
else return LOG_RTERRNEW (pctxt, RTERR_BUFOVFLW); \
} while (0)

#define OSRTSAFEMEMCPY1(pctxt,bdata,len,stat) \
do { \
if (rtxCheckOutputBuffer (pctxt, len) == 0) { \
OSCRTLMEMCPY (&(pctxt)->buffer.data[(pctxt)->buffer.byteIndex], bdata, len); \
(pctxt)->buffer.byteIndex += len; \
(pctxt)->lastChar = (pctxt)->buffer.data[(pctxt)->buffer.byteIndex-1]; \
stat = 0; } \
else stat = RTERR_BUFOVFLW; \
} while (0)

#define OSRTGETBUFUTF8LEN(pctxt) \
rtxCalcUTF8Len (OSRTBUFPTR (pctxt), OSRTBUFSIZE (pctxt))

#define OSRTCHKBUFUTF8LEN(pctxt,lower,upper,stat) \
do { size_t nchars = OSRTGETBUFUTF8LEN (pctxt); \
stat = (nchars >= lower && nchars <= upper) ? 0 : RTERR_CONSVIO; } while(0)

#define OSRTENDOFBUF(pctxt) ((pctxt)->buffer.byteIndex >= (pctxt)->buffer.size)

#ifdef __cplusplus
}
#endif

#endif
