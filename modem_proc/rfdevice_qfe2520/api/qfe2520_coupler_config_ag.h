
#ifndef QFE2520_COUPLER_CONFIG_AG_H
#define QFE2520_COUPLER_CONFIG_AG_H/*
WARNING: This QFE2520 driver is auto-generated.

Generated using: qcoupler_autogen.pl 
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
	Tab: qfe2520_cpl_settings

*/

/*=============================================================================

          RF DEVICE  A U T O G E N    F I L E

GENERAL DESCRIPTION
  This file is auto-generated and it captures the configuration of the RF Card.

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

$Header: //source/qcom/qct/modem/rfdevice/qfe2520/main/latest/etc/qcoupler_autogen.pl#3 : ybansal : 2014/04/10 04:18:31 60============================================================================*/

/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

 
#include "comdef.h"
#include "rfc_common.h"
#include "rfdevice_qcoupler.h"
#include "rfdevice_qcoupler_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif  

boolean rfdevice_qcoupler_qfe2520_validate_cfg_ag(rfc_device_cfg_info_type* cfg);

boolean rfdevice_qcoupler_qfe2520_construct_driver_ag
(
  rfdevice_qcoupler_settings_type* qcoupler_settings,
  rfdevice_qcoupler_func_tbl_type* qcoupler_fn_ptrs
);

boolean rfdevice_qcoupler_qfe2520_get_config_data_ag
(
  rfdevice_qcoupler_ag_settings_type* port_settings, 
  rfdevice_qcoupler_cfg_type qcoupler_cfg
);


#ifdef __cplusplus
}
#endif
#endif