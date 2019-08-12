/*!
  @file
  wci2_client.h

  @brief
  Client interface for packing and sending messages/data in WCI-2 format

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/wci2/src/wci2_client.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
06/16/14   btl     Initial version

==========================================================================*/
/*=============================================================================

                      INCLUDE FILES

=============================================================================*/
#include <queue.h>
#include <stringl.h>
#include <npa.h>

#include "wci2_client.h"
#include "wci2_utils.h"
#include "wci2_frame.h"
#include "wci2_uart.h"
#include "wci2_core.h"
#include "wci2_diag.h"
#include "wci2_trace.h"

/*=============================================================================

                      INTERNAL DEFINES AND TYPES

=============================================================================*/
#define WCI2_CLIENT_MAX_CLIENTS 5

#define INIT_WCI2_CLIENT_MUTEX() \
  if (DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, \
                     &wci2_client_sync_handle, \
                     &wci2_client_sync_obj)) \
  { \
    ERR_FATAL("cxm_uart_sync_obj create failed!", 0, 0, 0); \
  } 
#define ACQUIRE_WCI2_CLIENT_MUTEX() DALSYS_SyncEnter(wci2_client_sync_handle)
#define RELEASE_WCI2_CLIENT_MUTEX() DALSYS_SyncLeave(wci2_client_sync_handle)

typedef struct _wci2_client_s
{
  boolean                  in_use;
  wci2_client_open_s       opts;
  wci2_uart_npa_req_type_e npa_req_state;
  npa_client_handle        npa_handle;
} wci2_client_s;

char* wci2_client_npa_names[WCI2_CLIENT_MAX_CLIENTS] = 
{
  "wci2_client_0",
  "wci2_client_1",
  "wci2_client_2",
  "wci2_client_3",
  "wci2_client_4"
};

/*=============================================================================

                      GLOBAL VARIABLES

=============================================================================*/
/* storage for all info about connected WCI2 clients */
static wci2_client_s wci2_clients[WCI2_CLIENT_MAX_CLIENTS];

/* mutex for locking client table during reg/dereg */
static DALSYSSyncObj    wci2_client_sync_obj;
static DALSYSSyncHandle wci2_client_sync_handle;

/*=============================================================================

                      INTERNAL FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  wci2_client_init

=============================================================================*/
/*!
    @brief
    Initialize client registration tables and set up resources -- should be
    done before any client registers.

    @return
    void
*/
/*===========================================================================*/
void wci2_client_init( void )
{
  /*-----------------------------------------------------------------------*/
  memset( &wci2_clients, 0, sizeof(wci2_clients) );
  INIT_WCI2_CLIENT_MUTEX();
  return;
}

/*=============================================================================

  FUNCTION:  wci2_main_init

=============================================================================*/
/*!
    @brief
    Initialize WCI2 subsystem -- called by RCINIT

    @return
    void
*/
/*===========================================================================*/
void wci2_main_init( void )
{
  /*-----------------------------------------------------------------------*/
  wci2_trace_init();
  wci2_uart_init();
  wci2_client_init();
  wci2_diag_init();
  return;
}

