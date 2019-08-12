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
  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
*******************************************************************************/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     -----------------------------------------------------------
06-10-2015   AR      Added Driver specific registry
11-29-2014   PS      Support for virtio bringup
07-20-2014   VY      Disabled logging in uImage
06-11-2014   vy      Refactored for uImage
05-22-2014   MW      Fixed temp sensor type
01-28-2014   MW      Use enable_sched_data for on-change functionality
01-21-2014   MW      Ensure first data notification when sensor is enabled
01-15-2014   MW      Add on-change data notify behavior CR 581550
12-03-2013   MW      Attribute value and data log packet fixes for CR 582067
07-08-2013   MW      Added log function, added SNS_DDF_ATTRIB_RANGE case in get_attrib()
07-05-2013    --     Original version from Sensirion
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


extern sns_ddf_status_e sns_dd_rht_shtc1_init(
    sns_ddf_handle_t* dd_handle_ptr,
    sns_ddf_handle_t smgr_handle,
    sns_ddf_nv_params_s* nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t num_devices,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_e* sensors[],
    uint32_t* num_sensors);

extern sns_ddf_status_e sns_dd_rht_shtc1_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems);

extern sns_ddf_status_e sns_dd_rht_shtc1_set_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void* value);

extern sns_ddf_status_e sns_dd_rht_shtc1_reset(
    sns_ddf_handle_t dd_handle);

extern sns_ddf_status_e sns_dd_rht_shtc1_probe(
    sns_ddf_device_access_s* device_info,
    sns_ddf_memhandler_s* memhandler,
    uint32_t* num_sensors,
    sns_ddf_sensor_e** sensors );

extern sns_ddf_status_e sns_dd_rht_shtc1_run_test(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_test_e test,
    uint32_t* err);



/*============================================================================
                            FUNCTION PROTOTYPES

==============================================================================*/
static sns_ddf_status_e sns_dd_rht_shtc1_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_data_s** data);

static void sns_dd_rht_shtc1_handle_timer(
    sns_ddf_handle_t dd_handle, 
    void* arg);

bool sns_dd_rht_shtc1_check_crc(
    uint8_t *data, 
    uint8_t data_length, 
    uint8_t checksum);

static sns_ddf_status_e sns_dd_rht_shtc1_enable_sched_data(
    sns_ddf_handle_t handle,
    sns_ddf_sensor_e sensor,
    bool enable);

static sns_ddf_status_e sns_dd_rht_shtc1_start_measurement(sns_ddf_handle_t dd_handle);

void sns_dd_rht_shtc1_log_sensor_data(
   sns_dd_rht_shtc1_state_t* dd_ptr,
   sns_ddf_sensor_e       sensor_type,
   uint32_t               raw_data,
   uint32_t               data1_q16 );


/*============================================================================
                            STRUCTURES
==============================================================================*/
sns_ddf_driver_if_s sns_rht_shtc1_driver_fn_list =
{
  .init = &sns_dd_rht_shtc1_init,
  .get_data = &sns_dd_rht_shtc1_get_data,
  .set_attrib = &sns_dd_rht_shtc1_set_attr,
  .get_attrib = &sns_dd_rht_shtc1_get_attr,
  .handle_timer = &sns_dd_rht_shtc1_handle_timer,
  .handle_irq = 0, /**< handle irq */
  .reset = &sns_dd_rht_shtc1_reset,
  .run_test = &sns_dd_rht_shtc1_run_test,
  .enable_sched_data = &sns_dd_rht_shtc1_enable_sched_data,
  .probe = sns_dd_rht_shtc1_probe,
  0, /**< get fifo data */
};



/*===========================================================================
FUNCTION      sns_dd_rht_shtc1_log_sensor_data

DESCRIPTION   Log the latest sensor data

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None
===========================================================================*/
void sns_dd_rht_shtc1_log_sensor_data(
   sns_dd_rht_shtc1_state_t* dd_ptr,
   sns_ddf_sensor_e       sensor_type,
   uint32_t               raw_data,
   uint32_t               data1_q16 )
{
   sns_err_code_e err_code;
   sns_log_sensor_data_pkt_s* log_struct_ptr;

   if( sns_ddf_smgr_is_in_uimage() )
     return;
     
   /* Allocate log packet */
   err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                                sizeof(sns_log_sensor_data_pkt_s),
                                (void**)&log_struct_ptr);

   if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
   {
     log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
     log_struct_ptr->sensor_id = sensor_type;
     log_struct_ptr->vendor_id = SNS_DDF_VENDOR_SENSIRION;

     /* Timestamp the log with sample time */
     log_struct_ptr->timestamp = sns_ddf_get_timestamp();

     /* Log sensor data */
     if(sensor_type == SNS_DDF_SENSOR_HUMIDITY)
     {
       log_struct_ptr->num_data_types = 2;
       log_struct_ptr->data[0] = raw_data;
       log_struct_ptr->data[1] = data1_q16;
     }
     else
     {
       log_struct_ptr->num_data_types = 1;
       log_struct_ptr->data[0] = data1_q16;
     }
 
     /* Commit log (also frees up the log packet memory) */
     err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
                                  log_struct_ptr);
   }
   else
   {
     /* Do nothing. */
   }
}


