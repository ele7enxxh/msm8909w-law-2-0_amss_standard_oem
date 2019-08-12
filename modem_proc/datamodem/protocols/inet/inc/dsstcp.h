#ifndef DSSTCP_NEW_H
#define DSSTCP_NEW_H

/*===========================================================================

     D A T A  S E R V I C E S  S O C K E T  T C P  H E A D E R  F I L E

DESCRIPTION

 The Data Services sockets TCP interface file. This contains all the
 functions used by the sockets library to access TCP socket function calls.

EXTERNALIZED FUNCTIONS

  dsstcp_socket()
    TCP specific socket() function.  Sets socket state to INIT.

  dsstcp_connect()
    Connects a TCP socket.

  dsstcp_listen()
    Does the passive open for listener socket child sockets.

  dsstcp_accept()
    Removes and return the a child socket from a listener sockets
    ready queue. This will also try to refill the half open
    connections queue.

  dsstcp_write()
    TCP specific write() function.  Sends the dsm memory item out
    the TCP transport.

  dsstcp_close()
    TCP specific close() function.  Initiates the TCP active close.
    If the connection is already closed, cleans up the TCP socket and
    associated resources.

  dsstcp_bind()
    This is the TCP specific bind() function. It writes the port value into
    local.conn.port of the socket control block structure.

  dsstcp_setopt
    This function sets the tcp option value to the user specified value.

  dsstcp_flow_enabled_ind
    This function calls tcp_output() to send data out once socket is
    flow enabled

  dsstcp_lookup_conn()
    Given a connection structure, looks up the connection to determine
    if this is a valid connection.

  dsstcp_lookup_serv()
    Given a connection structure, looks up to determine if there are
    any valid listener sockets(server socket)

  dsstcp_new_ip_upcall()
    If a new IP address is negotiated with the Base Station, then send a
    reset to the peer, and then close down sockets and notify the user.


Copyright (c) 1998-2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsstcp.h_v   1.2   13 Feb 2003 12:21:28   ssinghai  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/dsstcp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/13/06   msr     Added APP_TO_PS_SOCKET_FLOW_ENABLED_IND
07/31/06   rt      Route Scope modifications.
03/27/06   rt      Added a new function dsstcp_window_reopen.
08/15/05   vp      Addition of flags argument to dsstcp_read().
04/18/05   vp      Change in signature of bind and listen functions.
08/16/04   ks/msr  Removed dsstcp_init_tcb_array function.
08/30/04   sv      Moved graceful dormant close flag and keep alive idle time
                   constants to socket config control block.
04/14/04   vp      Added declaration for tcp_keepalive_idle_time.
03/30/04   vp      Merged changes from June04Dev_05.00.00 PS branch.
01/27/04   vp      Added declaration of tcp_graceful_dormant_close flag
11/11/03   sv      Modified is_writeable to take scb_ptr instead of pcb.
10/17/03   sv      Modified protocol specific functions to take PCB instead
                   of SCB. Removed dsstcp_abort as it is no longer used.
09/09/03   sv      Added dsstcp_reset function to reset tcb.
08/18/03   sv      Modified the code to use IPv6 addresses internally.
08/08/03   ss/sv   Restructured the socket code.
06/27/03   sv      Modified dsstcp_is_writeable to return integer.
04/15/03   sv      Modified dsstcp_read to accept iov and iovcount as
                   arguments instead of buffer and bufferlen.
02/13/03   ss      Added dsstcp_shutdown()
12/22/02   aku     Changed dsstcp_new_ip_upcall() signature to take
                   ps_iface_ptr as argument instead of IP addresses.
05/02/02   mvl     File clean up, added meta_info_ptr to write() prototype
02/22/02   pjb     Added accept and listen
02/12/02   ss      Added prototype for dsstcp_cleanup_all_reseq().
02/11/02   ss      Added prototype for dsstcp_is_writeable().
10/11/01   mt      Add dss_errno argument to dsstcp_connect_setup().
03/27/01   mt      Changed type of second argument in dsstcp_read() from
                   byte * to void *.
03/27/01   js      Added extra parameter to function dsstcp_write(). The
                   extra parameter holds a pointer to the structure holding
                   the destination information. Though not used, the addition
                   is necessary to comply with the dss_fcn_ptr_table format.
02/08/01   yll     Added dsstcp_read_dsm_chain().
11/10/00   snn     Changed scb_type to scb_s
10/10/00   na      Removed tcp_ptr which is not longer used. Added prototype
                   for dsstcp_init_tcb_array().
08/03/00   mvl/js  Added prototype for dsstcp_bind() function.
03/22/00   rc/na   Added prototype for function dsstcp_new_ip_upcall().
09/22/99   rc      Incorporated code review changes and comments.
08/11/99   rc/ak   Extern for tcp abort function.
12/14/98   na/hcg  Created module.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_Sock_Def.h"
#include "ps_in.h"
#include "ps_socket_defs.h"
#include "ps_socket_event_defs.h"
#include "ps_rt_meta_info.h"
#include "ps_pkt_meta_info.h"


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSSTCP_SOCKET()

DESCRIPTION
  This function is the protocol-specific socket function.  For TCP, it
  just sets the socket state to INIT.

  This function is called from the context of the application task.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS

  On error, return DSS_ERROR and places the error condition value in *errno.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dsstcp_socket
(
  uint16     family,
  void    *  scb_handle,
  int32      sockfd,
  int32   *  pcb_handle_ptr,
  void    *  reader_ptr,
  void    *  writer_ptr,
  void    *  errq_ptr,
  int16   *  errno_ptr
);

/*===========================================================================
FUNCTION DSSTCP_CONNECT()

DESCRIPTION
  This function connects a TCP socket and should only be called when
  PPP/traffic channel is established.

  This function is called from the context of the PS  task.

DEPENDENCIES
  Before this function is called, the destination IP address/port number
  should have been put in the scb in the name and peername fields.

RETURN VALUE
  On success, returns DSS_SUCCESS.

SIDE EFFECTS
  None.
===========================================================================*/
void dsstcp_connect
(
  int32                     pcb_handle,
  int32                     sockfd,
  struct ps_sockaddr_in6 *  dest_addr_ptr
);

