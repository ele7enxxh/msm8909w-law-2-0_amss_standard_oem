/******************************************************************************
  @file    sns_qdsp_playback_clnt.h

  ---------------------------------------------------------------------------
 * Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
 *******************************************************************************/
#include "qmi_client.h"
#include "sns_common_v01.h"


#define SNS_QDSP_SIM_SAM_ENABLE_RESP 0x0002
#define SNS_QDSP_SIM_SAM_REPORT_IND  0x0005
#define SNS_QDSP_SIM_SAM_ERROR_IND   0x0006

typedef struct {
	sns_common_resp_s_v01 resp;
	uint8_t instance_id_valid;
    uint8_t instance_id;
} sam_qdsp_common_resp_msg;


int sns_plbk_sam_amd_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_amd_disable(qmi_client_type *clnt);

int sns_plbk_sam_rmd_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_rmd_disable(qmi_client_type *clnt);

int sns_plbk_sam_fns_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_fns_disable(qmi_client_type *clnt);

int sns_plbk_sam_bte_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_bte_disable(qmi_client_type *clnt);

int sns_plbk_sam_mag_cal_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_mag_cal_disable(qmi_client_type *clnt);

int sns_plbk_sam_filtered_mag_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_filtered_mag_disable(qmi_client_type *clnt);

int sns_plbk_sam_mag_cal_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_mag_cal_disable(qmi_client_type *clnt);

int sns_plbk_sam_rotation_vector_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_rotation_vector_disable(qmi_client_type *clnt);

int sns_plbk_sam_quaternion_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_quaternion_disable(qmi_client_type *clnt);

int sns_plbk_sam_gravity_vector_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_gravity_vector_disable(qmi_client_type *clnt);

int sns_plbk_sam_orientation_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_orientation_disable(qmi_client_type *clnt);

int sns_plbk_sam_basic_gestures_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_basic_gestures_disable(qmi_client_type *clnt);

int sns_plbk_sam_tap_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_tap_disable(qmi_client_type *clnt);

int sns_plbk_sam_facing_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_facing_disable(qmi_client_type *clnt);

int sns_plbk_sam_integ_angle_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_integ_angle_disable(qmi_client_type *clnt);

int sns_plbk_sam_gyro_tap2_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_gyro_tap2_disable(qmi_client_type *clnt);

int sns_plbk_sam_gyrobuf_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_gyrobuf_disable(qmi_client_type *clnt);

int sns_plbk_sam_gyroint_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_gyroint_disable(qmi_client_type *clnt);

int sns_plbk_sam_ped_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_ped_disable(qmi_client_type *clnt);

int sns_plbk_sam_pam_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_pam_disable(qmi_client_type *clnt);

int sns_plbk_sam_smd_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_smd_disable(qmi_client_type *clnt);

int sns_plbk_sam_cmc_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_cmc_disable(qmi_client_type *clnt);

int sns_plbk_sam_game_rotation_vector_enable(int *algo_config, qmi_client_type *clnt);

int sns_plbk_sam_game_rotation_vector_disable(qmi_client_type *clnt);




