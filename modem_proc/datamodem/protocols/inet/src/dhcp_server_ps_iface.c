/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              D H C P _ S E R V E R _ P S _ I F A C E . C

GENERAL DESCRIPTION
  DMSS Dynamic Host Configuration Protocol server shim.  This file
  contains the shim between the PS IFACE on which requests will be
  received from and sent to the client, and the core parser.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  - dhcp_server_new() is called to allocate a new server object and set
    the object up internally.
  - dhcp_server_start() is called to glue the server into the other
    objects that make up the DHCP server.  This will register the
    filter with the PS stack that will cause incoming messages.
  - dhcp_server_stop() is called to close down this module and free
    the handle.

Copyright (c) 2004-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dhcp_server_ps_iface.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/10/14    vl     Optimization of F3 messages
11/01/13    rk     Removed feature mobileap.
07/08/13    cx     Replace memcpy and memmove with memscpy and memsmove
12/12/12    ds     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR.
01/10/11    cp     Featurisation changes.
07/02/10    cp     Calling DHCP ARP Cache IOTCTLS for updating and clearing
                   ARP cache entries.
06/02/10    cp     IOCTL changes for Soft AP DHCP server.
02/17/10    ss     Using dsm_queue_destroy() to free the DSM watermark when it
                   is no longer needed.
01/11/10    ss     Lint warning fixes.
05/22/09    kk     Q6 compiler warning fixes.
12/16/08    am     Dont assert in sock_cb for sockfd mimatches. This happens
                   if DHCP cmds are non-serialized by PS.
10/23/08    am     Dont ASSERT if cookie_ptr check fails in server_stop().
10/22/08    am     Changes to ARP IP cache update if dst IP addr is bcast.
03/10/08    am     Removed ASSERT for error in dhcp_server_reply_send().
09/26/08    am     Changed malloc/free names for off target support.
04/02/08    am     High Lint fixes.
02/07/07    scb    Fixed Klocwork High Errors
11/04/06    rt     Changed the sequence of SYS SOCK setting during DHCP start
09/18/06    hm     Changed usage of DSSOCKI_IFACE_GET_ID to PS_IFACE_GET_ID
08/24/06    rt     Modified dhcpi_server_sock_cb to post a command and
                   do the processing as part of event.
04/25/06    msr    L4/Tasklock code review changes
11/29/05    jd     Free received DHCP packets after processing
04/17/05    msr    Changed to reflect new dssocki_reg_ip_filter signature.
01/18/05    clp    DHCP server receives via socket2 interface rather than
                   directly from IP filtering.
04/15/04    clp    Initial development work done.  Added comments.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "dssocket.h"
#include "dssinternal.h"
#include "dhcp.h"
#include "dhcpi.h"
#include "ps_iface.h"
#include "ps_iface_ipfltr.h"
#include "ps_in.h"
#include "ps_udp_io.h"

#include <string.h>
#include "err.h"
#include "msg.h"
#include "ps_arp.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* The udp ports to receive on and send to. */
#define DHCP_SERVER_PORT (dss_htons(67))
#define DHCP_CLIENT_PORT (dss_htons(68))

static uint32 dhcp_static_fltr_handle;

/* The server handle structure */
typedef struct dhcp_server_info {
  dhcp_server_init            init;
  dhcp_server_config          config;

  /* The socket handle for the socket we are communicating over. */
  sint15 sock;

  /* This indicates if we are currently trying to close the socket. */
  boolean closing;

  /* This watermark holds any items that can't be written out right
   * away.  The items will be dequeued and sent when the write event
   * callback is called.
   */
  dsm_watermark_type write_wm;

  /* The size of the watermark DNE, apperently in bytes */
#define DHCP_SERVER_SERVER_WM_SZ (2000)

  /* This is the queue head needed by the above watermark */
  q_type write_wm_q;


  /* cookies: The cookie_ptr takes a value of a pointer to the
   * beginning of this structure. This happens when the core is
   * started.
   */
  struct dhcp_server_info   * cookie_ptr;
  /* The cookie takes the value below when allocated in _new */
#define DHCP_SERVER_COOKIE (0x5EC00C1E)
  uint32                      cookie;

  /* sock_event ptr -> sock_cb() takes care of allocating
   * memory, and sock_event_cmd_hdlr() of freeing it
   */
  void *sock_event_ptr;
} dhcp_server_info;

/* Command structure to send socket event info to PS task */
typedef struct {
  dhcp_server_info * info;
  uint32             event_mask;
  int16              sockfd;
} dhcp_server_sock_event;

