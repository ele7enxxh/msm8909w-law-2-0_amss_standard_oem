/*=============================================================================
  @file sns_smgr_util.c

  This file implements the utility functions of Sensor Manager

*******************************************************************************
* Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/smgr/src/sns_smgr_util_uimg.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-01-07  pn   Added more conditions for when sensor_is_ok() should return FALSE
  2015-12-09  jtl  tilt_event and ORIENTATION_EVENT added to on-change sensor list.
  2015-10-13  jtl  Added heartbeat check function
  2015-04-09  pn   Replaced sns_smgr_get_tick64() with sns_smgr_update_last_tick()
  2015-01-22  BD   Changed the way of putting functions in uImage to use sections
  2014-06-05  vy   Further refactored to support uImage
  2014-05-12  tc   Converted frequencies to fixed point, Q16. This is to
                   accommodate frequencies less than 1. Q16 fixed point is
                   used, instead of floating point, because the Q6 does not
                   have hardware-support for floating point division.
  2014-04-23  pn   Initial version

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "sns_em.h"
#include "sns_osa.h"
#include "sns_memmgr.h"

#include "sns_smgr_priv.h"
#include "sns_smgr_main.h"
#include "sns_smgr_util.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  Variables
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  Function Prototypes
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION:   sns_smgr_update_last_tick

===========================================================================*/
/*!
  @brief returns 64 bits current tick.

  @detail also the tick is saved into sns_smgr.last_tick so that SMGR can maintains the high_tick

  @param[i] NONE

  @return
    NONE

*/
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE void sns_smgr_update_last_tick(void)
{
  sns_smgr.last_tick = sns_em_get_system_time();
}

/*===========================================================================

  FUNCTION:   sns_smgr_sensor_is_ok

===========================================================================*/
/*!
  @brief Checks to see if the driver has returned samples in a timely manner

  @param[i] ddf_sensor_ptr - Pointer to driver to check

  @return
    true  - sensor/driver is operating correctly
    false - sensor/driver is NOT operating correctly

*/
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE bool sns_smgr_sensor_is_ok(
 const sns_smgr_ddf_sensor_s *ddf_sensor_ptr)
{
  int32_t delta_time;
  sns_ddf_time_t time_now = sns_ddf_get_timestamp();
  bool sensor_is_ok;

  if ( ddf_sensor_ptr->heartbeat_interval == SMGR_MAX_TICKS )
  {
    sensor_is_ok = true;
  }
  else if ( ddf_sensor_ptr->sensor_ptr->sensor_state == SENSOR_STATE_FAILED )
  {
    sensor_is_ok = false;
  }
  else
  {
    if ( ddf_sensor_ptr->latest_sample_ts != 0 )
    {
      delta_time = time_now - ddf_sensor_ptr->latest_sample_ts;
    }
    else
    {
      delta_time = time_now - ddf_sensor_ptr->sensor_ptr->odr_change_tick;
    }
    if ( delta_time > (int)ddf_sensor_ptr->heartbeat_interval )
    {
      sensor_is_ok = false;
    }
    else
    {
      sensor_is_ok = true;
    }
  }
  return sensor_is_ok;
}


/*===========================================================================

  FUNCTION:   sns_smgr_match_ddf_sensor

===========================================================================*/
/*!
  @brief Given the DDF sensor ID finds the corresponding ddf_sensor structure.
  @detail
  @param[i] sensor_ptr
  @param[i] ddf_sensor_type

  @return
   Pointer the the corresponding DDF sensor, or NULL of not found
 */
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE sns_smgr_ddf_sensor_s* sns_smgr_match_ddf_sensor(
  const sns_smgr_sensor_s* sensor_ptr,
  sns_ddf_sensor_e     ddf_sensor_type)
{
  sns_smgr_ddf_sensor_s* ddf_sensor_ptr = NULL;
  uint8_t i;
  for ( i=0; i<ARR_SIZE(sensor_ptr->ddf_sensor_ptr); i++ )
  {
    if ( (sensor_ptr->ddf_sensor_ptr[i] != NULL) &&
         (SMGR_SENSOR_TYPE(sensor_ptr, i) == ddf_sensor_type) )
    {
      ddf_sensor_ptr = sensor_ptr->ddf_sensor_ptr[i];
      break;
    }
  }
  return ddf_sensor_ptr;
}

