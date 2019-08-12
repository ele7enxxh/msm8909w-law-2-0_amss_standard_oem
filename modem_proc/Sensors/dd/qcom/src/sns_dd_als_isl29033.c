/*==============================================================================
*
*  			SENSORS  AMBIENT LIGHT SENSOR  DRIVER
*
* DESCRIPTION : Implements the driver for the Intersil ALS(Ambient Light Sensor) 
*      This driver has 2 sub-modules:
*      1. The common handler that provides the driver interface
*      2. The light(ALS) driver that handles light data type
*  
* AUTHOR : Louis Park  @  <lpark@intersil.com> 
*                            
* Copyright (c) 2013 by Intersil Corporation All Rights Reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*        notice, this list of conditions and the following disclaimer in the
*        documentation and/or other materials provided with the distribution.
*     3. Neither the name of the Intersil Corporation nor the
*        names of its contributors may be used to endorse or promote products
*        derived from this software without specific prior written permission.
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
*
==============================================================================*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
  -----------------------------------------------------------------------------*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     -----------------------------------------------------------
08-08-2014   MW      Eliminated KW error
06-24-2014   MW      Fixed polling mode, added DRI mode, added log packets

==============================================================================*/

/*==============================================================================
INCLUDE FILES
==============================================================================*/

#include "sns_dd_als_isl29033.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"

#include "sns_log_types_public.h"
#include "sns_log_api_public.h"

static sns_ddf_sensor_e als_timer = SNS_DDF_SENSOR_AMBIENT;

/*==============================================================================
STATIC VARIABLE DEFINITIONS
==============================================================================*/
sns_ddf_sensor_e sns_dd_als_sensor_types [SNS_DD_ALS_NUM_SENSOR_TYPES]=
{
    SNS_DDF_SENSOR__NONE,
    SNS_DDF_SENSOR_AMBIENT
};

sns_ddf_driver_if_s sns_als_isl29033_driver_fn_list;

static sns_ddf_odr_t isl29033_odr_list[] = {
    SNS_DD_ISL_ALS_FREQ
};

static uint32_t isl29033_odr_list_size = sizeof(isl29033_odr_list)/sizeof(isl29033_odr_list[0]);

/*==============================================================================
STATIC FUNCTION PROTOTYPES
===============================================================================*/


/*===========================================================================

FUNCTION      sns_dd_als_isl29033_log_data

DESCRIPTION   Log ALS data

RETURN VALUE  none

===========================================================================*/
static void sns_dd_als_isl29033_log_data(
   sns_ddf_sensor_e       sensor_type,
   uint32_t               data1, 
   uint32_t               data2,
   uint32_t               data3,
   sns_ddf_time_t         timestamp) 
{
    sns_err_code_e err_code;
    sns_log_sensor_data_pkt_s* log_struct_ptr;

    ISL29033_MSG_3(HIGH,"sns_dd_als_isl29033_log_data data1=%d data2=%X data3=%d", data1, data2, data3);

    /* Allocate log packet */
    err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
        sizeof(sns_log_sensor_data_pkt_s),
        (void**)&log_struct_ptr);

    if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
    {
        log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
        log_struct_ptr->sensor_id = sensor_type;
        log_struct_ptr->vendor_id = SNS_DDF_VENDOR_INTERSIL;

        /* Timestamp the log with sample time */
        log_struct_ptr->timestamp = timestamp;

        /* Log sensor data */
        log_struct_ptr->num_data_types = 3;
        log_struct_ptr->data[0] = data1; // data in mlux in unit32_t
        log_struct_ptr->data[1] = data2; // data in lux in Q16
        log_struct_ptr->data[2] = data3; // raw clear channel data

        /* Commit log (also frees up the log packet memory) */
        err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
            log_struct_ptr);
        if(err_code != SNS_SUCCESS)
        {
            ISL29033_MSG_0(ERROR,"Error in sns_logpkt_commit");
        }	
    }
    else
    {
        ISL29033_MSG_1(ERROR,"sns_dd_als_isl29033_log_data - error=%d", err_code);
    }
}

/*===========================================================================

FUNCTION      sns_dd_als_isl29033_set_threshold

DESCRIPTION   Update low and high ALS threshold

RETURN VALUE  SNS_DDF_SUCCESS if successful else error code

===========================================================================*/
static sns_ddf_status_e sns_dd_als_isl29033_set_threshold
(
    sns_dd_als_state_t * dd_ptr,
    uint16_t             low_thresh,
    uint16_t             high_thresh
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    uint8_t          i2c_data[2];
    uint8_t          bytes_rw;

    /* Write low threshold */
    i2c_data[0] = (uint8_t)low_thresh;
    i2c_data[1] = (uint8_t)(low_thresh >> 8);

    status = sns_ddf_write_port(dd_ptr->port_handle,
        ALS_DD_ISL_LOW_THRES_LBYTE,
        i2c_data,
        2,
        &bytes_rw);
    if(status != SNS_DDF_SUCCESS)
    {
        ISL29033_MSG_1(ERROR,"write failed status=%d", status);
        return status;
    }

    /* Write high threshold */
    i2c_data[0] = (uint8_t)high_thresh;
    i2c_data[1] = (uint8_t)(high_thresh >> 8);

    status = sns_ddf_write_port(dd_ptr->port_handle,
        ALS_DD_ISL_HIGH_THRES_LBYTE,
        i2c_data,
        2,
        &bytes_rw);
    if(status != SNS_DDF_SUCCESS)
    {
        ISL29033_MSG_1(ERROR,"write failed status=%d", status);
        return status;
    }

    dd_ptr->sns_dd_als_db.als_low_thres = low_thresh;
    dd_ptr->sns_dd_als_db.als_high_thres = high_thresh;

    return status;
}

/*===========================================================================

FUNCTION      sns_dd_als_isl29033_start_timer

DESCRIPTION   Starts a timer

RETURN VALUE  None

===========================================================================*/
static void sns_dd_als_isl29033_start_timer
(
    sns_dd_als_state_t*   dd_ptr,
    sns_ddf_sensor_e	sensor_type, 
    uint32_t		duration
)
{
    sns_dd_timer_type* timer_ptr;
    sns_ddf_status_e status;

    if((dd_ptr == NULL) || (sensor_type != SNS_DDF_SENSOR_AMBIENT)){
        return;
    }

    ISL29033_MSG_2(HIGH,"sns_dd_als_isl29033_start_timer sensor=%d duration=%d",sensor_type, duration);

    if(sensor_type == SNS_DDF_SENSOR_AMBIENT){
        timer_ptr = &dd_ptr->sns_dd_als_common_db.als_timer;
    }else{
        ISL29033_MSG_1(ERROR,"Unknown sensor Device Access %d",sensor_type);
        return;
    }
    if( timer_ptr->active == true ){
        return;
    }
    status = sns_ddf_timer_start(timer_ptr->timer_obj, duration);
    if ( status != SNS_DDF_SUCCESS ){
        ISL29033_MSG_1(ERROR,"Failed to start the general error=%d",status);
        return;
    }
    timer_ptr->active = true;

    ISL29033_MSG_0(MED,"General Timer Active");
}