/* Local function declaration.*/
static void
dhcpi_server_sock_event_cmd
(
   ps_cmd_enum_type  cmd,
   void            * user_data_ptr
);


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
STATIC FUNCTION DHCPI_SERVER_REPLY_EXTRACT

Description:
  This function extracts data from a DSM item using dsm_extract.  This
  is called by the core to get the current incoming message.

Arguements:
  void * dest - Where to put the data.
  void * src - The dsm item to extract the data from.
  uint32 offset - Where to start extracting data from.
  uint32 len - How much data to extract.

Return value:
  boolean - TRUE if extracted successful. FALSE if failed to extract.

Dependencies:
  None.
===========================================================================*/
static boolean
dhcpi_server_reply_extract
(
  void * dest,
  void * src,
  uint32 offset,
  uint32 len
)
{
  ASSERT(NULL != dest);
  ASSERT(NULL != src);
  return (len == dsm_extract(src, (uint16)offset, dest, (uint16)len)) ?
    TRUE : FALSE;
} /* dhcpi_server_reply_extract */
/*===========================================================================
STATIC FUNCTION DHCPI_SERVER_SOCK_CB

Description:
  This function gets called by the sockets layer when there is an
  event on the socket.  We handle each of the possible events that can
  come in.  Note: we can get more than one event in the callback.

Arguements:
  int16 dss_nethandle - The app id (if any) associated with the socket
  int16 sockfd - The socket descriptor
  uint32 event_mask - The event(s) that occured on the socket
  void * handle - A handle to our info.

Return value:
  void

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_server_sock_cb
(
   int16 dss_nethandle,
   int16 sockfd,
   uint32 event_mask,
   void * handle
)
{
  dhcp_server_info        * info = handle;
  dhcp_server_sock_event  * event;

  /* Verify sanity */
  ASSERT(NULL != handle);
  ASSERT(NULL != info);
  if ((info->cookie_ptr != info) || (DHCP_SERVER_COOKIE != info->cookie))
  {
    LOG_MSG_ERROR_2( "dhcpi_server_sock_cb: cookie 0x%x, cookie_ptr 0x%p check failed",
                     info->cookie, info->cookie_ptr );
    return;
  }

  if (info->sock != sockfd)
  {
    LOG_MSG_ERROR_2( "dhcpi_server_sock_cb: Bad sockfd %d passed, expected %d",
                     sockfd, info->sock );
    return;
  }

  info->sock_event_ptr = event = (dhcp_server_sock_event *)
    info->init.d_malloc(sizeof(dhcp_server_sock_event ));
  if ( NULL == event )
  {
    DATA_ERR_FATAL("dhcpi_server_sock_cb unable to get parameter memory");
    return;
  }

  event->info       = info;
  event->sockfd     = sockfd;
  event->event_mask = event_mask;

 (void) ps_set_cmd_handler( PS_DHCP_SERVER_SOCK_EVENT_CMD,
                             dhcpi_server_sock_event_cmd );
  ps_send_cmd( PS_DHCP_SERVER_SOCK_EVENT_CMD, event );
} /* dhcpi_server_sock_cb */

