/*===========================================================================

                      D S S  _ S T U B S . C

DESCRIPTION

EXTERNALIZED FUNCTIONS

Copyright (c) 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dssocket.c_v   1.29   27 Feb 2003 12:00:40   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/dss_stubs.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/31/09    pp     Initial Release.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"




#include "ps_acl.h"


#ifdef FEATURE_DATA_PS_QOS
  #define FEATURE_DATA_PS_QOS /* QOS Stubs not required */
#endif /* FEATURE_DATA_PS_QOS */




#ifndef FEATURE_DATA_PS_QOS

#include "msg.h"
#include "dssocket.h"
#include "dss_ping.h"
#include "dssdns.h"
#include "ds_Utils_DebugMsg.h"

#define DSS_STUBS_PERROR_NO_FEAT(api, feat)                         \
  LOG_MSG_ERROR(#api": "#feat"+DATA_PS feature turned off."             \
           "Returning error.", 0, 0, 0)

#define DS_SOCKETS_PERROR(api)                                      \
  DSS_STUBS_PERROR_NO_FEAT(api, DS_SOCKETS)

#define PS_DNS_PERROR(api)                                          \
  DSS_STUBS_PERROR_NO_FEAT(api, DATA_PS_DNS)

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

//#ifndef _NO_DS_SOCKETS_STUBS is always defined
#if 0
/*===========================================================================
FUNCTION DSS_SOCKET()

DESCRIPTION
 Create a socket and related data structures,and return a socket descriptor.

 The mapping to actual protocols is as follows:

 ADDRESS FAMILY         Stream          Datagram

 DSS_AF_INET             TCP             UDP

  Note this function must be called to obtain a valid socket descriptor, for
  use with all other socket-related functions.  Thus, before any socket
  functions can be used (e.g. I/O, asynchronous notification, etc.), this
  call must have successfully returned a valid socket descriptor.  The
  application must also have made a call to dss_open_netlib() to obtain
  a valid application ID, and to put the Data Services task into "sockets"
  mode.

  Note:  This implementation version has no support for Raw IP sockets, and
         will return an error, if the application attempts to create one.

         Sockets created using this call are bound to the dss_nethandle used in
         creating this socket.

DEPENDENCIES
  The function dss_open_netlib() must be called to open the network library
  and put the DS/PS managers into sockets mode.

RETURN VALUE
  On successful creation of a socket, this function returns socket file
  descriptor which is a sint15 value between 0x1000 and 0x1FFF.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EAFNOSUPPORT     address family not supported
  DS_EBADAPP          invalid application ID
  DS_ESOCKNOSUPPORT   invalid or unsupported socket parameter specified
  DS_EPROTONOSUPPORT  invalid or unsupported protocol
  DS_EMFILE           too many descriptors open.  A socket is already open
                      or has not closed compeletely.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_socket
(
  sint15 dss_nethandle,
  byte   family,                      /* Address family - DSS_AF_INET only */
  byte   type,                                              /* socket type */
  byte   protocol,                                        /* protocol type */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_socket);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_socket() */

/*===========================================================================
FUNCTION DSS_SOCKET2()

DESCRIPTION
 Create a socket and related data structures, and return a socket descriptor.

 The mapping to actual protocols is as follows:

 ADDRESS FAMILY         Stream          Datagram

 DSS_AF_INET             TCP             UDP

  Note this function must be called to obtain a valid socket descriptor, for
  use with all other socket-related functions.  Thus, before any socket
  functions can be used (e.g. I/O, asynchronous notification, etc.), this
  call must have successfully returned a valid socket descriptor.  The
  application must also have made a call to dss_open_netlib() to obtain
  a valid application ID, and to put the Data Services task into "sockets"
  mode.

  Note:  This implementation version has no support for Raw IP sockets, and
         will return an error, if the application attempts to create one.

         Sockets created using socket2 are not bound to any particular dss_nethandle.

DEPENDENCIES
  Netpolicy structure needs to be initialized by calling dss_init_netpolicy.

RETURN VALUE
  On successful creation of a socket, this function returns socket file
  descriptor which is a sint15 value between 0x1000 and 0x1FFF.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EAFNOSUPPORT     address family not supported
  DS_EBADAPP          invalid application ID
  DS_EPROTOTYPE       specified protocol invalid for socket type
  DS_ESOCKNOSUPPORT   invalid or unsupported socket parameter specified
  DS_EPROTONOSUPPORT  specified protocol not supported
  DS_EMFILE           too many descriptors open.  A socket is already open or
                      has not closed compeletely.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_socket2
(
  byte   family,                                         /* Address family */
  byte   type,                                              /* socket type */
  byte   protocol,                                        /* protocol type */
  dss_sock_cb_fcn sock_cb,                     /* socket callback function */
  void * sock_cb_user_data,              /* User data for socket call back */
  dss_net_policy_info_type * policy_info_ptr,       /* Network policy info */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_socket2);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_socket2() */

/*===========================================================================
FUNCTION DSS_CONNECT()

DESCRIPTION
  For TCP, attempts to establish the TCP connection.  Upon
  successful connection, calls the socket callback function asserting that
  the DS_WRITE_EVENT is TRUE.  For udp it fills in the peers ipaddr in the
  socket control block and binds the socket to an interface.
  The function must receive (as a parameter) a valid socket descriptor,
  implying a previous successful call to dss_socket().

DEPENDENCIES
  Network subsystem must be established and available.

RETURN VALUE
  Return DSS_ERROR and places the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block
  DS_EBADF            invalid socket descriptor is specfied
  DS_ECONNREFUSED     connection attempt refused
  DS_ETIMEDOUT        connection attempt timed out
  DS_EFAULT           addrlen parameter is invalid
  DS_EIPADDRCHANGED   IP address changed due to PPP resync
  DS_EINPROGRESS      connection establishment in progress
  DS_EISCONN          a socket descriptor is specified that is already
                      connected
  DS_ENETDOWN         network subsystem unavailable
  DS_EOPNOTSUPP       invalid server address specified
  DS_EADDRREQ         destination address is required.
  DS_NOMEM            not enough memory to establish connection

SIDE EFFECTS
  For TCP, initiates active open for connection.
===========================================================================*/
sint15 dss_connect
(
  sint15 sockfd,                                      /* Socket descriptor */
  struct ps_sockaddr *servaddr,                     /* destination address */
  uint16 addrlen,                                    /* length of servaddr */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_connect);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

}  /* dss_connect() */

/*===========================================================================
FUNCTION DSS_LISTEN()

DESCRIPTION

  For TCP, this starts a passive open for connections.  Upon a
  sucessful connection, the socket callback function is invoked
  asserting DS_ACCEPT_EVENT as TRUE.  The application should respond
  with a call to dss_accept(). If a connection is recieved and there
  are no free queue slots the new connection is rejected
  (ECONNREFUSED).  The backlog queue is for ALL unaccepted sockets
  (half-open, or completely established).

  A listening UDP doesn't make sense, and as such isn't supported.
  DS_EOPNOTSUPP is returned.

  The sockfd parameter is a created (dss_socket) and bound (dss_bind)
  socket that will become the new listening socket.  The backlog
  parameter indiates the maximum length for the queue of pending
  sockets.  If backlog is larger than the maximum, it will be
  reduced to the maximum (see DSS_SOMAXCONN). This is the BSD behavior.

  The argument dss_error should point to a memory location in which
  error conditions can be recorded.

DEPENDENCIES

  Network subsystem must be established and available.

  The sockfd should get a valid socket descriptor (implying a
  previously successful call to dss_socket) This socket should be
  bound to a specific port number (implying a previously successful
  call to dss_bind) .

RETURN VALUE

  Returns DSS_SUCCESS on success.  If the backlog was truncated
  DS_EFAULT will be set in errno, but the call will still be
  successful.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block (PJ: I don't think this CAN happen)
  DS_EBADF            invalid socket descriptor is specfied
  DS_EOPNOTSUPP       The socket is not capable of listening (UDP)
  DS_EFAULT           backlog parameter is invalid
  DS_ENETNONET        network subsystem unavailable for some unknown reason
  DS_ENETINPROGRESS   network subsystem establishment currently in progress
  DS_ENETCLOSEINPROGRESS network subsystem close in progress.
  DS_NOMEM            not enough memory to establish backlog connections.
  DS_EINVAL           Socket already open, closed, unbound or not one
                      you can listen on.

SIDE EFFECTS
  For TCP, initiates passive open for new connections.
===========================================================================*/
sint15 dss_listen
(
  sint15 sockfd,                                      /* Socket descriptor */
  sint15 backlog,                      /* Number of connections to backlog */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_listen);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

}  /* dss_listen() */

