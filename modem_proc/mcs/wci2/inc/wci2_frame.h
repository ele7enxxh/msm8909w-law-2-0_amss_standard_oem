#ifndef WCI2_FRAME_H
#define WCI2_FRAME_H
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/wci2/inc/wci2_frame.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/21/14   btl     Add SLIP
06/16/14   btl     Initial version

==========================================================================*/
/*=============================================================================

                      INCLUDE FILES

=============================================================================*/
#include <IxErrno.h>
#include "wci2_client.h"

/*===========================================================================

                        DATA TYPES AND MACROS

===========================================================================*/
/* see WCI2_CORE.H for the Type2 frame size definitions */

/*=============================================================================

                      FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  wci2_frame_set_mode

=============================================================================*/
/*!
    @brief
    Set type2 framing mode into HDLC or SLIP protocol. Only one can be
    active at a time.

    @return
    void
*/
/*===========================================================================*/
void wci2_frame_set_mode( wci2_frame_mode_e mode );

/*=============================================================================

  FUNCTION:  wci2_frame_buffer_rx

=============================================================================*/
/*!
    @brief
    Buffer until start/stop flag is encountered. When this happens, return
    a pointer to the complete buffer (must be freed when done)

    @return
    wci2_t2_dyn_msg ptr
*/
/*===========================================================================*/
wci2_t2_dyn_msg* wci2_frame_buffer_rx( uint8 byte );

/*=============================================================================

  FUNCTION:  wci2_frame_unframe

=============================================================================*/
/*!
    @brief
    Given a pointer to a frame, unframe and retreive the message
    inside. Also verify CRC.

    NOTE: this will return a pointer into the original buffer. DO NOT
          modify it and do not free it.

    @detail
    Start/Stop flags and escape characters should already be stripped out

    @return
    void
*/
/*===========================================================================*/
wci2_error_e wci2_frame_unframe( 
  uint8  *in_buff,  /**< Input - buffer containing framed msg */
  uint16  in_size,  /**< Input - size of framed msg */
  uint8 **out_buff, /**< Output- buffer containing unframed msg */
  uint16 *out_size, /**< Output- size of unframed msg */
  uint16 *out_addr  /**< Output- address from frame, IFF HDLC, else ret 0 */
);

/*=============================================================================

                      FUNCTION DEFINITIONS: HDLC

=============================================================================*/

/*=============================================================================

  FUNCTION:  wci2_hdlc_frame_and_escape

=============================================================================*/
/*!
    @brief
    Frame the given raw byte buffer using HDLC-like framing. To be used for
    transporting messages over UART.

    @detail
    * The maximum message size is defined by WCI2_TYPE2_MAX_MSG_SIZE
    Frame Structure:
      +----------+----------+----------+----------+
      |   Flag   | Payload  |   CRC    |   Flag   |
      |   0x7e   |    *     | 16 bits  |   0x7e   |
      +----------+----------+----------+----------+
       - only one flag sequence needed btw two frames
       - CRC transmitted least-significant octet first (highest term coefficients)
       - CRC does not include Flags or CRC fields. i.e. escape after CRC calculation
       - closing flag is used to locate CRC field
       - recommended that transmitters send a new open flag sequence when an 
         appreciable time has elapsed (1sec?)
       - payload must be at least 4 bytes?? for CRC?

    @return
    size of frame in bytes
*/
/*===========================================================================*/
wci2_error_e wci2_hdlc_frame_and_escape ( 
  const void *msg,             /**< In - frame payload */
  uint16      msg_size,        /**< In - size of msg payload in bytes */
  uint8       address,         /**< In - address field of frame */
  uint8      *frame_buff,      /**< In/Out - buffer to write frame to */
  uint16      frame_buff_size, /**< In - size of frame buffer in bytes */
  uint16     *frame_size       /**< Out - size of frame after framing */
);

/*=============================================================================

  FUNCTION:  wci2_hdlc_unframe

=============================================================================*/
/*!
    @brief
    Take the WCI-2 HDLC-like frame and strip out all the frame info (while
    validating the frame), returning a buffer containing the original raw
    bytestream. If frame is invalid, will silently drop.

    @detail
    Start/Stop flags and escape characters should already be stripped out

    @return
    boolean frame_valid
*/
/*===========================================================================*/
wci2_error_e wci2_hdlc_unframe( 
  uint8  *in_buff,  /**< Input - buffer containing framed msg */
  uint16  in_size,  /**< Input - size of framed msg */
  uint8 **out_buff, /**< Output- buffer containing unframed msg */
  uint16 *out_size, /**< Output- size of unframed msg */
  uint16 *out_addr  /**< Output- address from frame, IFF HDLC, else ret 0 */
);

/*=============================================================================

                      FUNCTION DEFINITIONS: SLIP

=============================================================================*/

/*=============================================================================

  FUNCTION:  wci2_slip_frame_and_escape

=============================================================================*/
/*!
    @brief
    Frame the given raw byte buffer using HDLC-like framing. To be used for
    transporting messages over UART.

    @detail
    * The maximum message size is defined by WCI2_TYPE2_MAX_MSG_SIZE
    Frame Structure:
      +----------+----------+----------+----------+
      |   Flag   | Payload  |   CRC    |   Flag   |
      |   0x7e   |    *     | 16 bits  |   0x7e   |
      +----------+----------+----------+----------+
       - only one flag sequence needed btw two frames
       - CRC transmitted least-significant octet first (highest term coefficients)
       - CRC does not include Flags or CRC fields. i.e. escape after CRC calculation
       - closing flag is used to locate CRC field
       - recommended that transmitters send a new open flag sequence when an 
         appreciable time has elapsed (1sec?)
       - payload must be at least 4 bytes?? for CRC?

    @return
    size of frame in bytes
*/
/*===========================================================================*/
wci2_error_e wci2_slip_frame_and_escape ( 
  const void *msg,             /**< In - frame payload */
  uint16      msg_size,        /**< In - size of msg payload in bytes */
  uint8      *frame_buff,      /**< In/Out - buffer to write frame to */
  uint16      frame_buff_size, /**< In - size of frame buffer in bytes */
  uint16     *frame_size       /**< Out - size of frame after framing */
);

/*=============================================================================

  FUNCTION:  wci2_slip_unframe

=============================================================================*/
/*!
    @brief
    Take the WCI-2 HDLC-like frame and strip out all the frame info (while
    validating the frame), returning a buffer containing the original raw
    bytestream. If frame is invalid, will silently drop.

    @detail
    Start/Stop flags and escape characters should already be stripped out

    @return
    boolean frame_valid
*/
/*===========================================================================*/
wci2_error_e wci2_slip_unframe( 
  uint8  *in_buff,   /**< Input - buffer containing framed msg */
  uint16  in_size,   /**< Input - size of framed msg */
  uint8 **out_buff,  /**< Output- buffer containing unframed msg */
  uint16 *out_size,  /**< Output- size of unframed msg */
  uint8  *seq_n,     /**< Output- sequence number */
  uint8  *chksum_tst /**< Output- checksum_tst */
);

#endif /* WCI2_FRAME_H */
