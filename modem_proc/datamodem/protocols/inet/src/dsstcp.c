/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  T C P   F I L E

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

  dsstcp_read()
    TCP specific read() function.  Reads specified number of bytes from
    the TCP transport.

  dsstcp_read_dsm_chain()
    TCP specific read_dsm_chain() function.  Reads a DSM item chain from
    the TCP transport.

  dsstcp_close()
    TCP specific close() function.  Initiates the TCP active close.
    If the connection is already closed, cleans up the TCP socket and
    associated resources.

  dsstcp_bind()
    This is the TCP specific bind() function.  It writes the port number into
    the local.conn.port of the socket control block structure.

  dsstcp_setopt
    This function sets the tcp option value to the user specified value.

  dsstcp_register_upcall
    This function registers upcall function and user data with the TCB.

  dsstcp_flow_enabled_ind
    This function calls tcp_output() to send data out once socket is
    flow enabled

Copyright (c) 1998-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsstcp.c_v   1.9   13 Feb 2003 12:20:46   ssinghai  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dsstcp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/20/15    ss     Add IP family check before picking up TCP server socket
01/02/13    ssh    Local TCP connections should use IP layer-supplied MTU
10/01/10    sa     Added appropriate log messages before ASSERT(0).
06/15/10    vm     IDLE event related fixes
05/22/09    kk     Q6 compiler warning fixes.
03/17/03    mct    Ensure we clear the source ip addr when our ip changes.
11/08/07    hm     Fixed a bug in TCP RTT computation
03/16/07    scb    Fixed Klocwork High errors
02/07/07    scb    Fixed Klocworks high errors
02/05/07    rt     Removed EIFEL code.
12/05/06    msr    Passing sockfd in ps_event_info_type
11/22/06    sv     Added support for IPSEC transport mode support.
11/03/06    hm     Checking the return value of dssocki_send_to_ps. Using
                   dssocki_alloc_ps_cmd_buffer and dssocki_send_ps_cmd_buffer
                   instead of dssocki_send_to_ps in many places.
11/02/06    mct    Updates for allowing binding to local IPv6 addresses.
10/19/06    msr    Freeing TCB's rcvq in dsstcp_close() as well
10/13/06    msr    Handling APP_TO_PS_SOCKET_FLOW_ENABLED_IND
10/13/06    hm     RTT related code cleanup
09/27/06    rt     Setting data_available flag as FALSE if tcb rcvcnt is 0
                   in dsstcp_read functions.
08/02/06    ks     Merded Fix for RCVBUF casting issue in open_tcp call.
07/31/06    rt     Route Scope modifications.
06/13/06    msr    Making sure that timers are non-NULL before manipulating
                   them. Async TCB is not creating all the timers
04/24/06    mct    Drop TCP packets if routing_cache is null.
03/27/06    rt     L4 TASKLOCK review changes.
03/13/06    msr    Fixed bug in dsstcp_accept() where metainfo is not freed
03/13/06    msr    Fixed bug in dsstcp_bind() where metainfo is not freed
02/22/06    rt     Using single critical section
02/07/06    rt     L4 TASKLOCK changes.
01/31/05    rt     Added new F3 message in BIND function.
12/03/05    rt     Added code for new tcp option DSS_TCP_EIFEL.
12/03/05    rt     Added a new scb_ptr field in dssocki_send_to_ps() fn and
                   got rid of pcb and protocol fields.
09/26/05    msr    Fixed a compilation warning
08/30/05    vp     Check for backlog before generating meta info.
08/16/05    vp     Cleanup for propagating sys sock info to ipcb.
08/15/05    vp     Propagating sys sock info to ipcb.
08/15/05    kr     Added support for stats update
08/15/05    vp     Addition of flags argument to dsstcp_read(). Resetting
                   err_available flag when so_error is returned.
08/05/05    sv     Update metainfo when we get a new metainfo ptr in write.
06/13/05    vp     Support for SO_ERROR and SO_ERROR_SNABLE socket options.
04/22/05    vp     Send the tcb localname to open_tcp in dsstcp_listen().
04/18/05    vp     Changes for enforcing netpolicy on input.
04/17/05    msr    Using new signature of dssocki_generate_meta_info().
04/16/05    ks     Not checking Iface state any more, during dormant close.
04/04/05    rt     Fixed the problem of backlogs not freed if client sends a
                   reset after receiving the SYN/ACK from server.
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
03/01/05    vp     Changes due to change in signature of dssocki_fill_sockaddr
02/22/05    ssh    Replaced INTLOCK()s with TASKLOCK()s
02/15/05    msr    Calling ip_setopt() for IPV6 level options.
01/17/05    ks     removed call to PS_META_INFO_FREE macro from dsstcp_connect
01/10/05    sv     Merged IPSEC changes.
12/20/04    msr    Fixed dsstcpi_persist_timer_cb() to use
                   numb_bytes_in_transit instead of snd_ptr.