/*===========================================================================
STATIC FUNCTION DHCPI_SERVER_SOCK_EVENT_CMD

Description:
  This function gets called by the sockets layer when there is an
  event on the socket.  We handle each of the possible events that can
  come in.  Note: we can get more than one event in the callback.

Arguements:
  ps_cmd_enum_type - the command type (PS_DHCP_SERVER_SOCK_EVENT_CMD)
  user_data_ptr - the user data with the event info.

Return value:
  void

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_server_sock_event_cmd
(
   ps_cmd_enum_type cmd,
   void * user_data_ptr
)
{
  dhcp_server_sock_event  * event = (dhcp_server_sock_event*)user_data_ptr;
  dhcp_server_info * info ;
  sint31 dss_ret;
  dsm_item_type * dss_packet = NULL;
  struct ps_sockaddr_in dss_addr;
  uint16 dss_addr_len = sizeof( struct ps_sockaddr_in );
  int16 dss_errno;
  int16 sockfd;
  uint32 event_mask;

  if ( PS_DHCP_SERVER_SOCK_EVENT_CMD != cmd )
  {
    LOG_MSG_ERROR_2( "Didn't get sock event cmd %d %d",
                     cmd, PS_DHCP_SERVER_SOCK_EVENT_CMD );
    ASSERT(0);
    return;
  }

  if ( NULL == event )
  {
    DATA_ERR_FATAL("Got NULL cmd in sock event cmd");
    return;
  }

  info       = event->info;
  sockfd     = event->sockfd;
  event_mask = event->event_mask;

  if ( NULL == info )
  {
    DATA_ERR_FATAL("Got NULL info in sock callback");
    return;
  }

  if ( info->cookie_ptr != info )
  {
    LOG_MSG_ERROR_2( "Cookie pointer failed in sock callback %x %x",
                     info->cookie_ptr, info );
    return;
  }
  if ( DHCP_SERVER_COOKIE != info->cookie )
  {
    LOG_MSG_ERROR_2( "Cookie mismatch in sock callback %x %x",
                     info->cookie, DHCP_SERVER_COOKIE );
    return;
  }
  if ( info->sock != sockfd )
  {
    LOG_MSG_ERROR_2("Got callback for unknown socket %d %d",
                    info->sock, sockfd);
    return;
  }

  info->init.d_free(event);

  /* Branch on the event mask, there can be multiple events, so we
   * can't switch on the event
   */
  if ( DS_READ_EVENT & event_mask )
  {
    while ( 0 < dss_recvfrom_dsm_chain( info->sock,
                                        &dss_packet,
                                        0,
                                        (struct ps_sockaddr*)&dss_addr,
                                        &dss_addr_len,
                                        &dss_errno ) )
    {
      info->config.recv_msg( info->config.recv_msg_handle,
                             dss_packet,
                             dhcpi_server_reply_extract);
      dsm_free_packet( &dss_packet );
      dss_addr_len = sizeof( struct ps_sockaddr_in );
    }
    dsm_free_packet( &dss_packet );

    if (DS_EWOULDBLOCK == dss_errno)
    {
      dss_ret = dss_async_select( info->sock, DS_READ_EVENT, &dss_errno );
      if ( DSS_SUCCESS != dss_ret )
      {
        LOG_MSG_ERROR_2( "Failed async select READ on %x %d",
                         info->sock, dss_errno );
        ASSERT(0);
      }
    }

  }

  if ( DS_ACCEPT_EVENT & event_mask )
  {
    LOG_MSG_ERROR_2( "Got ACCEPT event on client socket %x %x",
                     sockfd, event_mask );
    ASSERT(0);
  }

  if ( DS_WRITE_EVENT & event_mask )
  {
    while ( NULL != (dss_packet = dsm_dequeue(&info->write_wm)) )
    {
      /* Grabbing the IP address that was added to the packet when
       * enqueued
       */
      if ( sizeof(uint32) != dsm_pullup(&dss_packet,
                                        &dss_addr.ps_sin_addr.ps_s_addr,
                                        sizeof(uint32)) )
      {
        DATA_ERR_FATAL("Unable to pullup addr from packet");
        dsm_free_packet( &dss_packet );
        continue;
      }
      dss_addr.ps_sin_family = DSS_AF_INET;
      dss_addr.ps_sin_port = DHCP_CLIENT_PORT; /**/
      /**/
      dss_ret = dss_sendto_dsm_chain( info->sock, &dss_packet, 0,
                                      (struct ps_sockaddr*)&dss_addr,
                                      sizeof(struct ps_sockaddr_in),
                                      &dss_errno );
      /**/

      if ( DSS_ERROR == dss_ret && DS_EWOULDBLOCK == dss_errno )
      {
        /* Put the IP address back into the packet */
        if ( sizeof(uint32) != dsm_pushdown_packed(&dss_packet,
                                                   &dss_addr.ps_sin_addr.ps_s_addr,
                                                   sizeof(uint32),
                                                   DSM_DS_SMALL_ITEM_POOL) )
        {
          LOG_MSG_ERROR_0( "Pushdown packed unable to push down addr" );
          dsm_free_packet( &dss_packet );
        }
        else
        {
          dsm_enqueue( &info->write_wm, &dss_packet );
          dss_ret = dss_async_select( info->sock, DS_WRITE_EVENT,
                                      &dss_errno );
          if ( DSS_SUCCESS != dss_ret )
          {
            LOG_MSG_ERROR_2( "Failed async select WRITE on %x %d",
                             info->sock, dss_errno );
            ASSERT(0);
            while ( NULL != (dss_packet = dsm_dequeue(&info->write_wm)) )
            {
              dsm_free_packet( &dss_packet );
            }
          }
        }
        /* Now that we have put the message back on the queue (or
         * not), we hop out here to avoid an infinite loop
         */
        break;
      }
      else if ( DSS_ERROR == dss_ret )
      {
        LOG_MSG_ERROR_3("sendto returned %d %d %d",
                        dss_ret, dss_errno, info->sock);
        dsm_free_packet( &dss_packet );
        /* In this case, we have a non- EWOULDBLOCK error, try again
         * with the next packet, not that we expect a better result or
         * anything
         */
      }
    }
  }

  if ( TRUE == info->closing && DS_CLOSE_EVENT & event_mask )
  {
    dss_ret = dss_close( info->sock, &dss_errno );
    if (  DSS_SUCCESS != dss_ret )
    {
      LOG_MSG_ERROR_2( "Failed close on %x %d", info->sock, dss_errno );
      ASSERT(0);
    }
    info->init.d_free(info);
  }
}/*dhcpi_server_sock_event_cmd*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
//MSR TODO Add doxygen documentation in header file for below 2 functions
void dhcp_server_ipfltr_init
(
  void
)
{
  ip_filter_type                   filter;
  ps_iface_ipfltr_add_param_type   fltr_param;
  int16                            ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Set up UL STATIC filter on RmNet group of ifaces to filter out
    IPv4 packets destined to DHCP server port
  -------------------------------------------------------------------------*/
  memset( &filter, 0, sizeof(filter) );
  memset( &fltr_param, 0, sizeof(ps_iface_ipfltr_add_param_type));

  filter.ip_vsn                       = IP_V4;
  filter.ip_hdr.v4.field_mask         = IPFLTR_MASK_IP4_NEXT_HDR_PROT;
  filter.ip_hdr.v4.next_hdr_prot      = PS_IPPROTO_UDP;
  filter.next_prot_hdr.udp.field_mask = IPFLTR_MASK_UDP_DST_PORT;
  filter.next_prot_hdr.udp.dst.port   = DHCP_SERVER_PORT;

  fltr_param.enable             = TRUE;
  fltr_param.fi_result          = PS_IFACE_IPFLTR_RESULT_UL_STATIC_LOCAL;
  fltr_param.fi_ptr_arr         = &filter;
  fltr_param.num_filters        = 1;
  fltr_param.filter_type        = IPFLTR_DEFAULT_TYPE;
  fltr_param.is_validated       = FALSE;
  fltr_param.fltr_priority      = PS_IFACE_IPFLTR_PRIORITY_DEFAULT;
  fltr_param.fltr_compare_f_ptr = NULL;

  dhcp_static_fltr_handle =
    ps_iface_ipfltr_add_by_group(RM_GROUP,
                                 IP_FLTR_CLIENT_SOCKETS,
                                 &fltr_param,
                                 &ps_errno);

  if (PS_IFACE_IPFLTR_INVALID_HANDLE == dhcp_static_fltr_handle)
  {
    LOG_MSG_ERROR_1("dhcp_server_ipfltr_init(): Failed to reg fltr, err %d",
                    ps_errno);
    ASSERT(0);
  }

  return;
} /* dhcp_server_ipfltr_init() */

