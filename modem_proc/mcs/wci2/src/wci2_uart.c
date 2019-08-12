/*!
  @file
  wci2_uart.c

  @brief
  Implementation of WCI-2's UART interface APIs

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/wci2/src/wci2_uart.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/14/14   btl     Reorg, generalize with basic uart functions
05/12/14   ckk     Address return status for UART powerdown request
02/12/14   btl     Mutex + states to prevent UART on/off race condition
11/22/13   btl     Save dsm_item_type* in RX FIFO
03/21/13   btl     Fix RX FIFO concurrency problems
01/29/13   btl     DCVS vote when UART active
11/05/12   ckk     Add WCI2's event trace logging
10/19/12   btl     Add additional F3 logging
09/20/12   btl     Added RX support
09/13/12   cab     Remove tx flush from init sequence
09/13/12   cab     Added power down and wakeup modes for power savings
           ckk     Initial implementation

===========================================================================*/

/*===========================================================================

                   INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include <IxErrno.h>
#include <sio.h>
#include <dsm.h>
#include <npa_resource.h>
#include <DALSys.h>
#include <timer.h>

#include "wci2_utils.h"
#include "wci2_uart.h"
#include "wci2_frame.h"
#include "wci2_client.h"
#include "wci2_core.h"
#include "wci2_trace.h"
#include "wci2_diag.h"

/*===========================================================================

                   INTERNAL TYPES

===========================================================================*/
#define WCI2_UART_DIR_READ_MSG_BUF_SIZE 8
#define WCI2_UART_TYPE0_INTR_BIT 0x01
#define WCI2_UART_TYPE1_INTR_BIT 0x02
#define WCI2_UART_TYPE2_INTR_BIT 0x04
#define WCI2_UART_TYPE3_INTR_BIT 0x08
#define WCI2_UART_TYPE4_INTR_BIT 0x10
#define WCI2_UART_TYPE5_INTR_BIT 0x20
#define WCI2_UART_TYPE6_INTR_BIT 0x40
#define WCI2_UART_TYPE7_INTR_BIT 0x80

#define INIT_WCI2_UART_MUTEX() \
  if (DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE, \
                     &cxm_uart_sync_handle, \
                     &cxm_uart_sync_obj)) \
  { \
    ERR_FATAL("cxm_uart_sync_obj create failed!", 0, 0, 0); \
  } 
#define ACQUIRE_WCI2_UART_MUTEX() DALSYS_SyncEnter(cxm_uart_sync_handle)
#define RELEASE_WCI2_UART_MUTEX() DALSYS_SyncLeave(cxm_uart_sync_handle)
#define WCI2_UART_CONSEC_FLUSH_POWERDOWN_REQS_LIMIT 5

typedef enum
{
  WCI2_UART_OFF,
  WCI2_UART_FLUSHING,
  WCI2_UART_ON
} wci2_uart_state_e;

typedef struct
{
  /* enable bits for type0-7 on direct read */
  uint32 intr_enable;

  /* status bits for type0-7 on direct read */
  uint32 intr_status;

  /* incoming direct read message buffer, one byte per type0-7 */
  uint8 msg_buf[WCI2_UART_DIR_READ_MSG_BUF_SIZE];
} wci2_uart_dir_read_type;

typedef struct
{
  wci2_uart_state_e       state;
  wci2_uart_state_e       prev_state;
  wci2_uart_dir_read_type dir_read;
  wci2_uart_baud_type_e   baud;
  boolean                 cxm_hw; /* is cxm mode enabled? */
  boolean                 adv_hw; /* is the advanced hw enabled? */
  sio_stream_id_type      stream_id;
  uint32                  cnt_consec_pwrdwn_reqs;
  uint32                  cnt_total_pwrdwn_reqs;
  uint32                  cnt_misaligned_t2;
  boolean                 wci2_tx_uart_high;
  boolean                 initialized;
  boolean                 diag_active;
} wci2_uart_state_info_s;

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/
/* uart initial state */
static wci2_uart_state_info_s wci2_uart_state = 
{
  WCI2_UART_OFF,
  WCI2_UART_OFF,
  {
    (WCI2_UART_TYPE1_INTR_BIT | WCI2_UART_TYPE6_INTR_BIT), 
    0, 
    { 0, 0, 0, 0, 0, 0, 0 }
  },
  WCI2_UART_BAUD_3000000,
  FALSE, /* cxm hw en? */
  FALSE, /* advanced hw en? */
  SIO_NO_STREAM_ID,
  0,     /* consec pwrdwn reqs */
  0,     /* total pwrdwn reqs */
  0,     /* cnt misaligned t2 bytes */
  FALSE, /* dsm items not full */
  FALSE, /* not initialized */
  FALSE  /* diag not active */
};

static DALSYSSyncObj cxm_uart_sync_obj;
static DALSYSSyncHandle cxm_uart_sync_handle;

#ifdef FEATURE_DSM_WM_CB
STATIC void wci2_uart_lowater_cb(struct dsm_watermark_type_s *wm_ptr, void *v_ptr);
STATIC void wci2_uart_hiwater_cb(struct dsm_watermark_type_s *wm_ptr, void *v_ptr);
#else /* FEATURE_DSM_WM_CB */
STATIC void wci2_uart_lowater_cb(void);
STATIC void wci2_uart_hiwater_cb(void);
#endif /* FEATURE_DSM_WM_CB */
STATIC dsm_watermark_type wci2_uart_wmq;
STATIC q_type wci2_uart_echo_q;

/* NPA node data */
STATIC npa_resource_state wci2_uart_npa_driver ( npa_resource *resource,
                                                 npa_client *client,
                                                 npa_resource_state state );

