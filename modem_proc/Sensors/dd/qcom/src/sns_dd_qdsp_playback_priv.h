/** 
 * @file sns_dd_qdsp_playback_priv.h
 * @brief Common definitions for qdsp sim playback driver.
 *  
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_qdsp_playback_priv.h#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why 
  ---------- ---  -----------------------------------------------------------
  2013-06-11 ae   Initial revision
==============================================================================*/

#ifndef SNS_DD_QDSP_PLAYBACK__H
#define SNS_DD_QDSP_PLAYBACK__H

#include "fixed_point.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_util.h"


/*------------------------------------------------------------------------------ 
  Utility macros
------------------------------------------------------------------------------*/
// Number of sensors supported by this driver (accel, gyro and mag )
#define QDSP_PLAYBACK_NUM_SENSORS           4

#define QDSP_PLAYBACK_CONFIG_FILE "qdsp_playback/qdsp_playback.cfg"
#define QDSP_PLAYBACK_STATUS_FILE "qdsp_playback/qdsp_playback_sim_status"


// Convert Degrees/sec to Radians/sec, Q16 format.
#define QDSP_PLAYBACK_DPS_TO_RPSQ16(x)      FX_FLTTOFIX_Q16((x) * PI / 180.0)

// Returns the size of a static table (i.e., array).
#define QDSP_PLAYBACK_TABLE_SIZE(a)         (sizeof(a) / sizeof(*a))

// Number of axes of inertial sensors
#define QDSP_PLAYBACK_NUM_INERTIAL_AXES     3

#define QDSP_PLAYBACK_MAX_FIFO_WATERMARK   100
#define QDSP_MAX_NUM_SAMPLES               100
#define QDSP_PLAYBACK_MD_MIN_WAIT_TIME      30

// Axis index
#define X 0
#define Y 1
#define Z 2

#define QDSP_PLAYBACK_IDLE_TO_READY_TIME      30
#define QDSP_PLAYBACK_DATA_TIME                5


#ifdef QDSP_PLAYBACK_DEBUG
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define QDSP_PLAYBACK_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "QDSP_PLAYBACK - " msg)
#define QDSP_PLAYBACK_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "QDSP_PLAYBACK - " msg,p1)
#define QDSP_PLAYBACK_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "QDSP_PLAYBACK - " msg,p1,p2)
#define QDSP_PLAYBACK_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "QDSP_PLAYBACK - " msg,p1,p2,p3)
#else
#define QDSP_PLAYBACK_MSG_0(level,msg)
#define QDSP_PLAYBACK_MSG_1(level,msg,p1)
#define QDSP_PLAYBACK_MSG_2(level,msg,p1,p2)
#define QDSP_PLAYBACK_MSG_3(level,msg,p1,p2,p3)
#endif

/*------------------------------------------------------------------------------ 
  Definitions shared across the driver's files
------------------------------------------------------------------------------*/
// Interrupts supported by this driver.
typedef enum {
    QDSP_PLAYBACK_INT_DATA_READY,
    QDSP_PLAYBACK_INT_MOTION_DETECT
} sns_dd_qdsp_playback_interrupt_t;


// Describes which sensors are enabled for data retrieval via DRI. 
typedef struct
{
    sns_ddf_sensor_e enabled[QDSP_PLAYBACK_NUM_SENSORS + 1]; // Sensors enabled for DRI
    uint8_t          num_enabled;                  // Size of 'enabled'
    uint8_t          enabled_markers;              
} sns_dd_qdsp_playback_dri_sensors_e;

typedef struct
{
  uint32_t               md_dup_warning;
  uint32_t               md_err;
  uint32_t               md_state;
  uint32_t               set_odr_err;
} sns_dd_qdsp_playback_cnt_s;


// A Playback driver instance.
typedef struct
{
   sns_ddf_handle_t      smgr_handle[QDSP_PLAYBACK_NUM_SENSORS + 1]; // Handle used to notify async events
   uint8_t               range[QDSP_PLAYBACK_NUM_SENSORS + 1];        // Current range selection for sensors
   q16_t                 accel_biases[3];
   q16_t                 gyro_biases[3];
   sns_ddf_axes_map_s    axes_map[QDSP_PLAYBACK_NUM_SENSORS + 1];
   
   uint32_t              reqed_odr[QDSP_PLAYBACK_NUM_SENSORS + 1];    //0, if there is no requested for the sensor type
   uint32_t              cur_odr[QDSP_PLAYBACK_NUM_SENSORS];
   uint32_t              supported_odr[QDSP_PLAYBACK_NUM_SENSORS + 1];
   FILE*                 input_fd[QDSP_PLAYBACK_NUM_SENSORS + 1];
   FILE*                 input_md_fd;
   bool                  fifo_supported[QDSP_PLAYBACK_NUM_SENSORS + 1];
   uint32_t              sensor_watermark[QDSP_PLAYBACK_NUM_SENSORS + 1];

   bool                  dri_enabled;
   bool                  md_enabled;
   sns_dd_qdsp_playback_cnt_s cnt;               // counts for history 
   sns_dd_qdsp_playback_dri_sensors_e  dri_sensors;

   sns_ddf_sensor_data_s          sensor_data;      // to use when notifying the data to SMGR 
   sns_ddf_sensor_data_s          data_cache[QDSP_PLAYBACK_NUM_SENSORS + 1];      // Holds the latest sample from input file 
} sns_dd_qdsp_playback_s;


/*------------------------------------------------------------------------------ 
  Declarations for functions defined in one file and used in another.
------------------------------------------------------------------------------*/

sns_ddf_status_e sns_dd_qdsp_playback_init(
    sns_ddf_handle_t*        dd_handle_ptr,
    sns_ddf_handle_t         smgr_handle,
    sns_ddf_nv_params_s*     nv_params,
    sns_ddf_device_access_s  device_info[],
    uint32_t                 num_devices,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_e*        sensors[],
    uint32_t*                num_sensors);

sns_ddf_status_e sns_dd_qdsp_playback_get_attr(
   sns_ddf_handle_t       dd_handle,
   sns_ddf_sensor_e       sensor,
   sns_ddf_attribute_e    attrib,
   sns_ddf_memhandler_s*  memhandler,
   void**                 value,
   uint32_t*              num_elems);

sns_ddf_status_e sns_dd_qdsp_playback_get_data(
   sns_ddf_handle_t       dd_handle,
   sns_ddf_sensor_e       sensors[],
   uint32_t               num_sensors,
   sns_ddf_memhandler_s*  memhandler,
   sns_ddf_sensor_data_s* data[]);

sns_ddf_status_e sns_dd_qdsp_playback_reset(sns_ddf_handle_t dd_handle);


sns_ddf_status_e sns_dd_qdsp_playback_enable_interrupt(
    sns_dd_qdsp_playback_s*           state,
    sns_dd_qdsp_playback_interrupt_t  interrupt);


#endif /* SNS_DD_QDSP_PLAYBACK_PRIV_H */

