/*===========================================================================

        D A T A  S E R V I C E S  S O C K E T  I C M P  F I L E

DESCRIPTION

  The Data Services sockets ICMP interface file. This file contains all the
  functions used by the sockets library to access ICMP socket function calls.


EXTERNALIZED FUNCTIONS

  dssicmp_socket()
    This function creates an ICMP socket. NOTE: This function is cannot be
    mapped by the protocol specific function pointer. To create an ICMP
    socket, this function must be invoked directly (instead of through
    dss_socket()).

  dssicmp_read()
    Reads 'nbytes' bytes into the buffer from the ICMP transport.  Fills in
    address structure with values from who sent the data in fromaddr.
    This function asserts that fromaddr is not NULL. This function is the
    ICMP specific dss_recvfrom()

  dssicmp_write()
    Send DSM item out to ICMP (write to ICMP socket). The actual write is
    performed in icmp_output() in icmp.c This function is the ICMP
    specific dss_sendto()

  dssicmp_close()
    This function is the ICMP specific dss_close() function.  It simply sets
    the socket state to DSSOCKI_NULL and frees up the socket control block
    and related data structures.

  dssicmp_ps_input()
    When an ICMP message is arrived, if it is NOT ECHO, INFO_RQST or
    TIMESTAMP, the ICMP message will be handled by this fuction in the
    ps context. NOTE: the packet is not released in this should an
    error occured. Instead DSS_ERROR is returned and the packet will
    be freed by icmp_input().

  dssicmp_setopt()
    This function sets the ICMP options to the user specified value.

  dssicmp_set_route_meta_info()
    This function sets Routing Meta info inside IPCB.

Copyright (c) 2001-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dssicmp.c_v   1.2   26 Nov 2002 08:50:16   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dssicmp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/19/14    ss     Adding validation for incoming error value for 
                   dssicmp_int_socket
07/30/12    dvk    Fixed Klocwork errors
07/08/13    cx     Replace memcpy and memmove with memscpy and memsmove
10/01/10    sa     Added appropriate log messages before ASSERT(0).
07/20/10    sy     Removed critical section.
01/19/10    ss     Lint fixes.
01/15/10    ss     Fixed a bug related to simultaneous multiple ping sessions.
05/22/09    kk     Q6 compiler warning fixes.
03/13/08    pp     Metainfo optimizations.
07/26/07    ss     dssicmp_socket(): For DSSICMP_ICMP_V6, default policy's
                   family set to DSS_AF_INET6.
04/29/07    ss     Added support for creating, reading, writing to
                   ICMPv6 sockets. Added dssicmp6_ps_input().
02/28/07    ss/sv  Pass ICMP echo replies to the socket associated with echo
                   request.
12/05/06    msr    Passing sockfd in ps_event_info_type
04/24/06    mct    Drop ICMP packets if routing_cache is null.
03/27/06    rt     L4 TASKLOCK review comment changes.
02/22/06    rt     Using single critical section
02/07/06    rt     L4 TASKLOCK changes.
08/15/05    vp     Changed the signature of dssicmp_ps_input(). Check for
                   whether its a sys_sock if iface is in configuring state.
08/15/05    kr     Added support for stats update
08/15/05    vp     Addition of flags argument to dssicmp_read()
07/19/05    rt     Added passing of protocol field when calling
                   dssocki_allocscb()
05/17/05    rt     Fix made to set the socket family to DSS_AF_INET in
                   dssicmp_socket function.
03/01/05    vp     Change due to change in signature of dssocki_fill_sockaddr
02/22/05    ssh    Replaced INTLOCK()s with TASKLOCK()s
02/11/05    msr    Added ICMP type and code to SCB's cached info.
12/27/04    ifk    Replaced setting of tracer field with DSM_TOUCH_ITEM
11/24/04    msr    memsetting ICMCB to 0 after it is freed in
                   dssicmpi_free_icmp_cb().
10/13/04    vp     Removed the byte order conversion in dssicmp_ps_input()
                   as source address is passed in network order already.
08/30/04    ks/msr Integrated ICMBs in to ps_mem allocation. Maintaining
                   ICMBs in a queue instead of an array.
09/27/04    vp     Fixed the freeing of INTLOCK in dssicmp_ps_input when the
                   function exits because of errors.
06/11/04    vp     Removal of included files psglobal.h, netuser.h, ps_ip.h
                   inclusion of dssocket_defs.h, ps_in.h. Changes for
                   representation of ip address as struct ps_in_addr
05/24/04    sv     Fixed compilation issue when LOG_MSG_INFO3 is turned on.
04/27/04    sv     Fixed LOG_MSG_INFO3 statement to print v4 address from v4 mapped
                   v6 address.
03/30/04    vp     Merged changes from June04Dev_05.00.00 ps branch.
03/18/04    vp     Changes for removing direct scb access from PS context.
                   All such events are posted through info_change_upcall.
03/10/04    vp     Change for dssocki_socket_event_occurred() to be executed
                   through upcall mechanism.
03/10/04    vp     Added new function dssicmp_register_upcall()
02/03/04    mct    Updated dssicmp_ps_input to print ICMP error code/type.
01/21/04    sv     Changed icmp_socket to take sock_cb and netpolicy
                   instead of appid.
10/21/03    sv     Updated the code to use the acl_policy_info instead of
                   dss_netpolicy_info.
10/17/03    sv     Modified protcol specific functions to take protocol control
                   block as an argument.
10/06/03    sv     Fixed bug in dssicmp_socket. Now scb_ptr is assigned
                   in socket instead of init function.
10/06/03    sv     Added message whenver we are dropping ICMP packets due
                   to memory constraints.
08/21/03    ss     Passed meta_info to icmp_output()
08/18/03    sv     Modified code to use V4mappedV6 addresses.
08/08/03    ss/sv  Restructured the socket code.
07/17/03    sv     Initialized sock_cb, user_data and netpolicy of scb_ptr.
04/15/03    sv     Modified dssicmp_read to accept iov and iovcount instead
                   of buffer and bufferlen as arguments.
11/25/02    usb    Added routing cache for ICMP sockets
10/08/02    aku    Removed FEATURE_DATA_MM featurization.
08/04/02    aku    Merged in changes for multimode ps under FEATURE_DATA_MM
07/14/02    na     Fixed ip_addr_type to support FEATURE_DATA_MM.
05/02/02    mvl    File clean up, added meta_info_ptr to write()
12/18/01    rsl    Updated tracer field.
11/21/01    aku    dssocki_socket_event_occurred() takes additional acb_ptr
                   arg
06/28/01    mvl    Modified msg levels to reduce extra debug chatter
05/10/01    na     Added check to see if scb ptr is valid before
                   dereferencing in dssicmp_ps_input
03/27/01    mt     Changed instances of ps_errno to dss_errno.  Changed second
                   parameter type of dssicmp_read() from byte * to void *.
03/21/01    js     Created module.
===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR THE MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "amssassert.h"
#include "msg.h"
#include "dssocket.h"
#include "dssocket_defs.h"
#include "dssicmp.h"
#include "ps.h"
#include "dsm.h"
#include "ps_icmp.h"
#include "ps_pcb_close_reason_codes.h"
#include "ps_iface.h"
#include "ps_ifacei.h"
#include "ps_in.h"
#include "memory.h"
#include "err.h"
#include "ps_utils.h"
#include "ps_stat_sock.h"
#include "ps_stat_icmpv4.h"
#include "ps_iface_defs.h"
#include "ps_tx_meta_info.h"
#include "ps_mem.h"
#include "ps_metai_info.h"
#include "ds_Sock_Def.h"
#include "ds_Sock_ISocketLocalPriv.h"
#include "PS_Sock_IO_IReader.h"
#include "PS_Sock_IO_IWriter.h"
#include "ps_socket_defs.h"
#include "ps_socket_event_defs.h"
#include "ds_Utils_DebugMsg.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_stat_icmpv6.h"
#include "ps_icmp6.h"
#include "ps_icmp6_msg.h"
#endif


/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION DSSICMPI_CREATE_ICMP_CB()

DESCRIPTION
  This function creates an ICMP control block upon the creation of an ICMP
  socket.

DEPENDENCIES
  None.

RETURN VALUE
  Returns a pointer to the ICMP control block; NULL if an error occured.

SIDE EFFECTS
  None.

===========================================================================*/
static struct icmp_cb *dssicmpi_create_icmp_cb
(
  void *  scb_handle,
  int32   sockfd
)
{
  struct icmp_cb * icmp_cb_ptr = NULL;  /* Ptr to CB to be created         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (scb_handle == NULL )
  {
    DATA_ERR_FATAL("NULL scb_handle");
    return NULL;
  }

  icmp_cb_ptr = (struct icmp_cb *) ps_mem_get_buf(PS_MEM_DSS_ICMB_TYPE);
  if (NULL == icmp_cb_ptr)
  {
    LOG_MSG_ERROR_0("Memory allocation for ICMP CB failed");
    return (NULL);
  }

  memset( icmp_cb_ptr, 0, sizeof( struct icmp_cb));
  icmp_cb_ptr->scb_ptr = scb_handle;
  icmp_cb_ptr->sockfd  = sockfd;

  /*------------------------------------------------------------------------
    Initialize link and add ICMB to queue
  -------------------------------------------------------------------------*/
  (void) q_link(icmp_cb_ptr, &(icmp_cb_ptr->link));
  q_put(&sock_config_cb.icmp_cb_q, &(icmp_cb_ptr->link));

  return icmp_cb_ptr;

} /* dssicmpi_create_icmp_cb() */


