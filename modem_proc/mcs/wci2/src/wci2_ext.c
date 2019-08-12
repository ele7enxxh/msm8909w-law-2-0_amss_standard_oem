/*!
  @file
  wci2_ext.c

  @brief
  Interface for customer to send/recv using SLIP and BASIC WCI2 modes

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/wci2/src/wci2_ext.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
           ckk     Initial revision

==========================================================================*/
/*=============================================================================

                      INCLUDE FILES

=============================================================================*/
#include <comdef.h>
#include <qurt_signal.h>
#include <DALSys.h>
#include "wci2.h"
#include "wci2_client.h"
#include "wci2_utils.h"
#include "wci2_frame.h"
#include "wci2_uart.h"

/*=============================================================================

                      GLOBALS

=============================================================================*/
/* Basic TX character rate possible */
#define BASIC_TX_RATE_MAX 300
#define BASIC_TX_RATE_MIN 1
/* Type2 SLIP TX character rate possible */
#define SLIP_TX_RATE_MAX 150
#define SLIP_TX_RATE_MIN 1

#define INVALID_HANDLE 0xFFFFFFFF

/* Max number of type2 RX frames we can have queued before processing */
#define MAX_RX_Q_LEN 20

typedef struct
{
  uint32             handle;
  wci2_client_mode_e mode;
  q_type             rx_q;
  qurt_signal_t      rx_sig;
  uint16             tx_rate;
  uint16             tx_char_delay;
  wci2_t2_dyn_msg   *frame_ptr;
} wci2_ext_client_s;

typedef union
{
  dsm_item_type   *basic_dsm_ptr;
  wci2_t2_dyn_msg *wci2_t2_ptr;
} wci2_ext_client_data_u;

/* struct for q-ing recvd data into RX q to be processed later by recv api */
typedef struct
{
  q_link_type            qlink;
  wci2_ext_client_data_u data;
} wci2_ext_client_rx_q_s;

/* type 2 message holding buffer */
static wci2_ext_client_rx_q_s rx_msgs[MAX_RX_Q_LEN];

static wci2_ext_client_s wci2_ext_client = { INVALID_HANDLE };

/*=============================================================================

                      INTERNAL FUNCTION DEFINITIONS

=============================================================================*/
/*=============================================================================

  FUNCTION:  wci2_basic_client_recv_cb

=============================================================================*/
/*!
    @brief
    Callback for WCI2 to give us new basic-mode RX messages

    @return
    void
*/
/*===========================================================================*/
void wci2_basic_client_recv_cb( dsm_item_type *dsm_ptr )
{
  static unsigned int drop_count = 0;
  unsigned int i;
  /*-----------------------------------------------------------------------*/
  /* queue message */
  for( i = 0; i < MAX_RX_Q_LEN; i++ )
  {
    if( rx_msgs[i].data.basic_dsm_ptr == NULL )
    {
      /* queue message up for processing by task later */
      q_link( &rx_msgs[i], &rx_msgs[i].qlink );
      rx_msgs[i].data.basic_dsm_ptr = dsm_ptr;
      q_put( &wci2_ext_client.rx_q, &rx_msgs[i].qlink );
      qurt_signal_set( &wci2_ext_client.rx_sig, 0x01 );
      break;
    }
  }

  if( i == MAX_RX_Q_LEN )
  {
    /* we dropped a message! couldn't find an empty slot */
    drop_count++;
    WCI2_MSG_1( ERROR, "Customer RX buff full, dropped frame. Cnt=%d", drop_count );
    dsm_free_packet( &dsm_ptr );
  }

  return;
}

/*=============================================================================

  FUNCTION:  wci2_type2_slip_client_recv_cb

=============================================================================*/
/*!
    @brief
    Callback for WCI2 to give us new cxm-mode RX type 2 messages

    @return
    void
*/
/*===========================================================================*/
void wci2_type2_slip_client_recv_cb( wci2_t2_dyn_msg *msg_ptr )
{
  static unsigned int drop_count = 0;
  unsigned int i;
  /*-----------------------------------------------------------------------*/
  /* queue message */
  for( i = 0; i < MAX_RX_Q_LEN; i++ )
  {
    if( rx_msgs[i].data.wci2_t2_ptr == NULL )
    {
      /* queue message up for processing by task later */
      q_link( &rx_msgs[i], &rx_msgs[i].qlink );
      rx_msgs[i].data.wci2_t2_ptr = msg_ptr;
      q_put( &wci2_ext_client.rx_q, &rx_msgs[i].qlink );
      /* signal waiting recv API to process */
      qurt_signal_set( &wci2_ext_client.rx_sig, 0x01 );
      break;
    }
  }

  if( i == MAX_RX_Q_LEN )
  {
    /* we dropped a message! couldn't find an empty slot */
    drop_count++;
    WCI2_MSG_1( ERROR, "Customer RX buff full, dropped frame. Cnt=%d", drop_count );
    wci2_client_free_t2( msg_ptr );
  }

  return;
}

