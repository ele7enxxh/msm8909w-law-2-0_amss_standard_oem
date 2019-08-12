
#ifndef RFCOMMON_FBRX_CCS_H
#define RFCOMMON_FBRX_CCS_H
/*!
  @file
  rfcommon_fbrx_ccs.h

  @brief
  This module contains interface of common FBRx CCS Events.
  
*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies, Inc. (QTI). All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa3/main/3.0/rf/common/rf/fbrx/inc/rfcommon_fbrx_ccs.h

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/19/14    aa     FBRx + GPS concurrency SM implementation
08/06/14   aro     Interface to export FBRx event handler
05/02/14    aa     Updated the default script size
04/01/14    aa     Initial version - Support for FBRX CCS Events

==============================================================================*/


/*==============================================================================
                           INCLUDE FILES
==============================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "rfcommon_fbrx_api.h"
#ifdef FEATURE_RF_FBRX
#include "rflm_api_fbrx.h"
#endif /* FEATURE_RF_FBRX */

#include "rfm_mode_types.h"
#include "rfdevice_cmn_intf.h"
#include "rflm_dm_api.h"

#include "rf_hal_buffer.h"
#include "rfcommon_mdsp_event_intf.h"
#include "rfcommon_msm.h"
#include "rfcommon_mdsp.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*==============================================================================
                EXTERNAL DEFINITIONS AND TYPES : MACROS
==============================================================================*/

#define RFCOMMON_FBRX_MAX_SSBI_SCRIPT_SIZE  0
#define RFCOMMON_FBRX_MAX_GRFC_SCRIPT_SIZE  0
#define RFCOMMON_FBRX_MAX_RFFE_SCRIPT_SIZE  20
#define RFCOMMON_FBRX_DEFAULT_SCRIPT_SIZE   RFLM_FBRX_SCRIPT_DATA_BUF_SIZE_WORDS*4

/*==============================================================================
                    EXTERNAL FUNCTION PROTOTYPES
==============================================================================*/

/*----------------------------------------------------------------------------*/
boolean
rfcommon_fbrx_ccs_allocate_associate_script
(
  lm_handle_type                fbrx_handle,
  rflm_fbrx_ccs_event_type_e    event_id 
);

/*----------------------------------------------------------------------------*/
void*
rfcommon_fbrx_ccs_get_buffer
(
  lm_handle_type                            fbrx_handle,
  rflm_fbrx_ccs_event_type_e                event_id,
  rfcommon_mdsp_transaction_settings_type   *ccs_event_transaction_params, 
  boolean                                   *is_event_issue_seq
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_fbrx_ccs_configure_event
(
  const rfcommon_mdsp_event_settings_type   *event_settings,
  rf_buffer_intf                            *buf_obj_ptr,
  lm_handle_type                            fbrx_handle,
  rflm_handle_tx_t                          tx_handle,
  rflm_fbrx_ccs_event_type_e                event_id,
  rfcommon_mdsp_event_handle                **event_handle
);

/*----------------------------------------------------------------------------*/
boolean 
rfcommon_fbrx_ccs_configure_on_off_scripts
( 
  rfm_device_enum_type    device,
  lm_handle_type          fbrx_handle,
  rflm_handle_tx_t        tx_handle 
);

/*----------------------------------------------------------------------------*/
rfcommon_mdsp_event_handle**
rfcommon_fbrx_get_event_handle_ptr
(
  rflm_fbrx_ccs_event_type_e event_id
);

/*----------------------------------------------------------------------------*/

boolean 
rfcommon_fbrx_ccs_configure_ldo_scripts
( 
  rfm_device_enum_type        device,
  lm_handle_type              fbrx_handle,
  rflm_fbrx_ccs_event_type_e  fbrx_event_id,
  boolean                     fbrx_get_setup_script_flag,
  rfdevice_fbrx_scenario_type fbrx_ldo_scenario, 
  rflm_handle_tx_t            tx_handle 
);
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_FBRX_CCS_H */
