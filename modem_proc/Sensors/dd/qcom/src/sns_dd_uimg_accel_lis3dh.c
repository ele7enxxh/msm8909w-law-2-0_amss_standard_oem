/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
*
* File Name         : lis3dh_ddf_drv.c
* Authors           : Jianjian Huo (samuel.huo@st.com)
* Version           : V 1.0.0
* Date              : 08/29/2011
* Description       : LIS3DH Accelerometer driver source file
*
********************************************************************************
* Copyright (c) 2011, STMicroelectronics.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of the STMicroelectronics nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* REVISON HISTORY
*
* VERSION | DATE          | DESCRIPTION
*         | 06/11/2014    | Refactored for uImage
*         | 06/19/2012    | (vh) Added run time switch for I2C code
*         | 05/02/2012    | Added sns_dd_acc_lis3dh_handle_int to sns_dd_acc_lis3dh_if
*                         | use sns_dd_acc_lis3dh_disable_int return status for disabling MD status
*         | 03/05/2012    | Changed interrupt APIs
* 1.0.1   | 09/15/2011    | Removed delay in reset/set ODR/set range functions
* 1.0.0   | 08/29/2011    | Switch motion detection interrupt to INT pin 2.
*         | 08/23/2011    | Updated LPF frequency array to comply with DDF spec
*         | 08/13/2011    | Changed the unit of LOWPASS to q16_t
*         | 07/15/2011    | Added Driver level logpacket
*         | 06/17/2011    | Fixed Z-axis data conversion issue
*         | 05/25/2011    | Changed to adapt SENSOR__ALL too in addition to SENSOR_ACCEL
* 0.9.3   | 05/24/2011    | Disabled interrupt latch bit.
* 0.9.2   | 05/20/2011    | Merged bug fixes from STM and renumbered versions.
* 0.9.1   | 05/05/2011    | Used a header file to configure axis and polarity.
* 0.9.0   | 04/27/2011    | Created with motion detection support.
*******************************************************************************/
#include "sns_dd_accel_lis3dh.h"
#include "sns_dd_stm_config.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_signal.h"
//Include files required to support accel log packet
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include <string.h>
#include "sns_memmgr.h"

#define STM_LIS3DH_NUM_READ_BYTES  6
#define AUTO_INCREMENT             0x80

/*------------------------------------------------------
Extern Variables
------------------------------------------------------*/
extern volatile boolean EnableI2C;

/*------------------------------------------------------
  Access to externalized functions.
------------------------------------------------------*/

extern sns_ddf_status_e sns_dd_acc_lis3dh_init(
    sns_ddf_handle_t* dd_handle_ptr,
    sns_ddf_handle_t smgr_handle,
    sns_ddf_nv_params_s* nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t num_devices,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_e* sensors[],
    uint32_t* num_sensors);

extern sns_ddf_status_e sns_dd_acc_lis3dh_get_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s* memhandler,
    void** value,
    uint32_t* num_elems);

extern sns_ddf_status_e sns_dd_acc_lis3dh_reset(sns_ddf_handle_t dd_handle);

extern sns_ddf_status_e sns_dd_acc_lis3dh_probe(
  sns_ddf_device_access_s*  device_info,
  sns_ddf_memhandler_s*     memhandler,
  uint32_t*                 num_sensors,
  sns_ddf_sensor_e**        sensors );

extern sns_ddf_status_e sns_dd_acc_lis3dh_set_attr(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void* value);


/*------------------------------------------------------
  Function prototypes
------------------------------------------------------*/
sns_ddf_status_e sns_dd_acc_lis3dh_disable_int(
    sns_ddf_handle_t dd_handle);

static void sns_dd_acc_lis3dh_handle_int(
    sns_ddf_handle_t  dd_handle,
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp);

static void sns_dd_acc_lis3dhaccel_log_data(sns_ddf_sensor_data_s *accel_data_ptr);

static sns_ddf_status_e sns_dd_acc_lis3dh_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_data_s** data);

/*------------------------------------------------------
  Structures
------------------------------------------------------*/
/**
 * LIS3DH device driver interface.
 */