void dhcp_server_ipfltr_deinit
(
  void
)
{
  int32  ret_val;
  int16  ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ret_val = ps_iface_ipfltr_delete(NULL,
                                   IP_FLTR_CLIENT_SOCKETS,
                                   dhcp_static_fltr_handle,
                                   &ps_errno);
  if (-1 == ret_val)
  {
    LOG_MSG_ERROR_1("dhcp_server_ipfltr_deinit(): "
                    "Failed to de-reg fltr, err %d", ps_errno);
  }

  return;
} /* dhcp_server_ipfltr_deinit() */

/*===========================================================================
FUNCTION DHCP_SERVER_NEW()

DESCRIPTION
  This function allocates and intializes the server shim.

DEPENDENCIES
  Info in init must be valid.

PARAMETERS
  dhcp_server_init * - pointer to functions used to init the server system.

RETURN VALUE
  Handle for server functions.

SIDE EFFECTS
  Allocates a server shim handle.
===========================================================================*/
void *
dhcp_server_new
(
  dhcp_server_init * init
)
{
  dhcp_server_info * server;

  LOG_MSG_INFO3_0( "INFO: --- DHCP SERVER INIT ---" );
  /* sanity check info */
  ASSERT(NULL != init);
  ASSERT(NULL != init->d_malloc);
  ASSERT(NULL != init->d_free);
  /* Get some memory for our info */
  server = init->d_malloc(sizeof(dhcp_server_info));
  if ( NULL == server )
  {
    DATA_ERR_FATAL("FAIL: Insufficent memory to init DHCP server");
    return NULL;
  }
  /* initialize info from config */
  memset( server, 0, sizeof(dhcp_server_info) );
  server->init = *init;
  server->cookie = DHCP_SERVER_COOKIE;
  server->sock = 0;
  server->closing = FALSE;
  server->sock_event_ptr = NULL;

  return server;
} /* dhcp_server_new */


