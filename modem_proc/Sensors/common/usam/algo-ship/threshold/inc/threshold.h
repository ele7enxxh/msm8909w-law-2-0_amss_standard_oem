#ifndef THRESHOLD_H
#define THRESHOLD_H

/*============================================================================
  @file threshold.h

  @brief
  Data structures and constants used internally by the threshold algorithm.

  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "fixed_point.h"
#include "sns_sam_algo_api.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/

/* Number of elements/axis for threshold algo */
#define THRESH_MAX_NUM_AXES     (3)

/* Threshold type */
#define THRESH_TYPE_ABSOLUTE     (0)
#define THRESH_TYPE_RELATIVE     (1)

/*============================================================================
  Type Declarations
  ===========================================================================*/

/* Threshold configuration structure */
typedef struct
{
  uint8_t  sensor_id;
  uint8_t  data_type;
  uint8_t  num_axis;
  q16_t sample_rate;  // in Hz, Q16
  q16_t thresh[THRESH_MAX_NUM_AXES];    // Q16
  q16_t report_period; // unit of seconds TODO: confirm
  uint8_t  threshold_type;
} threshold_config_s;

/* Threshold input structure */
typedef struct
{
  q16_t sample[THRESH_MAX_NUM_AXES];     // sensor sample data, Q16
} threshold_input_s;

/* Threshold output structure */
typedef struct
{
  bool    do_report;     // whether samples are to be reported
  uint8_t  num_axis;
  q16_t sample[THRESH_MAX_NUM_AXES];     // sensor sample data, Q16
  sns_sam_timestamp timestamp;
} threshold_output_s;

/* Threshold state structure */
typedef struct
{
  threshold_config_s config;  //Threshold algorithm configuration
  q16_t last_report[THRESH_MAX_NUM_AXES]; /* Last event exceeding threshold */
  sns_sam_timestamp last_timestamp; /* Timestamp associated with last_report */
  uint32_t ts_start_period[THRESH_MAX_NUM_AXES];
} threshold_state_s;

typedef struct {
  sns_sam_sensor_uid const *anySensorSUID;
  /* The first Sensor UID returned during initialization which makes threshold able to
   * be registered. More for debugging purpose. Leave this structure here since SAM 
   * framework requires every algo has its own persist data structure 
   * TODO: this is not really used. Will remove once SAM framework is updated.
   */
} threshold_persist_s;
/*============================================================================
  Function Declarations
  ===========================================================================*/

/**
* Register the Threshold algorithm
*
* See sns_sam_algo_register
*/
sns_sam_err threshold_sam_register(
      sns_sam_algo_api **algoAPI,
      sns_sam_algo_msg_api **msgAPI,
      qmi_idl_service_object_type *serviceObj,
      sns_sam_sensor_uid *sensorUID );

#endif /* THRESHOLD_H */
