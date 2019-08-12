/*!
  @file
  rflm_wcdma_autopin.h

  @brief
  This module is reponsible for handling Autopin operation
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/wcdma/inc/rflm_wcdma_autopin.h#1 $


when       who     what, where, why
--------   ---     ---------------------------------------------------------------------------------
11/16/15   ag      Initial Version 

==================================================================================================*/

#include "rflm.h"
#include "comdef.h"
#include "rflm_api_cmn.h"
#include "rflm_api_wcdma.h"
#include "rflm_wcdma_sw_api.h"
#include "rflm_dm_api.h"
#include "rflm_cmn_intf.h"

/*------------------------------------------------------------------------------------------------*/
/*! Enumeration listing the status of autopin event processing status */
typedef enum
{
  RFLM_WCDMA_ENABLE_AUTOPIN_EVENT_SCHEDULED,
  /*!< Autopin event is successfully scheduled */

  RFLM_WCDMA_ENABLE_AUTOPIN_SCHEDULE_SKIPPED
  /*!< Autopin event scheduling skipped */

} rflm_wcdma_enable_autopin_status;


/*------------------------------------------------------------------------------------------------*/
/*!
  @brief
  This API will trigger par read to calculate pin comp

  @details
*/
void
rflm_wcdma_trigger_autopin_par_read
(
rflm_handle_tx_t handle
);

/*------------------------------------------------------------------------------------------------*/

/*!
  @brief
  This API will 

  @details
*/

rflm_wcdma_enable_autopin_status
rflm_wcdma_enable_autopin_event_proc
(
  rflm_handle_tx_t handle,
  rflm_wcdma_tx_proc_in_t *tx_proc,
  rflm_wcdma_tx_dm_buf_type *dm_buf_ptr,
  rflm_ustmr_t api_start_time
);




