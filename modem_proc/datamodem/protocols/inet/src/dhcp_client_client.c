/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  D H C P _ C L I E N T _ C L I E N T . C

GENERAL DESCRIPTION
  DMSS Dynamic Host Configuration Protocol client socket interface code.
  This file provides the shim layer between the core and the
  DSS layer to provide transport for the DHCP messages.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dhcp_client_client.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/25/14    ds     Added IP filter registration logic to receive and process 
                   response from DHCP server.
01/10/14    vl     Optimization of F3 messages
09/21/12    ss     Replacing LOG_MSG_FATAL_ERROR with LOG_MSG_ERROR and 
                   evaluating ASSERT requirement.
02/17/10    ss     Using dsm_queue_destroy() to free the DSM watermark when it
                   is no longer needed.
01/11/10    ss     Lint warning fixes.
05/22/09    kk     Q6 compiler warning fixes.
09/26/08    am     Changed malloc/free names for off target support.
04/02/08    am     High Lint fixes.
05/15/07    es     Fixed policy_flag to use POLICY_UP_SPEC instead of 
                   POLICY_SPECIFIED to allow outgoing msg on multiple pdp 
                   contexts.
03/16/07    es     Moved bind call after sys socket option to allow multiple
                   clients bound on same DHCP port.
02/23/06    clp    Fixed race with closed socket and using it.
11/29/05    clp    Fixed race condition with closing socket then using it.
11/05/05    rt     Fixed coverity errors.
08/18/05    clp    Fix memory leak.
05/11/05    clp    Fix lint issues.
03/14/05    clp    Added serialization for processing the socket cb
12/30/04    clp    Code review: static functions now have dhcpi_ prefix
11/03/04    clp    Initial development work done.
===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "dhcp_clienti.h"
#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "dsm.h"
#include "dssocket.h"
#include "dssinternal.h" /* For the dss_*_dsm_chain functions */
#include "ps_in.h" /* For hton* macros */
#include "ps_svc.h" /* For cmd */
#include "ds_Utils_DebugMsg.h"

#define DHCP_SERVER_PORT (dss_htons(67))
#define DHCP_CLIENT_PORT (dss_htons(68))

#define DHCP_NUM_FILTERS 1

typedef enum {
  DHCP_CLIENT_CLIENT_STATE_OPEN,
  DHCP_CLIENT_CLIENT_STATE_CLOSING,
  DHCP_CLIENT_CLIENT_STATE_CLOSED
} dhcp_client_client_state_enum;

typedef struct dhcp_client_client_info {
  /* module init info */
  dhcp_client_client_init init;
  /* module config info */
  dhcp_client_client_config config;

  /* The socket handle for the socket we are communicating over. */
  sint15 sock;

  /* This indicates if we are currently trying to close the socket. */
  dhcp_client_client_state_enum state;

  /* This watermark holds any items that can't be written out right
   * away.  The items will be dequeued and sent when the write event
   * callback is called.
   */
  dsm_watermark_type write_wm;

  /* The size of the watermark DNE, apperently in bytes */
#define DHCP_CLIENT_CLIENT_WM_SZ (2000)

  /* This is the queue head needed by the above watermark */
  q_type write_wm_q;

  /* cookies: The cookie_ptr takes a value of a pointer to the
   * beginning of this structure. This happens when the shim is
   * started.
   */
  struct dhcp_client_client_info * cookie_ptr;
  /* The cookie takes the value below when allocated in _new */
#define DHCP_CLIENT_CLIENT_COOKIE ((uint32)0XD55CD55C)
  uint32 cookie;

  /* sock_event ptr -> sock_cb() takes care of allocating
   * memory, and sock_event_cmd_hdlr() of freeing it
   */
  void *sock_event_ptr;
} dhcp_client_client_info;

/* Command structure to send socket event info to PS task */
typedef struct {
  dhcp_client_client_info * info;
  sint15 sockfd;
  uint32 event_mask;
} dhcp_client_sock_event;
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           PRIVATE FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_CLIENT_REPLY_EXTRACT

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
dhcpi_client_client_reply_extract
(
  void * dest,
  void * src,
  uint32 offset,
  uint32 len
)
{
  if ( NULL == dest || NULL == src )
  {
    LOG_MSG_ERROR_2( "got null dest or src %d %d", dest, src );
    ASSERT(0);
    return FALSE;
  }
  return (len == dsm_extract(src, (uint16)offset, dest, (uint16)len)) ?
    TRUE : FALSE;
} /* dhcpi_client_client_reply_extract */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_CLIENT_SOCK_EVENT_CMD

