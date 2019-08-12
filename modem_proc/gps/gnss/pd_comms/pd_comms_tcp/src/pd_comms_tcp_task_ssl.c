/*======================================================================

     PDCOMM Application task implementation file (SSL-related with BIT APIs)

 GENERAL DESCRIPTION
  This file contains the implementation of PDCOMM Application task.
  This file contains routines related to SSL on top of BIT APIs.

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/pd_comms_tcp/src/pd_comms_tcp_task_ssl.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 06/30/11   rh   Cleaned up SSL close/delete session code
 05/27/11   rh   Delay ACK for SSL write until errno_callback from SSL
 02/01/11   rh   Initial creation derived from pd_comms_tcp_task.c
======================================================================*/

#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_CGPS_PDCOMM 

#ifdef FEATURE_GNSS_BIT_API

#include "dssocket.h"
#include "dserrno.h"
#include "dsm_item.h" 
#include "msg.h"
#include "secapi.h"
#include "secssl.h"
#include "secssldef.h"
#include "aries_os_api.h"
#include "aries_os_globals.h"

#include "pd_comms_api.h"
#include "pd_comms_client_api.h"
#include "pd_comms_app_api.h"
#include "pd_comms_tcp_local_api.h"
#include "pd_comms_tcp_data.h"

#include "gpsone_bit_api.h"  
#include "gpsone_bit_local_api.h"  


/***************************** Constants *************************************/
#define GPSONE_BIT_MAX_SSL_PKT_SIZE    (32767)  // max value for sint15

/***************************** Variables *************************************/
extern pd_comms_tcp_task_handle_data_type z_handle_table[];

/**************************** Data Structures *******************************/


/*****************************  Function Definitions ***************************/

// functions implemented in tcp_task_common.c
extern void pd_comms_tcp_task_update_state (pd_comms_handle_type z_handle, pd_comms_state_type z_new_state,
                                            boolean notify_client, uint32 q_result);

extern void  pd_comms_tcp_task_reset_inactivity_count (uint32 q_handle_id);

extern pd_comms_tcp_task_operation_status_type pd_comms_tcp_task_tcp_connect (pd_comms_handle_type z_handle);

extern void pd_comms_tcp_task_tcp_disconnect (pd_comms_handle_type z_handle, boolean b_abort, boolean u_notifyClient);

// forward function declarations
void pd_comms_tcp_task_ssl_forward_event(sint15 sockfd);


