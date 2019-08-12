/*******************************************************************************
 * Copyright (c) 2013, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the Sensirion AG nor the
 *        names of its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

/*******************************************************************************
  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
*******************************************************************************/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     -----------------------------------------------------------
06-10-2015   AR       Added Driver specific registry
07-22-2014   MW      Eliminated compiler warning
05-30-2014   MW      Added SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST
04-18-2014   MW      Fixed compiler warning
04-18-2014   DC      Change casting in set attribute for ODR to enum type instead of uint32
04-03-2014   MW      Updated temp sensor type 
01-28-2014   MW      Use enable_sched_data for on-change functionality
01-21-2014   MW      Ensure first data notification when sensor is enabled
01-15-2014   MW      Add on-change data notify behavior CR 581550
12-03-2013   MW      Attribute value and data log packet fixes for CR 582067
07-08-2013   MW      Added log function, added SNS_DDF_ATTRIB_RANGE case in get_attrib()
07-05-2013	 --		   Original version from Sensirion
==============================================================================*/

/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_dd_rht_shtc1.h"



extern sns_ddf_driver_if_s sns_rht_shtc1_driver_fn_list;

extern bool sns_dd_rht_shtc1_check_crc(
    uint8_t *data, 
    uint8_t data_length, 
    uint8_t checksum);


/*============================================================================
                            FUNCTION PROTOTYPES

==============================================================================*/
sns_ddf_status_e sns_dd_rht_shtc1_init(
    sns_ddf_handle_t* dd_handle_ptr,
    sns_ddf_handle_t smgr_handle,
    sns_ddf_nv_params_s* nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t num_devices,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_e* sensors[],
    uint32_t* num_sensors);

sns_ddf_status_e sns_dd_rht_shtc1_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems);

sns_ddf_status_e sns_dd_rht_shtc1_set_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void* value);

sns_ddf_status_e sns_dd_rht_shtc1_reset(
    sns_ddf_handle_t dd_handle);

sns_ddf_status_e sns_dd_rht_shtc1_probe(
    sns_ddf_device_access_s* device_info,
    sns_ddf_memhandler_s* memhandler,
    uint32_t* num_sensors,
    sns_ddf_sensor_e** sensors );

sns_ddf_status_e sns_dd_rht_shtc1_run_test(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_test_e test,
    uint32_t* err);


/*============================================================================
                            STRUCTURES

==============================================================================*/
/* List of sensors supported by this driver */
static sns_ddf_sensor_e shtc1_sensors[] = {
    SNS_DDF_SENSOR_HUMIDITY,
    SNS_DDF_SENSOR_AMBIENT_TEMP
};

static sns_ddf_odr_t odr_list[] = {

    SHTC1_MAX_READ_FREQ_HPM_HZ
};

static uint32_t odr_list_size = sizeof(odr_list)/sizeof(odr_list[0]);

/*============================================================================
                            FUNCTION DEFINITIONS

==============================================================================*/
/**
 * @brief Initializes the shtc1 temperature and humidity sensor
 *
 * @detail
 *   Allocates memory for driver state structure.
 *   Opens the device port by calling sns_ddf_open_port routine
 *   Calls sns_dd_rht_shtc1_reset routine
 *
 * @param[out] dd_handle_ptr  Pointer that this function must malloc and
 *                            populate. This is a handle to the driver
 *                            instance that will be passed in to all
 *                            other functions.
 * @param[in]  smgr_handle    Handle used to identify this driver when it
 *                            calls into Sensors Manager functions.
 * @param[in]  nv_params      NV parameters retrieved for the driver.
 * @param[in]  device_info    Information describing each of the physical
 *                            devices controlled by this driver. This is
 *                            used to configure the bus and talk to the
 *                            device.
 * @param[in]  memhandler     Memory handler used to dynamically allocate
 *                            output parameters, if applicable. NB: Do
 *                            not use memhandler to allocate memory for
 *                            \a dd_handle_ptr.
 * @param[in]  num_devices    Length of \a device_info.
 * @param[out] sensors        List of supported sensors, populated and
 *                            returned by this function.
 * @param[out] num_sensors    Length of \a sensors.
 *
 * @return
 *   The error code definition within the DDF
 *   SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 */