/*===========================================================================

FUNCTION      sns_dd_als_isl29033_stop_timer

DESCRIPTION   Stops a timer

RETURN VALUE  None

===========================================================================*/
static void sns_dd_als_isl29033_stop_timer
(
    sns_dd_als_state_t* 	dd_ptr,
    sns_ddf_sensor_e      sensor_type
)
{
    sns_dd_timer_type* timer_ptr;

    if(dd_ptr == NULL){
        return;
    }

    ISL29033_MSG_1(HIGH,"sns_dd_als_isl29033_stop_timer sensor_type=%d", sensor_type);

    if (sensor_type == SNS_DDF_SENSOR_AMBIENT){
        timer_ptr = &dd_ptr->sns_dd_als_common_db.als_timer;
    }else{
        ISL29033_MSG_1(ERROR,"Unknown sensor Device Access %d",sensor_type);
        return;
    }
    if(true == timer_ptr->active){
        sns_ddf_timer_cancel(timer_ptr->timer_obj); /* Will return error if timer
                                                    already stopped */
        timer_ptr->active = false;
    }

    ISL29033_MSG_0(MED,"General Timer Stopped");
}


/*===========================================================================

FUNCTION      sns_dd_als_isl29033_get_device_config

DESCRIPTION   Reads device registers to store configuration in state

RETURN VALUE  SNS_DDF_SUCCESS is successful else error code

===========================================================================*/
static sns_ddf_status_e sns_dd_als_isl29033_get_device_config(sns_dd_als_state_t* dd_ptr)
{
    uint8_t i2c_data_reg, bytes_rw;
    uint8_t data[2];
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    if(dd_ptr == NULL){
        return SNS_DDF_EINVALID_PARAM;
    }

    /* Read Current configuration from command register 1 */
    status = sns_ddf_read_port(dd_ptr->port_handle,
        ALS_DD_ISL_COMMAND_REG_1,
        (uint8_t *)&i2c_data_reg,
        1,
        &bytes_rw);

    if(status != SNS_DDF_SUCCESS){
        return status;
    }

    ISL29033_MSG_1(MED,"ALS_DD_ISL_COMMAND_REG_1=%X", i2c_data_reg);

    switch((i2c_data_reg & ALS_DD_ISL_MODE_MASK) >> 5){
    case 0x00:dd_ptr->sns_dd_als_db.mode = SNS_DD_DEV_POWER_DOWN;
        break;
    case 0x05:dd_ptr->sns_dd_als_db.mode = SNS_DD_DEV_ALS_CONT;
        break;
    case 0x06:dd_ptr->sns_dd_als_db.mode = SNS_DD_DEV_IR_CONT;
        break;
    default:  dd_ptr->sns_dd_als_db.mode = SNS_DD_DEV_UNDEF;
        break;
    }          

    switch(i2c_data_reg & ALS_DD_ISL_INTR_PERSIST_MASK)
    {
    case 0x00:dd_ptr->sns_dd_als_db.persist = SNS_DD_DEV_PERSIST_1;
        break;
    case 0x01:dd_ptr->sns_dd_als_db.persist = SNS_DD_DEV_PERSIST_4;
        break;
    case 0x02:dd_ptr->sns_dd_als_db.persist = SNS_DD_DEV_PERSIST_8;
        break;
    case 0x03:dd_ptr->sns_dd_als_db.persist = SNS_DD_DEV_PERSIST_16;
        break;
    default  :break;
    }

    /* Read Current configuration in command register 2 */
    status = sns_ddf_read_port(dd_ptr->port_handle,
        ALS_DD_ISL_COMMAND_REG_2,
        &i2c_data_reg,
        1,
        &bytes_rw
        );
    if(status != SNS_DDF_SUCCESS){
        return status;
    }

    ISL29033_MSG_1(MED,"ALS_DD_ISL_COMMAND_REG_2=%X", i2c_data_reg);

    switch((i2c_data_reg & ALS_DD_ISL_ADC_RES_MASK) >> 2)
    {
    case 0x00:dd_ptr->sns_dd_als_db.adc_resolution = SNS_DD_DEV_ADC_16;
        break;
    case 0x01:dd_ptr->sns_dd_als_db.adc_resolution = SNS_DD_DEV_ADC_12;
        break;
    case 0x02:dd_ptr->sns_dd_als_db.adc_resolution = SNS_DD_DEV_ADC_8;
        break;
    case 0x03:dd_ptr->sns_dd_als_db.adc_resolution = SNS_DD_DEV_ADC_4;
        break;
    default:  break;
    }

    switch(i2c_data_reg & ALS_DD_ISL_RANGE_MASK)
    {
    case 0x00:dd_ptr->sns_dd_als_db.range = SNS_DD_DEV_RANGE_125;
        break;
    case 0x01:dd_ptr->sns_dd_als_db.range = SNS_DD_DEV_RANGE_500;
        break;
    case 0x02:dd_ptr->sns_dd_als_db.range = SNS_DD_DEV_RANGE_2000;
        break;
    case 0x03:dd_ptr->sns_dd_als_db.range = SNS_DD_DEV_RANGE_8000;
        break;
    default:  break;
    }

    /* Read The ALS Data Register */
    status = sns_ddf_read_port(dd_ptr->port_handle,
        ALS_DD_ISL_DATA_REG_LBYTE,
        data,
        2,
        &bytes_rw
        );
    if(status != SNS_DDF_SUCCESS){
        return status;
    }

    dd_ptr->sns_dd_als_db.als_data = (data[1] << 8) | (data[0]);

    /* Read The Low Threshold Register */
    status = sns_ddf_read_port(dd_ptr->port_handle,
        ALS_DD_ISL_LOW_THRES_LBYTE,
        data,
        2,
        &bytes_rw
        );
    if(status != SNS_DDF_SUCCESS){
        return status;
    }
    dd_ptr->sns_dd_als_db.als_low_thres = (data[1] << 8) | (data[0]);

    /* Read The High Threshold Register */
    status = sns_ddf_read_port(	dd_ptr->port_handle,
        ALS_DD_ISL_HIGH_THRES_LBYTE,
        data,
        2,	
        &bytes_rw
        );
    if(status != SNS_DDF_SUCCESS){
        return status;
    }

    dd_ptr->sns_dd_als_db.als_high_thres = (data[1] << 8) | (data[0]);

    return status;
}

