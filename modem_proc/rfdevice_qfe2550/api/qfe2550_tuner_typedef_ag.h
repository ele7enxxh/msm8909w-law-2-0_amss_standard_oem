
#ifndef QFE2550_TUNER_TYPEDEF_AG_H
#define QFE2550_TUNER_TYPEDEF_AG_H
/*
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
#ifdef __cplusplus
extern "C" {
#endif  

/* Device Identifiers */
#define QFE2550_TUNER_MANUFACTURER_ID 0x217
#define QFE2550_TUNER_PRODUCT_ID 0x4
#define QFE2550_TUNER_CHIP_REV 
#define QFE2550_TUNER_CHIP_REV_ADDR 
#define QFE2550_TUNER_INSTANCE 0

/* Tuner common settings */
#define QFE2550_TUNER_PRECONFIG_SCRIPT_SIZE 12
#define QFE2550_TUNER_INIT_SCRIPT_SIZE 3
#define QFE2550_TUNER_DISABLE_SCRIPT_SIZE 3
#define QFE2550_TUNER_TUNE_CODE_SCRIPT_SIZE 3
#define QFE2550_TUNER_RL_DISABLE_SCRIPT_SIZE 5

/* Tuner Device specific settings */
#define QFE2550_TUNER_TUNE_CODE_SIZE 2
#define QFE2550_ALGO_MAP 5
#define QFE2550_NUM_C 2
#define QFE2550_NUM_R 0

#ifdef __cplusplus
}
#endif
#endif