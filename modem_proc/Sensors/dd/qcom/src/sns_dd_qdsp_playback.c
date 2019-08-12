/**
 * @file sns_dd_qdsp_playback.c
 * @brief Playback driver, supporting accel, gyro and mag.
 *
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_qdsp_playback.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2014-02-11 ae   FIFO support
  2013-06-11 ae   Initial revision
==============================================================================*/


#include <stdbool.h>
#include <stdio.h>
#include "sns_dd_qdsp_playback_priv.h"
#include "fixed_point.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_dd_inertial_test.h"
#include "sns_ddf_signal.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"

// Make this driver a singleton by only using one instance, saved here.
sns_dd_qdsp_playback_s* qdsp_playback_singleton = NULL;

sns_ddf_time_t md_timestamp = ~0, playback_next_sample_ts;
bool md_timestamp_valid=false, md_int_happened = false;
        
// The driver presents "virtual" instances to SMGR while maintaining
// a singleton instance underneath. This is done in order to circumvent a
// limitation where SMGR cannot support more than two data types per driver.
// For simplicity, we assume the sensors are initialized in the oder as defined here.
static sns_ddf_sensor_e sensors_instance[QDSP_PLAYBACK_NUM_SENSORS] = 
    						                  {
											   SNS_DDF_SENSOR_ACCEL,
	                                           SNS_DDF_SENSOR_GYRO,
                                               SNS_DDF_SENSOR_MAG,
                                               SNS_DDF_SENSOR_PRESSURE,
                                              };
// sensors type for playback only care the first/primary sensor type
// ie., secondary sensor type is ignored
static int num_instances = 0;

uint8_t qdsp_playback_num_fifo_sensors =0;

boolean DummyData=FALSE;

// Forward declaration of driver interface functions.
sns_ddf_driver_if_s sns_dd_qdsp_playback_if;

// Accelerometer sensitivity values, in G/LSB. These must correlate directly
// with the range setting in sns_dd_qdsp_playback_accel_range[].
q16_t sns_dd_qdsp_playback_accel_sensitivity[] =
{
    FX_FLTTOFIX_Q16(G / (1024 * 2))
};

// correlate directly with the range setting in sns_dd_qdsp_playback_gyro_range[].
const q16_t sns_dd_qdsp_playback_gyro_sensitivity[] =
{
    QDSP_PLAYBACK_DPS_TO_RPSQ16(1.0 / 16.4)  << 9
};


#define QDSP_PLAYBACK_MAG_SENSITIVITY	(0.15f)   // uT/LSB
#define UTESLA_TO_GAUSS			(0.01f)   // Convert from uT to Gauss
const q16_t sns_dd_qdsp_playback_mag_sensitivity[] =
{
	FX_FLTTOFIX_Q16((QDSP_PLAYBACK_MAG_SENSITIVITY * UTESLA_TO_GAUSS))
};

const q16_t sns_dd_qdsp_playback_pressure_sensitivity[] =
{
    FX_FLTTOFIX_Q16(0.01)
};

// Accelerometer range values
static sns_ddf_range_s sns_dd_qdsp_playback_accel_range[] =
{
   { FX_FLTTOFIX_Q16(-16 * G), FX_FLTTOFIX_Q16(16 * G) }
};

// Gyroscope range values
static sns_ddf_range_s sns_dd_qdsp_playback_gyro_range[] =
{
   { QDSP_PLAYBACK_DPS_TO_RPSQ16(-2000), QDSP_PLAYBACK_DPS_TO_RPSQ16(2000) }
};

// Mag range values
static sns_ddf_range_s sns_dd_qdsp_playback_mag_range[] =
{
   { QDSP_PLAYBACK_DPS_TO_RPSQ16(-3219128), QDSP_PLAYBACK_DPS_TO_RPSQ16(3219128) }
};

// Pressure range values
static sns_ddf_range_s sns_dd_qdsp_playback_pressure_range[] =
{
   { FX_FLTTOFIX_Q16(300.0), FX_FLTTOFIX_Q16(1100.0) }
};

static q16_t playback_lpf = 1 << 16; //value 1.0 hz 

// Supported sensor types
static sns_ddf_sensor_e sns_dd_qdsp_playback_sensors[] =
{
   SNS_DDF_SENSOR_ACCEL,
   SNS_DDF_SENSOR_GYRO,
   SNS_DDF_SENSOR_MAG,
};


static sns_ddf_status_e sns_dd_qdsp_playback_set_odr(
    sns_dd_qdsp_playback_s* state, sns_ddf_sensor_e    sensor, uint32_t odr);


sns_ddf_status_e sns_dd_qdsp_playback_enable_interrupt(
    sns_dd_qdsp_playback_s*           state,
    sns_dd_qdsp_playback_interrupt_t  interrupt)
{
    sns_ddf_status_e status = SNS_DDF_EFAIL;

    // Configure interrupt specific parameters
    switch(interrupt)
    {
    case QDSP_PLAYBACK_INT_MOTION_DETECT:
        if(state->input_md_fd != NULL)
        {
            state->md_enabled = TRUE;
            state->dri_enabled = FALSE;
		    status = SNS_DDF_SUCCESS;
        }
        break;

    case QDSP_PLAYBACK_INT_DATA_READY:

        state->dri_enabled = TRUE;
		status = SNS_DDF_SUCCESS;
        break;

    default:
        return SNS_DDF_EINVALID_PARAM;
    }

    return status;
}

/**
 * @brief disable interrupt
 * 
 */