static sns_ddf_status_e sns_dd_rht_shtc1_start_measurement(sns_ddf_handle_t dd_handle)
{
  uint32_t timer_value;
  uint8_t bytes_written;
  uint8_t write_buffer_start_meas_hpm[SHTC1_CMD_SIZE_BYTES] = SHTC1_CMD_MEASURE_HPM;
  uint8_t write_buffer_start_meas_lpm[SHTC1_CMD_SIZE_BYTES] = SHTC1_CMD_MEASURE_LPM;
  sns_ddf_status_e status;
  sns_dd_rht_shtc1_state_t *state =(sns_dd_rht_shtc1_state_t *)dd_handle;

  if(state->mode_use_LPM == false)
  {
    //write buffer HPM
    timer_value = SHTC1_MEAS_DUR_HPM_US;
    status = sns_ddf_write_port(
      state->port_handle,
      write_buffer_start_meas_hpm[0],
      &write_buffer_start_meas_hpm[1],
      1,
      &bytes_written);
  }
  else
  {
    //write Buffer LPM
    timer_value = SHTC1_MEAS_DUR_LPM_US;
    status = sns_ddf_write_port(
      state->port_handle,
      write_buffer_start_meas_lpm[0],
      &write_buffer_start_meas_lpm[1],
      1,
      &bytes_written);
  }

  if(status != SNS_DDF_SUCCESS)
  {
    SHTC1_MSG_1(ERROR,"sns_ddf_write_port error=%d", status);
    return status;
  }

  //start timer
  status = sns_ddf_timer_start(state->timer_obj, timer_value);
  if(status != SNS_DDF_SUCCESS)
  {
    SHTC1_MSG_1(ERROR,"sns_ddf_timer_start error=%d", status);
    //return status;
  }
  state->measurement_running = true;
  
  return status;
}

/**
 * @brief Called by the SMGR to get data
 *
 * @detail
 *   Requests a single sample of sensor data from the sensor (Async).
 *   The measurement starts immediately and when its done a notification
 *   is sent that new data is ready.
 *
 * @param[in] dd_handle     Handle to a driver instance.
 * @param[in] sensors       List of sensors for which data is requested.
 * @param[in] num_sensors   Length of \a sensors.
 * @param[in] memhandler    Memory handler used to dynamically allocate
 *                          output parameters, if applicable.
 * @param[out] data         Sampled sensor data. The number of elements
 *                          must match \a num_sensors.
 *
 * @return
 *   The error code definition within the DDF
 *   SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 */
static sns_ddf_status_e sns_dd_rht_shtc1_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_data_s** data)
{
  SHTC1_MSG_0(ERROR,"sns_dd_rht_shtc1_get_data");
  // streaming is not supported
  return SNS_DDF_EFAIL;
}


/**
 * @brief Compares Checksum with calculated CRC.
 *
 * @detail
 *   Checks if the bytes received have the same CRC as the CRC received
 *
 * @param[in] *data: Ptr to the received data
 * @param[in] data_length: length of data
 * @param[in] checksum: checksum to compare with
 *
 * @return
 *   true on success; Otherwise false
 */
bool sns_dd_rht_shtc1_check_crc(uint8_t *data, uint8_t data_length, 
                                uint8_t checksum)
{
  uint8_t crc = SHTC1_CRC_INIT;
  uint8_t current_byte;
  uint8_t bit;
  
  //calculates 8-Bit checksum with given polynomial
  for (current_byte = 0; current_byte < data_length; ++current_byte)
  {
    crc ^= (data[current_byte]);
    for ( bit = 8; bit > 0; --bit)
    {
      if (crc & 0x80)
        crc = (crc << 1) ^ SHTC1_CRC_POLYNOMIAL;
      else
        crc = (crc << 1);
    }
  }

  return crc == checksum;
}

/**
 * @brief SMRG calls this function when previously started timer expires.
 *
 * @detail
 *   - When this function gets called, the measurement is done
 *   - It is converted to qt16 format and the SMRG gets notified with
 *     the new data
 *
 * @param[in] dd_handle   Handle to a driver instance.
 * @param[in] *arg        Pointer that was sent with the start timer command.
 *
 * @return
 *   The error code definition within the DDF
 *   SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 */
