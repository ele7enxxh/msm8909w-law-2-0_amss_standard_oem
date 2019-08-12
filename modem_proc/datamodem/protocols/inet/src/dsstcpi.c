/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  T C P   F I L E

DESCRIPTION

 The Data Services sockets TCP interface file. This contains all the
 functions used by the sockets library to access TCP socket function calls.

EXTERNALIZED FUNCTIONS
  dsstcp_socket()
    TCP specific socket() function.  Sets socket state to INIT.

  dsstcp_connect_setup()
    Sets up specific parameters for TCP sockets.  Called prior
    dsstcp_connect(), it sets up the appropriate state of the socket
    so that the correct error response will be generated as needed.

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

  dsstcp_abort()
    TCP specific abort() function. Aborts the TCP connection and cleans-up.
    This is usually called when we are dormant.

  dsstcp_get_conn_error()
    Gets the appropriate error code for connection errors.

  dsstcp_lookup_conn()
    Given a connection structure, looks up the connection to determine
    if this is a valid connection.

  dsstcp_lookup_serv()
    Given a connection structure, looks up to determine if there are
    any valid listener sockets(server socket)

  dsstcp_new_ip_upcall()
    If a new IP address is negotiated with the Base Station, close down
    sockets and notify the user.

  dsstcp_bind()
    This is the TCP specific bind() function.  It writes the port number into
    the local.conn.port of the socket control block structure.

  dsstcp_setopt
    This function sets the tcp option value to the user specified value.

  dsstcp_register_upcall
    This function registers upcall function and user data with the TCB.

dsstcp_window_reopen()
    This function calls the tcp_output function to reopen the previously
    closed tcp receive window.

dsstcp_set_rscope()
    TCP specific set_rscope function. Sets the incoming route scope in
    the TCP control block.

  dsstcp_flow_enabled_ind
    This function calls tcp_output() to send data out once socket is
    flow enabled

Copyright (c) 1998-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsstcp.c_v   1.9   13 Feb 2003 12:20:46   ssinghai  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dsstcpi.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/28/13    ss     Fix to send socket close notification only after moving
                   to CLOSED state and not from TIME_WAIT state.
05/16/13    ss     Fix to delete tcb_ptr after invoking socket's event
                   callback.
12/12/12    ds     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR.
02/13/12    ss     Protecting sock_config_cb to avoid concurrency issues with
                   inet event callbacks(running in DS).
07/01/11    am     Allowed linger timer to continue in TW state.
10/01/10    sa     Added appropriate log messages before ASSERT(0).
06/15/10    vm     IDLE event related fixes
02/05/10    ss     Destroy the queue using q_destroy () when it is no longer
                   needed to free up queue mutex.
03/26/09    pp     CMI De-featurization.
03/17/08    mct    Ensure we clear the source ip addr when our ip changes.
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
01/07/04    vp	   Modified dsstcpi_half_open_timer_cb to remove scb access
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
#include "dssocket.h"
#include "dssocket_defs.h"
#include "event.h"
#include "ps_tcp.h"
#include "ps_iface.h"
#include "ps_ifacei.h"
#include "ps.h"
#include "dsm.h"
#include "err.h"
#include "msg.h"
#include "psi.h"
#include "pstimer.h"
#include "memory.h"
#include "ps_utils.h"
#include "ps_crit_sect.h"
#include "ps_socket_event_defs.h"
#include "ps_socket_defs.h"
#include "dsstcpi.h"
#include "ps_mem.h"
#include "ps_pcb_close_reason_codes.h"
#include "ps_tcp_events.h"
#include "ps_route.h"
#include "ps_stat_sock.h"
#include "ps_stat_tcp.h"
#include "ps_stat_logging.h"
#include "PS_Sock_IO_IReader.h"
#include "PS_Sock_IO_IWriter.h"
#include "ds_Sock_Def.h"
#include "ds_Utils_DebugMsg.h"



/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSSTCPI_SEND_KEEPALIVE_PROBE()

DESCRIPTION
  This function is responsible for forming a keepalive probe packet and
  sending it to the peer to know whether the peer is up and reachable or not.
  The trick lies in sending the packet with sequence number equal to 1 less
  than the last sequence number acknowledged by the peer. Using such a
  sequence number causes the transmitted zero-length segment to lie outside
  the receive window. This requires the peer to respond with either an ACK
  (in case the connection is still alive) or a RST (if there is no connection
  anymore with the peer).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void dsstcpi_send_keepalive_probe