/*===========================================================================
FUNCTION GPSONE_BIT_READ_DSM_CHAIN()

DESCRIPTION
  Reads a DSM item chain from the BIT TCP transport.

  This function will be called in SSL's task context

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read.  A return of 0 indicates that an End-of-File
      condition has occurred.

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
  DS_EINVAL           can't read from a listen socket

SIDE EFFECTS
  None.
===========================================================================*/
static sint15 gpsone_bit_read_dsm_chain
(
  sint15 sockfd,                                      /* socket descriptor */
  dsm_item_type  **item_ptr,          /* ptr to item chain containing data */
  sint15 *dss_errno                               /* error condition value */
)
{
  uint32 max_read, total_bytes = 0;
  uint32 x_bytes_read = 0, x_bytes_leftover = 0;
  gpsone_bit_status_e_type   status;
  /* SSL will call rx_fptr() from a single thread, so no concurrent access to this
     static buffer. i.e just need one global buffer, not per connection buffer */
  static uint8 rx_dsm_buf[GPSONE_BIT_MAX_BUF_SIZE];

  PDCOMM_APP_MSG_LOW("gpsone_bit_read_dsm_chain: fd=%d", sockfd, 0, 0);

  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a dss_errno variable was specified - Prevents
    dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if( dss_errno == NULL )
  {
    PDCOMM_APP_MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Make sure that the passed item_ptr is valid
  -------------------------------------------------------------------------*/
  if ( item_ptr == NULL)
  {
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Make sure it's a valid SSL connection
  -------------------------------------------------------------------------*/
  if (sockfd < 0 || sockfd >= PDCOMM_MAX_HANDLES ||
      z_handle_table[sockfd].in_use == FALSE ||
      z_handle_table[sockfd].z_ssl_handle == NULL)
  {
    *dss_errno = DS_EBADF;
    return DSS_ERROR;
  }

  // It's OK to do SSL read/write during CONNECTING/DISCONNECTING/CONNECTED states
  if (z_handle_table[sockfd].e_link_status == PDCOMM_STATE_DISCONNECTED)
  {
    *dss_errno = DS_ENOTCONN;
    return DSS_ERROR;  
  }

  *dss_errno = DS_ENOERR; /* 0x7FF for success, 0 is for DS_EEOF */

  do
  {
    max_read = sizeof(rx_dsm_buf);
    if (total_bytes + max_read > GPSONE_BIT_MAX_SSL_PKT_SIZE)
    {
      max_read = GPSONE_BIT_MAX_SSL_PKT_SIZE - sizeof(rx_dsm_buf); // limit the read size
    }

    status = gpsone_bit_recv(z_handle_table[sockfd].session_handle, (uint8 *)&rx_dsm_buf, 
                             max_read, &x_bytes_read, &x_bytes_leftover);

    if (status == GPSONE_BIT_STATUS_NO_MORE_DATA)
    {
      // no more data available, not fatal error!
      x_bytes_read = x_bytes_leftover = 0;
      if (total_bytes > 0)
      {
        return (sint15)total_bytes; // no more data, but we got some already
      }
      else
      {
        *dss_errno = DS_EWOULDBLOCK;
        return DSS_ERROR; // -1
      }
    }
    else if ( status != GPSONE_BIT_STATUS_SUCCESS ) // failure case, no GPSONE_BIT_STATUS_WAIT for gpsone_bit_recv()!
    {
      dsm_free_packet(item_ptr); // can take NULL ptr
      *dss_errno = DS_EPIPE;
      return DSS_ERROR; // TODO: do we need to set DS_CLOSE_EVENT?
    }

    if (x_bytes_read > 0)
    {
      if (*item_ptr == NULL) 
      {
        *item_ptr = dsm_new_buffer(DSM_DS_POOL_SIZE(x_bytes_read));
        PDCOMM_APP_MSG_LOW("Allocating new dsm %x for RX(%d bytes read)", *item_ptr, x_bytes_read, 0);
        if (*item_ptr == NULL)
        {
          *dss_errno = DS_EFAULT;
          return DSS_ERROR; // can't allocate DSM!!
        }
      }

      total_bytes += x_bytes_read;
      if (dsm_pushdown_tail(item_ptr, rx_dsm_buf, (uint16)x_bytes_read, DSM_DS_POOL_SIZE(x_bytes_read)) != x_bytes_read)
      {
        PDCOMM_APP_MSG_ERROR("Can't push %d bytes to DSM", x_bytes_read, 0, 0);
        dsm_free_packet(item_ptr);
        *dss_errno = DS_EFAULT;
        return DSS_ERROR; // can't add bytes to DSM
      }
    }

  }while(x_bytes_read > 0 && x_bytes_leftover > 0 && total_bytes < GPSONE_BIT_MAX_SSL_PKT_SIZE);
    
  // still has some data available, send another DS_READ_EVENT
  if (x_bytes_leftover > 0) 
  {
    gpsone_bit_event_payload_type  event_payload;

    // generate SEND_RESULT manually
    event_payload.event = GPSONE_BIT_EVENT_DATA_READY;
    event_payload.arg.bytes_available = x_bytes_leftover;
    event_payload.result = GPSONE_BIT_STATUS_SUCCESS;

    if (gpsone_bit_post_event(z_handle_table[sockfd].session_handle, 
                              GPSONE_BIT_MAKE_TRANSID(((uint32)sockfd), 0), &event_payload) != GPSONE_BIT_STATUS_SUCCESS)
    {
       PDCOMM_APP_MSG_ERROR("gpsone_bit_post_event failed!", 0, 0, 0); // IPC failure
    }
    else
    {
       PDCOMM_APP_MSG_LOW("Generated DATA_READY for %d bytes(sockfd=%d)", x_bytes_leftover, sockfd, 0 );
    }
  }

  PDCOMM_APP_MSG_LOW("Leave gpsone_bit_read_dsm_chain(fd=%d, read_bytes=%d, leftover=%d)", sockfd, total_bytes, x_bytes_leftover);

  return (sint15)total_bytes;
} /* gpsone_bit_read_dsm_chain() */


/*===========================================================================
FUNCTION GPSONE_BIT_WRITE_DSM_CHAIN()

DESCRIPTION
  Sends the specified DSM item chain over the TCP transport.  Lower layers
  will be responsible for deallocating the DSM item chain. 

  This function will be called in SSL's task context

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written.

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
  DS_EOPNOTSUPP       invalid server address specified

SIDE EFFECTS
  The pointer to the DSM item chain is set to NULL.
===========================================================================*/
static sint15 gpsone_bit_write_dsm_chain
(
  sint15 sockfd,                                      /* socket descriptor */
  dsm_item_type **item_ptr,          /* DSM item chain containing the data */
  sint15 *dss_errno                               /* error condition value */
)
{
  gpsone_bit_status_e_type status;
  uint32 dsm_len, total_bytes;
  uint16 cnt;
  /* SSL will call tx_fptr() from a single thread, so no concurrent access to this
     static buffer. i.e just need one global buffer, not per connection buffer */
  static uint8 tx_dsm_buf[GPSONE_BIT_MAX_BUF_SIZE];

  PDCOMM_APP_MSG_LOW("gpsone_bit_write_dsm_chain: fd=%d", sockfd, 0, 0);

  /*-------------------------------------------------------------------------
    Assert that a valid ptr to a dss_errno variable was specified - Prevents
    dereferencing of NULL ptrs.
  -------------------------------------------------------------------------*/
  if( dss_errno == NULL )
  {
    PDCOMM_APP_MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Make sure that the passed item_ptr is valid
  -------------------------------------------------------------------------*/
  if ( item_ptr == NULL  || *item_ptr == NULL)
  {
    *dss_errno = DS_EFAULT;
    return DSS_ERROR;
  }

  /*-------------------------------------------------------------------------
    Make sure it's a valid SSL connection
  -------------------------------------------------------------------------*/
  if (sockfd < 0 || sockfd >= PDCOMM_MAX_HANDLES ||
      z_handle_table[sockfd].in_use == FALSE ||
      z_handle_table[sockfd].z_ssl_handle == NULL)
  {
    *dss_errno = DS_EBADF;
    return DSS_ERROR;
  }

  // It's OK to do SSL read/write during CONNECTING/DISCONNECTING/CONNECTED states
  if (z_handle_table[sockfd].e_link_status == PDCOMM_STATE_DISCONNECTED)
  {
    *dss_errno = DS_ENOTCONN;
    return DSS_ERROR;  
  }

  dsm_len = dsm_length_packet(*item_ptr);
  if (z_handle_table[sockfd].ssl_to_write_bytes != 0) // last write still pending
  {
    PDCOMM_APP_MSG_ERROR("Last SSL write(%d bytes) is still pending, requested new write(%d bytes)", 
              z_handle_table[sockfd].ssl_to_write_bytes, dsm_len, 0);
    *dss_errno = DS_EWOULDBLOCK;

    // clear the previous write event if bit is set since SSL need to wait for the new write event
    z_handle_table[sockfd].ssl_events &= ~DS_WRITE_EVENT;  

#if 1
    return DSS_ERROR;  
#else
    z_handle_table[sockfd].ssl_to_write_bytes = 0;  // for testing, allow another write
#endif
  }

  if (dsm_len > GPSONE_BIT_MAX_SSL_PKT_SIZE)
  {
    PDCOMM_APP_MSG_ERROR("Requested new SSL write(%d bytes), max allowed %d", 
              dsm_len, GPSONE_BIT_MAX_SSL_PKT_SIZE, 0);
    *dss_errno = DS_EWOULDBLOCK; // TODO: any better error code?
    return DSS_ERROR;
  }

  if ( dsm_len == 0)
  {
    PDCOMM_APP_MSG_ERROR("Trying to write 0 byte to DSM", 0, 0, 0);
    return 0; // 0 byte written
  }

  z_handle_table[sockfd].ssl_written_bytes = 0;  // 0 bytes written
  total_bytes = 0;

  while(dsm_len > 0)
  {
    // will send cnt bytes to BIT GPS daemon
    if (dsm_len > sizeof(tx_dsm_buf))
    {
      cnt = (uint16)sizeof(tx_dsm_buf);
    }
    else
    {
      cnt = (uint16)dsm_len;
    }

    // will remove from DSM (dsm_extract just copy won't remove)
    if (dsm_pullup(item_ptr, tx_dsm_buf, cnt) != cnt)  
    {
      PDCOMM_APP_MSG_ERROR("Failed to extract %d bytes from DSM (extracted %d bytes)", 
                cnt, total_bytes, 0);
      dsm_free_packet(item_ptr); // free the packet
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
    }

    // increase early in case SEND_RESULT event comes back immediately
    // This value may be changed during SEND_RESULT event handling
    z_handle_table[sockfd].ssl_to_write_bytes += cnt; 

    // (for now, write is sequential! ie. no 2nd write until 1st write's result comes back)
    status = gpsone_bit_send(z_handle_table[sockfd].session_handle, (uint32)sockfd, (uint8 *)&tx_dsm_buf, cnt);

    if( status == GPSONE_BIT_STATUS_SUCCESS )
    {
      // all length bytes have been written successfully, no notify() event needed
      z_handle_table[sockfd].ssl_written_bytes =+ cnt; 
    }
    else if (status == GPSONE_BIT_STATUS_WAIT)  
    {
      // write pending, wait for notify() event then increase ssl_written_bytes
    }
    else  // failure case
    {
      dsm_free_packet(item_ptr); // free the packet
      *dss_errno = DS_EPIPE;  // TODO: do we need to set DS_CLOSE_EVENT?
      return DSS_ERROR;
    }

    total_bytes += cnt;
    dsm_len -= cnt;
  }

  PDCOMM_APP_MSG_LOW("Leave gpsone_bit_write_dsm_chain(fd=%d, bytes=%d written=%d)", 
          sockfd, total_bytes, z_handle_table[sockfd].ssl_written_bytes);

  // send a SEND_RESULT event to PDCOMM so DS_WRITE_EVENT can be generated for SSL
  if (z_handle_table[sockfd].ssl_written_bytes > 0)
  {
    // some bytes written synchronously by GPS daemon
    gpsone_bit_event_payload_type  event_payload;

    // generate SEND_RESULT manually
    event_payload.event = GPSONE_BIT_EVENT_SEND_RESULT;
    event_payload.arg.bytes_sent = z_handle_table[sockfd].ssl_written_bytes;
    event_payload.result = GPSONE_BIT_STATUS_SUCCESS;

    if (gpsone_bit_post_event(z_handle_table[sockfd].session_handle, 
                              GPSONE_BIT_MAKE_TRANSID(((uint32)sockfd), 0), &event_payload) != GPSONE_BIT_STATUS_SUCCESS)
    {
       PDCOMM_APP_MSG_ERROR("gpsone_bit_post_event failed!", 0, 0, 0); // IPC failure
    }
    else
    {
       PDCOMM_APP_MSG_LOW("Generated SEND_RESULT for %d bytes(sockfd=%d, to_write=%d)", 
               z_handle_table[sockfd].ssl_written_bytes, sockfd, z_handle_table[sockfd].ssl_to_write_bytes);
    }
    z_handle_table[sockfd].ssl_written_bytes = 0; 
  }

  dsm_free_packet(item_ptr); // free the packet

  *dss_errno = DS_ENOERR; /* 0x7FF for success, 0 is for DS_EEOF */

  return (sint15)total_bytes;

} /* gpsone_bit_write_dsm_chain() */


/*===========================================================================
FUNCTION GPSONE_BIT_ASYNC_SELECT()

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
static sint31 gpsone_bit_async_select
(
  sint15 sockfd,                                      /* socket descriptor */
  sint31 interest_mask,                        /* bitmask of events to set */
  sint15 *dss_errno                               /* error condition value */
)
{
  PDCOMM_APP_MSG_LOW("gpsone_bit_async_select(): sockfd=%d, interest_mask=0x%x",
          sockfd, interest_mask, 0);

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    PDCOMM_APP_MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Make sure it's a valid SSL connection
  -------------------------------------------------------------------------*/
  if (sockfd < 0 || sockfd >= PDCOMM_MAX_HANDLES ||
      z_handle_table[sockfd].in_use == FALSE ||
      z_handle_table[sockfd].z_ssl_handle == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Update requested event mask for the socket. Add it to existing requested
    mask.
  -------------------------------------------------------------------------*/
  z_handle_table[sockfd].ssl_event_mask |= (uint32)interest_mask;

  // some events pending already, notify immediately (SSL will set REX_SIG)
  pd_comms_tcp_task_ssl_forward_event(sockfd);

  return (DSS_SUCCESS);

} /* gpsone_bit_async_select() */


/*===========================================================================
FUNCTION GPSONE_BIT_ASYNC_DESELECT()

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
static sint15 gpsone_bit_async_deselect
(
  sint15 sockfd,                                      /* socket descriptor */
  sint31 clr_interest_mask,                  /* bitmask of events to clear */
  sint15 *dss_errno                               /* error condition value */
)
{
  PDCOMM_APP_MSG_LOW("gpsone_bit_async_deselect(): sockfd=%d, clr_mask=0x%x",
          sockfd, clr_interest_mask, 0);

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    PDCOMM_APP_MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }
  *dss_errno = DSS_SUCCESS;

  /*-------------------------------------------------------------------------
    Make sure it's a valid SSL connection
  -------------------------------------------------------------------------*/
  if (sockfd < 0 || sockfd >= PDCOMM_MAX_HANDLES ||
      z_handle_table[sockfd].in_use == FALSE ||
      z_handle_table[sockfd].z_ssl_handle == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Update requested event mask for the socket. Clear out any of the bits
    specified.
  -------------------------------------------------------------------------*/

  //PS_ENTER_CRIT_SECTION(&global_ps_crit_section); // TODO
  z_handle_table[sockfd].ssl_event_mask &= ~((uint32)clr_interest_mask);
  //PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return (DSS_SUCCESS);
} /* gpsone_bit_async_deselect() */


/*===========================================================================
FUNCTION GPSONE_BIT_GETNEXTEVENT()

DESCRIPTION
  This function performs a real-time check to determine if any of the events
  of interest specified in the socket 's event mask have
  occurred.  It also clears any bits in the event mask that have occurred.
  The application must re-enable these events through a subsequent call to
  gpsone_bit_async_select().  The application may pass in a pointer to a single
  socket descriptor to determine if any events have occurred for that socket

DEPENDENCIES
  None.

RETURN VALUE
  Returns an event mask of the events that were asserted for specified sockets
  descriptor.  A value of 0 indicates that no events have occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF             invalid socket descriptor is specfied

SIDE EFFECTS
  Clears the bits in the event mask, corresponding to the events that have
  occurred.
===========================================================================*/
static sint31 gpsone_bit_getnextevent
(
  sint15 dss_nethandle,                               /* application ID */
  sint15 *sockfd_ptr,                                 /* socket descriptor */
  sint15 *dss_errno                               /* error condition value */
)
{
  uint32 evt=0;                              /* returned event mask */
  sint15 sockfd;

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a dss_errno variable is specified
  -------------------------------------------------------------------------*/
  if (dss_errno == NULL)
  {
    PDCOMM_APP_MSG_ERROR("dss_errno is NULL", 0, 0, 0);
    return(DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Check that a valid ptr to a sockfd_ptr variable is specified
  -------------------------------------------------------------------------*/
  if( sockfd_ptr == NULL )
  {
    PDCOMM_APP_MSG_HIGH("sockfd_ptr is NULL", 0, 0, 0);
    *dss_errno = DS_EBADF;
    return(DSS_ERROR);
  }

  sockfd = *sockfd_ptr;
  PDCOMM_APP_MSG_MED(" gpsone_bit_getnextevent() called (fd=%d) ", sockfd, 0, 0);

  /*-------------------------------------------------------------------------
    Make sure it's a valid SSL connection
  -------------------------------------------------------------------------*/
  if (sockfd < 0 || sockfd >= PDCOMM_MAX_HANDLES ||
      z_handle_table[sockfd].in_use == FALSE ||
      z_handle_table[sockfd].z_ssl_handle == NULL)
  {
    *dss_errno = DS_EBADF;
    return (DSS_ERROR);
  }
  
  *dss_errno = DSS_SUCCESS;

  /*-----------------------------------------------------------------------
    Get the event mask for the socket and if the requested events for that
    socket are enabled, return the event mask, and remove the events
    from the requested_event_mask.
  ----------------------------------------------------------------------*/
  evt = (z_handle_table[sockfd].ssl_event_mask & z_handle_table[sockfd].ssl_events);

  if (evt)
  {
    //PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    z_handle_table[sockfd].ssl_event_mask &= ~(evt);

    // clear the reported events
    z_handle_table[sockfd].ssl_events &= ~(evt);

    //PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return (sint31)(evt);
  } 

  /*-------------------------------------------------------------------------
    No events have occurred, so just return 0.
  -------------------------------------------------------------------------*/
  return(DSS_SUCCESS);

} /* gpsone_bit_getnextevent() */


/*===========================================================================
FUNCTION pd_comms_tcp_task_ssl_forward_event

DESCRIPTION
 This function forwards simulated DS events to SSL layer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pd_comms_tcp_task_ssl_forward_event(sint15 sockfd)
{
  uint32 event;

  //PS_ENTER_CRIT_SECTION(&global_ps_crit_section); // TODO

  event = (z_handle_table[sockfd].ssl_events & z_handle_table[sockfd].ssl_event_mask);
  if (event)
  {
    PDCOMM_APP_MSG_LOW("pd_comms_tcp_task_ssl_forward_event(): all_events=0x%x evtmask=0x%x evt=0x%x",
            z_handle_table[sockfd].ssl_events, z_handle_table[sockfd].ssl_event_mask, event);

    // DSS2 way, post events directly to SSL
    /* x_app_id is for DSS (not being used), use 0
       secssl.c's secssl_ds2_default_sock_event_handler() will call secssli_dss_socket2_cb()
       in secssli.c, will set SECSSL_SOCKET_CB_SIG, doesn't check appid along the call */
    secssl_ds2_default_sock_event_handler(0, // z_handle_table[q_handle_id].x_app_id,
                                        sockfd,
                                        event,
                                        z_handle_table[sockfd].z_ssl_handle);

    z_handle_table[sockfd].ssl_events &= ~event;  // clear the reported events
  }

   // DSS1 way, set this SOCKET_EVENT, then SSL will call getnextevent() to get it
  /*secssl_ds_default_event_handler(SECSSL_DS_SOCKET_EVENT);*/

  //PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* pd_comms_tcp_task_ssl_forward_event */


/*===========================================================================
FUNCTION pd_comms_tcp_task_teardown_tcp

DESCRIPTION
 This function tears down TCP connection (external socket)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static pd_comms_tcp_task_operation_status_type pd_comms_tcp_task_teardown_tcp(
  /*handle for which connection is to be closed */
  pd_comms_handle_type z_handle)
{
  pd_comms_tcp_task_operation_status_type  e_status = SUCCEEDED;
  gpsone_bit_status_e_type                 status;
  gpsone_bit_disconnect_params_type        disconnect_param;

  disconnect_param.force_disconnect = FALSE;

  switch(z_handle_table[z_handle.q_handle_id].e_link_status)
  {
    case PDCOMM_STATE_DISCONNECTED:   // already disconnected, do nothing
    {
      break;
    }

    case PDCOMM_STATE_OPENING:
    {
      // state not used by BIT API  (opening PPP)
      break;
    }

    case PDCOMM_STATE_DISCONNECTING:
    case PDCOMM_STATE_CONNECTED:
    case PDCOMM_STATE_CONNECTING:
    {
      // use z_handle.q_handle_id as the transaction_id so we can look it up when notify() send back RESULT
      status = gpsone_bit_disconnect(z_handle_table[z_handle.q_handle_id].session_handle, z_handle.q_handle_id, 
                                     &disconnect_param);  // call wrapper function to disconnect

      if (status == GPSONE_BIT_STATUS_SUCCESS)
      {
        pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED,
                                     TRUE,(uint32) PDCOMM_RESULT_OK);
      }
      else if (status == GPSONE_BIT_STATUS_WAIT)
      {
        if (z_handle_table[z_handle.q_handle_id].e_link_status != PDCOMM_STATE_DISCONNECTING)
        {
          pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                       TRUE,(uint32) PDCOMM_RESULT_WAIT);
        }
        e_status = PENDING; // will move to DISCONNECTED when DISCONNECT_RESULT is received
      }
      else // failure? mark it as DISCONNECTED anyway 
      { 
        pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTED, 
                                     TRUE,(uint32) PDCOMM_DISCONNECT_ERROR_UNKNOWN);
        e_status = FAILED;
      }

      break;
    }

  } /* switch(link_status) ends*/

  return e_status;

}/*pd_comms_tcp_task_teardown_tcp*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_retrieve_root_certificate

DESCRIPTION
 This function retrieves a pointer to the NULL terminated H-SLP root certificate

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if succeeded, FALSE if failed

SIDE EFFECTS
  None
===========================================================================*/
static boolean pd_comms_tcp_task_retrieve_root_certificate(
  /*handle id*/  
  uint32 q_handle_id,
  /*Pointer to the root certificate*/
  uint8 **p_cert,
  /*length of the returned root certificate*/
  uint16 *p_length )
{
  if(p_cert ==NULL || p_length == NULL)
  {
    return FALSE;
  }
  
  *p_cert = z_handle_table[q_handle_id].p_ssl_cert;
  *p_length = z_handle_table[q_handle_id].w_ssl_cert_len;

  return TRUE;

}/*pd_comms_tcp_task_retrieve_root_certificate*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_ioctl_set_ssl_cert

DESCRIPTION
  This function handles IOCTL command to set SSL certificate for a TCP/SSL connection

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_handle_ioctl_set_ssl_cert(
  /*ioctl command header*/
  const pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr)  
{
  pd_comms_handle_type z_handle;
  pd_comms_ioctl_ack_params_type z_ioctl_ack;
    
  if(p_hdr_ptr == NULL)
  {
    return;
  }
  
  z_handle = p_hdr_ptr->z_handle;  

  PDCOMM_APP_MSG_HIGH(" Received SET_SSL_CERT ioctl for handle %d(len=%d)",
                        z_handle.q_handle_id,p_hdr_ptr->q_argument_len,0);

  if(z_handle.q_handle_id < PDCOMM_MAX_HANDLES)
  {
    if(p_hdr_ptr->q_argument_len > PDCOMM_MAX_SSL_CERT_SIZE)
    {
      PDCOMM_APP_MSG_ERROR("SSL certificate size[%d] greater than PDCOMM SSL buffer[%d]",
                            p_hdr_ptr->q_argument_len, PDCOMM_MAX_SSL_CERT_SIZE,0); 
    }

	z_handle_table[z_handle.q_handle_id].w_ssl_cert_len =
		memscpy(z_handle_table[z_handle.q_handle_id].p_ssl_cert, PDCOMM_MAX_SSL_CERT_SIZE,
		       (void*)p_hdr_ptr->p_data,p_hdr_ptr->q_argument_len);

    z_ioctl_ack.q_ioctl_request = PDCOMM_IOCTL_CMD_SET_SSL_CERT;
    z_ioctl_ack.q_transaction_id = p_hdr_ptr->q_transaction_id;
    z_ioctl_ack.q_argument_len = 0;
    (void)pd_comms_send_ioctl_ack_to_client(z_handle,p_hdr_ptr->q_source_thread_id,
                                            p_hdr_ptr->q_base_msg_id,&z_ioctl_ack);
  }/*if(z_handle.q_handle_id < PDCOMM_MAX_HANDLES)*/
  else
  {
    pd_comms_notification_params_type z_params;   
        
    z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
    z_params.q_result = (uint32)PDCOMM_IOCTL_ERROR_INVALID_PARAMETERS;
    
    PDCOMM_APP_MSG_HIGH(" Received INVALID handle %d in SET_SSL_CERT ioctl ",
                          z_handle.q_handle_id,0,0);
    (void)pd_comms_notify_client(z_handle,
                           p_hdr_ptr->q_source_thread_id,
                           p_hdr_ptr->q_base_msg_id,
                           p_hdr_ptr->q_transaction_id,
                           ERROR_RPT,&z_params);

  }      
  
} /*pd_comms_tcp_task_handle_ioctl_set_ssl_cert*/

