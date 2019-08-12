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
 * @file rtxStream.h
 * Input/output data stream type definitions and function prototypes.
 */
#ifndef _RTXSTREAM_H_
#define _RTXSTREAM_H_

#ifndef _NO_STREAM

#include "rtxsrc/rtxContext.h"
#include "rtxsrc/rtxMemBuf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup rtxStream Input/Output Data Stream Utility Functions
 * @{
 *
 * Stream functions are used for unbuffered stream operations. All of the
 * operations with streams are performed using a context block to maintain
 * state information.
 *
 * <p>These functions may be used for any input/output operations with streams.
 * Each stream should be initialized first by call to the \c rtxStreamInit
 * function. After initialization, the stream may be opened for reading or
 * writing by calling one of the following functions:
 *   - \c rtxStreamFileOpen
 *   - \c rtxStreamFileAttach
 *   - \c rtxStreamSocketAttach
 *   - \c rtxStreamMemoryCreate
 *   - \c rtxStreamMemoryAttach
 */

struct OSRTSTREAM;

/**
 * Stream read function pointer type. A user may implement a customized read
 * function for specific input streams. The read function is defined in the
 * OSRTSTREAM control structure.
 */
typedef long (*OSRTStreamReadProc)
   (struct OSRTSTREAM* pStream, OSOCTET* pbuffer, size_t bufSize);

/**
 * Stream blockingRead function pointer type. A user may implement a customized
 * read function for specific input streams. The blockingRead function is
 * defined in the OSRTSTREAM control structure.
 */
typedef long (*OSRTStreamBlockingReadProc)
   (struct OSRTSTREAM* pStream, OSOCTET* pbuffer, size_t toReadBytes);

/**
 * Stream write function pointer type. A user may implement a customized write
 * function for any specific output streams. The write function is defined in
 * the OSRTSTREAM control structure.
 */
typedef long (*OSRTStreamWriteProc)(struct OSRTSTREAM* pStream,
                                   const OSOCTET* data, size_t numocts);

/**
 * Stream flush function pointer type. A user may implement a customized flush
 * function for any specific output streams. The flush function is defined in
 * the OSRTSTREAM control structure.
 */
typedef int (*OSRTStreamFlushProc)(struct OSRTSTREAM* pStream);

/**
 * Stream close function pointer type. A user may implement a customized close
 * function for any specific input or output streams. The close function is
 * defined in the OSRTSTREAM control structure.
 */
typedef int (*OSRTStreamCloseProc)(struct OSRTSTREAM* pStream);

/**
 * Stream skip function pointer type. A user may implement a customized
 * function for a specific input stream type. The skip function is defined in
 * the OSRTSTREAM control structure.
 */
typedef int (*OSRTStreamSkipProc)
   (struct OSRTSTREAM* pStream, size_t skipBytes);

/**
 * Stream mark function pointer type. A user may implement a customized
 * function for a specific input stream type. The mark function is defined in
 * the OSRTSTREAM control structure.
 */
typedef int (*OSRTStreamMarkProc)
   (struct OSRTSTREAM* pStream, size_t readAheadLimit);

/**
 * Stream reset function pointer type. A user may implement a customized
 * function for a specific input stream type. The reset function is defined in
 * the OSRTSTREAM control structure.
 */
typedef int (*OSRTStreamResetProc) (struct OSRTSTREAM* pStream);

/**
 * Stream get position function pointer type. A user may implement a customized
 * function for a specific input stream type. The mark function is defined in
 * the OSRTSTREAM control structure.
 */
typedef int (*OSRTStreamGetPosProc)
   (struct OSRTSTREAM* pStream, size_t* ppos);

/**
 * Stream set position function pointer type. A user may implement a customized
 * function for a specific input stream type. The mark function is defined in
 * the OSRTSTREAM control structure.
 */
typedef int (*OSRTStreamSetPosProc)
   (struct OSRTSTREAM* pStream, size_t pos);

#define OSRTSTRMF_INPUT      0x0001
#define OSRTSTRMF_OUTPUT     0x0002
#define OSRTSTRMF_BUFFERED   0x8000 /* direct-buffer stream    */
#define OSRTSTRMF_UNBUFFERED 0x4000 /* force unbuffered stream */
#define OSRTSTRMF_POSMARKED  0x2000 /* stream has marked position */
#define OSRTSTRMF_FIXINMEM   0x1000 /* disable flushing */

#define OSRTSTRMF_BUF_INPUT    (OSRTSTRMF_INPUT|OSRTSTRMF_BUFFERED)
#define OSRTSTRMF_BUF_OUTPUT   (OSRTSTRMF_OUTPUT|OSRTSTRMF_BUFFERED)


#define OSRTSTRMID_FILE      1
#define OSRTSTRMID_SOCKET    2
#define OSRTSTRMID_MEMORY    3
#define OSRTSTRMID_BUFFERED  4
#define OSRTSTRMID_DIRECTBUF 5
#define OSRTSTRMID_CTXTBUF   6
#define OSRTSTRMID_ZLIB      7
#define OSRTSTRMID_USER   1000

#define OSRTSTRM_K_BUFSIZE 1024

#define OSRTSTRM_K_INVALIDMARK ((size_t)-1)

#define OSRTSTREAM_BYTEINDEX(pctxt) \
(((pctxt)->pStream->id == OSRTSTRMID_DIRECTBUF) ?                    \
 ((pctxt)->pStream->bytesProcessed + (pctxt)->buffer.byteIndex) :    \
 ((pctxt)->pStream->bytesProcessed /* was ioBytes */ ))

#define OSRTSTREAM_ID(pctxt) ((pctxt)->pStream->id)
#define OSRTSTREAM_FLAGS(pctxt) ((pctxt)->pStream->flags)

/**
 * The stream control block. A user may implement a customized stream by
 * defining read, skip, close functions for input streams and write, flush,
 * close for output streams.
 */
typedef struct OSRTSTREAM {
   OSRTStreamReadProc  read;     /**< pointer to read function */
   OSRTStreamBlockingReadProc
      blockingRead;      /**< pointer to blockingRead function */
   OSRTStreamWriteProc write;   /**< pointer to write function */
   OSRTStreamFlushProc flush;   /**< pointer to flush function */
   OSRTStreamCloseProc close;   /**< pointer to close function */
   OSRTStreamSkipProc  skip;    /**< pointer to skip function  */
   OSRTStreamMarkProc  mark;    /**< pointer to mark function  */
   OSRTStreamResetProc reset;   /**< pointer to reset function  */
   OSRTStreamGetPosProc getPos; /**< pointer to getPos function  */
   OSRTStreamSetPosProc setPos; /**< pointer to setPos function  */

   void*  extra;                /**< pointer to stream-specific data */
   size_t bufsize;              /**< physical size of pctxt->buffer.data
                                       buffer */
   size_t readAheadLimit;       /**< read ahead limit (used by
                                       ::rtxStreamMark/::rtxStreamReset */
   size_t bytesProcessed;       /**< the number of bytes processed by the
                                       application program */
   size_t markedBytesProcessed; /**< the marked number of bytes already
                                       processed */
   size_t ioBytes;              /**< the actual number of bytes
                                       read from or written to the stream */
   size_t nextMarkOffset;       /**< offset of next appropriate mark
                                       position */
   size_t segsize;              /**< size of decoded segment */
   OSUINT32 id;                 /**< id of stream (see OSRTSTRMID_* macros) */

   /**
    * Buffer into which data read from stream can be captured for
    * debugging purposes.
    */
   OSRTMEMBUF* pCaptureBuf;

   OSUINT16  flags;          /**< flags (see OSRTSTRMF_* macros          */
} OSRTSTREAM;

/**
 * This function closes the input or output stream and releases any system
 * resources associated with the stream. For output streams this function also
 * flushes all internal buffers to the stream.
 *
 * @param pctxt        Pointer to a context structure variable which has been
 *                       initialized for stream operations via a call to \c
 *                       rtxStreamInit.
 */
EXTERNRT int rtxStreamClose (OSCTXT* pctxt);

/**
 * This function flushes the output stream and forces any buffered output
 * octets to be written out.
 *
 * @param pctxt        Pointer to a context structure variable which has been
 *                       initialized for stream operations via a call to \c
 *                       rtxStreamInit.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamFlush (OSCTXT* pctxt);

/**
 * This function initializes a stream part of the context block. This function
 * should be called first before any operation with a stream.
 *
 * @param pctxt        Pointer to context structure variable, for which stream
 *                       to be initialized.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamInit (OSCTXT* pctxt);

/**
 * This function reads up to 'bufsize' bytes of data from the input stream into
 * an array of octets. An attempt is made to read as many as bufsize octets,
 * but a smaller number may be read, possibly zero. The number of octets
 * actually read is returned as an integer. This functions blocks until input
 * data is available, end of file is detected, or another error is occurred.
 *
 * @param pctxt        Pointer to a context structure variable which has been
 *                       initialized for stream operations via a call to
 *                       ::rtxStreamInit.
 * @param pbuffer      Pointer to a buffer to receive data.
 * @param bufSize      Size of the buffer.
 * @return             The total number of octets read into the buffer, or
 *                       negative value with error code if any error is
 *                       occurred.
 */
EXTERNRT long rtxStreamRead
   (OSCTXT* pctxt, OSOCTET* pbuffer, size_t bufSize);

/**
 * This function reads up to 'bufsize' bytes of data from the input stream into
 * an array of octets. An attempt is made to read as many as bufsize octets,
 * but a smaller number may be read, possibly zero. The number of octets
 * actually read is returned as an integer. This functions blocks until input
 * data is available, end of file is detected, or another error is occurred.
 *
 * @param pctxt        Pointer to a context structure variable which has been
 *                       initialized for stream operations via a call to
 *                       ::rtxStreamInit.
 * @param pbuffer      Pointer to a buffer to receive data.
 * @param readBytes    Number of bytes to read.
 * @return             The total number of octets read into the buffer, or
 *                       negative value with error code if any error is
 *                       occurred.
 */
EXTERNRT long rtxStreamBlockingRead
   (OSCTXT* pctxt, OSOCTET* pbuffer, size_t readBytes);

/**
 * This function skips over and discards the specified amount of data octets
 * from this input stream.
 *
 * @param pctxt        Pointer to a context structure variable which has been
 *                       initialized for stream operations via a call to
 *                       ::rtxStreamInit.
 * @param skipBytes    The number of octets to be skipped.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamSkip  (OSCTXT* pctxt, size_t skipBytes);

/**
 * This function writes the specified amount of octets from the specified array
 * to the output stream.
 *
 * @param pctxt        Pointer to a context structure variable which has been
 *                       initialized for stream operations via a call to
 *                       ::rtxStreamInit.
 * @param data         The pointer to data to be written.
 * @param numocts      The number of octets to write.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT long rtxStreamWrite
   (OSCTXT* pctxt, const OSOCTET* data, size_t numocts);

/**
 * This function returns the number of processed octets. If the stream was
 * opened as an input stream, then it returns the total number of read octets.
 * If the stream was opened as an output stream, then it returns the total
 * number of written octets. Otherwise, this function returns an error code.
 *
 * @param pctxt        Pointer to a context structure variable which has been
 *                       initialized for stream operations via a call to \c
 *                       rtxStreamInit.
 * @param pPos         Pointer to argument to receive total number of
 *                       processed octets.
 * @return             The total number of processed octets or error code
 *                       (negative value).
 */
EXTERNRT int rtxStreamGetIOBytes (OSCTXT* pctxt, size_t* pPos);

/**
 * Marks the current position in this input stream. A subsequent call to the
 * ::rtxStreamReset function repositions this stream at the last marked
 * position so that subsequent reads re-read the same bytes. The
 * \c readAheadLimit argument tells this input stream to allow many bytes
 * to be read before the mark position gets invalidated.
 *
 * @param pctxt             Pointer to a context structure variable that has
 *                            been initialized for stream operations.
 * @param readAheadLimit    The maximum limit of bytes that can be read before
 *                            the mark position becomes invalid.
 * @return                  Completion status of operation: 0 = success,
 *                            negative return value is error.
 */
EXTERNRT int rtxStreamMark (OSCTXT* pctxt, size_t readAheadLimit);

/**
 * Repositions this stream to the position recorded by the last call to the
 * ::rtxStreamMark function.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamReset (OSCTXT* pctxt);

/**
 * Tests if this input stream supports the mark and reset methods. Whether or
 * not mark and reset are supported is an invariant property of a particular
 * input stream instance. By default, it returns FALSE.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @return             TRUE if this stream instance supports the mark and reset
 *                       methods; FALSE otherwise.
 */
EXTERNRT OSBOOL rtxStreamMarkSupported (OSCTXT* pctxt);

/**
 * Tests if this stream opened (for reading or writing).
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @return             TRUE if this stream is opened for reading or writing;
 *                       FALSE otherwise.
 */
EXTERNRT OSBOOL rtxStreamIsOpened (OSCTXT* pctxt);

/**
 * Tests if this stream opened for reading.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @return             TRUE if this stream is opened for reading; FALSE
 *                       otherwise.
 */
EXTERNRT OSBOOL rtxStreamIsReadable (OSCTXT* pctxt);

/**
 * Tests if this stream opened for writing.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @return             TRUE if this stream is opened for writing; FALSE
 *                       otherwise.
 */
EXTERNRT OSBOOL rtxStreamIsWritable (OSCTXT* pctxt);

/**
 * This function releases the stream's resources. If it is opened for reading
 * or writing it will be closed.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamRelease (OSCTXT* pctxt);

/**
 * This function sets a capture buffer for the stream.  This is
 * used to record all data read from the stream.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pmembuf      Pointer to an initialized memory buffer structure.
 *                       This argument may be set to NULL to disable
 *                       capture if previously set.
 */
EXTERNRT void rtxStreamSetCapture (OSCTXT* pctxt, OSRTMEMBUF* pmembuf);

/**
 * This function returns the capture buffer currently assigned to the
 * stream.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @return             Pointer to memory buffer that was previously assigned
 *                       as a capture buffer to the stream.
 */
EXTERNRT OSRTMEMBUF* rtxStreamGetCapture (OSCTXT* pctxt);

/**
 * Get the current position in input stream.
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param ppos         Pointer to a variable to receive position.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamGetPos (OSCTXT* pctxt, size_t* ppos);

/**
 * Set the current position in input stream.
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pos          Stream position.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamSetPos (OSCTXT* pctxt, size_t pos);

/** @} rtxStream */

#ifdef __cplusplus
}
#endif

#endif /* _NO_STREAM */
#endif /* _RTXSTREAM_H_ */