sns_ddf_driver_if_s sns_dd_acc_lis3dh_if =
{
    .init = &sns_dd_acc_lis3dh_init,
    .get_data = &sns_dd_acc_lis3dh_get_data,
    .set_attrib = &sns_dd_acc_lis3dh_set_attr,
    .get_attrib = &sns_dd_acc_lis3dh_get_attr,
    .handle_timer = NULL,
    .handle_irq = &sns_dd_acc_lis3dh_handle_int,
    .reset = &sns_dd_acc_lis3dh_reset,
    .run_test = NULL,
    .enable_sched_data = NULL,
    .probe = &sns_dd_acc_lis3dh_probe
};


/*------------------------------------------------------
  Function definitions
------------------------------------------------------*/
 /**
 * @brief Disable accelerometer to generate motion detection interrupt.
 *
 * @param[in]  dd_handle      Handle to a driver instance.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e sns_dd_acc_lis3dh_disable_int(
    sns_ddf_handle_t dd_handle)
{
    sns_dd_acc_lis3dh_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;
    
    // disable AOI1 interrupt on INT2
    rw_buffer = 0
             | (0<<6);          // I2_INT1: Interrupt generator 1 on INT2 pin.
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DH_CTRL_REG6_A,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    // disable AOI1 interrupt.
    rw_buffer  = 0x0;
    status = sns_ddf_write_port(
        state->port_handle,
        STM_LIS3DH_INT1_CFG,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(rw_bytes != 1)
        return SNS_DDF_EBUS;

    return SNS_DDF_SUCCESS;
}

/**
 * @brief Called in response to an motion detection interrupt for this driver.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
static void sns_dd_acc_lis3dh_handle_int(
    sns_ddf_handle_t  dd_handle,
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp)
{
    sns_dd_acc_lis3dh_state_t* state = dd_handle;
    sns_ddf_status_e status;
    uint8_t rw_buffer = 0;
    uint8_t rw_bytes = 0;

    if (gpio_num != state->gpio_num)
        return;

    // verify motion detection interrupt by reading INT1_SRC.
    status = sns_ddf_read_port(
        state->port_handle,
        STM_LIS3DH_INT1_SRC,
        &rw_buffer,
        1,
        &rw_bytes);
    if(status != SNS_DDF_SUCCESS)
        return;
    if(rw_bytes != 1)
        return;

    if(((rw_buffer & 0x40) > 0) && ((rw_buffer & 0x2A) > 0))
    {
        // disable motion detection interrupt.
        status = sns_dd_acc_lis3dh_disable_int(dd_handle);
        if(status != SNS_DDF_SUCCESS)
            return;
        status = sns_ddf_signal_deregister(gpio_num);
        if(status != SNS_DDF_SUCCESS)
            return;

        // motion detected, send an event to SMGR
        status = sns_ddf_smgr_notify_event(
            state->smgr_hndl,
            SNS_DDF_SENSOR_ACCEL,
            SNS_DDF_EVENT_MOTION);
        if(status != SNS_DDF_SUCCESS)
            return;
    }
}
  
/*===========================================================================
  FUNCTION:   sns_dd_acc_lis3dhaccel_log_data
===========================================================================*/
/*!
  @brief log accel sensor data

  @detail
  - Logs latest set of sensor data sampled from the sensor.

  @param[in] accel_data_ptr: ptr to the driver data
*/
/*=========================================================================*/
static void sns_dd_acc_lis3dhaccel_log_data(sns_ddf_sensor_data_s *accel_data_ptr)
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s* log_struct_ptr;

  //Allocate log packet
  err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                               sizeof(sns_log_sensor_data_pkt_s),
                               (void**)&log_struct_ptr);

  if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = SNS_DDF_SENSOR_ACCEL;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_STMICRO;

    //Timestamp the log with sample time
    log_struct_ptr->timestamp = accel_data_ptr->timestamp;

    //Log the sensor data
    log_struct_ptr->num_data_types = STM_LIS3DH_NUM_AXES;
    log_struct_ptr->data[0]  = accel_data_ptr->samples[0].sample;
    log_struct_ptr->data[1]  = accel_data_ptr->samples[1].sample;
    log_struct_ptr->data[2]  = accel_data_ptr->samples[2].sample;

    //Commit log (also frees up the log packet memory)
    (void) sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
                             log_struct_ptr);
  }
}

/**
 * @brief Retrieves a single set of sensor data from lis3dh.
 *
 * Refer to sns_ddf_driver_if.h for definition.
 */
