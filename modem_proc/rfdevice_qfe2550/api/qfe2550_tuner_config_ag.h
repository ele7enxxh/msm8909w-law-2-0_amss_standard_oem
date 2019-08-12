
#ifndef QFE2550_TUNER_CONFIG_AG_H
#define QFE2550_TUNER_CONFIG_AG_H/*
WARNING: This QFE2550 driver is auto-generated.

Generated using: qtuner_autogen.pl 
Generated from-  

	File: QFE2550_RFFE_Settings.xlsx 
	Released: 10/22/2014
	Author: ycchiou
	Revision: v0.8
	Change Note: 1. qcm_cmn_compute_internal_clock
2. rtuner_cmn_compute_rtuner_code
3. qcm_cmn_get_ctuner_cal_param
4. qcm_cmn_get_ctuner_bin_values
5. rctuner_perform_c_tuner_cal
6. rctuner_load_tuner_cal

	Tab: qfe2550_tuner_settings

*/

/*=============================================================================

          RF DEVICE  A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the QFE2550 TUNER.

Copyright (c) 2013,2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

$Header: //source/qcom/qct/modem/rfdevice/qfe2550/main/latest/etc/qtuner_autogen.pl#6 : ybansal : 2014/07/22 22:57:09 60============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/


#include "comdef.h"
#include "rfc_common.h"
#include "rfdevice_qtuner_typedef.h"
#ifdef __cplusplus
extern "C" {
#endif  
boolean rfdevice_qtuner_qfe2550_validate_cfg_ag(rfc_device_cfg_info_type* cfg);

boolean rfdevice_qtuner_qfe2550_construct_driver_ag
(
  rfdevice_qtuner_settings_type* qtuner_settings,
  rfdevice_qtuner_func_tbl_type* qtuner_fn_ptrs,
  rfdevice_qtuner_params_type * qtuner_params
);

#ifdef __cplusplus
}
#endif
#endif