/*===========================================================================
FUNCTION DSSICMPI_INIT_ICMP_CB()

DESCRIPTION
  This function initializes the ICMP control block.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void dssicmpi_init_icmp_cb
(
  struct icmp_cb * icmp_cb_ptr                /* Ptr to ICMP control block */
)
{
  ip_init_ipcb(&icmp_cb_ptr->ipcb);

  /*-------------------------------------------------------------------------
    Initialize the receive queue and set it in the socket control block.
  ------------------------------------------------------------------------*/

  return;
}


/*===========================================================================

FUNCTION DSSICMPI_FREE_ICMP_CB()

DESCRIPTION
  This function releases an ICMP control block. It is called when an ICMP
  socket is being closed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void dssicmpi_free_icmp_cb
(
  struct icmp_cb *icmp_cb_ptr     /* pointer to the ICMP cb to be released */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == icmp_cb_ptr)
  {
    DATA_ERR_FATAL("dssicmpi_free_icmp_cb(): NULL icmp_cb_ptr");
    return;
  }

  if (icmp_cb_ptr->ipcb.meta_info_ptr != NULL)
  {
    PS_TX_META_INFO_FREE(&(icmp_cb_ptr->ipcb.meta_info_ptr));
  }

  /*-------------------------------------------------------------------------
    Release sndq, and rcvq
  -------------------------------------------------------------------------*/
  if (icmp_cb_ptr->reader != NULL)
  {
    ReleaseReader(icmp_cb_ptr->reader);
  }

  if (icmp_cb_ptr->writer != NULL)
  {
    ReleaseWriter(icmp_cb_ptr->writer);
  }

  /*-------------------------------------------------------------------------
    Remove ICMB from the queue and make it available for reuse.

    NOTE : ICMP control block must be memset to 0 because application could
    call dss_close() multiple times with same sockfd. By memsetting to 0,
    sockfd's status would become SOCKAVAIL and control block is not freed
    again because dssicmp_close() frees a control block only if sockfd's
    status is not SOCKAVAIL.
  -------------------------------------------------------------------------*/
  q_delete(&sock_config_cb.icmp_cb_q, (q_link_type *) icmp_cb_ptr);

  memset(icmp_cb_ptr, 0, sizeof(struct icmp_cb));
  PS_MEM_FREE(icmp_cb_ptr);

  SOCK_DEC_STATS(open_icmp_socks, 1);

} /* dssicmpi_free_icmp_cb() */