12/09/04    msr    Replaced ASSERTS in dsstcp_lookup_serv() with if check.
11/24/04    msr    memsetting tcb to 0 after it is freed in
                   dsstcpi_free_tcb().
10/25/04    msr    Added support for persist timer and added
                   dsstcpi_persist_timer_cb()
10/11/04    sv     Fixed dsstcpi_free_tcb to free SACK BLOCKS.
10/13/04    vp     Changes in dsstcpi_send_keepalive probe() for port byte
                   order and addition of tcp_hdr_create() instead of htontcp().
08/16/04    ks/msr Integrated TCBs in to ps_mem memory allocation. Maintaining
                   allocated TCBs in a list instead of array. Removed
                   dsstcp_init_tcb_array function.
08/30/04    sv     Moved graceful dormant close flag and keep alive idle time
                   constants to socket config control block.
08/19/04    vp     Changes for freeing up the tcb's only in PS context in
                   dsstcp_listen_setup().
08/19/04    vp     Set the local address and port in dsstcp_bind only when
                   tcb_ptr->sockfd != 0.
06/21/04    sv     Modified dsstcp_connect to set the user specified flow label
                   in IPCB.
06/11/04    vp     Removal of inclusion of psglobal.h, netuser.h ps_tcp.h.
                   Included files dssocket_defs.h, ps_tcp.h and ps_socket_defs,h
                   Changes for representation of IP addresses in struct ps_in_addr
                   or struct ps_in6_addr. Replaced use of TCP_PTCL with PS_IPPROTO_TCP
05/17/04    sv     Modified dsstcp_read to read a maximum of DSS_READ_MAX_BYTES
                   in a single read call.
05/14/04    sv     Updated init_tcb to reset the sndcnt and rcvcnt of TCB.
04/30/04    sv     Added TCP timestamp and SACK option. Changed the tcp
                   options type from uint32 to struct tcp_options_type.
04/28/04    mct    Changed max # of TCBs to DSS_MAX_TCP_SOCKS + 10.
04/26/04    vp     Removed unnecessary local variable if_v6addr in
                   dsstcp_connect
04/22/04    vp     Removal of the access of partial and established queues of
                   the scb from state_change_upcall.
04/14/04    vp     Changes for KEEPALIVE and DELAYED ACK as socket options.
03/30/04    vp     Merged changes from June04Dev_05.00.00 PS branch.
03/18/04    vp     Changes for removing direct scb access from PS context.
                   All such events are posted through info_change_upcall.
03/10/04    vp     Changes for dssocki_socket_event_occurred() to be executed
                   through upcall mechanism.
02/26/04    vp     Changed the name of setstate and close_self to tcp_setstate
                   and tcp_close_self
02/26/04    vp     Cleanup of dsstcpi_init_tcb()
02/04/04    vp     Replaced use of PS_IFACE_IP_V4_ADDR with ps_iface_get_addr
01/29/04    mct    Replaced DSS_LONG_CLOSE_INTERVAL with TCP_TIME_WAIT_INTERVAL
01/28/04    vp     Modified the if condition for abort during dormancy in
                   dsstcp_close
01/28/04    sv     Increased the initial TCP congestion window to 2*mss
01/27/04    vp     Modified dsstcp_is_writeable to enqueue minimum of SNDBUF
                   and Congestion window into sndq before PS task runs.
                   Modified dsstcpi_data_tx_upcall to update the cached cwind
                   in scb.
01/27/04    vp     Added definition of tcp_graceful_dormant_close flag.
                   Added check to close the dormant connection gracefully if
                   tcp_graceful_dormant_close flag is TRUE.
01/26/04    mct    Added additional check in read, read_dsm_chain, write,
                   shutdown, and setopt to check if tcb_ptr->sockfd == 0.
01/20/04    mct    Modified dsstcp_close() to return success if tcb->sockfd
                   is already equal to zero.
01/19/04    sv     Modified alloc_listener to get sock_cb from the scb
                   instead of acb.
01/14/04    vp     Replaced the call to reset_tcp with a call to close_self
                   in case of ip address change in dsstcp_new_ip_upcall()
01/07/04    vp     Modified dsstcpi_half_open_timer_cb to remove scb access
                   from ps context and use dsstcp_reset_tcb() instead.
12/02/03    sv     Changed tcp close reason to NETWORK if we abort the
                   connection because of network failure/down.
11/12/03    sv     Trigger socket event when we receive a fin from the peer
11/12/03    sv     Added check to make sure we don't enqueue more than
                   sndbuf size of data in tcp sndq.
10/20/03    sv     Modified the code in state_change_upcall to trigger the
                   socket closed event in TIME_WAIT state.
10/17/03    sv     Modified protocol specific functions to take PCB instead of
                   SCB.
10/01/03    sv     Modified dsstcp_close so that TCB is freed in PS context
                   and SCB is freed in application context. Removed tcp_abort.
