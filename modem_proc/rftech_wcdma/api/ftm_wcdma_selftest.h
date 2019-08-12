#ifndef FTM_WCDMA_SELFTEST_H
#define FTM_WCDMA_SELFTEST_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            F T M   W C D M A   S E L F T E S T

GENERAL DESCRIPTION
  This is the header file for the FTM WCDMA Selftest functionality


Copyright (c) 2002, 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2008, 2009, 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2011-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/24/14   jmf     [selftest] Support for VSWR v2 measurement 
11/13/14   jmf     [selftest] Implement WCDMA FBRx Noise Floor cal
10/16/14   jmf     [selftest] Implement multi-gain stage switch for ACLR improvement
06/30/14   daa     Created
===========================================================================*/

#include "ftm.h"
#include "DALStdDef.h"
#include "ftm_wcdma_ctl.h"
#include "rfcom.h"
#include "ftm_common_control.h"
#include "rfcommon_nv.h"
// #include "ftm_wcdma_data.h"
#include "rfm.h"
#include "ftm_msg.h"
#include "rf_test_task_v.h"
#include "ftm_common_iq_processing.h"
#include "ftm_common_dispatch.h"
#include "stringl.h"

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

} ftm_wcdma_selftest_meas_params_struct;

uint8 ftm_wcdma_selftest
(
  ftm_wcdma_selftest_meas_params_struct params,

  ftm_rsp_pkt_type *rsp_pkt,
  ftm_rf_fbrx_iq_acquired_samples_data_type *ftm_rf_fbrx_iq_acquired_samples_data_input,
  uint8 *ftm_selftest_pwr_only,
  ftm_rf_fbrx_meas_tx_params_rsp_pkt_type *ftm_rf_fbrx_meas_tx_params_rsp_pkt_input,
  boolean do_noise_cal
);

boolean ftm_wcdma_obtain_fbrx_noise_nv_item(rfcom_wcdma_band_type band, rfnv_item_id_enum_type *nv_item);

#endif