/*==============================================================================

FUNCTION        sns_dd_als_isl29033_req_data

DESCRIPTION     functions that handles the data request from sub drivers

RETURN VALUE    None

==============================================================================*/
static sns_ddf_status_e sns_dd_als_isl29033_req_data(sns_dd_als_state_t* dd_ptr,
    sns_ddf_sensor_e data_type)
{

    if(dd_ptr == NULL)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    ISL29033_MSG_0(HIGH,"sns_dd_als_isl29033_req_data");

    sns_dd_als_isl29033_start_timer(dd_ptr, data_type, SNS_DD_ALS_DATA_READY_uS);

    return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      sns_dd_als_isl29033_async_notify

DESCRIPTION   Notify to SMGR with a sensor data.

RETURN VALUE  None

==============================================================================*/
static void sns_dd_als_isl29033_async_notify
(
    sns_dd_als_state_t*   dd_ptr,
    sns_ddf_sensor_e      sensor_type,
    uint32_t              data_val,
    uint32_t              raw_data
)
{
    sns_ddf_sensor_sample_s *buf_ptr;
    sns_ddf_sensor_data_s *sensor_data;
    sns_ddf_time_t timestamp = sns_ddf_get_timestamp();

    if((dd_ptr == NULL) || (sensor_type != SNS_DDF_SENSOR_AMBIENT))
    {
        return;
    }

    buf_ptr = &dd_ptr->samples[0];
    sensor_data = &dd_ptr->s_data;

    if((buf_ptr == NULL) || (sensor_data == NULL))
    {
        return;
    }

    sensor_data->sensor = SNS_DDF_SENSOR_AMBIENT;
    sensor_data->status = SNS_DDF_SUCCESS;
    sensor_data->num_samples = 2;
    sensor_data->timestamp = timestamp;
    sensor_data->samples = buf_ptr;
    buf_ptr[0].status = SNS_DDF_SUCCESS;
    buf_ptr[0].sample = FX_CONV_Q16(data_val,0); // data in lux in Q16
    buf_ptr[1].status = SNS_DDF_SUCCESS;
    buf_ptr[1].sample = raw_data; // raw data

    sns_ddf_smgr_notify_data(dd_ptr->smgr_handle,
        sensor_data,
        1);

    /* Log sensor data */
    sns_dd_als_isl29033_log_data(SNS_DDF_SENSOR_AMBIENT, (data_val*1000), 
                                  buf_ptr[0].sample, raw_data, timestamp);
}

/*==============================================================================

FUNCTION      sns_dd_als_isl29033_get_lux

DESCRIPTION   Calculate lux output

RETURN VALUE  None

==============================================================================*/

static void sns_dd_als_isl29033_get_lux(sns_dd_als_state_t* dd_ptr, uint32_t *lux_data, uint16_t raw_data)
{
    if((dd_ptr == NULL) || (lux_data == NULL))
    {
        ISL29033_MSG_0(ERROR,"NULL pointer");
        return;
    }

    ISL29033_MSG_3(MED,"range=%d, raw_data=%d, resolution=%d", dd_ptr->sns_dd_als_db.range, raw_data, dd_ptr->sns_dd_als_db.adc_resolution);

    // lux = (FSR * raw_data / (2^n)
    // n is ADC resolution in bits
    *lux_data = (uint32_t)(((dd_ptr->sns_dd_als_db.range) * raw_data) >> dd_ptr->sns_dd_als_db.adc_resolution);

    ISL29033_MSG_1(MED,"lux_data=%d", *lux_data);
}

/*==============================================================================

FUNCTION      sns_dd_als_isl29033_data_ready

DESCRIPTION   This function is called when the ALS raw data is ready

RETURN VALUE  None

==============================================================================*/
static void sns_dd_als_isl29033_data_ready
(
    sns_dd_als_state_t* dd_ptr
)
{
    uint8_t       bytes_rw;
    uint16_t      raw_data;
    uint32_t      lux_data;
    uint8_t       i2c_data[2];
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    if(dd_ptr == NULL)
    {
        return;
    }

    ISL29033_MSG_0(HIGH,"sns_dd_als_isl29033_data_ready");
  
    /* Don't care about pend/idle state in DRI mode. */
    if(!dd_ptr->dri_enable)
    {

        if(dd_ptr->sns_dd_als_db.pend_state != SNS_DD_PEND_STATE_PEND)
        {
            ISL29033_MSG_1(ERROR,"pend state=%d", dd_ptr->sns_dd_als_db.pend_state);
            return;
        }

        dd_ptr->sns_dd_als_db.pend_state = SNS_DD_PEND_STATE_IDLE;
    }

    status = sns_ddf_read_port(dd_ptr->port_handle,
        ALS_DD_ISL_DATA_REG_LBYTE,
        i2c_data,
        2,
        &bytes_rw );
    if(status != SNS_DDF_SUCCESS)
    {
        ISL29033_MSG_1(ERROR,"read failed error=%d", status);
        return;
    }

    raw_data = (i2c_data[1]<<8) | (i2c_data[0]);

    ISL29033_MSG_1(MED,"raw_data=%d", raw_data);

    sns_dd_als_isl29033_get_lux(dd_ptr, &lux_data, raw_data);

    dd_ptr->sns_dd_als_db.als_data = lux_data;

    sns_dd_als_isl29033_async_notify(dd_ptr,
        SNS_DDF_SENSOR_AMBIENT,
        (uint32_t)dd_ptr->sns_dd_als_db.als_data,
        (uint32_t)raw_data);

    /* Update threshold if in DRI */
    if(dd_ptr->dri_enable)
    {
        /* use +/- 10% of raw data */
        sns_dd_als_isl29033_set_threshold(dd_ptr, (0.9*raw_data), (1.1*raw_data));
    }
}

/*==============================================================================

FUNCTION        sns_dd_als_isl29033_handle_irq

==============================================================================*/
/*
@brief        ISL29033 Interrupt Handler

@detail       This Function will be called within the context of the
interrupt service task (IST) not ISR.

@param[in]    dd_handle       Handle to a driver instance
@param[in]    irq             The IRQ Representing the interrupt that occured.

@return       None
==============================================================================*/
static void sns_dd_als_isl29033_handle_irq
(
    sns_ddf_handle_t  handle,
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp
)
{
    sns_dd_als_state_t* dd_ptr = (sns_dd_als_state_t*)handle;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    uint8_t       bytes_rw;
    uint8_t       i2c_data;

    if(dd_ptr == NULL)
    {
        return;
    }

    if(gpio_num != dd_ptr->gpio1)
    {
        ISL29033_MSG_1(ERROR,"Invalid IRQ request gpio=%d", gpio_num);
        return;
    }

    sns_dd_als_isl29033_data_ready(dd_ptr);

    /* Clear the interrupt flag */
    status = sns_ddf_read_port(dd_ptr->port_handle,
        ALS_DD_ISL_COMMAND_REG_1,
        &i2c_data,
        1,
        &bytes_rw);
    if(status != SNS_DDF_SUCCESS)
    {
        ISL29033_MSG_1(ERROR,"Failed to clear the interrupt error=%d", status);
        return;
    }

}

/*==============================================================================

Function        sns_dd_als_isl29033_handle_timer

==============================================================================*/
/*
@brief        called when the timer set by this driver has expired.
This is the callback function submitted when initializing a timer.

@detail       This will be called with in the context of the sensor 
manager task.it indicate that sensor data is ready

@param[in] 	dd_handle Handle to a driver instance
@param[in] 	arg the argument submitted when the timer was set

@return       None
==============================================================================*/

static void sns_dd_als_isl29033_handle_timer
(
    sns_ddf_handle_t handle,
    void* arg
)
{
    sns_dd_als_state_t* dd_ptr = (sns_dd_als_state_t*)handle;
    sns_ddf_sensor_e  sensor_type = *(sns_ddf_sensor_e*)arg;

    if(dd_ptr == NULL)
    {
        return;
    }

    ISL29033_MSG_1(HIGH,"sns_dd_als_isl29033_handle_timer sensor_type=%d", sensor_type);

    if(sensor_type != SNS_DDF_SENSOR_AMBIENT)
    {
        ISL29033_MSG_1(ERROR,"Unknown sensor device sensor_type=%d", sensor_type);
        return;
    }

    sns_dd_als_isl29033_stop_timer(dd_ptr,sensor_type);
    sns_dd_als_isl29033_data_ready(dd_ptr);

    if(dd_ptr->polling_enable)
    {
        dd_ptr->polling_enable = false;
    }
}


/*==============================================================================

FUNCTION        sns_dd_als_isl29033_default_config

DESCRIPTION     Initialize the light sensor (ALS)driver

DEPENDENCIES    None

RETURN VALUE    None
==============================================================================*/

static void sns_dd_als_isl29033_default_config(sns_dd_als_state_t* dd_ptr)
{
    sns_dd_als_db_type *als_db_ptr;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    uint8_t bytes_rw,i2c_data_reg;
    uint16_t i2c_data;

    if(dd_ptr == NULL)
    {
        return;
    }

    als_db_ptr = &dd_ptr->sns_dd_als_db;

    als_db_ptr->pend_state = SNS_DD_PEND_STATE_IDLE;
    dd_ptr->sns_dd_als_common_db.als_timer.active = false;
    dd_ptr->polling_enable = false;
    dd_ptr->dri_enable = false;
    dd_ptr->sns_dd_als_db.als_odr = 0;

    /* Set default configuration
    MODE : ALS Continuous
    PERSISTENCY : 4    */
    i2c_data_reg = ALS_DD_ISL_MODE_SET_ALS_CONT |
        ALS_DD_ISL_INTR_PERSIST_SET_4;

    status = sns_ddf_write_port(dd_ptr->port_handle,
        ALS_DD_ISL_COMMAND_REG_1,
        &i2c_data_reg,
        1,
        &bytes_rw);
    if(status != SNS_DDF_SUCCESS)
    {
        ISL29033_MSG_1(ERROR,"write failed status=%d", status);
        return;
    }

    /* Set Default Resolution and range
    Range = 8,000
    ADC Res = 16-bit */
    i2c_data_reg = ALS_DD_ISL_ADC_RES_SET_16 |
        ALS_DD_ISL_RANGE_SET_8000;

    status = sns_ddf_write_port(dd_ptr->port_handle,
        ALS_DD_ISL_COMMAND_REG_2,
        &i2c_data_reg,
        1,
        &bytes_rw);

    if(status != SNS_DDF_SUCCESS)
    {
        ISL29033_MSG_1(ERROR,"I2C write failed status=%d", status);
        return;
    }

    /* Set Default Low Threshold to 5% of High Value */
    i2c_data = ALS_DD_ISL_LT_VALUE;
    status = sns_ddf_write_port(dd_ptr->port_handle,
        ALS_DD_ISL_LOW_THRES_LBYTE,
        (uint8_t*)&i2c_data,
        2,
        &bytes_rw);

    if ( status != SNS_DDF_SUCCESS ){
        ISL29033_MSG_1(ERROR,"I2C write failed status=%d", status);
        return;
    }

    /* Set default high threshold to 20% of high value */
    i2c_data = ALS_DD_ISL_HT_VALUE;
    status = sns_ddf_write_port(dd_ptr->port_handle,
        ALS_DD_ISL_HIGH_THRES_LBYTE,
        (uint8_t*)&i2c_data,
        2,
        &bytes_rw);
    if(status != SNS_DDF_SUCCESS)
    {
        ISL29033_MSG_1(ERROR,"I2C write failed status=%d", status);
        return;
    }

}

/*===========================================================================

FUNCTION      sns_dd_als_isl29033_set_powerstate

DESCRIPTION   Sets the power state of the ISL ALS Sensor

RETURN VALUE  SNS_DDF_SUCCESS on success, error code otherwise

===========================================================================*/
static sns_ddf_status_e sns_dd_als_isl29033_set_powerstate
(
    sns_dd_als_state_t*           dd_ptr,
    sns_ddf_powerstate_e          op_mode,
    bool                          init_data
)
{
    uint8_t    i2c_data = 0;
    uint8_t    bytes_w;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    if(dd_ptr == NULL)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    /* Read the mode of operation */
    status = sns_ddf_read_port(dd_ptr->port_handle,
        ALS_DD_ISL_COMMAND_REG_1,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);

    if ( status != SNS_DDF_SUCCESS ){
        ISL29033_MSG_1(ERROR,"I2C read failed status=%d", status);
        return status;
    }

#ifdef ISL29033_DEBUG
    dd_ptr->last_mode = i2c_data & INTR_FLG_CLR;
#endif /* ISL29033_DEBUG */

    ISL29033_MSG_1(HIGH,"before ALS_DD_ISL_COMMAND_REG_1=%X", i2c_data);

    if ( op_mode == SNS_DDF_POWERSTATE_ACTIVE )
    {

        if(init_data == true){
            sns_dd_als_isl29033_default_config(dd_ptr);
        }

        i2c_data = (i2c_data & ALS_DD_ISL_WRITE_MASK) | ALS_DD_ISL_MODE_SET_ALS_CONT;
    }
    else{ /* SNS_DDF_POWERSTATE_LOWPOWER */

        /* stop any pending timers */
        sns_dd_als_isl29033_stop_timer(dd_ptr, SNS_DDF_SENSOR_AMBIENT);

        i2c_data = (i2c_data & ALS_DD_ISL_WRITE_MASK) | ALS_DD_ISL_MODE_SET_PWDN;

    }

    ISL29033_MSG_1(HIGH,"after ALS_DD_ISL_COMMAND_REG_1=%X", i2c_data);

    status = sns_ddf_write_port(dd_ptr->port_handle,
        ALS_DD_ISL_COMMAND_REG_1,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);
    if ( status != SNS_DDF_SUCCESS ){
        ISL29033_MSG_1(ERROR,"I2C write failed status=%d", status);
        return status;
    }
    dd_ptr->sns_dd_als_common_db.pwr_mode = op_mode;
    dd_ptr->sns_dd_als_common_db.state = SNS_DD_DEV_CONFIGURED;

    status = sns_dd_als_isl29033_get_device_config(dd_ptr);

    return status;
}

/*===========================================================================

FUNCTION:   sns_dd_als_isl29033_reset

===========================================================================*/
/*!
@brief 	Resets the driver and device so they return to the state they were
in after init() was called.

@detail	Resets  the driver state structure

@param[in] 	handle  Handle to a driver instance.

@return 	Success if the driver was able to reset its state and the device.
Otherwise a specific error code is returned.
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_als_isl29033_reset
(
    sns_ddf_handle_t dd_handle 
)
{
    sns_dd_als_state_t* dd_ptr = (sns_dd_als_state_t *)dd_handle;
    sns_ddf_status_e init_status = SNS_DDF_SUCCESS;

    if ( dd_handle == NULL ){
        return SNS_DDF_EINVALID_PARAM;
    }

    ISL29033_MSG_0(HIGH,"RESET");

    /* Initialize the device to default */
    sns_dd_als_isl29033_default_config(dd_ptr);
    if ((init_status = sns_dd_als_isl29033_set_powerstate(dd_ptr,
                                                 SNS_DDF_POWERSTATE_LOWPOWER,
                                                 false)) != SNS_DDF_SUCCESS )
    {
        ISL29033_MSG_1(ERROR,"Failed to Set Power state status=%d", init_status);
        return init_status;
    }
    return init_status;
}