/*=============================================================================

                      EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/
/*=============================================================================

  FUNCTION:  wci2_basic_client_create

=============================================================================*/
/*!
    @brief
    Setup CXM UART in BASIC mode & Create a client to handle raw data
 
    @return
    wci2_error_e
*/
/*===========================================================================*/
wci2_error_e wci2_basic_client_create (
  void
)
{
  wci2_error_e       retval = WCI2_E_SUCCESS;
  uint8              i;
  wci2_client_open_s params = {
    WCI2_UART_BAUD_3000000,
    WCI2_CLIENT_MODE_BASIC,
    WCI2_REG_TYPE_NONE,
    WCI2_FRAME_MODE_NONE,
    wci2_basic_client_recv_cb, /* basic rx cb */
    NULL, /* wci2 rx cb */
    NULL /* t2 rx cb */
  };
  /*-----------------------------------------------------------------------*/
  if( wci2_ext_client.handle != INVALID_HANDLE )
  {
    if( wci2_ext_client.mode != WCI2_CLIENT_MODE_BASIC )
    {
      WCI2_MSG_2( ERROR, "WCI2 external client already present: "
                  "[open_mode/req_mode] [%d/%d]",
                  wci2_ext_client.mode, params.mode );
      return WCI2_E_NO_MORE_CLIENTS;
    }
    else
    {
      WCI2_MSG_0( ERROR, "WCI2 external client already created..." );
      return WCI2_E_SUCCESS;
    }
  }

  retval = wci2_client_register( &params, &wci2_ext_client.handle );
  if( retval == WCI2_E_SUCCESS )
  {
    qurt_signal_init( &wci2_ext_client.rx_sig );
    wci2_ext_client.tx_rate = 0;
    wci2_ext_client.tx_char_delay = 0;
    wci2_ext_client.mode = WCI2_CLIENT_MODE_BASIC;
    wci2_ext_client.frame_ptr = NULL;

    /* initialize the rx queue for t2 messages */
    q_init( &wci2_ext_client.rx_q );
    for( i = 0; i < MAX_RX_Q_LEN; i++ )
    {
      rx_msgs[i].data.basic_dsm_ptr = NULL;
    }
  }

  return retval;
}

/*=============================================================================

  FUNCTION:  wci2_basic_client_destroy

=============================================================================*/
/*!
    @brief
    Destroys the BASIC uart client (if present)
 
    @return
    wci2_error_e
*/
/*===========================================================================*/
wci2_error_e wci2_basic_client_destroy (
  void
)
{
  wci2_error_e retval = WCI2_E_SUCCESS;
  uint8              i;
  /*-----------------------------------------------------------------------*/
  WCI2_MSG_2( HIGH, "Destroying external basic client, handle=%d mode=%d", 
              wci2_ext_client.handle, wci2_ext_client.mode );
  if( wci2_ext_client.mode == WCI2_CLIENT_MODE_BASIC )
  {
    retval = wci2_client_deregister( wci2_ext_client.handle );
    WCI2_ASSERT( WCI2_E_SUCCESS == retval );

    for( i = 0; i < MAX_RX_Q_LEN; i++ )
    {
      if( rx_msgs[i].data.basic_dsm_ptr != NULL )
      {
        dsm_free_packet( &(rx_msgs[i].data.basic_dsm_ptr) );
        rx_msgs[i].data.basic_dsm_ptr = NULL;
      }
    }

    qurt_signal_destroy( &wci2_ext_client.rx_sig );
    q_destroy( &wci2_ext_client.rx_q );
    memset( &wci2_ext_client, 0, sizeof( wci2_ext_client_s ) );
    wci2_ext_client.handle = INVALID_HANDLE;
  }
  else
  {
    retval = WCI2_E_UART_MODE_CONFLICT;
  }

  return retval;
}

