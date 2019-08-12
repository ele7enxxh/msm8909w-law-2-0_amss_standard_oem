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
 * @file OSRTMemoryOutputStream.h
 * C++ base class definitions for operations with output memory streams.
 */
#ifndef _OSRTMEMORYOUTPUTSTREAM_H_
#define _OSRTMEMORYOUTPUTSTREAM_H_

#include "rtxsrc/OSRTOutputStream.h"

/**
 * Generic memory output stream. This class provides methods for streaming
 * data to an output memory buffer.
 */
class EXTRTCLASS OSRTMemoryOutputStream : public OSRTOutputStream {
 public:
   /**
    * The default constructor initializes the memory output stream to use
    * a dynamic memory output buffer.  The status of the construction can
    * be obtained by calling the <code>getStatus</code> method.
    *
    * @see                            ::rtxStreamMemoryCreate
    */
   EXTRTMETHOD OSRTMemoryOutputStream ();

   /**
    * Initializes the memory output stream using the specified memory buffer.
    * The status of the construction can be obtained by calling the
    * <code>getStatus</code> method.
    *
    * @param pMemBuf                   The pointer to the buffer.
    * @param bufSize                   The size of the buffer.
    * @see                            ::rtxStreamMemoryAttach
    */
   EXTRTMETHOD OSRTMemoryOutputStream (OSOCTET* pMemBuf, size_t bufSize);

   /**
    * Initializes the memory output stream using the specified memory buffer.
    * The status of the construction can be obtained by calling the
    * <code>getStatus</code> method.
    *
    * @param pContext                  Pointer to a context to use.
    * @param pMemBuf                   The pointer to the buffer.
    * @param bufSize                   The size of the buffer.
    * @see                            ::rtxStreamMemoryAttach
    */
   EXTRTMETHOD OSRTMemoryOutputStream
      (OSRTContext* pContext, OSOCTET* pMemBuf, size_t bufSize);

   /**
    * This method returns the address of the memory buffer to which
    * data was written.  If the buffer memory is dynamic, it may be
    * freed using the rtxMemFreePtr function or it will be freed when
    * the stream object is destroyed.
    *
    * @param pSize  Pointer to a size variable to receive the number of bytes
    *                 written to the stream.  This is an optional parameter,
    *                 if a null pointer is passed, size is not returned.
    * @return       Pointer to memory buffer.
    */
   EXTRTMETHOD OSOCTET* getBuffer (size_t* pSize = 0);

   /**
    * This method is used to query a stream object in order to determine
    * its actual type.
    *
    * @param id  Enumerated stream identifier
    * @return    True if the stream matches the identifier
    */
   virtual OSBOOL isA (StreamID id) const {
      return id == Memory;
   }

   /**
    * This method resets the output memory stream internal buffer to allow
    * it to be overwritten with new data.  Memory for the buffer is not freed.
    *
    * @return    Completion status of operation: 0 = success, negative
    *              return value is error.
    */
   int reset ();

} ;

#endif /* _OSRTMEMORYOUTPUTSTREAM_H_ */