(
  struct tcb *tcb
)
{
  struct tcp         seg;                /* TCP segment                    */
  struct dsm_item_s  *header_ptr = NULL; /* Ptr to item with TCP header    */
  pseudo_header_type ph;                 /* Pseudo header to hold IP info  */
  int16  ps_errno;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  LOG_MSG_INFO2_1("Sending a keepalive probe for socket %d", tcb->sockfd);

  /*-------------------------------------------------------------------------
    Construct the pseudo header.
  -------------------------------------------------------------------------*/
  ph.source   = tcb->conn.local.address;
  ph.dest     = tcb->conn.remote.address;
  ph.protocol = PS_IPPROTO_TCP;
  /*-------------------------------------------------------------------------
    We do not send any data, hence we set the length as 0.tcp_hdr_create()
    will set the proper length for pseudo header.
  -------------------------------------------------------------------------*/
  ph.length   = 0;

  /*-------------------------------------------------------------------------
    Fill in the tcp segment.
  -------------------------------------------------------------------------*/
  memset(&seg,0,sizeof(seg));
  seg.source = tcb->conn.local.port;
  seg.dest   = tcb->conn.remote.port;

  /*-------------------------------------------------------------------------
    Note that the sequence number is 1 less than the first unacknowledged
    sequence number making the sequence number out of the receive window of
    the peer.
  -------------------------------------------------------------------------*/
  seg.seq       = tcb->snd.una - 1;
  seg.ack       = tcb->rcv.nxt;
  seg.wnd       = (uint16)tcb->rcv.wnd;
  seg.ack_flag = 1;

  if (tcp_hdr_create(&header_ptr,&seg,&ph) == FALSE)
  {
    if (header_ptr != NULL)
    {
      LOG_MSG_INFO1_0("Cannot send KEEPALIVE probe due to error in tcp_hdr_create");
      dsm_free_packet(&header_ptr);
      return;
    }
  }
  /*-------------------------------------------------------------------------
    If there is no dearth of items.
  -------------------------------------------------------------------------*/
  if (header_ptr != NULL)
  {
    TCP_INC_STATS(segs_tx, 1);
    TCP_INC_INSTANCE_STATS(tcb,segs_tx, 1);
    (void)ip_send(&tcb->ipcb,
                  ph.dest,
                  PS_IPPROTO_TCP,
                  header_ptr,
                  ph.length,
                  NULL,
                  &ps_errno);
  }
}/* dsstcpi_send_keepalive_probe */



/*===========================================================================
FUNCTION DSSTCPI_HALF_OPEN_KEEPALIVE_TIMER_CB()

DESCRIPTION
  This function handles both half-open timer and keepalive timer timeout.
  When half-open timer goes off this function resets and returns half-open
  tcp connections to the listen state.
  When TCP keepalive timer goes off this function is responsible for sending
  the keepalive probes to the peer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If it is handling keepalive timeout it restarts the keepalive timer with
  tcp_keepalive_interval as time if it is successful in sending the keepalive
  probe.

===========================================================================*/
static void dsstcpi_half_open_keepalive_timer_cb
(
 void  *in_tcb_ptr                         /* pointer to TCP control block */
)
{
  struct tcb           * tcb_ptr = (struct tcb*)in_tcb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
     Get tcb_ptr and make sure it is not NULL.
  -------------------------------------------------------------------------*/
  if (tcb_ptr == NULL)
  {
    LOG_MSG_ERROR_0("HALF_OPEN_KEEPALIVE_TIMER_CB: NULL TCB");
    return;
  }

  /*-------------------------------------------------------------------------
    Half-open timer handling. If the state is SYN_RECEIVED then half_open
    timer must have expired.
  -------------------------------------------------------------------------*/
  if (tcb_ptr->state == TCP_SYN_RECEIVED)
  {
    reset_tcp(tcb_ptr, PS_PCB_CLOSE_REASON_CODE_TIMEOUT);
    return;
  }
  else if ((tcb_ptr->state == TCP_ESTABLISHED ||
           tcb_ptr->state == TCP_CLOSE_WAIT) &&
           tcb_ptr->options.tcp_keepalive)
  {
    /*-----------------------------------------------------------------------
      If keepalive option is set for this connection and if the state is
      ESTABLISHED or CLOSE_WAIT, try sending a keepalive probe.
      Check whether we have already sent Maximum number of keepalive probes
      already and if so then send a reset and close self.
      Here we use the backoff counter to actually count the number of probes
      sent.
    -----------------------------------------------------------------------*/
    if (TCP_MAX_KEEPALIVE_PROBES < tcb_ptr->backoff++)
    {
      TCP_INC_STATS(conn_keepalive_aborts, 1);
      reset_tcp(tcb_ptr, PS_PCB_CLOSE_REASON_CODE_TIMEOUT);
      return;
    }

    /*-----------------------------------------------------------------------
      We fulfill all the conditions of sending the keepalive probe.
      Now send it.
    -----------------------------------------------------------------------*/
    dsstcpi_send_keepalive_probe(tcb_ptr);

    /*-----------------------------------------------------------------------
      Reset the Keepalive timer to 75 seconds after which another probe can
      be sent.
    -----------------------------------------------------------------------*/
    if (PS_TIMER_SUCCESS != ps_timer_start(tcb_ptr->half_open_keepalive_timer,
                                           (int64)TCP_KEEPALIVE_INTERVAL) )
    {
      LOG_MSG_ERROR_1("Cant start timer 0x%x",
                      tcb_ptr->half_open_keepalive_timer);
      ASSERT(0);
      return;
    }
  }
  else
  {
    LOG_MSG_INFO2_1("Keepalive timer fired in TCP state %d", tcb_ptr->state);
    return;
  }

} /* dsstcpi_half_open_keepalive_timer_cb() */


