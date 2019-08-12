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
 * @file OSRTOutputStreamIF.h
 * C++ interface class definitions for operations with output streams.
 */
#ifndef _OSRTOUTPUTSTREAMIF_H_
#define _OSRTOUTPUTSTREAMIF_H_

#include "rtxsrc/OSRTStreamIF.h"

/**
 * @defgroup ostrmclas  Generic Output Stream Classes
 * @ingroup cppruntime
 *
 * The interface class definition for operations with output streams.  Classes
 * that implement this interface are used for writing data to the various
 * stream types, not to encode ASN.1 messages.
 *
 * @{
 */
class EXTRTCLASS OSRTOutputStreamIF : public OSRTStreamIF {
 public:
   enum StreamID { Unknown, File, Memory, Socket };

   /**
    * Virtual destructor. Closes the stream if it was opened.
    */
   virtual EXTRTMETHOD ~OSRTOutputStreamIF ();

   /**
    * This method is used to query a stream object in order to determine
    * its actual type.
    *
    * @param id  Enumerated stream identifier
    * @return    True if the stream matches the identifier
    */
   virtual OSBOOL isA (StreamID id) const = 0;

   /**
    * Write data to the stream.  This method writes the given number of
    * octets from the given array to the output stream.
    *
    * @param pdata       Pointer to the data to be written.
    * @param size        The number of octets to write.
    * @return            The total number of octets written into the stream, or
    *                       negative value with error code if any error is
    *                       occurred.
    * @see               ::rtxStreamWrite
    */
   virtual long write (const OSOCTET* pdata, size_t size) = 0;

} ;

class EXTRTCLASS OSRTOutputStreamPtr {
   OSRTOutputStreamIF* mPtr;
 public:
   inline OSRTOutputStreamPtr (OSRTOutputStreamIF* ptr) : mPtr (ptr) {}
   inline ~OSRTOutputStreamPtr () { delete mPtr; }

   inline operator OSRTOutputStreamIF* ()   { return mPtr; }
   inline OSRTOutputStreamIF* operator-> () { return mPtr; }
} ;

/** @} */

#endif /* _OSRTOUTPUTSTREAMIF_H_ */
