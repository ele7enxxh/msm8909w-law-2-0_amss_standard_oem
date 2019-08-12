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
 * @file OSRTOutputStream.h
 * C++ base class definitions for operations with output streams.
 */
#ifndef _OSRTOUTPUTSTREAM_H_
#define _OSRTOUTPUTSTREAM_H_

#include "rtxsrc/OSRTOutputStreamIF.h"
#include "rtxsrc/OSRTStream.h"

/**
 * @ingroup ostrmclas
 *
 * The base class definition for operations with output streams.  As with
 * the input stream, this implementation is backed by memory buffers to
 * improve I/O performance.
 *
 * @{
 */
class EXTRTCLASS OSRTOutputStream :
public OSRTStream, public OSRTOutputStreamIF
{
 public:
   /**
    * The default constructor. It initializes a buffered stream.  A
    * buffered stream maintains data in memory before reading or writing
    * to the device.  This generally provides better performance than
    * an unbuffered stream.
    */
   EXTRTMETHOD OSRTOutputStream ();

   EXTRTMETHOD OSRTOutputStream
      (OSRTContext* mpContext, OSBOOL attachStream = FALSE);

   /**
    * Virtual destructor. Closes the stream if it was opened.
    */
   virtual EXTRTMETHOD ~OSRTOutputStream ();

   /**
    * Closes the output or output stream and releases any system resources
    * associated with the stream. For output streams this function also flushes
    * all internal buffers to the stream.
    *
    * @return            Completion status of operation:
    *                      - 0 = success,
    *                      - negative return value is error.
    * @see       ::rtxStreamClose, ::rtxStreamBufClose
    */
   virtual EXTRTMETHOD int close ();

   /**
    * Flushes the buffered data to the stream.
    *
    * @return            Completion status of operation:
    *                      - 0 = success,
    *                      - negative return value is error.
    * @see       ::rtxStreamFlush, ::rtxStreamBufFlush
    */
   virtual EXTRTMETHOD int flush ();

   /**
    * This method returns a pointer to the underlying OSRTContext object.
    *
    * @return             A reference-counted pointer to an OSRTContext object.
    *                       The OSRTContext object will not be released until
    *                       all referenced-counted pointer variables go out of
    *                       scope. This allows safe sharing of the context
    *                       between different run-time classes.
    */
   virtual OSRTCtxtPtr getContext () {
      return OSRTStream::getContext ();
   }

   /**
    * This method returns a pointer to the underlying OSCTXT object. This is
    * the structure used in calls to low-level C encode/decode functions.
    *
    * @return             Pointer to a context (OSCTXT) structure.
    */
   virtual OSCTXT* getCtxtPtr () {
      return OSRTStream::getCtxtPtr ();
   }

   /**
    * Returns error text in a dynamic memory buffer. Buffer will be allocated by
    * 'operator new []'. The calling routine is responsible to free the memory
    * by using 'operator delete []'.
    *
    * @return             A pointer to a newly allocated buffer with error text.
    */
   virtual char* getErrorInfo () {
      return OSRTStream::getErrorInfo ();
   }

   /**
    * Returns error text in a memory buffer. If buffer pointer is specified in
    * parameters (not NULL) then error text will be copied in the passed
    * buffer. Otherwise, this method allocates memory using the
    * 'operator new []' function. The calling routine is responsible to free
    * the memory by using 'operator delete []'.
    *
    * @param pBuf         A pointer to a destination buffer to obtain the error
    *                       text. If NULL, dynamic buffer will be allocated.
    * @param bufSize      A reference to buffer size. If pBuf is NULL it will
    *                       receive the size of allocated dynamic buffer.
    * @return             A pointer to a buffer with error text. If pBuf is not
    *                       NULL, the return pointer will be equal to it.
    *                       Otherwise, returns newly allocated buffer with error
    *                       text. NULL, if error occurred.
    */
   virtual char* getErrorInfo (char* pBuf, size_t& bufSize) {
      return OSRTStream::getErrorInfo (pBuf, bufSize);
   }

   /**
    * This method returns the completion status of previous operation.
    * It can be used to check completion status of constructors or methods,
    * which do not return completion status.
    *
    * @return            Runtime status code:
    *                      - 0 = success,
    *                      - negative return value is error.
    */
   virtual int getStatus () const {
      return OSRTStream::getStatus ();
   }

   /**
    * This method is used to query a stream object in order to determine
    * its actual type.
    *
    * @param id  Enumerated stream identifier
    * @return    True if the stream matches the identifier
    */
   virtual OSBOOL isA (StreamID id) const {
      return id == Unknown;
   }

   /**
    * Checks if the stream open or not.
    *
    * @return            s TRUE, if the stream is opened, FALSE otherwise.
    * @see               ::rtxStreamIsOpened
    */
   virtual EXTRTMETHOD OSBOOL isOpened ();

   /**
    * The printErrorInfo method prints information on errors contained within
    * the context.
    */
   inline void printErrorInfo () { OSRTStream::printErrorInfo (); }

   /**
    * The resetErrorInfo method resets information on errors contained within
    * the context.
    */
   inline void resetErrorInfo () { OSRTStream::resetErrorInfo (); }

   /**
    * Write data to the stream.  This method writes the given number of
    * octets from the given array to the output stream.
    *
    * @param pdata       The pointer to the data to be written.
    * @param size        The number of octets to write.
    * @return            The total number of octets written into the stream, or
    *                       negative value with error code if any error is
    *                       occurred.
    * @see               ::rtxStreamWrite
    */
   virtual EXTRTMETHOD long write (const OSOCTET* pdata, size_t size);

   /**
    * Write data to the stream.  This method writes data from a null-terminated
    * character string to the output stream.
    *
    * @param pdata       The pointer to the data to be written.
    * @return            The total number of octets written into the stream, or
    *                       negative value with error code if any error is
    *                       occurred.
    * @see               ::rtxStreamWrite
    */
   virtual EXTRTMETHOD long write (const char* pdata);

} ;

/** @} */

#endif /* _OSRTOUTPUTSTREAM_H_ */
