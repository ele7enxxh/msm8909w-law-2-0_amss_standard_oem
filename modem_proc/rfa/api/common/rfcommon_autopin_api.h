#ifndef RFCOMMON_AUTOPIN_API_H
#define RFCOMMON_AUTOPIN_API_H

/*!
  @file
  rfcommon_autopin_api.h

  @brief
  This module contains interfaces defined by common AutoPin.
*/

/*==============================================================================

  Copyright (c) 2015 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/rfcommon_autopin_api.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
02/01/16   whz     Log packet support
02/01/16   vs      Do not apply Pin if Peak avg power > 35k
12/09/15   kma     Added WTR PAR result into capture control structure  
11/05/15   whz     Move some interface to this file 
10/29/15   whz     Remove timers completely
08/03/15   kg      Add header "rfcommon_core_txlin_types.h"  
07/15/15   cdb     Add support to override Autopin operation
06/04/15   whz     Initial version

==============================================================================*/

#include "lm_types.h"
#include "rfm_mode_types.h"
#include "rflm_api_cmn.h"
#include "rflm_api_autopin.h"
#include "rfdevice_cmn_type_defs.h"
#include "fw_autopin_intf.h"


typedef enum
{
  RFCOMMON_AUTOPIN_SUCCESS = 0,
  RFCOMMON_AUTOPIN_ERROR = 1,
} rfcommon_autopin_error_type;

typedef enum 
{
  AUTOPIN_PAR_WTR_CAPTURE                = 0,
  AUTOPIN_PAR_REFLOG_CAPTURE             = 1,  
  AUTOPIN_MAX_SUPPORTED_ACTION
} rfcommon_autopin_capture_t;

typedef struct
{
  rfcommon_autopin_capture_t capture_type;
  boolean valid_capture;
  uint16 par;
  fw_autopin_psamp_result_out_t reflog_result;
  rfdevice_fbrx_autopin_data_type wtr_capture_result;
} rfcommon_autopin_capture_control_type;


#ifdef __cplusplus
extern "C" {
#endif



boolean rfcommon_autopin_is_enabled( rfm_mode_enum_type rfm_tech );


rfcommon_autopin_error_type rfcommon_autopin_mc_enter_mode( rfm_mode_enum_type rfm_tech );


rfcommon_autopin_error_type rfcommon_autopin_mc_exit_mode( rfm_mode_enum_type rfm_tech );


rfcommon_autopin_error_type rfcommon_autopin_mc_enable_tx( rflm_handle_tx_t tx_handle,
                                                           rfm_mode_enum_type rfm_tech );

rfcommon_autopin_error_type rfcommon_autopin_mc_disable_tx( rflm_handle_tx_t tx_handle,
                                                            rfm_mode_enum_type rfm_tech );


rfcommon_autopin_error_type rfcommon_autopin_mc_disable_tx_preserv( rflm_handle_tx_t tx_handle,
                                                                    rfm_mode_enum_type rfm_tech );

rfcommon_autopin_error_type rfcommon_autopin_override_ctrl(uint8 tx_handle,
                                                           uint8 ovr_cmd_id,
                                                           uint8 ovr_enable,
                                                           uint8 ovr_val1,
                                                           uint8 ovr_val2);

void rfcommon_autopin_capture_control(rflm_handle_tx_t handle, rfcommon_autopin_capture_control_type capture_input);


#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_AUTOPIN_API_H */


