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
 * @file rtxMemBuf.h
 */
/**
 * @defgroup buffermanfun Memory Buffer Management Functions
 * @{
 *
 * Memory buffer management functions handle the allocation, expansion, and
 * deallocation of dynamic memory buffers used by some encode/decode functions.
 * Dynamic memory buffers are buffers that can grow or shrink to hold variable
 * sized amounts of data. This group of functions allows data to be appended to
 * buffers, to be set within buffers, and to be retrieved from buffers.
 * Currently, these functions are used within the generated SAX decode routines
 * to collect data as it is parsed by an XML parser.
 */

/* memory buffer */

#ifndef _RTXMEMBUF_H_
#define _RTXMEMBUF_H_

#include "rtxsrc/rtxContext.h"

typedef struct OSRTMEMBUF {
   OSCTXT*      pctxt;
   OSSIZE       segsize;        /* segment size                     */
   OSSIZE       startidx;       /* start index of useful info       */
   OSSIZE       usedcnt;        /* used byte count                  */
   OSSIZE       bufsize;        /* buffer size                      */
   OSSIZE       bitOffset;      /* bit offset                       */
   OSUINT32     userState;      /* user state - any value, 0 is initial */
   OSOCTET*     buffer;         /* memory buffer                    */
   OSBOOL       isDynamic;      /* is buffer allocated dynamically? */
   OSBOOL       isExpandable;   /* is buffer expandable?            */
   OSBOOL       useSysMem;      /* use system memory management     */
} OSRTMEMBUF;

#define OSMBDFLTSEGSIZE 1024

#define OSMEMBUFPTR(pmb)    ((pmb)->buffer + (pmb)->startidx)
#define OSMEMBUFENDPTR(pmb) ((pmb)->buffer + (pmb)->startidx + (pmb)->usedcnt)
#define OSMEMBUFUSEDSIZE(pmb) ((OSSIZE)(pmb)->usedcnt)

#define OSMBAPPENDSTR(pmb,str) if (0 != str) \
rtxMemBufAppend(pmb,(OSOCTET*)str,OSCRTLSTRLEN(str))

#define OSMBAPPENDSTRL(pmb,str) \
rtxMemBufAppend(pmb,(OSOCTET*)str,OSCRTLSTRLEN(str))

#define OSMBAPPENDUTF8(pmb,str) if (0 != str) \
rtxMemBufAppend(pmb,(OSOCTET*)str,rtxUTF8LenBytes(str))