/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_ioctl_set_ssl_cert_dir

DESCRIPTION
  This function handles IOCTL command to set SSL certificate directory for 
  a TCP/SSL connection

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_handle_ioctl_set_ssl_cert_dir(
  /*ioctl command header*/
  const pd_comms_ipc_ioctl_hdr_type* p_hdr_ptr)  
{
  pd_comms_handle_type              z_handle;
  pd_comms_ioctl_ack_params_type    z_ioctl_ack;
  pd_comms_notification_params_type z_params;   
  uint8                             error_code = 0;
    
  if(p_hdr_ptr == NULL)
  {
    return;
  }
  
  do
  {
    z_handle = p_hdr_ptr->z_handle;  

    if(z_handle.q_handle_id >= PDCOMM_MAX_HANDLES)
    {
      error_code = 1;
      break;
    }

    if(p_hdr_ptr->q_argument_len > PDCOMM_MAX_SSL_CERT_DIR_PATH_SIZE)
    {
      error_code = 2;
      break;
    }
    else
    {
      (void) GNSS_STRLCPY(z_handle_table[z_handle.q_handle_id].p_ssl_cert_dir,
                          (void*)p_hdr_ptr->p_data,
                          PDCOMM_MAX_SSL_CERT_DIR_PATH_SIZE); /*lint !e420*/
    }

    z_ioctl_ack.q_ioctl_request  = PDCOMM_IOCTL_CMD_SET_SSL_CERT_DIR;
    z_ioctl_ack.q_transaction_id = p_hdr_ptr->q_transaction_id;
    z_ioctl_ack.q_argument_len   = 0;
    (void)pd_comms_send_ioctl_ack_to_client(z_handle,p_hdr_ptr->q_source_thread_id,
                                              p_hdr_ptr->q_base_msg_id,&z_ioctl_ack);
  }while (0);

  if(error_code != 0)
  {
    z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
    z_params.q_result = (uint32)PDCOMM_IOCTL_ERROR_INVALID_PARAMETERS;
    
    PDCOMM_APP_MSG_HIGH(" Error code %d in SET_SSL_CERT_DIR ioctl ", error_code, 0, 0);
    (void)pd_comms_notify_client(z_handle,
                           p_hdr_ptr->q_source_thread_id,
                           p_hdr_ptr->q_base_msg_id,
                           p_hdr_ptr->q_transaction_id,
                           ERROR_RPT,&z_params);

  }      
  
} /*pd_comms_tcp_task_handle_ioctl_set_ssl_cert_dir*/