/*===========================================================================

  FUNCTION:   sns_smgr_is_on_change_sensor

===========================================================================*/
/*!
  @brief Checks whether given sensor type is an on-change sensor.

  @Detail  An on-change sensor, as opposed to a streaming sensor, produces
           a new sample only when it detects a change in its surrounding.
           When configured for Polling mode, an on-change sensor appears
           to be a streaming sensor.

  @param[i] ddf_sensor_type - the sensor type defined by DDF

  @return
    TRUE if given sensor type is an on-change sensor
    FALSE to indicate streaming sensor
*/
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE bool sns_smgr_is_on_change_sensor(sns_ddf_sensor_e ddf_sensor_type)
{
  bool on_change = false;
  if ( (ddf_sensor_type == SNS_DDF_SENSOR_PROXIMITY)    ||
       (ddf_sensor_type == SNS_DDF_SENSOR_AMBIENT)      ||
       (ddf_sensor_type == SNS_DDF_SENSOR_HUMIDITY)     ||
       (ddf_sensor_type == SNS_DDF_SENSOR_RGB)          ||
       (ddf_sensor_type == SNS_DDF_SENSOR_CT_C)         ||
       (ddf_sensor_type == SNS_DDF_SENSOR_DOUBLETAP)    ||
       (ddf_sensor_type == SNS_DDF_SENSOR_IR_GESTURE)   ||
       (ddf_sensor_type == SNS_DDF_SENSOR_DOUBLETAP)    ||
       (ddf_sensor_type == SNS_DDF_SENSOR_STEP_EVENT )  ||
       (ddf_sensor_type == SNS_DDF_SENSOR_STEP_COUNT )  ||
       (ddf_sensor_type == SNS_DDF_SENSOR_SMD )         ||
       (ddf_sensor_type == SNS_DDF_SENSOR_SAR)          ||
       (ddf_sensor_type == SNS_DDF_SENSOR_HALL_EFFECT)  ||
       (ddf_sensor_type == SNS_DDF_SENSOR_AMBIENT_TEMP) ||
       (ddf_sensor_type == SNS_DDF_SENSOR_TILT_EVENT)   ||
       (ddf_sensor_type == SNS_DDF_SENSOR_ORIENTATION_EVENT))
  {
    on_change = true;
  }
  return on_change;
}

/*===========================================================================

  FUNCTION:   sns_smgr_is_event_sensor

===========================================================================*/
/*!
  @brief Checks whether given sensor is an event sensor.

  @Detail

  @param[i] sensor_ptr - the sensor leader structure
  @param[i] data_type  - primary or secondary

  @return
    TRUE if given sensor type is an event sensor
    FALSE to indicate streaming sensor
*/
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE bool sns_smgr_is_event_sensor(const sns_smgr_sensor_s* sensor_ptr, uint8_t data_type)
{
  bool is_event_sensor = false;
  if ( SMGR_SENSOR_IS_SELF_SCHED(sensor_ptr) )
  {
    is_event_sensor = 
      sns_smgr_is_on_change_sensor(SMGR_SENSOR_TYPE(sensor_ptr, data_type));
  }
  return is_event_sensor;
}


/*===========================================================================

  FUNCTION:   sns_smgr_ddf_sensor_is_event_sensor

===========================================================================*/
/*!
  @brief Checks whether given sensor is an event sensor.

  @Detail

  @param[i] ddf_sensor_ptr - the DDF sensor

  @return
    TRUE if given sensor type is an event sensor
    FALSE to indicate streaming sensor
*/
/*=========================================================================*/
SNS_SMGR_UIMAGE_CODE bool sns_smgr_ddf_sensor_is_event_sensor(const sns_smgr_ddf_sensor_s* ddf_sensor_ptr)
{
  return sns_smgr_is_event_sensor(ddf_sensor_ptr->sensor_ptr,
                                  ddf_sensor_ptr->data_type);
}