/*=============================================================================

                      EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  wci2_client_register

=============================================================================*/
/*!
    @brief
    Set up a client to send/receive WCI-2 format data over CXM UART

    @return
    wci2_error_e, opaque handle
*/
/*===========================================================================*/
wci2_error_e wci2_client_register( 
  wci2_client_open_s *params, 
  uint32             *handle 
)
{
  unsigned int i;
  uint8        rx_mask = 0;
  wci2_error_e retval = WCI2_E_SUCCESS;
  /*-----------------------------------------------------------------------*/

  /* validate pointers */
  WCI2_ASSERT( params != NULL && handle != NULL );

  /* validate parameter values */
  if( ( params->mode != WCI2_CLIENT_MODE_WCI2  &&
        params->mode != WCI2_CLIENT_MODE_BASIC )   ||
      ( params->baud != WCI2_UART_BAUD_115200  && 
        params->baud != WCI2_UART_BAUD_2000000 &&
        params->baud != WCI2_UART_BAUD_3000000 )   ||
      ( params->frame_mode != WCI2_FRAME_MODE_NONE &&
        params->frame_mode != WCI2_FRAME_MODE_HDLC &&
        params->frame_mode != WCI2_FRAME_MODE_SLIP ) )
  {
    retval = WCI2_E_INVALID_ARG;
  }
  else
  {
    /* args valid! */
    ACQUIRE_WCI2_CLIENT_MUTEX();

    /* find an empty spot in the client table */
    for( i = 0; i < WCI2_CLIENT_MAX_CLIENTS; i++ )
    {
      if( !wci2_clients[i].in_use )
      {
        break;
      }
    }

    /* save handle. if all clients are full, handle will be outside 
     * array bounds (invalid) */
    *handle = i;

    if( i < WCI2_CLIENT_MAX_CLIENTS )
    {
      /* we have successfully found an empty slot. fill out the info
       * and do error checking */
      memscpy( &wci2_clients[i].opts, sizeof(wci2_clients[i].opts), 
               params, sizeof(wci2_client_open_s) );

      /* confirm nobody else using a different uart mode (basic/wci2), 
       * baud rate, or frame type. All clients must agree on these */
      for( i = 0; i < WCI2_CLIENT_MAX_CLIENTS && retval == WCI2_E_SUCCESS; i++ )
      {
        if( wci2_clients[i].in_use )
        {
          if( wci2_clients[i].opts.mode != params->mode )
          {
            retval = WCI2_E_UART_MODE_CONFLICT;
          }
          if( wci2_clients[i].opts.baud != params->baud )
          {
            retval = WCI2_E_BAUD_CONFLICT;
          }
          if( wci2_clients[i].opts.mode == WCI2_CLIENT_MODE_WCI2 )
          {
            /* if someone is registered for WCI2, save the types they
             * are registered for to update UART layer below */
            rx_mask |= wci2_clients[i].opts.type_mask;
            if( wci2_clients[i].opts.type_mask & WCI2_REG_TYPE_2 &&
                wci2_clients[i].opts.frame_mode != params->frame_mode )
            {
              retval = WCI2_E_FRAME_MODE_CONFLICT;
            }
          }
        }
      }

      /* if we made it all the way here, no errors! complete registration
       * and set up client with their requested operation modes */
      if( retval == WCI2_E_SUCCESS )
      {
        wci2_clients[*handle].in_use = TRUE;

        /* create npa client for turning on/off uart */
        wci2_clients[*handle].npa_req_state = WCI2_UART_NPA_REQ_OFF;
        wci2_clients[*handle].npa_handle = npa_create_sync_client(
          "/modem/mcs/cxm_uart",
          wci2_client_npa_names[*handle],
          NPA_CLIENT_REQUIRED );
        WCI2_ASSERT( wci2_clients[*handle].npa_handle != NULL );

        /* need clocks on to set baud & cxm */
        wci2_client_enable_uart( *handle, TRUE );
        wci2_uart_set_cxm_mode( params->mode == WCI2_CLIENT_MODE_WCI2 );
        wci2_uart_set_baud( params->baud );
        if( params->mode == WCI2_CLIENT_MODE_WCI2 )
        {
          rx_mask |= params->type_mask;
          wci2_uart_set_wci2_rx_types( rx_mask );
          if( params->type_mask & WCI2_REG_TYPE_2 )
          {
            wci2_frame_set_mode( params->frame_mode );
          }
        }
        wci2_client_enable_uart( *handle, FALSE );
      }
    } /* end if space for client */
    else
    {
      WCI2_MSG_0( ERROR, "No empty client slots found" );
      retval = WCI2_E_NO_MORE_CLIENTS;
    }

    RELEASE_WCI2_CLIENT_MUTEX();
  } /* end if args valid */

  WCI2_MSG_2( HIGH, "Client register: handle=%d retval=%d", *handle, retval )
  WCI2_TRACE( WCI2_TRC_CLIENT_REG, retval, params->mode, *handle );

  return retval;
} /* wci2_client_register */

/*=============================================================================

  FUNCTION:  wci2_client_deregister

=============================================================================*/
/*!
    @brief
    Deregister the client -- tear down any resources client was using

    @return
    wci2_error_e: SUCCESS if successful, E_INVALID_ARG if not registered
*/
/*===========================================================================*/
wci2_error_e wci2_client_deregister( 
  uint32 handle 
)
{
  wci2_error_e retval = WCI2_E_SUCCESS;
  /*-----------------------------------------------------------------------*/

  if( handle < WCI2_CLIENT_MAX_CLIENTS &&
      wci2_clients[handle].in_use == TRUE )
  {
    wci2_client_enable_uart( handle, FALSE );
    npa_destroy_client( wci2_clients[handle].npa_handle );
    wci2_clients[handle].in_use = FALSE;
  }
  else
  {
    retval = WCI2_E_INVALID_ARG;
  }

  WCI2_MSG_2( HIGH, "Client dereg: handle=%d retval=%d", handle, retval );
  WCI2_TRACE( WCI2_TRC_CLIENT_DEREG, retval, handle, 0 );

  return retval;
}