/*===========================================================================

                      GLOBAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSSICMP_SOCKET()

DESCRIPTION
  This function creates an ICMP socket. NOTE: This function is cannot be
  mapped by the protocol specific function pointer. To create an ICMP
  socket, this function must be invoked directly (instead of through
  dss_socket()).

DEPENDENCIES
  None.

PARAMETERS
  uint8  type       -  ICMP socket type
  uint8  code       -  ICMP socket code
  dss_sock_cb_fcn sock_cb - socket callback function
  void * sock_cb_user_data - User data for socket call back
  dss_net_policy_info_type * policy_info_ptr-  Network policy info

  int16 *dss_errno -  error condition value

RETURN VALUE
  Errno Values
  ------------
  DS_EBADAPP          invalid application ID
  DS_EMFILE           too many descriptors open.  A socket is already open or
                      has not closed compeletely.
  DS_EPROTOTYPE       specified protocol invalid for socket type.


SIDE EFFECTS
  None.

===========================================================================*/
int16 dssicmp_int_socket
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
  struct icmp_cb * icmp_cb_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check arguments.
  -------------------------------------------------------------------------*/
  if (errno_ptr == NULL)
  {
    DATA_ERR_FATAL(" dssicmp_int_socket(): errno_ptr is NULL ");
    return DSS_ERROR;
  }

  /* Clear up the incoming errno value, user might not have done so */
  *errno_ptr = DS_ENOERR;

  if (errq_ptr != NULL)
  {
    LOG_MSG_ERROR_0( "dssicmp_int_socket(): errq_ptr is non NULL, "
                     "not supported " );
    *errno_ptr = DS_EOPNOTSUPP;
  }

  if (scb_handle == NULL || reader_ptr == NULL || writer_ptr == NULL ||
      pcb_handle_ptr == NULL)
  {
    LOG_MSG_ERROR_3( "dssicmp_int_socket(): "
                     "Null pointer value scb 0x%x reader 0x%x writer 0x%x",
                     scb_handle, reader_ptr, writer_ptr );
    *errno_ptr = DS_EFAULT;
  }
  
  if((DS_EFAULT==*errno_ptr) || (DS_EOPNOTSUPP==*errno_ptr))
  {
    DATA_ERR_FATAL(" dssicmp_int_socket(): Invalid arg passed or errq_ptr passed is NULL ");
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Create ICMP control block
  -------------------------------------------------------------------------*/
  if ((icmp_cb_ptr = dssicmpi_create_icmp_cb(scb_handle, sockfd)) == NULL )
  {
    *errno_ptr = DS_EMFILE;
    return DSS_ERROR;
  }

  dssicmpi_init_icmp_cb(icmp_cb_ptr);

  icmp_cb_ptr->family = family;
  icmp_cb_ptr->reader = reader_ptr;
  icmp_cb_ptr->writer = writer_ptr;

  SOCK_INC_STATS(icmp_socks_allocated, 1) ;
  SOCK_INC_STATS(open_icmp_socks, 1) ;
  SOCK_UPDATE_MAX_OPEN_STAT(icmp);

  *pcb_handle_ptr = (int32) icmp_cb_ptr;

  return DSS_SUCCESS;
  } /* dssicmp_int_socket() */