STATIC npa_resource_definition wci2_uart_resource[] =
{
  {"/modem/mcs/cxm_uart",     /**< Resource Name */
    "",                       /**< Resource Units - Informational only */
    1,                        /**< Resource Max  */
    &npa_binary_plugin,       /**< Aggregate with binary style */
    NPA_RESOURCE_DEFAULT,     /**< Default resource behavior */
    NULL}                     /**< User Data */
};

STATIC npa_node_definition wci2_uart_node = 
{
  "/node/modem/mcs/cxm_uart",   /**< Node name */
  wci2_uart_npa_driver,         /**< Node driver function */
  NPA_NODE_DEFAULT,             /**< Attributes */
  NULL,                         /**< User data */
  NPA_EMPTY_ARRAY,              /**< No dependencies */
  NPA_ARRAY(wci2_uart_resource) /**< Node resource from above */
};

/* Dynamic Clock and Voltage Scaling - needed to meet uart bandwidth req */
STATIC npa_client_handle wci2_uart_dcvs_client;

/*===========================================================================

                    INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

#ifdef FEATURE_DSM_WM_CB
STATIC void wci2_uart_hiwater_cb(struct dsm_watermark_type_s *wm_ptr, void *v_ptr)
#else /* FEATURE_DSM_WM_CB */
STATIC void wci2_uart_hiwater_cb()
#endif /* FEATURE_DSM_WM_CB */
{
  /*-----------------------------------------------------------------------*/
  wci2_uart_state.wci2_tx_uart_high = TRUE;
  WCI2_MSG_0( MED, "UART high watermark callback" );
  return;
}

#ifdef FEATURE_DSM_WM_CB
STATIC void wci2_uart_lowater_cb(struct dsm_watermark_type_s *wm_ptr, void *v_ptr)
#else /* FEATURE_DSM_WM_CB */
STATIC void wci2_uart_lowater_cb()
#endif /* FEATURE_DSM_WM_CB */
{
  /*-----------------------------------------------------------------------*/
  wci2_uart_state.wci2_tx_uart_high = FALSE;
  WCI2_MSG_0( MED, "UART low watermark callback" );
  return;
}

#ifdef WCI2_FEATURE_ADV_UART_HW
/*===========================================================================

  FUNCTION:  wci2_uart_dir_read_cb

===========================================================================*/
/*!
  @brief
    Called when a direct read of type0-7 is ready from uart

  @return none
*/
/*=========================================================================*/
STATIC void wci2_uart_dir_read_cb(
  void
)
{
  uint8 status;
  /*-----------------------------------------------------------------------*/
  status = (uint8) wci2_uart_state.dir_read.intr_enable & 
                   wci2_uart_state.dir_read.intr_status;
  WCI2_TRACE( WCI2_TRC_DIR_READ_CB, status,
              wci2_uart_state.dir_read.intr_enable,
              wci2_uart_state.dir_read.intr_status );
  wci2_client_notify_rx_dir( &wci2_uart_state.dir_read.msg_buf[0], status );

  return;
} /* wci2_uart_dir_read_cb */

/*===========================================================================

  FUNCTION:  wci2_uart_type2_misalign_cb

===========================================================================*/
/*!
  @brief
    Called when a direct read of type0-7 is ready from uart

  @return none
*/
/*=========================================================================*/
STATIC void wci2_uart_type2_misalign_cb(
  void
)
{
  /*-----------------------------------------------------------------------*/
  wci2_uart_state.cnt_misaligned_t2++;
  WCI2_TRACE( WCI2_TRC_T2_MISALIGNED, wci2_uart_state.cnt_misaligned_t2, 0, 0 );
  WCI2_MSG_1( ERROR, "Detected misaligned byte in type2 reception, cnt=%d", 
              wci2_uart_state.cnt_misaligned_t2 );
} /* wci2_uart_type2_misalign_cb */
#endif /* WCI2_FEATURE_ADV_UART_HW */

/*=============================================================================

  FUNCTION:  wci2_uart_type2_denibblize

=============================================================================*/
/*!
    @brief
    Process one type-2 byte at a time. After two are received, if the 
    type-2 format is acceptible, will combine them into one byte and pass
    it on.

    @return
    void
*/
/*===========================================================================*/
void wci2_uart_type2_denibblize( uint8 type2_byte )
{
  static uint8     whole_byte;
  static boolean   new_byte = TRUE;
  wci2_t2_dyn_msg* msg_ptr = NULL;
  /*-----------------------------------------------------------------------*/
  if( 0 == (type2_byte & WCI2_TYPE2_HSN_BIT) )
  {
    /* we've received a low nibble */
    if( new_byte )
    {
      /* we're expecting a new type-2 message, so everything looks good */
      whole_byte = 
        (type2_byte & WCI2_TYPE2_DATA_MASK) >> WCI2_TYPE2_BITS_TO_SHIFT_LSN;
      new_byte = FALSE;
    }
    else
    {
      /* something's wrong... expected a least-significant nibble. Drop
       * nibble and reset */
      wci2_uart_state.cnt_misaligned_t2++;
      WCI2_TRACE( WCI2_TRC_T2_MISALIGNED, wci2_uart_state.cnt_misaligned_t2, 
                  type2_byte, 0 );
      WCI2_MSG_1( ERROR, "Detected misaligned byte in type2 reception, cnt=%d", 
                  wci2_uart_state.cnt_misaligned_t2 );
      new_byte = TRUE;
    }
  }
  else
  {
    /* we've received a most-significant nibble */
    if( !new_byte )
    {
      /* we were expecting that, so finish building the byte and process it */
      whole_byte |= 
        (type2_byte & WCI2_TYPE2_DATA_MASK) >> WCI2_TYPE2_BITS_TO_SHIFT_MSN;

      msg_ptr = wci2_frame_buffer_rx( whole_byte );
      if( msg_ptr != NULL )
      {
        /* call client callbacks, pass msg */
        wci2_client_notify_rx_t2( msg_ptr );
      }

      new_byte = TRUE;
    }
  }
}