static sns_ddf_status_e sns_dd_acc_lis3dh_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_sensor_data_s** data)
{
    uint8_t read_buffer[STM_LIS3DH_NUM_READ_BYTES] = { 0 };
    uint8_t read_count = 0;
    sns_ddf_time_t timestamp;
    sns_ddf_status_e status;
    sns_ddf_sensor_data_s *data_ptr;
    sns_dd_acc_lis3dh_state_t *state = dd_handle;
    // sensor coordinate x,y,z axis raw hardware data
    int16_t hw_d[STM_LIS3DH_NUM_AXES] = { 0 };
    // SAE coordinate x,y,z axis data
    q16_t sae_d[STM_LIS3DH_NUM_AXES] = { 0 };

    //if current power mode is LOWPOWER , return error.
    if((SNS_DDF_POWERSTATE_LOWPOWER == state->power_state)||(num_sensors!=1))
        return SNS_DDF_EDEVICE;

    // This is a synchronous driver, so try to read data now.
    status = sns_ddf_read_port(
        state->port_handle,
        AUTO_INCREMENT | STM_LIS3DH_OUT_X_L_A,
        read_buffer,
        STM_LIS3DH_NUM_READ_BYTES,
        &read_count);
    if(status != SNS_DDF_SUCCESS)
        return status;
    if(read_count != STM_LIS3DH_NUM_READ_BYTES)
        return SNS_DDF_EBUS;

    // get current time stamp
    timestamp = sns_ddf_get_timestamp();

    //convert the raw data in read_buffer to X/Y/Z axis sensor data
    hw_d[0] = (int16_t) (((read_buffer[1]) << 8) | read_buffer[0]);
    hw_d[1] = (int16_t) (((read_buffer[3]) << 8) | read_buffer[2]);
    hw_d[2] = (int16_t) (((read_buffer[5]) << 8) | read_buffer[4]);

    //adjust sensor data per current sensitivity, and convert to Q16
    sae_d[0] = FX_FLTTOFIX_Q16((hw_d[0] >> 4) * state->sstvt_adj * G/1000);
    sae_d[1] = FX_FLTTOFIX_Q16((hw_d[1] >> 4) * state->sstvt_adj * G/1000);
    sae_d[2] = FX_FLTTOFIX_Q16((hw_d[2] >> 4) * state->sstvt_adj * G/1000);
    
    //map the sensor data to the phone's coordinates
    sns_ddf_map_axes(&state->axes_map, sae_d);

    state->out_of_bound_flag = 0;

    //allocate memory for sns_ddf_sensor_data_s data structure
    data_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_sensor_data_s));
    if(NULL == data_ptr)
    {
        return SNS_DDF_ENOMEM;
    }
    *data = data_ptr;

    data_ptr->sensor = sensors[0];
    data_ptr->status = SNS_DDF_SUCCESS;
    data_ptr->timestamp = timestamp;

    //allocate memory for data samples.
    data_ptr->samples = sns_ddf_memhandler_malloc(memhandler, STM_LIS3DH_NUM_AXES * sizeof(sns_ddf_sensor_sample_s));
    if(NULL == data_ptr->samples)
    {
       return SNS_DDF_ENOMEM;
    }

    //Fill out sensor output data and status
    //Axis and polarity conversion are configured in the header file.
    data_ptr->samples[0].sample = sae_d[0];
    data_ptr->samples[0].status = SNS_DDF_SUCCESS;
    data_ptr->samples[1].sample = sae_d[1];
    data_ptr->samples[1].status = SNS_DDF_SUCCESS;
    data_ptr->samples[2].sample = sae_d[2];
    data_ptr->samples[2].status = SNS_DDF_SUCCESS;
    data_ptr->num_samples = STM_LIS3DH_NUM_AXES;

    if (EnableI2C == FALSE)
    {
       static uint32 dummyLis3dhData = 1;
       data_ptr->samples[0].sample = dummyLis3dhData++;
       data_ptr->samples[1].sample = dummyLis3dhData++;
       data_ptr->samples[2].sample = dummyLis3dhData++;
    }

    sns_dd_acc_lis3dhaccel_log_data(data_ptr);

    return SNS_DDF_SUCCESS;
}