static void sns_dd_rht_shtc1_handle_timer(sns_ddf_handle_t dd_handle, void* arg)
{
  sns_ddf_status_e status;
  uint8_t bytes_read = 0;
  uint8_t read_buffer[SHTC1_READ_SIZE_BYTES];
  uint8_t write_buffer_dummy[2] = {0x78,0x60}; // SMBus<->I2C workaround
  sns_dd_rht_shtc1_state_t *state = (sns_dd_rht_shtc1_state_t *)arg;
  sns_ddf_time_t timestamp = sns_ddf_get_timestamp();
  bool start_new_measurement = true;
  state->measurement_running = false;

  // sanity check: zero-pointer? no return value available!
  if(dd_handle == 0)
  {
    return;
  }

  SHTC1_MSG_0(MEDIUM,"Handle Timer");

  status = sns_ddf_read_port(
      state->port_handle,
      write_buffer_dummy[state->mode_use_LPM],
      &read_buffer[0],
      SHTC1_READ_SIZE_BYTES,
      &bytes_read);

  if(status != SNS_DDF_SUCCESS)
  {
    return;
  }
  SHTC1_MSG_2(HIGH,"Reg Item ambient_temp_scale =%d, ambient_temp_bias =%d", state->nv_source.ambient_temp_scale, state->nv_source.ambient_temp_bias);
  SHTC1_MSG_2(HIGH,"Reg Item humidity_scale     =%d, humidity_bias     =%d", state->nv_source.humidity_scale, state->nv_source.humidity_bias);
  SHTC1_MSG_2(HIGH,"Reg Item ambient_temp_ds    =%d, humidity_ds       =%d", state->nv_source.ambient_temp_ds, state->nv_source.humidity_ds);
  //save temperature to sample[0] and humidity to sample[1]
  state->sample_cache[0].status=SNS_DDF_SUCCESS;
  state->sample_cache[1].status=SNS_DDF_SUCCESS;

#ifndef VIRTIO_BRINGUP
  if (!sns_dd_rht_shtc1_check_crc(read_buffer, 2, read_buffer[2]))
  {
    state->sample_cache[0].status = SNS_DDF_EBUS; // bus error when crc fails
  }

  state->sample_cache[0].sample = SHTC1_TICKSTOTEMP_Q16((uint32_t)read_buffer[1]
                                                        + (read_buffer[0]<<8));
  if (!sns_dd_rht_shtc1_check_crc(&read_buffer[3], 2, read_buffer[5]))
  {
    state->sample_cache[1].status = SNS_DDF_EBUS; // bus error when crc fails
  }

  state->sample_cache[1].sample = SHTC1_TICKSTOHUMI_Q16((uint32_t)read_buffer[4] 
                                                        + (read_buffer[3]<<8));
#else
  state->sample_cache[0].sample = state->prev_temp_value ^ FX_FLTTOFIX_Q16(SHTC1_TEMP_CHANGE*3);         //Fake data
  state->sample_cache[1].sample = state->prev_humidity_value ^ FX_FLTTOFIX_Q16(SHTC1_HUMIDITY_CHANGE*3); //Fake data
#endif /* VIRTIO_BRINGUP */

  
   if(state->enable_temp)
   {
      float prev, cache, diff;
      prev = (float)FX_FIXTOFLT_Q16(state->prev_temp_value);
      cache = (float)FX_FIXTOFLT_Q16(state->sample_cache[0].sample);

      diff = (prev < cache)?(cache-prev):(prev-cache);

      SHTC1_MSG_2(MED,"prev diff=%u, cache=%u", prev, cache);
	  SHTC1_MSG_2(MED,"temp diff=%d, diff_q16=%d", (uint32_t)diff, FX_FLTTOFIX_Q16(diff));
      if(diff > SHTC1_TEMP_CHANGE)
      {
        state->prev_temp_value = state->sample_cache[0].sample;
        state->sample_cache[0].sample = FX_FLTTOFIX_Q16((cache * state->nv_source.ambient_temp_scale/ 100) - state->nv_source.ambient_temp_bias);
        SHTC1_MSG_2(HIGH,"temp changed prev_temp=%d, curr_temp=%d", state->prev_temp_value, state->sample_cache[0].sample);
        
        state->sensor_data[0].sensor = SNS_DDF_SENSOR_AMBIENT_TEMP;
        state->sensor_data[0].samples = &state->sample_cache[0];
        state->sensor_data[0].status = state->sample_cache[0].status;
        state->sensor_data[0].timestamp = timestamp;
        state->sensor_data[0].num_samples = 1;        

        sns_ddf_smgr_notify_data(state->smgr_hndl, state->sensor_data, 1);

        /* Log Temperature data */
        sns_dd_rht_shtc1_log_sensor_data(state, SNS_DDF_SENSOR_AMBIENT_TEMP, 
                    (uint32_t)read_buffer[1] + (read_buffer[0]<<8),
                    state->sample_cache[0].sample);

      }
    }

    if(state->enable_humidity)
    {
      float prev, cache, diff;
      prev = (float)FX_FIXTOFLT_Q16(state->prev_humidity_value);
      cache = (float)FX_FIXTOFLT_Q16(state->sample_cache[1].sample);

      diff = (prev < cache)?(cache-prev):(prev-cache);
      SHTC1_MSG_2(MED,"prev diff=%u, cache=%u", prev, cache);
      SHTC1_MSG_2(MED,"humid diff=%d, diff_q16=%d", (uint32_t)diff, FX_FLTTOFIX_Q16(diff));

      if(diff > SHTC1_HUMIDITY_CHANGE)
      {
        state->prev_humidity_value = state->sample_cache[1].sample;
        state->sample_cache[1].sample = FX_FLTTOFIX_Q16((cache * state->nv_source.humidity_scale / 100) - state->nv_source.humidity_bias);
        SHTC1_MSG_2(HIGH,"humidity changed prev_humid=%d, curr_humid=%d", state->prev_humidity_value, state->sample_cache[1].sample);
        
        state->sensor_data[0].sensor = SNS_DDF_SENSOR_HUMIDITY;
        state->sensor_data[0].samples = &state->sample_cache[1];
        state->sensor_data[0].status = state->sample_cache[1].status;
        state->sensor_data[0].timestamp = timestamp;
        state->sensor_data[0].num_samples = 1;

        sns_ddf_smgr_notify_data(state->smgr_hndl, state->sensor_data, 1);

        /* Log Humidity data */
        sns_dd_rht_shtc1_log_sensor_data(state, SNS_DDF_SENSOR_HUMIDITY, 
                    (uint32_t)read_buffer[4] + (read_buffer[3]<<8),
                    state->sample_cache[1].sample);
      }
    }

 if(start_new_measurement)
 {
    SHTC1_MSG_0(MED,"start_new_measurement");
    //start new measurement
    status = sns_dd_rht_shtc1_start_measurement((sns_ddf_handle_t)arg);

    if(status != SNS_DDF_SUCCESS)
    {
      SHTC1_MSG_1(ERROR,"sns_dd_rht_shtc1_start_measurement error=%d", status);
    }
  }
}