sns_ddf_status_e sns_dd_rht_shtc1_init(
    sns_ddf_handle_t* dd_handle_ptr,
    sns_ddf_handle_t smgr_handle,
    sns_ddf_nv_params_s* nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t num_devices,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_e* sensors[],
    uint32_t* num_sensors)
{
  sns_ddf_status_e status;
  sns_dd_rht_shtc1_state_t* ptr;
  
  // sanity check: zero-pointers? other than one physical controlled 
  // device expected? 
  if((dd_handle_ptr == NULL) || (sensors == NULL) || (num_sensors == NULL))
  {
     return SNS_DDF_EINVALID_PARAM;
  }
  
  SHTC1_MSG_0(MEDIUM,"Initializing");
  
  // Allocate a driver instance.
  status = sns_ddf_malloc( (void**)&ptr, sizeof(sns_dd_rht_shtc1_state_t));
  if(status != SNS_DDF_SUCCESS)
  {
    return SNS_DDF_ENOMEM;
  }
  
  ptr->smgr_hndl = smgr_handle;
  // Open communication port to the device.
  status = sns_ddf_open_port( &ptr->port_handle, &device_info->port_config);
  if(status != SNS_DDF_SUCCESS)
  {
    sns_ddf_mfree(ptr);
    return status;
  }

  // Save the Device_Select Info for use in Set/Get Attrib for Driver Name, Etc...
  ptr->device_selection = device_info->device_select;
  
  // init timer
  status = sns_ddf_timer_init( 
    &ptr->timer_obj, 
    ptr->smgr_hndl, 
    &sns_rht_shtc1_driver_fn_list, 
    ptr, //always return dd_handle_ptr
    false);
  if(status != SNS_DDF_SUCCESS)
  {
    sns_ddf_close_port(ptr->port_handle);
    sns_ddf_mfree(ptr);
    return status;
  }

  /* Initialize Registry Items */
  if (!(nv_params) || (nv_params->data_len != sizeof(sns_dd_rht_shtc1_nv_t)))
  {
     /* Setting default values */
	 ptr->nv_source.ambient_temp_bias  = 0;
     ptr->nv_source.ambient_temp_scale = 100;
     ptr->nv_source.ambient_temp_ds    = 0;
     ptr->nv_source.humidity_bias      = 0;
     ptr->nv_source.humidity_scale     = 100;
     ptr->nv_source.humidity_ds        = 0;
  }
  else
  {
     sns_dd_rht_shtc1_nv_t      *nv_source = (sns_dd_rht_shtc1_nv_t *)nv_params->data;
   	 ptr->nv_source.ambient_temp_bias  = nv_source->ambient_temp_bias;
     ptr->nv_source.ambient_temp_scale = nv_source->ambient_temp_scale;
     ptr->nv_source.ambient_temp_ds    = nv_source->ambient_temp_ds;
     ptr->nv_source.humidity_bias      = nv_source->humidity_bias;
     ptr->nv_source.humidity_scale     = nv_source->humidity_scale;
     ptr->nv_source.humidity_ds        = nv_source->humidity_ds;
  }	

  // Fill out output parameters.
  *num_sensors = SHTC1_MAX_NUM_SENSORS;
  *sensors = shtc1_sensors;
  *dd_handle_ptr = (sns_ddf_handle_t)ptr;

  status = sns_dd_rht_shtc1_reset(ptr); // Resets the shtc1
  return status;
}


/**
 * @brief Called by the SMGR to retrieves the value of an attribute of
 *   the sensor.
 *
 * @detail
 *   range and resolution info is from the device data sheet.
 *
 * @param[in] dd_handle   Handle to a driver instance.
 * @param[in] sensor      Sensor whose attribute is to be retrieved.
 * @param[in] attrib      Attribute to be retrieved.
 * @param[in] memhandler  Memory handler used to dynamically allocate
 *                        output parameters, if applicable.
 * @param[out] value      Pointer that this function will allocate or set
 *                        to the attribute's value.
 * @param[out] num_elems  Number of elements in \a value.
 *
 * @return
 *   The error code definition within the DDF
 *   SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 */
