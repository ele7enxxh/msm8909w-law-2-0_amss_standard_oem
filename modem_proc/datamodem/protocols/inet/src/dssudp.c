/*===========================================================================

                           D S S U D P . C

DESCRIPTION

 The Data Services sockets UDP interface file. This contains all the
 functions used by the sockets library to access UDP socket function calls.

EXTERNALIZED FUNCTIONS
  dssudp_socket()
    UDP specific socket() function.  Sets up the UDP socket and creates
    the UDP control block.

  dssudp_write()
    UDP specific write() function.  Sends specified number of bytes out
    the UDP datagram transport.

  dssudp_read()
    UDP specific read() function.  Reads specified number of bytes from
    the UDP datagram transport.

  dssudp_close()
    UDP specific close() function.  Simply cleans up socket control
    block and makes the socket available for re-use.

  dssudp_bind()
    This is the UDP specific bind() function. It writes the port value into
    local.conn.port of the socket control block structure.

  dssudp_connect_setup()
    This is the UDP specific connect function. It fills in the peers ipaddr
    and binds the socket to a particular interface.

  dssudp_shutdown()
    This is the UDP specific shutdown function. It clears the receive queue
    of the socket and generates a socket event.

  dssudp_lookup_conn()
    Given a connection structure, looks up the connection to determine
    if this is a valid connection.

  dssudp_process_rx_pkt()
    Processes incoming UDP packet to see which socket it belongs to.

  dssudp_new_ip_upcall()
    If a new IP address is negotiated with the Base Station, then update
    udp ctrl blocks of the sockets using the interface with the new address

dssudp_set_rscope()
    UDP specific set_rscope function. Sets the incoming route scope in
    the UCB control block.

Copyright (c) 1998--2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dssudp.c_v   1.1   08 Oct 2002 23:16:52   akuzhiyi  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dssudp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/23/11    bd     Clean up pkt_meta_info_ptr() in the ip_cb control block 
                   after each packet write
10/01/10    sa     Added appropriate log messages before ASSERT(0).
05/22/09    kk     Q6 compiler warning fixes.
03/17/03    mct    Ensure we clear the source ip addr when our ip changes.
12/05/06    msr    Passing sockfd in ps_event_info_type
11/22/06    sv     Added support for IPSEC transport mode.
11/04/06    rt     Set the data_available flag as FALSE if the rcvcnt is 0
                   in UDP read functions.
11/03/06    hm     Using dssocki_alloc_ps_cmd_buffer and dssocki_send_ps_cmd_buffer
                   instead of dssocki_send_to_ps in dssudp_connect_setup.
11/02/06    mct    Added support for IPv6 Privacy Extensions.
07/31/06    rt     Route Scope modifications.
04/24/06    mct    Drop UDP packets if routing_cache is null.
04/17/06    rt     Changed comparison of uint in dssudp_read().
03/27/06    rt     L4 TASKLOCK review changes.
02/22/06    rt     Using single critical section
02/07/06    rt     L4 TASKLOCK changes.
01/31/05    rt     Added new F3 message in BIND function.
12/03/05    rt     Added a new scb_ptr field in dssocki_send_to_ps() fn and
                   got rid of pcb and protocol fields.
11/02/05    msr    Freeing metainfo in IPCB when dssudpi_free_ucb() is called
08/29/05    vp     Fixed bug in dssudp_read.
08/16/05    vp     Cleanup for propagating sys sock info to ipcb.
08/15/05    mct    Removed bcmcs featurization for multicast support.
08/15/05    vp     Propagating sys sock info to ipcb.
08/15/05    kr     Added support for stats update
08/15/05    vp     Support for IP_RECVERR.
08/12/05    sv     Cache ipcb routing cache from metainfo.
08/05/05    sv     Cache meta_info_ptr in IPCB.
04/18/05    vp     Changes for enforcing netpolicy on input.
04/18/05    msr    Fixed incorrect loop traversal in dssudp_lookup_conn().
04/17/05    msr    Using new signature of dssocki_generate_meta_info().
03/01/05    vp     Changes due to change in signature of dssocki_fill_sockaddr
02/22/05    ssh    Replaced INTLOCK()s with TASKLOCK()s
02/15/05    msr    Calling ip_setopt() for IPV6 level options.
02/11/05    msr    Moved binding a non-zero port for ephemeral ports to
                   dssocki_sendmsg().
12/27/04    ifk    Replaced setting of tracer field with DSM_TOUCH_ITEM
11/24/04    msr    memsetting UCB to 0 after it is freed in
                   dssudpi_free_ucb().
11/10/04    mct    Pull iface id off packet if IP_RECVIF option is set.
10/13/04    vp     Changes in dssudp_write() as port numbers are expected in
                   network order.
08/16/04    ks/msr Integrated UCBs in to ps_mem memory allocation.
                   Maintaining allocated UCBs in a list instead of array.
06/11/04    vp     Removal of included files psglobal.h, netuser.h. Included
                   files dssocket_defs.h and ps_socket_defs.h. Changes for
                   representation of IP addresses as struct ps_in_addr or struct
                   ps_in6_addr
05/25/04    sv     Modified dssudp_init_ucb to not to use high watermark for
                   rcvq of UDP socket.
05/17/04    sv     Modified dssudp_read to read a maximum of DSS_READ_MAX_BYTES
                   in a single read call.
04/028/04   aku    Added support for BCMCS feature.
03/30/04    vp     Merged changes from June04Dev_05.00.00 PS branch.
03/18/04    vp     Changes for removing direct scb access from PS context.
                   All such events are posted through info_change_upcall.
03/10/04    vp     Changes for dssocki_socket_event_occurred() to be executed
                   through the upcall mechanism.
02/24/04    sv     Added Null pointer check in dssudp_connect_setup.
02/04/04    vp     Replaced use of PS_IFACE_IP_V4_ADDR with ps_iface_get_addr
02/02/04    sv     Fixed host byte order bug in new_ip_upcall.
01/26/04    sv     Modified dssudp_read and dssudp_read_dsm_chain to use
                   the new watermark rcvq for UDP datagrams.
01/14/04    vp     Added function dssudp_new_ip_upcall() to take care of IP
                   address change due to handoff for udp sockets.
10/02/03    sv     Changed protocol specific function to take pcb instead
                   of SCB.
10/01/03    sv     Modified code so that udp_cb is accessed and freed only
                   in the PS task.
08/27/03    sv     Added TASKLOCK to udp connect code to avoid potential
                   race condition.
08/18/03    sv     Modified the code to use IPv6 addresses internally.
08/01/03    ss/sv  Restructured the code to support IPv6
07/22/03    sv     Modified the code to use new DSM macros for getting the
                   DSM declarations.
07/17/03    ss     Decremented the rcvcnt in dssudp_read() while discarding
                   data due to the supplied buffer being too small
05/28/03    ss     Changed code to handle rcvq as a dsm item chain instead of
                   queue. Added support for rcvbuf socket option.
05/27/03    om     Added dssudp_read_dsm_chain().
05/13/03    ss     Removed an unused variable declaration from dssudp_close()
05/08/03    ss     Added dssudpi_free_rcvq() and INTLOCK'ed queue accesses
05/05/03    ss     Protected q_put() and q_get() within INTLOCK's
04/15/03    sv     Added dssudp_connect_setup and dssudp_shutdown functions.
                   Modified dssudp_read to accept iov and iocount instead of
                   buffer and bufferlen as arguments.
04/15/03    sv     Modified dssudp_process_rx_pkt to support connected udp
                   sockets and to accept ucb_ptr and dsm_item as arguments.
                   Added dssudp_lookup_conn function.
04/10/03    ss     Modified dssudp_write() to change the parameter to
                   getephem_port()
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
08/04/02    aku    Merged in changes for multimode ps under FEATURE_DATA_MM
07/21/02    na     Fixed comment
07/15/02    na     Added dssudp_process_rx_pkt() (used to be ps_process_udp
                   in old psmgr.c)
07/14/02    na     Fixed ip_addr_type to support FEATURE_DATA_MM.
05/02/02    mvl    File clean up, added meta_info_ptr to write()
04/08/02    ss     Modified dssudp_read() to truncate the datagram if the
                   supplied buffer is too small.
12/18/01    rsl    Updated tracer field.
07/23/01    pjb    Return type of dsm_pushdown changed.
06/27/01    na     Replaced DSSUDPI_MAX_UDP_SOCKS with DSS_MAX_UDP_SOCKS
                   making the usage consistent.
03/27/01    mt     Changed type of second argument in dssudp_read() from
                   byte * to void *.
03/27/01    js     Added extra parameter to function dssudp_write(). The
                   extra parameter holds a pointer to the structure holding
                   the destination information. This avoids using the
                   destination information in scb, which may be stale.
02/14/01    js     Modified call to udp_output() to support DSS_IP_TTL.
11/10/00    snn    Renamed scb_type as scb_s
08/25/00    rc     Changed dsm_pushdown() to take take pool id as fourth
                   parameter.Made changes to handle dsm_pushdown()
                   returning FALSE.
08/03/00    mvl/js Added function dssudp_bind() for the protocol specific
                   assignment of local.conn.port.
06/07/00    rc     Removed ovld of link ptr in dsm items.
05/11/00    mvl    Modified DSSUDPI_MAX_UDP_SOCKS to support multiple UDP
                   sockets (the current number is 4).
12/23/99    na     Changed q_inserts to use the ovld of link ptr in dsm items.
04/09/99    hcg    Added FEATURE_Q_NO_SELF_QPTR to support queue services memory
                   optimization.
04/08/99    hcg    Added host to network byte conversion for dssudp_read() to
                   convert the incoming IP address to network byte ordering.
04/05/99    hcg    Incorporated code review changes and comments.  Changed
                   some of the diagnostic message levels.  Modified use of
                   INTLOCK()/INTFREE() macros to ensure protection of access
                   to global data.
12/09/98   na/hcg Created module.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "amssassert.h"
#include "msg.h"
#include "dssocket.h"
#include "dssocket_defs.h"
#include "ps.h"
#include "dsm.h"
#include "ps_iface.h"
#include "ps_ifacei.h"
#include "ps_ip.h"
#include "memory.h"
#include "msg.h"
#include "err.h"
#include "ps_tx_meta_info.h"
#include "ps_metai_info.h"
#include "ps_socket_defs.h"
#include "ps_socket_event_defs.h"
#include "ps_stat_udp.h"
#include "ps_stat_sock.h"
#include "ps_route.h"
#include "ps_pcb_close_reason_codes.h"
#include "ps_stat_logging.h"

#include "ps_udp_io.h"
#include "dssudp.h"
#include "ds_Sock_Def.h"
#include "PS_Sock_IO_IReader.h"
#include "PS_Sock_IO_IWriter.h"
#include "ds_Utils_DebugMsg.h"



/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION DSSUDPI_CREATE_UCB()

DESCRIPTION
  This function creates a new UDP control block.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to UDP control block structure.  If none are
  available, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
static struct udp_cb *dssudpi_create_ucb
(
  void  * scb_handle,
  int32   sockfd
)
{
  struct udp_cb* ucb_ptr = NULL;               /* Ptr to UCB to be created */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == scb_handle)
    {
    DATA_ERR_FATAL("NULL scb_ptr passed");
    SOCK_INC_STATS(ucb_alloc_failures, 1);
    return NULL;
  }

  ucb_ptr = (struct udp_cb *) ps_mem_get_buf(PS_MEM_DSS_UCB_TYPE);

  /*-------------------------------------------------------------------------
    If no UDP control block is available, return NULL.
   ------------------------------------------------------------------------*/
  if (NULL == ucb_ptr)
  {
    SOCK_INC_STATS(ucb_alloc_failures, 1);
    LOG_MSG_ERROR_0("Memory allocation for UCB failed");
    return (NULL);
  }

  memset(ucb_ptr, 0, sizeof(struct udp_cb));
  ucb_ptr->scb_ptr = scb_handle;
  ucb_ptr->sockfd  = sockfd;

  /*-------------------------------------------------------------------------
    Initialize link and add UCB to queue
  -------------------------------------------------------------------------*/
  (void) q_link(ucb_ptr, &(ucb_ptr->link));
  q_put(&sock_config_cb.udp_cb_q, &(ucb_ptr->link));

  SOCK_INC_STATS(open_udp_socks, 1);
  SOCK_INC_STATS(udp_socks_allocated, 1);
  SOCK_UPDATE_MAX_OPEN_STAT(udp);

  return (ucb_ptr);
} /* dssudpi_create_ucb() */