/*=============================================================================

  FUNCTION:  wci2_client_enable_uart

=============================================================================*/
/*!
    @brief
    Vote to power on/off the UART. UART must be powered on before
    transmitting/receiving. This is a VOTE so if anyone votes for power on,
    UART will be on.

    @return
    void
*/
/*===========================================================================*/
void wci2_client_enable_uart( 
  uint32 handle, 
  boolean enable 
)
{
  /*-----------------------------------------------------------------------*/
  WCI2_ASSERT( handle < WCI2_CLIENT_MAX_CLIENTS && 
               wci2_clients[handle].in_use == TRUE );
  WCI2_MSG_3( MED, "Client=%d vote UART state=%d, currently=%d", handle, 
              enable, wci2_clients[handle].npa_req_state );
  wci2_clients[handle].npa_req_state = (enable) ? 
    WCI2_UART_NPA_REQ_ON : WCI2_UART_NPA_REQ_OFF;
  npa_issue_required_request( wci2_clients[handle].npa_handle, 
                              wci2_clients[handle].npa_req_state );
  return;
}

/*=============================================================================

  FUNCTION:  wci2_client_free_t2

=============================================================================*/
/*!
    @brief
    Free a type 2 frame after using it. Must be called after receiving
    A type 2 frame through a t2 callback.

    @return
    void
*/
/*===========================================================================*/
void wci2_client_free_t2( 
  wci2_t2_dyn_msg *msg 
)
{
  /*-----------------------------------------------------------------------*/
  /* decrement ref count. if 0, free msg */
  ACQUIRE_WCI2_CLIENT_MUTEX();
  msg->ref_count--;
  if( msg->ref_count == 0 )
  {
    WCI2_MEM_FREE( msg );
  }
  RELEASE_WCI2_CLIENT_MUTEX();

  return;
}

/*=============================================================================

  FUNCTION:  wci2_client_notify_rx_t2

=============================================================================*/
/*!
    @brief
    Distribute a received t2 frame to all the registered clients (through
    their callbacks

    @return
    void
*/
/*===========================================================================*/
void wci2_client_notify_rx_t2( wci2_t2_dyn_msg *msg )
{
  unsigned int i, j = 0;
  /*-----------------------------------------------------------------------*/

  /* initialize to 1 to make sure if client free in the cb, we 
   * still have a non-zero ref count for the other clients */
  msg->ref_count = 1;

  for( i = 0; i < WCI2_CLIENT_MAX_CLIENTS; i++ )
  {
    if( (wci2_clients[i].in_use) && 
        (wci2_clients[i].opts.t2_rx_cb != NULL) &&
        (wci2_clients[i].opts.type_mask & WCI2_REG_TYPE_2) )
    {
      ACQUIRE_WCI2_CLIENT_MUTEX();
      msg->ref_count++;
      RELEASE_WCI2_CLIENT_MUTEX();
      j++;

      wci2_clients[i].opts.t2_rx_cb( msg );
    }
  }

  /* decrement ref count and free if nobody registered */
  WCI2_TRACE( WCI2_TRC_CLNT_NTFY_T2_RX, msg->ref_count, j, msg->len );
  wci2_client_free_t2( msg );

  return;
}

/*=============================================================================

  FUNCTION:  wci2_client_notify_rx_dir

=============================================================================*/
/*!
    @brief
    Notify registered clients that a direct read of type 0-7 is ready 
    from the UART

    @return
    void
*/
/*===========================================================================*/
void wci2_client_notify_rx_dir( uint8 *buff, uint8 status )
{
  unsigned int i, j = 0;
  uint8        client_status;
  /*-----------------------------------------------------------------------*/

  for( i = 0; i < WCI2_CLIENT_MAX_CLIENTS; i++ )
  {
    client_status = wci2_clients[i].opts.type_mask & status;
    if( (wci2_clients[i].in_use) && 
        (wci2_clients[i].opts.dir_rx_cb != NULL) &&
        client_status )
    {
      wci2_clients[i].opts.dir_rx_cb( buff, client_status );
      j++;
    }
  }

  WCI2_TRACE( WCI2_TRC_CLNT_NTFY_DIR_RX, j, status, 0 );

  return;
}