/*=============================================================================

  FUNCTION:  wci2_basic_client_set_char_rate

=============================================================================*/
/*!
    @brief
    Set the TX character rate (in kHz)
    NOTE: this is the rate of bytes exiting the UART (in case receiver is unable
    to keep up with the UART's baudrate).

    @return
    void
*/
/*===========================================================================*/
void wci2_basic_client_set_char_rate( 
  uint16 tx_rate
)
{
  if( tx_rate >= BASIC_TX_RATE_MAX )
  {
    wci2_ext_client.tx_rate = BASIC_TX_RATE_MAX;
  }
  else if ( tx_rate == 0 )
  {
    wci2_ext_client.tx_rate = BASIC_TX_RATE_MIN;
  }
  WCI2_MSG_2( HIGH, "Set Basic TX char rate: [request/actual] [%d/%d]kHz",
              tx_rate,
              wci2_ext_client.tx_rate );

  /* calculate character delay, assuming 3Mbps! */
  wci2_ext_client.tx_char_delay = (1000/wci2_ext_client.tx_rate) - 6;

  return;
}

/*=============================================================================

  FUNCTION:  wci2_basic_client_send_pkt

=============================================================================*/
/*!
    @brief
    Send passed along a byte packet over the CXM UART in basic mode
    NOTE: this call will not return until the TX is complete

    @return
    void
*/
/*===========================================================================*/
wci2_error_e wci2_basic_client_send_pkt( 
  void*  p_data, 
  uint16 len 
)
{
  uint16          i;
  wci2_msg_type_s wci2_msg;
  wci2_error_e    retval = WCI2_E_SUCCESS;
  uint8          *data_ptr;
  /*-----------------------------------------------------------------------*/
  WCI2_ASSERT( NULL != p_data );
  data_ptr = (uint8 *)p_data;

  /* transmit at indicated character rate */
  if( retval == WCI2_E_SUCCESS )
  {
    wci2_msg.type = WCI2_TYPE2;
    if( wci2_ext_client.tx_char_delay == 0 )
    {
      wci2_uart_send( data_ptr, len );
    }
    else
    {
      for( i = 0; i < len; i++ )
      {
        wci2_uart_send( data_ptr+i, 1 );
        DALSYS_BusyWait( wci2_ext_client.tx_char_delay );
      }
    }
  }

  return retval;}

/*=============================================================================

  FUNCTION:  wci2_basic_client_recv_pkt

=============================================================================*/
/*!
    @brief
    Receive a byte packet over the CXM UART in basic mode
    NOTE: this call will block until a message is available

    @return
    void
*/
/*===========================================================================*/
void wci2_basic_client_recv_pkt( 
  dsm_item_type  **p_data
)
{
  wci2_ext_client_rx_q_s *q_item;
  unsigned int            ret_sigs = 0x00;
  /*-----------------------------------------------------------------------*/
  WCI2_ASSERT( NULL != p_data );

  /* if Q empty, wait on signal from recv callback, indicating 
   * we have new RX data */
  /* unqueue msg from Q and process */
  q_item = (wci2_ext_client_rx_q_s *) q_get( &wci2_ext_client.rx_q );
  if( q_item == NULL )
  {
    ret_sigs = qurt_signal_wait( &wci2_ext_client.rx_sig,
                                 (unsigned int) 0x01,
                                 QURT_SIGNAL_ATTR_WAIT_ANY );
    /* unqueue msg from Q and process */
    q_item = (wci2_ext_client_rx_q_s *) q_get( &wci2_ext_client.rx_q );
  }

  qurt_signal_clear( &wci2_ext_client.rx_sig, 0x01 );
  WCI2_ASSERT( q_item != NULL );
  *p_data = q_item->data.basic_dsm_ptr;
  q_item->data.basic_dsm_ptr = NULL;

  return;
}

