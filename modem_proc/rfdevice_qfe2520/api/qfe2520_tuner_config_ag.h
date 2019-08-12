
#ifndef QFE2520_TUNER_CONFIG_AG_H
#define QFE2520_TUNER_CONFIG_AG_H/*
WARNING: This QFE2520 driver is auto-generated.

Generated using: qtuner_autogen.pl 
Generated from-  

	File: QFE2520_RFFE_Settings.xlsx 
	Released: 
	Author: ycchiou
	Revision: v0.34
	Change Note: Remove the internal device calibration:
QFE2520_TDET_Setting
1. k_sensor_cmn_init
2. k_sensor_cmn_enable_k1_sensor
3. k_sensor_cmn_read_k_sensor
4. k_sensor_cmn_read_k_sensor_imm
5. k_sensor_cmn_enable_k2_sensor
6. k_sensor_cmn_end_k_sensor_read
QFE2520_Tuner_settings
1. rtuner_cmn_init
2. rtuner_cmn_compute_rtuner_code
3. rcm_cmn_compute_internal_clock
4. rcm_cmn_compute_ctuner_code
5. rcm_cmn_compute_rtuner_code
6. rcm_cmn_perform_resistor_1_9900_cal
7. rcm_cmn_get_ctuner_bin_values
8. rctuner_perform_c_tuner_cal
9. rtuner_perform_r_tuner_cal
10. rtuner_load_r_tuner_cal
11. rctuner_load_tuner_cal
	Tab: qfe2520_tuner_settings

*/

/*=============================================================================

          RF DEVICE  A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the QFE2520 TUNER.

Copyright (c) 2013,2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

$Header: //source/qcom/qct/modem/rfdevice/qfe2520/main/latest/etc/qtuner_autogen.pl#9 : adkulkar : 2014/10/24 15:12:07 60============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/


#include "comdef.h"
#include "rfc_common.h"
#include "rfdevice_qtuner_typedef.h"
#ifdef __cplusplus
extern "C" {
#endif  
boolean rfdevice_qtuner_qfe2520_validate_cfg_ag(rfc_device_cfg_info_type* cfg);

boolean rfdevice_qtuner_qfe2520_construct_driver_ag
(
  rfdevice_qtuner_settings_type* qtuner_settings,
  rfdevice_qtuner_func_tbl_type* qtuner_fn_ptrs,
  rfdevice_qtuner_params_type * qtuner_params
);

#ifdef __cplusplus
}
#endif
#endif