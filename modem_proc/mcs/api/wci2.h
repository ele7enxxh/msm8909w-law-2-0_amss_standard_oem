#ifndef __WCI2_H__
#define __WCI2_H__
/*!
  @file wci2.h

  @brief
   APIs exposed by the WCI2 Layer to support external clients

*/

/*=============================================================================

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

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/wci2.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
           ckk     Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include <comdef.h>
#include <dsm.h>

/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/

/*===========================================================================

                        DATA TYPES AND MACROS

===========================================================================*/
/* WCI2 error types */
typedef enum {
  WCI2_E_SUCCESS = 0,
  WCI2_E_FAIL,
  WCI2_E_INVALID_ARG,
  WCI2_E_NO_MORE_CLIENTS,
  WCI2_E_UART_MODE_CONFLICT,
  WCI2_E_BAUD_CONFLICT,
  WCI2_E_FRAME_MODE_CONFLICT,
  WCI2_E_FRAME_CRC_MISMATCH,
  WCI2_E_FRAME_INVALID,
  WCI2_E_OUT_OF_MEM
} wci2_error_e;

/* max size for a WCI2 type2 frame (TX or RX) */
#define WCI2_TYPE2_MAX_FRAME_SIZE 128

/* WCI2 Type 2 RX frame struct */
typedef struct
{
  unsigned int ref_count;
  unsigned int len;
  uint8 buff[WCI2_TYPE2_MAX_FRAME_SIZE];
} wci2_t2_dyn_msg;

/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================

                    APIs TO SUPPORT BASIC UART CLIENT

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
);

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
);

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
);

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
);

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
);

/*=============================================================================

                 APIs TO SUPPORT WCI2 TYPE2 CXM UART CLIENT

=============================================================================*/
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
);

/*=============================================================================

  FUNCTION:  wci2_type2_slip_client_destroy

=============================================================================*/
/*!
    @brief
    Destroys the BASIC uart client (if present)
 
    @return
    wci2_error_e
*/
/*===========================================================================*/
wci2_error_e wci2_type2_slip_client_destroy (
  void
);

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
);

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
);

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
);

#endif /* __WCI2_H__ */

