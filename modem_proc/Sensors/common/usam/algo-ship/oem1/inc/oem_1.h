/*============================================================================
  @file oem_1.h

  @brief
  Data structures and constants used by OEM_1 custom algorithm template.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/

#include <stdint.h>
#include "fixed_point.h"
#include "sns_sam_algo_api.h"
#include "sns_sam_amd_v01.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/
#define OEM_1_ELEMS 3

/*============================================================================
  Type Declarations
  ===========================================================================*/

/* OEM_1 configuration structure */
typedef struct
{
  int32_t sample_rate;
  int32_t report_period;
} oem_1_config_s;

typedef enum {
  OEM1_INPUT_AMD,
  OEM1_INPUT_ACCEL
}oem1_input_type;

/* OEM_1 input structure */
typedef struct
{
  oem1_input_type inputType;
  union {
    int32_t sample_1[OEM_1_ELEMS];   //input sample from sensor 1
    sns_sam_motion_state_e_v01 amdState;
  };
} oem_1_input_s;


/* OEM_1 output structure */
typedef struct
{
  int32_t data[OEM_1_ELEMS];
  sns_sam_timestamp timestamp;
} oem_1_output_s;


/* OEM_1 state structure */
typedef struct
{
  uint32_t state_param1;
  oem_1_config_s config;
  int32_t data[OEM_1_ELEMS]; // Most recent OEM_1 calculation
  sns_sam_timestamp timestamp; // Timestamp of the most recent sensor sample
  float samplePeriod; // Current/instantaneous sample period
  sns_sam_motion_state_e_v01 amdState;  // From AMD indication
} oem_1_state_s;


/* OEM_1 persistant data */
typedef struct {
  bool regDataRecv; /* If the registry data has been received*/
  uint64_t power; /* Consumption for the sensor as reported by SMGR */
  q16_t defSampleRate; /* Default sampling rate if none specified */
  q16_t minSampleRate; /* Minimum sampling rate */
  q16_t maxSampleRate; /* Maximum sampling rate */
  sns_sam_sensor_uid const *accelSUID; /* SUID saved in dep_sensors_met() */
  sns_sam_sensor_uid const *amdSUID; /* SUID saved in dep_sensors_met() */
} oem_1_persist;


/* Functions defined in Oem_1.c & Oem_1_msg.c*/
sns_sam_err oem_1_mem_req(
  sns_sam_algo_config const *configData,
  sns_sam_algo_buf_size *bufferSizes );

sns_sam_err oem_1_reset(
  sns_sam_algo_config const *configData,
  sns_sam_algo_callback const *cbFunc,
  sns_sam_algo_persist *persistData,
  sns_sam_algo_state *algoState );

bool oem_1_check_config(
  sns_sam_algo_state const *algoState,
  sns_sam_algo_config const *configData );

void oem_1_mem_const_req(
  sns_sam_algo_const_buf_size *bufferSizes );

sns_sam_err oem_1_dep_sensors(
  uint32_t *sensorCount,
  struct sns_sam_algo_dep_sensor dependentSensors[] );

sns_sam_err oem_1_reg_grps(
  uint32_t *registryGrpCount,
  sns_sam_reg_group registryGroups[] );

void oem_1_dep_sensors_met(
  struct sns_sam_sensor_algo const *algo,
  sns_sam_algo_all_dep_met depMetFunc,
  sns_sam_algo_persist *persistData,
  sns_sam_sensor_uid const *sensorUID,
  sns_sam_algo_attr const (*sns_sam_attr_array)[ SAM_ALGO_ATTR_CNT ] );

void oem_1_get_attr(
  sns_sam_algo_persist const *persistData,
  sns_sam_algo_attr attributes[] );

sns_sam_err oem_1_process_enable_req(
  sns_sam_enable_req const *enableReq,
  sns_sam_algo_persist *persistData,
  sns_sam_client_attr *clientAttr,
  sns_sam_algo_config *configData );

sns_sam_err oem_1_gen_req(
  sns_sam_algo_state const *algoState,
  sns_sam_sensor_uid const *sensorUID,
  sns_sam_enable_req *enableReqMsg );

sns_sam_err oem_1_gen_report(
  sns_sam_algo_output const *outputData,
  sns_sam_resp_msg *respMsg);

bool oem_1_check_sensor(
  sns_sam_algo_state const *algoState,
  sns_sam_enable_req const *enableReq );

/*============================================================================
  Function Declarations
  ===========================================================================*/

/**
* Register the OEM_1 algorithm
*
* See sns_sam_algo_register
*/
sns_sam_err oem_1_register(
  sns_sam_algo_api **algoAPI,
  sns_sam_algo_msg_api **msgAPI,
  qmi_idl_service_object_type *serviceObj,
  sns_sam_sensor_uid *sensorUID );
