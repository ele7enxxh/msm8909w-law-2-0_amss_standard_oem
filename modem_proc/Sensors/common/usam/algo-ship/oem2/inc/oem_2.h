/*============================================================================
  @file oem_2.h

  @brief
  Data structures and constants used by OEM_2 custom algorithm template.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/

#include <stdint.h>
#include "fixed_point.h"
#include "sns_sam_algo_api.h"
#include "sns_sam_amd_v01.h"
#include "location_service_v02.h"
#include "sns_sam_prov_loc.h"
#include "sns_oem_2_v01.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/
#define OEM_2_MAX_DATA 450

/*============================================================================
  Type Declarations
  ===========================================================================*/

/* OEM_2 configuration structure */
typedef struct
{
  int32_t sample_rate;
  int32_t report_period;
  sns_oem_data_type_e_v01 inputType;
} oem_2_config_s;

/* Input/output data */
typedef struct
{
  sns_oem_data_type_e_v01 inputType;
  uint32_t dataLen;
  uint8_t data[OEM_2_MAX_DATA];
  sns_sam_timestamp timestamp;
} oem_2_sample_s;

/* OEM_2 input structure */
typedef struct
{
  oem_2_sample_s sample;
} oem_2_input_s;

/* OEM_2 output structure */
typedef struct
{
  oem_2_sample_s sample;
} oem_2_output_s;

/* OEM_2 state structure */
typedef struct
{
  oem_2_config_s config;

  oem_2_sample_s sample;
} oem_2_state_s;

/* OEM_2 persistant data */
typedef struct {
  sns_sam_sensor_uid const *locSUID; /* SUID saved in dep_sensors_met() */
  sns_sam_sensor_uid const *satSUID; /* SUID saved in dep_sensors_met() */
} oem_2_persist;

/* Functions defined in Oem_1.c & Oem_1_msg.c*/
sns_sam_err oem_2_mem_req(
  sns_sam_algo_config const *configData,
  sns_sam_algo_buf_size *bufferSizes );

sns_sam_err oem_2_reset(
  sns_sam_algo_config const *configData,
  sns_sam_algo_callback const *cbFunc,
  sns_sam_algo_persist *persistData,
  sns_sam_algo_state *algoState );

bool oem_2_check_config(
  sns_sam_algo_state const *algoState,
  sns_sam_algo_config const *configData );

void oem_2_mem_const_req(
  sns_sam_algo_const_buf_size *bufferSizes );

sns_sam_err oem_2_dep_sensors(
  uint32_t *sensorCount,
  struct sns_sam_algo_dep_sensor dependentSensors[] );

sns_sam_err oem_2_reg_grps(
  uint32_t *registryGrpCount,
  sns_sam_reg_group registryGroups[] );

void oem_2_dep_sensors_met(
  struct sns_sam_sensor_algo const *algo,
  sns_sam_algo_all_dep_met depMetFunc,
  sns_sam_algo_persist *persistData,
  sns_sam_sensor_uid const *sensorUID,
  sns_sam_algo_attr const (*sns_sam_attr_array)[ SAM_ALGO_ATTR_CNT ] );

void oem_2_get_attr(
  sns_sam_algo_persist const *persistData,
  sns_sam_algo_attr attributes[] );

sns_sam_err oem_2_process_enable_req(
  sns_sam_enable_req const *enableReq,
  sns_sam_algo_persist *persistData,
  sns_sam_client_attr *clientAttr,
  sns_sam_algo_config *configData );

sns_sam_err oem_2_gen_req(
  sns_sam_algo_state const *algoState,
  sns_sam_sensor_uid const *sensorUID,
  sns_sam_enable_req *enableReqMsg );

sns_sam_err oem_2_gen_report(
  sns_sam_algo_output const *outputData,
  sns_sam_resp_msg *respMsg);

bool oem_2_check_sensor(
  sns_sam_algo_state const *algoState,
  sns_sam_enable_req const *enableReq );

/*============================================================================
  Function Declarations
  ===========================================================================*/

/**
* Register the OEM_2 algorithm
*
* See sns_sam_algo_register
*/
sns_sam_err oem_2_register(
  sns_sam_algo_api **algoAPI,
  sns_sam_algo_msg_api **msgAPI,
  qmi_idl_service_object_type *serviceObj,
  sns_sam_sensor_uid *sensorUID );