/*===========================================================================

FUNCTION DSSICMP_WRITE()

DESCRIPTION
  Send DSM item out to ICMP (write to ICMP socket). The actual write is
  performed in icmp_output() in icmp.c This function is the ICMP
  specific dss_sendto()

DEPENDENCIES
  None.

PARAMETERS
  protocol_ctl_block_type pcb       -   protocol control block
  struct ps_sockaddr_in   *ta_ptr   -   destination socket info
  dsm_item_type           *item_ptr -  ptr to head of dsm memory pool items

RETURN VALUE
  Length of item written.

SIDE EFFECTS
  None.

===========================================================================*/
void dssicmp_write
(
  int32                     pcb_handle,
  int32                     sockfd,
  int32                     pkt_length,
  struct ps_sockaddr_in6  * ta_ptr,
  ps_pkt_meta_info_type   * pkt_meta_info_ptr
)
{
  struct icmp_cb     * icmp_cb_ptr = (struct icmp_cb *) pcb_handle;
  dsm_item_type      * item_ptr;
  struct ps_in6_addr * dest_ip_ptr;              /* Ptr to dest IP address */
  struct icmp          icmp_hdr;
#ifdef FEATURE_DATA_PS_IPV6
  ps_icmp6_msg_type    icmp6_hdr;
  ps_iface_type      * ps_iface_ptr;
#endif
  int16                ps_errno;
  (void) pkt_meta_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == icmp_cb_ptr)
  {
    DATA_ERR_FATAL("NULL icmb");
    return;
  }

  if (icmp_cb_ptr->sockfd != sockfd)
    {
    LOG_MSG_INFO1_2("Sockfd %d != ICMB's sockfd %d",
                    sockfd, icmp_cb_ptr->sockfd);
    return;
  }

  /*-------------------------------------------------------------------------
    Drop the ICMP packet if the iface is NULL. This can occur because there
    are already packets queued to be sent, but the iface down event is
    processed first and clears the routing_cache in the ipcb.
  -------------------------------------------------------------------------*/
  if(icmp_cb_ptr->ipcb.routing_cache == NULL)
  {
    if(icmp_cb_ptr->family == ds_AddrFamily_QDS_AF_INET)
    {
      ICMPV4_INC_STATS(msgs_dropped_tx,1);
    }
#ifdef FEATURE_DATA_PS_IPV6
    else
    {
      ICMPV6_INC_STATS(msgs_dropped_tx, 1);
    }
#endif

    LOG_MSG_ERROR_0("NULL routing_cache/iface has gone down." );
    return;
  }

  if (ta_ptr == NULL)
  {
    /*-----------------------------------------------------------------------
      The destination information missing
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_0("No dest info: ta_ptr is NULL");
    return;
  }
  
  memset(&icmp_hdr, 0, sizeof(struct icmp));
  memset(&icmp6_hdr, 0, sizeof(ps_icmp6_msg_type));

  /*-------------------------------------------------------------------------
    Use destination information from dsm_item chain.
  ------------------------------------------------ ------------------------*/
  dest_ip_ptr = &(ta_ptr->ps_sin6_addr);

  /*-------------------------------------------------------------------------
    Fill in all the known header fields, but not the checksum.
  ------------------------------------------------ ------------------------*/
  if(ps_iface_addr_family_is_v4((icmp_cb_ptr->ipcb.routing_cache)))
  {
    icmp_hdr.info.type = icmp_cb_ptr->type;
    icmp_hdr.info.code = icmp_cb_ptr->code;

    if(icmp_hdr.info.type == (uint8)ICMP_ECHO_REQ)
    {
      icmp_hdr.arg.echo.id = icmp_cb_ptr->hdr_arg.id;
      icmp_hdr.arg.echo.seq = icmp_cb_ptr->hdr_arg.seq;

      /* Increment for next ECHO request */
      ++icmp_cb_ptr->hdr_arg.seq;
    }

    LOG_MSG_INFO3_0("ICMP header successfully assembled");
  }
#ifdef FEATURE_DATA_PS_IPV6
  else
  {
    icmp6_hdr.cmn_hdr.type = icmp_cb_ptr->type;
    icmp6_hdr.cmn_hdr.code = icmp_cb_ptr->code;

    if(icmp6_hdr.cmn_hdr.type == (uint8)ICMP6_ECHO_REQ)
    {
      icmp6_hdr.msg_type.echo_msg.identifier = icmp_cb_ptr->hdr_arg.id;
      icmp6_hdr.msg_type.echo_msg.sequence_num = icmp_cb_ptr->hdr_arg.seq;

      /* Increment for next ECHO request */
      ++icmp_cb_ptr->hdr_arg.seq;
    }
  }
#endif

  /*-------------------------------------------------------------------------
    Collect length information and pass it down to icmp_output().
  ------------------------------------------------ ------------------------*/
  (void) Read(icmp_cb_ptr->reader, &item_ptr, NULL, 0, &ps_errno);

  /*-------------------------------------------------------------------------
    Call icmp_output, which will make up the icmp header and send the
    packet to IP.
  -------------------------------------------------------------------------*/
  if(ps_iface_addr_family_is_v4((icmp_cb_ptr->ipcb.routing_cache)))
  {
    icmp_output( dest_ip_ptr,
                 &icmp_hdr,
                 item_ptr,
                 icmp_cb_ptr,
                 (uint16)pkt_length,
                 NULL ); /* Pass NULL meta info here, so that ip4_send
                           will DUP it from IPCB */
  }
#ifdef FEATURE_DATA_PS_IPV6
  else
  {
    ps_iface_ptr = icmp_cb_ptr->ipcb.routing_cache;
    icmp6_output( dest_ip_ptr,
                  &icmp6_hdr,
                  item_ptr,
                  icmp_cb_ptr,
                  (uint16)pkt_length,
                  ps_iface_ptr);
  }
#endif

  return;
} /* dssicmp_write() */