/*===========================================================================
FUNCTION DSSTCPI_PERSIST_TIMER_CB()

DESCRIPTION
  This function handles persist timer timeout. When this timer goes off
  this function sends a window probe to the peer and restarts persist timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void dsstcpi_persist_timer_cb
(
 void  * in_tcb_ptr                        /* pointer to TCP control block */
)
{
  struct tcb  * tcb_ptr = (struct tcb *) in_tcb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == tcb_ptr)
  {
    LOG_MSG_ERROR_0("PERSIST_TIMER_CB: NULL TCB");
    return;
  }

  LOG_MSG_INFO2_1("Persist timer fired in TCP state %d", tcb_ptr->state);

  /*-------------------------------------------------------------------------
    Make sure that the unacknowledged byte is retransmitted in window
    probe. Since the unacknowledged byte is transmitted as part of window
    probe, it is not considered to have actually been transmitted.
    So reset snd.nxt and numb_bytes_in_transit.
  -------------------------------------------------------------------------*/
  tcb_ptr->snd.nxt               = tcb_ptr->snd.una;
  tcb_ptr->numb_bytes_in_transit = 0;

  /*-------------------------------------------------------------------------
    Transmit 1 byte of data to the peer
  -------------------------------------------------------------------------*/
  tcb_ptr->flags.force = TRUE;
  tcp_output(tcb_ptr);
  tcb_ptr->flags.force = FALSE;

  /*-------------------------------------------------------------------------
    Restart persist timer. If we don't start the timer here, deadlock could
    happen if ACK for this packet is lost.
  -------------------------------------------------------------------------*/
  tcp_start_persist_timer(tcb_ptr);

} /* dsstcpi_persist_timer_cb() */


static void dsstcpi_linger_timer_cb
(
 void  *in_tcb_ptr                         /* pointer to TCP control block */
)
{
  struct tcb * tcb_ptr = (struct tcb*) in_tcb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == tcb_ptr)
  {
    LOG_MSG_ERROR_0("LINGER_TIMER_CB: NULL TCB");
    return;
  }

  LOG_MSG_INFO2_1("Linger timer fired in TCP state %d", tcb_ptr->state);

  reset_tcp(tcb_ptr, PS_PCB_CLOSE_REASON_CODE_RESET);
}



/*===========================================================================
FUNCTION DSSTCPI_CLOSE_TIMER_CB()

DESCRIPTION
  This function is invoked when the close timer( 2MSL timeout, etc.), goes
  off for a TCP socket.
  This function sets the TCP state to CLOSED, which will invoke the
  CLOSE_EVENT on the interested sockets.

  This function is called in the context of the PS task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void dsstcpi_close_timer_cb
(
  void  *in_tcb_ptr                        /* pointer to TCP control block */
)
{
  struct tcb *tcb_ptr = (struct tcb*)in_tcb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
     Get tcb_ptr and make sure it is not NULL.
  -------------------------------------------------------------------------*/
  if (tcb_ptr == NULL)
  {
    DATA_ERR_FATAL(" TCP control block ptr is NULL ");
    return;
  }

  LOG_MSG_INFO2_2(" Close timer off; sockfd=%d; state=%d\n",
                  (int) tcb_ptr->sockfd, (int) tcb_ptr->state );

  /*-------------------------------------------------------------------------
    If TCP state is not CLOSED, set it to closed.
  -------------------------------------------------------------------------*/
  if (tcb_ptr->state != TCP_CLOSED)
  {
    tcp_setstate( tcb_ptr, TCP_CLOSED);
  }

} /* dsstcpi_close_timer_cb() */