sns_ddf_status_e sns_dd_rht_shtc1_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems)
{
  sns_dd_rht_shtc1_state_t *state = (sns_dd_rht_shtc1_state_t *)dd_handle;
  
  // sanity check: zero-pointers? valid sensor type? 
  if((dd_handle == 0) || (memhandler == 0))
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if((sensor != SNS_DDF_SENSOR_HUMIDITY) && (sensor != SNS_DDF_SENSOR_AMBIENT_TEMP))
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  SHTC1_MSG_2(HIGH,"Get Attrib sensor=%d attrib=%d ", sensor, attrib);
  
  switch(attrib)
  {
    case SNS_DDF_ATTRIB_POWER_INFO:
    {
      sns_ddf_power_info_s* power = sns_ddf_memhandler_malloc(
      memhandler, sizeof(sns_ddf_power_info_s));
      if(power == 0)
      {
        return SNS_DDF_ENOMEM;
      }

      power->active_current = SHTC1_CURRENT_MEASURING_UA;
      power->lowpower_current = SHTC1_CURRENT_IDLE_UA;
      *value = power;
      *num_elems = 1;

      return SNS_DDF_SUCCESS;
    }
    case SNS_DDF_ATTRIB_DELAYS:
    {
      sns_ddf_delays_s* delays = sns_ddf_memhandler_malloc(
      memhandler, sizeof(sns_ddf_delays_s));
      if(delays == 0)
      {
        return SNS_DDF_ENOMEM;
      }

      delays->time_to_active = SHTC1_TIME_TO_ACTIVE_US;
      if(state->mode_use_LPM)
      {
        delays->time_to_data = SHTC1_MEAS_DUR_LPM_US;
      }
      else
      {
        delays->time_to_data = SHTC1_MEAS_DUR_HPM_US;
      }

      *value = delays;
      *num_elems = 1;
      return SNS_DDF_SUCCESS;
    }
    case SNS_DDF_ATTRIB_RESOLUTION_ADC: 
    {
      sns_ddf_resolution_adc_s* res_adc = sns_ddf_memhandler_malloc(
      memhandler, sizeof(sns_ddf_resolution_adc_s));
      if(res_adc == 0)
      {
        return SNS_DDF_ENOMEM;
      }

      switch (sensor)
      {
        case SNS_DDF_SENSOR_AMBIENT_TEMP:
            if(state->mode_use_LPM)
            {
              res_adc->bit_len = SHTC1_ADC_RES_TEMP_LPM;
              res_adc->max_freq = SHTC1_MAX_READ_FREQ_LPM_HZ;
            }
            else
            {
              res_adc->bit_len = SHTC1_ADC_RES_TEMP_HPM;
              res_adc->max_freq = SHTC1_MAX_READ_FREQ_HPM_HZ;
            }
    
            break;
        case SNS_DDF_SENSOR_HUMIDITY:
            if(state->mode_use_LPM)
            {
              res_adc->bit_len = SHTC1_ADC_RES_HUMI_LPM;
              res_adc->max_freq = SHTC1_MAX_READ_FREQ_LPM_HZ;
            }
            else
            {
              res_adc->bit_len = SHTC1_ADC_RES_HUMI_HPM;
              res_adc->max_freq = SHTC1_MAX_READ_FREQ_HPM_HZ;
            }

            break;
        default:
          return SNS_DDF_EINVALID_PARAM;
      }

      *value = res_adc;
      *num_elems = 1;
      return SNS_DDF_SUCCESS;
    }
    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t* res = sns_ddf_memhandler_malloc(
      memhandler ,sizeof(sns_ddf_resolution_t));
      if(res == 0)
      {
        return SNS_DDF_ENOMEM;
      }

      switch (sensor)
      {
        case SNS_DDF_SENSOR_AMBIENT_TEMP:
          *res = SHTC1_RES_TEMP_Q16;
          break;
        case SNS_DDF_SENSOR_HUMIDITY:
          *res = SHTC1_RES_HUMI_Q16;
          break;
        default:
          return SNS_DDF_EINVALID_PARAM;        
      }

      *value = res;
      *num_elems = 1;
      return SNS_DDF_SUCCESS;
    }

	case SNS_DDF_ATTRIB_RANGE:
	{
		sns_ddf_range_s *device_ranges;
        device_ranges = sns_ddf_memhandler_malloc(memhandler,
                                       sizeof(sns_ddf_range_s));
        if (device_ranges == NULL)
        {
			return SNS_DDF_ENOMEM;
        }
        
        if (sensor == SNS_DDF_SENSOR_HUMIDITY)
        {
            device_ranges->min = FX_FLTTOFIX_Q16(0);		// 0%
            device_ranges->max = FX_FLTTOFIX_Q16(100.0);	// 100%
        }

        if (sensor == SNS_DDF_SENSOR_AMBIENT_TEMP)
        {
            device_ranges->min = FX_FLTTOFIX_Q16(-30.0); // -30 deg C
            device_ranges->max = FX_FLTTOFIX_Q16(100.0); // 100 deg C
        }

        *value = device_ranges;
		*num_elems = 1;

		return SNS_DDF_SUCCESS;
	}
    case SNS_DDF_ATTRIB_DRIVER_INFO:
    {
      sns_ddf_driver_info_s *info = sns_ddf_memhandler_malloc(
      memhandler,sizeof(sns_ddf_driver_info_s));
      if(info == 0)
      {
        return SNS_DDF_ENOMEM;
      }

      switch (state->device_selection)
      {
        case 0:
            info->name = "Sensirion SHTC1 driver";
            break;
        case 1:
            info->name = "Sensirion SHTW1 driver";
            break;
        default:
            info->name = "Sensirion SHTC1 driver";
            break;
      }
      info->version = 1;
      *value = info;
      *num_elems = 1;
      return SNS_DDF_SUCCESS;
    }
    case SNS_DDF_ATTRIB_DEVICE_INFO:
    {
      sns_ddf_device_info_s *info = sns_ddf_memhandler_malloc(
      memhandler, sizeof(sns_ddf_device_info_s));
      if(info == 0)
      {
        return SNS_DDF_ENOMEM;
      }

      switch (sensor)
      {
        case SNS_DDF_SENSOR_AMBIENT_TEMP:
            info->name = "Temperature";
            break;
        case SNS_DDF_SENSOR_HUMIDITY:
            info->name = "Humidity";
            break;
        default:
            return SNS_DDF_EINVALID_PARAM;
      }

      info->vendor = "Sensirion";
      switch (state->device_selection)
      {
        case 0:
            info->model = "SHTC1";
            break;
        case 1:
            info->model = "SHTW1";
            break;
        default:
            info->model = "SHTC1";
            break;
      }
      info->version = SHTC1_DEVICE_VERSION;
      *value = info;
      *num_elems = 1;
      return SNS_DDF_SUCCESS;
    }

    case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
    {

      *value = &odr_list;
      *num_elems = odr_list_size;

      return SNS_DDF_SUCCESS;

    }

    case SNS_DDF_ATTRIB_ODR:
    {
      uint32_t* odr_ptr;

      if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;

      if (sensor == SNS_DDF_SENSOR_HUMIDITY)
      {
          *odr_ptr = state->humidity_odr;
      }

      if (sensor == SNS_DDF_SENSOR_AMBIENT_TEMP)
      {
          *odr_ptr = state->temp_odr;
      }

      *(uint32_t **)value = odr_ptr;
      return SNS_DDF_SUCCESS;
    }
    default:
      return SNS_DDF_EINVALID_ATTR;
  }
}