/*=============================================================================

  FUNCTION:  wci2_uart_sio_type2_recv_cb

=============================================================================*/
/*!
    @brief
    Handler for the CxM's SIO stream to read incoming type 2 bytes
    (unniblized) when the advanced CXM hardware is enabled. One DSM packet
    passed to us through this callback.

    @return
    void
*/
/*===========================================================================*/
void wci2_uart_sio_type2_recv_cb( 
  dsm_item_type **wci2_rx_dsm_ptr 
)
{
  uint8            byte;
  int16            dsm_return;
  wci2_t2_dyn_msg* msg_ptr = NULL;
  /*-----------------------------------------------------------------------*/
  /* copy into global buffer, when frame complete, distribute to clients
   * through the client callback. */
  dsm_return = dsm_pull8(wci2_rx_dsm_ptr);
  while( -1 != dsm_return )
  {
    byte = (uint8) dsm_return;
    msg_ptr = wci2_frame_buffer_rx( byte );
    if( msg_ptr != NULL )
    {
      /* call client callbacks, pass msg */
      WCI2_MSG_1( MED, "RX T2 framed msg len=%d", msg_ptr->len );
      wci2_client_notify_rx_t2( msg_ptr );
    }

    /* try to get next byte in prep for next loop iteration */
    dsm_return = dsm_pull8(wci2_rx_dsm_ptr);
  }

  return;
}

/*=============================================================================

  FUNCTION:  wci2_uart_sio_basic_recv_cb

=============================================================================*/ /*!
    @brief
    Handler for the CxM's SIO stream to read incoming data on UART when
    in basic mode

    @return
    void
*/
/*===========================================================================*/
void wci2_uart_sio_basic_recv_cb( 
  dsm_item_type **wci2_rx_dsm_ptr
)
{
  uint8 byte;
  int16 dsm_return;
  /*-----------------------------------------------------------------------*/
  WCI2_MSG_0( MED, "SIO RX basic mode CB" );

  if( wci2_uart_state.diag_active )
  {
    dsm_return = dsm_pull8( wci2_rx_dsm_ptr );
    while( -1 != dsm_return )
    {
      byte = (uint8) dsm_return;
      wci2_diag_process_uart_rx_data( byte );

      /* try to get next byte in prep for next loop iteration */
      dsm_return = dsm_pull8( wci2_rx_dsm_ptr );
    }
  }
  else
  {
    /* pass a copy of the buffer to everyone registered. */
    wci2_client_notify_rx_basic( wci2_rx_dsm_ptr );
  }

  return;
}

/*=============================================================================

  FUNCTION:  wci2_uart_sio_process_rx_packet

=============================================================================*/
/*!
    @brief
    Callback with SIO for received DSM packets. Process according to
    whether UART is in type2 mode or regular mode

    @return
    void
*/
/*===========================================================================*/
void wci2_uart_sio_process_rx_packet(
  dsm_item_type **wci2_rx_dsm_ptr
)
{
#ifndef WCI2_FEATURE_ADV_UART_HW
  uint8 byte;
  uint8 type;
  int16 dsm_return;
  uint8 status = 0;
#endif /* WCI2_FEATURE_ADV_UART_HW */
  /*-----------------------------------------------------------------------*/
  WCI2_TRACE( WCI2_TRC_SIO_RX_CB, wci2_uart_state.cxm_hw, 
              wci2_uart_state.adv_hw, 0 );
  if( wci2_uart_state.cxm_hw )
  {
#ifdef WCI2_FEATURE_ADV_UART_HW
    wci2_uart_sio_type2_recv_cb( wci2_rx_dsm_ptr );
#else
    /* emulate the missing adv hardware. loop through and copy
     * into wci2-type buffer */
    dsm_return = dsm_pull8( wci2_rx_dsm_ptr );
    while( -1 != dsm_return )
    {
      byte = (uint8) dsm_return;

      /* interpret byte as WCI2 */
      type = byte & WCI2_TYPE_BITS_MASK;
      status |= 1 << type;
      wci2_uart_state.dir_read.msg_buf[type] = byte;
      WCI2_TRACE( WCI2_TRC_RX_WCI2, byte, type, wci2_uart_state.cxm_hw );
      //DBG: WCI2_MSG_2( "RX: WCI2 msg (type=%d byte=0x%x)", type, byte );

      /* if type 2, denibblize and check for framing */
      if( type == WCI2_TYPE2 )
      {
        wci2_uart_type2_denibblize( byte );
      }

      /* try to get next byte in prep for next loop iteration */
      dsm_return = dsm_pull8(wci2_rx_dsm_ptr);
    }

    /* alert clients that we have new WCI2 data */
    wci2_client_notify_rx_dir( &wci2_uart_state.dir_read.msg_buf[0], status );
#endif /* WCI2_FEATURE_ADV_UART_HW */
  }
  else
  {
    wci2_uart_sio_basic_recv_cb( wci2_rx_dsm_ptr );
  }

  return;
}