static sns_ddf_status_e sns_dd_rht_shtc1_enable_sched_data(
   sns_ddf_handle_t handle,
   sns_ddf_sensor_e sensor,
   bool enable )
{
  sns_ddf_status_e  status = SNS_DDF_SUCCESS;
  sns_dd_rht_shtc1_state_t *state =(sns_dd_rht_shtc1_state_t *)handle;
  
  SHTC1_MSG_2(HIGH,"sns_dd_rht_shtc1_enable_sched_data sensor=%d enable=%d", sensor, enable);
  
  // sanity check: zero-pointers? valid sensor (type and amount)? 
  if(handle == NULL)
  {
     return SNS_DDF_EINVALID_PARAM;
  }

  if((sensor != SNS_DDF_SENSOR_HUMIDITY) && (sensor != SNS_DDF_SENSOR_AMBIENT_TEMP))
  {
     return SNS_DDF_EINVALID_PARAM;
  }

  if(enable == true)
  {
    if(sensor == SNS_DDF_SENSOR_HUMIDITY)
    {
      state->enable_humidity = true;
      state->prev_humidity_value = 0;
    }
    else if(sensor == SNS_DDF_SENSOR_AMBIENT_TEMP)
    {
      state->enable_temp = true;
      state->prev_temp_value = 0;
    }   

    //return if measurement already running
    if(state->measurement_running == true)
    {
      SHTC1_MSG_0(MEDIUM,"Measurement in progress.");
      
      return SNS_DDF_SUCCESS;
    }

    //start new measurement
    status = sns_dd_rht_shtc1_start_measurement(handle);

    if(status != SNS_DDF_SUCCESS)
    {
      SHTC1_MSG_1(ERROR,"sns_dd_rht_shtc1_start_measurement error=%d", status);
      return status;
    }
  }
  else
  {
    if(sensor == SNS_DDF_SENSOR_HUMIDITY)
    {
      state->enable_humidity = false;
    }
    else if(sensor == SNS_DDF_SENSOR_AMBIENT_TEMP)
    {
      state->enable_temp = false;
    }   

    if(!state->enable_humidity && !state->enable_temp)
    {
      // stop timer if active
      status = sns_ddf_timer_cancel(state->timer_obj);
      state->measurement_running = false;
    }
  }

  return status;
}