/*===========================================================================
FUNCTION DSSICMP_CLOSE()

DESCRIPTION
  This function is the ICMP specific dss_close() function.  It simply sets
  the socket state to DSSOCKI_NULL and frees up the socket control block and
  related data structures.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS on success; DSS_ERROR otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
void dssicmp_close
(
  int32    pcb_handle,
  int32    sockfd,
  boolean  silent_close_flag
)
{
  struct icmp_cb            *  icmp_cb_ptr = (struct icmp_cb *) pcb_handle;
  ps_socket_event_info_type    socket_event_info;
  (void)silent_close_flag;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == icmp_cb_ptr)
  {
    DATA_ERR_FATAL("NULL icmb");
    return;
  }

  if (icmp_cb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != ICMB's sockfd %d",
                    sockfd, icmp_cb_ptr->sockfd);
    return;
  }

  /*-------------------------------------------------------------------------
    Post event to socket platform
  -------------------------------------------------------------------------*/
  socket_event_info.scb_ptr                = icmp_cb_ptr->scb_ptr;
  socket_event_info.data.pcb_closed_reason = PS_PCB_CLOSE_REASON_CODE_NORMAL;

  socket_platform_event_cback_f_ptr(PS_SOCKET_PCB_CLOSED_EV,
                                    &socket_event_info);

  /*-------------------------------------------------------------------------
    Free the ICMP control block.
   ------------------------------------------------------------------------*/
  dssicmpi_free_icmp_cb(icmp_cb_ptr);
  return;

} /* dssicmp_close () */


