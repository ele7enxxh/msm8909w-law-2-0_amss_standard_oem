/*
 * Copyright (c) 1997-2013 Objective Systems, Inc.
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
 * @file OSRTSocket.h
 * TCP/IP or UDP socket class definitions.
 */
#ifndef _OSRTSOCKET_H_
#define _OSRTSOCKET_H_

#ifndef _OS_NOSOCKET

/* Disable MS VC++ Level 4 warning about unreferenced inline functions */
#ifdef _MSC_VER
#pragma warning(disable: 4514)
#endif /* _MSC_VER */

#include "rtxsrc/rtxSocket.h"

/** @defgroup socketCpp TCP/IP or UDP Socket Classes
 * These classes provide utility methods for doing socket I/O.
 * @ingroup cppruntime
 * @{
 */

/**
 * Wrapper class for TCP/IP or UDP sockets.
 */
class EXTRTCLASS OSRTSocket {
 protected:
   OSRTSOCKET mSocket;  ///< handle of the socket
   int        mInitStatus;
   int        mStatus;
   OSBOOL     mOwner;   ///< indicates this class owns the socket

   inline OSBOOL isInitialized () { return mInitStatus == 0; }

 public:
   /**
    * This is the default constructor. It initializes all internal members
    * with default values and creates a new socket structure.
    * Use getStatus() method to determine
    * has error occurred during the initialization or not.
    *
    */
   EXTRTMETHOD OSRTSocket ();

   /**
    * This constructor initializes an instance by using an existing socket.
    *
    * @param socket       An existing socket handle.
    * @param ownership    Boolean flag that specifies who is the owner of
    *                     the socket. If it is TRUE then the socket will be
    *                     destroyed in the destructor. Otherwise, the user is
    *                     responsible to close and destroy the socket.
    */
   EXTRTMETHOD OSRTSocket (OSRTSOCKET socket, OSBOOL ownership = FALSE);

   /**
    * The copy constructor. The copied instance will have the same socket
    * handle as the original one, but will not be the owner of the handle.
    */
   EXTRTMETHOD OSRTSocket (const OSRTSocket& socket);

   /**
    * The destructor. This closes socket if the instance is the owner of the
    * socket.
    */
   EXTRTMETHOD ~OSRTSocket ();

   /**
    * This method permits an incoming connection attempt on a socket. It
    * extracts the first connection on the queue of pending connections on
    * the socket. It then creates a new socket and returns an instance of
    * the new socket. The newly created socket will handle the
    * actual connection and has the same properties as the original socket.
    *
    * @param destIP       Optional pointer to a buffer that receives the IP
    *                       address of the connecting entity. It may be NULL.
    * @param port         Optional pointer to a buffer that receives the port
    *                       of the connecting entity. It may be NULL.
    * @return             An instance of the new socket class. NULL, if error
    *                       occur. Use ::getStatus method to obtain error code.
    * @see               ::rtSocketAccept
    */
   EXTRTMETHOD OSRTSocket* accept (OSIPADDR* destIP = 0, int *port = 0);

   /**
    * This method converts an IP address to its string representation.
    *
    * @param ipAddr       The IP address to be converted.
    * @param pAddrStr     Pointer to the buffer to receive a
    *                        string with the IP address.
    * @param bufsize      Size of the buffer.
    * @return             Pointer to a string with IP-address. NULL, if error
    *                       occur.
    */
   EXTRTMETHOD static const char* addrToString
      (OSIPADDR ipAddr, char* pAddrStr, size_t bufsize);

   /**
    * This method associates a local address with a socket. It is used on an
    * unconnected socket before subsequent calls to the ::connect or ::listen
    * methods.
    *
    * @param addr         The local IP address to assign to the
    *                        socket.
    * @param port         The local port number to assign to the
    *                        socket.
    * @return             Completion status of operation:
    *                       - 0 = success,
    *                       - negative return value is error.
    * @see                ::rtSocketBind
    */
   EXTRTMETHOD int bind (OSIPADDR addr, int port);

   /**
    * This method associates a local address with a socket. It is used on an
    * unconnected socket before subsequent calls to the ::connect or ::listen
    * methods.  This version of the method allows a URL to be used instead
    * of address and port number.
    *
    * @param              Univeral resource locator (URL) string.
    * @return             Completion status of operation:
    *                       - 0 = success,
    *                       - negative return value is error.
    * @see                ::rtSocketBind
    */
   EXTRTMETHOD int bindUrl (const char* url);

   /**
    * This method associates a local address with a socket. It is used on an
    * unconnected socket before subsequent calls to the ::connect or ::listen
    * methods.
    *
    * @param pAddrStr     Null-terminated character string
    *                        representing a number expressed in the
    *                        Internet standard "." (dotted)
    *                        notation.
    * @param port         The local port number to assign to the
    *                        socket.
    * @return             Completion status of operation:
    *                       - 0 = success,
    *                       - negative return value is error.
    * @see                ::rtSocketBind
    */
   EXTRTMETHOD int bind (const char* pAddrStr, int port);