/*===========================================================================
FUNCTION DSS_ACCEPT()

DESCRIPTION

  The accept function is used on listening sockets to respond when
  DS_ACCEPT_EVENT is asserted.  The first backlog queued connection is
  removed from the queue, and bound to a new connected socket (as if
  you called dss_socket).  The newly created socket is in the
  connected state.  The listening socket is unaffect the queue size is
  maintained (ie. there is not need to call listen again.)

  The argument sockfd is the file descriptor of the listening socket

  The argument remote addr is a pointer to a struct ps_sockaddr.  This
  structure is populated with the address information for the remote
  end of the new connection. addrlen should initially contain the
  length of the struct ps_sockaddr passed in.  The length of the real
  address is placed in this location when the struct is populated.

  The argument dss_error should point to a memory location in which
  error conditions can be recorded.

DEPENDENCIES

  Network subsystem must be established and available.

  The sockfd should get a valid socket descriptor (implying a
  previously successful call to dss_socket) This socket should be
  bound to a specific port number (implying a previously successful
  call to dss_bind).  The socket should be listening (implying a
  previously successful call to dss_listen).

RETURN VALUE
  Returns the socket descriptor of the new socket on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block
  DS_EBADF            invalid socket descriptor is specfied
  DS_EOPNOTSUPP       The socket is not of type DSS_SOCK_STREAM
  DS_EINVAL           Socket is not listening.
  DS_EFAULT           The addr parameter is bogus.
  DS_ENETNONET        network subsystem unavailable for some unknown reason
  DS_ENETINPROGRESS   network subsystem establishment currently in progress
  DS_ENETCLOSEINPROGRESS network subsystem close in progress.
  DS_NOMEM            not enough memory to establish backlog connections.

SIDE EFFECTS

  The head backlog item from the queue of the listening socket is
  removed from that queue.
===========================================================================*/
sint15 dss_accept
(
  sint15 sockfd,                                      /* Socket descriptor */
  struct ps_sockaddr *remoteaddr,                    /* new remote address */
  uint16 *addrlen,                                   /* length of servaddr */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_accept);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

}  /* dss_accept() */

/*===========================================================================
FUNCTION DSS_OPEN_NETLIB()

DESCRIPTION

  Opens up the network library.  Assigns application ID and sets the
  application-defined callback functions to be called when library and
  socket calls would make progress.  The callback are called with a pointer
  to a sint15 containing the application ID for the callback.
  NOTE: the memory for the application ID is ephemeral and likely will not be
    available after the callback returns - if it is desired to use this
    information outside the scope of the callback it should be COPIED, a
    pointer MUST not be used.

  Puts data services manager into "socket" mode.

  This function is called from the context of the socket client's task.

  This function is wrapper around dss_opennetlib2. This calls opennetlib2
  with standard network and socket call back functions and user data as
  the user specified call back functions. The standard socket and network
  call back functions call the user specified call back functions.

DEPENDENCIES
  None.

RETURN VALUE
  Returns application ID on success.

  On error, return DSS_SUCCESS and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EMAPP      no more applications available - max apps exceeded.

SIDE EFFECTS
  Puts data services manager into "socket" mode.
===========================================================================*/
sint15 dss_open_netlib
(
  void   (*net_callback_fcn)(void *),         /* network callback function */
  void   (*socket_callback_fcn)(void *),       /* socket callback function */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_open_netlib);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_open_netlib() */

/*===========================================================================
FUNCTION DSS_OPEN_NETLIB2()

DESCRIPTION

  Opens up the network library.  Assigns application ID and sets the
  application-defined callback functions to be called when library and
  socket calls would make progress. Stores the network policy info and
  uses it in further calls.

  Puts data services manager into "socket" mode.

  This function is called from the context of the socket client's task.

DEPENDENCIES
  None.

RETURN VALUE
  Returns application ID on success.

  On error, return DSS_SUCCESS and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EMAPP      no more applications available - max apps exceeded.
  DS_EFAULT     Policy structure is uninitialized.

SIDE EFFECTS
  Puts data services manager into "socket" mode.
===========================================================================*/
sint15 dss_open_netlib2
(
  dss_net_cb_fcn net_cb,                      /* network callback function */
  void * net_cb_user_data,              /* User data for network call back */
  dss_sock_cb_fcn sock_cb,                     /* socket callback function */
  void * sock_cb_user_data,               /*User data for socket call back */
  dss_net_policy_info_type * policy_info_ptr,       /* Network policy info */
  sint15 * dss_errno                              /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_open_netlib2);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_open_netlib2() */

/*===========================================================================
FUNCTION DSS_CLOSE_NETLIB()

DESCRIPTION

  Closes the network library for the application.  All sockets must have
  been closed for the application, prior to closing.  If this is the last
  remaining application, the network subsytem (PPP/traffic channel) must
  have been brought down, prior to closing the network library.  If this
  is the last active application using the network library, this function
  takes the data services manager out of "socket" mode.

  This function is called from the context of the socket client's task.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP        invalid application ID
  DS_ESOCKEXIST     there are existing sockets
  DS_ENETEXIST      the network subsystem exists

SIDE EFFECTS
  Puts data services manager into "autodetect" mode.
===========================================================================*/
sint15 dss_close_netlib
(
  sint15 dss_nethandle,                                         /* application ID */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_close_netlib);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_close_netlib() */

/*===========================================================================
FUNCTION DSS_PPPOPEN()

DESCRIPTION
  Starts up the network subsystem (CDMA data service and PPP) over the Um
  interface for all sockets.

DEPENDENCIES
  Note that a valid application ID must be specified as a parameter,
  obtained by a successful return of dss_open_netlib().

RETURN VALUE
  Return DSS_ERROR and places the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP               invalid application ID specified
  DS_EWOULDBLOCK           the operation would block
  DS_EOPNOTSUPP            Phone in laptop call
  DS_ENETNONET             App specified interface is disabled or in
                           routable state.

SIDE EFFECTS
  Initiates call origination and PPP negotiation.
===========================================================================*/
sint15 dss_pppopen
(
  sint15 dss_nethandle,                                  /* application id */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_pppopen);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_pppopen() */

/*===========================================================================
FUNCTION DSS_PPPCLOSE()

DESCRIPTION
  Initiates termination to bring down PPP and the traffic channel.  Upon
  successful close of the network subsystem, invokes the network callback
  function.
  It is suggested that, before calling this function, dss_netstatus should
  be called to check the status of the connection.

DEPENDENCIES
  None.

RETURN VALUE
  If the network subsytem is already closed, return DSS_SUCCESS.

  Returns DSS_ERROR and places the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP               invalid application ID specified
  DS_EWOULDBLOCK           operation would block
  DS_ENETCLOSEINPROGRESS   network close in progress. A call to
                           dss_pppclose() has already been issued.

SIDE EFFECTS
  Initiates termination of PPP.  Brings down PPP and traffic channel.
===========================================================================*/
sint15 dss_pppclose
(
  sint15 dss_nethandle,                              /* application id     */
  sint15 *dss_errno                           /* error condition value     */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_pppclose);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_pppclose() */

/*===========================================================================
FUNCTION DSS_BIND()

DESCRIPTION
  For all client sockets, attaches a local address and port value to the
  socket.  If the call is not explicitly issued, the socket will implicitly
  bind in calls to dss_connect() or dss_sendto().  Note that this function
  does not support binding to a local IPv4 address, but rather ONLY a local
  port number.  The local IPv4 address is assigned automatically by the sockets
  library.  This function does support binding to a local IPv6 address,
  however, as this is required for using IPv6 Privacy Extensions (RFC 3041).
  The function must receive (as a parameter) a valid socket descriptor,
  implying a previous successful call to dss_socket().

DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EOPNOTSUPP       operation not supported
  DS_EADDRINUSE       the local address is already in use.
  DS_EINVAL           the socket is already attached to a local name
  DS_EFAULT           invalid address and/or invalid address length

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_bind
(
  sint15 sockfd,                                      /* socket descriptor */
  struct ps_sockaddr *localaddr,                          /* local address */
  uint16 addrlen,                                     /* length of address */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_bind);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_bind() */

/*===========================================================================
FUNCTION DSS_CLOSE()

DESCRIPTION
  Non-blocking close of a socket.  Performs all necessary clean-up of data
  structures and frees the socket for re-use.  For TCP initiates the active
  close for connection termination.  Once TCP has closed, the DS_CLOSE_EVENT
  will become TRUE, and the application can call dss_close() again to free
  the socket for re-use.  UDP sockets also need to call this to
  clean-up the socket and free it for re-use.

DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block - TCP close in progress
  DS_EBADF            invalid socket descriptor is specfied

SIDE EFFECTS
  Initiates active close for TCP connections.
===========================================================================*/
sint15 dss_close
(
  sint15 sockfd,                                      /* socket descriptor */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_close);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

}  /* dss_close() */