/*===========================================================================
FUNCTION pd_comms_tcp_task_secure_tcp_read

DESCRIPTION
  This function attempts to read one or more bytes from a secure TCP/IP socket 

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE/FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean pd_comms_tcp_task_secure_tcp_read(
  /*Handle of link to read from*/
  uint32 q_handle_id,
  /*wait for all requested data to arrive before sending it over to client or not*/ 
  boolean u_wait_for_all )
{
  uint16 w__bytes_read = PDCOMM_READ_DATA_BUF_SIZE;
  
  pd_comms_read_result_type e_read_error = PDCOMM_READ_ERROR_UNKNOWN;
  secerrno_enum_type e_error = E_SUCCESS;
  boolean result = TRUE;

 
  uint8 *p_read_data;
  uint16 *p_num_bytes_to_read;
  uint16 *p_num_bytes_already_read;
  

  p_read_data = &z_handle_table[q_handle_id].p_read_buff[0];
  p_num_bytes_to_read = &z_handle_table[q_handle_id].w_num_bytes_to_read;
  p_num_bytes_already_read = &z_handle_table[q_handle_id].w_num_bytes_already_read;

  /* Retrieve up to max_bytes from the socket */
  do
  {
    w__bytes_read = *p_num_bytes_to_read - *p_num_bytes_already_read;
    e_error = secssl_read( 
              z_handle_table[q_handle_id].z_ssl_handle,
              &p_read_data[*p_num_bytes_already_read],
              &w__bytes_read);
        

    if( e_error != E_SUCCESS )
    {
      switch (e_error)
      {
        case E_WOULD_BLOCK:
        {
          /* No more data available */
          if((*p_num_bytes_already_read == *p_num_bytes_to_read) || (u_wait_for_all == FALSE) )
          {
            pd_comms_read_data_params_type z_params;

            z_params.w_data_len = *p_num_bytes_already_read;
            z_params.p_data = p_read_data;
            PDCOMM_APP_MSG_MED("Received total %d bytes for handle_id [%d] (Expect %d bytes)",
                               *p_num_bytes_already_read,q_handle_id,*p_num_bytes_to_read); 
            (void)pd_comms_senddata_to_client(z_handle_table[q_handle_id].z_handle, 
                                        z_handle_table[q_handle_id].q_source_thread_id,
                                        z_handle_table[q_handle_id].q_base_msg_id,
                                        z_handle_table[q_handle_id].q_read_transaction_id,
                                        &z_params);

            
            *p_num_bytes_already_read = *p_num_bytes_to_read = 0;

            // no more pending READ, can send more READ_DATA event to PDCOMM client
            z_handle_table[q_handle_id].requested_read = FALSE;  
            
          } 
           
          result = TRUE;  
          break;
        }
        
        case E_IN_PROGRESS:
        case E_ALREADY_DONE:
        {
          /* No more data available */
          result = TRUE;
          break;
        }

        default:
        {
          PDCOMM_APP_MSG_ERROR("PDCOMM APP: Secure socket read - Unknown Error: %d for handle_id [%d]",
                    e_error,q_handle_id,0);
          e_read_error = PDCOMM_READ_ERROR_INTERNAL;
          result = FALSE;
          break;
        }
      }

    }/*if( e_error != E_SUCCESS )*/
    else
    {
      result = TRUE;
      
      if(w__bytes_read > 0)
      {
        *p_num_bytes_already_read += w__bytes_read;
        PDCOMM_APP_MSG_MED("Received %d bytes for handle_id [%d]",w__bytes_read,q_handle_id,0);  
       
        if((*p_num_bytes_already_read == *p_num_bytes_to_read) || (u_wait_for_all == FALSE) )
        {
            
          pd_comms_read_data_params_type z_params;

          z_params.w_data_len = *p_num_bytes_already_read;
          z_params.p_data = p_read_data;
          PDCOMM_APP_MSG_MED("Received total %d bytes for handle_id [%d] (Expect %d bytes)",
                             *p_num_bytes_already_read,q_handle_id,*p_num_bytes_to_read); 
          (void)pd_comms_senddata_to_client(z_handle_table[q_handle_id].z_handle, 
                                      z_handle_table[q_handle_id].q_source_thread_id,
                                      z_handle_table[q_handle_id].q_base_msg_id,
                                      z_handle_table[q_handle_id].q_read_transaction_id,
                                      &z_params);

          
          *p_num_bytes_already_read = *p_num_bytes_to_read = 0;

          // no more pending READ, can send more READ_DATA event to PDCOMM client
          z_handle_table[q_handle_id].requested_read = FALSE;  

          result = TRUE;
          e_error = E_FAILURE; /*to break from the do while() loop*/
          
        }
      }/*if(w__bytes_read > 0)*/
      else
      {
        PDCOMM_APP_MSG_ERROR("PDCOMM APP:Received %d bytes for handle_id [%d]",w__bytes_read,q_handle_id,0); 
      }
    }/*else for if( e_error != E_SUCCESS )*/
    
  }
  while( e_error == E_SUCCESS );

  // something is wrong, notify PDCOMM client
  if(result == FALSE)
  {
    pd_comms_notification_params_type z_params;
    z_params.e_new_state = z_handle_table[q_handle_id].e_link_status;

    z_params.q_result = (uint32) e_read_error;
    (void)pd_comms_notify_client(z_handle_table[q_handle_id].z_handle,
                           z_handle_table[q_handle_id].q_source_thread_id,
                           z_handle_table[q_handle_id].q_base_msg_id,
                           z_handle_table[q_handle_id].z_connect_params.q_transaction_id,
                           ERROR_RPT,&z_params);
  }

  return result;

}/*pd_comms_tcp_task_secure_tcp_read*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_ssl_open_callback

DESCRIPTION
  This function is called by the security library to provide status on a previously
  executed open command. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_ssl_open_callback(
  /*Pointer to optional user provided data*/
  void *p_udata,
  /*Error ( or success indication ) that occurred*/
  secerrno_enum_type e_error )
{
  uint32 q_handle_id;

  if( (e_error == E_SUCCESS) || (e_error == E_ALREADY_DONE) || 
      (e_error == E_WOULD_BLOCK) || (e_error == E_IN_PROGRESS) )
  {
    PDCOMM_APP_MSG_MED("SSL Open Request Successful (status=%d)", e_error,0,0);
  }
  else
  {
    PDCOMM_APP_MSG_ERROR("SSL Open Request Failed. Error: %d", e_error, 0, 0 );
    
    
    if(p_udata == NULL)
    {
      PDCOMM_APP_MSG_ERROR("Invalid user data in ssl_event callback",0,0,0);
      return;
    }                                        
    
    q_handle_id = *((uint32*)p_udata);

    if( q_handle_id < PDCOMM_MAX_HANDLES ) 
    {
      pd_comms_tcp_task_secure_tcp_open_cb_ipc_hdr_type* p_hdr_ptr; 
      os_IpcMsgType* p_ipc_ptr;

      PDCOMM_APP_MSG_MED("SSL open callback for handle_id [%d], Error = %d",
                          q_handle_id, (uint32)e_error, 0);

      p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_tcp_task_secure_tcp_open_cb_ipc_hdr_type),
                             IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP);             
      if(p_ipc_ptr == NULL)
      {
        PDCOMM_APP_MSG_HIGH("IPC Create failed during SSL event callback. Handle Id: %d",
                            q_handle_id,0,0);
      }
      else
      {
        /*lint -e826 */  
        p_hdr_ptr = (pd_comms_tcp_task_secure_tcp_open_cb_ipc_hdr_type*) p_ipc_ptr->p_Data;
        /*lint +e826 */ 
        p_ipc_ptr->q_MsgId        = (uint32)PDCOMM_APP_TASK_SEC_OPEN_CB;
        p_ipc_ptr->q_DestThreadId = (uint32)THREAD_ID_SM_PDCOMM_TCP;

        p_hdr_ptr->handle.q_handle_id = q_handle_id;
        p_hdr_ptr->e_error = e_error;

        if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
        {
          PDCOMM_APP_MSG_HIGH("IPC Send failed during SSL event callback. Handle Id: %d",
                              q_handle_id,0,0);
          (void)os_IpcDelete(p_ipc_ptr);
        }
      }

    }
   
  } /*else for if( (e_error == E_SUCCESS) || (e_error == E_ALREADY_DONE) || 
      (e_error == E_WOULD_BLOCK) || (e_error == E_IN_PROGRESS) )*/
  
} /*lint !e818 */
/*pd_comms_tcp_task_ssl_open_callback*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_ssl_close_callback

DESCRIPTION
  This function is called by the security library to provide status on a previously
  executed close command. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_ssl_close_callback(
  /*Pointer to optional user provided data*/
  void * p_udata,
  /*Error ( or success indication ) that occurred*/
  secerrno_enum_type e_error )
{
  uint32 q_handle_id;
  
  if(p_udata == NULL)
  {
    PDCOMM_APP_MSG_ERROR("Invalid user data in ssl_close callback",0,0,0);
    return;
  }                                        
  
  q_handle_id = *((uint32 *)p_udata);

  if( q_handle_id < PDCOMM_MAX_HANDLES ) 
  {
    pd_comms_tcp_task_secure_tcp_close_cb_ipc_hdr_type* p_hdr_ptr; 
    os_IpcMsgType* p_ipc_ptr;

    PDCOMM_APP_MSG_MED("SSL close callback for handle_id [%d], Error = %d",
                        q_handle_id, (uint32)e_error, 0);

    p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_tcp_task_secure_tcp_close_cb_ipc_hdr_type),
                           IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP);             
    if(p_ipc_ptr == NULL)
    {
      PDCOMM_APP_MSG_HIGH("IPC Create failed during SSL close callback. Handle Id: %d",
                          q_handle_id,0,0);
    }
    else
    {
      /*lint -e826 */ 
      p_hdr_ptr = (pd_comms_tcp_task_secure_tcp_close_cb_ipc_hdr_type*) p_ipc_ptr->p_Data;
      /*lint +e826 */
      p_ipc_ptr->q_MsgId        = (uint32)PDCOMM_APP_TASK_SEC_CLOSE_CB;
      p_ipc_ptr->q_DestThreadId = (uint32)THREAD_ID_SM_PDCOMM_TCP;

      p_hdr_ptr->handle.q_handle_id = q_handle_id;
      p_hdr_ptr->e_error = e_error;

      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
      {
        PDCOMM_APP_MSG_HIGH("IPC Send failed during SSL close callback. Handle Id: %d",
                            q_handle_id,0,0);
        (void)os_IpcDelete(p_ipc_ptr);
      }
    }
  }/*if( q_handle_id < PDCOMM_MAX_HANDLES ) */
  else
  {
    PDCOMM_APP_MSG_ERROR( "Got unknown SSL Close callback (handle_id = %d)", q_handle_id, 0, 0);   
  }
} /*lint !e818 */
/*pd_comms_tcp_task_ssl_close_callback*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_ssl_session_cleanup

DESCRIPTION
 This function handles ssl session cleanup (close and delete)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_ssl_session_cleanup(
  /*handle for which connection is to be cleaned up*/
  pd_comms_handle_type z_handle,
  boolean bCloseSession)
{
  boolean bDeleteSession = TRUE;

  if (bCloseSession)
  {
    secerrno_enum_type e_error;

    PDCOMM_APP_MSG_MED("SSL Closing session for handle_id [%d]",z_handle.q_handle_id,0,0);

    /* Close SSL Session */
    e_error = secssl_close_session( z_handle_table[z_handle.q_handle_id].z_ssl_handle,
                                  pd_comms_tcp_task_ssl_close_callback, 
                                  &z_handle_table[z_handle.q_handle_id].z_handle.q_handle_id);
    if (e_error != E_SUCCESS )
    {
      if (e_error!= E_ALREADY_DONE)
      {
        PDCOMM_APP_MSG_ERROR("PDCOMM APP: Could not close SSL session. Error: %d for handle_id [%d]",
                (uint32)e_error, z_handle.q_handle_id, 0);    
      }
      // close_session() fails, will continue to delete session
    }  
    else
    {
      // if e_error is E_SUCCESS, will do rest of cleanup in pd_comms_tcp_task_handle_sec_close_cb()
      bDeleteSession = FALSE;
    }

  }  /* if (bCloseSession) */

  if (bDeleteSession)
  {
    if (z_handle_table[z_handle.q_handle_id].z_ssl_handle != NULL)
    {
      (void)secssl_delete_session( z_handle_table[z_handle.q_handle_id].z_ssl_handle );

      z_handle_table[z_handle.q_handle_id].z_ssl_handle = NULL;

    }

    /* disconnect external socket */
    (void)pd_comms_tcp_task_teardown_tcp(z_handle); 
  } /* if (bDeleteSession) */

}/*pd_comms_tcp_task_ssl_session_cleanup*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_ssl_alert_callback

DESCRIPTION
  This function is called by the security library to indicate a security related
  alert has occurred. If necessary this function will notify the application of the
  security alert. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_ssl_alert_callback(  
  /*Pointer to optional user provided data*/  
  void                              * p_udata,
  /*Type of alert being reported*/
  secssl_ses_alert_level_enum_type  alert_level,
  /*Bit mask containing the sum of all of the current alerts*/
  secssl_ses_alert_mask_enum_type   alert_mask
)
{

  boolean u_closeConnection = FALSE;
  uint32 q_handle_id;
  
  if(p_udata == NULL)
  {
    PDCOMM_APP_MSG_ERROR("Invalid user data in ssl_alert callback",0,0,0);
    return;
  }                                        
  
  q_handle_id = *((uint32*)p_udata);
   
  switch( alert_level )
  {
    case SECSSL_ALERT_LEVEL_WARN:
    {
      PDCOMM_APP_MSG_MED(" SSL Alert Received. Level: Warning",0,0,0);
      break;
    }

    case SECSSL_ALERT_LEVEL_INFO:
    {
      PDCOMM_APP_MSG_MED(" SSL Alert Received. Level: Info",0,0,0);
      break;
    }

    case SECSSL_ALERT_LEVEL_FATAL:
    {
      PDCOMM_APP_MSG_ERROR(" SSL Alert Received. Level: Fatal",0,0,0);
      u_closeConnection = TRUE;
      break;
    }

    case SECSSL_ALERT_LEVEL_SUSPEND:
    {
      PDCOMM_APP_MSG_ERROR(" SSL Alert Received. Level: Suspend",0,0,0);
      u_closeConnection = TRUE;
      break;
    }

    default:
      break;
  }/*switch( alert_level )*/

  PDCOMM_APP_MSG_MED(" Alerts Received:",0,0,0);
/*lint -e655 */
  if( alert_mask == SECSSL_ALERT_CLOSE_NOTIFY )
  {
    PDCOMM_APP_MSG_MED("          CLOSE NOTIFY",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_UNEXPECTED_MESSAGE )
  {
    PDCOMM_APP_MSG_MED("          UNEXPECTED MESSAGE",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_BAD_RECORD_MAC )
  {
    PDCOMM_APP_MSG_MED("          BAD RECORD MAC",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_DECRYPTION_FAILED )
  {
    PDCOMM_APP_MSG_MED("          DECRYPTION FAILED",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_RECORD_OVERFLOW )
  {
    PDCOMM_APP_MSG_MED("          RECORD OVERFLOW",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_DECOMPRESSION_FAILURE )
  {
    PDCOMM_APP_MSG_MED("          DECOMPRESSION FAILURE",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_HANDSHAKE_FAILURE )
  {
    PDCOMM_APP_MSG_MED("          HANDSHAKE FAILURE",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_NO_CERTIFICATE )
  {
    PDCOMM_APP_MSG_MED("          NO CERTIFICATE",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_BAD_CERTIFICATE )
  {
    PDCOMM_APP_MSG_MED("          BAD CERTIFICATE",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_BAD_CERTIFICATE_SIGN )
  {
    PDCOMM_APP_MSG_MED("          BAD CERTIFICATE SIGN",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_BAD_CERTIFICATE_ISSUER )
  {
    PDCOMM_APP_MSG_MED("          BAD CERTIFICATE ISSUER",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_UNSUPPORTED_CERTIFICATE )
  {
    PDCOMM_APP_MSG_MED("          UNSUPPORTED CERTIFICATE",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_CERTIFICATE_REVOKED )
  {
    PDCOMM_APP_MSG_MED("          CERTIFICATE REVOKED",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_CERTIFICATE_EXPIRED )
  {
    PDCOMM_APP_MSG_MED("          CERTIFICATE EXPIRED",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_CERTIFICATE_UNKNOWN )
  {
    PDCOMM_APP_MSG_MED("          CERTIFICATE UNKNOWN",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_ILLEGAL_PARAMETER )
  {
    PDCOMM_APP_MSG_MED("          ILLEGAL PARAMETER",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_INVALID )
  {
    PDCOMM_APP_MSG_MED("          INVALID",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_UNKNOWN_CA )
  {
    PDCOMM_APP_MSG_MED("          UNKNOWN CA",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_ACCESS_DENIED )
  {
    PDCOMM_APP_MSG_MED("          ACCESS DENIED",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_DECODE_ERROR )
  {
    PDCOMM_APP_MSG_MED("          DECODE ERROR",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_DECRYPT_ERROR )
  {
    PDCOMM_APP_MSG_MED("          DECRYPT ERROR",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_EXPORT_RESTRICTION )
  {
    PDCOMM_APP_MSG_MED("          EXPORT RESTRICTION",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_PROTOCOL_VERSION )
  {
    PDCOMM_APP_MSG_MED("          PROTOCOL VERSION",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_INSUFFICIENT_SECURITY )
  {
    PDCOMM_APP_MSG_MED("          INSUFFICIENT SECURITY",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_INTERNAL_ERROR )
  {
    PDCOMM_APP_MSG_MED("          INTERNAL ERROR",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_USER_CANCELED )
  {
    PDCOMM_APP_MSG_MED("          USER CANCELLED",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_NO_RENEGOTIATION )
  {
    PDCOMM_APP_MSG_MED("          NO RENOGOTIATION",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_HOST_MISMATCH )
  {
    PDCOMM_APP_MSG_MED("          HOST MISMATCH",0,0,0);
  }

  if( alert_mask & SECSSL_ALERT_CERT_INFO )
  {
    PDCOMM_APP_MSG_MED("          CERT INFO",0,0,0);
  }

  if(u_closeConnection)
  {
    if(z_handle_table[q_handle_id].e_link_status != PDCOMM_STATE_DISCONNECTING )
    {
      pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                     PDCOMM_STATE_DISCONNECTING,
                                     TRUE,(uint32)PDCOMM_RESULT_WAIT);
    }

    // close & delete session
    pd_comms_tcp_task_ssl_session_cleanup(z_handle_table[q_handle_id].z_handle, TRUE);  
  }/*if(u_closeConnection)*/

/*lint +e655 */
} /*lint !e715 !e818 */
/*pd_comms_tcp_task_ssl_alert_callback*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_ssl_event_callback

DESCRIPTION
  This function is called by the security library to indicate a security related
  event has occurred. If necessary this function will notify the application of the
  security event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_ssl_event_callback(
  /*Session handle for which the event occurred*/  
  secssl_handle_type                z_session_handle,
  /*Event that occurred*/
  secssl_ses_event_enum_type        e_event,
  /*Pointer to optional user provided data*/
  void                              *p_udata
)
{
  uint32 q_handle_id;
  
  if(p_udata == NULL)
  {
    PDCOMM_APP_MSG_ERROR("Invalid user data in ssl_event callback",0,0,0);
    return;
  }                                        
  
  q_handle_id = *((uint32*)p_udata);

  if( q_handle_id < PDCOMM_MAX_HANDLES ) 
  {
    pd_comms_tcp_task_secure_tcp_cb_ipc_hdr_type* p_hdr_ptr; 
    os_IpcMsgType* p_ipc_ptr;

    PDCOMM_APP_MSG_MED("SSL event callback for handle_id [%d], Event = %d",
                        q_handle_id, (uint32)e_event, 0);

    p_ipc_ptr = os_IpcCreate(sizeof(pd_comms_tcp_task_secure_tcp_cb_ipc_hdr_type),
                           IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_PDCOMM_TCP);             
    if(p_ipc_ptr == NULL)
    {
      PDCOMM_APP_MSG_HIGH("IPC Create failed during SSL event callback. Handle Id: %d",
                          q_handle_id,0,0);
    }
    else
    {
      /*lint -e826 */ 
      p_hdr_ptr = (pd_comms_tcp_task_secure_tcp_cb_ipc_hdr_type*) p_ipc_ptr->p_Data;
      /*lint +e826 */
      p_ipc_ptr->q_MsgId        = (uint32)PDCOMM_APP_TASK_SEC_EVENT_CB;
      p_ipc_ptr->q_DestThreadId = (uint32)THREAD_ID_SM_PDCOMM_TCP;

      p_hdr_ptr->handle.q_handle_id = q_handle_id;
      p_hdr_ptr->event_mask = e_event;

      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_PDCOMM_TCP))
      {
        PDCOMM_APP_MSG_HIGH("IPC Send failed during SSL event callback. Handle Id: %d",
                            q_handle_id,0,0);
        (void)os_IpcDelete(p_ipc_ptr);
      }
    }
  }/*if( q_handle_id < PDCOMM_MAX_HANDLES ) */
  else
  {
    PDCOMM_APP_MSG_ERROR( "Got unknown SSL event callback (handle_id = %d)", q_handle_id, 0, 0);   
  }

} /*lint !e715 !e818 */
/*pd_comms_tcp_task_ssl_event_callback*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_ssl_errno_callback

DESCRIPTION
  This function is called by the security library to provide status on a previously
  executed command. Currently only secssl_write() and secssl_read() will call this
  callback. For read, no callback for SUCCESS or WOULD_BLOCK, only for errors. For
  write, callback will be called for both SUCCESS and FAILURE case.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void pd_comms_tcp_task_ssl_errno_callback(
  /*Pointer to optional user provided data*/
  void *p_udata,
  /*Error ( or success indication ) that occurred*/
  secerrno_enum_type e_error )
{
  if( e_error == E_SUCCESS )
  {
    if (p_udata != NULL)
    {
      uint32 q_handle_id = *(uint32*)p_udata;
      PDCOMM_APP_MSG_MED("SSL Operation Successful, handle=%d", q_handle_id,0,0);

      if (q_handle_id < PDCOMM_MAX_HANDLES &&
          z_handle_table[q_handle_id].in_use == TRUE &&
          z_handle_table[q_handle_id].z_ssl_handle != NULL &&
          z_handle_table[q_handle_id].e_link_status != PDCOMM_STATE_DISCONNECTED &&
          z_handle_table[q_handle_id].w_num_bytes_to_write > 0)
      {
        // there is a pending write, this is SSL's Write Ack (success)
        pd_comms_write_ack_params_type z_params;
    
        z_params.q_bytes_written = z_handle_table[q_handle_id].w_num_bytes_to_write;
        z_params.q_transaction_id = z_handle_table[q_handle_id].q_write_transaction_id;

        PDCOMM_APP_MSG_MED("Got SSL Write Ack for %d bytes (handle=%d)", 
                           z_handle_table[q_handle_id].w_num_bytes_to_write, q_handle_id,0);

        // reset the value, so PENDING write is done
        z_handle_table[q_handle_id].w_num_bytes_written = 0;
        z_handle_table[q_handle_id].w_num_bytes_to_write = 0;

        /* Acknowledge the write */
        (void)pd_comms_send_write_ack_to_client(z_handle_table[q_handle_id].z_handle,
                                      z_handle_table[q_handle_id].q_source_thread_id,
                                      z_handle_table[q_handle_id].q_base_msg_id,
                                      &z_params);
      }
    }
    else PDCOMM_APP_MSG_MED("SSL Operation Successful",0,0,0);
  }
  else
  {
    PDCOMM_APP_MSG_ERROR("SSL Operation Failed. Error: %d", e_error, 0, 0 );
    
  }
} /*lint !e715 !e818 */
/*pd_comms_tcp_task_ssl_errno_callback*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_sec_event_cb

DESCRIPTION
  Whenever there is a Security event callback, an IPC message is sent to 
  PDCOMM APP. This IPC message is handled in this function. Based upon the i
  received event_mask it takes the appropriate action 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_handle_sec_event_cb(
  /*Pointer to IPC message containing the security callback information */
  const os_IpcMsgType* p_msg )
{
  secssl_ses_event_enum_type e_event;
  uint32 q_handle_id;

  if(p_msg == NULL)
  {
    return;
  }
     
  /*lint -e826 */ 
  q_handle_id = ((pd_comms_tcp_task_secure_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->handle.q_handle_id;
  PDCOMM_APP_MSG_MED("Received SOCKET callback for handle_id [%d] with event: %d",
                      q_handle_id,
                     (uint32)((pd_comms_tcp_task_secure_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->event_mask,0);
  /*lint +e826 */ 

  if( q_handle_id < PDCOMM_MAX_HANDLES )
  {
    pd_comms_tcp_task_reset_inactivity_count(q_handle_id);

    /*lint -e826 */ 
    e_event = ((pd_comms_tcp_task_secure_tcp_cb_ipc_hdr_type*)p_msg->p_Data)->event_mask;
    /*lint +e826 */ 
    switch( e_event )
    {
      case SECSSL_SES_SOCK_READ_EVENT:
      {
        PDCOMM_APP_MSG_MED("SSL Socket READ event for handle_id [%d]",q_handle_id,0,0);  
        if( z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_CONNECTED )
        {
         
          if(z_handle_table[q_handle_id].w_num_bytes_already_read <
             z_handle_table[q_handle_id].w_num_bytes_to_read)
          {
           /*If a previous buffered read is pending, continue with that*/   
            (void) pd_comms_tcp_task_secure_tcp_read(q_handle_id, TRUE); // u_wait_for_all=TRUE
          }
          else 
          {
            // if we have not sent READ_DATA since last read operation, send it now
            if (z_handle_table[q_handle_id].requested_read == FALSE)  
            {
              /*Notify client about the read data availability*/  
              pd_comms_notification_params_type z_params;
              pd_comms_return_type e_result;
            
              z_params.e_new_state = z_handle_table[q_handle_id].e_link_status;
              e_result = pd_comms_notify_client(z_handle_table[q_handle_id].z_handle,
                         z_handle_table[q_handle_id].q_source_thread_id,
                         z_handle_table[q_handle_id].q_base_msg_id,
                         z_handle_table[q_handle_id].z_connect_params.q_transaction_id, 
                         READ_DATA,&z_params);
            
              if(e_result != PDCOMM_RESULT_OK)
              {
                PDCOMM_APP_MSG_ERROR("PDCOMM APP: IPC error %d for handle_id [%d]",
                          (uint32)e_result,q_handle_id,0);
              }    
              else
              {
                // successfully sent READ_DATA event to PDCOMM client to request read
                z_handle_table[q_handle_id].requested_read = TRUE;   
              }
            } /*if (z_handle_table[q_handle_id].requested_read == FALSE)*/

          }
        }/*if( z_handle_table[q_handle_id].e_link_status == PDCOMM_STATE_CONNECTED )*/
        else
        {
          PDCOMM_APP_MSG_ERROR("PDCOMM APP: SSL READ event while link is DISCONNECTED for handle_id [%d]",
                    q_handle_id,0,0);
        }
        break;
      }/*case SECSSL_SES_SOCK_READ_EVENT*/

      case SECSSL_SES_SOCK_CLOSE_EVENT:
      {
        PDCOMM_APP_MSG_MED("SSL Socket Closed for handle_id [%d]",q_handle_id,0,0);

        if(z_handle_table[q_handle_id].e_link_status != PDCOMM_STATE_DISCONNECTING )
        {
          pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                         PDCOMM_STATE_DISCONNECTING,
                                         TRUE,(uint32)PDCOMM_RESULT_WAIT);
        }

        // close & delete session
        pd_comms_tcp_task_ssl_session_cleanup(z_handle_table[q_handle_id].z_handle, TRUE);
        break;
      }/*case SECSSL_SES_SOCK_CLOSE_EVENT*/

      case SECSSL_SES_DNS_START_EVENT:
      {
        PDCOMM_APP_MSG_MED("SSL Attempting to resolve hostname for handle_id [%d]",
                           q_handle_id,0,0);
        break;
      }/*case SECSSL_SES_DNS_START_EVENT*/

      case SECSSL_SES_DNS_SUCCESS_EVENT:
      {
        PDCOMM_APP_MSG_MED("SSL hostname successfully resolved for handle_id [%d]",
                           q_handle_id,0,0);
        break;
      }/*case SECSSL_SES_DNS_SUCCESS_EVENT*/

      case SECSSL_SES_DNS_FAILURE_EVENT:
      {
        PDCOMM_APP_MSG_ERROR("SSL DNS Lookup Failed for handle_id [%d]",
                  q_handle_id,0,0);

        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                         PDCOMM_STATE_DISCONNECTING,
                                         TRUE,(uint32)PDCOMM_CONNECT_ERROR_DNS_LOOKUP_FAILED);

        // close & delete session
        pd_comms_tcp_task_ssl_session_cleanup(z_handle_table[q_handle_id].z_handle, TRUE);

        break;
      }/*case SECSSL_SES_DNS_FAILURE_EVENT*/

      case SECSSL_SES_CONNECT_START_EVENT:
      {
        PDCOMM_APP_MSG_MED("SSL Socket Connection Starting for handle_id [%d]",
                           q_handle_id,0,0);
        break;
      }/*case SECSSL_SES_CONNECT_START_EVENT*/

      case SECSSL_SES_CONNECT_SUCCESS_EVENT:
      {
        PDCOMM_APP_MSG_MED("SSL Socket Connected for handle_id [%d]",
                           q_handle_id,0,0);
        break;
      }/*case SECSSL_SES_CONNECT_SUCCESS_EVENT*/

      case SECSSL_SES_CONNECT_FAILURE_EVENT:
      {
        PDCOMM_APP_MSG_ERROR("SSL Socket Connection Failed for handle_id [%d]",
                  q_handle_id,0,0);

        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                         PDCOMM_STATE_DISCONNECTING,
                                         TRUE,(uint32)PDCOMM_CONNECT_ERROR_UNKNOWN);

        // close & delete session
        pd_comms_tcp_task_ssl_session_cleanup(z_handle_table[q_handle_id].z_handle, TRUE);

        break;
      }/*case SECSSL_SES_CONNECT_FAILURE_EVENT*/

      case SECSSL_SES_AUTH_START_EVENT:
      {
        PDCOMM_APP_MSG_MED("SSL Authentication Starting for handle_id [%d]",
                           q_handle_id,0,0);
        break;
      }/*case SECSSL_SES_AUTH_START_EVENT*/

      case SECSSL_SES_AUTH_SUCCESS_EVENT:
      {
        PDCOMM_APP_MSG_MED("SSL Authentication Successful for handle_id [%d]",
                           q_handle_id,0,0);
        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                         PDCOMM_STATE_CONNECTED,
                                         TRUE,(uint32)PDCOMM_RESULT_OK);
        break;
      }/*case SECSSL_SES_AUTH_SUCCESS_EVENT*/

      case SECSSL_SES_AUTH_FAILURE_EVENT:
      {
        /* Set the socket linger time to be 0 secs so that the TCP block does not remain in
           the TIME-WAIT state for 30 secs*/
        //pd_comms_tcp_task_setsockopt(q_handle_id); // FIXME

        PDCOMM_APP_MSG_ERROR("SSL Authentication Failure",0,0,0);

        pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle,
                                         PDCOMM_STATE_DISCONNECTING,
                                         TRUE,(uint32)PDCOMM_CONNECT_ERROR_SECURITY_FAILURE);

        pd_comms_tcp_task_ssl_session_cleanup(z_handle_table[q_handle_id].z_handle, TRUE);

        break;
      }/*case SECSSL_SES_AUTH_FAILURE_EVENT*/

      default:
      {
        PDCOMM_APP_MSG_ERROR("Unknown SSL Event: %d for handle_id [%d]",
                   e_event, q_handle_id, 0);
        break;
      }
    }/*switch( e_event )*/

  }/*if( q_handle_id < PDCOMM_MAX_HANDLES )*/
  else
  {
    PDCOMM_APP_MSG_ERROR("SSL event callback: handle_id %d out of range",
               q_handle_id, 0, 0);
  }

}/*pd_comms_tcp_task_handle_sec_event_cb*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_sec_open_cb

DESCRIPTION
  Whenever there is a Security open callback, an IPC message is sent to 
  PDCOMM APP. This IPC message is handled in this function. Based upon the i
  received event_mask it takes the appropriate action. An IPC message will be sent
  out for security open callback only if an open error has occured.  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_handle_sec_open_cb(
  /*Pointer to IPC message containing the security callback information */
  const os_IpcMsgType* p_msg )
{
  uint32 q_handle_id;
  pd_comms_connect_result_type z_connect_error;
  
  if(p_msg == NULL)
  {
    PDCOMM_APP_MSG_ERROR("NULL IPC received in security_open callback",0,0,0);
    return;
  }
  
  /*lint -e826 */ 
  q_handle_id = ((pd_comms_tcp_task_secure_tcp_open_cb_ipc_hdr_type*)p_msg->p_Data)->handle.q_handle_id;
  /*lint +e826 */ 

  PDCOMM_APP_MSG_MED("SEC_OPEN callback received for handle_id[%d]",q_handle_id,0,0);
  
  if( q_handle_id < PDCOMM_MAX_HANDLES )
  {
    PDCOMM_APP_MSG_MED("Received security open callback for handle_id [%d] with error: %d link_state[%d]",
                        q_handle_id,
                        (uint32)((pd_comms_tcp_task_secure_tcp_open_cb_ipc_hdr_type*)p_msg->p_Data)->e_error,
                        z_handle_table[q_handle_id].e_link_status); /*lint !e826*/

    if((z_handle_table[q_handle_id].e_link_status != PDCOMM_STATE_DISCONNECTING) &&
       (z_handle_table[q_handle_id].e_link_status != PDCOMM_STATE_DISCONNECTED))
    {
      z_connect_error = PDCOMM_CONNECT_ERROR_NETWORK_LIB_INIT_FAILURE; 
      pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle, PDCOMM_STATE_DISCONNECTING,
                                     TRUE,(uint32)z_connect_error);  

      pd_comms_tcp_task_ssl_session_cleanup(z_handle_table[q_handle_id].z_handle, FALSE); // just delete session
    }
  }
  else
  {
    PDCOMM_APP_MSG_ERROR("SSL open callback: handle_id %d out of range",
               q_handle_id, 0, 0);
  }
}/*pd_comms_tcp_task_handle_sec_open_cb*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_sec_close_cb

DESCRIPTION
  Whenever there is a Security close callback, an IPC message is sent to 
  PDCOMM APP. This IPC message is handled in this function. Based upon the 
  received event_mask it takes the appropriate action. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_handle_sec_close_cb(
  /*Pointer to IPC message containing the security callback information */
  const os_IpcMsgType* p_msg )
{
  secerrno_enum_type e_error;
  uint32 q_handle_id;
  
  if(p_msg == NULL)
  {
    PDCOMM_APP_MSG_ERROR("NULL IPC received in security_close callback",0,0,0);
    return;
  }
  
  /*lint -e826 */ 
  q_handle_id = ((pd_comms_tcp_task_secure_tcp_close_cb_ipc_hdr_type*)p_msg->p_Data)->handle.q_handle_id;
  e_error = ((pd_comms_tcp_task_secure_tcp_close_cb_ipc_hdr_type*)p_msg->p_Data)->e_error;
  PDCOMM_APP_MSG_MED("Received security close callback for handle_id [%d] with error: %d",
                      q_handle_id,
                     (uint32)e_error,0);
  /*lint +e826 */ 

  if( q_handle_id < PDCOMM_MAX_HANDLES )
  {
    if( e_error == E_SUCCESS )
    {
      PDCOMM_APP_MSG_MED("SSL Close Request Successful for handle_id [%d]",
                        q_handle_id,0,0);
    }
    else
    {
      PDCOMM_APP_MSG_ERROR(" SSL Close Request Failed. Error: %d for handle_id [%d]",
                e_error, q_handle_id, 0 );
    }

    /*In any case delete the existing SSL session*/
    pd_comms_tcp_task_ssl_session_cleanup(z_handle_table[q_handle_id].z_handle, FALSE); // just delete session

  }
  else
  {
    PDCOMM_APP_MSG_ERROR("SSL close callback: handle_id %d out of range",
               q_handle_id, 0, 0);
  }
} /*pd_comms_tcp_task_handle_sec_close_cb*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_handle_sec_close_net_cb

DESCRIPTION
  Whenever there is a Security close network callback, an IPC message is sent to 
  PDCOMM APP. This IPC message is handled in this function. Based upon the 
  received event_mask it takes the appropriate action. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_handle_sec_close_net_cb(
  /*Pointer to IPC message containing the security callback information */
  const os_IpcMsgType* p_msg )
{
  secerrno_enum_type e_error;
  uint32 q_handle_id;
  uint32 e_disc_error;
  
  if(p_msg == NULL)
  {
    PDCOMM_APP_MSG_ERROR("NULL IPC received in security_close callback",0,0,0);
    return;
  }
  /*lint -e826 */    
  q_handle_id = ((pd_comms_tcp_task_secure_tcp_close_net_cb_ipc_hdr_type*)p_msg->p_Data)->handle.q_handle_id;
  e_error = ((pd_comms_tcp_task_secure_tcp_close_net_cb_ipc_hdr_type*)p_msg->p_Data)->e_error;
  PDCOMM_APP_MSG_MED("Received security close network callback for handle_id [%d] with error: %d",
                      q_handle_id,
                     (uint32)e_error,0);
  /*lint +e826 */   
  if( q_handle_id < PDCOMM_MAX_HANDLES )
  {
    if( e_error == E_SUCCESS )
    {
      PDCOMM_APP_MSG_MED("SSL Close network Request Successful for handle_id [%d]",
                        q_handle_id,0,0);
      e_disc_error = (uint32)PDCOMM_RESULT_OK;
    }
    else
    {
      PDCOMM_APP_MSG_ERROR(" SSL Close network Request Failed. Error: %d for handle_id [%d]",
                e_error, q_handle_id, 0 );
      e_disc_error = (uint32)PDCOMM_DISCONNECT_ERROR_UNKNOWN;
    }

    pd_comms_tcp_task_update_state(z_handle_table[q_handle_id].z_handle, 
                                   PDCOMM_STATE_DISCONNECTED,!z_handle_table[q_handle_id].session_aborted, /*lint !e730*/
                                   (uint32)e_disc_error);
    
      
  }/*if( q_handle_id < PDCOMM_MAX_HANDLES )*/
  else
  {
    PDCOMM_APP_MSG_ERROR("SSL close callback: handle_id %d out of range",
               q_handle_id, 0, 0);
  }
} /*pd_comms_tcp_task_handle_sec_close_net_cb*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_set_ssl_params

DESCRIPTION
  This function opens a SSL session for an existing socket connection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean pd_comms_tcp_task_set_ssl_params(
  /*handle for which connection is to be opened*/
  pd_comms_handle_type z_handle)
{
  secerrno_enum_type e_error;
  secssl_ses_parameter_pdata_union_type z_ssl_params;
  secssl_handle_type *p_ssl_handle = &z_handle_table[z_handle.q_handle_id].z_ssl_handle;

  if(z_handle_table[z_handle.q_handle_id].session_handle == GPSONE_BIT_INVALID_SESSION_HANDLE)
  {
    PDCOMM_APP_MSG_ERROR("Invalid session_handle found while setting SSL params",0,0,0);  
    return FALSE;
  }
  
  /*Set the external socket for secure connections*/
  // will set SSL's server_socket & set socket_is_external=TRUE
  z_ssl_params.socket_fd_pdata = (sint15)(z_handle.q_handle_id); 
  e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_SOCKET_FD, &z_ssl_params );
  if( e_error != E_SUCCESS )
  {
    PDCOMM_APP_MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter SOCKET_FD", e_error, 0, 0);
    return FALSE;
  }

  // Setting BIT function pointers
  z_ssl_params.connection_fptr_pdata.rx_fptr = gpsone_bit_read_dsm_chain; // seclsslrx.c's secsslrx_read_v3_record() will call
  z_ssl_params.connection_fptr_pdata.tx_fptr = gpsone_bit_write_dsm_chain;  // selssltx.c's secssltx_send_pending() will call
  z_ssl_params.connection_fptr_pdata.select_fptr = gpsone_bit_async_select;
  z_ssl_params.connection_fptr_pdata.deselect_fptr = gpsone_bit_async_deselect;
  z_ssl_params.connection_fptr_pdata.next_event_fptr = gpsone_bit_getnextevent; 
  e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_CONNECTION_FPTRS, &z_ssl_params );
  if( e_error != E_SUCCESS )
  {
    PDCOMM_APP_MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter CONNECTION_FPTRS", e_error, 0, 0);
    return FALSE;
  }

  /*Ignore certificate expiration check*/
  z_ssl_params.ignore_expiration_pdata = TRUE;
  e_error = secssl_set_sesparam( *p_ssl_handle,  SECSSL_SES_IGNORE_EXPIRATION, &z_ssl_params );

  if( e_error != E_SUCCESS )
  {
    MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter to ignore certificate expiration check", e_error, 0, 0);
    return FALSE;
  }

  /* Set up TLS parameters */

  /*Set server's HOST_NAME*/  
  if(z_handle_table[z_handle.q_handle_id].z_connect_params.z_link_params.z_tcp_params.e_type == PD_COMMS_IP_ADDR)
  {
    PDCOMM_APP_MSG_ERROR("Unable to set SSL HOST_NAME. None provided",0,0,0);
    return FALSE;  
  }
  
  z_ssl_params.server_pdata.host_type = SECSSL_HOST_NAME;
  z_ssl_params.server_pdata.addr.host_name = 
  z_handle_table[z_handle.q_handle_id].z_connect_params.z_link_params.z_tcp_params.p_host_name;

  z_ssl_params.server_pdata.port = 
  dss_htons(z_handle_table[z_handle.q_handle_id].z_connect_params.z_link_params.z_tcp_params.w_ip_port);

  if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_SERVER, &z_ssl_params ) )
       != E_SUCCESS )
  {
    PDCOMM_APP_MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter", e_error, 0, 0);
    return FALSE;
  }

  /* SSL Version */
  z_ssl_params.ssl_version_pdata = SECSSL_SSL_VER_TLS10;
  if(z_handle_table[z_handle.q_handle_id].z_connect_params.e_ssl_ver_tls == SSL_VER_TLS_1_1)
  {
    z_ssl_params.ssl_version_pdata = SECSSL_SSL_VER_TLS11;
  }  
  MSG_HIGH("TLS version being used is 0x%x", z_ssl_params.ssl_version_pdata,0,0);

  if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_SSL_VERSION, &z_ssl_params ) )
       != E_SUCCESS )
  {
    PDCOMM_APP_MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter", e_error, 0, 0);
    return FALSE;
  }

  /* Inject Root Certificate info. Inject either a cert dir or a single cert */
  if( z_handle_table[z_handle.q_handle_id].p_ssl_cert_dir[0] == '\0' )
  {
    MSG_MED("PDCOMM APP: No Root Cert Dir", 0, 0, 0);
    /* See if a single Root Certificate can be injected */
    if( !pd_comms_tcp_task_retrieve_root_certificate( 
         z_handle.q_handle_id,
         (uint8 **)&z_ssl_params.ca_certificate_pdata.cert_data,
         &z_ssl_params.ca_certificate_pdata.cert_len ) )
    {
      MSG_ERROR("PDCOMM APP: Error Retrieving Root Certificate", 0, 0, 0);
      return FALSE;
    }
    if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_CA_CERTIFICATE, &z_ssl_params ) )
         != E_SUCCESS )
    {
      MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter", e_error, 0, 0);
      return FALSE;
    }
  }
  else
  {
    z_ssl_params.ca_cert_folder_pdata.session_ca_cert_folder = 
      z_handle_table[z_handle.q_handle_id].p_ssl_cert_dir;
  
    if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_CA_CERT_FOLDER, &z_ssl_params ) )
         != E_SUCCESS )
    {
      MSG_ERROR("PDCOMM APP: Error: %d setting SSL cert folder", e_error, 0, 0);
      return FALSE;
    }
  }

  /* Alert Callback */
  z_ssl_params.alert_cb_pdata.mode = SECSSL_SES_ALERT_MODE_AUTO;
  z_ssl_params.alert_cb_pdata.func_ptr = pd_comms_tcp_task_ssl_alert_callback;

  if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_ALERT_CB, &z_ssl_params ) )
       != E_SUCCESS )
  {
    PDCOMM_APP_MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter", e_error, 0, 0);
    return FALSE;
  }

  /* Event Callback */
  z_ssl_params.event_cb_pdata = pd_comms_tcp_task_ssl_event_callback;
  
  if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_EVENT_CB, &z_ssl_params ) )
       != E_SUCCESS )
  {
    PDCOMM_APP_MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter", e_error, 0, 0);
    return FALSE;
  }

  /* Errno Callback */
  z_ssl_params.errno_cb_pdata.func_ptr = pd_comms_tcp_task_ssl_errno_callback;
  z_ssl_params.errno_cb_pdata.udata_ptr = (void*)&z_handle_table[z_handle.q_handle_id].z_handle.q_handle_id;
  
  if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_ERRNO_CB, &z_ssl_params ) )
       != E_SUCCESS )
  {
    PDCOMM_APP_MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter", e_error, 0, 0);
    return FALSE;
  }

  /* Data Services App ID */
  /*z_ssl_params.ds_app_id_pdata = z_handle_table[z_handle.q_handle_id].x_app_id;*/
  /*
  z_ssl_params.ds_app_id_pdata = 0; // no need for this, it's for DSS only
  if( ( e_error = secssl_set_sesparam( *p_ssl_handle, SECSSL_SES_DS_APP_ID, &z_ssl_params ) )
       != E_SUCCESS )
  {
    PDCOMM_APP_MSG_ERROR("PDCOMM APP: Error: %d setting SSL parameter", e_error, 0, 0);
    return FALSE;
  }
  */

  return TRUE;
  
}/*pd_comms_tcp_task_set_ssl_params*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_open_ssl

DESCRIPTION
  This function opens a SSL session for an existing socket connection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_open_ssl(
  /*handle for which connection is to be opened*/
  pd_comms_handle_type z_handle)
{
  secerrno_enum_type e_error;
  secssl_handle_type *p_ssl_handle = &z_handle_table[z_handle.q_handle_id].z_ssl_handle;

  /* Create a new TLS session instance */
  if( ( e_error = secssl_new_session( p_ssl_handle ) ) != E_SUCCESS )
  {
    PDCOMM_APP_MSG_ERROR("Could not create new SSL session. Error: %d", e_error, 0, 0);
    (void) pd_comms_tcp_task_teardown_tcp(z_handle);
    return;
  }

  /*Set all the parameters for this SSL session*/
  if(!pd_comms_tcp_task_set_ssl_params(z_handle))
  {
    PDCOMM_APP_MSG_ERROR("Error setting SSL params. Deleting SSL session..",0,0,0); 
    pd_comms_tcp_task_ssl_session_cleanup(z_handle, FALSE); // just delete session
    return;
  }
  
  /* Attempt to open the connection */
  PDCOMM_APP_MSG_MED("Opening SSL Connection for handle_id [%d]",z_handle.q_handle_id,0,0);
  e_error = secssl_open_session( *p_ssl_handle, pd_comms_tcp_task_ssl_open_callback, 
                                 &z_handle_table[z_handle.q_handle_id].z_handle.q_handle_id );

  switch ( e_error )
  {
    case E_ALREADY_DONE:
    {
      /* Opened Successfully */
      PDCOMM_APP_MSG_MED("SSL Connection Established for handle_id [%d]",
                         z_handle.q_handle_id,0,0);
      pd_comms_tcp_task_update_state(z_handle, PDCOMM_STATE_CONNECTED,
                                     TRUE,(uint32)PDCOMM_RESULT_OK);

      break;
    }

    case E_SUCCESS:
    case E_WOULD_BLOCK:
    case E_IN_PROGRESS:
    {
      /* Open Pending */
      PDCOMM_APP_MSG_MED("SSL Connection Establishment Pending for handle_id [%d]",
                     z_handle.q_handle_id,0,0);
      /*pd_comms_tcp_task_update_state(z_handle, PDCOMM_STATE_CONNECTING,
                                     TRUE,(uint32)PDCOMM_RESULT_OK);*/
      break;
    }

    default:
    {
      /* An error occurred */
      PDCOMM_APP_MSG_ERROR("PDCOMM APP: Error: %d opening SSL session for handle_id [%d]",
                e_error, z_handle.q_handle_id, 0);
      pd_comms_tcp_task_ssl_session_cleanup(z_handle, FALSE); // just delete session

    }
  }/*switch ( e_error )*/
  
}/*pd_comms_tcp_task_open_ssl*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_secure_tcp_connect

DESCRIPTION
  This function handles a secure TCP connection request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_secure_tcp_connect(
  /*handle for which connection is to be opened*/
  pd_comms_handle_type z_handle)
{
  pd_comms_tcp_task_operation_status_type e_status = FAILED;
  /*Create the regular socket connection first and then OPEN a SSL
    session on that socket connection*/
  e_status  = pd_comms_tcp_task_tcp_connect(z_handle);

  // if e_status is PENDING, will do open_ssl when connection is established
  if(e_status == SUCCEEDED)
  {
    pd_comms_tcp_task_open_ssl(z_handle);  
       
  }
   
}/*pd_comms_tcp_task_secure_tcp_connect*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_secure_tcp_disconnect

DESCRIPTION
  This function handles a secure TCP link disconnect request. If abort option
  is set to true, the connection will be immediately closed and handle released. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pd_comms_tcp_task_secure_tcp_disconnect(
  /*handle corresponding to the connection for which connection is to be disconnected*/
  pd_comms_handle_type z_handle, 
  /*to abort or gracefully close a connection*/
  boolean b_abort,
  /*to notify client about the aborted connection or not*/
  boolean u_notifyClient)
{
  pd_comms_disconnect_result_type e_disc_error = PDCOMM_DISCONNECT_ERROR_UNKNOWN;

  if(b_abort)
  {
    PDCOMM_APP_MSG_HIGH("Abort received for handle_id [%d]",
                        z_handle.q_handle_id,0,0); 
    z_handle_table[z_handle.q_handle_id].session_aborted = TRUE;

    if(u_notifyClient)
    {
      pd_comms_notification_params_type z_params;
      pd_comms_return_type e_result;   
      
      z_params.e_new_state = PDCOMM_STATE_DISCONNECTED;
      z_params.q_result = (uint32)PDCOMM_RESULT_OK;

      /* Before doing anything else sendout a DISCONNECTED state to client
       * After that proceed silently (without updating client) with the disconnect
       * process*/ 

      e_result = pd_comms_notify_client(z_handle,
                           z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                           z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                           z_handle_table[z_handle.q_handle_id].q_disconnect_transaction_id,
                           STATE_CHANGE,&z_params);  

      if(e_result != PDCOMM_RESULT_OK)
      {
        PDCOMM_APP_MSG_ERROR("PDCOMM APP: IPC error %d for handle_id [%d]",
                  (uint32)e_result,z_handle.q_handle_id,0);

      }
    }/*if(u_notifyClient)*/
  }/*if(b_abort)*/

  switch(z_handle_table[z_handle.q_handle_id].e_link_status)
  {
    case PDCOMM_STATE_DISCONNECTED:
    {
      break;
    }

    case PDCOMM_STATE_OPENING:
    {
     /*This state doesn't apply to secure TCP connection*/
     /*PDCOMM_APP_MSG_ERROR("PDCOMM APP:Illegal state PDCOMM_STATE_OPENING for a secure tcp connection. Handle_id [%d]",
               z_handle.q_handle_id,0,0);*/
     break;

    }

    case PDCOMM_STATE_DISCONNECTING:
    {
      PDCOMM_APP_MSG_MED("Disconnect request received while in DISCONNECTING state for handle_id [%d]",
                         z_handle.q_handle_id,0,0);
      /*This is to mark the condition where client calls multiple disconnects
       *for the same handle or client sends a disconnect when disconnect process
       *is already going on becuase of some error*/
      z_handle_table[z_handle.q_handle_id].client_disconnect_recvd = TRUE;
      break;
    }

    case PDCOMM_STATE_CONNECTED:
    case PDCOMM_STATE_CONNECTING:
    {
      secerrno_enum_type e_error;

      if(z_handle_table[z_handle.q_handle_id].z_ssl_handle != NULL)
      {
      
        e_error = secssl_close_session( z_handle_table[z_handle.q_handle_id].z_ssl_handle,
                                        pd_comms_tcp_task_ssl_close_callback,
                                        &z_handle_table[z_handle.q_handle_id].z_handle.q_handle_id);   

        if( e_error != E_SUCCESS )
        {
          PDCOMM_APP_MSG_ERROR("PDCOMM APP: Could not close SSL session. Error: %d", e_error, 0, 0);

          if(!b_abort)
          {
            pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                           TRUE,(uint32)e_disc_error);
          }
          else
          {
            /*update local state but donot inform client*/
            pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                           FALSE,(uint32)e_disc_error);
          }

          pd_comms_tcp_task_ssl_session_cleanup(z_handle, FALSE); // just delete session        
        }
        else
        {
          if(!b_abort)
          {
            pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                           TRUE,(uint32)PDCOMM_RESULT_WAIT);
          }
          else
          {
            /*update local state but donot inform client*/
            pd_comms_tcp_task_update_state(z_handle,PDCOMM_STATE_DISCONNECTING,
                                           FALSE,(uint32)PDCOMM_RESULT_WAIT);
          }
        }
      }
      else
      {
        /*disconnect external socket */
        (void)pd_comms_tcp_task_teardown_tcp(z_handle);   
      }

      break;
    }

  } /* switch(link_status) ends*/

} /*pd_comms_tcp_task_secure_tcp_disconnect() ends*/