/*===========================================================================
FUNCTION DSSUDPI_INIT_UCB()

DESCRIPTION
  This function creates a new UDP control block.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to UDP control block structure.  If none are
  available, returns NULL.

SIDE EFFECTS
  None.
===========================================================================*/
static void dssudpi_init_ucb
(
  struct udp_cb * ucb_ptr                      /* Ptr to UDP control block */
)
{
  ip_init_ipcb(&ucb_ptr->ipcb);
  return;
}



/*===========================================================================
FUNCTION DSSUDPI_FREE_UCB()

DESCRIPTION
  This function frees UDP control block, making it available for re-use.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void dssudpi_free_ucb
(
  struct udp_cb* ucb_ptr                         /* Ptr to UCB to be freed */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == ucb_ptr )
  {
    DATA_ERR_FATAL("NULL UCB passed");
    return;
  }

  ps_stat_logging_inst_update_desc(PS_STAT_MODULE_INST_UDP,
                                   ucb_ptr->sockfd,
                                   PS_STAT_INST_DELETED);

  if (ucb_ptr->ipcb.meta_info_ptr != NULL)
  {
    PS_TX_META_INFO_FREE(&(ucb_ptr->ipcb.meta_info_ptr));
  }

  /*-------------------------------------------------------------------------
    Release sndq, rcvq, and errq
  -------------------------------------------------------------------------*/
  if (ucb_ptr->reader != NULL)
  {
    ReleaseReader(ucb_ptr->reader);
  }

  if (ucb_ptr->writer != NULL)
  {
    ReleaseWriter(ucb_ptr->writer);
  }

  if (ucb_ptr->errq_ptr != NULL)
  {
    ReleaseWriter(ucb_ptr->errq_ptr);
  }

  /*-------------------------------------------------------------------------
    Remove UCB from the queue and free it.

    NOTE : UCB must be memset to 0 because application could call dss_close()
    multiple times with same sockfd. By memsetting to 0, sockfd's status
    would become SOCKAVAIL and control block is not freed again because
    dssudp_close() frees a control block only if sockfd's status is not
    SOCKAVAIL.
   ------------------------------------------------------------------------*/
  q_delete(&sock_config_cb.udp_cb_q, (q_link_type *) ucb_ptr);
  memset(ucb_ptr, 0, sizeof(struct udp_cb));
  PS_MEM_FREE(ucb_ptr);

  SOCK_DEC_STATS(open_udp_socks, 1);
} /* dssudpi_free_ucb() */



