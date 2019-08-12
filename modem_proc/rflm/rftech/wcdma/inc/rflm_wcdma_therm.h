#ifndef RFLM_WCDMA_THERM_H
#define RFLM_WCDMA_THERM_H
/*!
  @file
  rflm_wcdma_therm.h

  @brief
  This module is responsible for handling THERM operations
*/

/*==================================================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document are confidential 
  and proprietary information of Qualcomm Technologies Incorporated and all rights therein are 
  expressly reserved. By accepting this material the recipient agrees that this material and the 
  information contained therein are held in confidence and in trust and will not be used, copied, 
  reproduced in whole or in part, nor its contents revealed in any manner to others without the 
  express written permission of Qualcomm Technologies Incorporated.

==================================================================================================*/

/*==================================================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/wcdma/inc/rflm_wcdma_therm.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------------------------------
03/12/15   gh      Initial Release

==================================================================================================*/

#include "rflm.h"
#include "comdef.h"
#include "rflm_api_cmn.h"
#include "rflm_api_wcdma.h"
#include "rflm_wcdma_sw_api.h"

/*------------------------------------------------------------------------------------------------*/
/*! Enumeration listing the status of meter event processing status */
typedef enum
{
  /*!< therm_enable event is successfully scheduled */
  RFLM_WCDMA_THERM_ENABLE_EVENT_SCHEDULED,

  /*!< therm_enable event scheduling skipped */
  RFLM_WCDMA_THERM_ENABLE_EVENT_SCHEDULE_SKIPPED,
  
  /*!< therm_enable event scheduling failed */
  RFLM_WCDMA_THERM_EVENT_SCHEDULE_FAILED

} rflm_wcdma_therm_enable_status;

/*------------------------------------------------------------------------------------------------*/
rflm_wcdma_therm_enable_status
rflm_wcdma_therm_enable_event_proc
(
  rflm_handle_tx_t handle,
  rflm_wcdma_tx_dm_buf_type *dm_buf_ptr,
  rflm_ustmr_t api_start_time
);

/*------------------------------------------------------------------------------------------------*/
boolean
rflm_wcdma_therm_enable_get_run_decision
(
  rflm_handle_tx_t handle,
  rflm_wcdma_tx_dm_buf_type *dm_buf_ptr,
  rflm_ustmr_t api_start_time
);

/*------------------------------------------------------------------------------------------------*/
void
rflm_wcdma_msgr_send_therm_enable_done
(
 boolean enabled_success,
 rflm_handle_tx_t handle,
 void* params,
 boolean error
);

#endif /* RFLM_WCDMA_THERM_H */
