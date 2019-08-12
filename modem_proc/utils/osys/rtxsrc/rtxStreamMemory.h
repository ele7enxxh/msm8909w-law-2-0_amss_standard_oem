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
 * @file rtxStreamMemory.h
 */
#ifndef _RTXSTREAMMEMORY_H_
#define _RTXSTREAMMEMORY_H_

#ifndef _NO_STREAM

#include "rtxsrc/rtxStream.h"

typedef struct DirBufDesc {
   OSCTXT* pctxt;
   OSRTSTREAM* pUnderStream; /* underlying stream object */
   size_t   savedIndex;
} DirBufDesc;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup rtxStreamMemory Memory stream functions.
 * Memory stream functions are used for memory stream operations. @{
 */
/**
 * Opens a memory stream. A memory buffer will be created by this function. The
 * 'flags' parameter specifies the access mode for the stream - input or
 * output.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param flags        Specifies the access mode for the stream:
 *                       - OSRTSTRMF_INPUT = input (reading) stream;
 *                       - OSRTSTRMF_OUTPUT = output (writing) stream.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamMemoryCreate (OSCTXT* pctxt, OSUINT16 flags);

/**
 * Opens a memory stream using the specified memory buffer. The 'flags'
 * parameter specifies the access mode for the stream - input or output.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pMemBuf      The pointer to the buffer.
 * @param bufSize      The size of the buffer.
 * @param flags        Specifies the access mode for the stream:
 *                       - OSRTSTRMF_INPUT = input (reading) stream;
 *                       - OSRTSTRMF_OUTPUT = output (writing) stream.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamMemoryAttach (OSCTXT* pctxt, OSOCTET* pMemBuf,
                                     size_t bufSize, OSUINT16 flags);

/**
 * This function returns the memory buffer and its size for the given memory
 * stream.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pSize        The pointer to size_t to receive the size of buffer.
 * @return             The pointer to memory buffer. NULL, if error occurred.
 */
EXTERNRT OSOCTET* rtxStreamMemoryGetBuffer (OSCTXT* pctxt, size_t* pSize);

/**
 * This function creates an input memory stream using the specified buffer.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pMemBuf      The pointer to the buffer
 * @param bufSize      The size of the buffer
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamMemoryCreateReader
   (OSCTXT* pctxt, OSOCTET* pMemBuf, size_t bufSize);

/**
 * This function creates an output memory stream using the specified buffer. If
 * \c pMemBuf or \c bufSize is NULL then new buffer will be allocated.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pMemBuf      The pointer to the buffer. Can be NULL - new buffer will
 *                       be allocated in this case.
 * @param bufSize      The size of the buffer. Can be 0 - new buffer will be
 *                       allocated in this case.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamMemoryCreateWriter
   (OSCTXT* pctxt, OSOCTET* pMemBuf, size_t bufSize);

/**
 * This function resets the output memory stream internal buffer to allow
 * it to be overwritten with new data.  Memory for the buffer is not freed.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamMemoryResetWriter (OSCTXT* pctxt);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* _NO_STREAM */
#endif /* _RTXSTREAMMEMORY_H_ */