/**
 * @brief Sets an attribute of the humidity and temperature sensor
 *
 * @detail
 *   Called by SMGR to set certain device attributes that are
 *   programmable. Curently its the mode.
 *
 * @param[in] dd_handle  Handle to a driver instance.
 * @param[in] sensor     Sensor for which this attribute is to be set.
 * @param[in] attrib     Attribute to be set.
 * @param[in] value      Value to set this attribute.
 *
 * @return
 *   The error code definition within the DDF
 *   SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
 *   SNS_DDF_EINVALID_PARAM
 */
sns_ddf_status_e sns_dd_rht_shtc1_set_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void* value)
{
  /*uint32_t attrib_val = *(uint32_t *)value;*/
  sns_dd_rht_shtc1_state_t* state = (sns_dd_rht_shtc1_state_t *)dd_handle;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  // sanity check: zero-pointers? valid sensor type? 
  if(dd_handle == 0)
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if((sensor != SNS_DDF_SENSOR_HUMIDITY) \
     && (sensor != SNS_DDF_SENSOR_AMBIENT_TEMP) \
     && (sensor != SNS_DDF_SENSOR__ALL))
  {
    return SNS_DDF_EINVALID_PARAM;
  }
  
  /*SHTC1_MSG_3(HIGH,"Set Attrib sensor=%d attrib=%d value=%d", sensor, attrib, attrib_val);*/

  switch(attrib)
  {
	case SNS_DDF_ATTRIB_POWER_STATE:
		/* Device does not support different power states. */
		if((sensor == SNS_DDF_SENSOR__ALL) && (*(sns_ddf_powerstate_e *)value != SNS_DDF_POWERSTATE_ACTIVE))
		{
		  status = sns_dd_rht_shtc1_reset(dd_handle);
		  if(status != SNS_DDF_SUCCESS)
		  {
		    //Do nothing. This is to eliminate compiler warning.
		  }
		}
		break;
	case SNS_DDF_ATTRIB_RANGE:
		/* Device does not support different ranges of operation. */
	
	case SNS_DDF_ATTRIB_RESOLUTION:
		/* Device resolution cannot be changed. */
		break;
  case SNS_DDF_ATTRIB_ODR:
    if (sensor == SNS_DDF_SENSOR_HUMIDITY)
    {
        state->humidity_odr = (uint32_t)(*(sns_ddf_odr_t *)value); 
    }

    if (sensor == SNS_DDF_SENSOR_AMBIENT_TEMP)
    {
        state->temp_odr = (uint32_t)(*(sns_ddf_odr_t *)value); 
    }
    break;

	default:
		return SNS_DDF_EINVALID_ATTR;
  }

  return SNS_DDF_SUCCESS;
}