09/24/03    sv     Changed bytes_available variable in dsstcp_read to uint16
                   to fix the larger window size problem.
09/09/03    sv     Added dsstcp_reset function to reset tcb. Changed
                   create_tcb to accept SCB as an argument.
08/18/03    sv     Modified the code to use IPv6 addresses internally.
08/08/03    sv     Fixed the duplicate syn problem.
08/08/03    ss/sv  Restructured the socket code.
06/27/03    sv     Modified dsstcp_is_writeable to use sndbuf in scb to
                   determine the number of bytes available in sndq.
06/16/03    ss     Modified dsstcp_close() to return immediately if the
                   socket state is null, wait for close or closed.
06/04/03    om     Fixed MSS setting to adjust for IPSec ESP overhead.
05/28/03    ss     Removed SNDQ_LIMIT from dsstcp_is_writeable()
05/13/03    ss     Modified code to insert the scb at the end of the estab
                   queue instead of at front for SYN_RECD->EST transition
05/04/03    ss     Modified state_change_upcall() to support SO_LINGER option
04/24/03    usb    Fixed bug in tcp_read where bytes read were not
                   getting updated correctly.
04/23/03    ss     Modified code to reflect new socket options handling
04/15/03    sv     Modify dsstcp_read to accept iov and iovcount instead of
                   buffer and length. Added support for tcp half open timer.
04/15/03    ss     Modified dsstcp_close() to transition socket state to
                   CLOSING only if the socket state is OPEN after close_tcp()
04/10/03    ss     Modified dsstcp_connect() to change the parameter to
                   getephem_port(). Modified dsstcp_shutdown() to correctly
                   test the value of how parameter.
03/07/03    js     Added event reporting for TCP.
02/13/03    ss     Added code to support socket shutdown
02/12/03    om     Use SCB's routing cache only.
12/22/02    aku    Changed dsstcp_new_ip_upcall() signature to take
                   ps_iface_ptr as argument instead of IP addresses.
11/19/02    mvl    removed JCDMA from window size socket option.
11/12/02    ss     Modified dsstcp_abort() so that the clean-up is done when
                   the socket is CLOSING also.
11/05/02    mvl    Added support for changing default window size.
10/30/02    ss     Moved the ASSERT() later in the code in dsstcp_close()
                   since tcb_ptr may be NULL for listener socket
10/17/02    ss     When in SynSent state, dsstcp_close() closes the socket
                   right away. data_available flag is reset when TCB is freed
                   so that the user cannot read data from a socket after TCP
                   reset.
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
09/25/02    om     Added setting of routing cache for LISTEN sockets.
09/19/02    na     Fixed returning of EEOF when in CLOSE_WAIT state.
09/10/02    aku    Removed global Ip_addr if FEATURE_DATA_MM is defined.
08/16/02    ss     Modified dsstcp_close() to support silent close option
08/04/02    aku    Merged in changes for multimode ps under FEATURE_DATA_MM
07/19/02    ss     Modified dsstcp_abort() to call dsstcp_close() in all the
                   states except OPEN and OPENING for necessary clean-up.
07/16/02    aku    Fixed the dsm kind names.
05/02/02    mvl    File clean up, added meta_info_ptr to write()
04/17/02    rc     Fixed bug in dsstcp_close() where INTFREE_SAV() was not
                   being called before returning from the function.
03/13/02    pjb    alloc_tcb was leaving tcb in funny state when timer
                   alloc failed.
03/05/02    pjb    Finished Accept Listen Code Review Items
02/22/02    pjb    Added accept and listen
02/11/02    ss     Added dsstcpi_data_tx_upcall() to notify application when
                   sndq can accept more data. Added dsstcp_is_writeable(),
                   accessed via fcn_ptr_table, to find out if sndq has space.
02/01/02    ss     The user now gets an EOF in case of passive close when
                   there is no more data to read.
12/18/01    rsl    Updated tracer field.
12/07/01    ss     Modified dsstcpi_state_change_upcall() not to call
                   close_tcp() when transitioning to CLOSED_WAIT state
12/07/01    ss     Modified dsstcp_abort() to not to free up scb
11/15/01    aku    dssocki_socket_event_occurred() takes an additonal
                   acb_ptr arg
11/09/01    ss     Changed dsstcpi_state_change_upcall() so that close_timer
                   is started only in FINWAIT2 and TIME_WAIT states. Modified
                   dsstcp_close() to perform TCP close in the PS context.
10/11/01    mt     Add dss_errno argument to dsstcp_connect_setup() and set
                   its contents accordingly.
07/23/01    pjb    Change location/prefix of command names.
05/18/01    yll    Removed an unnecessary assert statement in
                   dsstcp_read_dsm_chain().
04/16/01    js     Added a checking for TCP socket state in
                   dsstcp_new_ip_upcall() to avoid resetting the socket in
                   inappropriate states.