/*===========================================================================

                    EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*=============================================================================

  FUNCTION:  wci2_uart_init

=============================================================================*/
/*!
    @brief
    Initialize UARt as a plain basic UART (no special CXM hardware)

    @return
    void
*/
/*===========================================================================*/
void wci2_uart_init( void )
{
  npa_resource_state   npa_initial_state[] = { WCI2_UART_NPA_REQ_OFF };
  sio_open_type        wci2_sio_open_struct = {
    SIO_NO_STREAM_ID,           /**< Stream ID Type.  Filled in
                                     by SIO for internal use.     */
    SIO_STREAM_RXTX,            /**< Type of stream being opened.
                                     Filled in by SIO for 
                                     internal use only.           */
    SIO_DS_PKT_MODE,            /**< Stream Mode Type.            */
    NULL,                       /**< Received Data Queue.         */
    &wci2_uart_wmq,             /**< Transmit Data Queue.         */
    SIO_BITRATE_3000000,        /**< Bit-rate for reception.      */
    SIO_BITRATE_3000000,        /**< Bit-rate for transmission.   */
    SIO_PORT_UART_CXM,          /**< Port which needs to be used. */
    FALSE,                      /**< True, if tail character is used */
    '\0',                       /**< If tail character is used,
                                     this is the tail character.  */
    wci2_uart_sio_process_rx_packet, /**< If non-NULL, this function 
                                         will be called for each. 
                                         packet which is received. */
    SIO_FCTL_OFF,               /**< TX flow control method.       */
    SIO_FCTL_OFF,               /**< RX flow control method.       */
  };
  /*-----------------------------------------------------------------------*/
  /* initialize DSM watermark: set up queue for SIO */
  wci2_uart_wmq.q_ptr = &wci2_uart_echo_q;
  dsm_queue_init( &wci2_uart_wmq,    /* watermark queue */
                  1280,            /* dont_exceed_cnt */
                  &wci2_uart_echo_q  /* queue */
                );

  /* Setup port setting variable */
  wci2_uart_wmq.current_cnt = 0;
  wci2_uart_wmq.dont_exceed_cnt = 1280;
  wci2_uart_wmq.hi_watermark = 1024;
  wci2_uart_wmq.lo_watermark = 256;
  wci2_uart_wmq.hiwater_func_ptr = wci2_uart_hiwater_cb;
  wci2_uart_wmq.lowater_func_ptr = wci2_uart_lowater_cb;
  wci2_uart_state.wci2_tx_uart_high = FALSE;

  INIT_WCI2_UART_MUTEX();

#ifdef T_RUMI_EMULATION
  /* RUMI has slower clocks, only supports max of 115200 */
  wci2_uart_state.baud = WCI2_UART_BAUD_115200;
  wci2_sio_open_struct.rx_bitrate = SIO_BITRATE_115200;
  wci2_sio_open_struct.tx_bitrate = SIO_BITRATE_115200;
#endif

  /* Open UART using SIO interface */
  wci2_uart_state.stream_id = sio_open( &wci2_sio_open_struct );
  if( SIO_NO_STREAM_ID == wci2_uart_state.stream_id )
  {
    /* error opening UART!!! */
    WCI2_MSG_0( ERROR, "SIO_OPEN failed!" );
  }
  else
  {
    /* all the other calls rely on this being true!! so must be first */
    wci2_uart_state.initialized = TRUE;
    /* set up UART for CxM so it's ready to go later, then turn Cxm off */
    wci2_uart_set_cxm_mode( TRUE );
    /* if this isn't supported an a target, this call does nothing. */
    wci2_uart_set_adv_hw( TRUE );
    /* turn off CxM mode until client enables it */
    wci2_uart_set_cxm_mode( FALSE );

    /* power down UART until client needs it */
    wci2_uart_state.state = WCI2_UART_OFF;
    sio_ioctl(wci2_uart_state.stream_id, SIO_IOCTL_POWERDOWN, NULL);  
    WCI2_MSG_0( HIGH, "Powering down UART" );

    /* create client for adjusting clock when UART needed */
    wci2_uart_dcvs_client = npa_create_sync_client( 
      "/clk/cpu", "wci2_dcvs", NPA_CLIENT_REQUIRED );
    WCI2_ASSERT( wci2_uart_dcvs_client != NULL );
  }

  /* define npa node, set initial state to reflect powerdown state */
  npa_define_node( &wci2_uart_node, npa_initial_state, NULL);

  /* Initialize UART monitors */
  wci2_uart_state.prev_state = WCI2_UART_OFF;
  wci2_uart_state.cnt_consec_pwrdwn_reqs = 0;
  wci2_uart_state.cnt_total_pwrdwn_reqs = 0;

  return;
}

/*===========================================================================

  FUNCTION:  wci2_uart_deinit

===========================================================================*/
/*!
  @brief
    To de-initialize the UART driver

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type wci2_uart_deinit (
  void
)
{
  errno_enum_type retval = E_SUCCESS;
  /*-----------------------------------------------------------------------*/
  if( wci2_uart_state.initialized )
  {
    /* Close the UART using SIO interface */
    sio_close( wci2_uart_state.stream_id, NULL );

    /* close NPA clients */
    npa_destroy_client( wci2_uart_dcvs_client );
  }

  return retval;
} /* wci2_uart_deinit */

/*=============================================================================

  FUNCTION:  wci2_uart_set_cxm_mode

=============================================================================*/
/*!
    @brief
    Turn special CXM hardware on/off (type 2 nibblize, direct send/receive 
    for other WCI2 types)

    @return
    void
*/
/*===========================================================================*/
void wci2_uart_set_cxm_mode( boolean enable )
{
  sio_ioctl_param_type param;
  /*-----------------------------------------------------------------------*/
  if( wci2_uart_state.initialized )
  {
    ACQUIRE_WCI2_UART_MUTEX();
    WCI2_TRACE( WCI2_TRC_SET_CXM_MODE, enable, wci2_uart_state.cxm_hw, 0 );
    wci2_uart_state.cxm_hw = enable;
    param.set_cxm.enable_cxm = enable;
    param.set_cxm.sam = 0;
    sio_ioctl(wci2_uart_state.stream_id, SIO_IOCTL_SET_CXM, &param);
    RELEASE_WCI2_UART_MUTEX();
    WCI2_MSG_1( HIGH, "CXM HW set enabled=%d", enable );
  }

  return;
}