#ifdef __cplusplus
extern "C" {
#endif

/* Memory buffer functions */

/**
 * This function appends the data to the end of a memory buffer. If the buffer
 * was dynamic and full then the buffer will be reallocated. If it is static
 * (the static buffer was assigned by a call to rtxMemBufInitBuffer) or it is
 * empty (no memory previously allocated) then a new buffer will be allocated.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @param pdata        The pointer to the buffer to be appended. The data will
 *                       be copied at the end of the memory buffer.
 * @param nbytes       The number of bytes to be copied from pData.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxMemBufAppend
(OSRTMEMBUF* pMemBuf, const OSOCTET* pdata, OSSIZE nbytes);

/**
 * This function cuts off the part of memory buffer. The beginning of the
 * cutting area is specified by offset "fromOffset" and the length is specified
 * by "nbytes". All data in this part will be lost. The data from the offset
 * "fromOffset + nbytes" will be moved to "fromOffset" offset.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @param fromOffset   The offset of the beginning part, being cut off.
 * @param nbytes       The number of bytes to be cut off from the memory
 *                       buffer.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxMemBufCut
(OSRTMEMBUF* pMemBuf, OSSIZE fromOffset, OSSIZE nbytes);

/**
 * This function frees the memory buffer. If memory was allocated then it will
 * be freed. Do not use the memory buffer structure after this function is
 * called.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 */
EXTERNRT void rtxMemBufFree (OSRTMEMBUF* pMemBuf);

/**
 * This function returns the pointer to the used part of a memory buffer.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @param length       The pointer to the length of the used part of the memory
 *                       buffer.
 * @return             The pointer to the used part of the memory buffer.
 */
EXTERNRT OSOCTET* rtxMemBufGetData (const OSRTMEMBUF* pMemBuf, int* length);

/**
 * This function returns the pointer to the used part of a memory buffer.
 * The extended version returns length in a size-typed argument which is
 * a 64-bit value on many systems.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @param length       The pointer to the length of the used part of the memory
 *                       buffer.
 * @return             The pointer to the used part of the memory buffer.
 */
EXTERNRT OSOCTET* rtxMemBufGetDataExt
(const OSRTMEMBUF* pMemBuf, OSSIZE* length);

/**
 * This function returns the length of the used part of a memory buffer.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @return             The length of the used part of the buffer.
 */
EXTERNRT OSSIZE rtxMemBufGetDataLen (const OSRTMEMBUF* pMemBuf);

/**
 * This function initializes a memory buffer structure. It does not allocate
 * memory; it sets the fields of the structure to the proper states. This
 * function must be called before any operations with the memory buffer.
 *
 * @param pCtxt        A provides a storage area for the function to store all
 *                       working variables that must be maintained between
 *                       function calls.
 * @param pMemBuf      A pointer to the initialized memory buffer structure.
 * @param segsize      The number of bytes in which the memory buffer will be
 *                       expanded incase it is full.
 */
EXTERNRT void rtxMemBufInit
(OSCTXT* pCtxt, OSRTMEMBUF* pMemBuf, OSSIZE segsize);

/**
 * This function assigns a static buffer to the memory buffer structure. It
 * does not allocate memory; it sets the pointer to the passed buffer. If
 * additional memory is required (for example, additional data is appended to
 * the buffer using rtxMemBufAppend), a dynamic buffer will be allocated and
 * all data copied to the new buffer.
 *
 * @param pCtxt        A pointer to a context structure. This provides a
 *                       storage area for the function t store all working
 *                       variables that must be maintained between function
 *                       calls.
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @param buf          A pointer to the buffer to be assigned.
 * @param bufsize      The size of the buffer.
 * @param segsize      The number of bytes on which the memory buffer will be
 *                       expanded in case it is full.
 */
EXTERNRT void rtxMemBufInitBuffer (OSCTXT* pCtxt, OSRTMEMBUF* pMemBuf,
                                    OSOCTET* buf, OSSIZE bufsize,
                                    OSSIZE segsize);

/**
 * This function allocates a buffer with a predetermined amount of space.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @param nbytes       The number of bytes to be copied from pData.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxMemBufPreAllocate (OSRTMEMBUF* pMemBuf, OSSIZE nbytes);

/**
 * This function resets the memory buffer structure. It does not free memory,
 * just sets the pointer to the beginning and the used length to zero.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 */
EXTERNRT void rtxMemBufReset (OSRTMEMBUF* pMemBuf);

/**
 * This function sets part of a memory buffer to a specified octet value. The
 * filling is started from the end of the memory buffer. If the buffer is
 * dynamic and full, then the buffer will be reallocated. If it is static (a
 * static buffer was assigned by a call to rtxMemBufInitBuffer) or it is empty
 * (no memory previously was allocated) then a new buffer will be allocated.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @param value        The pointer to the buffer to be appended. The data will
 *                       be copied at the end of the memory buffer.
 * @param nbytes       The number of bytes to be copied from pData.
 * @return             Completion status of operation:
 *                       - 0 = success,
 *                       - negative return value is error.
 */
EXTERNRT int rtxMemBufSet (OSRTMEMBUF* pMemBuf, OSOCTET value, OSSIZE nbytes);

/**
 * This function sets "isExpandable" flag for the memory buffer object.
 * By default, this flag is set to TRUE, thus, memory buffer could be
 * expanded, even if it was initialized by static buffer (see
 * \c rtMemBufInitBuffer). If flag is cleared and buffer is full the
 * rtMemBufAppend/rtMemBufPreAllocate functions will return error status.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @param isExpandable TRUE, if buffer should be expandable.
 * @return             Previous state of "isExpandable" flag.
 */
EXTERNRT OSBOOL rtxMemBufSetExpandable
(OSRTMEMBUF* pMemBuf, OSBOOL isExpandable);

/**
 * This function sets a flag to indicate that system memory management
 * should be used instead of the custom memory manager.  This should
 * be used if the allocated buffer must be preserved after calls
 * to rtxMemFree or rtxMemReset.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @param enabled      Boolean indicating system memory management to be used.
 * @return             Previous state of "useSysMem" flag.
 */
EXTERNRT OSBOOL rtxMemBufSetUseSysMem (OSRTMEMBUF* pMemBuf, OSBOOL value);

/**
 * This function trims white space of the memory buffer.
 *
 * @param pMemBuf      A pointer to a memory buffer structure.
 * @return             Length of trimmed buffer.
 */
EXTERNRT OSSIZE rtxMemBufTrimW (OSRTMEMBUF* pMemBuf);

/**
 * @} buffermanfun
 */
#ifdef __cplusplus
}
#endif

#endif