/*=============================================================================

  FUNCTION:  wci2_client_notify_rx_basic

=============================================================================*/
/*!
    @brief
    Distribute the received packet to all the rgistered clients through
    their callbacks. For internal use only. Client must free their
    DSM item when they're done.
    NOTE: clients should not do processing from their callbacks.

    @return
    void
*/
/*===========================================================================*/
void wci2_client_notify_rx_basic(
  dsm_item_type **dsm_ptr
)
{
  unsigned int   i, j = 0;
  uint16         dsm_retval; /* count duped */
  uint32         dsm_length;
  dsm_item_type *client_dsm_ptr = NULL;
  /*-----------------------------------------------------------------------*/

  dsm_length = dsm_length_packet( *dsm_ptr );

  for( i = 0; i < WCI2_CLIENT_MAX_CLIENTS; i++ )
  {
    if( (wci2_clients[i].in_use) && 
        (wci2_clients[i].opts.basic_rx_cb != NULL) )
    {
      j++;

      /* dsm has built-in ref counts, dup packet and distribute copy
       * to each client */
      dsm_retval = dsm_dup_packet_pool(
        &client_dsm_ptr, /* dup ptr */
        DSM_DS_SMALL_ITEM_POOL,
        *dsm_ptr,        /* src ptr */
        0,               /* offset into pkt */
        (uint16) dsm_length /* size to copy */
      );
      WCI2_ASSERT( dsm_retval == dsm_length );

      wci2_clients[i].opts.basic_rx_cb( client_dsm_ptr );
    }
  }

  dsm_free_packet( dsm_ptr );
  WCI2_TRACE( WCI2_TRC_CLNT_NTFY_BASIC_RX, j, dsm_length, 0 );
  WCI2_MSG_2( MED, "SIO basic RX cb: %d bytes, %d clients", dsm_length, j );

  return;
}

/*=============================================================================

  FUNCTION:  wci2_client_t2_frame_and_send

=============================================================================*/
/*!
    @brief
    Frame the message into the set frame type (HDLC or SLIP) and transmit
    over the UART in WCi-2 Type2 format (nibblized).

    @return
    void */
/*===========================================================================*/
wci2_error_e wci2_client_t2_frame_and_send( 
  uint32               handle,
  const void*          msg, 
  uint16               msg_size, 
  wci2_type2_address_e addr   /* address needed for HDLC only */
)
{
  uint8           buff[WCI2_TYPE2_MAX_FRAME_SIZE];
  uint16          frame_size = 0;
  wci2_msg_type_s wci2_msg;
  wci2_error_e    retval = WCI2_E_SUCCESS;
  /*-----------------------------------------------------------------------*/

  /* validate client ID and translate to frame address */
  if( ( handle >= WCI2_CLIENT_MAX_CLIENTS ) ||
      ( !wci2_clients[handle].in_use ) || 
      ( WCI2_TYPE2_ADDRESS_MAX <= addr && 
        wci2_clients[handle].opts.frame_mode == WCI2_FRAME_MODE_HDLC) || 
      ( NULL == msg ) )
  {
    /* invalid args, return error */
    retval = WCI2_E_INVALID_ARG;
  }
  else
  {
    if( wci2_clients[handle].opts.frame_mode == WCI2_FRAME_MODE_HDLC )
    {
      /* HDLC mode */
      retval = wci2_hdlc_frame_and_escape( msg, msg_size, addr, buff, 
                                           WCI2_TYPE2_MAX_FRAME_SIZE, 
                                           &frame_size );
    }
    else
    {
      /* SLIP mode */
      retval = wci2_slip_frame_and_escape( msg, msg_size, buff, 
                                           WCI2_TYPE2_MAX_FRAME_SIZE, 
                                           &frame_size );
    }

    WCI2_MSG_2( MED, "Frame size=%d retval=%d", frame_size, retval );

    if( retval == WCI2_E_SUCCESS )
    {
      /* if framing succeeded we have something to send! */
      wci2_msg.type = WCI2_TYPE2;
      wci2_msg.data.type2_msg.length = frame_size;
      wci2_msg.data.type2_msg.buffer = buff;
      wci2_send_msg( &wci2_msg );
    }
  }

  return retval;
}