/*===========================================================================
FUNCTION DSSICMP_SETOPT()

DESCRIPTION
  This function sets the ICMP options to the user specified value.

DEPENDENCIES
  None.

PARAMETERS
  tcb_ptr - pointer to tcb

RETURN VALUE
  NONE.

SIDE EFFECTS
  None.
===========================================================================*/
void dssicmp_setopt
(
  int32  pcb_handle,
  int32  sockfd,
  int32  opt_level,
  int32  opt_name,
  int32  opt_val,
  int32  opt_len
)
{
  struct icmp_cb *  icmp_cb_ptr = (struct icmp_cb *) pcb_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == icmp_cb_ptr)
  {
    DATA_ERR_FATAL("NULL icmb");
    return;
  }

  if (icmp_cb_ptr->sockfd != sockfd)
  {
    LOG_MSG_INFO1_2("Sockfd %d != ICMB's sockfd %d",
                    sockfd, icmp_cb_ptr->sockfd);
    return;
  }

  if (opt_level == ds_Sock_OptLevel_QDS_LEVEL_IP ||
      opt_level == ds_Sock_OptLevel_QDS_LEVEL_IPV6)
  {
    ip_setopt(&icmp_cb_ptr->ipcb, opt_level, opt_name, &opt_val, opt_len);
    return;
  }

  if (opt_level != ds_Sock_OptLevel_QDS_LEVEL_ICMP)
  {
    LOG_MSG_INFO1_1("Unknown ICMP socket opt level %d", opt_level);
    return;
  }

  switch(opt_name)
  {
    case ds_Sock_OptNamePriv_QDS_ICMP_ECHO_SEQ_NUM:
      icmp_cb_ptr->hdr_arg.seq = (uint16) opt_val;
      break;

    case ds_Sock_OptNamePriv_QDS_ICMP_ECHO_ID:
      icmp_cb_ptr->hdr_arg.id = (uint16) opt_val;
      break;

    case ds_Sock_OptNamePriv_QDS_ICMP_CODE:
      icmp_cb_ptr->code = (uint8) opt_val;
      break;

    case ds_Sock_OptNamePriv_QDS_ICMP_TYPE:
      icmp_cb_ptr->type = (uint8) opt_val;
      break;

    default:
      LOG_MSG_INFO1_1("Unknown ICMP socket opt %d", opt_name);
      break;
  }
} /* dssicmp_setopt() */


