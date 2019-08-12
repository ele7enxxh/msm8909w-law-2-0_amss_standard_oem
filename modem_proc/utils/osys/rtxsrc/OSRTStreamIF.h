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
 * @file OSRTStreamIF.h
 * C++ interface class definitions for operations with I/O streams.
 */
#ifndef _OSRTSTREAMIF_H_
#define _OSRTSTREAMIF_H_

#include "rtxsrc/OSRTCtxtHolderIF.h"

/** @defgroup osrtstream ASN.1 Stream Classes
 *
 *  Classes that read or write ASN.1 messages to files, sockets, memory buffers,
 *  et c., are derived from this class.
 *
 *  @{
 */
class EXTRTCLASS OSRTStreamIF /*: public OSRTCtxtHolderIF*/ {
 public:
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
   virtual int close () = 0;

   /**
    * Flushes buffered data to the stream.
    *
    * @return            Completion status of operation:
    *                      - 0 = success,
    *                      - negative return value is error.
    * @see       ::rtxStreamFlush
    */
   virtual int flush () = 0;

   virtual OSRTCtxtPtr getContext () = 0;
   virtual OSCTXT* getCtxtPtr () = 0;

   /**
    * Checks if the stream is opened or not.
    * @returns TRUE, if the stream is opened, FALSE otherwise.
    * @see       ::rtxStreamIsOpened
    */
   virtual OSBOOL isOpened () = 0;

   virtual ~OSRTStreamIF () {};

} ;

/** @} */

#endif /* _OSRTSTREAMIF_H_ */