/*=============================================================================

  FUNCTION:  wci2_type2_slip_client_create

=============================================================================*/
/*!
    @brief
    Setup CXM UART in WCI2 mode & Create a client to handle type2 messages
 
    @return
    wci2_error_e
*/
/*===========================================================================*/
wci2_error_e wci2_type2_slip_client_create (
  void
)
{
  wci2_error_e       retval = WCI2_E_SUCCESS;
  uint8              i;
  wci2_client_open_s params = {
    WCI2_UART_BAUD_3000000,
    WCI2_CLIENT_MODE_WCI2,
    WCI2_REG_TYPE_2,
    WCI2_FRAME_MODE_SLIP,
    NULL, /* basic rx cb */
    NULL, /* wci2 rx cb */
    wci2_type2_slip_client_recv_cb /* t2 rx cb */
  };
  /*-----------------------------------------------------------------------*/
  if( wci2_ext_client.handle != INVALID_HANDLE )
  {
    if( wci2_ext_client.mode != WCI2_CLIENT_MODE_WCI2 )
    {
      WCI2_MSG_2( ERROR, "WCI2 external client already present: "
                  "[open_mode/req_mode] [%d/%d]",
                  wci2_ext_client.mode, params.mode );
      return WCI2_E_NO_MORE_CLIENTS;
    }
    else
    {
      WCI2_MSG_0( ERROR, "WCI2 external client already created..." );
      return WCI2_E_SUCCESS;
    }
  }

  retval = wci2_client_register( &params, &wci2_ext_client.handle );
  WCI2_ASSERT( WCI2_E_SUCCESS == retval );

  qurt_signal_init( &wci2_ext_client.rx_sig );
  wci2_ext_client.mode = WCI2_CLIENT_MODE_WCI2;
  wci2_ext_client.tx_rate = 0;
  wci2_ext_client.tx_char_delay = 0;

  /* initialize the rx queue for t2 messages */
  q_init( &wci2_ext_client.rx_q );
  for( i = 0; i < MAX_RX_Q_LEN; i++ )
  {
    rx_msgs[i].data.wci2_t2_ptr = NULL;
  }

  return retval;
}

/*=============================================================================

  FUNCTION:  wci2_type2_slip_client_destroy

=============================================================================*/
/*!
    @brief
    Destroys the WCI2 type2 uart client (if present)
 
    @return
    wci2_error_e
*/
/*===========================================================================*/
wci2_error_e wci2_type2_slip_client_destroy (
  void
)
{
  wci2_error_e retval = WCI2_E_SUCCESS;
  uint8              i;
  /*-----------------------------------------------------------------------*/
  WCI2_MSG_2( HIGH, "Destroying external WCI2 t2 SLIP client, handle=%d mode=%d", 
              wci2_ext_client.handle, wci2_ext_client.mode );
  if( wci2_ext_client.mode == WCI2_CLIENT_MODE_WCI2 )
  {
    retval = wci2_client_deregister( wci2_ext_client.handle );
    WCI2_ASSERT( WCI2_E_SUCCESS == retval );

    /* free any msgs still in the rx queue */
    if( wci2_ext_client.frame_ptr != NULL )
    {
      wci2_client_free_t2( wci2_ext_client.frame_ptr );
    }
    for( i = 0; i < MAX_RX_Q_LEN; i++ )
    {
      if( rx_msgs[i].data.wci2_t2_ptr != NULL )
      {
        wci2_client_free_t2( rx_msgs[i].data.wci2_t2_ptr );
        rx_msgs[i].data.wci2_t2_ptr = NULL;
      }
    }

    qurt_signal_destroy( &wci2_ext_client.rx_sig );
    q_destroy( &wci2_ext_client.rx_q );
    memset( (void*)&wci2_ext_client, 0, sizeof( wci2_ext_client_s ) );
    wci2_ext_client.handle = INVALID_HANDLE;
  }
  else
  {
    retval = WCI2_E_UART_MODE_CONFLICT;
  }

  return retval;
}

/*=============================================================================

  FUNCTION:  wci2_type2_slip_client_set_char_rate

=============================================================================*/
/*!
    @brief
    Set the TX character rate (in kHz)
    NOTE: this is the rate of type2 characters exiting the UART. So
    since each byte is nibblized into two type2 characters, we will
    halve the given rate.

    @return
    void
*/
/*===========================================================================*/
void wci2_type2_slip_client_set_char_rate( 
  uint16 tx_rate
)
{
  /*-----------------------------------------------------------------------*/
  tx_rate = tx_rate >> 1;
  if( tx_rate >= SLIP_TX_RATE_MAX )
  {
    wci2_ext_client.tx_rate = SLIP_TX_RATE_MAX;
  }
  else if ( tx_rate == 0 )
  {
    wci2_ext_client.tx_rate = SLIP_TX_RATE_MIN;
  }
  WCI2_MSG_2( HIGH, "Set SLIP TX char rate: [request/actual] [%d/%d]kHz",
              tx_rate,
              wci2_ext_client.tx_rate );

  /* calculate character delay, assuming 3Mbps! */
  wci2_ext_client.tx_char_delay = (1000/wci2_ext_client.tx_rate) - 6;

  return;
}