Description:
  This function gets called by the sockets layer when there is an
  event on the socket.  We handle each of the possible events that can
  come in.  Note: we can get more than one event in the callback.

Arguements:
  ps_cmd_enum_type - the command type (PS_DHCP_CLIENT_SOCK_EVENT_CMD)
  user_data_ptr - the user data with the event info.


Return value:
  void

Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_client_sock_event_cmd
(
  ps_cmd_enum_type cmd,
  void * user_data_ptr
)
{
  dhcp_client_sock_event * event = (dhcp_client_sock_event*)user_data_ptr;
  dhcp_client_client_info * info;
  sint15 sockfd;
  uint32 event_mask;
  sint31 dss_ret;
  dsm_item_type * dss_packet;
  struct ps_sockaddr_in dss_addr;
  uint16 dss_addr_len = sizeof(struct ps_sockaddr_in);
  sint15 dss_errno;

  if ( PS_DHCP_CLIENT_SOCK_EVENT_CMD != cmd )
  {
    LOG_MSG_ERROR_2( "Didn't get sock event cmd %d %d",
                     cmd, PS_DHCP_CLIENT_SOCK_EVENT_CMD );
    ASSERT(0);
    return;
  }

  if ( NULL == event )
  {
    DATA_ERR_FATAL("Got NULL cmd in sock event cmd");
    return;
  }
  info = event->info;
  sockfd = event->sockfd;
  event_mask = event->event_mask;

  /* Verify sanity */
  if ( NULL == info )
  {
    DATA_ERR_FATAL("Got NULL info in sock callback");
    return;
  }

  info->init.d_free(event);

  if ( info->cookie_ptr != info )
  {
    LOG_MSG_ERROR_2( "Cookie pointer failed in sock callback %x %x",
                     info->cookie_ptr, info );
    ASSERT(0);
    return;
  }
  if ( DHCP_CLIENT_CLIENT_COOKIE != info->cookie )
  {
    LOG_MSG_ERROR_2( "Cookie mismatch in sock callback %x %x",
                     info->cookie, DHCP_CLIENT_CLIENT_COOKIE );
    ASSERT(0);
    return;
  }
  if ( info->sock != sockfd )
  {
    LOG_MSG_ERROR_2("Got callback for unknown socket %d %d",
                    info->sock, sockfd);
    ASSERT(0);
    return;
  }

  /* Branch on the event mask, there can be multiple events, so we
   * can't switch on the event
   */
  if ( DHCP_CLIENT_CLIENT_STATE_OPEN == info->state &&
       DS_READ_EVENT & event_mask )
  {
    while ( 0 < dss_recvfrom_dsm_chain( info->sock,
                                        &dss_packet,
                                        0,
                                        (struct ps_sockaddr*)&dss_addr,
                                        &dss_addr_len,
                                        &dss_errno ) )
    {
      info->config.reply( info->config.reply_handle,
                          dss_packet,
                          dhcpi_client_client_reply_extract);
      dsm_free_packet( &dss_packet );
    }
    if (DS_EWOULDBLOCK == dss_errno)
    {
      dss_ret = dss_async_select( info->sock, DS_READ_EVENT, &dss_errno );
      if ( DSS_SUCCESS != dss_ret )
      {
        LOG_MSG_ERROR_2( "Failed async select READ on %x %d",
                         info->sock, dss_errno );
        ASSERT(0);
        return;
      }
    }
  }

  if ( DS_ACCEPT_EVENT & event_mask )
  {
    LOG_MSG_ERROR_2( "Got ACCEPT event on client socket %x %x",
                     sockfd, event_mask );
    ASSERT(0);
    return;
  }

  if ( DHCP_CLIENT_CLIENT_STATE_OPEN == info->state &&
       DS_WRITE_EVENT & event_mask )
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
      dss_addr.ps_sin_port = DHCP_SERVER_PORT; /**/
      dss_ret = dss_sendto_dsm_chain( info->sock, &dss_packet, 0,
                                      (struct ps_sockaddr*)&dss_addr,
                                      sizeof(struct ps_sockaddr_in),
                                      &dss_errno );
      
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

  if ( DHCP_CLIENT_CLIENT_STATE_OPEN != info->state &&
       DS_CLOSE_EVENT & event_mask )
  {
    if ( DHCP_CLIENT_CLIENT_STATE_CLOSING == info->state )
    {
      dss_ret = dss_close( info->sock, &dss_errno );
      if (  DSS_SUCCESS != dss_ret )
      {
        LOG_MSG_ERROR_2( "Failed close on %x %d", info->sock, dss_errno );
        ASSERT(0);
      }
      info->state = DHCP_CLIENT_CLIENT_STATE_CLOSED;
    }
    info->init.d_free(info);
  }
} /* dhcpi_client_client_sock_event_cmd */

