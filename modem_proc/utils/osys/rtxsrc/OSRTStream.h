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
 * @file OSRTStream.h
 * C++ base class definitions for operations with I/O streams.
 */
#ifndef _OSRTSTREAM_H_
#define _OSRTSTREAM_H_

#include "rtxsrc/OSRTCtxtHolder.h"
#include "rtxsrc/OSRTStreamIF.h"

/**
 * @ingroup osrtstream
 *
 * The default base class for using I/O streams.  This class may be
 * subclassed, as in the case of OSRTInputStream and OSRTOutputStream or other
 * custom implementations.
 *
 * @{
 *
 */
class EXTRTCLASS OSRTStream : public OSRTStreamIF {
 protected:
   OSRTCtxtHolder mCtxtHolder;
   OSBOOL mbAttached;   ///< Flag, TRUE for "attached" streams
   int mStatus;         ///< Last stream operation status
   int mInitStatus; ///< Initialization status. 0 if initialized successfully

   EXTRTMETHOD OSRTStream (OSRTContext* pContext, OSBOOL attachStream = FALSE);
   EXTRTMETHOD OSRTStream (OSRTStream& original);

   /**
    * The default constructor. It initializes a buffered stream.  A
    * buffered stream maintains data in memory before reading or writing
    * to the device.  This generally provides better performance than
    * an unbuffered stream.
    */
   EXTRTMETHOD OSRTStream ();

   EXTRTMETHOD char* getErrorInfo (size_t* pBufSize);

 public:
   /**
    * Virtual destructor. Closes the stream if it was opened.
    */
   virtual EXTRTMETHOD ~OSRTStream ();

   /**
    * Closes the input or output stream and releases any system resources
    * associated with the stream. For output streams this function also flushes
    * all internal buffers to the stream.
    *
    * @return            Completion status of operation:
    *                      - 0 = success,
    *                      - negative return value is error.
    * @see       ::rtxStreamClose
    */
   virtual EXTRTMETHOD int close ();

   /**
    * Flushes the buffered data to the stream.
    *
    * @return            Completion status of operation:
    *                      - 0 = success,
    *                      - negative return value is error.
    * @see       ::rtxStreamFlush
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
      return mCtxtHolder.getContext ();
   }

   /**
    * This method returns a pointer to the underlying OSCTXT object. This is
    * the structure used in calls to low-level C encode/decode functions.
    *
    * @return             Pointer to a context (OSCTXT) structure.
    */
   virtual OSCTXT* getCtxtPtr () {
      return mCtxtHolder.getCtxtPtr ();
   }

   /**
    * Returns error text in a dynamic memory buffer. Buffer will be allocated by
    * 'operator new []'. The calling routine is responsible to free the memory
    * by using 'operator delete []'.
    *
    * @return             A pointer to a newly allocated buffer with error text.
    */
   virtual char* getErrorInfo () {
      return mCtxtHolder.getErrorInfo ();
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
      return mCtxtHolder.getErrorInfo (pBuf, bufSize);
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
   inline int getStatus () const {
      return mStatus;
   }

   inline OSBOOL isInitialized () { return (OSBOOL)(mInitStatus == 0); }

   /**
    * Checks, is the stream opened or not.
    * @returns TRUE, if the stream is opened, FALSE otherwise.
    * @see       ::rtxStreamIsOpened
    */
   virtual EXTRTMETHOD OSBOOL isOpened ();

   /**
    * The printErrorInfo method prints information on errors contained within
    * the context.
    */
   inline void printErrorInfo () {
      mCtxtHolder.printErrorInfo ();
   }

   /**
    * The resetErrorInfo method resets information on errors contained within
    * the context.
    */
   inline void resetErrorInfo () {
      mCtxtHolder.resetErrorInfo ();
   }

} ;

/** @} */

#endif