static sns_ddf_status_e sns_dd_qdsp_playback_disable_interrupt(
    sns_dd_qdsp_playback_s*           state,
    sns_dd_qdsp_playback_interrupt_t  interrupt)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    switch(interrupt)
    {
    case QDSP_PLAYBACK_INT_DATA_READY:
        state->dri_enabled = FALSE;
        break;

    case QDSP_PLAYBACK_INT_MOTION_DETECT:
        state->md_enabled = FALSE;
        break;

    default:
        return SNS_DDF_EINVALID_PARAM;
    }

    return status;
}


static void sns_dd_qdsp_playback_add_sensor_pending_dri(
    sns_dd_qdsp_playback_s* state, sns_ddf_sensor_e sensor)
{
    uint8_t sensor_mask = (1 << sensor);

    // Check for duplicates
    if(state->dri_sensors.enabled_markers & sensor_mask)
        return;

    // Check for overflow
    if(state->dri_sensors.num_enabled >= QDSP_PLAYBACK_NUM_SENSORS)
        return;

    state->dri_sensors.enabled[state->dri_sensors.num_enabled++] = sensor;
    state->dri_sensors.enabled_markers ^= sensor_mask;
}


static void sns_dd_qdsp_playback_remove_sensor_pending_dri(
    sns_dd_qdsp_playback_s* state, sns_ddf_sensor_e sensor)
{
    uint8_t sensor_mask = (1 << sensor);

    // Check for presence
    if((state->dri_sensors.enabled_markers & sensor_mask) == 0)
        return;

    // Mark the sensor as pending DRI
    state->dri_sensors.enabled_markers ^= sensor_mask;

    // Recreate the list of pending sensors
    state->dri_sensors.num_enabled = 0;

    if(state->dri_sensors.enabled_markers & (1 << SNS_DDF_SENSOR_ACCEL))
        state->dri_sensors.enabled[state->dri_sensors.num_enabled++] = SNS_DDF_SENSOR_ACCEL;

    if(state->dri_sensors.enabled_markers & (1 << SNS_DDF_SENSOR_MAG))
        state->dri_sensors.enabled[state->dri_sensors.num_enabled++] = SNS_DDF_SENSOR_MAG;

    if(state->dri_sensors.enabled_markers & (1 << SNS_DDF_SENSOR_GYRO))
        state->dri_sensors.enabled[state->dri_sensors.num_enabled++] = SNS_DDF_SENSOR_GYRO;
}


sns_ddf_status_e sns_dd_qdsp_playback_enable_sched_data(
    sns_ddf_handle_t  handle,
    sns_ddf_sensor_e  sensor,
    bool              enable)
{
  sns_dd_qdsp_playback_s* state = qdsp_playback_singleton;

  if(enable)
  {
    /* Don't enable DRI interrupt if it is in motion detection mode */
    if ( (SNS_DDF_SENSOR_ACCEL != sensor) || (FALSE == state->md_enabled) )
    {
      sns_dd_qdsp_playback_enable_interrupt(state, QDSP_PLAYBACK_INT_DATA_READY);
      sns_dd_qdsp_playback_add_sensor_pending_dri(state, sensor);
    }
  }
  else
  {
    sns_dd_qdsp_playback_remove_sensor_pending_dri(state, sensor);
    if(state->dri_sensors.num_enabled == 0)
    {
      sns_dd_qdsp_playback_disable_interrupt(state, QDSP_PLAYBACK_INT_DATA_READY);
    }
  }
  return SNS_DDF_SUCCESS;
}



static sns_ddf_status_e sns_dd_qdsp_playback_set_odr (
    sns_dd_qdsp_playback_s* state, sns_ddf_sensor_e sensor, uint32_t odr)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  state->reqed_odr[sensor] = odr;
  state->cur_odr[sensor] = state->supported_odr[sensor];
  QDSP_PLAYBACK_MSG_2(HIGH, "set odr for sensor %d  odr=%d", sensor, odr);
  return status;
}


/**
 * @brief Sets the device to its default operational state
 * @see sns_ddf_driver_if_s.reset()
*/
sns_ddf_status_e sns_dd_qdsp_playback_reset(sns_ddf_handle_t dd_handle)
{
    return SNS_DDF_SUCCESS;
}


/**
 * @brief Sets an attribute of a sensor
 * @see sns_ddf_driver_if_s.set_attrib()
 */