/*===========================================================================

  FUNCTION:  wci2_uart_set_baud

===========================================================================*/
/*!
  @brief
    Sets the baud rate of the uart driver

  @return
    errno_enum_type
*/
/*=========================================================================*/
errno_enum_type wci2_uart_set_baud(
  wci2_uart_baud_type_e baud
)
{
  errno_enum_type      retval = E_SUCCESS;
  sio_ioctl_param_type param;
  /*-----------------------------------------------------------------------*/
  /* convert from wci2 baud data type to sio baud data type */
  WCI2_TRACE( WCI2_TRC_SET_BAUD, baud, wci2_uart_state.baud, 
              wci2_uart_state.initialized );
  WCI2_MSG_3( MED, "baud rate change: wci2_uart_baud_type_e=%d prev=%d init=%d",
              wci2_uart_state.baud, baud, wci2_uart_state.initialized );

  if( wci2_uart_state.initialized )
  {
    ACQUIRE_WCI2_UART_MUTEX();
    if ( baud == WCI2_UART_BAUD_115200 )
    {
      wci2_uart_state.baud = baud;
      param.bitrate = SIO_BITRATE_115200;
    }
    else if ( baud == WCI2_UART_BAUD_2000000 )
    {
      wci2_uart_state.baud = baud;
      param.bitrate = SIO_BITRATE_2000000;
    }
    else if ( baud == WCI2_UART_BAUD_3000000 )
    {
      wci2_uart_state.baud = baud;
      param.bitrate = SIO_BITRATE_3000000;
    }
    else
    {
      retval = E_INVALID_ARG;
    }

    if( wci2_uart_state.baud != baud )
    {
      /* have to have clocks on to change baud rate */
      if( wci2_uart_state.state != WCI2_UART_ON )
      {
        WCI2_MSG_0( ERROR, "Turn on clocks before setting baud" );
      }
      else
      {
        /* if we are initialized, send new rate direct to driver */
        sio_ioctl( wci2_uart_state.stream_id, SIO_IOCTL_CHANGE_BAUD_NOW, &param);
      }
    }
    RELEASE_WCI2_UART_MUTEX();
  }
  else
  {
    /* can't set baud until service initialized */
    retval = E_NOT_ALLOWED;
  }

  return retval;
}

/*===========================================================================

  FUNCTION:  wci2_uart_get_baud

===========================================================================*/
/*!
  @brief
    Gets the baud rate of the uart driver

  @return
    wci2_uart_baud_type_e 
*/
/*=========================================================================*/
wci2_uart_baud_type_e wci2_uart_get_baud(
  void
)
{
  /*-----------------------------------------------------------------------*/
  return wci2_uart_state.baud;
}