/*===========================================================================

FUNCTION:   sns_dd_als_isl29033_set_attr

===========================================================================*/
/*!
@brief: 	Sets an attribute of the ISL 29033 ALS sensor

@detail: 	Called by SMGR to set certain device attributes that are
programmable. Curently its the power mode and range.

@param[in] dd_handle:   Handle to a driver instance.
@param[in] sensor:      Sensor for which this attribute is to be set.
@param[in] attrib:      Attribute to be set.
@param[in] value:       Value to set this attribute.

@return 	The error code definition within the DDF
SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
SNS_DDF_EINVALID_PARAM
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_als_isl29033_set_attrib
(
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    void*                value
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    if ( (dd_handle == NULL) || (value == NULL) ){
        return SNS_DDF_EINVALID_PARAM;

    }

    ISL29033_MSG_2(HIGH,"sns_dd_als_isl29033_set_attrib sesnor=%d attrib=%d", sensor, attrib);

    switch ( attrib )
    {
    case SNS_DDF_ATTRIB_POWER_STATE:
        /* set power mode */
        status = sns_dd_als_isl29033_set_powerstate((sns_dd_als_state_t *)dd_handle,
            *(sns_ddf_powerstate_e *)value,
            true);

        ISL29033_MSG_1(HIGH,"SNS_DDF_ATTRIB_POWER_STATE=%d", *(sns_ddf_powerstate_e *)value);

        break;

    case SNS_DDF_ATTRIB_RANGE:
        // Use 8000 lux range by default
        /*status = sns_dd_als_set_range((sns_dd_als_state_t *)dd_handle,
        *(sns_ddf_range_e *)value);*/
        break;

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        // Use 16-bit ADC resolution by default
        /*status = sns_dd_als_set_adc_res((sns_dd_als_state_t *)dd_handle,
        *(sns_ddf_adc_res_e *)value);*/

        break;
    case SNS_DDF_ATTRIB_ODR:
        {
            sns_dd_als_state_t* dd_ptr = (sns_dd_als_state_t *)dd_handle;
            dd_ptr->sns_dd_als_db.als_odr =  (uint32_t)(*(sns_ddf_odr_t *)value);
        }
        break;
    default:
        return SNS_DDF_EINVALID_PARAM;
    }
    if(status != SNS_DDF_SUCCESS){

        ISL29033_MSG_1(ERROR,"Failed to set attributes status=%d", status);
        return status;
    }
    return status ;
}

