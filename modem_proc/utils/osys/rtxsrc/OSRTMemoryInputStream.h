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
 * @file OSRTMemoryInputStream.h
 * C++ base class definitions for operations with input memory streams.
 */
#ifndef _OSRTMEMORYINPUTSTREAM_H_
#define _OSRTMEMORYINPUTSTREAM_H_

#include "rtxsrc/OSRTInputStream.h"

/**
 * Generic memory input stream. This class provides methods for streaming
 * data from an input memory buffer.
 */
class EXTRTCLASS OSRTMemoryInputStream : public OSRTInputStream {
 public:
   /**
    * Initializes the memory input stream using the specified memory buffer.
    *
    * @param pMemBuf                   The pointer to the buffer.
    * @param bufSize                   The size of the buffer.
    * @see                            ::rtxStreamMemoryAttach
    */
   EXTRTMETHOD OSRTMemoryInputStream (const OSOCTET* pMemBuf, size_t bufSize);

   /**
    * Initializes the memory input stream using the specified memory buffer.
    *
    * @param pContext                  Pointer to a context to use.
    * @param pMemBuf                   The pointer to the buffer.
    * @param bufSize                   The size of the buffer.
    * @see                            ::rtxStreamMemoryAttach
    */
   EXTRTMETHOD OSRTMemoryInputStream
      (OSRTContext* pContext, const OSOCTET* pMemBuf, size_t bufSize);

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

} ;

#endif /* _OSRTMEMORYINPUTSTREAM_H_ */