/*===========================================================================
FUNCTION DSSTCPI_STATE_CHANGE_UPCALL()

DESCRIPTION
  This function will be invoked by the TCP input routine when the TCP state
  changes when in sockets mode. This function will use the old and the new
  TCP states to determine the processing requirements.  For sockets,
  this function currently does not actually use these values, but rather
  retains them to support backward compatibility to existing TCP code.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void dsstcpi_state_change_upcall
(
  struct tcb *tcb_ptr,         /* pointer to TCP control block which has a
                                  list head pointer to payload data        */
  tcp_state_enum_type old_state,  /* previous TCP state                    */
  tcp_state_enum_type new_state   /* new TCP state                         */
)
{
  tcp_event_payload_type       tcp_event_payload;
  ps_socket_event_info_type    info;
  boolean                      invoke_cb_flag = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check arguments.
  -------------------------------------------------------------------------*/
  if (NULL == tcb_ptr )
  {
    DATA_ERR_FATAL("NULL tcb_ptr passed");
    return;
  }

  LOG_MSG_INFO2_3("TCP socket=%d oldstate=%d newstate=%d",
          (int)tcb_ptr->sockfd, (int) old_state, (int) new_state);

  /*-------------------------------------------------------------------------
    Do processing for relevant TCP state changes only.  Note, this block
    will generate a number of Lint errors, as the state enum is defined
    within the tcb structure in tcp.h.  This was maintained so as to reduce
    risk of changing any legacy tcp code.
  -------------------------------------------------------------------------*/
  switch(tcb_ptr->state)
  {
    case TCP_SYN_RECEIVED:
      /*---------------------------------------------------------------------
        Here we need to set a timer to detect failed halfopen connections

        IS707 Async handler creates a TCB directly instead of opening a TCP
        socket. It may not have created half_open_keepalive_timer
      ---------------------------------------------------------------------*/
      if (tcb_ptr->half_open_keepalive_timer != PS_TIMER_INVALID_HANDLE)
      {
        if (PS_TIMER_SUCCESS !=
              ps_timer_start(tcb_ptr->half_open_keepalive_timer,
                             (int64)DSS_HALF_OPEN_INTERVAL ))
        {
          LOG_MSG_ERROR_1("Cant start timer 0x%x",
                          tcb_ptr->half_open_keepalive_timer);
          ASSERT(0);
        }
      }

      /*---------------------------------------------------------------------
          Generate TCP opening event
      ---------------------------------------------------------------------*/
      if (old_state != new_state)
      {
        tcp_event_payload.sock_fd = tcb_ptr->sockfd;
        tcp_event_payload.tcp_event_state = EVENT_TCP_OPENING;
        event_report_payload( EVENT_TCP,
                              sizeof(tcp_event_payload),
                              &tcp_event_payload);
      }

      break;

    case TCP_ESTABLISHED:
      /*---------------------------------------------------------------------
            Update Statistics
      ---------------------------------------------------------------------*/
      TCP_INC_STATS(crnt_open_conns, 1) ;

      info.scb_ptr                               = tcb_ptr->scb_ptr;
      info.data.tcp_connected_event.conn         = tcb_ptr->conn;
      info.data.tcp_connected_event.mss          = tcb_ptr->mss;
      info.data.tcp_connected_event.options_mask = 0;

      if (tcb_ptr->options.tcp_nodelay)
      {
        info.data.tcp_connected_event.options_mask |=
          1 << ds_Sock_OptName_QDS_TCP_NODELAY;
      }

      if (tcb_ptr->options.tcp_keepalive)
      {
        info.data.tcp_connected_event.options_mask |=
          1 << ds_Sock_OptName_QDS_SO_KEEPALIVE;
      }

      if (tcb_ptr->options.tcp_delayed_ack)
      {
        info.data.tcp_connected_event.options_mask |=
          1 << ds_Sock_OptName_QDS_TCP_DELAYED_ACK;
      }

      if (tcb_ptr->options.tcp_sack_permitted)
      {
        info.data.tcp_connected_event.options_mask |=
          1 << ds_Sock_OptName_QDS_TCP_SACK;
      }

      if (tcb_ptr->options.tcp_ts_permitted)
      {
        info.data.tcp_connected_event.options_mask |=
          1 << ds_Sock_OptName_QDS_TCP_TIMESTAMP;
      }

      socket_platform_event_cback_f_ptr(PS_SOCKET_TCP_CONNECTED_EV, &info);

      /*---------------------------------------------------------------------
          Generate TCP open event
      ---------------------------------------------------------------------*/
      if (old_state != new_state)
      {
        tcp_event_payload.sock_fd = tcb_ptr->sockfd;
        tcp_event_payload.tcp_event_state = EVENT_TCP_OPEN;
        event_report_payload( EVENT_TCP,
                              sizeof(tcp_event_payload),
                              &tcp_event_payload);

      }

      /*---------------------------------------------------------------------
        Where are we coming from: If this socket is halfopen (SYN
        sent) then we need to move it to the open queue and generate a
        socket event on the listener socket.
      ---------------------------------------------------------------------*/
      if (old_state == TCP_SYN_RECEIVED)
      {
        /*------------------------------------------------------------------
           Clear the half-open timer

           IS707 Async handler creates a TCB directly instead of opening a
           TCP socket. It may not have created half_open_keepalive_timer
        -------------------------------------------------------------------*/
        if (tcb_ptr->half_open_keepalive_timer != PS_TIMER_INVALID_HANDLE)
        {
          if (PS_TIMER_SUCCESS !=
                ps_timer_cancel( tcb_ptr->half_open_keepalive_timer) )
          {
            LOG_MSG_ERROR_1("Error canceling timer 0x%x",
                            tcb_ptr->half_open_keepalive_timer);
            ASSERT(0);
          }
        }
      }

      /*---------------------------------------------------------------------
        If the keepalive option is set then start the keepalive timer.

        IS707 Async handler creates a TCB directly instead of opening a TCP
        socket. It may not have created half_open_keepalive_timer
      ---------------------------------------------------------------------*/
      if (tcb_ptr->options.tcp_keepalive &&
          tcb_ptr->half_open_keepalive_timer != PS_TIMER_INVALID_HANDLE)
      {
        if (PS_TIMER_SUCCESS !=
              ps_timer_start(tcb_ptr->half_open_keepalive_timer,
                             (int64)sock_config_cb.tcp_keepalive_idle_time) )
        {
            LOG_MSG_ERROR_1("Error starting timer 0x%x",
                            tcb_ptr->half_open_keepalive_timer);
            ASSERT(0);
        }
      }

      break;

    case TCP_CLOSED:

      if (old_state >= TCP_ESTABLISHED 	)
      {
        TCP_DEC_STATS(crnt_open_conns, 1);
      }
      /*---------------------------------------------------------------------
          Generate TCP closed event
      ---------------------------------------------------------------------*/
      if (old_state != new_state)
      {
        tcp_event_payload.sock_fd = tcb_ptr->sockfd;
        tcp_event_payload.tcp_event_state = EVENT_TCP_CLOSED;
        event_report_payload( EVENT_TCP,
                              sizeof(tcp_event_payload),
                              &tcp_event_payload);

      }

      /*---------------------------------------------------------------------
        This TCP is now closed.
        Set the appropriate closing reason for the socket and then free the
        TCB.
      ---------------------------------------------------------------------*/
      if (tcb_ptr->scb_ptr != NULL)
      {
        info.scb_ptr                = tcb_ptr->scb_ptr;
        info.data.pcb_closed_reason = tcb_ptr->reason;
        invoke_cb_flag = TRUE;
      }

      dsstcpi_free_tcb( tcb_ptr);

      if (TRUE == invoke_cb_flag)
      {
        socket_platform_event_cback_f_ptr(PS_SOCKET_PCB_CLOSED_EV, &info);
      }
      break;

    case TCP_FINWAIT2:
      if (!tcb_ptr->flags.half_close)
      {
        tcp_setstate(tcb_ptr, TCP_TIME_WAIT);
      }
      else if (tcb_ptr->linger_timer != PS_TIMER_INVALID_HANDLE)
      {
        LOG_MSG_INFO1_1("Cancelling linger timer for sock %d",
                        tcb_ptr->sockfd);
        (void) ps_timer_cancel(tcb_ptr->linger_timer);
      }
      break;

    case TCP_TIME_WAIT:

      /*---------------------------------------------------------------------
        In these states, start state timer to clean up queues. Note that we
        want to start the timer for FINWAIT2 as well so that we don't hang
        in this state forever if the peer crashed after acking our FIN and
        before it could send its FIN.

        IS707 Async handler creates a TCB directly instead of opening a TCP
        socket. It may not have created close timer.

        Allow linger timer specified to close this socket first. This is
        because sock can move from FIN1->FIN2->TW before linger timer
        expires, in which case we dont want to wait for 30s before
        tcb is freed.
      ---------------------------------------------------------------------*/
      if (tcb_ptr->linger_timer != PS_TIMER_INVALID_HANDLE &&
          TRUE == ps_timer_is_running(tcb_ptr->linger_timer))
      {
        LOG_MSG_INFO1_1("Linger timer running for sock %d", tcb_ptr->sockfd);
        break;
      }
      else if (tcb_ptr->close_timer != PS_TIMER_INVALID_HANDLE)
      {
        if (PS_TIMER_SUCCESS != ps_timer_start(tcb_ptr->close_timer,
                                               (int64)TCP_TIME_WAIT_INTERVAL ))
        {
          LOG_MSG_ERROR_1("Error starting timer 0x%x", tcb_ptr->close_timer);
          ASSERT(0);
          dsstcpi_close_timer_cb( tcb_ptr);
        }
      }
      break;

    case TCP_CLOSE_WAIT:
      /*---------------------------------------------------------------------
          Generate TCP closing event
      ---------------------------------------------------------------------*/
      if (old_state != new_state)
      {
        tcp_event_payload.sock_fd = tcb_ptr->sockfd;
        tcp_event_payload.tcp_event_state = EVENT_TCP_CLOSING;
        event_report_payload( EVENT_TCP,
                              sizeof(tcp_event_payload),
                              &tcp_event_payload);

      }
//TODO Update cmt
      /*---------------------------------------------------------------------
        Although no more data is enqueued to the rcvq, it can't be deleted as
        TCB always calls GetSndCnt() before sending any packet out. At the
        very least, TCB need to send ACK for FIN out
      ---------------------------------------------------------------------*/
      if (tcb_ptr->writer != NULL)
      {
        ReleaseWriter(tcb_ptr->writer);
        tcb_ptr->writer = NULL;
      }

      break;

    case TCP_SYN_SENT:
      /*---------------------------------------------------------------------
          Generate TCP opening event
      ---------------------------------------------------------------------*/
      if (old_state != new_state)
      {
        tcp_event_payload.sock_fd = tcb_ptr->sockfd;
        tcp_event_payload.tcp_event_state = EVENT_TCP_OPENING;
        event_report_payload( EVENT_TCP,
                              sizeof(tcp_event_payload),
                              &tcp_event_payload);
      }
      break;

    case TCP_FINWAIT1:
      /*---------------------------------------------------------------------
          Generate TCP CLOSING event
      ---------------------------------------------------------------------*/
      if (old_state != new_state)
      {
        tcp_event_payload.sock_fd = tcb_ptr->sockfd;
        tcp_event_payload.tcp_event_state = EVENT_TCP_CLOSING;
        event_report_payload( EVENT_TCP,
                              sizeof(tcp_event_payload),
                              &tcp_event_payload);
      }

      break;

    case TCP_CLOSING:
    case TCP_LAST_ACK:
    case TCP_LISTEN:

      /*---------------------------------------------------------------------
        In these states, do nothing.
      ---------------------------------------------------------------------*/
      break;

    default:
      LOG_MSG_ERROR_1(" Invalid TCP state :%d",(int)tcb_ptr->state);
      ASSERT(0);
  } /* switch */

} /* dsstcpi_state_change_upcall() */