/*===========================================================================

                      GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION DSSUDP_SOCKET()

DESCRIPTION
  This function sets up the UDP socket and creates the UDP control block.

  This function is called from the context of the application task.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS

  On error, return DSS_ERROR and places the error condition value in *ps_errno.

  Errno Values
  ------------
  DS_EMFILE           too many descriptors open.  A socket is already open or
                      has not closed compeletely.
  DS_EPROTOTYPE       specified protocol invalid for socket type.

SIDE EFFECTS
  None.
===========================================================================*/
int16 dssudp_socket
(
  uint16    family,
  void    * scb_handle,
  int32     sockfd,
  int32   * pcb_handle_ptr,
  void    * reader_ptr,
  void    * writer_ptr,
  void    * errq_ptr,
  int16   * errno_ptr
)
{
  struct udp_cb *  ucb_ptr;
  (void)family;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check arguments.
  -------------------------------------------------------------------------*/
  if (errno_ptr == NULL)
  {
    DATA_ERR_FATAL(" dssudp_socket(): errno_ptr is NULL ");
    return DSS_ERROR;
  }

  if (scb_handle == NULL || reader_ptr == NULL || writer_ptr == NULL ||
      errq_ptr == NULL || pcb_handle_ptr == NULL)
  {
    *errno_ptr = DS_EFAULT;
    DATA_ERR_FATAL(" dssudp_socket(): Invalid argument passed ");
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Create UDP control block
   ------------------------------------------------------------------------*/
  if ( (ucb_ptr = dssudpi_create_ucb( scb_handle, sockfd)) == NULL)
  {
    SOCK_INC_STATS(ucb_alloc_failures, 1);
    LOG_MSG_INFO1_0(" Could not create a UCB");
    *errno_ptr = DS_EMFILE;
    return DSS_ERROR;
  }

  dssudpi_init_ucb( ucb_ptr);

  ucb_ptr->reader   = reader_ptr;
  ucb_ptr->writer   = writer_ptr;
  ucb_ptr->errq_ptr = errq_ptr;
  *pcb_handle_ptr   = (int32) ucb_ptr;

  return DSS_SUCCESS;
} /* dssudp_socket() */


/*===========================================================================
FUNCTION DSSUDP_WRITE()

DESCRIPTION
  Sends dsm memory item out to UDP using udp_output found in udp.c.

  This function is called from the context of the PS task.

DEPENDENCIES
  None.

RETURN VALUE
  The number of bytes written.

SIDE EFFECTS
  None.
===========================================================================*/
void dssudp_write
(
  int32                     pcb_handle,
  int32                     sockfd,
  int32                     pkt_length,
  struct ps_sockaddr_in6  * ta_ptr,
  ps_pkt_meta_info_type   * pkt_meta_info_ptr
)
{
  struct ps_in6_addr  * dest_ip_ptr;
  struct udp_cb       * ucb_ptr = (struct udp_cb *) pcb_handle;
  dsm_item_type       * item_ptr;
  udp_hdr_type          udp_hdr;
  int16                 length;
  int16                 ps_errno;
  (void) pkt_length;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ucb_ptr)
  {
    DATA_ERR_FATAL("NULL UCB");
    return;
  }

  if (ucb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != UCB's sockfd %d", sockfd, ucb_ptr->sockfd);
    return;
  }

  if (pkt_meta_info_ptr != NULL)
  {
    PS_TX_META_SET_PKT_META_INFO_PTR(ucb_ptr->ipcb.meta_info_ptr,
                                     pkt_meta_info_ptr);
  }

  /*-------------------------------------------------------------------------
    Ephemeral ports are taken care of in AMSS sockets platform
  -------------------------------------------------------------------------*/
  ASSERT(0 != ucb_ptr->conn.local.port);

  /*-------------------------------------------------------------------------
    Fill in the known UDP header fields, except the checksum.
  -------------------------------------------------------------------------*/
  udp_hdr.source = ucb_ptr->conn.local.port;

  /*-------------------------------------------------------------------------
    If destination information is passed as parameter, use it. Otherwise, use
    the cached information from UCB
  -------------------------------------------------------------------------*/
  if (!PS_IN6_ARE_ADDR_EQUAL(&(ta_ptr->ps_sin6_addr), &ps_in6addr_any))
  {
    dest_ip_ptr = &(ta_ptr->ps_sin6_addr);
    udp_hdr.dest = ta_ptr->ps_sin6_port;
  }
  else
  {
    dest_ip_ptr = &(ucb_ptr->conn.remote.address);
    udp_hdr.dest = ucb_ptr->conn.remote.port;
  }

  (void) Read(ucb_ptr->reader, &item_ptr, NULL, NULL, &ps_errno);

  length         = (sint15) dsm_length_packet(item_ptr);
  udp_hdr.length = (word) ((uint16) length + sizeof(udp_hdr_type));

  udp_output(dest_ip_ptr, &udp_hdr, item_ptr, ucb_ptr, NULL);

  /*-------------------------------------------------------------------------
    Free the old pkt meta info
  -------------------------------------------------------------------------*/
  if (PS_TX_META_GET_PKT_META_INFO_PTR(ucb_ptr->ipcb.meta_info_ptr) != NULL)
  {
    PS_PKT_META_INFO_FREE
    (
      &PS_TX_META_GET_PKT_META_INFO_PTR(ucb_ptr->ipcb.meta_info_ptr)
    );
  }
  
  return;
} /* dssudp_write() */