sns_ddf_status_e sns_dd_qdsp_playback_set_attr(
   sns_ddf_handle_t    dd_handle,
   sns_ddf_sensor_e    sensor,
   sns_ddf_attribute_e attrib,
   void*               value)
{
  sns_dd_qdsp_playback_s* state = qdsp_playback_singleton;

  if (dd_handle == NULL || value == NULL)
     return SNS_DDF_EINVALID_PARAM;

  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_STATE:
    {
      return SNS_DDF_SUCCESS;
    }

    case SNS_DDF_ATTRIB_RANGE:
    {
      return SNS_DDF_SUCCESS; 
    }

    case SNS_DDF_ATTRIB_LOWPASS:
    {
      return SNS_DDF_SUCCESS;
    }

    case SNS_DDF_ATTRIB_ODR:
    {
      sns_ddf_odr_t* odr = value;
      if ( (SNS_DDF_SENSOR_ACCEL == sensor) && (*odr > 0) && (state->md_enabled) )
      {
        QDSP_PLAYBACK_MSG_3(HIGH, "set_attr %d odr=%d md_en=%d", 
                      SNS_DDF_ATTRIB_ODR, *odr, state->md_enabled);
        state->cnt.set_odr_err++;
        return SNS_DDF_EFAIL;
      } 
      else
      {
        return sns_dd_qdsp_playback_set_odr(state, sensor, *odr);
      }
    }

    case SNS_DDF_ATTRIB_MOTION_DETECT:
    {
      sns_ddf_status_e status = SNS_DDF_EFAIL;
      sns_ddf_motion_detect_enable_t* flag = value;

      QDSP_PLAYBACK_MSG_3(HIGH, "set_attr %d md_en=%d flag=%d",
                    SNS_DDF_ATTRIB_ODR, state->md_enabled, *flag);
      if ( *flag == state->md_enabled )     /* is it a redundant request */
      {
        state->cnt.md_dup_warning++;
        return SNS_DDF_SUCCESS;
      }
      else if(*flag)
      {
        if ( state->reqed_odr[SNS_DDF_SENSOR_ACCEL] )   /* Accel shall be activated for MOTION_DETECT */
        {
          status = sns_dd_qdsp_playback_enable_interrupt(state, QDSP_PLAYBACK_INT_MOTION_DETECT);
          if( SNS_DDF_SUCCESS == status )
          {
            state->cnt.md_state++;
            sns_dd_qdsp_playback_set_odr(state, SNS_DDF_SENSOR_ACCEL, state->reqed_odr[SNS_DDF_SENSOR_ACCEL]);
          }
          else
          {
            state->cnt.md_err++;
          }
          /* enable motion detection */
        }
        else
        {
          state->cnt.md_err++;
        }
        return status;
      }
      else
      {
        /* disable motion detection */
        status = sns_dd_qdsp_playback_disable_interrupt( state, QDSP_PLAYBACK_INT_MOTION_DETECT);

        /* recover the previous freq */
        sns_dd_qdsp_playback_set_odr(state, SNS_DDF_SENSOR_ACCEL, state->reqed_odr[SNS_DDF_SENSOR_ACCEL]);
        state->md_enabled = FALSE;
        QDSP_PLAYBACK_MSG_3(HIGH, "set_attr %d status=%d md_state=%d",
                      SNS_DDF_ATTRIB_ODR, status, state->cnt.md_state);
        return status;
      }
    }
    case SNS_DDF_ATTRIB_FIFO:
    {
      // set fifo attribute.
      sns_ddf_fifo_set_s* fifo = value;

      if(state->fifo_supported[sensor] != TRUE)
      {
          return SNS_DDF_EINVALID_PARAM;
      }

      state->sensor_watermark[sensor] = fifo->watermark;
      return SNS_DDF_SUCCESS;
    }
    default:
      return SNS_DDF_EINVALID_PARAM;
  }
}



/** 
 * @brief Retrieves the value of an attribute for a sensor.
 * @see sns_ddf_driver_if_s.get_attrib()
 */