/*===========================================================================

FUNCTION      sns_dd_als_isl29033_prepare_data_from_device

DESCRIPTION   For preparing a sensor data, this function issues a request to
the arbitration manager

RETURN VALUE  true if error else false

===========================================================================*/
static bool  sns_dd_als_isl29033_prepare_data_from_device(sns_dd_als_state_t* dd_ptr)
{
    bool err = true;

    if(dd_ptr == NULL)
    {
        return err;
    }

    ISL29033_MSG_1(HIGH,"sns_dd_als_isl29033_prepare_data_from_device pend_state=%d", dd_ptr->sns_dd_als_db.pend_state);

    if ( dd_ptr->sns_dd_als_db.pend_state == SNS_DD_PEND_STATE_IDLE ){

        if (sns_dd_als_isl29033_req_data(dd_ptr, SNS_DDF_SENSOR_AMBIENT) == SNS_DDF_SUCCESS)
        {
            dd_ptr->sns_dd_als_db.pend_state = SNS_DD_PEND_STATE_PEND;
            err = false;
        }
    }
    return err;
}

/*===========================================================================

FUNCTION:   sns_dd_als_isl29023_get_data

===========================================================================*/
/*
@brief :  Called by the SMGR to get data

@detail : Requests a single sample of sensor data. Data is returned
immediately after being read from the sensor, in which case data[]
is populated in the same order it was requested.

This driver is a pure asynchronous one, so no data will be written to buffer.
As a result, the return value will be always PENDING if the process does
not fail.  This driver will notify the Sensors Manager via asynchronous
notification when data is available.

@param[in]  dd_handle    Handle to a driver instance.
@param[in]  sensors      List of sensors for which data is requested.
@param[in]  num_sensors  Length of @a sensors.
@param[in]  memhandler   Memory handler used to dynamically allocate
output parameters, if applicable.
@param[out] data         Sampled sensor data. The number of elements must
match @a num_sensors.

@return SNS_DDF_SUCCESS if data was populated successfully. If the
sensors queried are to be read asynchronously SNS_DDF_PENDING is
returned and data is via @a sns_ddf_smgr_data_notify() when
available. Otherwise a specific error code is returned.
*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_als_isl29033_get_data
(
    sns_ddf_handle_t        dd_handle,
    sns_ddf_sensor_e        sensors[],
    uint32_t                num_sensors,
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_data_s*  data[]                /* ignored by this async driver */
)
{
    sns_ddf_status_e status = SNS_DDF_PENDING;
    sns_dd_als_state_t* dd_ptr = (sns_dd_als_state_t *)dd_handle;
    bool err = true;

    if ((dd_handle == NULL) || (sensors == NULL) ||(num_sensors != 1)){
        return SNS_DDF_EINVALID_PARAM;
    }

    ISL29033_MSG_1(HIGH,"sns_dd_als_isl29033_get_data num_sensors=%d", num_sensors);

    if(dd_ptr->sns_dd_als_common_db.state != SNS_DD_DEV_CONFIGURED){
        /* Device state is stopped or pending */
        ISL29033_MSG_0(ERROR,"Device is not configured");
        return SNS_DDF_EFAIL;
    }

    if(sensors[0] == SNS_DDF_SENSOR_AMBIENT){
        err = sns_dd_als_isl29033_prepare_data_from_device(dd_ptr);
        if(err == true)
        {
            ISL29033_MSG_1(ERROR,"Get Data Request Error=%d", err);
            status = SNS_DDF_EFAIL;
        }
    }
    else
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    dd_ptr->polling_enable = true;

    return status; 
}