/*===========================================================================
FUNCTION DSS_NETSTATUS()

DESCRIPTION
  Provides status of network subsystem.  Called in response to DS_ENETDOWN
  errors.  Note that origination status is based on the last attempted
  origination.

DEPENDENCIES
  None.

RETURN VALUE

  Returns DSS_ERROR and places the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP               invalid application ID specified
  DS_ENETNONET             network subsystem unavailable for some unknown
                           reason
  DS_ENETISCONN            network subsystem is connected and available
  DS_ENETINPROGRESS        network subsystem establishment currently in
                           progress
  DS_ENETCLOSEINPROGRESS   network subsystem close in progress.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_netstatus
(
  sint15 dss_nethandle,                                  /* application ID */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_netstatus);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_netstatus() */

/*===========================================================================
FUNCTION DSS_ASYNC_SELECT()

DESCRIPTION
  Enables the events to be notified about through the asynchronous
  notification mechanism.  Application specifies a bitmask of events that it
  is interested in, for which it will receive asynchronous notification via
  its application callback function. This function also performs a real-time
  check to determine if any of the events have already occurred, and if so
  invokes the application callback.

DEPENDENCIES
  None.

RETURN VALUE

  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied

SIDE EFFECTS
  Sets the relevant event mask in the socket control block.  Will also
  notify the application via the callback function.
===========================================================================*/
sint31 dss_async_select
(
  sint15 sockfd,                                      /* socket descriptor */
  sint31 interest_mask,                        /* bitmask of events to set */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_async_select);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_async_select() */

/*===========================================================================
FUNCTION DSS_ASYNC_DESELECT()

DESCRIPTION
  Clears events of interest in the socket control block interest mask.  The
  application specifies a bitmask of events that it wishes to clear; events
  for which it will no longer receive notification.

DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied


SIDE EFFECTS
  Clears specified events from the relevant event mask.
===========================================================================*/
sint15 dss_async_deselect
(
  sint15 sockfd,                                      /* socket descriptor */
  sint31 clr_interest_mask,                  /* bitmask of events to clear */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_async_deselect);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_async_deselect() */

/*===========================================================================
FUNCTION DSS_GETNEXTEVENT()

DESCRIPTION
  This function performs a real-time check to determine if any of the events
  of interest specified in the socket control block's event mask have
  occurred.  It also clears any bits in the event mask that have occurred.
  The application must re-enable these events through a subsequent call to
  dss_async_select().  The application may pass in a pointer to a single
  socket descriptor to determine if any events have occurred for that socket

  Alternatively, the application may set this pointer's value to NULL (0)
  (note, not to be confused with a NULL pointer, but rather a pointer whose
  value is 0) in which case the function will return values for the next
  available socket.  The next available socket's descriptor will be placed
  in the socket descriptor pointer, and the function will return.  If no
  sockets are available (no events have occurred across all sockets for
  that application) the pointer value will remain NULL (originally value
  passed in), and the function will return 0, indicating that no events
  have occurred.

DEPENDENCIES
  None.

RETURN VALUE
  Returns an event mask of the events that were asserted for sockets whose
  app ID matches that passed in.  A value of 0 indicates that no events have
  occurred.

  On passing a pointer whose value is NULL into the function for
  the socket descriptor (not to be confused with a NULL pointer), places
  the next available socket descriptor in *sockfd_ptr and returns the
  event mask for that socket. If no sockets are available (no events have
  occurred across all sockets for that application) the pointer value
  will remain NULL (originally value passed in), and the function will
  return 0, indicating that no events have occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP           invalid app descriptor is specfied
  DS_EBADF             invalid socket descriptor is specfied

SIDE EFFECTS
  Clears the bits in the socket control block event mask, corresponding to
  the events that have occurred.
===========================================================================*/
sint31 dss_getnextevent
(
  sint15 dss_nethandle,                                         /* application ID */
  sint15 *sockfd_ptr,                                 /* socket descriptor */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_getnextevent);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_getnextevent() */

/*===========================================================================
FUNCTION DSS_WRITE()

DESCRIPTION
  Writes upto nbytes of data from buffer over the transport specified
  by the socket descriptor.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written, which could be less than the number of
      bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           can't write to a listen socket

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_write
(
  sint15 sockfd,                                      /* socket descriptor */
  const void *buffer,               /* user buffer from which to copy data */
  uint16 nbytes,                /* number of bytes to be written to socket */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_write);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_write() */

/*===========================================================================
FUNCTION DSS_WRITEV()

DESCRIPTION
  Provides the gather write variant of the dss_write() call, which
  allows the application to write from non-contiguous buffers.    Sends
  specified number of bytes in the buffer over the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written, which could be less than the number of
      bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           Cant write to a listen socket

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_writev
(
  sint15 sockfd,                                      /* socket descriptor */
  struct ps_iovec iov[],  /* array of data buffers from which to copy data */
  sint15 iovcount,                                /* number of array items */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_writev);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_writev() */

/*===========================================================================
FUNCTION DSS_READ()

DESCRIPTION
  Reads upto nbytes of data into buffer from the transport specified
  by the socket descriptor

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of
      bytes specified. A return of 0 indicates that an End-of-File condition
      has occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           Can't read a listen socket.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_read
(
  sint15 sockfd,                                      /* socket descriptor */
  void   *buffer,                     /* user buffer to which to copy data */
  uint16 nbytes,                 /* number of bytes to be read from socket */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_read);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_read() */

/*===========================================================================
FUNCTION DSS_READV()

DESCRIPTION
  Provides the scatter read variant of the dss_read() call, which
  allows the application to read into non-contiguous buffers.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of
      bytes specified. A return of 0 indicates that an End-of-File condition
      has occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           Can't read from a listen socket

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_readv
(
  sint15 sockfd,                                      /* socket descriptor */
  struct ps_iovec iov[],        /* array of data buffers to copy data into */
  sint15 iovcount,               /* number of bytes to be read from socket */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_readv);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_readv() */

/*===========================================================================
FUNCTION DSS_SENDTO()

DESCRIPTION
  Sends 'nbytes' bytes in the buffer over the transport specified by the
  socket descriptor.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ---------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EMSGSIZE         the msg is too large to be sent all at once

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_sendto
(
  sint15 sockfd,                /* socket descriptor                       */
  const void *buffer,           /* user buffer from which to copy the data */
  uint16 nbytes,                /* number of bytes to be written           */
  uint32 flags,                 /* used for SDB (if enabled)               */
  struct ps_sockaddr *toaddr,   /* destination address                     */
  uint16 addrlen,               /* address length                          */
  sint15 *dss_errno             /* error condition value                   */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_sendto);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_sendto() */

/*===========================================================================
FUNCTION DSS_RECVFROM()

DESCRIPTION
  Reads 'nbytes' bytes in the buffer from the transport specified by the
  socket descriptor.  Fills in address structure with values from who sent
  the data.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EINVAL           can't recv from a listen socket.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_recvfrom
(
  sint15 sockfd,                                      /* socket descriptor */
  void   *buffer,               /* user buffer from which to copy the data */
  uint16 nbytes,                          /* number of bytes to be written */
  uint32 flags,                                                  /* unused */
  struct ps_sockaddr *fromaddr,                     /* destination address */
  uint16 *addrlen,                                       /* address length */
  sint15 *dss_errno                               /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_recvfrom);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_recvfrom() */

/*===========================================================================
FUNCTION DSS_RECVMSG()
DESCRIPTION
  This function is a common read function for all the socket input
  functions. The message header contains an array of scattered buffers, a
  socket descriptor and an address field for filling the source address
  of the received packet.The function reads data into the scattered buffers
  over the transport specified by the socket descriptor

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EINVAL           can't recv from a listen socket.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_recvmsg
(
  sint15                  sockfd,   /* socket descriptor                   */
  struct dss_msghdr     * msg,      /* Message header for filling in data  */
  int                     flags,    /* flags from dss_recvfrom             */
  sint15                * dss_errno /* error condition value               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_recvmsg);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_recvmsg() */

/*===========================================================================
FUNCTION DSS_SENDMSG()

DESCRIPTION
  This function is a common write function for all the socket output
  functions. The message header contains an array of scattered buffers, a
  socket descriptor and destination address for unconnected udp sockets.
  The function writes data from the scattered buffers over the transport
  specified by the socket descriptor.
DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, in case of tcp it could be less
  than the number of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ---------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EMSGSIZE         the msg is too large to be sent all at once
  DS_EISCONN          if the socket is connected and the destination
                      address is other than it is connected to.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_sendmsg
(
  sint15                  sockfd,  /* socket descriptor                    */
  struct dss_msghdr     * msg,     /* Header containing data and dest addr */
  int                     flags,   /* flags used for SDB (if enabled)      */
  sint15                * dss_errno /* error condition value               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_sendmsg);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

}

/*===========================================================================
FUNCTION DSS_SETSOCKOPT()

DESCRIPTION
  Set the options associated with a socket. This fuction expects the
  following parameters:

DEPENDENCIES
  None.

PARAMETERS
  int sockfd        -     Socket file descriptor.
  int level         -     Socket option level.
  int optname,      -     Option name.
  void *optval      -     Pointer to the option value.
  uint32 *optlen    -     Pointer to the size of the option value.
  sint15 *errno     -     Error condition value.

RETURN VALUE
  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOPROTOOPT          the option is unknown at the level indicated
  DS_EINVAL               invalid option name or invalid option value
  DS_EFAULT               Invalid buffer or argument

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_setsockopt
(
  int sockfd,                            /* socket descriptor              */
  int level,                             /* socket option level            */
  int optname,                           /* option name                    */
  void *optval,                          /* value of the option            */
  uint32 *optlen,                        /* size of the option value       */
  sint15 *dss_errno                      /* error condition value          */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_setsockopt);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_setsockopt() */

