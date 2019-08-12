#ifndef WCI2_CLIENT_H
#define WCI2_CLIENT_H
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/wci2/inc/wci2_client.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
06/16/14   btl     Initial version

==========================================================================*/
/*=============================================================================

                      INCLUDE FILES

=============================================================================*/
#include <IxErrno.h>
#include <dsm.h>
#include <wci2.h>
#include "wci2_core.h"

#define WCI2_TYPE_BITS_MASK           0x07 //0b00000111
#define WCI2_BITS_TO_SHIFT_DATA       3

/* extract the WCI2 type info from a byte */
#define WCI2_GET_TYPE(byte) ((byte)&WCI2_TYPE_BITS_MASK)
/* extract the WCI2 data payload info from a byte */
#define WCI2_GET_PAYLOAD(byte) ((byte)>>WCI2_BITS_TO_SHIFT_DATA)

/*===========================================================================

                        DATA TYPES AND MACROS

===========================================================================*/
/* WCI2 (CXM) UART's supported baudrates */
typedef enum {
  WCI2_UART_BAUD_115200,
  WCI2_UART_BAUD_2000000,
  WCI2_UART_BAUD_3000000
} wci2_uart_baud_type_e;

/* WCI2 (CXM) UART's supported modes (basic & cxm/wci2) */
typedef enum {
  WCI2_CLIENT_MODE_BASIC,
  WCI2_CLIENT_MODE_WCI2
} wci2_client_mode_e;

/* registered_type_mask in wci2_client_open_s */
#define WCI2_REG_TYPE_NONE 0x00
#define WCI2_REG_TYPE_0    0x01
#define WCI2_REG_TYPE_1    0x02
#define WCI2_REG_TYPE_2    0x04
#define WCI2_REG_TYPE_3    0x08
#define WCI2_REG_TYPE_4    0x10
#define WCI2_REG_TYPE_5    0x20
#define WCI2_REG_TYPE_6    0x40
#define WCI2_REG_TYPE_7    0x80

typedef enum {
  WCI2_FRAME_MODE_NONE,
  WCI2_FRAME_MODE_HDLC,
  WCI2_FRAME_MODE_SLIP
} wci2_frame_mode_e;

typedef enum {
  WCI2_UART_NPA_REQ_OFF,
  WCI2_UART_NPA_REQ_ON
} wci2_uart_npa_req_type_e;

/* can be null pointers if you don't need these callbacks.
 * Since these are in SIO context, should be as short as possible */

/* each client must free this DSM item when it is done */
typedef void (*wci2_basic_rx_cb)( dsm_item_type *dsm_ptr );
/* this is a reference to a global array[8] (must not be modified), 
 * one byte per WCI2 type. Each time any type is received, this 
 * callback will be called. Client's responsible for seeing what changed.
 * NOTE: status bits considered cleared when cb returns, so save it */
typedef void (*wci2_dir_rx_cb)( uint8 *buff, uint8 status );
/* upon receiving this cb, should queue the msg and signal
 * your task to process it in your context. Then call wci2_frame_unframe 
 * on the message. */
typedef void (*wci2_t2_rx_cb)( wci2_t2_dyn_msg *msg );

typedef struct 
{
  wci2_uart_baud_type_e baud;        /* baud rate */
  wci2_client_mode_e    mode;        /* basic (vanilla uart) or wci2? */
  uint8                 type_mask;   /* if wci2 mode, what types do you 
                                        want to register for? */
  wci2_frame_mode_e     frame_mode;  /* if reg for type2, HDLC or SLIP? */
  wci2_basic_rx_cb      basic_rx_cb; /* callback to recv in basic uart mode */
  wci2_dir_rx_cb        dir_rx_cb;   /* callback to recv non-t2 wci2 msgs */
  wci2_t2_rx_cb         t2_rx_cb;    /* callback to recv type2 framed msgs */
} wci2_client_open_s;

/* addresses used to mux type-2 messages in HDLC framing */
typedef enum {
  WCI2_TYPE2_ADDRESS_QMB_COEX = 0x01,
  WCI2_TYPE2_ADDRESS_MAX
} wci2_type2_address_e;

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
);

/*=============================================================================

  FUNCTION:  wci2_client_deregister

=============================================================================*/
/*!
    @brief
    Deregister the client -- tear down any resources client was using

    @return
    void
*/
/*===========================================================================*/
wci2_error_e wci2_client_deregister( 
  uint32 handle 
);

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
);

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
);

/*=============================================================================

  FUNCTION:  wci2_client_notify_rx_t2

=============================================================================*/
/*!
    @brief
    Distribute a received t2 frame to all the registered clients (through
    their callbacks. For internal use only

    @return
    void
*/
/*===========================================================================*/
void wci2_client_notify_rx_t2( 
  wci2_t2_dyn_msg *msg 
);

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
void wci2_client_notify_rx_dir( uint8 *buff, uint8 status );

/*=============================================================================

  FUNCTION:  wci2_client_notify_rx_basic

=============================================================================*/
/*!
    @brief
    Distribute the received packet to all the rgistered clients through
    their callbacks. For internal use only. 
    NOTE: clients should not do processing from their callbacks.

    @return
    void
*/
/*===========================================================================*/
void wci2_client_notify_rx_basic(
  dsm_item_type **dsm_ptr
);

/*=============================================================================

  FUNCTION:  wci2_client_t2_frame_and_send

=============================================================================*/
/*!
    @brief
    Frame the message into the set frame type (HDLC or SLIP) and transmit
    over the UART in WCi-2 Type2 format (nibblized).

    @return
    void
*/
/*===========================================================================*/
wci2_error_e wci2_client_t2_frame_and_send( 
  uint32               handle,
  const void*          msg, 
  uint16               msg_size, 
  wci2_type2_address_e addr 
);

#endif /* WCI2_CORE_H */