sns_ddf_status_e sns_dd_qdsp_playback_get_attr(
   sns_ddf_handle_t      dd_handle,
   sns_ddf_sensor_e      sensor,
   sns_ddf_attribute_e   attrib,
   sns_ddf_memhandler_s* memhandler,
   void**                value,
   uint32_t*             num_elems)
{
    sns_dd_qdsp_playback_s* state = qdsp_playback_singleton;

    if(dd_handle == NULL || value == NULL)
       return SNS_DDF_EINVALID_PARAM;

    switch(attrib)
    {
       case SNS_DDF_ATTRIB_POWER_INFO:
       {
          sns_ddf_power_info_s* power_attrib = sns_ddf_memhandler_malloc(
              memhandler, sizeof(sns_ddf_power_info_s));
          if (power_attrib == NULL)
             return SNS_DDF_ENOMEM;

          if (sensor <= QDSP_PLAYBACK_NUM_SENSORS )
          {
              power_attrib->active_current = 500;
              power_attrib->lowpower_current = 5;
	      } 
          else 
          {
              return SNS_DDF_EINVALID_PARAM;
          }

          *value = power_attrib;
          *num_elems = 1;
          return SNS_DDF_SUCCESS;
       }

       case SNS_DDF_ATTRIB_RANGE:
       {
           switch(sensor)
           {
           case SNS_DDF_SENSOR_ACCEL:
               *value = sns_dd_qdsp_playback_accel_range;
               *num_elems = QDSP_PLAYBACK_TABLE_SIZE(sns_dd_qdsp_playback_accel_range);
               return SNS_DDF_SUCCESS;

           case SNS_DDF_SENSOR_MAG:
               *value = sns_dd_qdsp_playback_mag_range;
               *num_elems = QDSP_PLAYBACK_TABLE_SIZE(sns_dd_qdsp_playback_mag_range);
               return SNS_DDF_SUCCESS;

           case SNS_DDF_SENSOR_GYRO:
               *value = sns_dd_qdsp_playback_gyro_range;
               *num_elems = QDSP_PLAYBACK_TABLE_SIZE(sns_dd_qdsp_playback_gyro_range);
               return SNS_DDF_SUCCESS;

           case SNS_DDF_SENSOR_PRESSURE:
               *value = sns_dd_qdsp_playback_pressure_range;
               *num_elems = QDSP_PLAYBACK_TABLE_SIZE(sns_dd_qdsp_playback_pressure_range);
               return SNS_DDF_SUCCESS;

           default:
               break;
           }
       }

       case SNS_DDF_ATTRIB_LOWPASS:
       {
           switch(sensor)
           {
           case SNS_DDF_SENSOR_ACCEL:
           case SNS_DDF_SENSOR_GYRO:
               *value = &playback_lpf;
               *num_elems = 1;
               return SNS_DDF_SUCCESS;

           case SNS_DDF_SENSOR_MAG:
           case SNS_DDF_SENSOR_PRESSURE:
           default:
               return SNS_DDF_EINVALID_PARAM;
           }
       }

       case SNS_DDF_ATTRIB_ODR:
       {
          uint32_t *odr_ptr;

          odr_ptr = &state->reqed_odr[sensor];

          if (0 != *odr_ptr)
          {
            odr_ptr = &state->cur_odr[sensor];    /* returns current odr */
          }
          *value = odr_ptr;
          *num_elems = 1;
          return SNS_DDF_SUCCESS;
       }

       case SNS_DDF_ATTRIB_BIAS:
       {
           return SNS_DDF_EFAIL;
       }

       case SNS_DDF_ATTRIB_DRIVER_INFO:
       {
          sns_ddf_driver_info_s* driver_info = sns_ddf_memhandler_malloc(
              memhandler, sizeof(sns_ddf_driver_info_s));
          if(driver_info == NULL)
             return SNS_DDF_ENOMEM;

          driver_info->name = "Qualcomm QDSP_PLAYBACK Driver";
          driver_info->version = 1;

          *value = driver_info;
          *num_elems = 1;
          return SNS_DDF_SUCCESS;
       }

       case SNS_DDF_ATTRIB_DEVICE_INFO:
       {
          sns_ddf_device_info_s* device_info = sns_ddf_memhandler_malloc(
              memhandler, sizeof(sns_ddf_device_info_s));
          if(device_info == NULL)
             return SNS_DDF_ENOMEM;

          switch(sensor) 
          {
              case SNS_DDF_SENSOR_ACCEL:
              device_info->name = "Accelerometer";
              break;
              case SNS_DDF_SENSOR_MAG:
              device_info->name = "Mag";
              break;
              case SNS_DDF_SENSOR_GYRO:
              device_info->name = "Gyro";
              break;
              case SNS_DDF_SENSOR_PRESSURE:
              device_info->name = "Pressure";
              break;
              default:
              return SNS_DDF_EINVALID_PARAM;
          }
          device_info->vendor = "Qualcomm";
          device_info->model = "QDSP_PLAYBACK";
          device_info->version = 1;

          *value = device_info;
          *num_elems = 1;
          return SNS_DDF_SUCCESS;
       }

       case SNS_DDF_ATTRIB_DELAYS:
       {
          sns_ddf_delays_s *delays = sns_ddf_memhandler_malloc(
              memhandler, sizeof(sns_ddf_delays_s));
          if (delays == NULL)
             return SNS_DDF_ENOMEM;

          delays->time_to_active = QDSP_PLAYBACK_IDLE_TO_READY_TIME;
          delays->time_to_data = QDSP_PLAYBACK_DATA_TIME;   

          *value = delays;
          *num_elems = 1;
          return SNS_DDF_SUCCESS;
       }

       case SNS_DDF_ATTRIB_RESOLUTION_ADC:
       {
          sns_ddf_resolution_adc_s *device_res = sns_ddf_memhandler_malloc(
              memhandler, sizeof(sns_ddf_resolution_adc_s));
          if(device_res == NULL)
             return SNS_DDF_ENOMEM;
          
          device_res->bit_len = 16;
          device_res->max_freq = state->supported_odr[sensor]; //provided in config file
          *value = device_res;
          *num_elems = 1;
          return SNS_DDF_SUCCESS;
       }

       case SNS_DDF_ATTRIB_RESOLUTION:
       {
          sns_ddf_resolution_t* device_res = sns_ddf_memhandler_malloc(
              memhandler, sizeof(sns_ddf_resolution_t));
          if(device_res == NULL)
             return SNS_DDF_ENOMEM;
     
          switch(sensor)
          {
              case SNS_DDF_SENSOR_GYRO:
              *device_res = sns_dd_qdsp_playback_gyro_sensitivity[state->range[SNS_DDF_SENSOR_GYRO]] >> 9;
              break;

              case SNS_DDF_SENSOR_ACCEL:
              *device_res = sns_dd_qdsp_playback_accel_sensitivity[state->range[SNS_DDF_SENSOR_ACCEL]];
              break;

              case SNS_DDF_SENSOR_MAG:
              *device_res = sns_dd_qdsp_playback_mag_sensitivity[state->range[SNS_DDF_SENSOR_MAG]];
              break;

              case SNS_DDF_SENSOR_PRESSURE:
              *device_res = sns_dd_qdsp_playback_pressure_sensitivity[state->range[SNS_DDF_SENSOR_PRESSURE]];
              break;

              default:
              break;
          }

          *value = device_res;
          *num_elems = 1;
          return SNS_DDF_SUCCESS;
       }

     case SNS_DDF_ATTRIB_FIFO:
       {
            uint32_t i=0, j=0, size=0;
            sns_ddf_fifo_attrib_get_s *fifo_attr = NULL;

            if(state->fifo_supported[sensor] != TRUE)
            {
                return SNS_DDF_EINVALID_PARAM;
            }
           
            size= sizeof(sns_ddf_fifo_attrib_get_s)+
                       (qdsp_playback_num_fifo_sensors-1)*sizeof(sns_ddf_fifo_share_sensor_s *);
            
            fifo_attr=sns_ddf_memhandler_malloc(memhandler ,size);

            if( fifo_attr == NULL  )
            {
                return SNS_DDF_ENOMEM;
            }

            size = sizeof(sns_ddf_fifo_share_sensor_s);
            for ( i=0; i<( qdsp_playback_num_fifo_sensors-1); i++)
            {
               fifo_attr->share_sensors[i] = sns_ddf_memhandler_malloc(memhandler,size);
               if( fifo_attr->share_sensors[i]==NULL )
               {
                   return SNS_DDF_ENOMEM;
               }
            }
            
            fifo_attr->is_supported = true;
            fifo_attr->is_sw_watermark = false;
            // For now use all of FIFO size.
            fifo_attr->max_watermark = QDSP_PLAYBACK_MAX_FIFO_WATERMARK;
            fifo_attr->share_sensor_cnt = qdsp_playback_num_fifo_sensors - 1;

            for(i=1, j=0; i <= QDSP_PLAYBACK_NUM_SENSORS; i++)
            {
                if(i != sensor && state->fifo_supported[i])
                {
                   fifo_attr->share_sensors[j]->sensor =  i;
                   fifo_attr->share_sensors[j]->dd_handle = &sensors_instance[i];
                   j++;
                }
            }

            *value = fifo_attr;
            *num_elems = 1;

            return SNS_DDF_SUCCESS;
       }

       default:
          return SNS_DDF_EINVALID_PARAM;
    }
}