/*===========================================================================
FUNCTION DSSTCP_LISTEN()

DESCRIPTION
  This starts a passive open on a tcp socket.
  This is called from the context of the protocol stack
 // PJB Fill in from .c file....
DEPENDENCIES
  Before this is called the source port should be put in the socket
  control block with bind.
 // PJB Fill in from .c file....
RETURN VALUE
  Returns success on success, or error on error.  If there
  is an error dss_errno is filled in to indicate the error.

SIDE EFFECTS
  None.
===========================================================================*/
void dsstcp_listen
(
  int32  pcb_handle,
  int32  sockfd,
  int32  backlog,
  int32  rcv_window                              /* receive window size */
);

int32 dsstcp_accept
(
  int32                        pcb_handle,
  int32                        sockfd,
  ps_socket_event_info_type  * accept_event_info_ptr
);

/*===========================================================================
FUNCTION DSSTCP_WRITE()

DESCRIPTION
  Sends dsm memory item out to TCP.

  This function is called from the context of the PS task.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dsstcp_write
(
  int32                     pcb_handle,
  int32                     sockfd,
  int32                     pkt_length,
  struct ps_sockaddr_in6  * ta_ptr,
  ps_pkt_meta_info_type   * pkt_meta_info_ptr
);

/*===========================================================================
FUNCTION DSSTCP_CLOSE()

DESCRIPTION
  Initiates the active close for TCP.  If the connection is already closed,
  cleans up the TCP socket and associated resources.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS.

SIDE EFFECTS
  Initiates active close for TCP connections.
===========================================================================*/
void dsstcp_close
(
  int32    pcb_handle,
  int32    sockfd,
  boolean  silent_close_flag
);

/*===========================================================================
FUNCTION DSSTCP_BIND()

DESCRIPTION
  This is the TCP specific bind() function. It writes the port value into
  local.conn.port of the socket control block structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dsstcp_bind
(
  int32                     pcb_handle,
  int32                     sockfd,
  struct ps_sockaddr_in6 *  local_addr_ptr
);

/*===========================================================================
FUNCTION DSSTCP_SHUTDOWN()

DESCRIPTION
  This function implements the half-close functionality for tcp sockets.

DEPENDENCIES
  None.

PARAMETERS
  scb_ptr - pointer to scb which needs to be shut down
  item_ptr - a dsm item containing information about what kind of shutdown
             needs to be done (read, write or both)
  dss_errno - error number returned in case of error

RETURN VALUE
  On success, returns DSS_SUCCESS otherwise returns DSS_ERROR and places
  the error number in dss_errno.

SIDE EFFECTS
  None.
===========================================================================*/
void dsstcp_shutdown
(
  int32  pcb_handle,
  int32  sockfd,
  int32  shutdown_dir
);

/*===========================================================================
FUNCTION DSSTCP_SETOPT()

DESCRIPTION
  This function sets the option value to the user specified value.

DEPENDENCIES
  None.

PARAMETERS
  tcb_ptr - pointer to tcb

RETURN VALUE
  NONE.

SIDE EFFECTS
  None.
===========================================================================*/
void dsstcp_setopt
(
  int32  pcb_handle,
  int32  sockfd,
  int32  opt_level,
  int32  opt_name,
  int32  val,
  int32  len
);

/*===========================================================================
FUNCTION DSSTCP_FLOW_ENABLED_IND()

DESCRIPTION
  This function processes flow_enabld_ind on socket's routing and flow cache

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsstcp_flow_enabled_ind
(
  int32  pcb_handle,
  int32  sockfd
);

/*===========================================================================
FUNCTION DSSTCP_SET_ROUTE_META_INFO()

DESCRIPTION
  This function processes setting of routing meta info inside IPCB.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsstcp_set_route_meta_info
(
  int32                   pcb_handle,
  int32                   sockfd,
  ps_rt_meta_info_type *  rt_meta_info_ptr
);

/*===========================================================================
FUNCTION DSSTCP_SET_ROUTE_SCOPE()

DESCRIPTION
  This is the TCP specific set_route_scope() function. It writes the route
  scope in the TCB's IPCB

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsstcp_set_route_scope
(
  int32                  pcb_handle,
  int32                  sockfd,
  ps_route_scope_type  * route_scope_ptr
);

/*===========================================================================
FUNCTION DSSTCP_SET_ROUTE_META_INFO()

DESCRIPTION
  This function processes setting of routing meta info inside IPCB.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsstcp_set_linger_reset
(
  int32    pcb_handle,
  int32    sockfd,
  boolean  is_linger_on,
  int32    time_in_sec
);

/*===========================================================================
FUNCTION DSSTCP_CLEANUP_ALL_RESEQ()

DESCRIPTION
  This function cleans up the resequencing queue of all the TCBs. This
  fucntion should be called to free up memory potentially consumed by the
  segments lying in the resequencing queues.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dsstcp_cleanup_all_reseq
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* DSSTCP_NEW_H */
