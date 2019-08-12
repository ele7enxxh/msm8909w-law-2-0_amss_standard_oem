#ifndef CXM_DIAG_H
#define CXM_DIAG_H
/*!
  @file
  cxm_diag.h

  @brief
  APIs for CXM's DIAG interface

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/inc/cxm_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/20/12   btl     Update diag commands, add UART RX capability
           ckk     Initial implementation

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <IxErrno.h>

/*===========================================================================

                       DEFINITIONS & DATA TYPES

===========================================================================*/
#define FRAME_SYNC_GRFC_ENGINE  11
#define FRAME_SYNC_GRFC_NUM     38
#define RX_PRIORITY_GRFC_ENGINE 12
#define RX_PRIORITY_GRFC_NUM    39
#define TX_ACTIVE_GRFC_ENGINE   22
#define TX_ACTIVE_GRFC_NUM      37

#define CXM_UART_BUFF_LEN       128

#define CXM_DIAG_BURST_LENGTH   4

#define CXM_NUM_NOTIFI_SUBFRS   5
#define CXM_USTMR_TIME_INVALID  0xFFFFFFFF

typedef enum
{
  CXM_DIAG_SUBFR_NOTIFI
} cxm_diag_test_type;

typedef struct
{
  cxm_diag_test_type test;
} cxm_diag_arb_msg_info_type;

typedef struct
{
  uint8 index;
  uint32 timing_ustmr[CXM_NUM_NOTIFI_SUBFRS];
  boolean transmit[CXM_NUM_NOTIFI_SUBFRS];
} cxm_diag_subfr_notifi_type;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  cxm_diag_init

===========================================================================*/
/*!
  @brief
    To initialize CXM's DIAG interface

  @return
    None
*/
/*=========================================================================*/
void cxm_diag_init (
  void
);

/*===========================================================================

  FUNCTION:  cxm_diag_deinit

===========================================================================*/
/*!
  @brief
    To de-initialize CXM's DIAG interface

  @return
    None
*/

/*=========================================================================*/
void cxm_diag_deinit (
  void
);

/*===========================================================================

  FUNCTION:  cxm_diag_send_arb_msgs_timer_handler

===========================================================================*/
/*!
  @brief
    timer handler to periodically send bytes for cxm_diag_send*()

  @return
    void
*/

/*=========================================================================*/
void cxm_diag_send_arb_msgs_timer_handler (
  void
);

/*===========================================================================

  FUNCTION:  cxm_diag_process_uart_rx_data

===========================================================================*/
/*!
  @brief
    routes data appropriately within cxm_diag

  @return
    TRUE if data was processed, FALSE if cxm_diag didn't want it
*/

/*=========================================================================*/
boolean cxm_diag_process_uart_rx_data (
  uint8
);

#endif /* CXM_DIAG_H */