/*===========================================================================
STATIC FUNCTION DHCPI_CLIENT_CLIENT_SOCK_CB


Description:
  This function gets called by the sockets layer when there is an
  event on the socket.  This is then queued for processing in our own
  task space.


Arguements:
  sint15 dss_nethandle - The app id (if any) associated with the socket
  sint15 sockfd - The socket descriptor
  uint32 event_mask - The event(s) that occured on the socket
  void * handle - A handle to our info.


Return value:
  void


Dependencies:
  None.
===========================================================================*/
static void
dhcpi_client_client_sock_cb
(
  sint15 dss_nethandle,
  sint15 sockfd,
  uint32 event_mask,
  void * handle
)
{
  dhcp_client_client_info * info = handle;
  dhcp_client_sock_event * event;

  /* Verify sanity */
  if ( NULL == info )
  {
    DATA_ERR_FATAL("Got NULL info in sock callback");
    return;
  }
  if ( info->cookie_ptr != info )
  {
    LOG_MSG_ERROR_2( "Cookie pointer failed in sock callback %x %x",
                     info->cookie_ptr, info );
    ASSERT(0);
    return;
  }
  if ( DHCP_CLIENT_CLIENT_COOKIE != info->cookie )
  {
    LOG_MSG_ERROR_2( "Cookie mismatch in sock callback %x %x",
                     info->cookie, DHCP_CLIENT_CLIENT_COOKIE );
    ASSERT(0);
    return;
  }
  if ( info->sock != sockfd )
  {
    LOG_MSG_ERROR_2("Got callback for unknown socket %d %d",
                    info->sock, sockfd);
    ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_0("dhcpi_client_client_sock_cb called, received event on socket");

  info->sock_event_ptr = event = (dhcp_client_sock_event *)
    info->init.d_malloc(sizeof(dhcp_client_sock_event ));
  if ( NULL == event )
  {
    DATA_ERR_FATAL("dhcpi_client_client_sock_cb unable to get parameter memory");
    return;
  }

  event->sockfd = sockfd;
  event->event_mask = event_mask;
  event->info = info;


  (void) ps_set_cmd_handler( PS_DHCP_CLIENT_SOCK_EVENT_CMD,
                             dhcpi_client_client_sock_event_cmd );
  ps_send_cmd( PS_DHCP_CLIENT_SOCK_EVENT_CMD, event );
} /* dhcpi_client_client_sock_cb */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DHCP_CLIENTI_CLIENT_NEW

Description:
  This function initializes the client layer with internal data and
  returns a handle to the core object.

Arguements:
  dhcp_client_client_init * init - The information needed to
    initialize the client layer.

Return value:
  void * - A handle to the new client object, or NULL on failure.

Dependencies:
  This function must be called to get a handle before any other
  client functions are called.  The handle must be started (see
  dhcp_clienti_client_start() below) before the  layer is
  fully functional.
===========================================================================*/
void *
dhcp_clienti_client_new
(
   dhcp_client_client_init * init
)
{
  dhcp_client_client_info * handle;
  LOG_MSG_INFO3_0( "INFO: --- DHCP CLIENT CLIENT INIT ---" );

  /* Sanity check parameters */
  if ( NULL == init )
  {
    DATA_ERR_FATAL("Got NULL pointer for init");
    return NULL;
  }
  if ( NULL == init->d_malloc ||
       NULL == init->d_free )
  {
    LOG_MSG_ERROR_2("Got NULL for malloc and/or d_free %d %d",
                    init->d_malloc, init->d_free );
    ASSERT(0);
    return NULL;
  }

  handle = init->d_malloc(sizeof(dhcp_client_client_info));
  if ( NULL == handle )
  {
    DATA_ERR_FATAL("Insufficent memory to init DHCP client layer");
    return NULL;
  }
  memset( handle, 0, sizeof(dhcp_client_client_info) );
  handle->init = *init; /* struct copy */
  handle->cookie = DHCP_CLIENT_CLIENT_COOKIE;
  handle->sock = 0;
  handle->state = DHCP_CLIENT_CLIENT_STATE_OPEN;
  handle->sock_event_ptr = NULL;

  return handle;
} /* dhcp_clienti_client_new */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CLIENT_START

Description:
  This function configures the client layer with the information on how
  to interact with the other client modules.

Arguements:
  void * handle - Handle to the client object to start.
  dhcp_client_client_config * config - The client information
  needed to interface with other modules in the DHCP client architecture.

Return value:
  Boolean - TRUE if success, otherwise FALSE.

Dependencies:
  The handle must be have been allocated by dhcp_clienti_client_new()
  above.
===========================================================================*/
boolean
dhcp_clienti_client_start
(
  void * handle,
  dhcp_client_client_config * config
)
{
  dhcp_client_client_info * info = handle;
  sint31 dss_ret;
  dss_net_policy_info_type policy;
  sint15 dss_errno;
  boolean ret = FALSE;
  int val = TRUE;  /* This type is used internally by dss_setsockopt */
  uint32 val_len = sizeof( int );
  struct ps_sockaddr_in dss_sockaddr;
  ip_filter_type              filter;

  LOG_MSG_INFO3_0( "INFO: --- DHCP CLIENT CLIENT START ---" );

  /* sanity check parameters */
  if ( NULL == info )
  {
    DATA_ERR_FATAL("Got NULL info in sock callback");
    return FALSE;
  }
  if ( DHCP_CLIENT_CLIENT_COOKIE != info->cookie )
  {
    LOG_MSG_ERROR_2( "Cookie mismatch in sock callback %x %x",
                     info->cookie, DHCP_CLIENT_CLIENT_COOKIE );
    ASSERT(0);
    return FALSE;
  }
  if ( NULL == config )
  {
    LOG_MSG_ERROR_0("Got NULL config" );
    ASSERT(0);
    return FALSE;
  }

  /* Initialize "config" info */
  info->config = *config; /* struct copy */

  /* Initialize the write watermark */
  dsm_queue_init( &info->write_wm,
                  DHCP_CLIENT_CLIENT_WM_SZ,
                  &info->write_wm_q );

  /* Init the policy */
  memset( &policy, 0, sizeof(dss_net_policy_info_type) );
  dss_init_net_policy_info(&policy);
  /* Set up the socket. */
  policy.policy_flag = DSS_IFACE_POLICY_UP_SPEC;
  policy.iface.kind = DSS_IFACE_ID;
  policy.iface.info.id = info->init.iface;
  policy.family = DSS_AF_INET;

  dss_ret = dss_socket2( DSS_AF_INET, DSS_SOCK_DGRAM, (uint8)PS_IPPROTO_UDP,
                      dhcpi_client_client_sock_cb, handle,
                      &policy, &dss_errno );
  if ( DSS_ERROR != dss_ret )
  {
    info->sock = (sint15)dss_ret;
    dss_ret = dss_setsockopt( info->sock, (int32)DSS_SOL_SOCKET, 
                              (int32)DSS_SO_SYS_SOCK, &val, &val_len,
                              &dss_errno );
    if ( DSS_ERROR != dss_ret )
    {
      info->cookie_ptr = info;
      ret = TRUE;
    }
    else
    {
      LOG_MSG_ERROR_2( "Unable to set socket to system %d %d",
                       dss_ret, dss_errno );
      ASSERT(0);
      return FALSE;
    }

    dss_sockaddr.ps_sin_family = DSS_AF_INET;
    dss_sockaddr.ps_sin_port = DHCP_CLIENT_PORT;
    dss_sockaddr.ps_sin_addr.ps_s_addr = PS_INADDR_ANY;
    dss_ret = dss_bind( info->sock,
                        (struct ps_sockaddr*)&dss_sockaddr,
                        sizeof(struct ps_sockaddr_in),
                        &dss_errno );
    if ( DSS_SUCCESS != dss_ret )
    {
      LOG_MSG_ERROR_1( "Unable to bind to DHCP client port %d", dss_errno);
      ASSERT(0);
      return FALSE;
    }

    dss_ret = dss_async_select( info->sock, DS_READ_EVENT, &dss_errno );
    if ( DSS_SUCCESS != dss_ret )
    {
      LOG_MSG_ERROR_2( "Failed async select READ on %x %d",
                       info->sock, dss_errno );
      ASSERT(0);
      return FALSE;
    }

    /* Set up the incoming filter */
    memset( &filter, 0, sizeof(ip_filter_type) );
    filter.ip_vsn       = IP_V4;
  
    filter.ip_hdr.v4.field_mask = 
      (ipfltr_ip4_hdr_field_mask_type)IPFLTR_MASK_IP4_NEXT_HDR_PROT;
    filter.ip_hdr.v4.next_hdr_prot = (uint8)PS_IPPROTO_UDP;
  
    filter.next_prot_hdr.udp.field_mask  = 
      (ipfltr_udp_hdr_field_mask_type)IPFLTR_MASK_UDP_DST_PORT;
    filter.next_prot_hdr.udp.src.port = 0;
    filter.next_prot_hdr.udp.dst.port = DHCP_CLIENT_PORT; /**/
  
    if ( 0 !=
         dss_reg_ip_filter ( info->sock,
                           info->init.iface,
                           IPFLTR_DEFAULT_TYPE,
                           DHCP_NUM_FILTERS,
                           (void *)&filter,
                           &dss_errno ) )
    {
      DATA_ERR_FATAL("FAIL:dhcp_client_start unable to add filter");
      ret = FALSE;
    }
  }
  else
  {
    LOG_MSG_ERROR_2( "Unable to open socket %d %d", dss_ret, dss_errno );
    ASSERT(0);
    return FALSE;
  }

  return ret;
} /* dhcp_clienti_client_start */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CLIENT_STOP

Description:
  This function stops a DHCP client client layer object and frees the
  associated memory.  Additionally, the handle pointer will be NULL'd.

Arguements:
  void ** handle - The handle to the client layer object to release.

Return value:
  None.

Dependencies:
  The handle must have been allocated by dhcp_clienti_client_new().
===========================================================================*/
void
dhcp_clienti_client_stop
(
  void ** handle
)
{
  dhcp_client_client_info * info;
  sint31 dss_ret;
  sint15 dss_errno;

  /* Sanity check parameters */
  if ( NULL == handle )
  {
    DATA_ERR_FATAL("Got NULL handler in client stop");
    return;
  }
  info = *handle;

  if ( NULL == info )
  {
    DATA_ERR_FATAL("Got NULL info in sock callback");
    return;
  }
  if ( DHCP_CLIENT_CLIENT_COOKIE != info->cookie )
  {
    LOG_MSG_ERROR_2( "Cookie mismatch in sock callback %x %x",
                     info->cookie, DHCP_CLIENT_CLIENT_COOKIE );
    ASSERT(0);
    return;
  }

  /* Free anything left on the write WM and destroy the watermark */
  dsm_queue_destroy(&info->write_wm);
  
  /* Close the socket, if we can */
  dss_ret = dss_close( info->sock, &dss_errno );
  if ( DSS_ERROR == dss_ret && DS_EWOULDBLOCK == dss_errno )
  {
    info->state = DHCP_CLIENT_CLIENT_STATE_CLOSING;
    /* In this case, we free info when the close event comes in */
    dss_ret = dss_async_select( info->sock, DS_CLOSE_EVENT, &dss_errno );
    if ( DSS_SUCCESS != dss_ret )
    {
      LOG_MSG_ERROR_2( "Failed async select CLOSE on %x %d",
                       info->sock, dss_errno );
      info->init.d_free(info);
      ASSERT(0);
    }
  }
  else if ( DSS_ERROR == dss_ret )
  {
    LOG_MSG_ERROR_2( "Error closing socket %x %d", info->sock, dss_errno );
    info->state = DHCP_CLIENT_CLIENT_STATE_CLOSED;
    info->init.d_free(info);
    ASSERT(0);
  }
  else
  {
    /* We successfully closed the socket, but we may still have
     * outstanding socket callbacks pending.  To avoid referencing a
     * freed info struct, we send ourselfs another closed event
     * here.
     */
    info->state = DHCP_CLIENT_CLIENT_STATE_CLOSED;
    dhcpi_client_client_sock_cb( 0, info->sock, DS_CLOSE_EVENT, info );
  }
  LOG_MSG_INFO3_0( "INFO: --- DHCP CLIENT CLIENT STOP ---" );
  *handle = NULL;
} /* dhcp_clienti_client_stop */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CLIENT_REQUEST_NEW

Description:
  This function allocates a new request object.  This object is used to build
  a request to be sent out through the client layer.  Data is added to the
  object using the dhcp_clienti_client_request_pushdown_tail function, and sent
  using the dhcp_clienti_client_request_send function which will also free the
  object.  If the send should be aborted after the object has been allocated,
  then the dhcp_clienti_client_request_free function should be used.

Arguements:
  void * handle - Handle to the client layer object allocated by
    dhcp_clienti_client_new().
  void ** msg - The request object to be allocated.
  uint32 size - The size of the message to be sent.

Return value:
  boolean - TRUE if msg allocated correctly, otherwise FALSE.
  void ** msg - The allocated object.

Dependencies:
  The client layer must have been properly started using
  dhcp_clienti_client_start().
===========================================================================*/
boolean
dhcp_clienti_client_request_new
(
  void * handle,
  void ** msg,
  uint32 size
)
{
  dhcp_client_client_info * info = handle;
  (void)size;

  /* Sanity check parameters */
  if ( NULL == info )
  {
    DATA_ERR_FATAL("Got NULL info in sock callback");
    return FALSE;
  }
  if ( info->cookie_ptr != info )
  {
    LOG_MSG_ERROR_2( "Cookie pointer failed in sock callback %x %x",
                     info->cookie_ptr, info );
    ASSERT(0);
    return FALSE;
  }
  if ( DHCP_CLIENT_CLIENT_COOKIE != info->cookie )
  {
    LOG_MSG_ERROR_2( "Cookie mismatch in sock callback %x %x",
                     info->cookie, DHCP_CLIENT_CLIENT_COOKIE );
    ASSERT(0);
    return FALSE;
  }
  if ( NULL == msg )
  {
    DATA_ERR_FATAL( "Got NULL message pointer pointer" );
    return FALSE;
  }

  *msg = NULL;
  return TRUE;
} /* dhcp_clienti_client_request_new */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CLIENT_REQUEST_PUSHDOWN_TAIL

Description:
  This function adds data to the end of a request object.

Arguements:
  void * handle - The client layer handle.
  void ** msg - The request object to add data to.
  void * data - The data to add
  uint32 size - The amount of data to add.

Return value:
  boolean - TRUE if data added correctly, otherwise FALSE.

Dependencies:
  The client layer must have been properly started using
  dhcp_clienti_client_start().
  The msg must have been allocated using the dhcp_clienti_client_request_new()
  function above.
===========================================================================*/
boolean
dhcp_clienti_client_request_pushdown_tail
(
  void * handle,
  void ** msg,
  void * data,
  uint32 size
)
{
  dhcp_client_client_info * info = handle;

  /* Sanity check parameters */
  if ( NULL == info )
  {
    DATA_ERR_FATAL("Got NULL info in sock callback");
    return FALSE;
  }
  if ( info->cookie_ptr != info )
  {
    LOG_MSG_ERROR_2( "Cookie pointer failed in sock callback %x %x",
                     info->cookie_ptr, info );
    ASSERT(0);
    return FALSE;
  }
  if ( DHCP_CLIENT_CLIENT_COOKIE != info->cookie )
  {
    LOG_MSG_ERROR_2( "Cookie mismatch in sock callback %x %x",
                     info->cookie, DHCP_CLIENT_CLIENT_COOKIE );
    ASSERT(0);
    return FALSE;
  }
  if ( NULL == msg )
  {
    DATA_ERR_FATAL( "Got NULL message pointer pointer" );
    return FALSE;
  }

  return dsm_pushdown_tail( (dsm_item_type **)msg, data, (uint16)size,
                       DSM_DS_SMALL_ITEM_POOL ) < size ?  FALSE : TRUE;
} /* dhcp_clienti_client_request_pushdown_tail */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CLIENT_REQUEST_SEND

Description:
  This function sends the data collected in the request object out the
  configured interface.

Arguements:
  void * handle - The client layer handle.
  void ** msg - The request object to send.
  uint32 ip_addr - The IPv4 address to which to send the data.  This may be
    the broadcast address.

Return value:
  None.  The msg handle will be freed by the client layer after the message
  has been sent.

Dependencies:
  The client layer must have been properly started using
  dhcp_clienti_client_start().
  The msg must have been allocated using the dhcp_clienti_client_request_new()
  function above.
===========================================================================*/
void
dhcp_clienti_client_request_send
(
  void * handle,
  void ** msg,
  uint32 ip_addr
)
{
  dhcp_client_client_info * info = handle;
  struct ps_sockaddr_in dss_addr;
  sint15 dss_errno;
  sint31 dss_ret;
  dsm_item_type ** dss_packet_ptr = (dsm_item_type **) msg;

  /* Sanity check parameters */
  if ( NULL == info )
  {
    DATA_ERR_FATAL("Got NULL info in sock callback");
    return;
  }
  if ( info->cookie_ptr != info )
  {
    LOG_MSG_ERROR_2( "Cookie pointer failed in sock callback %x %x",
                     info->cookie_ptr, info );
    ASSERT(0);
    return;
  }
  if ( DHCP_CLIENT_CLIENT_COOKIE != info->cookie )
  {
    LOG_MSG_ERROR_2( "Cookie mismatch in sock callback %x %x",
                     info->cookie, DHCP_CLIENT_CLIENT_COOKIE );
    ASSERT(0);
    return;
  }
  if ( NULL == msg )
  {
    DATA_ERR_FATAL( "Got NULL message pointer pointer" );
    return;
  }


  dss_addr.ps_sin_family = DSS_AF_INET;
  dss_addr.ps_sin_port = DHCP_SERVER_PORT;
  dss_addr.ps_sin_addr.ps_s_addr = ip_addr;
  dss_ret = dss_sendto_dsm_chain( info->sock, dss_packet_ptr, 0,
                                  (struct ps_sockaddr*)&dss_addr,
                                  sizeof(struct ps_sockaddr_in),
                                  &dss_errno );
  
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
      dsm_enqueue( &info->write_wm, dss_packet_ptr );
      dss_ret = dss_async_select( info->sock, DS_WRITE_EVENT, &dss_errno );
      if ( DSS_SUCCESS != dss_ret )
      {
        LOG_MSG_ERROR_2( "Failed async select WRITE on %x %d",
                         info->sock, dss_errno );
        ASSERT(0);
        while ( NULL != (*dss_packet_ptr = dsm_dequeue(&info->write_wm)) )
        {
          dsm_free_packet( dss_packet_ptr );
        }
      }
    }
  }
  else if ( DSS_ERROR == dss_ret )
  {
    LOG_MSG_ERROR_3("sendto returned %d %d %d",
                    dss_ret, dss_errno, info->sock);
    dsm_free_packet( dss_packet_ptr );
  }
} /* dhcp_clienti_client_request_send */

/*===========================================================================
FUNCTION DHCP_CLIENTI_CLIENT_REQUEST_FREE

Description:
  This function frees the allocated request object.  This function should only
  be called if the object is NOT to be sent with the
  dhcp_client_cleint_request_send() function above.

Arguements:
  void * handle - The client layer handle.
  void ** msg - The request object to send.

Return value:
  None. The msg handle will be freed.

Dependencies:
  The client layer must have been properly started using
  dhcp_clienti_client_start().
  The msg must have been allocated using the dhcp_clienti_client_request_new()
  function above.
===========================================================================*/
void
dhcp_clienti_client_request_free
(
  void * handle,
  void ** msg
)
{
  (void)handle;
  /* Sanity check parameters that we care about */
  if ( NULL == msg )
  {
    DATA_ERR_FATAL("Got NULL message pointer pointer");
    return;
  }

  dsm_free_packet( (dsm_item_type **)msg );
} /* dhcp_clienti_client_request_free */