/*===========================================================================
FUNCTION DSSTCPI_INIT_TCB()

DESCRIPTION
  This function initializes a new TCP control block.

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  Initailizes TCB
===========================================================================*/
static void dsstcpi_init_tcb
(
  struct tcb   *tcb_ptr                        /* Ptr to tcp control block */
)
{
  /*-------------------------------------------------------------------------
    Initialize the queues and set the default values.
  -------------------------------------------------------------------------*/
  (void)q_init( &(tcb_ptr->reseq));
  tcb_ptr->state    = TCP_CLOSED;
  tcb_ptr->cwind    = PS_TCP_DEF_CWIND;
  tcb_ptr->mss      = DEF_MSS ;
  tcb_ptr->ssthresh = SSTHRESH;
  tcb_ptr->srtt     = DEF_RTT;
  tcb_ptr->mdev     = 0;
  tcb_ptr->reason   = PS_PCB_CLOSE_REASON_CODE_NORMAL;
  tcb_ptr->tcp_rexmit_timer_val = DEF_RTT;
  tcb_ptr->rcvbuf   = DEF_WND;
  tcb_ptr->sndcnt   = 0;
  tcb_ptr->max_backoff_time = TCP_MAX_TOTAL_BACKOFF_TIME;
  memset(&(tcb_ptr->snd), 0, sizeof(tcb_ptr->snd));
  memset(&(tcb_ptr->rcv), 0, sizeof(tcb_ptr->rcv));
  ip_init_ipcb(&tcb_ptr->ipcb);

  /*-------------------------------------------------------------------------
    Setup the TCP control block callback functions.
  -------------------------------------------------------------------------*/
  tcb_ptr->s_upcall = dsstcpi_state_change_upcall;

  return;
} /* dsstcpi_init_tcb() */



