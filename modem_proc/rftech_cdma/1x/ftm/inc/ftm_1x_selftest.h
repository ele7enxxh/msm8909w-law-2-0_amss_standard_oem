#ifndef FTM_1X_SELFTEST_H
#define FTM_1X_SELFTEST_H

/*!
  @file
  ftm_1x_selftest.h

  @brief
  This module contains prototypes and definitions to be used by selftest code for FTM.
*/

/*==============================================================================

  Copyright (c) 2011, 2012, 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/07/14   dbz     [selftest] Add support for VSWR V2 capture and processing 
10/16/14   jmf     [selftest] Implement multi-gain stage switch for ACLR improvement
06/19/14   daa     Initial revision

==============================================================================*/

#include "ftm_command_id.h"
#include "rfcommon_time_profile.h"
#include "rfm_1x.h"
#include "rfm.h"
#include "ftm_cdma_data.h"
#include "ftm_common_control.h"
#include "rf_test_task_v.h"
#include "ftm_common_iq_processing.h"
#include "ftm_msg.h"
#include <stringl/stringl.h>
#include "msg.h"
#include "DALStdDef.h"
#include "rfcommon_time_profile.h"
#include "math.h"
#include "rfnv_items.h"
#include "sys.h"
#include "rf_cdma_utils_freq.h"
#include "ftm_1x_control.h"
#include "ftm_common_dispatch.h"
#include "rfc.h"
#include "rf_cdma_utils.h"
#include "rfm_cdma_ftm.h"
#include "rfm_1x_ftm.h"

typedef struct {

  boolean enable_tx_pwr_meas;
  boolean enable_aclr_meas;
  boolean enable_evm_meas;
  boolean enable_vswr_meas;
  boolean enable_vswr_meas_v2;

  rfcom_device_enum_type device;
  uint16 num_averages;
  uint32 capture_offset;
  uint32 tx_measurement_config;

} ftm_1x_selftest_meas_params_struct;

uint8 ftm_1x_selftest(ftm_1x_selftest_meas_params_struct params, ftm_rsp_pkt_type *rsp_pkt_ptr, ftm_rf_cdma_fbrx_iq_acquired_samples_data_type *ftm_rf_fbrx_iq_acquired_samples_data_ptr_input, ftm_rf_fbrx_meas_tx_params_rsp_pkt_type *ftm_rf_fbrx_meas_tx_params_rsp_pkt, uint8 *ftm_selftest_pwr_only, boolean use_iq_rms_for_pwr_meas);

#endif /* FTM_1X_SELFTEST_H */