03/27/01    mt     Changed type of second argument in dsstcp_read() from
                   byte * to void *.
03/27/01    js     Added extra parameter to function dsstcp_write(). The
                   extra parameter holds a pointer to the structure holding
                   the destination information. Though not used, the addition
                   is necessary to comply with dss_fcn_ptr_table format.
02/08/01    yll    Added dsstcp_read_dsm_chain().
11/10/00    snn    Modified dword to uint32 and word to uint16.
                   Modified some comments.
                   Renamed callback functions with a postscript of _cb.
                   Removed errno as the parameter for timer functions,
                     because they are never used.

10/10/00    na     Added support for Multiple TCP sockets. Added include
                   files. Removed dsstcp_tcb_ptr because the TCB is now
                   accessed as an array.
                   Added a new array dsstcpi_arr.
08/25/00    rc     Changed dsm_new_buffer() calls to take pool id as input
                   parameter. So, a new macro DSM_DS_POOL_SIZE is used to
                   return pool id based on pool size.
08/03/00    mvl/js Added function dsstcp_bind() to support the protocol
                   specific assignment of local.conn.port.
03/22/00    rc/na  Added function dsstcp_new_ip_upcall() for the case when
                   ppp resyncs and the IP address changed. In this case, the
                   tcp connection is reset.
11/04/99    rc     Modified function dsstcp_close() to close TCP and clean up
                   if socket is already closed.
09/22/99    rc     Incorporated code review changes and comments.
08/19/99    ak     Removed ARM2.5 compiler warnings.
08/11/99    ak/rc  Added new_ip_upcall, for the case where we do ppp
                   re-negotiation and get a new IP.  Affects TCP esp.
                   in dormant mode re-originations.  Code for aborting
                   a TCP socket.
04/25/99    hcg    Added ASSERT to dsstcp_read() to guard against NULL tcp
                   receive queue.
04/05/99    hcg    Incorporated code review changes and comments.  Changed
                   some of the diagnostic message levels.
12/09/98    na/hcg Created module.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "amssassert.h"
#include "dssocket_defs.h"
#include "ps_tcp.h"
#include "ps_iface.h"
#include "ps_ifacei.h"
#include "ps.h"
#include "dsstcp.h"
#include "dsm.h"
#include "err.h"
#include "msg.h"
#include "psi.h"
#include "memory.h"
#include "ps_utils.h"
#include "ps_crit_sect.h"
#include "ps_route_scope.h"
#include "ps_pcb_close_reason_codes.h"
#include "ps_socket_event_defs.h"
#include "ps_socket_defs.h"
#include "dsstcpi.h"
#include "ps_mem.h"
#include "ps_tx_meta_info.h"
#include "ps_rt_meta_info.h"
#include "ps_metai_info.h"
#include "PS_Sock_IO_IReader.h"
#include "PS_Sock_IO_IWriter.h"
#include "ds_Utils_DebugMsg.h"

#include "ps_iface_addr_v6.h"
#include "ps_in.h"


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSSTCP_SOCKET()

DESCRIPTION
  This function is the protocol-specific socket function.  For TCP, it
  just sets the socket state to INIT.

  This function is called from the context of the application task.

DEPENDENCIES
  None

RETURN VALUE
  On success, returns DSS_SUCCESS
  On error, return DSS_ERROR and places the error condition value in *errno.

  Errno Values
  ------------
  DS_EPROTOTYPE       specified protocol invalid for socket type
  DS_EMFILE           too many descriptors open.  A socket is already open or
                      has not closed compeletely.

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
)
{
  struct tcb *  tcb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check arguments.
  -------------------------------------------------------------------------*/
  if (errno_ptr == NULL)
  {
    DATA_ERR_FATAL(" dsstcp_socket():errno_ptr is NULL ");
    return DSS_ERROR;
  }

  if (errq_ptr != NULL)
  {
    *errno_ptr = DS_EOPNOTSUPP;
    DATA_ERR_FATAL(" dsstcp_socket(): errq_ptr passed is NULL ");
    return DSS_ERROR;
  }

  if (scb_handle == NULL || reader_ptr == NULL || writer_ptr == NULL ||
      pcb_handle_ptr == NULL)
  {
    *errno_ptr = DS_EFAULT;
    DATA_ERR_FATAL(" dsstcp_socket(): Invalid arg passed ");
    return DSS_ERROR;
  }

  LOG_MSG_INFO1_3( "dsstcp_socket() family %d sockfd %d scb 0x%x", 
                   family, sockfd, scb_handle );

  /*-------------------------------------------------------------------------
    Create TCP control block
  -------------------------------------------------------------------------*/
  if ( (tcb_ptr = dsstcpi_create_tcb(scb_handle, sockfd)) == NULL)
  {
    LOG_MSG_INFO1_0(" Could not create a TCB");
    *errno_ptr = DS_EMFILE;
    return DSS_ERROR;
  }

  /* Set Family */
  tcb_ptr->family = family;

  tcb_ptr->reader = reader_ptr;
  tcb_ptr->writer = writer_ptr;
  *pcb_handle_ptr = (int32) tcb_ptr;

  return DSS_SUCCESS;
}  /* dsstcp_socket() */


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
  On failure returns DSS_ERROR

