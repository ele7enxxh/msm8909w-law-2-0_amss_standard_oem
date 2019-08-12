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
 * @file OSRTSocketOutputStream.h
 * C++ base class definitions for operations with output socket streams.
 */
#ifndef _OSRTSOCKETOUTPUTSTREAM_H_
#define _OSRTSOCKETOUTPUTSTREAM_H_

#ifndef _OS_NOSOCKET

#include "rtxsrc/OSRTSocket.h"
#include "rtxsrc/OSRTOutputStream.h"

/**
 * Generic socket output stream. This class opens an existing socket for
 * output in binary mode and reads data from it.
 */
class EXTRTCLASS OSRTSocketOutputStream : public OSRTOutputStream {
 protected:
   OSRTSocket mSocket; ///< a socket
 public:
   /**
    * Creates and initializes a socket output stream using the OSRTSocket
    * instance of socket.
    *
    * @param socket                  Reference to OSRTSocket instance.
    * @see                           ::rtxStreamSocketOpen
    */
   EXTRTMETHOD OSRTSocketOutputStream (OSRTSocket& socket);

   /**
    * Creates and initializes a socket output stream using the OSRTSocket
    * instance of socket.
    *
    * @param pContext                Pointer to a context to use.
    * @param socket                  Reference to OSRTSocket instance.
    * @see                           ::rtxStreamSocketOpen
    */
   EXTRTMETHOD OSRTSocketOutputStream
      (OSRTContext* pContext, OSRTSocket& socket);

   /**
    * Initializes the socket output stream using the socket handle.
    *
    * @param socket                  Handle of the socket.
    * @param ownership               Indicates ownership of the socket.
    *                                  Set to TRUE to pass ownership to
    *                                  this object instance.  The socket
    *                                  will be closed when this object
    *                                  instance is deleted or goes out
    *                                  of scope.
    * @see                           ::rtxStreamSocketAttach
    */
   EXTRTMETHOD OSRTSocketOutputStream
      (OSRTSOCKET socket, OSBOOL ownership = FALSE);

   /**
    * Initializes the socket output stream using the socket handle.
    *
    * @param pContext                Pointer to a context to use.
    * @param socket                  Handle of the socket.
    * @param ownership               Indicates ownership of the socket.
    *                                  Set to TRUE to pass ownership to
    *                                  this object instance.  The socket
    *                                  will be closed when this object
    *                                  instance is deleted or goes out
    *                                  of scope.
    * @see                           ::rtxStreamSocketAttach
    */
   OSRTSocketOutputStream (OSRTContext* pContext,
                           OSRTSOCKET socket,
                           OSBOOL ownership = FALSE);

   /**
    * This method is used to query a stream object in order to determine
    * its actual type.
    *
    * @param id  Enumerated stream identifier
    * @return    True if the stream matches the identifier
    */
   virtual OSBOOL isA (StreamID id) const {
      return id == Socket;
   }

} ;

#endif /* _OS_NOSOCKET */

#endif /* _OSRTSOCKETOUTPUTSTREAM_H_ */