/*===========================================================================
FUNCTION DHCP_SERVER_START()

DESCRIPTION
  This function registers the functions that interface the server shim
  with the DHCP Core.  This function also configures server to
  receieve DHCP messages.  This function registers the filter with the
  PS layer to receive DHCP messages.

DEPENDENCIES
  Handle must have been allocated by dhcp_server_new() above.
  Config must contain valid information.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  dhcp_server_config * - pointer to functions used to connect to other
                         systems.

RETURN VALUE
  boolean - TRUE if successful, FALSE if not.

SIDE EFFECTS
  Registers the filter with the PS to receive DHCP messages.
===========================================================================*/
boolean
dhcp_server_start
(
  void               * handle,
  dhcp_server_config * config
)
{
  dhcp_server_info   * server = handle;
  ps_ip_addr_type      ip_addr;
  sint31 dss_ret;
  dss_net_policy_info_type policy;
  sint15 dss_errno;
  boolean ret = TRUE;
  int val = TRUE;  /* This type is used internally by dss_setsockopt */
  uint32 val_len = sizeof( int );
  struct ps_sockaddr_in dss_sockaddr;

  LOG_MSG_INFO3_0( "INFO: --- DHCP SERVER START ---" );

  /* Sanity check info */
  ASSERT(NULL != handle);
  ASSERT(NULL != config);
  ASSERT(DHCP_SERVER_COOKIE == server->cookie);
  ASSERT(NULL != config->addr_info_server_id);

  /* Update the config info */
  server->config = *config;

  /* Initialize the write watermark */
  dsm_queue_init( &server->write_wm,
                  DHCP_SERVER_SERVER_WM_SZ,
                  &server->write_wm_q );

  /* Init the policy */
  memset( &policy, 0, sizeof(dss_net_policy_info_type) );
  dss_init_net_policy_info(&policy);

  /* Set up the socket. */
  policy.policy_flag = DSS_IFACE_POLICY_SPECIFIED;


  policy.iface.kind = DSS_IFACE_ID;
  policy.iface.info.id = PS_IFACE_GET_ID(server->init.iface);
  policy.family = DSS_AF_INET;
  policy.is_routeable = TRUE;
  server->init.iface_id = policy.iface.info.id;

  dss_ret = dss_socket2( DSS_AF_INET, DSS_SOCK_DGRAM, (uint8)PS_IPPROTO_UDP,
                      dhcpi_server_sock_cb, handle,
                      &policy, &dss_errno );
  if ( DSS_ERROR != dss_ret )
  {
    server->sock = (sint15)dss_ret;

    dss_ret = dss_setsockopt( server->sock, (int32)DSS_SOL_SOCKET, 
                              (int32)DSS_SO_SYS_SOCK, &val, &val_len, 
                              &dss_errno );
    if ( DSS_SUCCESS != dss_ret )
    {
      LOG_MSG_ERROR_2( "Unable to set socket to system %d %d",
                       dss_ret, dss_errno );
      ASSERT(0);
      ret = FALSE;
    }

    dss_sockaddr.ps_sin_family = DSS_AF_INET;
    dss_sockaddr.ps_sin_port = DHCP_SERVER_PORT; /**/
    dss_sockaddr.ps_sin_addr.ps_s_addr = 0;
    /**/
    dss_ret = dss_bind( server->sock,
                        (struct ps_sockaddr*)&dss_sockaddr,
                        sizeof(struct ps_sockaddr_in),
                        &dss_errno );
    /**/
    if ( DSS_SUCCESS != dss_ret )
    {
      LOG_MSG_ERROR_1( "Unable to bind to DHCP client port %d", dss_errno);
      ASSERT(0);
      ret = FALSE;
    }
    dss_ret = dss_async_select( server->sock, DS_READ_EVENT, &dss_errno );
    if ( DSS_SUCCESS != dss_ret )
    {
      LOG_MSG_ERROR_2( "Failed async select READ on %x %d", server->sock, dss_errno);
      ASSERT(0);
      ret = FALSE;
    }
  }
  else
  {
    LOG_MSG_ERROR_2( "Unable to open socket %d %d", dss_ret, dss_errno );
    ASSERT(0);
    ret = FALSE;
  }

  /* we need to set the server IP address. */
  ip_addr.type = IPV4_ADDR;
  ip_addr.addr.v4.ps_s_addr =
    config->addr_info_server_id( config->addr_info_handle );
  if ( ps_iface_set_v4_addr( server->init.iface, &ip_addr ) )
  {
    DATA_ERR_FATAL("DHCP Server failed to set server IP");
    ret = FALSE;
  }
  if ( TRUE == ret )
  {
    server->cookie_ptr = server;
  }
  return ret;
} /* dhcp_server_start */
/*===========================================================================
FUNCTION DHCP_SERVER_STOP()

DESCRIPTION
  This function unregisters the functions that interface the server shim.
  This function also cleans up any connections with the PS including clearing
  any filters that are in place.

DEPENDENCIES
  Handle must be valid.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.

RETURN VALUE
  None.

SIDE EFFECTS
  Unregisters filters associated with receiving messages on the server
  interface associated with handle.
  Frees handle.
===========================================================================*/
void
dhcp_server_stop
(
  void ** handle
)
{
  dhcp_server_info * server = NULL;
  sint31 dss_ret;
  sint15 dss_errno;

  if(NULL == handle || NULL == (server = *handle))
  {
    DATA_ERR_FATAL("dhcp_server_stop(): handle or *handle ptr is NULL");
    return;
  }

  ASSERT(DHCP_SERVER_COOKIE == server->cookie);

  /* Dont ASSERT */
  if(server != server->cookie_ptr)
  {
    LOG_MSG_INFO1_0("server_stop() called before  server_start()");
    server->init.d_free(server);
    *handle = NULL;
    return;
  }

  /* Free anything left on the write WM and destroy the watermark */
  dsm_queue_destroy(&server->write_wm);

  /* Close the socket, if we can */
  dss_ret = dss_close( server->sock, &dss_errno );
  if ( DSS_ERROR == dss_ret && DS_EWOULDBLOCK == dss_errno )
  {
    server->closing = TRUE;
    /* In this case, we free info when the close event comes in */
    dss_ret = dss_async_select( server->sock, DS_CLOSE_EVENT, &dss_errno );
    if ( DSS_SUCCESS != dss_ret )
    {
      LOG_MSG_ERROR_2( "Failed async select CLOSE on %x %d",
                       server->sock, dss_errno );
      server->cookie = 0;
      server->cookie_ptr = NULL;
      server->sock = 0;
      server->init.d_free(server);
      ASSERT(0);
    }
  }
  else if ( DSS_ERROR == dss_ret )
  {
    LOG_MSG_ERROR_2( "Error closing socket %x %d", server->sock, dss_errno );
    server->cookie = 0;
    server->cookie_ptr = NULL;
    server->sock = 0;
    server->init.d_free(server);
    ASSERT(0);
  }
  else
  {
    server->cookie = 0;
    server->cookie_ptr = NULL;
    server->sock = 0;
    server->init.d_free(server);
  }
  LOG_MSG_INFO3_0( "INFO: --- DHCP SERVER STOP ---" );
  *handle = NULL;
} /* dhcp_server_stop */
/*===========================================================================
FUNCTION DHCP_SERVER_REPLY_NEW()

DESCRIPTION
  This function "allocates" as needed a message for the reply.
  In this particular case, we are using dsm, and we don't need to
  prealloc anything.

DEPENDENCIES
  Handle must have been started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  void ** msg - The message to allocate.
  uint32 len - The length of the message.

RETURN VALUE
  boolean - TRUE if successful, FALSE if not.

SIDE EFFECTS
  Updates the server handle.
===========================================================================*/
boolean
dhcp_server_reply_new
(
  void * handle,
  void ** msg,
  uint32 len
)
{
  dhcp_server_info * server = handle;
  (void)len;

  if( NULL == msg )
  {
    DATA_ERR_FATAL("NULL msg ptr");
    return FALSE;
  }

  if( NULL == server )
  {
    DATA_ERR_FATAL("NULL handle passed");
    return FALSE;
  }

  if( DHCP_SERVER_COOKIE != server->cookie ||
      server != server->cookie_ptr )
  {
    LOG_MSG_ERROR_2("Inv server 0x%p or cookie 0x%x", server, server->cookie);
    ASSERT(0);
    return FALSE;
  }

  *msg = NULL;
  return TRUE;
} /* dhcp_server_reply_new */
/*===========================================================================
FUNCTION DHCP_SERVER_REPLY_PUSHDDOWN_TAIL()

DESCRIPTION
  This fucntion appends data of len to msg.

DEPENDENCIES
  Handle must have been started.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  void ** msg - The message to allocate.
  void * data - data to append.
  uint32 len - The length to append.

RETURN VALUE
  boolean - TRUE if successful, FALSE if not.

SIDE EFFECTS
  Adds data to msg.  Updates length of current message in handle.
===========================================================================*/
boolean
dhcp_server_reply_pushdown_tail
(
  void * handle,
  void ** msg,
  void * data,
  uint32 len
)
{
  dhcp_server_info * server = handle;

  if( NULL == server || NULL == msg )
  {
    LOG_MSG_ERROR_2("NULL server 0x%p or msg 0x%p", server, msg);
    ASSERT(0);
    return FALSE;
  }

  if( DHCP_SERVER_COOKIE != server->cookie ||
      server != server->cookie_ptr )
  {
    LOG_MSG_ERROR_2("Invalid server 0x%p or cookie 0x%x",
                    server, server->cookie);
    ASSERT(0);
    return FALSE;
  }

  return dsm_pushdown_tail( (dsm_item_type **)msg, data, (uint16)len,
                       DSM_DS_SMALL_ITEM_POOL ) < len ?  FALSE : TRUE;
} /* dhcp_server_reply_pushdown_tail */
/*===========================================================================
FUNCTION DHCP_SERVER_REPLY_FREE()

DESCRIPTION
  Frees msg in the case that something went wrong.

DEPENDENCIES
  None.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  void ** msg - The message to free.

RETURN VALUE
  None.

SIDE EFFECTS
  Frees message.
===========================================================================*/
void
dhcp_server_reply_free
(
  void * handle,
  void ** msg
)
{
  (void)handle;
  ASSERT(NULL != msg);
  dsm_free_packet( (dsm_item_type **)msg );
} /* dhcp_server_reply_free */