/*===========================================================================
FUNCTION DSSUDP_CLOSE()

DESCRIPTION
  This function is the UDP specific close() function.  It simply sets the
  socket state to DSSOCKI_NULL and frees up the socket control block and
  related data structures.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS.

SIDE EFFECTS
  Socket state is set to NULL.  Socket is freed and made available for
  re-use.
===========================================================================*/
void dssudp_close
(
  int32    pcb_handle,
  int32    sockfd,
  boolean  silent_close_flag
)
{
  struct udp_cb              * ucb_ptr = (struct udp_cb *) pcb_handle;
  ps_socket_event_info_type    info;
  boolean invoke_cb_flag = FALSE;
  (void)silent_close_flag;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == ucb_ptr)
  {
    DATA_ERR_FATAL("NULL UCB");
    return;
  }

  if (ucb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != UCB's sockfd %d", sockfd, ucb_ptr->sockfd);
    return;
  }

  if (NULL != ucb_ptr->scb_ptr)
  {
    /*-------------------------------------------------------------------------
      Post event to socket platform
    -------------------------------------------------------------------------*/
    info.scb_ptr                = ucb_ptr->scb_ptr;
    info.data.pcb_closed_reason = PS_PCB_CLOSE_REASON_CODE_NORMAL;
    invoke_cb_flag = TRUE;
  }

  /*-------------------------------------------------------------------------
    Free UCB
  -------------------------------------------------------------------------*/
  dssudpi_free_ucb(ucb_ptr);

  if (TRUE == invoke_cb_flag)
  {
    socket_platform_event_cback_f_ptr(PS_SOCKET_PCB_CLOSED_EV, &info);
  }

  return;
  
} /* dssudp_close() */