/*===========================================================================
FUNCTION DSSTCPI_CREATE_TCB()

DESCRIPTION
  This function creates a new TCP control block.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to TCP control block structure.  If none are
  available, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
struct tcb *dsstcpi_create_tcb
(
  void  * scb_handle,
  int32   sockfd
)
{
  struct tcb            * tcb_ptr = NULL;      /* Ptr to tcb to be created */
  ps_timer_handle_type    timer_handle;        /* Self explanatory         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == scb_handle)
  {
    DATA_ERR_FATAL("NULL scb_handle");
    SOCK_INC_STATS(tcb_alloc_failures, 1);
    return NULL;
  }

  tcb_ptr = (struct tcb *) ps_mem_get_buf(PS_MEM_DSS_TCB_TYPE);

  /*-------------------------------------------------------------------------
    If no TCP control block is available, return NULL.
  -------------------------------------------------------------------------*/
  if (tcb_ptr == NULL)
  {
    SOCK_INC_STATS(tcb_alloc_failures, 1);
    LOG_MSG_ERROR_0("Memory allocation for TCB failed");
    return (NULL);
  }

  memset(tcb_ptr, 0, sizeof(struct tcb));
  tcb_ptr->scb_ptr = scb_handle;
  tcb_ptr->sockfd  = sockfd;

  dsstcpi_init_tcb(tcb_ptr);

  /*-------------------------------------------------------------------------
    Now define the timers for the TCP control block.
    TCP needs four timers:
    timer       : For TCP timeouts
    acker       : To send delayed acks
    close_timer : For 2MSL timeouts and other close timeouts.
    half open timer: For detecting failed half-open connections
    Note, it is important to maintain the order of the 4 calls to ps_timer_
    alloc() below.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Allocate the retransmission timeout.
    If allocation fails, free any other allocated timers and return failure.
  -------------------------------------------------------------------------*/
  timer_handle = ps_timer_alloc( tcp_timeout_cb, tcb_ptr);
  if (timer_handle == PS_TIMER_INVALID_HANDLE)
  {
    goto bail;
  }
  else
  {
    tcb_ptr->timer = timer_handle;
  }

  /*-------------------------------------------------------------------------
    Allocate the delayed ack timer.
    If allocation fails, free any other allocated timers and return failure.
  -------------------------------------------------------------------------*/
  timer_handle = ps_timer_alloc( tcp_delayed_cb, tcb_ptr);
  if (timer_handle == PS_TIMER_INVALID_HANDLE)
  {
    goto bail;
  }
  else
  {
    tcb_ptr->acker = timer_handle;
  }

  /*-------------------------------------------------------------------------
    Allocate the close(2MSL) timer.
    If allocation fails, free any other allocated timers and return failure.
  -------------------------------------------------------------------------*/
  timer_handle = ps_timer_alloc( dsstcpi_close_timer_cb, tcb_ptr);
  if (timer_handle == PS_TIMER_INVALID_HANDLE)
  {
    goto bail;
  }
  else
  {
    tcb_ptr->close_timer = timer_handle;
  }

  /*-------------------------------------------------------------------------
    Allocate the half open timer
    If allocation fails, free any other allocated timers and return failure.
  -------------------------------------------------------------------------*/
  timer_handle = ps_timer_alloc(dsstcpi_half_open_keepalive_timer_cb,tcb_ptr);
  if (timer_handle == PS_TIMER_INVALID_HANDLE)
  {
    goto bail;
  }
  else
  {
    tcb_ptr->half_open_keepalive_timer = timer_handle;
  }

  /*-------------------------------------------------------------------------
    Allocate the persist timer
    If allocation fails, free any other allocated timers and return failure.
  -------------------------------------------------------------------------*/
  timer_handle = ps_timer_alloc(dsstcpi_persist_timer_cb, tcb_ptr);
  if (timer_handle == PS_TIMER_INVALID_HANDLE)
  {
    goto bail;
  }
  else
  {
    tcb_ptr->persist_timer = timer_handle;
  }

  /*-------------------------------------------------------------------------
    Allocate the persist timer
    If allocation fails, free any other allocated timers and return failure.
  -------------------------------------------------------------------------*/
  timer_handle = ps_timer_alloc(dsstcpi_linger_timer_cb, tcb_ptr);
  if (timer_handle == PS_TIMER_INVALID_HANDLE)
  {
    goto bail;
  }
  else
  {
    tcb_ptr->linger_timer = timer_handle;
  }

  /*-------------------------------------------------------------------------
    Initialize link and add TCB to queue
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( &global_ps_crit_section);
  (void) q_link(tcb_ptr, &(tcb_ptr->link));
  q_put(&sock_config_cb.tcp_cb_q, &(tcb_ptr->link));
  PS_LEAVE_CRIT_SECTION( &global_ps_crit_section);

  SOCK_INC_STATS(tcp_socks_allocated, 1);
  SOCK_INC_STATS(open_tcp_socks, 1);
  SOCK_UPDATE_MAX_OPEN_STAT(tcp);
  return (tcb_ptr);

bail:
  if (PS_TIMER_INVALID_HANDLE != tcb_ptr->timer &&
      PS_TIMER_SUCCESS != ps_timer_free( tcb_ptr->timer))
  {
    LOG_MSG_ERROR_1("Error freeing timer 0x%x", tcb_ptr->timer);
    ASSERT(0);
  }

  if (PS_TIMER_INVALID_HANDLE != tcb_ptr->acker &&
      PS_TIMER_SUCCESS != ps_timer_free( tcb_ptr->acker))
  {
    LOG_MSG_ERROR_1("Error freeing timer 0x%x", tcb_ptr->acker);
    ASSERT(0);
  }

  if (PS_TIMER_INVALID_HANDLE != tcb_ptr->close_timer &&
      PS_TIMER_SUCCESS != ps_timer_free( tcb_ptr->close_timer))
  {
    LOG_MSG_ERROR_1("Error freeing timer 0x%x", tcb_ptr->close_timer);
    ASSERT(0);
  }

  if (PS_TIMER_INVALID_HANDLE != tcb_ptr->half_open_keepalive_timer &&
      PS_TIMER_SUCCESS != ps_timer_free( tcb_ptr->half_open_keepalive_timer))
  {
    LOG_MSG_ERROR_1("Error freeing timer 0x%x",
                    tcb_ptr->half_open_keepalive_timer);
    ASSERT(0);
  }

  if (PS_TIMER_INVALID_HANDLE != tcb_ptr->persist_timer &&
      PS_TIMER_SUCCESS != ps_timer_free( tcb_ptr->persist_timer))
  {
    LOG_MSG_ERROR_1("Error freeing timer 0x%x", tcb_ptr->persist_timer);
    ASSERT(0);
  }

  PS_MEM_FREE(tcb_ptr);
  SOCK_INC_STATS(tcb_alloc_failures, 1);

  return NULL;
} /* dsstcpi_create_tcb() */