/*===========================================================================

  FUNCTION:  wci2_uart_flush_complete

===========================================================================*/
/*!
  @brief
    Finishes placing UART in powerdown state, allowing low power modes

  @return
    none
*/
/*=========================================================================*/
void wci2_uart_flush_complete( void )
{
  sio_ioctl_param_type param1, param2, param3;
  sio_status_type ioctl_status = SIO_DONE_S;
  uint8                clear_byte = 0x00;
  uint16               dsm_return = 0;
  dsm_item_type*       wci2_dsm_ptr = NULL;
  /*-----------------------------------------------------------------------*/
  ACQUIRE_WCI2_UART_MUTEX();

  /* if we are ON or OFF, then flush was cancelled or duplicate, do nothing */
  if ( wci2_uart_state.state == WCI2_UART_FLUSHING )
  {
    WCI2_MSG_0( HIGH, "uart flush request complete callback being processed" );
    /* for now hard-coding to sio interface instead of local, for optimization */
    /* tx buffer flushed, now place UART in powerdown mode */
    param1.enable_loopback_mode = TRUE;
    sio_ioctl( wci2_uart_state.stream_id, SIO_IOCTL_SET_LOOPBACK_MODE, &param1 );
    DALSYS_BusyWait( 4 );
    /* allocate the DSM packet and pack in the data */
    dsm_return = dsm_pushdown( &wci2_dsm_ptr, (void *)&clear_byte,
                               sizeof(clear_byte), DSM_DS_SMALL_ITEM_POOL );
    WCI2_ASSERT( sizeof(clear_byte) == dsm_return );
    sio_transmit( wci2_uart_state.stream_id, wci2_dsm_ptr );
    DALSYS_BusyWait( 10 );
    param2.cxm_tx.tx_sticky = 0;
    param2.cxm_tx.clear = TRUE;
    sio_ioctl( wci2_uart_state.stream_id, SIO_IOCTL_GET_CXM_TX, &param2 );
    param2.cxm_tx.clear = FALSE;
    sio_ioctl( wci2_uart_state.stream_id, SIO_IOCTL_GET_CXM_TX, &param2 );

    /* turn loopback off */
    param1.enable_loopback_mode = FALSE;
    sio_ioctl(wci2_uart_state.stream_id, SIO_IOCTL_SET_LOOPBACK_MODE, &param1);

    /* attempt to power down the uart */
    param3.status_ptr = (void *)&ioctl_status;
    /* keeping a count of overall system flush-powerdown requests */
    wci2_uart_state.cnt_total_pwrdwn_reqs++;
    sio_ioctl(wci2_uart_state.stream_id, SIO_IOCTL_POWERDOWN, &param3);
    /* Check whether uart powerdown request went through */
    if( ioctl_status == SIO_DONE_S )
    {
      /* UART powered down */
      wci2_uart_state.prev_state = wci2_uart_state.state;
      wci2_uart_state.state = WCI2_UART_OFF;
      WCI2_TRACE( WCI2_TRC_UART_POWER_STATE, wci2_uart_state.state, 
                  wci2_uart_state.prev_state, 0 );
      WCI2_MSG_1( HIGH, "uart powered down, loopback setting: %d",
                  param1.enable_loopback_mode );  
      wci2_uart_state.cnt_consec_pwrdwn_reqs = 0;

      /* npa vote - higher dynamic clock no longer needed */
      WCI2_ASSERT( wci2_uart_dcvs_client != NULL );
      npa_complete_request( wci2_uart_dcvs_client );
    }
    else /* ioctl_status == SIO_BADP_S */
    {
      /* UART power down request failed due to potential pending data
         re-start flushing tx buffer */
      WCI2_MSG_0( HIGH, "uart power down stalled, resending flushing request" );
      sio_flush_tx( wci2_uart_state.stream_id, wci2_uart_flush_complete );
      wci2_uart_state.prev_state = wci2_uart_state.state;
      wci2_uart_state.state = WCI2_UART_FLUSHING;
      WCI2_TRACE( WCI2_TRC_UART_POWER_STATE, wci2_uart_state.state, 
                  wci2_uart_state.prev_state, 0 );

      /* Check to see if the UART ever went through a successful
          powering "down & on" cycle */
      if( wci2_uart_state.prev_state == wci2_uart_state.state )
      {
        wci2_uart_state.cnt_consec_pwrdwn_reqs++;
        WCI2_MSG_1( HIGH, "Consecutive uart flush-powerdown request count: %d",
                    wci2_uart_state.cnt_consec_pwrdwn_reqs );

        if( wci2_uart_state.cnt_consec_pwrdwn_reqs >= WCI2_UART_CONSEC_FLUSH_POWERDOWN_REQS_LIMIT )
        {
          WCI2_ERR_FATAL( "Too many consecutive uart flush-powerdown requests: %d, threshold set to %d",
                          wci2_uart_state.cnt_consec_pwrdwn_reqs, 
                          WCI2_UART_CONSEC_FLUSH_POWERDOWN_REQS_LIMIT, 0 );
        }
      }
      else
      {
        /* UART has previously gone through a successful powering "down & on" cycle */
        wci2_uart_state.cnt_consec_pwrdwn_reqs = 0;
        WCI2_MSG_2( HIGH, "Resetting consecutive uart flush-powerdown request count,  previous state %d != current state %d",
                    wci2_uart_state.prev_state, wci2_uart_state.state );
      }
    }
  }
  else
  {
    WCI2_MSG_0( HIGH, "uart flush request was previously cancelled" );
    wci2_uart_state.cnt_consec_pwrdwn_reqs = 0;
  }

  RELEASE_WCI2_UART_MUTEX();
  return;
}

/*===========================================================================

  FUNCTION:  wci2_uart_set_loopback_mode

===========================================================================*/
/*!
  @brief
    Enable/disable loopback mode on CxM's SIO stream on UART

  @return
    void
*/
/*=========================================================================*/
void wci2_uart_set_loopback_mode(
  boolean enable
)
{
  sio_ioctl_param_type param;
  /*-----------------------------------------------------------------------*/
  if( wci2_uart_state.initialized )
  {
    /* send command to enable/disable loopback mode */
    param.enable_loopback_mode = enable;
    sio_ioctl(wci2_uart_state.stream_id, SIO_IOCTL_SET_LOOPBACK_MODE, &param);
  }

  return;
}

/*===========================================================================

  FUNCTION:  wci2_uart_send

===========================================================================*/
/*!
  @brief
    Send an arbitrary-length buffer out the UART. If UART in CXM mode,
    bytes will be nibblized in type 2. Else, bytes will be sent raw.

  @return
    void
*/
/*===========================================================================*/
void wci2_uart_send(
  const uint8* buffer,
  uint16       size
)
{
  uint16 dsm_return = 0;
  dsm_item_type* wci2_dsm_ptr = NULL;
  /*-----------------------------------------------------------------------*/
  WCI2_ASSERT(  NULL != buffer );
  if( wci2_uart_state.initialized )
  {
    /* allocate a DSM memory buffer, copy the data into the new buffer,
     * and hand the buffer over to SIO to transmit */
    dsm_return = dsm_pushdown( &wci2_dsm_ptr, (void *)buffer,
                               size, DSM_DS_SMALL_ITEM_POOL );
    WCI2_ASSERT( size == dsm_return );

    /* transmit the data over UART */
    sio_transmit( wci2_uart_state.stream_id, wci2_dsm_ptr );
    WCI2_MSG_1( MED, "Sent raw data array size %d B over uart", size );
    WCI2_TRACE( WCI2_TRC_UART_SND_MSG, size, 0, 0 );

    /* count all UART transmit events
       NOTE: Depending on type of message, multiple bytes could be sent across */
    wci2_counter_event( WCI2_CNT_UART_TX, size );
  }

  return;
} /* wci2_uart_send */

/*=============================================================================

  FUNCTION:  wci2_uart_basic_send

=============================================================================*/
/*!
    @brief
    Send the buffer as if in "basic" mode. If UART is set up in WCI2/CXM mode,
    this will be sent using busy-wait with direct reg. If UART is already in
    basic mode, this will be sent using the normal SIO driver.

    @return
    void
*/
/*===========================================================================*/
void wci2_uart_basic_send( 
  const uint8 *buffer, 
  uint16       size
)
{
  uint16 i;
  /*-----------------------------------------------------------------------*/
  if( wci2_uart_state.initialized )
  {
    if( !wci2_uart_state.cxm_hw )
    {
      wci2_uart_send( buffer, size );
    }
    else
    {
      for( i = 0; i < size; i++ )
      {
        wci2_uart_send_dir_char( buffer[i] );
      }
    }
  }

  return;
}