/*===========================================================================
FUNCTION DSSUDP_BIND()

DESCRIPTION
  This is the UDP specific bind() function. It writes the port value into
  local.conn.port of the socket control block structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssudp_bind
(
  int32                     pcb_handle,
  int32                     sockfd,
  struct ps_sockaddr_in6 *  local_addr_ptr
)
{
  struct udp_cb * ucb_ptr = (struct udp_cb *) pcb_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ucb_ptr)
  {
    DATA_ERR_FATAL("NULL UCB");
    return;
  }

  if (ucb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != UCB's sockfd %d", sockfd, ucb_ptr->sockfd);
    return;
  }

  LOG_MSG_INFO2_2("Called UDP BIND on sock %d with port %d",
                  sockfd, dss_ntohs(local_addr_ptr->ps_sin6_port));

  ucb_ptr->conn.local.port    = local_addr_ptr->ps_sin6_port;
  ucb_ptr->conn.local.address = local_addr_ptr->ps_sin6_addr;
  ucb_ptr->ipcb.src_ip_addr   = local_addr_ptr->ps_sin6_addr;

  ps_stat_logging_inst_update_desc(PS_STAT_MODULE_INST_UDP,
                                   ucb_ptr->sockfd,
                                   PS_STAT_INST_CREATED);

}


/*===========================================================================
FUNCTION DSSUDP_CONNECT()

DESCRIPTION
  This function sets the remote address for the udp socket.

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
void dssudp_connect
(
  int32                     pcb_handle,
  int32                     sockfd,
  struct ps_sockaddr_in6 *  dest_addr_ptr
)
{
  struct udp_cb              * ucb_ptr = (struct udp_cb *) pcb_handle;
  ps_ip_addr_type              ip_addr;
  ps_socket_event_info_type    info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ucb_ptr)
  {
    DATA_ERR_FATAL("NULL UCB");
    return;
  }

  if (ucb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != UCB's sockfd %d", sockfd, ucb_ptr->sockfd);
    return;
  }

  ucb_ptr->conn.remote.address = dest_addr_ptr->ps_sin6_addr;
  ucb_ptr->conn.remote.port    = dest_addr_ptr->ps_sin6_port;

  /*-------------------------------------------------------------------------
    Update flow_label from dest_ptr in IPCB
  -------------------------------------------------------------------------*/
  ucb_ptr->ipcb.flow_label = dest_addr_ptr->ps_sin6_flowinfo;

  if (!PS_IN6_IS_ADDR_UNSPECIFIED(&( ucb_ptr->conn.remote.address)))
  {
    if(PS_IN6_IS_ADDR_UNSPECIFIED(&( ucb_ptr->conn.local.address)))
    {
      /*---------------------------------------------------------------------
        Check if routing cache is present
      ---------------------------------------------------------------------*/
      if (ucb_ptr->ipcb.meta_info_ptr == NULL ||
          PS_TX_META_GET_RT_META_INFO_PTR(ucb_ptr->ipcb.meta_info_ptr) == NULL)
      {
        DATA_ERR_FATAL("IPCB meta_info_ptr is NULL in dssudp_connect");
        return;
      }

      /*---------------------------------------------------------------------
        Get the source address with appropriate scope based on destination
        addr.  This must be done for ICMPv6 messages.
      ---------------------------------------------------------------------*/
      ps_iface_get_ipv6_addr((ps_iface_type *) PS_TX_META_GET_ROUTING_CACHE(ucb_ptr->ipcb.meta_info_ptr), &dest_addr_ptr->ps_sin6_addr, &ip_addr);

      if(ip_addr.type == IP_ADDR_INVALID)
      {
        LOG_MSG_INFO1_0("Error");
        return;
      }

      ucb_ptr->conn.local.address = ip_addr.addr.v6;
      ucb_ptr->ipcb.src_ip_addr   = ip_addr.addr.v6;
    }
  }

  info.scb_ptr   = ucb_ptr->scb_ptr;
  info.data.conn = ucb_ptr->conn;

  socket_platform_event_cback_f_ptr(PS_SOCKET_UDP_CONNECTED_EV, &info);

  /*-------------------------------------------------------------------------
    Stat/logging stuff
  -------------------------------------------------------------------------*/
  ps_stat_logging_inst_update_desc(PS_STAT_MODULE_INST_UDP,
                                   ucb_ptr->sockfd,
                                   PS_STAT_INST_CREATED);

  return;

} /* dssudp_connect() */