SIDE EFFECTS
  None.
===========================================================================*/
void dsstcp_connect
(
  int32                     pcb_handle,
  int32                     sockfd,
  struct ps_sockaddr_in6 *  dest_ptr
)
{
  struct tcb       * tcb_ptr = (struct tcb *) pcb_handle;
  ps_ip_addr_type    ip_addr;
  uint16             mtu;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == tcb_ptr)
  {
    DATA_ERR_FATAL("NULL TCB");
    return;
  }

  if (tcb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != TCB's sockfd %d", sockfd, tcb_ptr->sockfd);
    return;
  }

  /*-------------------------------------------------------------------------
    Check if metainfo is present
  -------------------------------------------------------------------------*/
  if (tcb_ptr->ipcb.meta_info_ptr == NULL ||
      PS_TX_META_GET_RT_META_INFO_PTR(tcb_ptr->ipcb.meta_info_ptr) == NULL)
  {
    DATA_ERR_FATAL("TCB IPCB Meta info ptr is NULL!");
    return;
  }

  LOG_MSG_INFO1_2("dsstcp_connect(): sock %d using iface 0x%p",
                sockfd,
                  PS_TX_META_GET_ROUTING_CACHE(tcb_ptr->ipcb.meta_info_ptr));

  tcb_ptr->conn.remote.address = dest_ptr->ps_sin6_addr;
  tcb_ptr->conn.remote.port    = dest_ptr->ps_sin6_port;

  /*-------------------------------------------------------------------------
    Update flow_label from dest_ptr in IPCB
  -------------------------------------------------------------------------*/
  tcb_ptr->ipcb.flow_label = dest_ptr->ps_sin6_flowinfo;

  if(PS_IN6_IS_ADDR_UNSPECIFIED(&( tcb_ptr->conn.local.address)))
  {
    /*-------------------------------------------------------------------------
      Get the source address with appropriate scope based on destination addr.
      This must be done for ICMPv6 messages.
    -------------------------------------------------------------------------*/
    ps_iface_get_ipv6_addr(
      (ps_iface_type *) PS_TX_META_GET_ROUTING_CACHE(tcb_ptr->ipcb.meta_info_ptr),
      &dest_ptr->ps_sin6_addr,
      &ip_addr);

    if(ip_addr.type == IP_ADDR_INVALID)
    {
      LOG_MSG_INFO1_0("dsstcp_connect: Error : IP_ADDR_INVALID");
      return;
    }

    tcb_ptr->conn.local.address = ip_addr.addr.v6;
    tcb_ptr->ipcb.src_ip_addr   = ip_addr.addr.v6;
  }

  /*-------------------------------------------------------------------------
    Update MSS based on iface family. Otherwise, MSS is always set to 1220
    for IPv4 sockets if IPv6 is turned on.
  -------------------------------------------------------------------------*/
  if (ps_iface_addr_family_is_v4(tcb_ptr->ipcb.routing_cache))
  {
    mtu = ps_ip_calc_path_mtu( IFACE_IPV4_ADDR_FAMILY, 
                               tcb_ptr->ipcb.routing_cache ) - IPLEN - TCPLEN;
  }
  else
  {
    mtu = ps_ip_calc_path_mtu( IFACE_IPV6_ADDR_FAMILY, 
                               tcb_ptr->ipcb.routing_cache ) - IP6LEN - TCPLEN;
  }

  tcb_ptr->mss = MIN(tcb_ptr->mss, mtu);

  /*-------------------------------------------------------------------------
    Open an active TCP connection.
  -------------------------------------------------------------------------*/
  open_tcp(&(tcb_ptr->conn.local),
           &(tcb_ptr->conn.remote),
           TCP_ACTIVE,
           tcb_ptr->rcvbuf,
           (uint16)tcb_ptr->mss,
           0,
           -1,
           tcb_ptr);

  return;

} /* dsstcp_connect() */


/*===========================================================================
FUNCTION DSSTCP_LISTEN()

DESCRIPTION
  This starts a passive open on a tcp socket.
  This is called from the context of the protocol stack

DEPENDENCIES
  Before this is called the source port should be put in the socket
  control block with bind.

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
)
{
  struct tcb* tcb_ptr = (struct tcb *) pcb_handle;
  struct socket localname;
  (void)backlog;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == tcb_ptr)
  {
    DATA_ERR_FATAL("NULL TCB");
    return;
  }

  if (tcb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != TCB's sockfd %d", sockfd, tcb_ptr->sockfd);
    return;
  }

  localname.port = tcb_ptr->conn.local.port;
  localname.address = tcb_ptr->conn.local.address;

  /*-------------------------------------------------------------------------
    Open an passive TCP connection.
  -------------------------------------------------------------------------*/
  open_tcp(&localname,
           NULL,
           TCP_PASSIVE,
           (uint32) rcv_window,
           (uint16)tcb_ptr->mss,
           0,
           -1,
           tcb_ptr);

  return;
} /* dsstcp_listen() */