/*===========================================================================
FUNCTION pd_comms_tcp_task_secure_tcp_write

DESCRIPTION
  This function handles a write request for  secure TCP connection. 
  It also notifies the client about the result of the write operation

DEPENDENCIES
  None

RETURN VALUE
  boolean - write operation successfull or not 

SIDE EFFECTS
  None
===========================================================================*/
boolean pd_comms_tcp_task_secure_tcp_write(
  /*handle corresponding to the connection for which data is to be written*/
  pd_comms_handle_type z_handle)
{
  uint16 l_bytes_written_temp;
  secerrno_enum_type e_error;
  pd_comms_write_result_type e_write_error = PDCOMM_WRITE_ERROR_UNKNOWN;
  boolean result;

  /* Attempt to write length bytes to socket. bytes_written will return how many
     bytes actually got sent. */ 

  l_bytes_written_temp =  z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write;

  e_error = secssl_write( z_handle_table[z_handle.q_handle_id].z_ssl_handle,
                        &z_handle_table[z_handle.q_handle_id].p_write_buff[0],
                        &l_bytes_written_temp );

  if( e_error == E_SUCCESS )
  {
    /* // Delay Write Ack until SSL's callback comes in 
    pd_comms_write_ack_params_type z_params;
    
    z_params.q_bytes_written = l_bytes_written_temp;
    z_params.q_transaction_id = z_handle_table[z_handle.q_handle_id].q_write_transaction_id;

    // Acknowledge the write 
    (void)pd_comms_send_write_ack_to_client(z_handle,
                                      z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                                      z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                                      &z_params);
    z_handle_table[z_handle.q_handle_id].w_num_bytes_written = 0;
    z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = 0;
    */
    result =  TRUE;
  }
  else
  {
    /* A write error has occurred */

    PDCOMM_APP_MSG_ERROR("PDCOMM APP: SSL Write failure, error=%d",(uint32)e_error,0,0);
    e_write_error = PDCOMM_WRITE_ERROR_WRITE_INTERNAL_ERROR;
    result = FALSE;
    z_handle_table[z_handle.q_handle_id].w_num_bytes_written  = 0;
    z_handle_table[z_handle.q_handle_id].w_num_bytes_to_write = 0;

  } /*else for if( bytes_written_temp >= 0 ) ends*/

  if(result == FALSE)
  {
    pd_comms_notification_params_type z_params;
    
    z_params.e_new_state = z_handle_table[z_handle.q_handle_id].e_link_status;
    z_params.q_result = (uint32) e_write_error;
    
    (void)pd_comms_notify_client(z_handle,
                           z_handle_table[z_handle.q_handle_id].q_source_thread_id,
                           z_handle_table[z_handle.q_handle_id].q_base_msg_id,
                           z_handle_table[z_handle.q_handle_id].q_write_transaction_id,
                           ERROR_RPT,&z_params);
  }

  return( result );  

} /*pd_comms_tcp_task_secure_tcp_write() ends*/


#endif /* FEATURE_GNSS_BIT_API */  

#endif /* FEATURE_CGPS_PDCOMM */

