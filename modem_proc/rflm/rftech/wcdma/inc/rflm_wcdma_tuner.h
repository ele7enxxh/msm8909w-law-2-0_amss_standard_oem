
/*!
  @file
  rflm_wcdma_tuner.h

  @brief
  Definitions for RFLM WCDMA Tuner module
  
  @detail
  Provides definitions and function prototypes used by RFLM WCDMA tuner module 

*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/wcdma/inc/rflm_wcdma_tuner.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/14/14   rmb     Fix Compilation Error because of rflm_wcdma_abort_tuner_open_loop_update()
04/28/14    kg     Add API for rflm_wcdma_abort_tuner_open_loop_update
04/14/14    dw     Initial version
==============================================================================*/

#ifndef RFLM_WCDMA_TUNER_H
#define RFLM_WCDMA_TUNER_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "rflm_api_cmn.h"
#include "rflm_cmn_intf.h"
#include "rflm_cmn_tuner.h"

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/


#ifdef __cplusplus
extern "C" {
#endif

/*! @brief 
  Tuner AOL Margin to be added to the boundary , 40 us = 758 USMTR units 
  Note : this is the time required for the FBrx to complete guarenteeing AOL does not collide with
  FBRx . This delay is dependent on WTR/Modem , 40usec may not be enough for all RFC's */

#define RFLM_WCDMA_TUNER_AOL_TIME_DELAY_USTMR                                 768

/*! @brief 
  Minumum measurement time required for tuner closed loop event, Tx power needs to
  stay constant for this period. This includes HW measurement time (270us) + CL trigger
  transaction time (30us)
  */

#define RFLM_WCDMA_TUNER_CL_MEAS_TIME                                         5760

/*! 
  @brief  
  Type definition for RFLM WCDMA tuner event
*/
typedef enum
{
  RFLM_WCDMA_TUNER_AOL_EVENT,
  RFLM_WCDMA_TUNER_CL_EVENT,
  RFLM_WCDMA_TUNEr_MAX_EVENT
}rflm_wcdma_tuner_event_type;


/*==============================================================================
  Tuner event proc function's prototypes
==============================================================================*/

/*---------------------------------------------------------------------------*/
/*! 
  @brief 
  Process Tuner Events (advanced open loop and closed loop)
 
  @details
  This function is calle by Tx proc to schedule tuner related events when Tx
  power is stable. It shall proritize AOL over CL, depends on the CCS availability
 
  @param handle
  RFLM handle
 
  @param tx_proc
  Input structure from rflm_wcdma_tx_proc_stable_pwr, which includes Tx power, start and stop time
  of the Tx quiet period
 
  @param dm_buf_ptr
  Tx DM tech buffer pointer
*/
void
rflm_wcdma_tuner_event_proc
(
  rflm_handle_tx_t handle,
  rflm_wcdma_tx_proc_in_t *tx_proc,
  rflm_wcdma_tx_dm_buf_type *dm_buf_ptr
);
#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_WCDMA_TUNER_H */