/*===========================================================================
FUNCTION DHCP_SERVER_REPLY_SEND()

DESCRIPTION
  This function takes a reply and sends it to the server interface.

DEPENDENCIES
  IP address must be an IPV4 address.
  The handle must be valid.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  void ** msg - The message to process.
  uint32 - address to send reply to.
  uint8 * - hw address (for arp).
  uint32 - length of hw address.
  uint32 - address to be updated in ARP cache if dest addr is bcast.

RETURN VALUE
  None.

SIDE EFFECTS
  Injects the message into the middle of the UDP stack.
===========================================================================*/
void
dhcp_server_reply_send
(
  void * handle,
  void ** msg,
  uint32 addr,
  uint8 * hwaddr,
  uint32 hwlen,
  uint32 arp_ip_addr
)
{
  dhcp_server_info * server = handle;
  struct ps_sockaddr_in dss_addr;
  sint15 dss_errno;
  sint31 dss_ret;
  dsm_item_type ** dss_packet_ptr = (dsm_item_type **) msg;

  (void)hwlen;
  ASSERT(NULL != server);
  ASSERT(DHCP_SERVER_COOKIE == server->cookie);
  ASSERT(server == server->cookie_ptr);
  ASSERT(NULL != msg);
  ASSERT(NULL != hwaddr);

  /* The "ARP" layer needs to know not to arp for this address, if it
   * is not a broadcast addr
   */
  if(server->config.arp_cache_update_cb != NULL)
  {
    if ( (uint32)0xffffffff != addr )
    {
      LOG_MSG_INFO2_1 ("Client IP 0x%x adding to ARP cache", addr);
      server->config.arp_cache_update_cb(server->init.iface, TRUE,
                                         addr, hwaddr);
    } 
    else if(arp_ip_addr != 0)
    {
      LOG_MSG_INFO2_1 ("ARP IP 0x%x adding to ARP cache", arp_ip_addr);
      server->config.arp_cache_update_cb(server->init.iface, TRUE,
                                         arp_ip_addr, hwaddr);
    }
  }

  dss_addr.ps_sin_family = DSS_AF_INET;
  dss_addr.ps_sin_port = DHCP_CLIENT_PORT; /**/
  dss_addr.ps_sin_addr.ps_s_addr = addr;
  /**/
  dss_ret = dss_sendto_dsm_chain( server->sock, dss_packet_ptr, 0,
                                  (struct ps_sockaddr*)&dss_addr,
                                  sizeof(struct ps_sockaddr_in),
                                  &dss_errno );
  /**/

  if ( DSS_ERROR == dss_ret && DS_EWOULDBLOCK == dss_errno )
  {
    /* Push the IP address onto the packet so that we will know where to send
     * the packet later
     */
    if ( sizeof(uint32) != dsm_pushdown_packed(dss_packet_ptr,
                                               &dss_addr.ps_sin_addr.ps_s_addr,
                                               sizeof(uint32),
                                               DSM_DS_SMALL_ITEM_POOL) )
    {
      LOG_MSG_ERROR_0( "Pushdown packed unable to push down addr" );
      dsm_free_packet( dss_packet_ptr );
    }
    else
    {
      dsm_enqueue( &server->write_wm, dss_packet_ptr );
      dss_ret = dss_async_select( server->sock, DS_WRITE_EVENT, &dss_errno );
      if ( DSS_SUCCESS != dss_ret )
      {
        LOG_MSG_ERROR_2( "Failed async select WRITE on %x %d",
                         server->sock, dss_errno );
        ASSERT(0);
        while ( NULL != (*dss_packet_ptr = dsm_dequeue(&server->write_wm)) )
        {
          dsm_free_packet( dss_packet_ptr );
        }
      }
    }
  }
  else if ( DSS_ERROR == dss_ret )
  {
    LOG_MSG_ERROR_3("sendto returned %d %d %d",
                    dss_ret, dss_errno, server->sock);
    dsm_free_packet( dss_packet_ptr );
  }
  *msg = NULL;
} /* dhcp_server_reply_send */
/*===========================================================================
FUNCTION DHCP_SERVER_ADDR_FREE()

DESCRIPTION
  This function cleans up when an address is freed by the address
  manager.  This consists of cleaning the ARP cache for the freed
  lease.

DEPENDENCIES
  The IP address must be an IPV4 address.
  The handle must be valid.

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  ip_addr_type - ip address to associate with the hardware address.
  char * hwaddr - hardware address.
  uint32 len - The length of the hardware address.

RETURN VALUE
  None.

SIDE EFFECTS
  Updates (removes the IP address from) the ARP cache.
===========================================================================*/
void
dhcp_server_addr_free
(
  void * handle,
  uint32 ip_addr,
  uint8 * hwaddr,
  uint32 len
)
{
  dhcp_server_info * server = handle;
  (void)len;

  ASSERT(NULL != handle);
  ASSERT(DHCP_SERVER_COOKIE == server->cookie);
  ASSERT(server == server->cookie_ptr);
  ASSERT(NULL != hwaddr);

  if(server->config.arp_cache_update_cb != NULL)
  {
    server->config.arp_cache_update_cb( server->init.iface, FALSE, ip_addr, hwaddr);
  }
} /* dhcp_server_addr_free */
/*===========================================================================
FUNCTION DHCP_SERVER_SET_PARAMETERS()

DESCRIPTION
  This function interfaces between the DHCP Address manager and the
  server interface for informing the server interface of the operating
  parameters for this connection.

DEPENDENCIES
  dhcp_server_start() must be called first. (So that the server iface is known).

PARAMETERS
  void * - Handle to the glue needed to get at the connection details.
  ip_addr_type route_ip - The address that the server interface should answer
                          to.
  ip_addr_type netmask - The subnet mask for the server interface.

RETURN VALUE
  None.

SIDE EFFECTS
  The PS interface for this handle is informed of the parameters
  passed in.
===========================================================================*/
void
dhcp_server_set_parameters
(
  void * handle,
  uint32 server_addr,
  uint32 netmask
)
{
  dhcp_server_info * server = handle;
  ps_ip_addr_type ip_addr;

  (void)netmask;

  ASSERT(NULL != server);
  ASSERT(DHCP_SERVER_COOKIE == server->cookie);
  ASSERT(server == server->cookie_ptr);
  /* We don't particularly care about the netmask at this point.  BUT,
   * we need to set the server IP address.
   */
  ip_addr.type = IPV4_ADDR;
  ip_addr.addr.v4.ps_s_addr = server_addr;
  if ( ps_iface_set_v4_addr( server->init.iface, &ip_addr ) )
  {
    DATA_ERR_FATAL("DHCP Server failed to set server IP");
  }
} /* dhcp_server_set_parameters */