/**
 * @brief Called to get the latest sample from sensor input logs.
 */
sns_ddf_sensor_e sns_dd_qdsp_playback_get_latest_sample(sns_dd_qdsp_playback_s* state)
{
   int i, j, num_items;
   sns_ddf_time_t time_i=~0, time_j=~0;

   /* Get the latest sample from all sensors input files */
   for (i = 1; i <= QDSP_PLAYBACK_NUM_SENSORS; i++) 
   {
        if(state->data_cache[i].status == SNS_DDF_EINVALID_DATA && state->input_fd[i]) 
        {
            switch(i) 
            {
                case SNS_DDF_SENSOR_ACCEL:
                case SNS_DDF_SENSOR_GYRO:
                case SNS_DDF_SENSOR_MAG:
                    if(state->fifo_supported[i] == TRUE)
                    {
                       num_items = fscanf(state->input_fd[i],  "%d,%d,%d,",
 	                            (int*)&state->data_cache[i].timestamp,
 	                            (int*)&state->data_cache[i].end_timestamp,
 	                            (int*)&state->data_cache[i].num_samples);

                       if(num_items != 3)
                           return SNS_DDF_SENSOR__NONE; 

                       for(j = 0; j < state->data_cache[i].num_samples; j++)
                       {
                           num_items = fscanf(state->input_fd[i],  "%d,%d,%d,",
	                                     (int*)&(state->data_cache[i].samples[3*j + 0].sample),
	                                     (int*)&(state->data_cache[i].samples[3*j + 1].sample),
	                                     (int*)&(state->data_cache[i].samples[3*j + 2].sample)); 
                           if(num_items != 3)
                              return SNS_DDF_SENSOR__NONE; 
                       }
                       state->data_cache[i].num_samples = 3 * state->data_cache[i].num_samples;
                       state->data_cache[i].status = SNS_DDF_SUCCESS; //mark the data as valid.
                    } else 
                    {
                        num_items = fscanf(state->input_fd[i],  "%d,%d,%d,%d\n",
 	                            (int*)&state->data_cache[i].timestamp,
	                            (int*)&(state->data_cache[i].samples[0].sample),
	                            (int*)&(state->data_cache[i].samples[1].sample),
	                            (int*)&(state->data_cache[i].samples[2].sample)); 
                        if(num_items != 4)
                             return SNS_DDF_SENSOR__NONE; 
                        state->data_cache[i].status = SNS_DDF_SUCCESS; //mark the data as valid.
                    }
                   break;
                case SNS_DDF_SENSOR_PRESSURE:
                   if(state->fifo_supported[i] == TRUE)
                    {
                       num_items = fscanf(state->input_fd[i],  "%d,%d,%d,",
 	                            (int*)&state->data_cache[i].timestamp,
 	                            (int*)&state->data_cache[i].end_timestamp,
 	                            (int*)&state->data_cache[i].num_samples);

                       if(num_items != 3)
                           return SNS_DDF_SENSOR__NONE; 

                       for(j = 0; j < state->data_cache[i].num_samples; j++)
                       {
                           num_items = fscanf(state->input_fd[i],  "%d,",
	                                     (int*)&(state->data_cache[i].samples[j].sample));
                           if(num_items != 1)
                              return SNS_DDF_SENSOR__NONE; 
                       }
                       state->data_cache[i].status = SNS_DDF_SUCCESS; //mark the data as valid.
                    } else
                    {
                            num_items = fscanf(state->input_fd[i],  "%d,%d\n",
 	                                    (int*)&state->data_cache[i].timestamp,
	                                    (int*)&(state->data_cache[i].samples[0].sample));
                             if(num_items != 2)
                               return SNS_DDF_SENSOR__NONE; 
                            state->data_cache[i].status = SNS_DDF_SUCCESS; //mark the data as valid.
                    }

                default:
                break;

            }
        }
    }

    /* Find the sample with the earliest timestamp assuming atleast one valied fd */
    for (i=1, j=1; i <= QDSP_PLAYBACK_NUM_SENSORS; i++) 
    {
        if((state->data_cache[i].status == SNS_DDF_SUCCESS) && state->input_fd[i]) 
        {
            time_i = state->fifo_supported[i] ? state->data_cache[i].end_timestamp : 
                                                state->data_cache[i].timestamp; 
            if(state->input_fd[j])
            {
                time_j = state->fifo_supported[j] ? state->data_cache[j].end_timestamp : 
                                                    state->data_cache[j].timestamp;
            }
            if(time_i < time_j)
            {
                j = i;
            }
        }
    }

    playback_next_sample_ts = state->data_cache[j].timestamp;

    return j;
}