/*===========================================================================
FUNCTION DSSUDP_SHUTDOWN()

DESCRIPTION
  This function implements the half-close functionality for udp sockets.

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
void dssudp_shutdown
(
  int32  pcb_handle,
  int32  sockfd,
  int32  shutdown_dir
)
{
  struct udp_cb      *  ucb_ptr = (struct udp_cb *) pcb_handle;
  (void)shutdown_dir;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ucb_ptr)
  {
    DATA_ERR_FATAL("NULL UCB");
    return;
  }

  if (ucb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != UCB's sockfd %d", sockfd, ucb_ptr->sockfd);
    return;
  }

  return;
} /* dssudp_shutdown() */


/*===========================================================================
FUNCTION DSSUDP_SETOPT()

DESCRIPTION
  This function sets the option to the user specified value.

DEPENDENCIES
  None.

PARAMETERS
  ucb_ptr - pointer to udp_cb

RETURN VALUE
  NONE.

SIDE EFFECTS
  None.
===========================================================================*/
void dssudp_setopt
(
  int32  pcb_handle,
  int32  sockfd,
  int32  opt_level,
  int32  opt_name,
  int32  opt_val,
  int32  opt_len
)
{
  struct udp_cb *  ucb_ptr = (struct udp_cb *) pcb_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ucb_ptr)
  {
    DATA_ERR_FATAL("NULL UCB");
    return;
  }

  if (ucb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != UCB's sockfd %d", sockfd, ucb_ptr->sockfd);
    return;
  }

  LOG_MSG_INFO2_1("UDP Setopt called for socket option %d", opt_name);

  switch (opt_name)
  {
    case ds_Sock_OptName_QDS_SO_RCVBUF:
      break;

    default:
      /*---------------------------------------------------------------------
        If option is an IP level option, call IP function to process
        the option
      ---------------------------------------------------------------------*/
      if (ds_Sock_OptLevel_QDS_LEVEL_IP == opt_level ||
          ds_Sock_OptLevel_QDS_LEVEL_IPV6 == opt_level)
      {
        ip_setopt(&ucb_ptr->ipcb, opt_level, opt_name, &opt_val, opt_len);
      }

      break;
  } /* end switch() */

} /* dssudp_setopt() */


