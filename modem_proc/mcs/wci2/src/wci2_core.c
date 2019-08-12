/*!
  @file
  wci2_core.c

  @brief
  Service for sending WCI-2 messages over UART

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/wci2/src/wci2_core.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
01/14/15   ckk     Add support for type7[ant_tx]
07/12/14   ckk     Add support for type4 & type7[pwr state]
02/26/14   btl     Add type 7
10/15/13   ckk     Moving to WCI2
06/27/13   btl     Initial version

==========================================================================*/

/*=============================================================================

                      INCLUDE FILES

=============================================================================*/
#include <comdef.h>
#include "wci2_uart.h"
#include "wci2_core.h"
#include "wci2_utils.h"
#include "wci2_trace.h"

/*=============================================================================

                      DEFINES, CONSTANTS, GLOBALS

=============================================================================*/
#define WCI2_TYPE2_BITS_TO_SHIFT_DATA 4
#define WCI2_TYPE2_LO_NIBBLE_MASK 0x02
#define WCI2_TYPE2_HI_NIBBLE_MASK 0x0a

/* set data 5th bit in wci2 type 6 message if this is a TX subframe */
#define WCI2_TYPE6_SF_TX_MASK 0x80

/* set data bits for wci2 type 7 message */
#define WCI2_TYPE7_MDM_CONN_ST_BIT    0x08
#define WCI2_TYPE7_MDM_PWR_ST_BIT     0x10
#define WCI2_TYPE7_MDM_TX_ANT_SEL_BIT 0x20

/* max size for nibblizing type2 frame */
#define WCI2_TYPE2_NIBBLIZE_BUF_SIZE 2*WCI2_TYPE2_MAX_FRAME_SIZE

/*=============================================================================

                      INTERNAL FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  cxm_wci2_type2_nibblize

=============================================================================*/
/*!
    @brief
    Splits input byte stream into two nibbles per byte

    @return
    void
*/
/*===========================================================================*/
void cxm_wci2_type2_nibblize( const uint8 *in_buffer, uint16 size )
{
  uint8  out_buffer[WCI2_TYPE2_NIBBLIZE_BUF_SIZE];
  uint16 out_size;
  uint16 in_pos, out_pos = 0;
  /*-----------------------------------------------------------------------*/
  WCI2_ASSERT( NULL != in_buffer && size <= WCI2_TYPE2_MAX_FRAME_SIZE );

  /* determine the needed size (2x the input array size) */
  out_size = size * 2;

  /* split bytes into nibbles and pack in the wci-2 type 2 bits */
  for( in_pos = 0; in_pos < size; in_pos++ )
  {
    /* low nibble goes first */
    out_buffer[out_pos++] = (uint8) (((in_buffer[in_pos] & 0x0F) << 
                            WCI2_TYPE2_BITS_TO_SHIFT_LSN) | WCI2_TYPE2);
    /* next goes upper nibble */
    out_buffer[out_pos++] = (uint8) (((in_buffer[in_pos] & 0xF0) <<
                            WCI2_TYPE2_BITS_TO_SHIFT_MSN) | 
                            WCI2_TYPE2 | WCI2_TYPE2_HSN_BIT);
  }

  wci2_uart_send( out_buffer, out_size );

  return;
}

