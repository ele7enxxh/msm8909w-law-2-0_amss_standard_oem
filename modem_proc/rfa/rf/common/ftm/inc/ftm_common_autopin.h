#ifndef FTM_COMMON_AUTOPIN_H
#define FTM_COMMON_AUTOPIN_H
/*!
  @file
  ftm_common_autopin.h

  @brief
  This module contains APIs that encapsulate critical section routines
*/

/*==============================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_common_autopin.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/10/15   kma     Removed TA featurization
10/23/15   kma     Added REFLOG capture callback api and data structure
10/21/15   kma     Added CCS support for WTR capture
09/30/15   kma     Initial revision

==============================================================================*/

#include "comdef.h"
#include "ftm.h"
#include "ftmdiag.h"
#include "ftm_log.h"
#include "rfm_mode_types.h"
#include "rfdevice_cmn_type_defs.h"
#include "rflm_ccs_rf_intf.h"
#include "rflm_ccs_task_defs.h"
#include "fw_rf_common_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Autopin capture type */
typedef enum 
{
  FTM_AUTOPIN_PAR_WTR_CAPTURE                = 0,
  FTM_AUTOPIN_PAR_REFLOG_CAPTURE             = 1,  
  FTM_AUTOPIN_PAR_WTR_REFLOG_CAPTURE         = 2,
  FTM_AUTOPIN_PAR_FETCH_RESULTS              = 3,
  FTM_AUTOPIN_MAX_SUPPORTED_ACTION
} ftm_autopin_action_t;

/*----------------------------------------------------------------------------*/

/*! Structure used to hold ftm_common_autopin_capture input parameters */
typedef PACK(struct)
{
  ftm_autopin_action_t capture_type;
  uint8 fbrx_gain_state;
} ftm_autopin_capture_param_type;

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*! Structure used to hold output parameters */
typedef PACK(struct)
{
  uint8   cmd;
  uint8   sub_sys;
  uint16  mode;
  uint16  subsys_cmd;
  uint16  req_len;
  uint16  rsp_len;
  uint8   status;
  rfdevice_fbrx_autopin_data_type wtr_capture_response;
  fw_autopin_output_params_t reflog_capture_response;
} ftm_autopin_response_type;

/*----------------------------------------------------------------------------*/

ftm_rsp_pkt_type ftm_common_autopin_capture(ftm_common_pkt_type *cmd_ptr);

void ftm_common_autopin_wtr_callback(rflm_ccs_rf_tq_handle_t *tq, uint32 task_idx, ccs_rf_task_status_t status, void *arg);

void ftm_common_autopin_reflog_callback(fw_autopin_output_params_t *pout);

#ifdef __cplusplus
}
#endif

#endif /* FTM_COMMON_AUTOPIN_H */