/*===========================================================================

  FUNCTION:  wci2_uart_send_dir_char

===========================================================================*/
/*!
  @brief
    Send a byte through the direct uart register

  @return
    void
*/
/*=========================================================================*/
void wci2_uart_send_dir_char( uint8 out_byte )
{
  sio_ioctl_param_type param;
  /*-----------------------------------------------------------------------*/
  if( wci2_uart_state.initialized )
  {
    param.cxm_tx_direct_char.character = out_byte;
    sio_ioctl(wci2_uart_state.stream_id, SIO_IOCTL_CXM_TX_DIRECT_CHAR, &param); 
    /* check to see if dir send succeeded */
    if( param.cxm_tx_direct_char.tx_direct_char_status != SIO_DONE_S )
    {
      /* send failed. register busy */
      WCI2_MSG_1( ERROR, "Failed to direct_reg_send byte 0x%x", out_byte );
    }
  }

  return;
}

/*===========================================================================

  FUNCTION:  wci2_is_uart_tx_full

===========================================================================*/
/*!
  @brief
    Use to check if there's room in the wci2_uart TX queue

  @return
    boolean FALSE if room
*/
/*=========================================================================*/
boolean wci2_is_uart_tx_full (
  void
)
{
  /*-----------------------------------------------------------------------*/
  return wci2_uart_state.wci2_tx_uart_high;
}

/*===========================================================================

  FUNCTION:  wci2_uart_npa_driver

===========================================================================*/
/*!
  @brief
    Used to deal with state transitions for wci2 uart npa resource.  When
    client voting causes a change in resource state, this function is called
    to impose the new state on hardware.

  @return
    npa_resource_state new npa state for uart resource
*/
/*=========================================================================*/
STATIC npa_resource_state wci2_uart_npa_driver ( 
  npa_resource      *resource,
  npa_client        *client,
  npa_resource_state state 
)
{
  wci2_msg_type_s wci2_msg;
  /*-----------------------------------------------------------------------*/
  if( wci2_uart_state.initialized )
  {
    ACQUIRE_WCI2_UART_MUTEX();
    WCI2_ASSERT( (state == WCI2_UART_NPA_REQ_OFF) || 
                 (state == WCI2_UART_NPA_REQ_ON) );
    if( state == (npa_resource_state) WCI2_UART_NPA_REQ_OFF )
    {
      /* start flushing tx buffer in prep for power down */
      wci2_uart_state.prev_state = wci2_uart_state.state;
      wci2_uart_state.state = WCI2_UART_FLUSHING;
      WCI2_TRACE( WCI2_TRC_UART_POWER_STATE, wci2_uart_state.state, 
                  wci2_uart_state.prev_state, 0 );
      sio_flush_tx(wci2_uart_state.stream_id, wci2_uart_flush_complete);

      /* processing continues in wci2_uart_flush_complete once flush is done */  
      WCI2_MSG_0( HIGH, "Triggering UART powering down, sending flushing request" );
    }
    else
    {
      wci2_uart_state.prev_state = wci2_uart_state.state;
      if( wci2_uart_state.state == WCI2_UART_FLUSHING )
      {
        /* if we are currently flushing, cancel and return to ON */
        sio_ioctl(wci2_uart_state.stream_id, SIO_IOCTL_CANCEL_FLUSH_TX, NULL);
        WCI2_MSG_0( HIGH, "Canceling pending flushing request powering UART back on" );
        wci2_uart_state.cnt_consec_pwrdwn_reqs = 0;
      }
      wci2_uart_state.state = WCI2_UART_ON;
      WCI2_TRACE( WCI2_TRC_UART_POWER_STATE, wci2_uart_state.state, 
                  wci2_uart_state.prev_state, 0 );

      /* npa vote for higher dynamic clock - 192MHz */
      WCI2_ASSERT( wci2_uart_dcvs_client != NULL );
      npa_issue_required_request( wci2_uart_dcvs_client, 192 );

      /* policy is now enforced, wakeup UART if it was powered down */
      sio_ioctl(wci2_uart_state.stream_id, SIO_IOCTL_WAKEUP, NULL);

      /* send out type1 to get updated type0 information from remote end */
      wci2_msg.type = WCI2_TYPE1;
      wci2_send_msg( &wci2_msg );

      WCI2_MSG_0( HIGH, "Powering up UART" );
    }

    RELEASE_WCI2_UART_MUTEX();
  }
  else
  {
    WCI2_MSG_0( ERROR, "Can't power up/down UART, not initialized" );
  }

  return state;
}

/*=============================================================================

  FUNCTION:  wci2_uart_set_diag

=============================================================================*/
/*!
    @brief
    Enable/Disable Diag RX mode
    Set UART into basic HW mode when running DIAG RX, then set back when done

    @return
    void
*/
/*===========================================================================*/
void wci2_uart_set_diag_rx( boolean enabled )
{
  static boolean       prev_mode = TRUE;
  boolean              new_mode;
  /*-----------------------------------------------------------------------*/
  /* save the previous UART state, then set up uart for DIAG RX */
  if( wci2_uart_state.initialized )
  {
    WCI2_MSG_1( HIGH, "DIAG RX set enabled=%d", enabled );
    if( enabled )
    {
      prev_mode = wci2_uart_state.cxm_hw;
      new_mode = FALSE;
    }
    else
    {
      new_mode = prev_mode;
    }
    wci2_uart_state.diag_active = enabled;
    wci2_uart_set_cxm_mode( new_mode );
  }

  return;
}