/*===========================================================================
FUNCTION DSSICMP_SET_ROUTE_META_INFO()

DESCRIPTION
  This function processes setting of routing meta info inside IPCB.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssicmp_set_route_meta_info
(
  int32                   pcb_handle,
  int32                   sockfd,
  ps_rt_meta_info_type *  rt_mi_ptr             /* Routing Meta info */
)
{
  struct icmp_cb        *  icmp_cb_ptr = (struct icmp_cb *) pcb_handle;
  ps_tx_meta_info_type  * tx_mi_ptr;          /* Ptr to TX meta info */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (rt_mi_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL rt_mi_ptr");
    return;  
  }

  if (NULL == icmp_cb_ptr)
  {
    PS_RT_META_INFO_FREE(&rt_mi_ptr);
    DATA_ERR_FATAL("NULL ICMB");
    return;
  }

  if (icmp_cb_ptr->sockfd != sockfd)
  {
    PS_RT_META_INFO_FREE(&rt_mi_ptr);
    LOG_MSG_INFO1_2("Sockfd %d != ICMB's sockfd %d",
                    sockfd, icmp_cb_ptr->sockfd);
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
  if (icmp_cb_ptr->ipcb.meta_info_ptr != NULL)
  {
    PS_TX_META_INFO_FREE(&icmp_cb_ptr->ipcb.meta_info_ptr);
  }

  icmp_cb_ptr->ipcb.meta_info_ptr = tx_mi_ptr;
  
  ps_ip_update_routing_cache_in_ipcb
  (
    &(icmp_cb_ptr->ipcb),
    (ps_iface_type *)
      PS_TX_META_GET_ROUTING_CACHE(icmp_cb_ptr->ipcb.meta_info_ptr)
  );

  return;

} /* dssicmp_set_route_meta_info() */


/*===========================================================================
FUNCTION DSSICMP_PS_INPUT()

DESCRIPTION
  When an ICMP message is arrived, if it is NOT ECHO, INFO_RQST or
  TIMESTAMP, the ICMP message will be handled by this fuction in the
  ps context. NOTE: the packet is not released in this should an
  error occured. Instead DSS_ERROR is returned and the packet will
  be freed by icmp_input().

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS on success; DSS_ERROR otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
int16 dssicmp_ps_input
(
  struct icmp           *icmp_hdr,               /* ptr to the icmp header */
  dsm_item_type         *pkt_ptr,          /* ptr to the arriving ICMP pkt */
  ip_pkt_info_type      *ip_pkt_info_ptr      /* IP Packet Info            */
)
{
  struct icmp_cb    *icmp_cb_ptr;
  boolean            success;
  struct ps_in6_addr v6addr;                               /* IPv6 address */
  ps_ip_addr_type    if_addr;
  uint32             length;
  int16              ps_errno;
  ps_pkt_read_info   read_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("Unsupported ICMP packet Type: %d, Code: %d received.",
                  icmp_hdr->info.type, icmp_hdr->info.code);

  /*-------------------------------------------------------------------------
    Traverse the queue of ICMP socket control blocks and find whose type/code
    filed matches that of icmp_hdr.
  -------------------------------------------------------------------------*/
  success = FALSE;
  PS_IN6_GET_V4_MAPPED_V6_FROM_V4(&v6addr,
                               ip_pkt_info_ptr->ip_hdr.v4.source.ps_s_addr);
  

  icmp_cb_ptr = (struct icmp_cb *) q_check(&sock_config_cb.icmp_cb_q);
  while (NULL != icmp_cb_ptr)
  {
    ASSERT(SOCKAVAIL != icmp_cb_ptr->sockfd);

    /*-----------------------------------------------------------------------
      Check for matching type filed. NOTE: !! By default, code filed is
      not filtered.
      We need to compare the id while receiving responses for multiple ping 
      sessions, or else the response would be passed to the first session in 
      queue.
    -----------------------------------------------------------------------*/
    if ( ( icmp_cb_ptr->type == (uint8)ICMP_ECHO_REQ &&
           icmp_hdr->info.type == (uint8)ICMP_ECHO_REPLY &&
           icmp_cb_ptr->hdr_arg.id == ps_ntohs(icmp_hdr->arg.echo.id )) ||           
         icmp_cb_ptr->type == icmp_hdr->info.type )
    {
      if_addr.type = IPV4_ADDR;
      ps_iface_get_addr((ps_iface_type*) ip_pkt_info_ptr->if_ptr, &if_addr);

      /*---------------------------------------------------------------------
        Check if there's still memory in the dsm pool, this will prevent
        the receive queue on the icmp socket from becoming a bottleneck.
      ---------------------------------------------------------------------*/
      if (((DSM_LESS_THAN_FEW_FREE_ITEMS(DSM_DS_SMALL_ITEM_POOL) == TRUE) ||
          ((DSM_LESS_THAN_FEW_FREE_ITEMS(DSM_DS_LARGE_ITEM_POOL) == TRUE) )))
      {
        LOG_MSG_INFO1_0("Low in memory: Discarding the ICMP packet");
        break;
      }

      memscpy(&(read_info.v6RemoteSockAddr.ps_sin6_addr),
             sizeof(read_info.v6RemoteSockAddr.ps_sin6_addr),
             &v6addr,
             sizeof(v6addr));

      /*---------------------------------------------------------------------
        Put the packet on queue.
      ---------------------------------------------------------------------*/
      DSM_TOUCH_ITEM(pkt_ptr);
      length = dsm_length_packet(pkt_ptr);
      read_info.length = length;
      (void) Write(icmp_cb_ptr->writer,
                   &pkt_ptr,
                   length,
                   (char *)&read_info,
                   sizeof(read_info),
                   &ps_errno);

      success = TRUE;
      break;
    } /* if (matched type) */

    icmp_cb_ptr
      = (struct icmp_cb *) q_next(&sock_config_cb.icmp_cb_q,
                              (q_link_type *) icmp_cb_ptr);
  } /* while (ICMP sockets in queue) */
  

  /*-------------------------------------------------------------------------
    Process return value.
  -------------------------------------------------------------------------*/
  if (success == 1)
  {
    return (DSS_SUCCESS);
  }
  else
  {
    return (DSS_ERROR);
  }
}/* dssicmp_ps_input () */


#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION DSSICMP6_PS_INPUT()

DESCRIPTION
  When an ICMP6 message arrives, if it is NOT ECHO, INFO_RQST or
  TIMESTAMP, the ICMP message will be handled by this fuction in the
  ps context. NOTE: the packet is not released in this should an
  error occured. Instead DSS_ERROR is returned and the packet will
  be freed by icmp_input().