/*==============================================================================

FUNCTION:   sns_dd_als_isl29033_init

==============================================================================*/
/*
@brief :   Initializes the Ambient Light Sensing and Allocates a handle to a
driver instance, opens a communication port to associated device,
configures the driver and device, and places the devices in the          
default power state. Returns the instance handle. 
This function will be called at init time.

@param[out] dd_handle_ptr  Pointer that this function must malloc and 
populate. This is a handle to the driver
instance that will be passed in to all other
functions. NB: Do not use @a memhandler to
allocate this memory.
@param[in]  smgr_handle    Handle used to identify this driver when it 
calls into Sensors Manager functions.

@param[in]  nv_params      NV parameters retrieved for the driver.
@param[in]  device_info    Access info for physical devices controlled by 
this driver. Used to configure the bus
and talk to the devices.
@param[in]  num_devices    Number of elements in @a device_info. 
@param[in]  memhandler     Memory handler used to dynamically allocate 
output parameters, if applicable. NB: Do not
use memhandler to allocate memory for
@a dd_handle_ptr.
@param[out] sensors        List of supported sensors, allocated, 
populated, and returned by this function.
@param[out] num_sensors    Number of elements in @a sensors.

@return Success if @a dd_handle_ptr was allocated and the driver was 
configured properly. Otherwise a specific error code is returned.
==============================================================================*/
static sns_ddf_status_e sns_dd_als_isl29033_init
(
    sns_ddf_handle_t*             dd_ptr,
    sns_ddf_handle_t              smgr_handle,
    sns_ddf_nv_params_s*          reg_params,
    sns_ddf_device_access_s       device_info[],
    uint32_t                      num_devices,
    sns_ddf_memhandler_s*         memhandler,
    sns_ddf_sensor_e*             sensors[],
    uint32_t*                     num_sensors
)
{
    sns_ddf_status_e  status = SNS_DDF_SUCCESS;
    sns_dd_als_state_t    *ptr;

    if((dd_ptr == NULL) || (num_sensors == NULL) || (sensors == NULL))
        return SNS_DDF_EINVALID_PARAM;


    if(sns_ddf_malloc((void**)&ptr,sizeof(sns_dd_als_state_t)) != SNS_DDF_SUCCESS)
        return SNS_DDF_ENOMEM;

    *dd_ptr = (sns_ddf_handle_t)ptr;

    ptr->smgr_handle = smgr_handle;
    status = sns_ddf_open_port(&ptr->port_handle,&device_info->port_config);
    if(status != SNS_DDF_SUCCESS){
        ISL29033_MSG_1(ERROR,"Failed to open port status=%d", status);
        return status;
    }            

    status = sns_ddf_timer_init(&ptr->sns_dd_als_common_db.als_timer.timer_obj,
        ptr,
        &sns_als_isl29033_driver_fn_list,
        &als_timer,
        false);
    if(status != SNS_DDF_SUCCESS)
    {
        ISL29033_MSG_1(ERROR,"failed to initialize the timer for status=%d", status);
        return status;
    }

    ptr->gpio1 = device_info->first_gpio;

    /* Fill the Sensor Information */
    *num_sensors = SNS_DD_ALS_NUM_SENSOR_TYPES;
    *sensors = (sns_ddf_sensor_e*)sns_dd_als_sensor_types;

    /* Reset the driver */
    if(sns_dd_als_isl29033_reset((sns_ddf_handle_t)ptr) != SNS_DDF_SUCCESS)
    {
        ISL29033_MSG_0(ERROR,"Failed to reset the isl29033 sensor");
        return SNS_DDF_EDEVICE;
    }
    return status;
}