/*===========================================================================
FUNCTION DSS_GETSOCKOPT()

DESCRIPTION
  Return an option associated with a socket. This fuction expects the
  following parameters:

DEPENDENCIES
  None.

PARAMETERS
  int sockfd        -     Socket file descriptor.
  int level         -     Socket option level.
  int optname,      -     Option name.
  void *optval      -     Pointer to the option value.
  uint32 *optlen    -     Pointer to the size of the option value.
  sint15 *errno     -     Error condition value.

RETURN VALUE
  optlen is a value-result parameter, initially containing the size of
  the buffer pointed to by optval, and modified on return to indicate the
  actual  size  of the value returned. On error, return DSS_ERROR and places
  the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOPROTOOPT          the option is unknown at the level indicated
  DS_EINVAL               invalid option name or invalid option value
  DS_EFAULT               Invalid buffer or argument

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_getsockopt
(
  int sockfd,                            /* socket descriptor              */
  int level,                             /* socket option level            */
  int optname,                           /* option name                    */
  void *optval,                          /* value of the option            */
  uint32 *optlen,                        /* size of the option value       */
  sint15 *dss_errno                      /* error condition value          */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_getsockopt);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_getsock_opt ()*/

/*===========================================================================
FUNCTION DSS_ENABLE_DORM_TIMER()

DESCRIPTION
  NOTE - THIS FUNCTION IS NOW DEPRECATED.
  Applications should use the new ioctl interface - dss_iface_ioctl() -
  to enable/disable dormancy timer. Refer to dss_iface_ioctl.h more details.

  Applications can call this function to enable or disable
  dormancy timer for an IS707 (1X) packet data call only.

DEPENDENCIES
  If multiple applications call this function, principle used is last
  caller wins.
  If this function is invoked to disable timer, the disabling of timer
  remains in effect until next data call (or if the function is again
  invoked in between to enable timer).


PARAMETERS
  dss_nethandle           application ID
  flag             true/false - enable/disable dorm timer

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dss_enable_dorm_timer
(
  sint15 dss_nethandle,
  boolean flag
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_enable_dorm_timer);

} /* dss_enable_dorm_timer() */


/*===========================================================================
FUNCTION DSS_GETSOCKNAME

DESCRIPTION
  Returns the current local address assigned to the specified socket.

DEPENDENCIES
  None.

PARAMETERS
  sockfd    -  socket file descriptor
  addr      -  local address currently associated with the socket
  addrlen   -  address length. This parameter is initialized to indicate
               the amount of space pointed by addr and on return, it
               contains the actual size of the address returned.
  dss_errno -  error number

RETURN VALUE
  Returns DSS_SUCCESS upon successful completion and places the socket
  address and the address length in addr and addrlen parameters, resp.

  If the address is larger than the supplied buffer then it is silently
  truncated. The value returned in addrlen indicates the size prior to
  truncation, if any.

  On error, returns DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_EFAULT               addr parameter points to an invalid memory
                          location

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_getsockname
(
  sint15              sockfd,               /* socket descriptor           */
  struct ps_sockaddr* addr,                 /* address of the socket       */
  uint16*             addrlen,              /* address length              */
  sint15*             dss_errno             /* error number                */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_getsockname);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_getsockname() */

/*===========================================================================
FUNCTION DSS_GETPEERNAME

DESCRIPTION
  Returns the address of the peer connected to the specified socket.

DEPENDENCIES
  None.

PARAMETERS
  sockfd    -  socket file descriptor
  addr      -  address of the peer connected with the socket
  addrlen   -  address length. This parameter is initialized to indicate
               the amount of space pointed by addr and on return, it
               contains the actual size of the address returned.
  dss_errno -  error number

RETURN VALUE
  Returns DSS_SUCCESS upon successful completion and places the peer
  address and the address length in addr and addrlen parameters, resp.

  If the address is larger than the supplied buffer then it is silently
  truncated. The value returned in addrlen indicates the size prior to
  truncation, if any.

  On error, returns DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_EFAULT               addr parameter points to an invalid memory
                          location
  DS_ENOTCONN             the socket is not connected

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_getpeername
(
  sint15              sockfd,               /* socket descriptor           */
  struct ps_sockaddr* addr,                 /* address of the socket       */
  uint16*             addrlen,              /* address length              */
  sint15*             dss_errno             /* error number                */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_getpeername);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_getpeername() */

/*===========================================================================
FUNCTION DSS_SHUTDOWN

DESCRIPTION
  Shuts down the connection of the specified socket depending on the
  'how' parameter as follows:

  DSS_SHUT_RD:   Disallow subsequent calls to recv function
  DSS_SHUT_WR:   Disallow subsequent calls to send function
  DSS_SHUT_RDWR: Disallow subseuqnet calls to both recv and send functions

DEPENDENCIES
  None.

PARAMETERS
  sockfd    -  socket file descriptor
  how       -  action to be performed: shutdown read-half, write-half or
               both
  dss_errno -  error number

RETURN VALUE
  In case of successful completion, returns DSS_SUCCESS. Otherwise, returns
  DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specfied
  DS_ENOTCONN             the socket is not connected
  DS_EINVAL               invalid operation (e.g., how parameter is invalid)
  DS_ENOMEM               insufficient memory available to complete the
                          operation

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_shutdown
(
  sint15           sockfd,                  /* socket descriptor           */
  uint16           how,                     /* what action to perform      */
  sint15*          dss_errno                /* error number                */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_shutdown);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_shutdown */

/*===========================================================================
FUNCTION DSS_INIT_NET_POLICY_INFO

DESCRIPTION
  Populates the policy info structure with default values.

DEPENDENCIES
  None.

PARAMETERS
  POLICY_INFO_PTR  pointer to policy info data structure.

RETURN VALUE
  None

SIDE EFFECTS
  Initializes the fields in the policy info data structure.
===========================================================================*/
void dss_init_net_policy_info
(
  dss_net_policy_info_type * policy_info_ptr       /* policy info structure */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_init_net_policy_info);

} /* dss_init_net_policy_info() */

/*===========================================================================
FUNCTION DSS_GET_APP_NET_POLICY

DESCRIPTION
 Fills in the policy info structure with the current net policy of the
 application.

DEPENDENCIES
  None.

PARAMETERS
  DSS_NETHANDLE            application id
  POLICY_INFO_PTR  pointer to policy info data structure.
  DSS_ERRNO        error number

RETURN VALUE
  In case of successful completion, returns DSS_SUCCESS. Otherwise, returns
  DSS_ERROR and places the error number in dss_errno.
  Errno Values
  ------------
  DS_EBADAPP              Invalid application ID is specfied
  DS_EFAULT               Invalid policy_info_ptr is specified.

SIDE EFFECTS
  Initializes the fields in the policy info data structure.
===========================================================================*/
sint15 dss_get_app_net_policy
(
  sint15 dss_nethandle,                                          /* Application id */
  dss_net_policy_info_type * policy_info_ptr,     /* policy info structure */
  sint15 * dss_errno                                       /* error number */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_get_app_net_policy);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_get_app_net_policy() */

/*===========================================================================
FUNCTION DSS_SET_APP_NET_POLICY

DESCRIPTION
   Sets the appliation netpolicy to the user specified value.

DEPENDENCIES
  None.

PARAMETERS
  DSS_NETHANDLE            application id
  POLICY_INFO_PTR  pointer to policy info data structure.
  DSS_ERRNO        error number

RETURN VALUE
  In case of successful completion, returns DSS_SUCCESS. Otherwise, returns
  DSS_ERROR and places the error number in dss_errno.
  Errno Values
  ------------
  DS_EBADAPP              Invalid application ID is specfied
  DS_EFAULT               Invalid policy_info_ptr is specified.
  DS_EINVAL               Operation is not allowed in current state.

SIDE EFFECTS
  Sets the appliation netpolicy to the user specified value.
===========================================================================*/
sint15 dss_set_app_net_policy
(
  sint15 dss_nethandle,                                          /* Application id */
  dss_net_policy_info_type * policy_info_ptr,     /* policy info structure */
  sint15 * dss_errno                                       /* error number */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_set_app_net_policy);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_set_app_net_policy() */