/*=============================================================================

  FUNCTION:  wci2_type2_slip_client_send_pkt

=============================================================================*/
/*!
    @brief
    Encode and send a byte buffer as a SLIP packet over the CXM UART
    NOTE: this call will not return until the TX is complete

    @return
    void
*/
/*===========================================================================*/
wci2_error_e wci2_type2_slip_client_send_pkt( 
  void*  p_data, 
  uint16 len 
)
{
  uint8           buf[WCI2_TYPE2_MAX_FRAME_SIZE];
  uint16          i;
  uint16          frame_size = 0;
  wci2_msg_type_s wci2_msg;
  wci2_error_e    retval = WCI2_E_SUCCESS;
  /*-----------------------------------------------------------------------*/
  WCI2_ASSERT( NULL != p_data );

  /* encode/frame with SLIP */
  retval = wci2_slip_frame_and_escape( p_data, len, buf, 
                                       WCI2_TYPE2_MAX_FRAME_SIZE, 
                                       &frame_size );

  /* transmit at indicated character rate */
  if( retval == WCI2_E_SUCCESS )
  {
    WCI2_ASSERT( frame_size <= WCI2_TYPE2_MAX_FRAME_SIZE );
    wci2_msg.type = WCI2_TYPE2;
    if( wci2_ext_client.tx_char_delay == 0 )
    {
      /* no delay -- send at default baud rate */
      wci2_msg.data.type2_msg.length = frame_size;
      wci2_msg.data.type2_msg.buffer = buf;
      wci2_send_msg( &wci2_msg );
    }
    else
    {
      for( i = 0; i < frame_size; i++ )
      {
        wci2_msg.data.type2_msg.length = 1;
        wci2_msg.data.type2_msg.buffer = &buf[i];
        wci2_send_msg( &wci2_msg );
        DALSYS_BusyWait( wci2_ext_client.tx_char_delay );
      }
    }
  }

  return retval;
}

/*=============================================================================

  FUNCTION:  wci2_type2_slip_client_recv_pkt

=============================================================================*/
/*!
    @brief
    Receive a SLIP-encoded message over the CXM UART
    NOTE: this call will block until a message is available

    @return
    void
*/
/*===========================================================================*/
void wci2_type2_slip_client_recv_pkt( 
  uint8  **p_data, 
  uint16 *len, 
  uint8  *seq_n, 
  uint8  *checksum_tst 
)
{
  wci2_ext_client_rx_q_s *q_item;
  unsigned int            ret_sigs = 0x00;
  wci2_error_e            retval;
  /*-----------------------------------------------------------------------*/
  WCI2_ASSERT( NULL != p_data && NULL != len && 
               NULL != seq_n &&  NULL != checksum_tst );
  /* if the SLIP unframe fails, try again. */
  do
  {
    /* free the previous message when done processing */
    if( wci2_ext_client.frame_ptr != NULL )
    {
      wci2_client_free_t2( wci2_ext_client.frame_ptr );
      wci2_ext_client.frame_ptr = NULL;
    }

    /* if Q empty, wait on signal from recv callback, indicating 
     * we have new RX data */
    /* unqueue msg from Q and process */
    q_item = (wci2_ext_client_rx_q_s *) q_get( &wci2_ext_client.rx_q );
    if( q_item == NULL )
    {
      ret_sigs = qurt_signal_wait( &wci2_ext_client.rx_sig,
                                   (unsigned int) 0x01,
                                   QURT_SIGNAL_ATTR_WAIT_ANY );
      /* unqueue msg from Q and process */
      q_item = (wci2_ext_client_rx_q_s *) q_get( &wci2_ext_client.rx_q );
    }

    qurt_signal_clear( &wci2_ext_client.rx_sig, 0x01 );
    WCI2_ASSERT( q_item != NULL );
    wci2_ext_client.frame_ptr = q_item->data.wci2_t2_ptr;
    q_item->data.wci2_t2_ptr = NULL;
    retval = wci2_slip_unframe( wci2_ext_client.frame_ptr->buff, 
                                wci2_ext_client.frame_ptr->len, 
                                p_data, len, seq_n, checksum_tst );
  } while( retval != WCI2_E_SUCCESS );

  return;
}