/*==============================================================================

FUNCTION      sns_dd_als_isl29033_query

DESCRIPTION   Called by sns_dd_als_isl29033_get_attrib to get an attribute 
              value for ALS(light)
data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

==============================================================================*/
static sns_ddf_status_e sns_dd_als_isl29033_query
(
    sns_dd_als_state_t*  dd_ptr,
    sns_ddf_memhandler_s* memhandler,
    sns_ddf_attribute_e   attrib,
    void**                value,
    uint32_t*             num_elems
)      
{
    sns_ddf_power_info_s* power_ptr;

    switch(attrib)
    {
    case SNS_DDF_ATTRIB_RANGE:
        {
            sns_ddf_range_s *device_ranges;
            if((*value = sns_ddf_memhandler_malloc(memhandler,
                sizeof(sns_ddf_range_s)))== NULL)
            {
                return SNS_DDF_ENOMEM;
            }
            device_ranges = *value;
            device_ranges[0].min = FX_FLTTOFIX_Q16(0.0019);
            device_ranges[0].max = FX_FLTTOFIX_Q16(8000.0);
            *num_elems = 1;
            break;
        }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            sns_ddf_resolution_adc_s* resp;
            if((resp = sns_ddf_memhandler_malloc(memhandler,
                sizeof(sns_ddf_resolution_adc_s)))== NULL)
            {
                return SNS_DDF_ENOMEM;
            }
            *num_elems = 1;
            resp->bit_len = 16;
            resp->max_freq = SNS_DD_ISL_ALS_FREQ;
            *value = resp;
            break;
        }

    case SNS_DDF_ATTRIB_POWER_INFO:
        {
            if((power_ptr = sns_ddf_memhandler_malloc(memhandler,
                sizeof(sns_ddf_power_info_s))) == NULL)
            {
                return SNS_DDF_ENOMEM;
            }
            *num_elems = 1;
            power_ptr->active_current = SNS_DD_ALS_PWR;
            power_ptr->lowpower_current = SNS_DD_ALS_LOW_PWR;
            *(sns_ddf_power_info_s**)value = power_ptr;
            break;
        }

    case SNS_DDF_ATTRIB_RESOLUTION:
        {
            q16_t* resp;
            if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(q16_t))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }
            *num_elems = 1;
            *resp = ALS_DD_RES;
            *value = resp;
            break;
        }

    case SNS_DDF_ATTRIB_ODR:
        {
            uint32_t* odr_ptr;

            if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }
            *num_elems = 1;
            *odr_ptr = dd_ptr->sns_dd_als_db.als_odr;
            *(uint32_t **)value = odr_ptr;
            ISL29033_MSG_1(MED, "als_odr=%d", *odr_ptr);
            break;
        }

    case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
        {
            *value = &isl29033_odr_list;
            *num_elems = isl29033_odr_list_size;

            break;
        }

    default: 
        return SNS_DDF_EINVALID_ATTR;
    }
    return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION:   sns_dd_als_isl29033_get_attrib

==============================================================================*/
/*
@brief Called by the SMGR to retrieves the value of an attribute of the sensor

@detail Returns the requested attribute

@param[in]  handle      Handle to a driver instance.
@param[in]  sensor      Sensor whose attribute is to be retrieved.
@param[in]  attrib      Attribute to be retrieved. 
@param[in]  memhandler  Memory handler used to dynamically allocate 
output parameters, if applicable.
@param[out] value       Pointer that this function will allocate or set 
to the attribute's value.
@param[out] num_elems   Number of elements in @a value.

@return
Success if the attribute was retrieved and the buffer was 
populated. Otherwise a specific error code is returned.
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_als_isl29033_get_attrib
(
    sns_ddf_handle_t     handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    sns_ddf_memhandler_s* memhandler,
    void**                value,
    uint32_t*             num_elems
)
{
    sns_dd_als_state_t* dd_ptr = (sns_dd_als_state_t *)handle;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    bool generic_attrib = false;
    sns_ddf_driver_info_s *driver_info_ptr;

    sns_ddf_driver_info_s driver_info_l = {
        "ISL29033", /* name */
        1           /* version */
    };
    sns_ddf_device_info_s *device_info_ptr;
    sns_ddf_device_info_s device_info_l = {
        "ALS",      /* name */
        "Intersil", /* vendor */
        "ISL29033", /* model */
        1           /* version */
    };

    if ((handle == NULL) || (memhandler == NULL))
        return SNS_DDF_EINVALID_PARAM;

    ISL29033_MSG_2(HIGH,"sns_dd_als_isl29033_get_attrib sesnor=%d attrib=%d", sensor, attrib);

    /* check first if the query is for generic attributes */
    switch ( attrib )
    {
    case SNS_DDF_ATTRIB_DRIVER_INFO:
        {
            if ( (driver_info_ptr = sns_ddf_memhandler_malloc(memhandler, 
                sizeof(sns_ddf_driver_info_s))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }
            *driver_info_ptr = driver_info_l;
            *(sns_ddf_driver_info_s**)value = driver_info_ptr;
            *num_elems = 1;
            generic_attrib = true;
            break;
        }
    case SNS_DDF_ATTRIB_DEVICE_INFO:
        {
            if ( (device_info_ptr = sns_ddf_memhandler_malloc(memhandler, 
                sizeof(sns_ddf_device_info_s))) == NULL )
            {
                return SNS_DDF_ENOMEM;
            }
            *device_info_ptr = device_info_l;
            *(sns_ddf_device_info_s**)value = device_info_ptr; 
            *num_elems = 1;
            generic_attrib = true;
            break;
        }
    default:
        /* do nothing */
        break;
    }

    /* The query is not for generic attribute but is for specific data type */
    if ( generic_attrib != true )
    {
        switch ( sensor )
        {
        case SNS_DDF_SENSOR_AMBIENT:
        //case SNS_DDF_SENSOR__NONE:
            status = sns_dd_als_isl29033_query(dd_ptr, memhandler, attrib, value, num_elems);
            break;

        default:
            status = SNS_DDF_EINVALID_ATTR;
            break;
        }
    }
    return status;
}