/*===========================================================================
FUNCTION DSS_GET_IFACE_ID()

DESCRIPTION
  This function is used to get the interface name and instance of a specific
  interface. The interface can be specified using either an dss_nethandle or a
  sockfd. Note that the interface would be the one that the app/socket is
  currently using.

DEPENDENCIES
  None.

PARAMETERS
  dss_id_info_type id_info - Specifies whether an dss_nethandle or sockfd is being
    as an identifier.

RETURN VALUE
  DSS_IFACE_INVALID_ID: If no valid iface could be obtained based on id_info
  iface_id: Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
dss_iface_id_type dss_get_iface_id
(
  sint15  dss_nethandle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_get_iface_id);
  return (DSS_IFACE_INVALID_ID);

} /* dss_get_iface_id() */

/*===========================================================================
FUNCTION DSS_GET_IFACE_ID_BY_POLICY()

DESCRIPTION
  This function return the interface id based on the specified network policy.

DEPENDENCIES
  None.

PARAMETERS
  dss_net_policy_info_type  net_policy_info: specifies the network policy for
  determining the interface.

RETURN VALUE
  iface_id: If a valid iface could be obtained based on policy
   On error, return DSS_IFACE_INVALID_ID and places the error condition value
   in *dss_errno.

  dss_errno Values
  ----------------
  DS_EFAULT      Netpolicy structure is not initialized.
  DS_ENOROUTE    No interface can be determined from the network policy.

SIDE EFFECTS
  None.
===========================================================================*/
dss_iface_id_type dss_get_iface_id_by_policy
(
  dss_net_policy_info_type   net_policy_info,       /* Network policy info */
  sint15                   * dss_errno            /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_get_iface_id_by_policy);
  *dss_errno = DS_EFAULT;
  return (DSS_IFACE_INVALID_ID);

} /* dss_get_iface_id_by_policy() */

/*===========================================================================
FUNCTION DSS_GET_IFACE_ID_BY_ADDR()

DESCRIPTION
  This function return the interface id matching an IP address

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  iface_id: If a valid iface could be obtained based on address
  On error, return DSS_IFACE_INVALID_ID and places the error condition value
  in *dss_errno.

  dss_errno Values
  ----------------
  DS_EFAULT      Invalid arguments passed to the function.
  DS_ENOROUTE    No interface can be determined for the address.
  DS_EOPNOTSUPP  This operation is not supported.

SIDE EFFECTS
  None.
===========================================================================*/
dss_iface_id_type dss_get_iface_id_by_addr
(
  ip_addr_type * ip_addr_ptr,                     /* IP address info       */
  sint15       * dss_errno                        /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_get_iface_id_by_addr);
  *dss_errno = DS_EFAULT;
  return (DSS_IFACE_INVALID_ID);

} /* dss_get_iface_id_by_addr() */

/*===========================================================================
FUNCTION DSS_GET_IFACE_ID_BY_QOS_HANDLE()

DESCRIPTION
  This function returns the iface_id pointing to the iface and the
  corresponding link providing a specific QOS.  The QOS instance is
  identified by the handle previously returned by
  DSS_IFACE_IOCTL_QOS_REQUEST.  The iface_id thus returned is a handle to
  the secondary link of the iface which is providing this QOS instance and
  can be used to perform an IOCTL on that particular link, for example
  registering a PHYS_LINK event on the secondary link.  The handle to the
  secondary link can also be used for DSS_IFACE_IOCTL_QOS_GET_FLOW_SPEC as
  described below.  Only the application (identified by dss_nethandle) which
  previously requested QOS identified by the handle is allowed to retrieve
  the iface_id for the secondary link.

DEPENDENCIES
  None.

PARAMETERS
  handle: qos handle.

RETURN VALUE
  DSS_IFACE_INVALID_ID: If no valid iface could be obtained based on id_info
  iface_id: Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
dss_iface_id_type dss_get_iface_id_by_qos_handle
(
  dss_qos_handle_type    handle                     // Handle to QOS instance
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_get_iface_id_by_qos_handle);
  return (DSS_IFACE_INVALID_ID);

} /* dss_get_iface_id_by_qos_handle */

/*===========================================================================
FUNCTION DSS_GET_SCOPE_ID_BY_IFACE_ID()

DESCRIPTION
  This function allows to retrieve a route_scope from the iface_id.
  Currently, for applications the notion of scope id is basically same as
  iface id as we do not support sitelocal addresses. However, applications
  need not know that scopeid and ifaceid are same as the interpretation can
  change in future when sitelocal multicast is supported.

DEPENDENCIES
  None.

PARAMETERS
  uint32  - Iface id.
  sint15* - Errno.

RETURN VALUE
  On success - Scope Id
  On failure - 0

  dss_errno Values
  ----------------
  DS_EINVAL      Invalid iface id.

SIDE EFFECTS
  None
===========================================================================*/
dss_scope_id_type dss_get_scope_id_by_iface_id
(
  dss_iface_id_type   iface_id,
  sint15 *dss_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_get_scope_id_by_iface_id);
  *dss_errno = DS_EFAULT;
  return (DSS_INVALID_SCOPE_ID);

} /* dss_get_route_scope_by_iface_id() */

/*===========================================================================
FUNCTION DSS_LAST_NETDOWNREASON()

DESCRIPTION
  This function provides an interface to the applications for retrieving the
  reason for the network interface going down.

DEPENDENCIES
  None.

  PARAMETERS
  dss_nethandle     -  application id calling this function.
  reason    -  network down reason.
  dss_errno -  error number.


RETURN VALUE
  In case of successful completion, returns DSS_SUCCESS and places the
  network down reason in reason. Otherwise, returns DSS_ERROR and places
  the error number in dss_errno.

  dss_errno Values
  ---------------
  DS_EBADAPP               invalid application ID specified
  DS_EFAULT                invalid buffer or argument.

SIDE EFFECTS
  None.
===========================================================================*/
sint15
dss_last_netdownreason
(
  sint15  dss_nethandle,                                      /* Application id    */
  dss_net_down_reason_type  *reason,                /* network down reason */
  sint15  *dss_errno                                  /* error number      */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_last_netdownreason);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);

} /* dss_last_netdownreason() */

/*===========================================================================
FUNCTION DSS_GET_APP_PROFILE_ID

DESCRIPTION
   Gets the application profile id.

DEPENDENCIES
  None.

PARAMETERS
  app_type            application type

RETURN VALUE
  Returns profile_id.

SIDE EFFECTS
  None.

===========================================================================*/
int32 dss_get_app_profile_id
(
  uint32 app_type
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_get_app_profile_id);
  return (DSS_ERROR);

} /* dss_get_app_profile_id() */

/*===========================================================================
FUNCTION DSS_PING_INIT_OPTIONS

DESCRIPTION
  This function initializes the ping options (e.g, number of times to ping
  the destination) to default values.

DEPENDENCIES
  None.

RETURN VALUE
  On success, return DSS_SUCCESS. On error, return DSS_ERROR.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_ping_init_options
(
  dss_ping_config_type  *ping_configs
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ping_configs == NULL )
  {
    LOG_MSG_INFO1("dss_ping_init_options(): "
                  "NULL pointer", 0, 0, 0);
  }

  DS_SOCKETS_PERROR(dss_ping_init_options);
  return (DSS_ERROR);
} /* dss_ping_init_options() */

/*===========================================================================
FUNCTION DSS_PING_START()

DESCRIPTION
 This function is used to ping the specified IP address. It is non-blocking.
 The user is notified of the ping results through the user-specified
 callback function.

DEPENDENCIES
  The ping options are specified through the dss_ping_config_type structure.
  So, before invoking the dss_ping() API, the user MUST initialize the
  dss_ping_config_type structure by calling dss_ping_init_options().
  Once the structure has been initialized with default ping options, the user
  can overwrite these default options with her own specific options.

RETURN VALUE
  On success, return a valid ping handle. On error, return DSS_ERROR and place
  the error condition in errno.


SIDE EFFECTS
  None.
===========================================================================*/
dss_ping_handle dss_ping_start
(
  dss_net_policy_info_type                *net_policy_ptr,
  char                                    *dest_addr_ptr,
  dss_ping_ip_addr_enum_type              dest_addr_type,
  dss_ping_config_type                    *app_ping_options,
  dss_ping_callback_fn_type               app_callback_fn,
  dss_ping_sess_summary_callback_fn_type  app_sess_summary_callback_fn,
  void                                    *app_user_data,
  int16                                   *dss_errno
)
{
  DS_SOCKETS_PERROR(dss_ping_start);
  return (DSS_ERROR);
} /* dss_ping_start() */

/*===========================================================================
FUNCTION DSS_PING_STOP()

DESCRIPTION
 Stop pinging and close the ping session.

RETURN VALUE
  DSS_SUCCESS: if a correct ping_handle was specified.
  DSS_ERROR: if an incorrect ping_handle was specified.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_ping_stop
(
  dss_ping_handle  ping_handle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_ping_stop);
  return (DSS_ERROR);
} /* dss_ping_stop() */

