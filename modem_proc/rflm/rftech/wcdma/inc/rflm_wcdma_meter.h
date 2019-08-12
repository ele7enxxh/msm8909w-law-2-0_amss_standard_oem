#ifndef RFLM_WCDMA_METER_H
#define RFLM_WCDMA_METER_H
/*!
  @file
  rflm_wcdma_meter.h

  @brief
  This module is reponsible for handling Tx Power Meter operation
*/

/*==================================================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/wcdma/inc/rflm_wcdma_meter.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------------------------------
09/17/14   gh      Add APIs rflm_wcdma_meter_get_run_decision and rflm_wcdma_msgr_send_compute_mtpl
05/20/14   aro     FW standalone Image compilation error fix
05/01/14   aro     Initial Release

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
  RFLM_WCDMA_METER_EVENT_SCHEDULED,
  /*!< Meter event is successfully scheduled */

  RFLM_WCDMA_METER_EVENT_SCHEDULE_SKIPPED
  /*!< Meter event scheduling skipped */

} rflm_wcdma_meter_status;

/*------------------------------------------------------------------------------------------------*/
rflm_wcdma_meter_status
rflm_wcdma_meter_event_proc
(
  rflm_handle_tx_t handle,
  rflm_wcdma_tx_proc_in_t *tx_proc,
  rflm_wcdma_tx_dm_buf_type *dm_buf_ptr,
  rflm_ustmr_t api_start_time
);

/*------------------------------------------------------------------------------------------------*/
boolean
rflm_wcdma_meter_get_run_decision
(
  rflm_handle_tx_t handle,
  rflm_wcdma_tx_proc_in_t *tx_proc,
  rflm_wcdma_tx_dm_buf_type *dm_buf_ptr,
  rflm_ustmr_t api_start_time
);

/*------------------------------------------------------------------------------------------------*/
void
rflm_wcdma_msgr_send_compute_mtpl
(
 boolean uses_fbrx_ilpc_data,
 rflm_handle_tx_t handle,
 void* params
);

#endif /* RFLM_WCDMA_METER_H */
