/** 
 * @file sns_dd_daf_playback_accel_priv.h
 * @brief Common definitions for DAF playback driver for Accel.
 *  
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_daf_playback_accel_priv.h#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why 
  ---------- ---  -----------------------------------------------------------
  2015-04-09 pn   Changed all timestamps to sns_ddf_time_t
  2015-01-15 SH   Initial revision
==============================================================================*/

#ifndef SNS_DD_DAF_PLAYBACK_ACCEL__H
#define SNS_DD_DAF_PLAYBACK_ACCEL__H

#include "fixed_point.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_util.h"


/*------------------------------------------------------------------------------ 
  Macros
------------------------------------------------------------------------------*/

#define DAF_PLAYBACK_SENSOR_TYPE        SNS_DDF_SENSOR_ACCEL
#define DAF_PLAYBACK_NUM_AXES           3
#define DAF_PLAYBACK_MAX_NUM_SAMPLES    128


//#define DAF_PLAYBACK_DEBUG
#ifdef DAF_PLAYBACK_DEBUG
#define DAF_PLAYBACK_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "DAF_PLAYBACK_ACC - " msg)
#define DAF_PLAYBACK_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "DAF_PLAYBACK_ACC - " msg,p1)
#define DAF_PLAYBACK_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "DAF_PLAYBACK_ACC - " msg,p1,p2)
#define DAF_PLAYBACK_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "DAF_PLAYBACK_ACC - " msg,p1,p2,p3)
#else
#define DAF_PLAYBACK_MSG_0(level,msg)
#define DAF_PLAYBACK_MSG_1(level,msg,p1)
#define DAF_PLAYBACK_MSG_2(level,msg,p1,p2)
#define DAF_PLAYBACK_MSG_3(level,msg,p1,p2,p3)
#endif

/*------------------------------------------------------------------------------ 
  Types
------------------------------------------------------------------------------*/

// A Playback driver instance.
typedef struct
{
    sns_ddf_handle_t        smgr_handle;    // Handle used to notify async events

    // Note: ODRs are for information only. Data will be reported using the
    // timestamps given to the playback driver, which are not guaranteed to
    // match the current ODR.
    uint32_t                current_odr;
    uint32_t                recorded_odr;   // ODR from recorded data (that is being played back)

    bool                    sched_enabled;
    sns_ddf_timer_s         timer;

    sns_ddf_time_t          start_time;

    struct {
        sns_ddf_time_t      timestamp;
        q16_t               sample[DAF_PLAYBACK_NUM_AXES];
    } data_cache[DAF_PLAYBACK_MAX_NUM_SAMPLES]; // Stores incoming data samples
    uint8_t                 write_index;    // Next index in data_cache to write to
    uint8_t                 read_index;     // Next index in data_cache to read from

    sns_ddf_sensor_data_s   sensor_data;    // Holds one sample for notifying SMGR

    sns_ddf_motion_detect_enable_t md_enabled; // Is motion detection enabled?
} sns_dd_daf_playback_accel_s;

#endif /* SNS_DD_DAF_PLAYBACK_ACCEL__H */