static void sns_dd_qdsp_playback_log_data(
    sns_dd_qdsp_playback_s* state,
    sns_ddf_sensor_e  sensor)
{
    sns_err_code_e status;
    sns_log_sensor_data_pkt_s* log;
    uint8_t i;

    status = sns_logpkt_malloc(
        SNS_LOG_CONVERTED_SENSOR_DATA,
        sizeof(sns_log_sensor_data_pkt_s),
        (void**)&log);

    if(status != SNS_SUCCESS || log == NULL)
    {
        // Can't allocate log packet. Ignore silently.
        return;
    }

    log->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log->sensor_id = sensor;
    log->vendor_id = SNS_DDF_VENDOR_INVENSENSE; //use smth
    log->timestamp = state->data_cache[sensor].timestamp;

    switch(sensor)
    {
       case SNS_DDF_SENSOR_ACCEL:
            log->num_data_types = 3;
            break;

       case SNS_DDF_SENSOR_GYRO:
            log->num_data_types = 3;
            break;

       case SNS_DDF_SENSOR_MAG:
            log->num_data_types = 3;
            break;
       case SNS_DDF_SENSOR_PRESSURE:
            log->num_data_types = 1;
            break;
       default: 
            log->num_data_types = 0;
            break;
    }

    for(i = 0; i < state->sensor_data.num_samples; i++)
    {
      log->data[i] = state->data_cache[sensor].samples[i].sample;
    }

    // Commit and free the log packet.
    sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, log);
}


/**
 * @brief Called by the SMGR to get instantaneous data
 * @see sns_ddf_driver_if_s.get_data()
 */
sns_ddf_status_e sns_dd_qdsp_playback_get_data(
   sns_ddf_handle_t       dd_handle,
   sns_ddf_sensor_e       sensors[],
   uint32_t               num_sensors,
   sns_ddf_memhandler_s*  memhandler,
   sns_ddf_sensor_data_s* data[])
{
    uint8_t i, j;
    sns_ddf_sensor_data_s* data_iter;
    sns_dd_qdsp_playback_s* state = qdsp_playback_singleton;

    if(dd_handle == NULL || sensors == NULL || memhandler == NULL || data == NULL)
        return SNS_DDF_EINVALID_PARAM;

    for(i = 0; i < num_sensors; i++)
    {
       if(sensors[i] != SNS_DDF_SENSOR_ACCEL &&
          sensors[i] != SNS_DDF_SENSOR_GYRO &&
          sensors[i] != SNS_DDF_SENSOR_MAG)
          return SNS_DDF_EINVALID_PARAM;
    }

    *data = sns_ddf_memhandler_malloc(
        memhandler, num_sensors * sizeof(sns_ddf_sensor_data_s));

    if(*data == NULL)
        return SNS_DDF_ENOMEM;

    for(i = 0; i < num_sensors; i++)
    {
       data_iter = &((*data)[i]);

       data_iter->sensor = sensors[i];
       data_iter->timestamp = sns_ddf_get_timestamp();
       data_iter->status = SNS_DDF_SUCCESS;

       switch(sensors[i])
       {
         case SNS_DDF_SENSOR_ACCEL:
              data_iter->num_samples = 3;
              break;
         case SNS_DDF_SENSOR_GYRO:
              data_iter->num_samples = 3;
              break;
         case SNS_DDF_SENSOR_MAG:
              data_iter->num_samples = 3;
              break;
         default:
              return SNS_DDF_EINVALID_PARAM;
       }

       data_iter->samples = sns_ddf_memhandler_malloc(
           memhandler, sizeof(sns_ddf_sensor_sample_s) * data_iter->num_samples);

       if (data_iter->samples == NULL)
           return SNS_DDF_ENOMEM;

	   if (DummyData == TRUE)
       {
           static uint32_t dummyData = 1;
           for (j = 0; j < data_iter->num_samples; j++)
           {
           data_iter->samples[j].sample = dummyData++;
           data_iter->samples[j].status = SNS_DDF_SUCCESS;
           }
	       return SNS_DDF_SUCCESS;
       }

       if(state->data_cache[sensors[i]].status == SNS_DDF_EINVALID_DATA) 
            sns_dd_qdsp_playback_get_latest_sample(state); 

       for (j = 0; j < data_iter->num_samples; j++)
       {
           data_iter->samples[j].sample = state->data_cache[sensors[i]].samples[j].sample;
           data_iter->samples[j].status = SNS_DDF_SUCCESS;
       }
        state->data_cache[sensors[i]].status = SNS_DDF_EINVALID_DATA;
    }
    return SNS_DDF_SUCCESS;
}

/**
 * @brief Called when motion detect is enabled to parse md log.
 */
bool sns_dd_qdsp_playback_md(sns_dd_qdsp_playback_s* state)
{
    int i,j;
    sns_ddf_time_t time_i, time_j;

    if(state->input_md_fd == NULL)
    {
        return FALSE;
    }
                   
    if(!md_timestamp_valid)
    {
        if( fscanf(state->input_md_fd, "%d\n", (int*)&md_timestamp) == EOF)
        {
            /* If last md is generated set it to some large value */
            md_timestamp = ~0;
        }
        md_timestamp_valid = TRUE;
    }

    /* Find the sample with the earliest timestamp assuming atleast one valied fd */
    for (i=1, j=1; i <= QDSP_PLAYBACK_NUM_SENSORS; i++) 
    {
        if(state->data_cache[i].status == SNS_DDF_SUCCESS && state->input_fd[i]) 
        {
            time_i = state->fifo_supported[i] ? state->data_cache[i].end_timestamp : 
                                                state->data_cache[i].end_timestamp; 
            time_j = state->fifo_supported[j] ? state->data_cache[j].end_timestamp : 
                                                state->data_cache[j].timestamp;
            if(time_i < time_j)
            {
                j = i;
            }
        }
    }

    if (md_timestamp <= 
            (state->fifo_supported[j] ? state->data_cache[j].end_timestamp : 
                                        state->data_cache[j].timestamp))
    {
          return TRUE;
    }

    return FALSE;
}
    