/*===========================================================================
FUNCTION DSSTCPI_FREE_TCB()

DESCRIPTION
  This function frees TCP control block, making it available for re-use.

  This function executes in the context of the PS task.

ARGUMENTS:
  tcb_ptr: This is a pointer to the TCP control block that is being freed.
  This argument must point to a valid TCP control block and cannot be NULL.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets the protocol control block of the scb_ptr to NULL.
===========================================================================*/
void dsstcpi_free_tcb
(
  struct tcb* tcb_ptr                            /* Ptr to TCB to be freed */
)
{
  dsm_item_type *item_ptr;      /* To hold dsm_items that need to be freed */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check for valid arguments. tcb_ptr should not be NULL.
  -------------------------------------------------------------------------*/
  if (tcb_ptr == NULL)
  {
    DATA_ERR_FATAL(" TCP control block ptr is NULL ");
    return;
  }

  /*-------------------------------------------------------------------------
    TCP state for this TCB should always be closed because this function is
    only called when TCP state changes to CLOSED.
  -------------------------------------------------------------------------*/
  ASSERT( tcb_ptr->state == TCP_CLOSED);

  /*-------------------------------------------------------------------------
    Inform diag tools that the instance is being removed
  -------------------------------------------------------------------------*/
  ps_stat_logging_inst_update_desc(PS_STAT_MODULE_INST_TCP,
                                   tcb_ptr->sockfd,
                                   PS_STAT_INST_DELETED);

  /*-------------------------------------------------------------------------
    Cleanup the socket timers for this tcb.
  -------------------------------------------------------------------------*/
  if (PS_TIMER_SUCCESS != ps_timer_free( tcb_ptr->timer) )
  {
    LOG_MSG_ERROR_1("Error freeing timer 0x%x", tcb_ptr->timer);
    ASSERT(0);
  }

  if (PS_TIMER_SUCCESS != ps_timer_free( tcb_ptr->acker) )
  {
    LOG_MSG_ERROR_1("Error freeing timer 0x%x", tcb_ptr->acker);
    ASSERT(0);
  }

  if (PS_TIMER_SUCCESS != ps_timer_free( tcb_ptr->close_timer) )
  {
    LOG_MSG_ERROR_1("Error freeing timer 0x%x", tcb_ptr->close_timer);
    ASSERT(0);
  }

  if (PS_TIMER_SUCCESS != ps_timer_free( tcb_ptr->half_open_keepalive_timer) )
  {
    LOG_MSG_ERROR_1("Error freeing timer 0x%x",
                    tcb_ptr->half_open_keepalive_timer);
    ASSERT(0);
  }

  if (PS_TIMER_SUCCESS != ps_timer_free( tcb_ptr->persist_timer) )
  {
    LOG_MSG_ERROR_1("Error freeing timer 0x%x", tcb_ptr->persist_timer);
    ASSERT(0);
  }

  if (PS_TIMER_SUCCESS != ps_timer_free( tcb_ptr->linger_timer) )
  {
    LOG_MSG_ERROR_1("Error freeing timer 0x%x", tcb_ptr->linger_timer);
    ASSERT(0);
  }

  /*-------------------------------------------------------------------------
    Cleanup the SACK information for TCP.
  -------------------------------------------------------------------------*/
  tcp_clear_sack_data(tcb_ptr);

  /*-------------------------------------------------------------------------
    Cleanup TCP reseq. queue.
  -------------------------------------------------------------------------*/
  while (( item_ptr = (dsm_item_type *)q_get( &(tcb_ptr->reseq))) != NULL)
  {
    dsm_free_packet( &item_ptr);
  }

  /*-------------------------------------------------------------------------
    Destroy the reseq queue
  -------------------------------------------------------------------------*/
  q_destroy(&(tcb_ptr->reseq));

  if (tcb_ptr->ipcb.meta_info_ptr != NULL)
  {
    PS_TX_META_INFO_FREE(&(tcb_ptr->ipcb.meta_info_ptr));
  }

  if (tcb_ptr->reader != NULL)
  {
    ReleaseReader(tcb_ptr->reader);
  }

  if (tcb_ptr->writer != NULL)
  {
    ReleaseWriter(tcb_ptr->writer);
  }

  /*-------------------------------------------------------------------------
    Remove TCB from the queue and make it available for reuse.

    NOTE : TCB must be memset to 0 because application could call dss_close()
    multiple times with same sockfd. By memsetting to 0, sockfd's status
    would become SOCKAVAIL and control block is not freed again because
    dsstcp_close() frees a control block only if sockfd's status is not
    SOCKAVAIL.
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION( &global_ps_crit_section);
  q_delete(&sock_config_cb.tcp_cb_q, (q_link_type *) tcb_ptr);
  memset(tcb_ptr, 0, sizeof(struct tcb));
  PS_MEM_FREE(tcb_ptr);
  PS_LEAVE_CRIT_SECTION( &global_ps_crit_section);

  SOCK_DEC_STATS(open_tcp_socks, 1);

} /* dsstcpi_free_tcb() */