/*===========================================================================
FUNCTION DSSTCP_WRITE()

DESCRIPTION
  Sends dsm memory item out to TCP.
  This function is called from the context of the PS task.

DEPENDENCIES
  None.

RETURN VALUE
  Uses return values from send_tcp().  On error, the function returns -1.
  Otherwise, it returns the number of bytes appended to TCP send queue.

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
)
{
  struct tcb  * tcb_ptr = (struct tcb *) pcb_handle;
  (void) ta_ptr;
  (void) pkt_meta_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == tcb_ptr)
  {
    DATA_ERR_FATAL("NULL TCB");
    return;
  }

  if (tcb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != TCB's sockfd %d", sockfd, tcb_ptr->sockfd);
    return;
  }

  tcb_ptr->sndcnt += pkt_length;

  /*-------------------------------------------------------------------------
    Start persist timer only if the peer's window size is 0 and there is
    more data to be sent
  -------------------------------------------------------------------------*/
  if (0 == tcb_ptr->snd.wnd && 0 < tcb_ptr->sndcnt &&
      tcb_ptr->persist_timer != PS_TIMER_INVALID_HANDLE &&
      ps_timer_is_running(tcb_ptr->timer) == FALSE &&
      ps_timer_is_running(tcb_ptr->persist_timer) == FALSE)
  {
    tcp_start_persist_timer(tcb_ptr);
  }
  else
  {
    tcp_output(tcb_ptr);
  }

  return;
} /* dsstcp_write() */


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
)
{
  struct tcb* tcb_ptr = (struct tcb *) pcb_handle;
  ps_socket_event_info_type    info;
  int unack_data;               /* Number of bytes of un-acknowledged data */
  boolean invoke_cb_flag = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == tcb_ptr)
  {
    DATA_ERR_FATAL("NULL TCB");
    return;
  }

  if (tcb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != TCB's sockfd %d", sockfd, tcb_ptr->sockfd);
    return;
  }

  if(tcb_ptr->state == TCP_CLOSED)
  {
    /*-----------------------------------------------------------------------
      Post event to socket platform
    -----------------------------------------------------------------------*/
    if (NULL != tcb_ptr->scb_ptr)
    {
      info.scb_ptr                = tcb_ptr->scb_ptr;
      info.data.pcb_closed_reason = PS_PCB_CLOSE_REASON_CODE_NORMAL;
      invoke_cb_flag = TRUE;
    }

    dsstcpi_free_tcb(tcb_ptr);
 
    if (TRUE == invoke_cb_flag)
    {
      socket_platform_event_cback_f_ptr(PS_SOCKET_PCB_CLOSED_EV, &info);
    }

    return;
  }

  unack_data = tcb_ptr->sndcnt;

  /*-------------------------------------------------------------------------
    sndcnt is inclusive of FIN. So, decrement it if FIN is sent.
  -------------------------------------------------------------------------*/
  if(tcb_ptr->state == TCP_FINWAIT1 ||
     tcb_ptr->state == TCP_CLOSING ||
     tcb_ptr->state ==TCP_LAST_ACK)
  {
    unack_data --;
  }

  ASSERT(unack_data >= 0);

  /*-------------------------------------------------------------------------
    If the call is dormant (Physical link is down or going down and interface
    is up) and graceful_dormant_close flag is FALSE and there is no
    un-acknowledged data in the send queue then abort the connection.
  -------------------------------------------------------------------------*/
  if((tcb_ptr->ipcb.routing_cache == NULL) ||
     (((ps_iface_phys_link_state(tcb_ptr->ipcb.routing_cache) ==
         PHYS_LINK_GOING_DOWN) ||
        (ps_iface_phys_link_state(tcb_ptr->ipcb.routing_cache) ==
         PHYS_LINK_DOWN)) &&
      (sock_config_cb.tcp_graceful_dormant_close == FALSE) &&
      (unack_data == 0)))
  {
    tcp_close_self(tcb_ptr, PS_PCB_CLOSE_REASON_CODE_NETWORK);
    return;
  }

  if ( silent_close_flag == TRUE )
  {
    reset_tcp(tcb_ptr, PS_PCB_CLOSE_REASON_CODE_RESET);
  }
  else
  {
    if (tcb_ptr->is_linger_on == TRUE && tcb_ptr->time_in_sec != 0)
    {
      (void) ps_timer_start(tcb_ptr->linger_timer, tcb_ptr->time_in_sec);
    }

    (void) close_tcp(tcb_ptr);
  }

  return;
} /* dsstcp_close() */