/**
 * @brief Called in response to an interrupt for this driver.
 * @see sns_ddf_driver_if_s.handle_irq()
 */
static void sns_dd_qdsp_playback_handle_irq(
        sns_ddf_handle_t  dd_handle,
        uint32_t          gpio_num,
        sns_ddf_time_t    timestamp)
{
    int j;
    sns_ddf_handle_t smgr_handle;
	sns_dd_qdsp_playback_s* state = qdsp_playback_singleton;
    sns_ddf_sensor_e sensor=SNS_DDF_SENSOR__NONE;
   

    /* Get next sensor sample */
	sensor = sns_dd_qdsp_playback_get_latest_sample(state);

     /* Get next motion detect timestamp */
    if ((state->md_enabled == FALSE) && (md_timestamp_valid == FALSE))
    {
        sns_dd_qdsp_playback_md(state);
    }

    /* If motion detect notify event */
    if (state->md_enabled && sns_dd_qdsp_playback_md(state))
    {
        
        static sns_ddf_time_t start_tick=0;

        if(start_tick == 0)
        {
            start_tick = sns_ddf_get_timestamp();
        }
        if (( sns_ddf_get_timestamp() - start_tick ) < QDSP_PLAYBACK_MD_MIN_WAIT_TIME)
        {
            return;
        }

        start_tick = 0;
        md_timestamp_valid = FALSE;
        md_int_happened = TRUE;
		sns_dd_qdsp_playback_disable_interrupt(state, QDSP_PLAYBACK_INT_MOTION_DETECT);
		sns_ddf_smgr_notify_event(state->smgr_handle[SNS_DDF_SENSOR_ACCEL],
								  SNS_DDF_SENSOR_ACCEL,
								  SNS_DDF_EVENT_MOTION);
        /* make accel stream */
        state->md_enabled = FALSE;
        sns_dd_qdsp_playback_set_odr(state, SNS_DDF_SENSOR_ACCEL, 
		                             state->reqed_odr[SNS_DDF_SENSOR_ACCEL]);
        return;
     }

     md_int_happened = FALSE;

     if(md_timestamp < playback_next_sample_ts)
     {
         return;
     }

     if (state->dri_enabled) 
     {
        if(sensor == SNS_DDF_SENSOR__NONE) 
        {
            //indicate that the simulation is over as data read failed
            static FILE *status_fp = NULL;

            if(!status_fp) 
            {
                //delay for 100 msec before setting status so the data processed
                sns_ddf_delay(100000); 
                status_fp = fopen(QDSP_PLAYBACK_STATUS_FILE, "r+");
                rewind(status_fp);
                fprintf(status_fp, "%d", 1);
                fclose(status_fp);
            }
            return;
        }
        //notify the sample with latest timestamp
        smgr_handle = state->smgr_handle[sensor];

        state->sensor_data.status = state->data_cache[sensor].status;
        state->sensor_data.sensor = sensor;
        state->sensor_data.num_samples = state->data_cache[sensor].num_samples;
        state->sensor_data.timestamp = state->data_cache[sensor].timestamp;
        
        if(state->fifo_supported[sensor])
        {
           state->sensor_data.end_timestamp = state->data_cache[sensor].end_timestamp;
        }
        for(j = 0; j < state->sensor_data.num_samples; j++)
        {
          state->sensor_data.samples[j].sample = 
                                   state->data_cache[sensor].samples[j].sample;
          state->sensor_data.samples[j].status = SNS_DDF_SUCCESS;
        }

        sns_ddf_smgr_notify_data(smgr_handle, &state->sensor_data, 1);
        sns_dd_qdsp_playback_log_data(state, sensor);

        /* Set the sample status to EINVALID as this is notified and stale */
        state->data_cache[sensor].status = SNS_DDF_EINVALID_DATA;
     }
}



/** 
 * @brief Parses the config file and opens the input files for reading sensor samples. 
 */
sns_ddf_status_e sns_dd_qdsp_playback_config_input(sns_dd_qdsp_playback_s *state, 
                                                   char *config_fname) 
{
    char line[256], key[60], fname[60], log_type[60];
    FILE *config_fp = NULL;
    int odr, nitems, status = SNS_DDF_EFAIL;

   config_fp = fopen(config_fname, "r");
   if (config_fp == NULL) {
      return status;
   }

   while (fgets(line, sizeof(line), config_fp) != NULL)
   {
      /* skip comments */
      if (strstr(line, "//"))
         continue;

      nitems = sscanf(line, "%s %s %d %s", key, fname, &odr, log_type);


      if (nitems == 2)
      { 
         if (!strcmp(key, "INPUT_ACCEL_MD"))
         {
            state->input_md_fd = fopen(fname, "r");
            if (state->input_md_fd == NULL) {
      		    return SNS_DDF_EFAIL;
            }
         }
      }
      else if (nitems >= 3)
      { 
        sns_ddf_sensor_e sensor = SNS_DDF_SENSOR__NONE;
	    
         if (!strcmp(key, "INPUT_ACCEL"))
         {
	        sensor = SNS_DDF_SENSOR_ACCEL;	 
            state->data_cache[sensor].num_samples = 3;
         }
         else if (!strcmp(key, "INPUT_MAG"))
         {
	        sensor = SNS_DDF_SENSOR_MAG;	 
            state->data_cache[sensor].num_samples = 3;
         }
         else if (!strcmp(key, "INPUT_GYRO"))
         {
	        sensor = SNS_DDF_SENSOR_GYRO;	 
            state->data_cache[sensor].num_samples = 3;
         }
         else if (!strcmp(key, "INPUT_PRESSURE"))
         {
	        sensor = SNS_DDF_SENSOR_PRESSURE;	 
            state->data_cache[sensor].num_samples = 1;
         }

	    if(sensor != SNS_DDF_SENSOR__NONE) 
	    {
            if(!strcmp(log_type, "FIFO") || !strcmp(log_type, "fifo"))
            {
                state->fifo_supported[sensor] = TRUE;
                qdsp_playback_num_fifo_sensors++;
                strcpy(log_type,"not-fifo");
            }
	        state->supported_odr[sensor] = odr;
            state->input_fd[sensor] = fopen(fname, "r");
            if (state->input_fd[sensor] != NULL) 
            {
      		    status = SNS_DDF_SUCCESS;
            } else 
            { 
                return SNS_DDF_EFAIL;
            }
	    }
      }
   }

   if(config_fp) {
       fclose(config_fp);
   }
   return status;
}