/*===========================================================================
FUNCTION DSS_IFACE_IOCTL()

DESCRIPTION
  This function dstermines the ps_iface_ptr associated with the passed in
  identifier. It then calls ps_iface_ioctl().

DEPENDENCIES
  None.

PARAMETERS

  dss_iface_id_type         - Interface ID on which the specified operations
                              is to be performed

  dss_iface_ioctl_type      - The operation name

  void*                     - Pointer to operation specific structure

  sint15*                   - Error code returned in case of failure (Error
                              values are those defined in dserrno.h)

                              DS_EBADF - Returned by dss_iface_ioctl() if the
                              specified id_ptr is invalid (i.e. id_ptr does
                              not map to a valid ps_iface_ptr).

                              DS_EINVAL - Returned by dss_iface_ioctl() when
                              the specified IOCTL does not belong to the
                              common set of IOCTLs and there is no IOCTL mode
                              handler registered for the specified interface.

                              DS_EOPNOTSUPP - Returned by the lower level
                              IOCTL mode handler when specified IOCTL is not
                              supported by the interface. For instance, this
                              would be returned by interfaces that do not
                              support a certain "iface specific common IOCTL"
                              (i.e. these are common IOCTLs, but the
                              implementation is mode specific, for example,
                              GO_DORMANT).

                              DS_EFAULT - This error code is returned if the
                              specified arguments for the IOCTL are incorrect
                              or if dss_iface_ioctl() or a mode handler
                              encounters an error while executing the IOCTL..
                              For instance, if the 1X interface cannot
                              "GO_DORMANT" it would return this error.

                              DS_NOMEMORY - This error code is returned if we
                              run out of buffers during execution.

RETURN VALUE
  0 - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
int dss_iface_ioctl
(
  dss_iface_id_type    iface_id,
  dss_iface_ioctl_type ioctl_name,
  void                 *argval_ptr,
  sint15               *dss_errno
)
{
  /* Set return error code */
  *dss_errno = DS_EBADF;
  DS_SOCKETS_PERROR(dss_iface_ioctl);
  return (DSS_ERROR);
} /* dss_iface_ioctl */

#endif /* DS_SOCKETS_STUBS is always off */

#ifndef FEATURE_DATA_PS_QOS
/*===========================================================================
FUNCTION DSS_GET_QOS_SHARE_HANDLE()

DESCRIPTION
  This function returns a qos share handle to the set of QOS instances
  requested using the specified nethandle. Applications can only use this
  handle to set the QOS_SHARE_HANDLE socket option for sockets created using
  socket2. Setting the share handle will enable sockets to use the QOS
  instances created by the nethandle.

DEPENDENCIES
  None.

PARAMETERS
  net_handle: network handle

RETURN VALUE


SIDE EFFECTS
  None.
===========================================================================*/
int32 dss_get_qos_share_handle
(
  sint15  net_handle,
  sint15  *dss_errno
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_SOCKETS_PERROR(dss_get_qos_share_handle);
  *dss_errno = DS_EFAULT;
  return (DSS_ERROR);
}
#endif /* FEATURE_DATA_PS_QOS */

//#ifndef _NO_DNS_STUBS is always defined
#if 0
/*===========================================================================
FUNCTION  DSS_DNS_INIT()

DESCRIPTION
  Initialize the DNS subsystem during powerup.

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  PS mem and Q subsystems must be initialized.

SIDE EFFECTS
  None.
===========================================================================*/
void dss_dns_init
(
  void
)
{
  PS_DNS_PERROR(dss_dns_init);
  return;
} /* dss_dns_init */

/*===========================================================================

FUNCTION DSS_DNS_GETIPNODEBYNAME

DESCRIPTION
  This function performs a DNS resolution on the passed domain name.  It
  returns the resolved mapping in a dss_hostent structure.  In case the
  domain name is a dotted decimal ipv4 or dotted decimal / colon seperated
  ipv6 address it is copied to the dss_hostent structure and returned.  If
  a mapping is not found the return value is NULL and the appropriate error
  code is set in dss_errno.  In case the dss_errno value is DS_EWOULDBLOCK
  the resolver was unable to locate the dns mapping in its local cache (if
  one exists) and would query the DNS servers and return the mapping by
  invoking the callback function.

  The returned dss_hostent structure is freed by calling the
  dss_freehostent() function.

  At present support for IPv6 is not added.

PARAMETERS
  name              string with name of an ipv4 or ipv6 host or dotted
                    decimal ipv4 or colon seperated/dotted decimal ipv6 host
  af                address family of name.  DSS_AF_INET or DSS_AF_INET6
  flags             options - none currently supported.
  callback          callback function pointer
  cb_voidptr        voidptr passed in by the application.  This is returned
                    back to the application in the callback.
  dss_errno         error code returned

DEPENDENCIES
  None.

RETURN VALUE
  Address of a dss_hostent structure if successful.
  If no mapping found then returns a NULL and the *dss_errno is set to one of
  the following values.

  dss_errno values
  ----------------
  DS_EHOSTNOTFOUND    domain name asked is not known.
  DS_ETRYAGAIN        temporary and transient error e.g., resolver out of
                      resources, server temporarily unreachable.
  DS_ENORECOVERY      irrecoverable server error occurred
  DS_EFAULT           invalid parameters passed to function
  DS_EAFNOSUPPORT     invalid value for the address family parameter
  DS_EOPNOTSUPP       invalid value for the flags parameter
  DS_ENOMEM           out of memory in the DNS subsystem
  DS_NAMEERR          domain name is malformed
  DS_EWOULDBLOCK      DNS servers being queried would invoke the callback
                      with the answer

SIDE EFFECTS
  Memory allocated to hostent structures is from DNS subsystem heap. The
  application must copy the hostent results to its memory and call
  dss_freehostents().

===========================================================================*/
struct dss_hostent *dss_getipnodebyname
(
  char                            * hostname_ptr,
  int32                             addr_family,
  int32                             flags,
  dss_dns_hostent_cback_f_type      cback_f_ptr,
  void                            * user_data_ptr,
  int16                           * dss_errno
)
{
  PS_DNS_PERROR(dss_getipnodebyname);
  *dss_errno = DS_EBADF;
  return NULL;
} /* dss_getipnodebyname */

/*===========================================================================

FUNCTION DSS_GETIPNODEBYADDR

DESCRIPTION
  This function performs reverse DNS resolutions on the passed host address
  returning the resolved mapping in a dss_hostent structure.  If a mapping
  is not found the return value is NULL and the appropriate error code is
  set in dss_errno.  In case the dss_errno value is DS_EWOULDBLOCK the
  resolver was unable to locate the dns mapping in its local cache (if one
  exists) and would query the DNS servers returning the mapping by invoking
  the callback function.

  The returned dss_hostent structure is freed by calling the
  dss_freehostent() function.

  At present support for IPv6 is not added.

PARAMETERS
  addr              based on the value of 'af' this will be either a
                    'struct ps_in_addr' (for af == DSS_AF_INET)  or 'struct
                    ps_in6_addr' (for af == DSS_AF_INET6)
  len               length of the addr field
  af                address family of addr DSS_AF_INET or DSS_AF_INET6
  callback          callback function pointer
  cb_voidptr        voidptr passed in by the application.  This is returned
                    back to the application in the callback.
  dss_errno         error code returned

DEPENDENCIES
  None.

RETURN VALUE
  Address of a hostent structure if successful.
  If no mapping found then returns a NULL and the *dss_errno is set to one of
  the following values.

  dss_errno values
  ----------------
  DS_EHOSTNOTFOUND    the specified host is unknown
  DS_ETRYAGAIN        temporary and transient error e.g., resolver out of
                      resources, server temporarily unreachable.
  DS_EFAULT           invalid parameters passed to function
  DS_EAFNOSUPPORT     invalid value for the address family parameter
  DS_ENORECOVERY      irrecoverable server error occurred
  DS_ENOMEM           out of memory in the DNS subsystem
  DS_NAMEERR          query is malformed
  DS_EWOULDBLOCK      query being processed would invoke the callback
                      with the answer

SIDE EFFECTS
  Memory allocated to hostent structures is from DNS subsystem heap. The
  application must copy the hostent results to its memory and call
  dss_freehostents().

===========================================================================*/
struct dss_hostent *dss_getipnodebyaddr
(
  void                            * addr_ptr,
  int32                             addr_len,
  int32                             addr_family,
  dss_dns_hostent_cback_f_type      cback_f_ptr,
  void                            * user_data_ptr,
  int16                           * dss_errno
)
{
  PS_DNS_PERROR(dss_getipnodebyaddr);
  *dss_errno = DS_EBADF;
  return NULL;
} /* dss_getipnodebyaddr */

/*===========================================================================
FUNCTION DSS_FREEHOSTENT

DESCRIPTION
  This function is called to free the dss_hostent structure returned by
  dss_getipnodebyname() or dss_getipnodebyaddr() functions.  It also frees
  all associated data structures used to resolve the query.

PARAMETERS
  phostent    pointer to dss_hostent structure to be freed

DEPENDENCIES
  The query entry to be freed should be in the valid q

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dss_freehostent
(
  struct dss_hostent *phostent         /* Pointer to dss_hostent to free   */
)
{
  PS_DNS_PERROR(dss_freehostent);
  return;
} /* dss_freehostent */