/**
 * @brief Resets the driver and device so they return to the state they were
 *        in after init() was called.
 *
 * @detail
 *   - resets the mode to High Precision mode which is stored in the driver
 *     state structure.
 *   - Performs a soft reset on the sensor
 *
 * @param[in] dd_handle   Handle to a driver instance.
 *
 * @return
 *   The error code definition within the DDF
 *   SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 */
sns_ddf_status_e sns_dd_rht_shtc1_reset(sns_ddf_handle_t dd_handle)
{  
  sns_dd_rht_shtc1_state_t* state = (sns_dd_rht_shtc1_state_t *)dd_handle;
  sns_ddf_status_e status;
  uint8_t write_buffer_reset[SHTC1_CMD_SIZE_BYTES] = SHTC1_CMD_SOFT_RESET;
  uint8_t bytes_written;
  
   // sanity check: zero-pointer?
  if(dd_handle == 0)
  {
    return SNS_DDF_EINVALID_PARAM;
  }
  
  SHTC1_MSG_0(HIGH,"Reset");
  
  // Reset driver state.
  state->mode_use_LPM = false;
  state->measurement_running = false;
  state->enable_humidity = false;
  state->enable_temp = false;
  state->prev_humidity_value = 0;
  state->prev_temp_value = 0;
  state->humidity_odr = 0;
  state->temp_odr = 0;

  // stop timer if active
  status = sns_ddf_timer_cancel(state->timer_obj);

  // wait 12 ms for possible running measurement to finish.
  sns_ddf_delay(12000);
  // Reset the device.	
  status = sns_ddf_write_port(
      state->port_handle,
      write_buffer_reset[0],
      &write_buffer_reset[1],
      1,
      &bytes_written);
  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }

  // wait 1 ms for power up.
  sns_ddf_delay(1000);

  return status;
}


/**
 * @brief Probing the device with the provided driver configuration
 *
 * @detail
 *   - When this function gets called, a communication port is opened and
 *     SNS_DDF_SUCCESS is returned when a device is present.
 *   - Communication port is closed before returning
 *
 * @param[in]  device_info    Information describing the physical
 *                            device controlled by this driver. This is
 *                            used to configure the bus and talk to the
 *                            device.
 * @param[in] memhandler      Memory handler used to dynamically allocate
 *                            output parameters, if applicable.
 * @param[out] num_sensors    Length of \a sensors.
 * @param[out] sensors        List of supported sensors, populated and
 *                            returned by this function.
 *
 * @return
 *   The error code definition within the DDF
 *   SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 */
