#ifndef WCI2_UART_H
#define WCI2_UART_H
/*!
  @file
  wci2_uart.h

  @brief
  APIs for WCI-2's UART interface

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/wci2/inc/wci2_uart.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
           ckk     Initial implementation

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <comdef.h>
#include <IxErrno.h>
#include <atomic_ops.h>
#include "wci2_client.h"

/*===========================================================================

                        DATA TYPES AND MACROS

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  wci2_uart_init

===========================================================================*/
/*!
  @brief
    To initialize the UART driver

  @return
    errno_enum_type
*/
/*=========================================================================*/
void wci2_uart_init(
  void
);

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
);

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
);

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
errno_enum_type wci2_uart_deinit(
  void
);

/*===========================================================================

  FUNCTION:  wci2_uart_send

===========================================================================*/
/*!
  @brief
    Handler for sending data over the CxM UART

  @return
    void
*/
/*=========================================================================*/
void wci2_uart_send( 
 const uint8* buffer,
 uint16       size
);

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
);

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
);

/*===========================================================================

  FUNCTION:  wci2_is_uart_tx_full

===========================================================================*/
/*!
  @brief
    Use to check if there's room in the wci2_uart TX queue

  @return
    boolean False if room
*/
/*=========================================================================*/
boolean wci2_is_uart_tx_full(
  void
);

/*===========================================================================

  FUNCTION:  wci2_uart_process_rx_data

===========================================================================*/
/*!
  @brief
    Route available wci2_uart RX data to the appropriate place

  @return
    void
*/
/*=========================================================================*/
void wci2_uart_process_rx_data(
  void
);

/*===========================================================================

  FUNCTION:  wci2_uart_send_dir_char

===========================================================================*/
/*!
  @brief
    Send char through direct uart register

  @return
    void
*/
/*=========================================================================*/

void wci2_uart_send_dir_char(
  uint8 byte_to_send
);

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
void wci2_uart_set_cxm_mode( 
  boolean enable 
);

/*=============================================================================

  FUNCTION:  wci2_uart_set_diag

=============================================================================*/
/*!
    @brief
    Enable/Disable Diag RX mode

    @return
    void
*/
/*===========================================================================*/
void wci2_uart_set_diag_rx( 
  boolean enabled
);

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
void wci2_uart_set_adv_hw( 
  boolean enabled 
);

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
void wci2_uart_set_wci2_rx_types( 
  uint8 rx_mask
);

#endif /* WCI2_UART_H */