/*===========================================================================
FUNCTION DSS_INET_ATON

DESCRIPTION
  Convert internet address from dotted string to network ordered struct
  ps_in_addr.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_ERROR    For failure
  DSS_SUCCESS  For success

  The IP address is returned in struct ps_in_addr *addr

SIDE EFFECTS
  None.
===========================================================================*/
int32 dss_inet_aton
(
  const  char       *cp,               /* IPv4 addr in dotted quad string  */
  struct ps_in_addr *addr              /* Returned IPv4 address            */
)
{
  PS_DNS_PERROR(dss_inet_aton);
  return DSS_ERROR;
} /* dss_inet_aton */

/*===========================================================================
FUNCTION  DSS_INET_NTOA

DESCRIPTION
  Convert network order internet address to dotted string form.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS   If successful
  DSS_ERROR     If an error occured

SIDE EFFECTS
  None.
===========================================================================*/
int32 dss_inet_ntoa
(
  const struct ps_in_addr  in,     /* IPv4 address to be converted         */
  uint8                   *buf,    /* Buffer to hold the converted address */
  int32                    buflen  /* Length of buffer                     */
)
{
  PS_DNS_PERROR(dss_inet_ntoa);
  return DSS_ERROR;
} /* dss_inet_ntoa */

/*===========================================================================
FUNCTION  DSS_INET_PTON

DESCRIPTION
  This function is called with a presentation (printable or ASCII) format
  address to be converted to its network address (binary) format.  The af
  argument can be either DSS_AF_INET if the address to be converted is an IPv4
  address or AF_INET6 if the address is an IPv6 address.  In case of error
  the error code is returned in the dss_errno argument.

DEPENDENCIES
  The dst argument should have sufficient memory for the network address
  of the appropriate family.  For IPv4 it should be at least
  sizeof(struct ps_in_addr) while for IPv6 it should be at least
  sizeof(struct ps_in6_addr).

RETURN VALUE
  DSS_SUCCESS in case of success with the network format address
              returned in the dst argument.
  DSS_ERROR   in case of error with the error code returned in dss_errno
              argument.

              dss_errno values returned:
              DS_EFAULT         invalid arguments passed to function
              DS_EAFNOSUPPORT   invalid value for the address family
                                argument
              DS_NAMEERR        Malformed address passed to be converted
              DS_EMSGTRUNC      Insufficient buffer space in return argument

SIDE EFFECTS
  Returns the converted printable format IPv6 address in the dst argument.
  Any errors are returned in dss_errno argument.
===========================================================================*/
int32 dss_inet_pton
(
  const char *src,       /* String containing presentation form IP address */
  int32       af,        /* Address family of address in src argument      */
  void       *dst,       /* Memory for returning address in network format */
  uint32      dst_size,  /* Size of memory passed in dst argument          */
  int16      *dss_errno  /* Error code returned in case of DSS_ERROR return*/
)
{
  PS_DNS_PERROR(dss_inet_pton);
  *dss_errno = DS_EBADF;
  return DSS_ERROR;
} /* dss_inet_pton */

/*===========================================================================
FUNCTION  DSS_INET_NTOP

DESCRIPTION
  This function is called with an IPv4 or IPv6 address in network (binary)
  format to be converted into the presentation (dotted decimal for IPv4
  or colon seperated for IPv6) format.  The af argument can be either
  DSS_AF_INET if the address to be converted is an IPv4 address or DSS_AF_INET6 
  if the address is an IPv6 address.  In case of error the error code is
  returned in the dss_errno argument.

DEPENDENCIES
  The dst argument should have sufficient memory for the string address.
  To convert an IPv4 address it is suggested to be PS_IN_ADDRSTRLEN
  in size while to convert an IPv6 address it is suggested to be 
  PS_IN6_ADDRSTRLEN.

RETURN VALUE
  DSS_SUCCESS in case of success with the presentation format address
              returned in the dst argument.
  DSS_ERROR   in case of error with the error code returned in dss_errno
              argument.

              dss_errno values returned:
              DS_EFAULT         invalid arguments passed to function
              DS_EAFNOSUPPORT   invalid value for the address family
                                argument
              DS_NAMEERR        Malformed address passed to be converted
              DS_EMSGTRUNC      Insufficient buffer space in return argument
              DS_ENORECOVERY    Failed to convert

SIDE EFFECTS
  Returns the converted printable format IPv6 address in the dst argument.
  Any errors are returned in dss_errno argument.
===========================================================================*/
int32 dss_inet_ntop
(
  const char *src,       /* Memory containing network form IP address      */
  int32       af,        /* Address family of address in src argument      */
  void       *dst,       /* Memory to return address in presentation format*/
  uint32      dst_size,  /* Size of memory passed in dst argument          */
  int16      *dss_errno  /* Error code returned in case of DSS_ERROR return*/
)
{
  PS_DNS_PERROR(dss_inet_ntop);
  *dss_errno = DS_EBADF;
  return DSS_ERROR;
} /* dss_inet_ntop */

/*===========================================================================
FUNCTION  DSS_DNS_CREATE_SESSION()

DESCRIPTION
  The function creates a session control block corresponding to a set of DNS
  lookups. The created session must be cleaned by the application after
  all the DNS lookups are complete. The application MUST specify callback
  information while creating a session.

PARAMETERS
  app_cback_f_ptr   - Application registered callback function pointer.
  user_data_ptr     - Application user data. (Optional)
  dss_errno             - Error code returned in case of error.

DEPENDENCIES
  None.

RETURN VALUE
  Session handle on success
  DSS_DNS_SESSION_MGR_INVALID_HANDLE on error and sets the dss_errno to
  error code.

  dss_errno values
  ------------
  DS_EFAULT     - NULL callback info provided.
  DS_ENOMEM     - No memory available to allocate control block.

SIDE EFFECTS
  None.
===========================================================================*/
dss_dns_session_mgr_handle_type dss_dns_create_session
(
  dss_dns_cback_f_type    app_cback_f_ptr,
  void                  * user_data_ptr,
  int16                 * dss_errno
)
{
  PS_DNS_PERROR(dss_dns_create_session);
  *dss_errno = DS_EBADF;
  return DSS_ERROR;
} /* dss_dns_create_session */

/*===========================================================================
FUNCTION  DSS_DNS_SET_CONFIG_PARAMS()

DESCRIPTION
  This function sets a specified configuration parameter for a given session.

PARAMETERS
  session_handle    - Identifies the session.
  param_name        - Name of the configuration parameter to be set.
  param_val_ptr     - Value of the parameter to be set.
  param_len         - Identifies the length of the param_val_ptr that
                      can be used.
  dss_errno             - Error code in case of error.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR on failure and sets error code in dss_errno.

  dss_errno values
  ------------
  DS_EBADF       - Wrong session handle
  DS_EFAULT      - Invalid arguments.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_dns_set_config_params
(
  dss_dns_session_mgr_handle_type     session_handle,
  dss_dns_config_params_enum_type     param_name,
  void                              * param_val_ptr,
  uint32                              param_len,
  int16                             * dss_errno
)
{
  PS_DNS_PERROR(dss_dns_set_config_params);
  *dss_errno = DS_EBADF;
  return DSS_ERROR;
} /* dss_dns_set_config_params */

/*===========================================================================
FUNCTION  DSS_DNS_GET_CONFIG_PARAMS()

DESCRIPTION
  This function gets a specified configuration parameter for a given session.

PARAMETERS
  session_handle    - Identifies the session.
  param_name        - Name of the configuration parameter to get.
  param_val_ptr     - Value of the parameter to be set.
  param_len         - Identifies the length of the param_val_ptr that
                      can be filled in.
  dss_errno             - Error code in case of error.

DEPENDENCIES
  None.

RETURN VALUE

  DSS_SUCCESS on success: param_val_ptr contains the result and param_len
  contains the length of the results.
  DSS_ERROR on failure and sets error code in dss_errno.

  dss_errno values
  ----------------
  DS_EBADF       - Wrong session handle
  DS_EFAULT      - Invalid arguments.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dss_dns_get_config_params
(
  dss_dns_session_mgr_handle_type     session_handle,
  dss_dns_config_params_enum_type     param_name,
  void                              * param_val_ptr,
  uint32                              param_len,
  int16                             * dss_errno
)
{
  PS_DNS_PERROR(dss_dns_get_config_params);
  *dss_errno = DS_EBADF;
  return DSS_ERROR;
} /* dss_dns_get_config_params */