/**
* @brief Runs a factory test case.
*  
* Tests may include embedded hardware tests in cases where the sensor 
* supports it, as well as driver based sensor tests. This is generally run 
* in a factory setting and must not be called while a device is streaming 
* data. 
*  
* @param[in]  dd_handle  Handle to a driver instance. 
* @param[in]  sensor     Sensor on which to run the test. 
* @param[in]  test       Test case to run. 
* @param[out] err        Optional driver-specific error code.
*  
* @return One of the following error codes:
*     SNS_DDF_SUCCESS        - Test passed.
*     SNS_DDF_PENDING        - Test result will be sent as an event.
*     SNS_DDF_EDEVICE_BUSY   - Device is busy streaming, cannot run test.
*     SNS_DDF_EINVALID_TEST  - Test is not defined for this sensor.
*     SNS_DDF_EINVALID_PARAM - One of the parameters is invalid.
*     SNS_DDF_EFAIL          - Unknown error occured.
*/
static sns_ddf_status_e sns_dd_als_isl29033_run_test
(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_test_e test,
    uint32_t* err
)
{

    sns_dd_als_state_t* dd_ptr = (sns_dd_als_state_t *)dd_handle;

    if ((dd_handle == NULL) 
         || (sensor != SNS_DDF_SENSOR_AMBIENT)
         || (test != SNS_DDF_TEST_SELF))
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    if(dd_ptr->sns_dd_als_db.mode == SNS_DD_DEV_ALS_CONT)
    {
        return SNS_DDF_EDEVICE_BUSY;
    }

    return sns_dd_als_isl29033_reset(dd_handle);
}

 /**
 * @brief Probes for the device with a given configuration.
 *
 * This commands the driver to look for the device with the specified
 * configuration (ie, I2C address/bus defined in the sns_ddf_device_access_s
 * struct.
 *
 * @param[in]  dev_info    Access info for physicol devices controlled by 
 *                         this driver. Used to determine if the device is
 *                         physically present.
 * @param[in]  memhandler  Memory handler used to dynamically allocate 
 *                         output parameters, if applicable.
 * @param[out] num_sensors Number of sensors supported. 0 if none.
 * @param[out] sensor_type Array of sensor types supported, with num_sensor
 *                         elements. Allocated by this function.
 *
 * @return SNS_DDF_SUCCESS if the part was probed function completed, even
 *         if no device was found (in which case num_sensors will be set to
 *         0).
 */
static sns_ddf_status_e sns_dd_als_isl29033_probe
(
    sns_ddf_device_access_s* device_info,
    sns_ddf_memhandler_s*    memhandler,
    uint32_t*                num_sensors,
    sns_ddf_sensor_e**       sensors
)
{
    sns_ddf_status_e status;
    sns_ddf_handle_t port_handle;
    uint8_t i2c_buff;
    uint8_t bytes_read;

    *num_sensors = 0;
    *sensors = NULL;

    status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
    if (status != SNS_DDF_SUCCESS) 
    {
        return status;
    }

    /* Read & Verify COMMAND1 */
    status = sns_ddf_read_port( port_handle,
        ALS_DD_ISL_COMMAND_REG_1,
        &i2c_buff,
        1,
        &bytes_read );

    if ((status != SNS_DDF_SUCCESS) || (bytes_read != 1))  
    {
        sns_ddf_close_port( port_handle );
        return status;
    }

    /* POR value of COMMAND1 is 0x00 */
    if (i2c_buff != 0x00) { 
        sns_ddf_close_port( port_handle );
        return SNS_DDF_SUCCESS;
    }

    /* Read & Verify INT HT low byte */
    status = sns_ddf_read_port( port_handle,
        ALS_DD_ISL_HIGH_THRES_LBYTE,
        &i2c_buff,
        1,
        &bytes_read );

    if ((status != SNS_DDF_SUCCESS) || (bytes_read != 1))  
    {
        sns_ddf_close_port( port_handle );
        return status;
    }

    /* POR value of INT HT low byte is 0xFF */
    if (i2c_buff != 0xFF) { 
        sns_ddf_close_port( port_handle );
        return SNS_DDF_SUCCESS;
    }

    /* Registers registers are correct. This is ISL29033. */
    *num_sensors = 2;
    *sensors = sns_ddf_memhandler_malloc( memhandler,
        sizeof(sns_ddf_sensor_e) * *num_sensors );

    if ( *sensors != NULL )
    {
        (*sensors)[0] = SNS_DDF_SENSOR__NONE;
        (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;

        status = SNS_DDF_SUCCESS;
    } 
    else 
    {
        status = SNS_DDF_ENOMEM;
    }

    sns_ddf_close_port( port_handle );
    return status;
}

 /**
 * @brief Begins device-scheduled sampling and enables notification via Data 
 *        Ready Interrupts (DRI).
 *
 * The driver commands the device to begin sampling at the configured 
 * ODR (@a SNS_DDF_ATTRIB_ODR) and enables DRI. When data is ready, the 
 * driver's handle_irq() function is called and the driver notifies 
 * SMGR of the event via @a sns_ddf_smgr_notify_event() and @a 
 * SNS_DDF_EVENT_DATAREADY. 
 *  
 * @param[in] handle  Handle to the driver's instance.
 * @param[in] sensor  Sensor to be sampled.
 * @param[in] enable  True to enable or false to disable data stream.
 * 
 * @return SNS_DDF_SUCCESS if sensor was successfully configured and 
 *         internal sampling has commenced or ceased. Otherwise an
 *         appropriate error code.
 */
static sns_ddf_status_e sns_dd_als_isl29033_enable_sched_data
(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    bool              enable
)
{
    sns_dd_als_state_t* dd_ptr = (sns_dd_als_state_t *)dd_handle;

    if ((dd_ptr == NULL) 
         || (sensor != SNS_DDF_SENSOR_AMBIENT)
         || (dd_ptr->gpio1 == 0xFFFF))
    {
       ISL29033_MSG_3(ERROR,"gpio=%d, sensor=%d, dd_ptr=%X", dd_ptr->gpio1, sensor, dd_ptr);
        return SNS_DDF_EINVALID_PARAM;
    }

    ISL29033_MSG_1(HIGH,"sns_dd_als_isl29033_enable_sched_data enable=%d", enable);

    if(dd_ptr->sns_dd_als_common_db.state != SNS_DD_DEV_CONFIGURED){
        ISL29033_MSG_0(ERROR,"Device is not configured");
        return SNS_DDF_EFAIL;
    }

    if(enable == true)
    {
        dd_ptr->dri_enable = true;
        sns_dd_als_isl29033_set_threshold(dd_ptr, 0xFFFF, 0x0000);
        return sns_ddf_signal_register(dd_ptr->gpio1, dd_handle, 
                                       &sns_als_isl29033_driver_fn_list, 
                                       SNS_DDF_SIGNAL_IRQ_FALLING);
    }
    else
    {
        dd_ptr->dri_enable = false;
        sns_dd_als_isl29033_set_threshold(dd_ptr, 0x0000, 0xFFFF);
        return sns_ddf_signal_deregister(dd_ptr->gpio1);
    }
}

sns_ddf_driver_if_s sns_als_isl29033_driver_fn_list = {
    &sns_dd_als_isl29033_init,
    &sns_dd_als_isl29033_get_data,
    &sns_dd_als_isl29033_set_attrib,
    &sns_dd_als_isl29033_get_attrib,
    &sns_dd_als_isl29033_handle_timer,
    &sns_dd_als_isl29033_handle_irq,
    &sns_dd_als_isl29033_reset,
    &sns_dd_als_isl29033_run_test,
    &sns_dd_als_isl29033_enable_sched_data,
    &sns_dd_als_isl29033_probe
};