/** 
 * @brief Initializes the driver and sets up devices. 
 * @see sns_ddf_driver_if_s.init()
 */
sns_ddf_status_e sns_dd_qdsp_playback_init(
    sns_ddf_handle_t*       dd_handle_ptr,
    sns_ddf_handle_t        smgr_handle,
    sns_ddf_nv_params_s*    nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t                num_devices,
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_e*       sensors[],
    uint32_t*               num_sensors)
{
    sns_ddf_status_e status;
    sns_dd_qdsp_playback_s* state;

    

    
    /* NOTE: Get the sensor type from input *sensors after SMGR changes 
     * sensor_type = (*sensors)[0];
     * if(sensor_type > PLAYBACK_NUM_SENSORS)
     * return SNS_DDF_EINVALID_PARAM; 
     */
    
    sns_ddf_sensor_e sensor_type = sensors_instance[num_instances];
    
    
    // Suppress compiler warning.
    (void)sns_dd_qdsp_playback_sensors;

    if(dd_handle_ptr == NULL || num_sensors == NULL || sensors == NULL)
        return SNS_DDF_EINVALID_PARAM;
   
    if(qdsp_playback_singleton == NULL)
    {
        // Driver being initialized for the first time.
        status = sns_ddf_malloc((void**)&state, sizeof(sns_dd_qdsp_playback_s));
        if(status != SNS_DDF_SUCCESS)
            return status;

        qdsp_playback_singleton = state;

        status = sns_dd_qdsp_playback_config_input(state, QDSP_PLAYBACK_CONFIG_FILE);
        if(status != SNS_DDF_SUCCESS)
            return status;

        status = sns_ddf_malloc((void**)&(state->sensor_data.samples),
                sizeof(sns_ddf_sensor_sample_s)*(QDSP_MAX_NUM_SAMPLES)*(QDSP_PLAYBACK_NUM_INERTIAL_AXES));
        if(status != SNS_DDF_SUCCESS)
            return status;

        status = sns_ddf_malloc((void**)&(state->data_cache[sensor_type].samples),
            sizeof(sns_ddf_sensor_sample_s)*(QDSP_MAX_NUM_SAMPLES)*(QDSP_PLAYBACK_NUM_INERTIAL_AXES+1));
        if(status != SNS_DDF_SUCCESS)
            return status;

        state->data_cache[sensor_type].sensor= sensor_type;
        state->data_cache[sensor_type].status= SNS_DDF_EINVALID_DATA;
        state->cur_odr[sensor_type] = 0;
        state->reqed_odr[sensor_type] = 0;
        sns_ddf_axes_map_init(
        &state->axes_map[sensor_type], (nv_params != NULL) ? nv_params->data : NULL);

        state->smgr_handle[sensor_type] = smgr_handle;

        *sensors = &sensors_instance[num_instances];
        *dd_handle_ptr = &sensors_instance[num_instances];
        *num_sensors = 1;
	    num_instances++;
        return SNS_DDF_SUCCESS;
    } 
    else 
    {
        *dd_handle_ptr = &sensors_instance[num_instances];
        state = qdsp_playback_singleton;

        sns_ddf_axes_map_init(
            &state->axes_map[sensor_type], (nv_params != NULL) ? nv_params->data : NULL);

        status = sns_ddf_malloc((void**)&(state->data_cache[sensor_type].samples),
             sizeof(sns_ddf_sensor_sample_s)*(QDSP_MAX_NUM_SAMPLES)*(QDSP_PLAYBACK_NUM_INERTIAL_AXES+1));
        if(status != SNS_DDF_SUCCESS)
            return status;

        state->data_cache[sensor_type].sensor= sensor_type;
        state->data_cache[sensor_type].status = SNS_DDF_EINVALID_DATA;
        state->cur_odr[sensor_type] = 0;
        state->reqed_odr[sensor_type] = 0;

        state->smgr_handle[sensor_type] = smgr_handle; 
        *sensors = &sensors_instance[num_instances];
        *num_sensors = 1;
	    num_instances++;
        return SNS_DDF_SUCCESS;
    }
}



// Playback driver API
sns_ddf_driver_if_s sns_dd_qdsp_playback_if =
{
    .init = &sns_dd_qdsp_playback_init,
    .get_data = &sns_dd_qdsp_playback_get_data,
    .set_attrib = &sns_dd_qdsp_playback_set_attr,
    .get_attrib = &sns_dd_qdsp_playback_get_attr,
    .handle_timer = NULL,
    .handle_irq = &sns_dd_qdsp_playback_handle_irq,
    .reset = &sns_dd_qdsp_playback_reset,
    .run_test = NULL,
    .enable_sched_data = &sns_dd_qdsp_playback_enable_sched_data,
    .probe = NULL
};