/*===========================================================================
FUNCTION DSSUDP_SET_ROUTE_META_INFO()

DESCRIPTION
  This function processes setting of routing meta info inside IPCB.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssudp_set_route_meta_info
(
  int32                   pcb_handle,
  int32                   sockfd,
  ps_rt_meta_info_type *  rt_mi_ptr
)
{
  struct udp_cb         * ucb_ptr = (struct udp_cb *) pcb_handle;
  ps_tx_meta_info_type  * tx_mi_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (rt_mi_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL rt_mi_ptr");
    return;
  }

  if (NULL == ucb_ptr)
  {
    PS_RT_META_INFO_FREE(&rt_mi_ptr);
    DATA_ERR_FATAL("NULL UCB");
    return;
  }

  if (ucb_ptr->sockfd != sockfd)
  {
    PS_RT_META_INFO_FREE(&rt_mi_ptr);
    LOG_MSG_INFO1_2("Sockfd %d != UCB's sockfd %d", sockfd, ucb_ptr->sockfd);
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
  if (ucb_ptr->ipcb.meta_info_ptr != NULL)
  {
    PS_TX_META_INFO_FREE(&ucb_ptr->ipcb.meta_info_ptr);
  }

  ucb_ptr->ipcb.meta_info_ptr = tx_mi_ptr;

  ps_ip_update_routing_cache_in_ipcb
  (
    &(ucb_ptr->ipcb),
    (ps_iface_type *)
      PS_TX_META_GET_ROUTING_CACHE(ucb_ptr->ipcb.meta_info_ptr)
  );

  return;
} /* dssudp_set_route_meta_info() */


/*===========================================================================
FUNCTION DSSUDP_SET_ROUTE_SCOPE()

DESCRIPTION
  This is the UDP specific set_route_scope() function. It writes the route
  scope in the UCB's IPCB

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssudp_set_route_scope
(
  int32                  pcb_handle,
  int32                  sockfd,
  ps_route_scope_type  * route_scope_ptr
)
{
  struct udp_cb  * ucb_ptr = (struct udp_cb *) pcb_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ucb_ptr)
  {
    DATA_ERR_FATAL("NULL UCB");
    return;
  }

  if (ucb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != UCB's sockfd %d", sockfd, ucb_ptr->sockfd);
    return;
  }

  if (NULL == route_scope_ptr)
  {
    DATA_ERR_FATAL("NULL rscope");
    return;
  }

  PS_ROUTE_SCOPE_PRINT(route_scope_ptr);

  ucb_ptr->ipcb.route_scope = *route_scope_ptr;
  return;

} /* dssudp_set_route_scope() */

