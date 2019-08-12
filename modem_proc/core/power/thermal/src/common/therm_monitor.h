#ifndef THERM_MONITOR_H
#define THERM_MONITOR_H

/*============================================================================
  @file therm_monitor.h

  Simple thermal monitor implementation.
  
  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/power/thermal/src/common/therm_monitor.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/20/11   clm     Initial Version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "npa.h"

/*----------------------------------------------------------------------------
 * Type Definitions
 * -------------------------------------------------------------------------*/
#define MAX_NUM_THERM_STATES  8
#define MAX_NUM_THERM_SENSORS 8
#define MAX_NUM_THERM_MITIGATION_RES 8
#define MAX_RESOURCE_PATH     64
#define CONFIG_FILE_SIZE_MAX  1024
#define DEFAULT_SAMPLING_MS   10000
#define INVALID_SENSOR_STATE  (-1)
#define MONITOR_SUCCESS       0
#define MONITOR_FAILED        1

typedef enum
{
  THERM_MONITOR_NONE,
  THERM_MONITOR_SET_MITIGATE_STATE
} therm_monitor_action_type;


typedef struct
{
  /* Individual state sampling rate req */
  uint32                    sampling;

  /* Thresh to enter this state */
  int32                     thresh;

  /* Thresh to fall back to previous state */
  int32                     thresh_clr;  

  /* Action to perform for this state */
  therm_monitor_action_type action;

  /* Action info */
  uint32                    action_info;

} therm_monitor_thresh_state_type;


typedef struct
{
  /* Individual states thresholds and actions to take */
  therm_monitor_thresh_state_type state_data[MAX_NUM_THERM_STATES];

  /* Current state that sensor is in */
  uint32            current_state;
  
  /* Number of valid states */
  uint32            num_of_states;

  /* Sensor event handle */
  npa_event_handle  sensor_event_handle;

  /* Resource path name */
  char              sensor_resource_path[MAX_RESOURCE_PATH];

  /* Sensor sampling client handle*/
  npa_client_handle sampling_handle;

  /* Resource path name */
  char              sampling_resource_path[MAX_RESOURCE_PATH];

  /* Sensor mitgate state client handle*/
  npa_client_handle mitigate_state_handle;

  /* Resource path name */
  char              mitigate_state_resource_path[MAX_RESOURCE_PATH];
} therm_monitor_sensor_info_type;


typedef struct
{
  /* Sensor info. */
  therm_monitor_sensor_info_type sensor[MAX_NUM_THERM_SENSORS];

  /* Number of valid sesnors */
  uint32                         num_valid_sensors;

} therm_monitor_type;


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define THERM_K_TO_C(x) ((int32)(((int32)x) - 273))
#define THERM_C_TO_K(x) ((int32)(((int32)x) + 273)) 


/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS
                     
===========================================================================*/

/**
  @brief therm_monitor_load_config

  Thermal monitor load config file.

  @param  : None.

  @return : MONITOR_SUCCESS or MONITOR_FAILED.

*/
int therm_monitor_load_config(therm_monitor_type *config_data);
#endif /* THERM_MONITOR_H */

