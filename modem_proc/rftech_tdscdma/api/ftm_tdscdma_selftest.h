#ifndef FTM_TDSCDMA_SELFTEST_H
#define FTM_TDSCDMA_SELFTEST_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            F T M   T D S C D M A   S E L F T E S T

GENERAL DESCRIPTION
  This is the header file for the FTM TDSCDMA Selftest functionality


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
11/24/14   jmf     [selftest] Add support for TDSCDMA VSWR mag/phase measurement
10/16/14   jmf     [selftest] Implement multi-gain stage switch for ACLR improvement
06/30/14   daa     Created
===========================================================================*/

#include "DALStdDef.h"
#include "ftm_tdscdma_ctl.h"
#include "rfcom.h"
#include "ftm_common_control.h"
#include "rfcommon_time_profile.h"
#include "stringl.h"
#include "math.h"
#include "ftm_msg.h"
#include "rfm.h"
#include "ftm_common_iq_processing.h"

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

} ftm_tdscdma_selftest_meas_params_struct;

uint8 ftm_tdscdma_selftest
(
  ftm_tdscdma_selftest_meas_params_struct params,
  ftm_rsp_pkt_type *rsp_pkt,
  ftm_rf_fbrx_meas_tx_params_rsp_pkt_type *ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt_input,
  ftm_rf_tdscdma_fbrx_iq_acquired_samples_data_type *ftm_rf_tdscdma_fbrx_iq_acquired_samples_data_input,
  int16 *ftm_tdscdma_power_dbm_input,
  boolean *ftm_selftest_pwr_only
);

#endif