DEPENDENCIES
  None.

RETURN VALUE
  DSS_SUCCESS on success; DSS_ERROR otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
int16 dssicmp6_ps_input
(
  ps_icmp6_msg_type     *icmp6_hdr_ptr,    /* ptr to the icmp header */
  dsm_item_type         *pkt_ptr,          /* ptr to the arriving ICMP pkt */
  ip_pkt_info_type      *ip_pkt_info_ptr   /* IP Packet Info            */
)
{
  struct icmp_cb    *icmp_cb_ptr;
  boolean            success;
  struct ps_in6_addr v6addr;                               /* IPv6 address */
  ps_ip_addr_type    if_addr;
  int32              length;
  int16              ps_errno;
  ps_pkt_read_info   read_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("Unsupported ICMPv6 packet Type: %d, Code: %d received.",
                  icmp6_hdr_ptr->cmn_hdr.type, icmp6_hdr_ptr->cmn_hdr.code);

  /*-------------------------------------------------------------------------
    Traverse the queue of ICMP socket control blocks and find whose type/code
    filed matches that of icmp6_hdr_ptr.
  -------------------------------------------------------------------------*/
  success = FALSE;
  v6addr = ip_pkt_info_ptr->ip_hdr.v6.hdr_body.base_hdr.src_addr;
  

  icmp_cb_ptr = (struct icmp_cb *) q_check(&sock_config_cb.icmp_cb_q);
  while (NULL != icmp_cb_ptr)
  {
    ASSERT(SOCKAVAIL != icmp_cb_ptr->sockfd);

    /*-----------------------------------------------------------------------
      Check for matching type filed. NOTE: !! By default, code filed is
      not filtered.
      We need to compare the id while receiving responses for multiple ping 
      sessions, or else the response would be passed to the first session in 
      queue.      
    -----------------------------------------------------------------------*/
    if ( ( icmp_cb_ptr->type == (uint8)ICMP6_ECHO_REQ &&
           icmp6_hdr_ptr->cmn_hdr.type == (uint8)ICMP6_ECHO_RSP &&
           icmp_cb_ptr->hdr_arg.id == 
                    icmp6_hdr_ptr->msg_type.echo_msg.identifier ) ||           
         icmp_cb_ptr->type == icmp6_hdr_ptr->cmn_hdr.type )
    {
      if_addr.type = IPV6_ADDR;
      ps_iface_get_addr((ps_iface_type *) ip_pkt_info_ptr->if_ptr, &if_addr);

      /*---------------------------------------------------------------------
        Check if there's still memory in the dsm pool, this will prevent
        the receive queue on the icmp socket from becoming a bottleneck.
      ---------------------------------------------------------------------*/
      if (((DSM_LESS_THAN_FEW_FREE_ITEMS(DSM_DS_SMALL_ITEM_POOL) == TRUE) ||
          ((DSM_LESS_THAN_FEW_FREE_ITEMS(DSM_DS_LARGE_ITEM_POOL) == TRUE) )))
      {
        LOG_MSG_INFO1_0("Low in memory: Discarding the ICMPv6 packet");
        break;
      }

      memscpy(&(read_info.v6RemoteSockAddr.ps_sin6_addr),
             sizeof(read_info.v6RemoteSockAddr.ps_sin6_addr),
             &v6addr,
             sizeof(v6addr));

      /*---------------------------------------------------------------------
        Put the packet on queue.
      ---------------------------------------------------------------------*/
      DSM_TOUCH_ITEM(pkt_ptr);
      length = dsm_length_packet(pkt_ptr);
      read_info.length = length;
      (void) Write(icmp_cb_ptr->writer,
                   &pkt_ptr,
                   length,
                   (char *)&read_info,
                   sizeof(read_info),
                   &ps_errno);

      success = TRUE;
      break;
    } /* if (matched type) */

    icmp_cb_ptr
      = (struct icmp_cb *) q_next(&sock_config_cb.icmp_cb_q,
                              (q_link_type *) icmp_cb_ptr);
  } /* while (ICMP sockets in queue) */
  

  /*-------------------------------------------------------------------------
    Process return value.
  -------------------------------------------------------------------------*/
  if (success == 1)
  {
    return (DSS_SUCCESS);
  }
  else
  {
    return (DSS_ERROR);
  }
}/* dssicmp6_ps_input () */
#endif /* FEATURE_DATA_PS_IPV6 */



