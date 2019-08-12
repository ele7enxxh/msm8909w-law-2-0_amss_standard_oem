/*!
  @file
  rf_control_msg.h

  @brief
  Message router UMIDs header for RF task control. This file defines
  interfaces in the RF task control and query. This is a common header for
  rf driver of all technologies.

  @note
  This file must be named with a _msg.h suffix for the build environment
  to correctly process the messages defined within.

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfwcdma_control_msg.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/08/13    jj     add item rfa_rf_wcdma_set_sar_limit_ind_s to rfwcdma_msg_u
04/29/12    ac     compiler warning fix
02/06/12    ka     Initial version

===========================================================================*/

#ifndef RFWCDMA_CONTROL_MSG_H
#define RFWCDMA_CONTROL_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "appmgr_msg.h"
#include "rfa_msgr.h"
#include "msgr.h"
#include "msgr_types.h"
#include "rfwcdma_msg.h"

/*===========================================================================

             EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/****************************************************************************
                RF Task Supervisory
****************************************************************************/



/*! @brief Union of all RF receiving msgs that RF task can receive
*/

/* Note: The msgr_receive( ) API requires providing the size of the buffer 
to accommodate all receiving messages. Constrained by this buffer size requirement, 
we have to define a union of all receiving messages to consider the largest message 
size for copying of received messages by message router. 

This is dictated by the underlying OS support.  On UMB, we used Qube, which allocated 
the buffer internally, which then required the user to free it after getting the msg out.  
We are now using a Posix interface, which requires that the client provide the buffer.  */

/*! @todo May need to move this to a common_msg.h as the union of 
all receiving msgs (inclduing control, RF Tech subsystem msgs) is required 
due to the restriction on the msgr_receive( ) interface, which needs msgr
buffer size as the argument  */


typedef union
{

  /* Supervisor Requests */
  msgr_spr_loopback_s               msgr_spr_loopback;
  msgr_spr_loopback_reply_s         msgr_spr_loopback_reply;

  /* WCDMA subsystem messages */
  /*! @todo: Future RF receive message types need to be present here*/
  rfa_rf_wcdma_enter_mode_req_s         rfa_rf_wcdma_enter_mode;
  rfa_rf_wcdma_exit_mode_req_s          rfa_rf_wcdma_exit_mode;
  rfa_rf_wcdma_cmd_dispatch_req_s       rfa_rf_wcdma_cmd_dispatch_mode;
  rfa_rf_wcdma_tune_to_multi_chan_req_s rfa_rf_wcdma_tune_to_multi_chan_mode;
  rfa_rf_wcdma_enable_tx_req_s          rfa_rf_wcdma_enable_tx;
  rfa_rf_wcdma_disable_tx_req_s         rfa_rf_wcdma_disable_tx;
  rfa_rf_wcdma_sleep_req_s              rfa_rf_wcdma_sleep;
  rfa_rf_wcdma_wakeup_req_s             rfa_rf_wcdma_wakeup;
  rfa_rf_wcdma_enable_rx_req_s          rfa_rf_wcdma_enable_rx;
  rfa_rf_wcdma_disable_rx_req_s         rfa_rf_wcdma_disable_rx;
  rfa_rf_wcdma_power_clock_on_req_s     rfa_rf_wcdma_power_clock_on;
  rfa_rf_wcdma_power_clock_off_req_s    rfa_rf_wcdma_power_clock_off;
  rfa_rf_wcdma_init_req_s               rfa_rf_wcdma_init;
  rfa_rf_wcdma_init_tx_req_s            rfa_rf_wcdma_init_tx;
  rfa_rf_wcdma_set_sar_limit_ind_s      rfa_rf_wcdma_set_sar_limit;

} rfwcdma_msg_u;

/*!
  @brief
  Outcome of the dispatch. 

  @details The different outcomes of the dispatch to RF task used by LTE MC Dispatcher and LTE FTM Calibration Dispatch. 

*/
typedef enum
{
  RFWCDMA_DISPATCH_SUCCESSFUL=1,
  RFWCDMA_DISPATCH_UNSUCCESSFUL=2,
  RFWCDMA_DISPATCH_NULL_PTR=3,
  RFWCDMA_DISPATCH_INVALID_PARAM =4,
  RFWCDMA_DISPATCH_MAX
}rfwcdma_dispatch_outcome_t;

#endif /* RFWCDMA_CONTROL_MSG_H */


