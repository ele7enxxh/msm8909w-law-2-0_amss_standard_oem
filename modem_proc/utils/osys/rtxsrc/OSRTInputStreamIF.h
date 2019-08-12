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
 * @file OSRTInputStreamIF.h
 * C++ interface class definitions for operations with input streams.
 */
#ifndef _OSRTINPUTSTREAMIF_H_
#define _OSRTINPUTSTREAMIF_H_

#include "rtxsrc/OSRTStreamIF.h"

/**
 * @defgroup istrmclas Generic Input Stream Classes
 * @ingroup cppruntime
 *
 * The C++ interface class definitions for operations with input streams.
 * Classes that implement this interface are used to input data from the
 * various stream types, not to decode ASN.1 messages.
 *
 * @{
 */
class EXTRTCLASS OSRTInputStreamIF : public OSRTStreamIF {
 public:
   enum StreamID { Unknown, File, Memory, Socket };

   /**
    * Virtual destructor. Closes the stream if it was opened.
    */
   virtual EXTRTMETHOD ~OSRTInputStreamIF ();

   /**
    * This method is used to query a stream object in order to determine
    * its actual type.
    *
    * @param id  Enumerated stream identifier
    * @return    True if the stream matches the identifier
    */
   virtual OSBOOL isA (StreamID id) const = 0;

   /**
    * This method returns the current position in the stream (in octets).
    *
    * @return             The number of octets already read from the stream.
    */
   virtual size_t currentPos () = 0;

   /**
    * Returns the current stream position.  This may be used with the
    * <code>setPosition</code> method to reset back to an arbitrary point
    * in the input stream.
    *
    * @param ppos         Pointer to a variable to receive position.
    * @return             Completion status of operation: 0 = success,
    *                       negative return value is error.
    */
   virtual int getPosition (size_t* ppos) = 0;

   /**
    * Tests if this input stream supports the mark and reset methods. Whether
    * or not mark and reset are supported is an invariant property of a
    * particular input stream instance. By default, it returns FALSE.
    *
    * @return             TRUE if this stream instance supports the mark and
    *                       reset methods; FALSE otherwise.
    * @see               ::rtxStreamIsMarkSupported
    */
   virtual OSBOOL markSupported () = 0;

   /**
    * This method marks the current position in this input stream. A subsequent
    * call to the reset method repositions this stream at the last marked
    * position so that subsequent reads re-read the same bytes. The
    * readAheadLimit argument tells this input stream to allow that many bytes
    * to be read before the mark position gets invalidated.
    *
    * @param readAheadLimit           the maximum limit of bytes that can be
    *                                   read before the mark position becomes
    *                                   invalid.
    * @return            Completion status of operation:
    *                      - 0 = success,
    *                      - negative return value is error.
    * @see                           ::rtxStreamMark, ::rtxStreamReset
    */
   virtual int mark (size_t readAheadLimit) = 0;

   /**
    * Read data from the stream. This method reads up to \c maxToRead bytes
    * from the stream. It may return a value less then this if the mamimum
    * number of bytes is not available.
    *
    * @param pDestBuf                 Pointer to a buffer to receive a data.
    * @param maxToRead                Size of the buffer.
    * @return             The total number of octets read into the buffer, or
    *                       negative value with error code if any error is
    *                       occurred.
    * @see                           ::rtxStreamRead
    */
   virtual long read (OSOCTET* pDestBuf, size_t maxToRead) = 0;

   /**
    * Read data from the stream. This method reads up to \c maxToRead bytes
    * from the stream. It may return a value less then this if the mamimum
    * number of bytes is not available.
    *
    * @param pDestBuf                 Pointer to a buffer to receive a data.
    * @param toReadBytes              Number of bytes to be read.
    * @return             The total number of octets read into the buffer, or
    *                       negative value with error code if any error is
    *                       occurred.
    * @see                           ::rtxStreamRead
    */
   virtual long readBlocking (OSOCTET* pDestBuf, size_t toReadBytes) = 0;

   /**
    * Repositions this stream to the position at the time the mark method was
    * last called on this input stream.
    *
    * @return            Completion status of operation:
    *                      - 0 = success,
    *                      - negative return value is error.
    * @see                           ::rtxStreamMark, ::rtxStreamReset
    */
   virtual int reset () = 0;

   /**
    * Sets the current stream position to the given offset.
    *
    * @param pos          Position stream is to be reset to.  This
    *                       is normally obtained via a call to
    *                       <code>getPosition</code>, although in most
    *                       cases it is a zero-based offset.
    * @return             Completion status of operation: 0 = success,
    *                       negative return value is error.
    */
   virtual int setPosition (size_t pos) = 0;

   /**
    * Skips over and discards the specified amount of data octets from this
    * input stream.
    *
    * @param n           The number of octets to be skipped.
    * @return            Completion status of operation:
    *                      - 0 = success,
    *                      - negative return value is error.
    * @see                           ::rtxStreamSkip
    */
   virtual int skip (size_t n) = 0;

} ;

class EXTRTCLASS OSRTInputStreamPtr {
   OSRTInputStreamIF* mPtr;
 public:
   inline OSRTInputStreamPtr (OSRTInputStreamIF* ptr) : mPtr (ptr) {}
   inline ~OSRTInputStreamPtr () { delete mPtr; }

   inline operator OSRTInputStreamIF* ()   { return mPtr; }
   inline OSRTInputStreamIF* operator-> () { return mPtr; }
} ;

/** @} */

#endif /* _OSRTINPUTSTREAMIF_H_ */