   /**
    * This method associates only a local port with a socket. It is used on an
    * unconnected socket before subsequent calls to the OSRTSocket::connect or
    * OSRTSocket::listen methods.
    *
    * @param port         The local port number to assign to the
    *                        socket.
    * @return             Completion status of operation:
    *                       - 0 = success,
    *                       - negative return value is error.
    * @see                ::rtSocketBind
    * @see bind ()
    */
   inline int bind (int port) {
      return bind (OSIPADDR_ANY, port);
   }

   /**
    * This method receives data from the connected socket. In this case,
    * the connection is blocked until either the requested number of bytes
    * is received or the socket is closed or an error occurs.
    *
    * @param pbuf         Pointer to the buffer for the incoming data.
    * @param readBytes    Number of bytes to receive.
    * @return             If no error occurs, returns the number of bytes
    *                       received. Otherwise, the negative value is
    *                       error code.
    */
   EXTRTMETHOD int blockingRead (OSOCTET* pbuf, size_t readBytes);

   /**
    * This method closes this socket.
    *
    * @return             Completion status of operation:
    *                       - 0 = success,
    *                       - negative return value is error.
    * @see                ::rtSocketClose
    */
   EXTRTMETHOD int close ();

   /**
    * This method establishes a connection to this socket. It is used to create
    * a connection to the specified destination. When the socket call completes
    * successfully, the socket is ready to send and receive data.
    *
    * @param host         Null-terminated character string
    *                        representing a number expressed in the
    *                        Internet standard "." (dotted)
    *                        notation.
    * @param port         The destination port to connect.
    * @return             Completion status of operation:
    *                       - 0 = success,
    *                       - negative return value is error.
    * @see                            ::rtSocketConnect
    */
   EXTRTMETHOD int connect (const char* host, int port);

   /**
    * This method establishes a connection to this socket. It is used to
    * create a connection to the specified destination. In this version,
    * destination is specified using a URL.
    *
    * @param              Univeral resource locator (URL) string.
    * @return             Completion status of operation:
    *                       - 0 = success,
    *                       - negative return value is error.
    * @see                            ::rtSocketConnect
    */
   EXTRTMETHOD int connectUrl (const char* url);

   /**
    * Returns the ownership of underlying O/S socket.
    *
    * @returns TRUE, if the socket object has the ownership of
    *            underlying O/S socket.
    */
   OSBOOL getOwnership () { return mOwner; }

   /**
    * This method returns the handle of the socket.
    *
    * @return             The handle of the socket.
    */
   OSRTSOCKET getSocket () const { return mSocket; }

   /**
    * Returns a completion status of last operation.
    *
    * @return             Completion status of last operation:
    *                       - 0 = success,
    *                       - negative return value is error.
    */
   inline int getStatus () { return mStatus; }

   /**
    * This method places a socket into a state where it is listening for
    * an incoming connection.
    *
    * @param maxConnections    Maximum length of the queue of pending
    *                             connections.
    * @return                  Completion status of operation:
    *                            - 0 = success,
    *                            - negative return value is error.
    * @see                     ::rtSocketListen
    */
   EXTRTMETHOD int listen (int maxConnections);

   /**
    * This method receives data from a connected socket. It is used to read
    * incoming data on sockets. The socket must be connected before calling
    * this function.
    *
    * @param pbuf             Pointer to the buffer for the incoming
    *                            data.
    * @param bufsize          Length of the buffer.
    * @return                 If no error occurs, returns the number
    *                           of bytes received. Negative error code if
    *                           error occurred.
    * @see                    ::rtSocketRecv
    */
   EXTRTMETHOD int recv (OSOCTET* pbuf, size_t bufsize);

   /**
    * Transfers an ownership of the underlying O/S socket to or from the
    * socket object. If the socket object has the ownership of the underlying
    * O/S socket it will close the O/S socket when the socket object is being
    * closed or destroyed.
    *
    * @param ownership   TRUE, if socket object should have ownership of the
    *                      underlying O/S socket; FALSE, otherwise.
    */
   inline void setOwnership (OSBOOL ownership) { mOwner = ownership; }

   /**
    * This method sends data on a connected socket. It is used to write
    * outgoing data on a connected socket.
    *
    * @param pdata            Buffer containing the data to be
    *                            transmitted.
    * @param size             Length of the data in pdata.
    * @return                 Completion status of operation:
    *                           - 0 = success,
    *                           - negative return value is error.
    * @see                    ::rtSocketSend
    */
   EXTRTMETHOD int send (const OSOCTET* pdata, size_t size);

   /**
    * This method converts a string containing an Internet Protocol dotted
    * address into a proper OSIPADDR address.
    *
    * @param pAddrStr         Null-terminated character string
    *                            representing a number expressed in the
    *                            Internet standard "." (dotted)
    *                            notation.
    * @return                 If no error occurs, returns OSIPADDR.
    *                            OSIPADDR_INVALID, if error occurred.
    */
   EXTRTMETHOD static OSIPADDR stringToAddr (const char* pAddrStr);
} ;

/** @} */

#endif /* _OS_NOSOCKET */

#endif //_OSRTSOCKET_H_