/*===========================================================================
FUNCTION DSSTCP_BIND()

DESCRIPTION
  This is the TCP specific bind() function. It writes the port number into
  the local.conn.port of the socket control block structure.

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
)
{
  struct tcb * tcb_ptr = (struct tcb *) pcb_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == tcb_ptr)
  {
    DATA_ERR_FATAL("NULL TCB");
    return;
  }

  if (tcb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != TCB's sockfd %d", sockfd, tcb_ptr->sockfd);
    return;
  }

  LOG_MSG_INFO2_3("Called TCP BIND on sock %d with port %d family %d addr:<next-line>",
                  sockfd, dss_ntohs(local_addr_ptr->ps_sin6_port), local_addr_ptr->ps_sin6_family);

  // Address used for binding
  IPV6_ADDR_MSG(local_addr_ptr->ps_sin6_addr.ps_s6_addr64);

  tcb_ptr->conn.local.port    = local_addr_ptr->ps_sin6_port;
  tcb_ptr->conn.local.address = local_addr_ptr->ps_sin6_addr;
  tcb_ptr->ipcb.src_ip_addr   = local_addr_ptr->ps_sin6_addr;

} /* dsstcp_bind() */


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
)
{
  struct tcb *          tcb_ptr = (struct tcb *) pcb_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == tcb_ptr)
  {
    DATA_ERR_FATAL("NULL TCB");
    return;
  }

  if (tcb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != TCB's sockfd %d", sockfd, tcb_ptr->sockfd);
    return;
  }

  /*-----------------------------------------------------------------------
    If the write-half needs to be shut down, send FIN and close the write
    direction
  -----------------------------------------------------------------------*/
  if (shutdown_dir == ds_Sock_ShutdownDir_QDS_WRITE_DIR ||
      shutdown_dir == ds_Sock_ShutdownDir_QDS_READ_WRITE_DIR)
  {
    tcb_ptr->flags.half_close = TRUE;

    /*---------------------------------------------------------------------
      Call close_tcp() which will send a FIN after sending out all the
      queued up data.
    ---------------------------------------------------------------------*/
    (void) close_tcp(tcb_ptr);
  }

  return;
} /* dsstcp_shutdown() */