/*=============================================================================

  FUNCTION:  wci2_uart_set_adv_hw

=============================================================================*/
/*!
    @brief
    Enable/disable the advanced CXM hardware mods:
      - Direct RX
      - Automatic Type 2 [de]nibblize

    @return
    void
*/
/*===========================================================================*/
void wci2_uart_set_adv_hw( boolean enabled )
{
#ifdef WCI2_FEATURE_ADV_UART_HW
  sio_ioctl_param_type param;
  sio_status_type      ioctl_status = SIO_DONE_S;
#endif /* WCI2_FEATURE_ADV_UART_HW */
  /*-----------------------------------------------------------------------*/
#ifdef WCI2_FEATURE_ADV_UART_HW
  if( wci2_uart_state.initialized )
  {
    WCI2_MSG_1( HIGH, "Setting CXM_UART Adv HW: enabled=%d", enabled );
    ACQUIRE_WCI2_UART_MUTEX();
    wci2_uart_state.adv_hw = enabled;
    if( enabled )
    {
      /* enable direct read operation, type2 nib/denib in fifo */
      param.uart_cxm_type2_misalign_param.cxm_type2_misalign_cb = 
        (sio_vpu_func_ptr_type) wci2_uart_type2_misalign_cb;
      param.uart_cxm_type2_misalign_param.status_ptr = &ioctl_status;
      sio_ioctl( wci2_uart_state.stream_id, 
                SIO_IOCTL_ENABLE_CXM_TYPE2, &param );
      WCI2_ASSERT( ioctl_status == SIO_DONE_S );

      /* setup direct receive and associated message buffer */
      param.uart_cxm_type2_wci2_msg_param.cxm_type2_wci2_msg_cb = 
        (sio_vv_func_ptr_type) wci2_uart_dir_read_cb;
      param.uart_cxm_type2_wci2_msg_param.wci2_msg_ptr = wci2_uart_state.dir_read.msg_buf;
      param.uart_cxm_type2_wci2_msg_param.wci2_msg_length = WCI2_UART_DIR_READ_MSG_BUF_SIZE;
      param.uart_cxm_type2_wci2_msg_param.wci2_msg_intr_status_ptr = 
        &wci2_uart_state.dir_read.intr_status;
      param.uart_cxm_type2_wci2_msg_param.status_ptr = &ioctl_status;
      sio_ioctl( wci2_uart_state.stream_id, 
                 SIO_IOCTL_ENABLE_CXM_TYPE2_WCI2_MSG_EVENT, &param );
      WCI2_ASSERT( ioctl_status == SIO_DONE_S );

      /* restrict direct receive to only types of interest */
      param.cxm_type2_wci2_msg_intr_mask = ~(wci2_uart_state.dir_read.intr_enable);
      sio_ioctl( wci2_uart_state.stream_id, 
                 SIO_IOCTL_SET_TYPE2_WCI2_MSG_INTR_MASK, &param );
    }
    else
    {
      param.uart_cxm_type2_misalign_param.cxm_type2_misalign_cb = NULL;
      param.uart_cxm_type2_misalign_param.status_ptr = &ioctl_status;
      sio_ioctl( wci2_uart_state.stream_id, SIO_IOCTL_DISABLE_CXM_TYPE2, &param );
      WCI2_ASSERT( ioctl_status == SIO_DONE_S );
      param.uart_cxm_type2_wci2_msg_param.cxm_type2_wci2_msg_cb = NULL;
      param.uart_cxm_type2_wci2_msg_param.wci2_msg_ptr = NULL;
      param.uart_cxm_type2_wci2_msg_param.wci2_msg_length = 0;
      param.uart_cxm_type2_wci2_msg_param.wci2_msg_intr_status_ptr = NULL;
      param.uart_cxm_type2_wci2_msg_param.status_ptr = &ioctl_status;
      sio_ioctl( wci2_uart_state.stream_id, 
                 SIO_IOCTL_DISABLE_CXM_TYPE2_WCI2_MSG_EVENT, &param );
      WCI2_ASSERT( ioctl_status == SIO_DONE_S );
    }
    RELEASE_WCI2_UART_MUTEX();
  }
#else
  WCI2_MSG_0( ERROR, "No advanced CXM HW on this target" );
#endif /* WCI2_FEATURE_ADV_UART_HW */

  return;
}

/*=============================================================================

  FUNCTION:  wci2_uart_set_wci2_rx_types

=============================================================================*/
/*!
    @brief
    Set WCi2 types that we would like to receive (register for with SIO layer)

    @return
    void
*/
/*===========================================================================*/
void wci2_uart_set_wci2_rx_types( uint8 rx_mask )
{
#ifdef WCI2_FEATURE_ADV_UART_HW
  sio_ioctl_param_type param;
#endif /* WCI2_FEATURE_ADV_UART_HW */
  /*-----------------------------------------------------------------------*/
  if( wci2_uart_state.initialized )
  {
    /* restrict direct receive to only types of interest */
    /* type2 is done through normal driver instead of Direct RX interrupts,
     * so disable that bit. */
    WCI2_MSG_1( MED, "WCI2 RX en type mask=0x%x", rx_mask );
    wci2_uart_state.dir_read.intr_enable = rx_mask & ~(WCI2_UART_TYPE2_INTR_BIT);
#ifdef WCI2_FEATURE_ADV_UART_HW
    param.cxm_type2_wci2_msg_intr_mask = ~(wci2_uart_state.dir_read.intr_enable);
    sio_ioctl( wci2_uart_state.stream_id, 
               SIO_IOCTL_SET_TYPE2_WCI2_MSG_INTR_MASK, &param );
#endif /* WCI2_FEATURE_ADV_UART_HW */
  }

  return;
}