sns_ddf_status_e sns_dd_rht_shtc1_probe(
    sns_ddf_device_access_s* device_info,
    sns_ddf_memhandler_s* memhandler,
    uint32_t* num_sensors,
    sns_ddf_sensor_e** sensors )
{
  sns_ddf_status_e status;
  sns_ddf_handle_t port_handle;
  uint8_t write_buffer_read_id[SHTC1_CMD_SIZE_BYTES] = SHTC1_CMD_READ_ID_REG;
  uint8_t read_buffer[3] = {0,0,0};
  uint8_t bytes_read;
  uint8_t bytes_written;
  
  // sanity check: zero-pointers?
  if((device_info == 0) || (memhandler == 0))
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  // open communication port
  status = sns_ddf_open_port( &port_handle, &device_info->port_config);
  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }

  // probe SHTC1
  status = sns_ddf_write_port(
      port_handle,
      write_buffer_read_id[0],
      &write_buffer_read_id[1],
      1,
      &bytes_written);

  if(status != SNS_DDF_SUCCESS)
  {
    // close communication port
    sns_ddf_close_port(port_handle);
    return status;
  }

  status = sns_ddf_read_port(
      port_handle,
      write_buffer_read_id[0],
      &read_buffer[0],
      3,
      &bytes_read);

  // close communication port
  status = sns_ddf_close_port(port_handle);
  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }

  // check CRC & ID register content
  if (!sns_dd_rht_shtc1_check_crc(read_buffer, 2, read_buffer[2]))
  {
    return SNS_DDF_EDEVICE;
  }

  if ((read_buffer[1] & SHTC1_ID_REG_MASK) != SHTC1_ID_REG_CONTENT)
  {
    return SNS_DDF_EDEVICE;
  }
  
  // probing successful -> write out parameters
  *num_sensors = SHTC1_MAX_NUM_SENSORS;
  *sensors = shtc1_sensors;  
  return status;
}

/**
 * @brief Called to verify the basic functionality
 *
 * @detail
 *   Only SNS_DDF_TEST_CONNECTIVITY implemented. Performs a read ID Register
 *   to verify the connectivity of the device.
 *
 * @param[in]  dd_handle    Handle to a driver instance.
 * @param[in] sensor        Sensor for which testing is requested. Can be
 *                          Humidity, Temperature or All
 * @param[in] test          Defines what test has to be done. Only
 *                          SNS_DDF_TEST_CONNECTIVITY supported.
 * @param[out] err          Output Error Code enumerated
 *                          0x00000000: no error
 *                          0x00000001: error while writing to device
 *                          0x00000002: error while reading from device
 *                          0x00000003: CRC mismatch
 *                          0x00000004: ID register content mismatch
 * @return
 *   The error code definition within the DDF
 *   SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 */
sns_ddf_status_e sns_dd_rht_shtc1_run_test(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_test_e test,
    uint32_t* err)
{ 
  sns_ddf_status_e status;
  uint8_t write_buffer_read_id[SHTC1_CMD_SIZE_BYTES] = SHTC1_CMD_READ_ID_REG;
  uint8_t read_buffer[3] = {0,0,0};
  uint8_t bytes_read;
  uint8_t bytes_written;
  sns_dd_rht_shtc1_state_t* state = (sns_dd_rht_shtc1_state_t *)dd_handle;
    
  // sanity check: zero-pointers? Valid sensor type?
  if(dd_handle == 0)
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if((sensor != SNS_DDF_SENSOR_HUMIDITY) \
     && (sensor != SNS_DDF_SENSOR_AMBIENT_TEMP) \
     && (sensor != SNS_DDF_SENSOR__ALL))
  {
    return SNS_DDF_EINVALID_PARAM;
  }
  
  //reset err out parameter
  *err = 0;
  
  //check if measurement running
  if(state->measurement_running == true)
  {
    return SNS_DDF_EDEVICE_BUSY;
  }
  
  SHTC1_MSG_0(MEDIUM,"Run Test");
  
  //execute test
  switch (test)
  {
	case SNS_DDF_TEST_SELF:
	case SNS_DDF_TEST_CONNECTIVITY:
	  // probe SHTC1
      status = sns_ddf_write_port(
          state->port_handle,
          write_buffer_read_id[0],
          &write_buffer_read_id[1],
          1,
          &bytes_written);
      if(status != SNS_DDF_SUCCESS)
      {
        *err = 0x00000001;
        return status;
      }

      status = sns_ddf_read_port(
          state->port_handle,
          write_buffer_read_id[0],
          &read_buffer[0],
          3,
          &bytes_read);
      if(status != SNS_DDF_SUCCESS)
      {
        *err = 0x00000002;
        return status;
      }

      // check CRC & ID register content
      if (!sns_dd_rht_shtc1_check_crc(read_buffer, 2, read_buffer[2]))
      {
        *err = 0x00000003;
        return SNS_DDF_EDEVICE;
      }

      if ((read_buffer[1] & SHTC1_ID_REG_MASK) != SHTC1_ID_REG_CONTENT)
      {
        *err = 0x00000004;
        return SNS_DDF_EDEVICE;
      }

      return SNS_DDF_SUCCESS;

    default:
      return SNS_DDF_EINVALID_TEST;
  }
}