/*===========================================================================
FUNCTION DSSTCP_SETOPT()

DESCRIPTION
  This function sets the option to the user specified value.

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
  int32  opt_val,
  int32  opt_len
)
{
  struct tcb * tcb_ptr = (struct tcb *) pcb_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == tcb_ptr)
  {
    DATA_ERR_FATAL("NULL TCB");
    return;
  }

  if (tcb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != TCB's sockfd %d", sockfd, tcb_ptr->sockfd);
    return;
  }

  /*-------------------------------------------------------------------------
    If option is is an IP level option, call IP function to process the option
  -------------------------------------------------------------------------*/
  if (ds_Sock_OptLevel_QDS_LEVEL_IP == opt_level || ds_Sock_OptLevel_QDS_LEVEL_IPV6 == opt_level)
  {
    ip_setopt(&tcb_ptr->ipcb, opt_level, opt_name, &opt_val, opt_len);
    return;
  }

  switch (opt_name)
  {
    case ds_Sock_OptName_QDS_SO_RCVBUF:
      tcb_ptr->rcvbuf = opt_val;
      break;

    case ds_Sock_OptName_QDS_SO_KEEPALIVE:
      tcb_ptr->options.tcp_keepalive = opt_val ? TRUE : FALSE;
      break;

    case ds_Sock_OptName_QDS_TCP_MAXSEG:
      if (tcb_ptr->state == TCP_CLOSED)
      {
        tcb_ptr->mss = (uint16) opt_val;
      }
      else
      {
        if (tcb_ptr->mss > opt_val)
        {
          tcb_ptr->mss = (uint16) opt_val;
        }
        else
        {
          LOG_MSG_INFO2_3("NEGLECTING MSS SET REQUEST",
                  tcb_ptr->state, tcb_ptr->mss, opt_val);
        }
      }
      break;

    case ds_Sock_OptName_QDS_TCP_NODELAY:
      tcb_ptr->options.tcp_nodelay = opt_val ? TRUE : FALSE;
      break;

    case ds_Sock_OptName_QDS_TCP_DELAYED_ACK:
      tcb_ptr->options.tcp_delayed_ack = opt_val ? TRUE : FALSE;
      break;

    case ds_Sock_OptName_QDS_TCP_SACK:
      tcb_ptr->options.tcp_sack_permitted = opt_val ? TRUE : FALSE;
      break;

    case ds_Sock_OptName_QDS_TCP_TIMESTAMP:
      tcb_ptr->options.tcp_ts_permitted = opt_val ? TRUE : FALSE;
      break;

    case ds_Sock_OptName_QDS_TCP_MAX_BACKOFF_TIME:
      tcb_ptr->max_backoff_time = (uint32)opt_val;
      break;

    default:
      break;
  }
} /* dsstcp_setopt() */


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
  int32    pcb_handle,
  int32    sockfd
)
{
  struct tcb * tcb_ptr = (struct tcb *) pcb_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == tcb_ptr)
  {
    DATA_ERR_FATAL("NULL TCB");
    return;
  }

  if (tcb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != TCB's sockfd %d", sockfd, tcb_ptr->sockfd);
    return;
  }

  /*-------------------------------------------------------------------------
    TCP is allowed to write data again
  -------------------------------------------------------------------------*/
  tcp_output(tcb_ptr);
  return;

} /* dsstcp_flow_enabled_ind() */


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
  ps_rt_meta_info_type *  rt_mi_ptr             /* Routing Meta info */
)
{
  struct tcb            * tcb_ptr = (struct tcb *) pcb_handle;
  ps_tx_meta_info_type  * tx_mi_ptr;           /* Ptr to TX meta info */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (rt_mi_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL rt_mi_ptr");
    return;
  }

  if (NULL == tcb_ptr )
  {
    PS_RT_META_INFO_FREE(&rt_mi_ptr);
    DATA_ERR_FATAL("NULL TCB");
    return;
  }

  if (tcb_ptr->sockfd != sockfd)
  {
    PS_RT_META_INFO_FREE(&rt_mi_ptr);
    LOG_MSG_INFO1_2("Sockfd %d != TCB's sockfd %d", sockfd, tcb_ptr->sockfd);
    return;
  }

  /*-------------------------------------------------------------------------
    Get TX meta info and update incoming routing meta info!
  -------------------------------------------------------------------------*/
  PS_TX_META_INFO_GET(tx_mi_ptr);
  if (tx_mi_ptr == NULL)
  {
    PS_RT_META_INFO_FREE(&rt_mi_ptr);
    DATA_ERR_FATAL("Out of mem for TX_META_INFO");
    return;
  }

  PS_TX_META_SET_RT_META_INFO_PTR(tx_mi_ptr, rt_mi_ptr);

  /*-------------------------------------------------------------------------
    Update IPCB TX meta info!
    - Clear any existing meta info before assigning new TX meta info in IPCB
  -------------------------------------------------------------------------*/
  if (tcb_ptr->ipcb.meta_info_ptr != NULL)
  {
    PS_TX_META_INFO_FREE(&tcb_ptr->ipcb.meta_info_ptr);
  }

  tcb_ptr->ipcb.meta_info_ptr = tx_mi_ptr;

  ps_ip_update_routing_cache_in_ipcb
  (
    &(tcb_ptr->ipcb),
    (ps_iface_type *)
      PS_TX_META_GET_ROUTING_CACHE(tcb_ptr->ipcb.meta_info_ptr)
  );

  return;
} /* dsstcp_set_route_meta_info() */


/*===========================================================================
FUNCTION DSSTCP_SET_ROUTE_SCOPE()

DESCRIPTION
  This is the TCP specific set_route_scope() function. It writes the route
  scope in the PCB's IPCB

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
)
{
  struct tcb * tcb_ptr = (struct tcb *) pcb_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == tcb_ptr)
  {
    DATA_ERR_FATAL("NULL TCB");
    return;
  }

  if (tcb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != TCB's sockfd %d", sockfd, tcb_ptr->sockfd);
    return;
  }

  if (NULL == route_scope_ptr)
  {
    DATA_ERR_FATAL("NULL rscope");
    return;
  }

  PS_ROUTE_SCOPE_PRINT(route_scope_ptr);

  tcb_ptr->ipcb.route_scope = *route_scope_ptr;
  return;

} /* dsstcp_set_route_scope() */


void dsstcp_set_linger_reset
(
  int32    pcb_handle,
  int32    sockfd,
  boolean  is_linger_on,
  int32    time_in_sec
)
{
  struct tcb * tcb_ptr = (struct tcb *) pcb_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == tcb_ptr )
  {
    DATA_ERR_FATAL("NULL TCB");
    return;
  }

  if (tcb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != TCB's sockfd %d", sockfd, tcb_ptr->sockfd);
    return;
  }

  tcb_ptr->is_linger_on = is_linger_on;
  tcb_ptr->time_in_sec  = time_in_sec;

  return;
} /* dsstcp_set_linger_reset() */


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
void dsstcp_cleanup_all_reseq()
{
  struct tcb *tcb_ptr;                                   /* Pointer to TCB */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tcb_ptr = (struct tcb *) q_check(&sock_config_cb.tcp_cb_q);
  while (NULL != tcb_ptr)
  {
    ASSERT(0 != tcb_ptr->sockfd);

    tcp_cleanup_reseq(tcb_ptr);

    tcb_ptr =
      (struct tcb *) q_next(&sock_config_cb.tcp_cb_q, (q_link_type *) tcb_ptr);
  } /* while */

} /* dsstcp_cleanup_all_reseq() */