/*===========================================================================
FUNCTION  DSS_DNS_DELETE_SESSION()

DESCRIPTION
  This function deletes a session specified by the session handle.

DEPENDENCIES
  None

PARAMETERS
  session_handle  - Session identifier
  dss_errno           - Error code in case of error

RETURN VALUE
  DSS_SUCCESS on success.
  DSS_ERROR on failure and sets the error code in dss_errno.

  dss_errno values
  ------------
  DS_EBADF     - Wrong session handle

SIDE EFFECTS
  All the active queries associated with the session will be silently
  dropped without notifying the application.
===========================================================================*/
int16 dss_dns_delete_session
(
  dss_dns_session_mgr_handle_type     session_handle,
  int16                             * dss_errno
)
{
  PS_DNS_PERROR(dss_dns_delete_session);
  *dss_errno = DS_EBADF;
  return DSS_ERROR;
} /* dss_dns_delete_session */

/*===========================================================================
FUNCTION  DSS_DNS_GET_ADDRINFO()

DESCRIPTION
  This function starts the resolver to get a list of dss_dns_addrinfo
  structures. This is a blocking function. After the query operations are
  complete, the application would be notified using the callback registered
  with the session manager. The application must call dss_dns_read_addrinfo()
  funtion to read the results.

  Currently service names are not supported. Hence servname argument must
  be NULL or a numeric(decimal) port string.

PARAMETERS
  session_handle - Handle to a valid session control block.
                   (obtained by calling dss_dns_create_session)

  hostname       - Hostname argument
                   Can be a NULL pointer or a IPv4 dotted decimal string
                   or IPv6 colon seperated string or a valid domain name.

  servname       - Currently service names are not supported. Hence this
                   argument must be NULL or a decimal port string.

  hints          - A dss_dns_addrinfo struct pointer which directs the
                   operation of the resolver by providing input values
                   or by limiting the return information.

                   A NULL hints parameter is treated as one with
                   ai_family=AF_UNSPEC, ai_socktype=0, ai_protocol=0,
                   and ai_flags=0

                   Only the following fields of hints are processed.

                   ai_family
                     DSS_AF_UNSPEC/0 : Caller shall accept any address family
                     DSS_AF_INET     : Caller shall accept only DSS_AF_INET
                     DSS_AF_INET6    : Caller shall accept only DSS_AF_INET6

                   ai_socktype
                     0           : Caller shall accept any socktype
                     DSS_SOCK_STREAM : Caller shall accept DSS_SOCK_STREAM only
                     DSS_SOCK_DGRAM  : Caller shall accept DSS_SOCK_DGRAM only

                   ai_protocol
                     0           : Caller shall accept any protocol
                     PS_IPPROTO_TCP : Caller shall accpet PS_IPPROTO_TCP only
                     PS_IPPROTO_UDP : Caller shall accpet PS_IPPROTO_UDP only

                   ai_flags

                     AI_PASSIVE
                       If hostname argument is NULL, INADDR_ANY or
                       IN6ADDR_ANY will be returned depending upon
                       supported family for the iface and ai_family
                       parameter of the hints message. Ignored if hostname
                       is non-NULL.

                     AI_CANONICAL
                       If the hostname is non-NULL attempts to find the
                       canonical name. Ignored if hostname is NULL.

                     AI_NUMERICHOST
                       If the hostname is non-NULL and contains a valid
                       V4 or V6 numeric address, that will be returned.
                       Otherwise dss_errno will be set to DS_NAMEERR.

                     AI_NUMERICSERV
                       This flag is currently ignored.

                     AI_V4MAPPED
                       If address family is AF_INET6/AF_UNSPEC and no
                       IPv6 adresses are found, all IPv4 addresses found
                       will be returned as IPv4-mapped-IPv6 address.
                       Ignored if address family is AF_INET.

                     AI_ALL
                       All addresses found will be returned IPv6 format.
                       Ignored if AI_V4MAPPED is not set.

                     AI_ADDRCONFIG
                       Addresses configured on local systems are returned.

                   A NULL hints parameter is treated as one with
                   ai_family=AF_UNSPEC, ai_socktype=0, ai_protocol=0,
                   and ai_flags=0

RETURN VALUE
  On success, returns a query handle identifying the query started.
  On error, returns DSS_DNS_QUERY_INVALID_HANDLE and sets the
  dss_errno to the error code.

  dss_errno values
  ----------------
  DS_EFAULT     - Invalid arguments
  DS_EBADF      - Invalid session handle
  DS_ENOMEM     - Out of memory
  DS_NAMERR     - Malformed query
  DS_EWOULBLOCK - Operation would block

DEPENDENCIES
  A valid DNS session must be created prior to calling this function.

SIDE EFFECTS
  None.
===========================================================================*/
dss_dns_query_handle_type dss_dns_get_addrinfo
(
  dss_dns_session_mgr_handle_type     session_handle,
  const char                        * hostname_ptr,
  const char                        * service_ptr,
  const struct dss_dns_addrinfo     * hints_ptr,
  int16                             * dss_errno
)
{
  PS_DNS_PERROR(dss_dns_get_addrinfo);
  *dss_errno = DS_EBADF;
  return DSS_ERROR;
} /* dss_dns_get_addrinfo */

/*===========================================================================
FUNCTION  DSS_DNS_GET_NAMEINFO()

DESCRIPTION
  This function queries for dss_dns_nameinfo records. This function is used
  to convert a socket address to a pair of hostname and service name
  arguments. Currently service name argument is not supported.

PARAMETERS

  session_handle  - Session Identifier
  sa_ptr          - Socket address ptr
  sa_len          - Socket address length
  flags           - Flags
  dss_errno       - Error code

RETURN VALUE
  Returns a handle to the query started on success
  Returns DSS_DNS_QUERY_INVALID_HANDLE on error.

  dss_errno values
  ------------
  DS_EFAULT     - Invalid arguments
  DS_EBADF      - Invalid session handle
  DS_ENOMEM     - Out of memory
  DS_NAMERR     - Malformed query.
  DS_EWOULBLOCK - Operation would block

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
dss_dns_query_handle_type dss_dns_get_nameinfo
(
  dss_dns_session_mgr_handle_type     session_handle,
  const struct ps_sockaddr          * sa_ptr,
  uint16                              sa_len,
  uint32                              flags,
  int16                             * dss_errno
)
{
  PS_DNS_PERROR(dss_dns_get_nameinfo);
  *dss_errno = DS_EBADF;
  return DSS_ERROR;
} /* dss_dns_get_nameinfo */

/*===========================================================================
FUNCTION  DSS_DNS_READ_ADDRINFO()

DESCRIPTION
  This function reads the dss_dns_addrinfo records which were looked up
  for earlier using dss_dns_get_addrinfo.

PARAMETERS
 session_handle -  Session Identifier
 query_handle   -  Query identifier
 results_buf    -  Results buffer
 num_records    -  Number of records
 dss_errno          -  Error code

RETURN VALUE
  DSS_SUCCESS on success and reads the results into the results buffer
  DSS_ERROR on error.

  dss_errno values
  ----------------
  DS_EBADF       - Invalid session or query handle
  DS_EWOULDBLOCK - Operation not complete yet.

DEPENDENCIES
  This function must be called after the application has been signalled
  using callback function after dss_dns_get_addrinfo.

SIDE EFFECTS
  The API control block instance, as well as the results Q are freed.
===========================================================================*/
int16 dss_dns_read_addrinfo
(
  dss_dns_session_mgr_handle_type     session_handle,
  dss_dns_query_handle_type           query_handle,
  dss_dns_addrinfo                  * results_buf,
  uint16                              num_records,
  int16                             * dss_errno
)
{
  PS_DNS_PERROR(dss_dns_read_addrinfo);
  *dss_errno = DS_EBADF;
  return DSS_ERROR;
} /* dss_dns_read_addrinfo */

/*===========================================================================
FUNCTION  DSS_DNS_READ_NAMEINFO()

DESCRIPTION
  This function reads the dss_dns_nameinfo records which were looked up
  for earlier using dss_dns_get_nameinfo.

PARAMETERS
 session_handle -  Session Identifier
 query_handle   -  Query identifier
 results_buf    -  Results buffer
 num_records    -  Number of records
 dss_errno          -  Error code

RETURN VALUE
  DSS_SUCCESS on success and reads the results into the results buffer
  DSS_ERROR on error.

  dss_errno values
  ----------------
  DS_EBADF       - Invalid session or query handle
  DS_EWOULDBLOCK - Operation not complete yet.

DEPENDENCIES
  This function must be called after the application has been signalled
  using callback function after dss_dns_get_nameinfo.

SIDE EFFECTS
  The API control block instance, as well as the results Q are freed.
===========================================================================*/
int16 dss_dns_read_nameinfo
(
  dss_dns_session_mgr_handle_type     session_handle,
  dss_dns_query_handle_type           query_handle,
  dss_dns_nameinfo                  * results_buf,
  uint16                              num_records,
  int16                             * dss_errno
)
{
  PS_DNS_PERROR(dss_dns_read_nameinfo);
  *dss_errno = DS_EBADF;
  return DSS_ERROR;
} /* dss_dns_read_nameinfo */

#endif  /* _NO_DNS_STUBS */
#endif  /* FEATURE_DATA_PS_QOS */