/*=============================================================================

                      EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  wci2_send_msg

=============================================================================*/
/*!
    @brief
    send a message over CXM_UART in WCI2 format

    @return
    void
*/
/*===========================================================================*/
void wci2_send_msg( const wci2_msg_type_s* msg )
{
  uint8 byte_to_send = 0x00;
  /*-----------------------------------------------------------------------*/
  WCI2_ASSERT( WCI2_TYPE0 <= msg->type && WCI2_TYPE7 >= msg->type );
  switch( msg->type )
  {
    /* prep the byte in WCI-2 format, then transmit the data over UART */
    case WCI2_TYPE1:
      byte_to_send = (uint8) ((WCI2_TYPE1_RESEND_REAL_TIME<<WCI2_BITS_TO_SHIFT_DATA) | WCI2_TYPE1);
      wci2_uart_send_dir_char( byte_to_send );
      wci2_counter_event( WCI2_CNT_UART_TX_WCI2_TYPE1, byte_to_send );
      break;

    case WCI2_TYPE2:
      #ifdef WCI2_FEATURE_ADV_UART_HW
      wci2_uart_send( msg->data.type2_msg.buffer, msg->data.type2_msg.length );
      #else
      cxm_wci2_type2_nibblize( msg->data.type2_msg.buffer, msg->data.type2_msg.length );
      #endif
      break;

    case WCI2_TYPE3:
      if( WCI2_TYPE3_MAX_INACT_DURN_MS <= msg->data.type3_inact_durn )
      {
        /* indicates WCI2 reserved infinite time of 0b11111 */
        byte_to_send = (uint8) (31<<WCI2_BITS_TO_SHIFT_DATA | WCI2_TYPE3);
      }
      else
      {
        byte_to_send = (uint8) (((msg->data.type3_inact_durn/5)<<WCI2_BITS_TO_SHIFT_DATA) | WCI2_TYPE3);
      }
      wci2_uart_send_dir_char( byte_to_send );
      wci2_counter_event( WCI2_CNT_UART_TX_WCI2_TYPE3, byte_to_send );
      break;

    case WCI2_TYPE4:
      byte_to_send = (uint8) ((msg->data.type4_freq_index<<WCI2_BITS_TO_SHIFT_DATA) | WCI2_TYPE4);
      wci2_uart_send_dir_char( byte_to_send );
      wci2_counter_event( WCI2_CNT_UART_TX_WCI2_TYPE4, byte_to_send );
      break;

    case WCI2_TYPE6:
      byte_to_send = (uint8)(((msg->data.type6_adv_tx_sfn.sfn%10)<<WCI2_BITS_TO_SHIFT_DATA) | WCI2_TYPE6);
      if( msg->data.type6_adv_tx_sfn.tx )
      {
        byte_to_send |= WCI2_TYPE6_SF_TX_MASK;
      }
      wci2_uart_send_dir_char( byte_to_send );
      wci2_counter_event( WCI2_CNT_UART_TX_WCI2_TYPE6, byte_to_send );
      break;

    case WCI2_TYPE7:
      byte_to_send = (uint8)(msg->data.type7_wwan_state.wwan_tx_active ? 
        WCI2_TYPE7_MDM_CONN_ST_BIT : byte_to_send);
      byte_to_send = (uint8)(msg->data.type7_wwan_state.wwan_tx_pwr_active ? 
        byte_to_send | WCI2_TYPE7_MDM_PWR_ST_BIT : byte_to_send);
      byte_to_send = (uint8)(msg->data.type7_wwan_state.tx_ant_sel ?
        byte_to_send | WCI2_TYPE7_MDM_TX_ANT_SEL_BIT : byte_to_send);
      byte_to_send = byte_to_send | WCI2_TYPE7;
      wci2_uart_send_dir_char( byte_to_send );
      wci2_counter_event( WCI2_CNT_UART_TX_WCI2_TYPE7, byte_to_send );
      break;

    case WCI2_TYPE0:
    case WCI2_TYPE5:
      WCI2_MSG_1( ERROR, "UNSUPPORTED WCI2 message msg type = %x", msg->type );
      wci2_counter_event( WCI2_CNT_UART_TX_WCI2_TYPE_UNSUPPORTED, msg->type );
      break;

    default:
      WCI2_MSG_1( ERROR, "INVALID WCI2 message msg type = %x", msg->type );
      break;
  }

  WCI2_TRACE( WCI2_TRC_TX_WCI2, byte_to_send, msg->type, 0 );
  WCI2_MSG_2( MED, "Sending WCI2 message (type=%d byte=%x)", 
              msg->type, byte_to_send );

  return;
}

