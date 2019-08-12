#ifndef WCI2_DIAG_H
#define WCI2_DIAG_H
/*!
  @file
  wci2_diag.h

  @brief
  APIs for WCI2's DIAG interface

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/wci2/inc/wci2_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
           btl     Initial implementation

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <IxErrno.h>
#include <diagpkt.h>
#include <diagcmd.h>

/*===========================================================================

                       DEFINITIONS & DATA TYPES

===========================================================================*/
/* list of wci2 diag request indexes */
/* gaps indicate commands present on other branches, but not here */
typedef enum
{
  WCI2_ENABLE_UART                = 0x00,
  WCI2_SEND_ARBITRARY_MSG_SEQ     = 0x02,
  WCI2_SEND_8_ARBITRARY_MSGS      = 0x03,
  WCI2_STOP_CURRENT_TEST          = 0x04,
  WCI2_SET_UART_LOOPBACK_MODE     = 0x05,
  WCI2_START_ORIGINATOR_MODE      = 0x06,
  WCI2_GET_ORIGINATOR_MODE_RESULT = 0x07,
  WCI2_START_RECEIVER_MODE        = 0x08,
  WCI2_SET_UART_BAUD_RATE         = 0x09,
  WCI2_SET_UART_MODE              = 0x0A,
  WCI2_SET_UART_ADV_HW            = 0x0B,
  WCI2_SET_UART_REG_WCI2_RX       = 0x0C,
  WCI2_SET_EXTERNAL_CLIENT        = 0x0D,
  WCI2_EXT_CLIENT_SEND_PKT        = 0x0E,
  WCI2_EXT_CLIENT_RECV_PKT        = 0x0F
} wci2_diag_table_id_type;

/*--------------------------------------------------------------------------
                          SEND ARBITRARY WCI2 MSG SEQ

This interface provides a way for a test tool to trigger mechanism to send
out various sequences  of arbitrary wCI2 type bytes over the CXM UART. 
Sends continously until told to stop.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(WCI2, WCI2_SEND_ARBITRARY_MSG_SEQ)
  uint8           seq_index;
  uint16          time_apart_ms;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCI2, WCI2_SEND_ARBITRARY_MSG_SEQ)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                           SEND 8 ARBITRARY WCI2 MSGS

This interface provides a way for a test tool to send out 8 random bytes
(specified by tester) over the WCI2 UART. Sends continuosly until told to 
stop.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(WCI2, WCI2_SEND_8_ARBITRARY_MSGS)
  uint8           byte0;
  uint8           byte1;
  uint8           byte2;
  uint8           byte3;
  uint8           byte4;
  uint8           byte5;
  uint8           byte6;
  uint8           byte7;
  uint16          time_apart_ms;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCI2, WCI2_SEND_8_ARBITRARY_MSGS)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         START ORIGINATOR MODE

This interface provides a way to send CXM_DIAG_BURST_LENGTH (4) bytes and 
confirm the same were received correctly. In other words, MDM acts as 
originator in the test
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(WCI2, WCI2_START_ORIGINATOR_MODE)
  uint8           byte0;
  uint8           byte1;
  uint8           byte2;
  uint8           byte3;
  uint16          time_apart_ms;
  uint16          num_times;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCI2, WCI2_START_ORIGINATOR_MODE)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         GET ORIGINATOR MODE RESULT

This interface provides a way to collect the results of the previously-run
originator mode test
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(WCI2, WCI2_GET_ORIGINATOR_MODE_RESULT)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCI2, WCI2_GET_ORIGINATOR_MODE_RESULT)
  uint16          num_run;
  uint16          num_failed;
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         START RECEIVER MODE

Start test mode where MDM acts in receiver mode, looping back data
through the UART

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(WCI2, WCI2_START_RECEIVER_MODE)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCI2, WCI2_START_RECEIVER_MODE)
  errno_enum_type error;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                           STOP CURRENT TEST

This interface provides a way for a test tool to stop whatever test is
currently occupying the wci2 task
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(WCI2, WCI2_STOP_CURRENT_TEST)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(WCI2, WCI2_STOP_CURRENT_TEST)
DIAGPKT_RSP_END

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  wci2_diag_init

===========================================================================*/
/*!
  @brief
    To initialize WCI2's DIAG interface

  @return
    None
*/
/*=========================================================================*/
void wci2_diag_init( void );

/*===========================================================================

  FUNCTION:  wci2_diag_deinit

===========================================================================*/
/*!
  @brief
    To de-initialize WCI2's DIAG interface

  @return
    None
*/

/*=========================================================================*/
void wci2_diag_deinit (
  void
);

/*===========================================================================

  FUNCTION:  wci2_diag_process_uart_rx_data

===========================================================================*/
/*!
  @brief
    routes data appropriately within wci2_diag

  @return
    TRUE if data was processed, FALSE if wci2_diag didn't want it
*/

/*=========================================================================*/
void wci2_diag_process_uart_rx_data (
  uint8 byte
);

/*===========================================================================

  FUNCTION:  wci2_diag_send_arbitrary_wci2_msg_seq

===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to send arbitrary stream of WCI2 byte message
    note: cannot run at same time as wci2_diag_send_8_arbitrary_wci2_msgs,
          and loops until signalled to stop

  @return
    DIAG_RSP_PKT pointer 
*/

/*=========================================================================*/
PACK(void *) wci2_diag_send_arbitrary_wci2_msg_seq (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
);

/*===========================================================================

  FUNCTION:  wci2_diag_send_8_arbitrary_wci2_msgs

===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to send tester provided 8 arbitrary WCI2 bytes
    note: cannot run at same time as wci2_diag_send_arbitrary_wci2_msg_seq,
          and loops until signalled to stop

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
PACK(void *) wci2_diag_send_8_arbitrary_wci2_msgs (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
);

/*===========================================================================

  FUNCTION:  wci2_diag_stop_current_test


===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to stop sending test msg bytes to wci2_uart.

  @return
    DIAG_RSP_PKT pointer
*/
/*=========================================================================*/
PACK(void *) wci2_diag_stop_current_test (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
);

/*===========================================================================

  FUNCTION:  wci2_diag_start_originator_mode

===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to send WCI2_DIAG_BURST_LENGTH (4) bytes and confirm the same were
    received correctly

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
PACK(void *) wci2_diag_start_originator_mode (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
);

/*===========================================================================

  FUNCTION:  wci2_diag_get_originator_mode_result

===========================================================================*/
/*!
  @brief
    To handle DIAG CMD request to check results of previously-issued
    originator_mode command

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
PACK(void *) wci2_diag_get_originator_mode_result (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
);

/*===========================================================================

  FUNCTION:  wci2_diag_start_receiver_mode

===========================================================================*/
/*!
  @brief
    start test mode where MDM acts in receiver mode, looping back data
    through the UART

  @return
    DIAG_RSP_PKT pointer
*/

/*=========================================================================*/
PACK(void *) wci2_diag_start_receiver_mode (
  PACK(void*) req_pkt, /*!< request packet */
  uint16 pkt_len       /*!< request packet length */
);

#endif /* WCI2_DIAG_H */
