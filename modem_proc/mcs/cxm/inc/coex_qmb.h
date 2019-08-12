#ifndef COEX_QMB_H
#define COEX_QMB_H
/*!
  @file
  coex_qmb.h

  @brief
  Coexistance service using COEX IDL v2 over QMB (with transport layer 
  WCI-2 type 2)

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/inc/coex_qmb.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/25/13   btl     Initial version

==========================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include <comdef.h>
#include <IxErrno.h>

/*=============================================================================

                      FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  coex_qmb_init

=============================================================================*/
/*!
    @brief
    Register this service as a client with QMB to open a connection to 
    the WCI2 UART transport

    @return
    wci2 client handle
*/
/*===========================================================================*/
uint32 coex_qmb_init(
  void
);

/*=============================================================================

  FUNCTION:  coex_qmb_deinit

=============================================================================*/
/*!
    @brief
    Close connection to QMI MB and release handle

    @return
    void
*/
/*===========================================================================*/
void coex_qmb_deinit(
  void
);

/*=============================================================================

  FUNCTION:  coex_qmb_send_msg

=============================================================================*/
/*!
    @brief
    Send a message over QMB (and by extension UART over WCI2 type2

    @detail
    Broadcasts to the bus. At this point the bus is a UART with one endpoint,
    so this is the same as sending a message to one recipient (doesn't 
    support addressing ). QMI will encode for us.

    @return
    void
*/
/*===========================================================================*/
void coex_qmb_publish_msg(
  unsigned int msg_id,
  void*        msg,
  unsigned int len
);

/*=============================================================================

  FUNCTION:  coex_qmb_push_deferred_req

=============================================================================*/
/*!
    @brief
    Helper API to push delayed/deferred QMB request onto the publisher's stack

    @return
    void
*/
/*===========================================================================*/
errno_enum_type coex_qmb_push_deferred_req (
  uint32           *deferred_req_id,   /*!< Deferred Request ID*/
  unsigned int      msg_id             /*!< Message Id */
);

/*=============================================================================

  FUNCTION:  coex_qmb_pop_deferred_req

=============================================================================*/
/*!
    @brief
    Helper API to pop delayed/deferred QMB request from the publisher's stack

    @return
    void
*/
/*===========================================================================*/
errno_enum_type coex_qmb_pop_deferred_req (
  uint32            deferred_req_id,   /*!< Deferred Request ID*/
  unsigned int      msg_id             /*!< Message Id */
);

/*=============================================================================

  FUNCTION:  coex_qmb_process_sig

=============================================================================*/
/*!
    @brief
    Get a message off the queue/buffer and process it/route it appropriately

    @return
    void
*/
/*===========================================================================*/
void coex_qmb_process_sig( void );

#endif /* include guard */
