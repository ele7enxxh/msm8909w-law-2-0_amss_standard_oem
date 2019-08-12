/********************************************************************************
* Copyright (c) 2014, PixArt Imaging Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* 3. Neither the name of the PixArt Imaging Inc. nor the
* names of its contributors may be used to endorse or promote products
* derived from this software without specific prior written permission.
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
*******************************************************************************/
/*============================================================================

                                INCLUDE FILES

============================================================================*/
#include <stdio.h>
#include "fixed_point.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "qurt_elite_diag.h"

#include "sns_dd_pah8002.h"
#ifdef PXIALG_INSIDE
#include "pxialg.h"
#endif

#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"

#ifdef CONFIG_LGE
#include "DDITlmm.h"
#endif

// enable this macro to view debug messages
#define PAH8002_DEBUG


#ifdef PAH8002_DEBUG
#if defined QDSP6
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define PAH8002_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg)
#define PAH8002_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1)
#define PAH8002_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1,p2)
#define PAH8002_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1,p2,p3)
#define PAH8002_MSG_4(level,msg,p1,p2,p3,P4) MSG_4(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1,p2,p3,P4)
#else
#define MED MEDIUM
#include "sns_debug_str.h"
#define PAH8002_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define PAH8002_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
#define PAH8002_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
#define PAH8002_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
#define PAH8002_MSG_4(level,msg,p1,p2,p3,p4) SNS_PRINTF_STRING_ID_##level##_4(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3,p4)
#endif
#else /* AD7146_DEBUG */
#define PAH8002_MSG_0(level,msg)
#define PAH8002_MSG_1(level,msg,p1)
#define PAH8002_MSG_2(level,msg,p1,p2)
#define PAH8002_MSG_3(level,msg,p1,p2,p3)
#define PAH8002_MSG_3(level,msg,p1,p2,p3,p4)
#endif /* PAH8002_DEBUG */

/*
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define PAH8002_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, " - " msg)
#define PAH8002_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, " - " msg,p1)
#define PAH8002_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, " - " msg,p1,p2)
#define PAH8002_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, " - " msg,p1,p2,p3)
*/

/*=======================================================================

  Preprocessor Definitions and Constants

  ========================================================================*/
#define PPG_RESOLUTION 1
#define PPG_MAX_RANGE (0xFFFFFFFF)
//#define PXIALG_INSIDE

// Polling mode
#define HEART_RATE_MODE_SAMPLES_PER_READ 4     //for 20 ODR 4 channels - 200ms
#define STRESS_MODE_SAMPLES_PER_READ     4     //for 200 ODR

#define MAX_SAMPLES_PER_READ (HEART_RATE_MODE_SAMPLES_PER_READ > STRESS_MODE_SAMPLES_PER_READ ? HEART_RATE_MODE_SAMPLES_PER_READ : STRESS_MODE_SAMPLES_PER_READ)
#define TOTAL_CHANNELS 4
#define MAX_SAMPLES_PER_CH_READ (MAX_SAMPLES_PER_READ/TOTAL_CHANNELS)
#define HEART_RATE_MODE_SAMPLES_PER_CH_READ (HEART_RATE_MODE_SAMPLES_PER_READ/TOTAL_CHANNELS)

#define MAX_FIFO_READ_SIZE 832
/*=======================================================================

  INTERNAL ENUMS

  ========================================================================*/

typedef enum
{
  SNS_DDF_PPG_ERROR_1=1,
  SNS_DDF_PPG_ERROR_2,
  SNS_DDF_PPG_ERROR_3,
  SNS_DDF_PPG_ERROR_4,
  SNS_DDF_PPG_ERROR_5,
  SNS_DDF_PPG_ERROR_6,
  SNS_DDF_PPG_ERROR_7,
  SNS_DDF_PPG_ERROR_8,
  SNS_DDF_PPG_ERROR_9,
  SNS_DDF_PPG_ERROR_10,
  SNS_DDF_PPG_ERROR_11,
  SNS_DDF_PPG_ERROR_12,
  SNS_DDF_PPG_ERROR_13,
  SNS_DDF_PPG_ERROR_14,
  SNS_DDF_PPG_ERROR_15,
  SNS_DDF_PPG_ERROR_16,
  SNS_DDF_PPG_ERROR_17,
  SNS_DDF_PPG_ERROR_18,
  SNS_DDF_PPG_ERROR_19,
  SNS_DDF_PPG_ERROR_20,
  SNS_DDF_PPG_ERROR_21,
  SNS_DDF_PPG_ERROR_22,
  SNS_DDF_PPG_ERROR_23,
  SNS_DDF_PPG_ERROR_24,
  SNS_DDF_PPG_ERROR_25,
  SNS_DDF_PPG_ERROR_26
}SNS_DDF_PPG_ERROR_TYPE;

/*=======================================================================

  INTERNAL FUNCTION PROTOTYPES

  ========================================================================*/
static sns_ddf_status_e sns_dd_pixart_ppg_init(
        sns_ddf_handle_t*        dd_handle_ptr,
        sns_ddf_handle_t         smgr_handle,
        sns_ddf_nv_params_s*     nv_params,
        sns_ddf_device_access_s  device_info[],
        uint32_t                 num_devices,
        sns_ddf_memhandler_s*    memhandler,
        sns_ddf_sensor_e*        sensors[],
        uint32_t*                num_sensors);

static sns_ddf_status_e sns_dd_pixart_ppg_get_data(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s*   memhandler,
        sns_ddf_sensor_data_s*  data[]);

static sns_ddf_status_e sns_dd_pixart_ppg_set_attr(
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        sns_ddf_attribute_e  attrib,
        void*                value);

static sns_ddf_status_e sns_dd_pixart_ppg_get_attr(
        sns_ddf_handle_t       dd_handle,
        sns_ddf_sensor_e       sensor,
        sns_ddf_attribute_e    attrib,
        sns_ddf_memhandler_s*  memhandler,
        void**                 value,
        uint32_t*              num_elems);

static sns_ddf_status_e sns_dd_pixart_run_test(
        sns_ddf_handle_t  dd_handle,
        sns_ddf_sensor_e  sensor,
        sns_ddf_test_e    test,
        uint32_t*         err);

static sns_ddf_status_e sns_dd_pixart_probe(
        sns_ddf_device_access_s* device_info,
        sns_ddf_memhandler_s*    memhandler,
        uint32_t*                num_sensors,
        sns_ddf_sensor_e**       sensors );

static sns_ddf_status_e sns_dd_pixart_trigger_fifo_data(
    sns_ddf_handle_t         dd_handle,
    sns_ddf_sensor_e         sensor,
    uint16_t                 num_samples,
    bool                     trigger_now);
static void sns_dd_pixart_timer_handler(sns_ddf_handle_t dd_handle, void* arg);
static sns_ddf_status_e sns_dd_pixart_ppg_reset(sns_ddf_handle_t dd_handle);

static void sns_dd_pixart_ppg_irq_handler(
        sns_ddf_handle_t dd_handle,
        //sns_ddf_signal_irq_e irq);
        uint32_t gpio_num, sns_ddf_time_t timestamp);

static sns_ddf_status_e sns_dd_pixart_suspend(sns_ddf_handle_t dd_handle);
static sns_ddf_status_e sns_dd_pixart_resume(sns_ddf_handle_t dd_handle);
static sns_ddf_status_e sns_dd_pixart_enable_sched_data(sns_ddf_handle_t  dd_handle, sns_ddf_sensor_e  sensor, bool enable);

static bool pah8002_sw_reset(sns_ddf_handle_t dd_handle);
static bool pah8002_start(sns_ddf_handle_t dd_handle);
//static bool pah8002_stop(sns_ddf_handle_t dd_handle);
static bool pah8002_touch_mode_init(sns_ddf_handle_t dd_handle);
static bool pah8002_normal_mode_init(sns_ddf_handle_t dd_handle);
static bool pah8002_stress_mode_init(sns_ddf_handle_t dd_handle);
static sns_ddf_status_e pah8002_read_reg(sns_ddf_handle_t dd_handle, uint8_t addr, uint8_t *data);
static sns_ddf_status_e pah8002_burst_read_reg(sns_ddf_handle_t dd_handle, uint8_t addr, uint8_t *data, uint16_t num);
static sns_ddf_status_e pah8002_write_reg(sns_ddf_handle_t dd_handle, uint8_t addr, uint8_t data);

static bool pah8002_task(sns_ddf_handle_t dd_handle);
static bool pah8002_enter_touch_mode(sns_ddf_handle_t dd_handle);
static bool pah8002_enter_normal_mode(sns_ddf_handle_t dd_handle);
static bool pah8002_enter_stress_mode(sns_ddf_handle_t dd_handle);
//static bool pah8002_get_touch_flag(sns_ddf_handle_t dd_handle, uint8_t *touch_flag);
static int pah8002_wakeup(sns_ddf_handle_t dd_handle);
//static int pah8002_check(sns_ddf_handle_t dd_handle);

/*=======================================================================

  STRUCTURE DEFINITIONS

  ========================================================================*/
//sns_ddf_driver_if_s sns_pixart_ppg_driver_fn_list =

// Forward declaration
sns_ddf_driver_if_s sns_dd_pah8002_if =
{
    &sns_dd_pixart_ppg_init,
    &sns_dd_pixart_ppg_get_data,
    &sns_dd_pixart_ppg_set_attr,
    &sns_dd_pixart_ppg_get_attr,
    NULL, // &sns_dd_pixart_timer_handler,
    &sns_dd_pixart_ppg_irq_handler, /* handle irq */
    &sns_dd_pixart_ppg_reset,
    &sns_dd_pixart_run_test, /* self test */
    &sns_dd_pixart_enable_sched_data,
#ifndef DDK
    &sns_dd_pixart_probe,
#endif
    &sns_dd_pixart_trigger_fifo_data
};

/* State struct for pixart_ppg driver */
#define HRD_DATA_LENGTH 13
#define MEMS_DATA_LENGTH 1
#define RAW_DATA_LENGTH 3

typedef struct {
    sns_ddf_handle_t smgr_hndl;    /* SDDI handle used to notify_data */
    sns_ddf_handle_t port_handle; /* handle used to access the I2C bus */
    uint32_t odr_touch;
    uint32_t odr_ppg;
    uint8_t pah8002_ppg_data[MAX_FIFO_READ_SIZE] ;
    uint8_t pah8002_touch_flag ;
    uint8_t pah8002_frame_count;
    sns_ddf_time_t pah8002_previous_timestamp;

    uint32_t interrupt_gpio;
    sns_ddf_sensor_data_s  sensor_data; /*for DRI*/
    uint8_t ppg_int_enable;
    /* Touch */
    sns_ddf_sensor_data_s touch_data;
    sns_ddf_signal_irq_trigger_e rising_falling;
    bool int_registered;

    uint8_t op_mode ;
#ifdef PXIALG_INSIDE
    pah8002_data_t pah8002_data;
    int16_t mems_data[MAX_SAMPLES_PER_CH_READ * 3];
#endif

    sns_ddf_timer_s timer;
    uint8_t u8IRDAC;//IR Led DAC
    uint8_t u8IRET;//IR Exposure Time
    uint8_t chip_id;
    uint16_t water_mark;
    uint8_t last_ppg_touch_status;

} sns_dd_pixart_ppg_state_t;

enum {
    OP_MODE_NONE = 0,
    OP_MODE_TOUCH,
    OP_MODE_PPG,
    OP_MODE_STRESS,
};

/*============================================================================

  Global Data Definitions

  ============================================================================*/

static sns_ddf_sensor_e my_sensors[] =
{
    SNS_DDF_SENSOR_HEART_RATE_RAW,   //for PPG mode
    SNS_DDF_SENSOR_HEART_RATE,       //for Touch mode
};

/*===========================================================================

FUNCTION:   sns_dd_pah8002_log

===========================================================================*/

void sns_dd_pah8002_log(
        sns_dd_pixart_ppg_state_t* dd_ptr,
        sns_ddf_sensor_e       sensor_type,
        uint32_t               data1,
        uint32_t               data2,
        uint32_t               data3)
{
    sns_err_code_e err_code;
    sns_log_sensor_data_pkt_s* log_struct_ptr;

    /* Allocate log packet */
    err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
               sizeof(sns_log_sensor_data_pkt_s),
               (void**)&log_struct_ptr);

    PAH8002_MSG_1(HIGH, ">>> sns_dd_pah8002_log err_code = %d\r\n",err_code);
    if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
    {
        log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
        log_struct_ptr->sensor_id = sensor_type;
        log_struct_ptr->vendor_id = SNS_DDF_VENDOR_PIXART;

        /* Timestamp the log with sample time */
        log_struct_ptr->timestamp = sns_ddf_get_timestamp();

        log_struct_ptr->num_data_types = 3;
        log_struct_ptr->data[0] = data1;
        log_struct_ptr->data[1] = data2;
        log_struct_ptr->data[2] = data3;

        PAH8002_MSG_0(HIGH, ">>> sns_logpkt_commit \r\n");
        /* Commit log (also frees up the log packet memory) */
        err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, log_struct_ptr);
    }
}


/*===========================================================================

FUNCTION:   sns_dd_pixart_ppg_reset

===========================================================================*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_pixart_ppg_reset(sns_ddf_handle_t dd_handle)
{
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;

    //Bank0
    PAH8002_MSG_0(HIGH, ">>> sns_dd_pixart_ppg_reset \r\n");

    //soft reset
    //if( !pah8002_sw_reset(dd_handle) )
    //{
    //    return SNS_DDF_EFAIL ;
    //}

    state->pah8002_frame_count = 0 ;
    state->pah8002_previous_timestamp = 0 ;
    state->int_registered = 0 ;
    state->op_mode = OP_MODE_NONE ;

    return sns_dd_pixart_suspend(dd_handle);

    //return SNS_DDF_SUCCESS ;
}

/*===========================================================================

FUNCTION:   sns_dd_pixart_ppg_init

===========================================================================*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_pixart_ppg_init(
        sns_ddf_handle_t*        dd_handle_ptr,
        sns_ddf_handle_t         smgr_handle,
        sns_ddf_nv_params_s*     nv_params,
        sns_ddf_device_access_s  device_info[],
        uint32_t                 num_devices,
        sns_ddf_memhandler_s*    memhandler,
        sns_ddf_sensor_e*        sensors[],
        uint32_t*                num_sensors
        )
{
    sns_ddf_status_e stat = SNS_DDF_EFAIL;
    sns_dd_pixart_ppg_state_t *ptr;
    uint32_t i = 0 ;
    PAH8002_MSG_2(HIGH, ">> sns_dd_pixart_ppg_init %d, %d\r\n", SNS_DDF_SENSOR_HEART_RATE_RAW, SNS_DDF_SENSOR_HEART_RATE);

    for(i=0; i<*num_sensors; i++)
    {
      PAH8002_MSG_2(HIGH, "sensor INPUT<%d> %d\r\n", i, sensors[i]);
    }
    if(sns_ddf_malloc((void **)&ptr, sizeof(sns_dd_pixart_ppg_state_t)) != SNS_DDF_SUCCESS)
    {
        return SNS_DDF_ENOMEM;
    }
    ptr->smgr_hndl = smgr_handle;

    stat = sns_ddf_malloc((void**)&(ptr->sensor_data.samples),sizeof(sns_ddf_sensor_sample_s)*(3)); /* +1 for temperature */
    if(stat != SNS_DDF_SUCCESS){
        return stat;
    }

    stat = sns_ddf_malloc((void**)&(ptr->touch_data.samples),sizeof(sns_ddf_sensor_sample_s)*(1));
    if(stat != SNS_DDF_SUCCESS){
        return stat;
    }

    if((stat = sns_ddf_open_port(&(ptr->port_handle) , &(device_info->port_config)))!= SNS_DDF_SUCCESS)
    {
        return stat;
    }

//    initialize timer
//    if((stat = sns_ddf_timer_init(&(ptr->timer), ptr, &sns_dd_pah8002_if, NULL, false) )  != SNS_DDF_SUCCESS)
//    {
//        return stat ;
//    }


    /* Fill out supported sensor info */
    *num_sensors = 2;
    *sensors = my_sensors;
    *dd_handle_ptr = (sns_ddf_handle_t)ptr;

    ptr->interrupt_gpio = device_info->first_gpio;
    ptr->ppg_int_enable = 0;
    ptr->rising_falling = SNS_DDF_SIGNAL_IRQ_RISING;
    ptr->chip_id = 0;
    ptr->water_mark = 0 ;
    ptr->last_ppg_touch_status = -1;
    if( (stat = pah8002_wakeup(ptr)) != SNS_DDF_SUCCESS)
    {
        return stat ;
    }

#ifdef PXIALG_INSIDE
    ptr->pah8002_data.frame_count = 0;
    ptr->pah8002_data.nf_ppg_channel = TOTAL_CHANNELS;
    ptr->pah8002_data.nf_ppg_per_channel = HEART_RATE_MODE_SAMPLES_PER_CH_READ;
    ptr->pah8002_data.ppg_data = (int32_t *)ptr->pah8002_ppg_data;
    ptr->pah8002_data.nf_mems = sizeof(ptr->mems_data)/(sizeof(int16_t) * 3) ;
    ptr->pah8002_data.mems_data = ptr->mems_data;
    memset(ptr->mems_data, 0, sizeof(ptr->mems_data));
#endif

	sns_dd_pixart_suspend( (sns_ddf_handle_t)ptr );

    return stat;
}

/*===========================================================================

FUNCTION:   sns_dd_pixart_ppg_set_attr

===========================================================================*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_pixart_ppg_set_attr (
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        sns_ddf_attribute_e  attrib,
        void*                value)
{
    sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    PAH8002_MSG_2(HIGH, "set_attr %d - Sensor: %d", attrib,sensor);
    switch(attrib)
    {
       case SNS_DDF_ATTRIB_FIFO:
        /* Set FIFO watermark */
       {
           sns_ddf_fifo_set_s wm = *(sns_ddf_fifo_set_s *)value;
           state->water_mark = wm.watermark;
           PAH8002_MSG_1(HIGH, "FIFO water_mark %d",  state->water_mark);
       }
       break;
       case SNS_DDF_ATTRIB_POWER_STATE:
       {
           sns_ddf_powerstate_e *mode = (sns_ddf_powerstate_e *)value ;
           switch (*mode)
           {
             case SNS_DDF_POWERSTATE_ACTIVE:
             {
               return sns_dd_pixart_resume(dd_handle);
             }
             break;
             case SNS_DDF_POWERSTATE_LOWPOWER:
             {
               return sns_dd_pixart_suspend(dd_handle);
             }
             break;
             default:
             return SNS_DDF_EINVALID_PARAM;
           }
       }
       break;
       case SNS_DDF_ATTRIB_RANGE:
           break;
       case SNS_DDF_ATTRIB_LOWPASS:
           break;
       case SNS_DDF_ATTRIB_RESOLUTION_ADC:
           break;
       case SNS_DDF_ATTRIB_MOTION_DETECT:
           break;
       case SNS_DDF_ATTRIB_ODR:
       {
         uint32_t odr = *(uint32_t *)value;
         PAH8002_MSG_1(HIGH, "PAH8002 :: SNS_DDF_ATTRIB_ODR %d", odr);
         if(sensor == SNS_DDF_SENSOR_HEART_RATE)
         {
           if(odr > 0)
           {
             state->odr_touch = odr;
             state->op_mode = OP_MODE_TOUCH;
           }
           else
           {
              state->odr_touch  = 0 ;
           }
         }
         else if(sensor == SNS_DDF_SENSOR_HEART_RATE_RAW)
         {
           if(odr == 200)
           {
             state->odr_ppg = odr;
             state->op_mode = OP_MODE_STRESS;
           }
           else if(odr > 0)
           {
             state->odr_ppg = 20;
             state->op_mode = OP_MODE_PPG;
           }
           else
           {
             state->odr_ppg  = 0 ;
           }
         }
       break;
       }
       default:
       return SNS_DDF_EINVALID_PARAM;
     }
     return(ret_val);
}


/*===========================================================================

FUNCTION:   sns_dd_pixart_ppg_get_data

===========================================================================*/
/*!
  @brief Called by the SMGR to get data

  @detail
  - Requests a single sample of sensor data from each of the specified
  sensors. Data is returned immediately after being read from the
  sensor, in which case data[] is populated in the same order it was
  requested,

  @param[in]  dd_handle    Handle to a driver instance.
  @param[in] sensors       List of sensors for which data isrequested.
  @param[in] num_sensors   Length of @a sensors.
  @param[in] memhandler    Memory handler used to dynamically allocate
  output parameters, if applicable.
  @param[out] data         Sampled sensor data. The number of elements
  must match @a num_sensors.

  @return
  The error code definition within the DDF
  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Update the each sample status after a check for bound is done
 */
/*=========================================================================*/
static sns_ddf_status_e sns_dd_pixart_ppg_get_data (
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s*   memhandler,
        sns_ddf_sensor_data_s*  data[])
{
    //report raw data through irq handler
    return SNS_DDF_PENDING ;
}

/*===========================================================================

FUNCTION:   sns_dd_pixart_ppg_get_attr

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.

  @detail
  - range and resolution info is from the device data sheet.

  @param[in]  dd_handle   Handle to a driver instance.
  @param[in] sensor       Sensor whose attribute is to be retrieved.
  @param[in]  attrib      Attribute to be retrieved.
  @param[in] memhandler  Memory handler used to dynamically allocate
  output parameters, if applicable.
  @param[out] value      Pointer that this function will allocate or set
  to the attribute's value.
  @param[out] num_elems  Number of elements in @a value.

  @return
  The error code definition within the DDF
  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
 */
/*=========================================================================*/

static sns_ddf_status_e sns_dd_pixart_ppg_get_attr (
        sns_ddf_handle_t       dd_handle,
        sns_ddf_sensor_e       sensor,
        sns_ddf_attribute_e    attrib,
        sns_ddf_memhandler_s*  memhandler,
        void**                 value,
        uint32_t*              num_elems)
{
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    PAH8002_MSG_2(HIGH, "Get_attr %d - Sensor: %d", attrib,sensor);
    switch(attrib)
    {
      case SNS_DDF_ATTRIB_FIFO:
      {
          sns_ddf_fifo_attrib_get_s *fifo_info;
          PAH8002_MSG_0(HIGH, "SNS_DDF_ATTRIB_FIFO");
          if ((fifo_info = (sns_ddf_fifo_attrib_get_s *)sns_ddf_memhandler_malloc_ex(
                            memhandler,sizeof(sns_ddf_fifo_attrib_get_s) , state->smgr_hndl)) == NULL)
          {
            return SNS_DDF_ENOMEM;
          }
          fifo_info->is_supported = true;
          fifo_info->is_sw_watermark = false;
          fifo_info->max_watermark = 50; // = 2 green-channel data. Total max 200(800Bytes) samples (IR + Green).FIFO depth in number of sensor samples.
          fifo_info->share_sensor_cnt = 0;
          fifo_info->share_sensors[0] = NULL;
          *value = fifo_info;
      }
      break;
      case SNS_DDF_ATTRIB_POWER_INFO:
      {
          //TODO - check with IC spec
          sns_ddf_power_info_s* power_attrib;
          if( (*value = sns_ddf_memhandler_malloc(memhandler,
                                sizeof(sns_ddf_power_info_s))) == NULL)
          {
              return SNS_DDF_ENOMEM;
          }
          power_attrib = *value;
          power_attrib->active_current = 1500;    //1500uA
          power_attrib->lowpower_current = 15;    //15uA
          *num_elems = 1;
      }
      break;
      case SNS_DDF_ATTRIB_RANGE:
      {
          //TODO - check with IC spec
          sns_ddf_range_s *device_ranges;
          if( (*value = sns_ddf_memhandler_malloc(memhandler,
                         sizeof(sns_ddf_range_s))) == NULL)
          {
            return SNS_DDF_ENOMEM;
          }
          device_ranges = *value;
          *num_elems = 1;
          device_ranges->min = 0;
          device_ranges->max = FX_FLTTOFIX_Q16(PPG_MAX_RANGE);//FX_CONV_Q16(PPG_MAX_RANGE, 16);
      }
      break;
      case SNS_DDF_ATTRIB_RESOLUTION_ADC:
      {
        //TODO - check with IC spec
        sns_ddf_resolution_adc_s *device_res;
        if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_resolution_adc_s))) == NULL)
        {
          return SNS_DDF_ENOMEM;
        }
        device_res = *value;
        *num_elems = 1;
        device_res->bit_len = 32;
        device_res->max_freq = 200;
      }
      break;
      case SNS_DDF_ATTRIB_RESOLUTION:
      {
        //TODO - check with IC spec
        sns_ddf_resolution_t *device_res;
        if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_resolution_t))) == NULL)
        {
          return SNS_DDF_ENOMEM;
        }
        device_res = *value;
        *num_elems = 1;
        *device_res = FX_FLTTOFIX_Q16(PPG_MAX_RANGE);//FX_CONV_Q16(PPG_MAX_RANGE, 16);
      }
      break;
      case SNS_DDF_ATTRIB_DELAYS:
      {
              //TODO - check with IC spec
              sns_ddf_delays_s *device_delay;
              if( (*value = sns_ddf_memhandler_malloc(memhandler,
                              sizeof(sns_ddf_delays_s))) == NULL)
              {
                  return SNS_DDF_ENOMEM;
              }
              device_delay = *value;
              *num_elems = 1;
              //Not clear what this should be!
              //TODO -
              device_delay->time_to_active = 2000; //2ms is the startup time
              device_delay->time_to_data = 50000;    //20Hz, 50000us
      }
      break;
      case SNS_DDF_ATTRIB_DRIVER_INFO:
      {
              sns_ddf_driver_info_s *driver_info;
              if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_driver_info_s))) == NULL)
              {
                  return SNS_DDF_ENOMEM;
              }
              driver_info = *value;
              *num_elems = 1;
              //      driver_info->name = "Pixart PPG";
              driver_info->name = ((sensor == SNS_DDF_SENSOR_HEART_RATE) ? "Pixart PPG" : "Pixart PPG Raw");
              driver_info->version = 1;
      }
      break;
      case SNS_DDF_ATTRIB_DEVICE_INFO:
      {
              PAH8002_MSG_0(HIGH, "get_attr SNS_DDF_ATTRIB_DEVICE_INFO");
              sns_ddf_device_info_s *device_info;
              if( (*value = sns_ddf_memhandler_malloc(memhandler,sizeof(sns_ddf_device_info_s))) == NULL)
              {
                  return SNS_DDF_ENOMEM;
              }
              device_info = *value;
              *num_elems = 1;
              device_info->model = "PIXART_PPG#";
              device_info->vendor = "Pixart";
              //device_info->name = "pixart_ppg";
              device_info->name = ((sensor == SNS_DDF_SENSOR_HEART_RATE) ? "pixart_ppg" : "pixart_ppg_raw");
              device_info->version = state->chip_id >> 4;
      }
      break;
      case SNS_DDF_ATTRIB_ODR:
      {
              uint32_t *odr_ptr = &state->odr_touch;
              if(sensor == SNS_DDF_SENSOR_HEART_RATE) {
                  odr_ptr = &state->odr_touch;
              } else if (sensor == SNS_DDF_SENSOR_HEART_RATE_RAW) {
                  odr_ptr = &state->odr_ppg;
              }
              *value = odr_ptr;
              *num_elems = 1;
              PAH8002_MSG_1(HIGH, "odr = %d", *(uint32_t *)(*value));
      }
      break;
      case SNS_DDF_ATTRIB_LOWPASS:
      case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
      {
              sns_ddf_odr_t* odr_list = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_odr_t)*2);
              uint16_t num_odrs = 0;
              if (odr_list == NULL)
                  return SNS_DDF_ENOMEM;
              odr_list[num_odrs++] = 1;
              odr_list[num_odrs++] = 10;
              odr_list[num_odrs++] = 20;
              odr_list[num_odrs++] = 200;
              *value = odr_list;
              *num_elems = num_odrs;
              break;
      }
      default:
      return SNS_DDF_EINVALID_PARAM;
  }

    return SNS_DDF_SUCCESS;
}

static sns_ddf_status_e sns_dd_pixart_suspend(sns_ddf_handle_t dd_handle)
{
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    int i = 0 ;
    int errorcode;
    PAH8002_MSG_0(HIGH, ">> sns_dd_pixart_suspend");
    state->odr_touch = 0 ;
    state->odr_ppg = 0 ;

    if( !pah8002_sw_reset(dd_handle))
    {
        errorcode = SNS_DDF_PPG_ERROR_2;
        goto RTN;
    }
    if(pah8002_wakeup(dd_handle) == SNS_DDF_EFAIL)
    {
        errorcode = SNS_DDF_PPG_ERROR_3;
        goto RTN;
    }
    PAH8002_MSG_1(MED, "CHIP_ID: %x",state->chip_id);

    for(i = 0; i < REVD_SUSPEND_REG_ARRAY_SIZE;i++)
    {
        if ( pah8002_write_reg(dd_handle,
                    revd_suspend_register_array[i][0],
                    revd_suspend_register_array[i][1]) != 0 )
        {
            PAH8002_MSG_1(ERROR, "sns_dd_pixart_suspend FAIL!! i=%d", i);
            return SNS_DDF_EFAIL;
        }
    }
//  pah8002_check(dd_handle);
    PAH8002_MSG_0(HIGH, "<< sns_dd_pixart_suspend SUCCESS");
    return SNS_DDF_SUCCESS;
    RTN:
    PAH8002_MSG_1(HIGH, "<< FAIL!!! pah8002_sw_reset error %d\r\n",errorcode);
    return SNS_DDF_EFAIL;
}

static sns_ddf_status_e sns_dd_pixart_resume(sns_ddf_handle_t dd_handle)
{
    PAH8002_MSG_0(HIGH, ">> sns_dd_pixart_resume START");
    //pah8002_stop(dd_handle);
    PAH8002_MSG_0(HIGH, "<< sns_dd_pixart_resume SUCCESS");
    return SNS_DDF_SUCCESS;
}
/*
void sns_dd_pixart_timer_handler(sns_ddf_handle_t dd_handle, void* arg)
{

}
*/

/**
 * @brief Called by SMGR to start sensor test
 *
 * @see sns_ddf_driver_if_s.run_test()
 */
sns_ddf_status_e sns_dd_pixart_run_test(
        sns_ddf_handle_t  dd_handle,
        sns_ddf_sensor_e  sensor,
        sns_ddf_test_e    test,
        uint32_t*         err)
{
    sns_ddf_status_e status;
    if(test == SNS_DDF_TEST_CONNECTIVITY)
    {
        status = sns_dd_pixart_ppg_reset(dd_handle);
    }
    else if(test == SNS_DDF_TEST_OEM)
    {
        status = sns_dd_pixart_ppg_reset(dd_handle);
    }
    else
    {
        status = sns_dd_pixart_ppg_reset(dd_handle);
    }
    return status;
}

static sns_ddf_status_e sns_dd_pixart_probe(
        sns_ddf_device_access_s* device_info,
        sns_ddf_memhandler_s*    memhandler,
        uint32_t*                num_sensors,
        sns_ddf_sensor_e**       sensors )
{
    sns_ddf_status_e stat = SNS_DDF_EFAIL;
    sns_ddf_handle_t port_handle; /* handle used to access the I2C bus */
    uint8_t reg = 0 , out;

    PAH8002_MSG_0(HIGH, "sns_dd_pixart_probe");
    if((stat = sns_ddf_open_port(&(port_handle) , &(device_info->port_config)))!= SNS_DDF_SUCCESS)
    {
        return stat;
    }
    reg = 0;
    sns_ddf_write_port(    port_handle,
            0x7F, //bank 0
            &reg,
            1,
            &out);
    sns_ddf_read_port(    port_handle,
            0x0,
            &reg,
            1,
            &out);

    PAH8002_MSG_1(HIGH, "ID = %d", reg);
    if(reg != 0x02)
    {
        *num_sensors = 0 ;
    }
    else
    {
        *num_sensors = 2;
        *sensors = my_sensors;
    }
    if((stat = sns_ddf_close_port(port_handle))!= SNS_DDF_SUCCESS)
    {
        return stat;
    }
    return SNS_DDF_SUCCESS;
}

/*===========================================================================
  FUNCTION      sns_dd_pixart_ppg_enable_interrupt
  ===========================================================================*/
static sns_ddf_status_e sns_dd_pixart_ppg_enable_interrupt(
        sns_ddf_handle_t  handle)
{
    sns_dd_pixart_ppg_state_t* dd_ptr = (sns_dd_pixart_ppg_state_t*)handle;
    sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;
    PAH8002_MSG_1(MED, "###sns_dd_pixart_ppg_enable_interrupt### %d",dd_ptr->int_registered);
    if(!dd_ptr->int_registered)
    {
        ret_val = sns_ddf_signal_register( dd_ptr->interrupt_gpio,
                (sns_ddf_handle_t)dd_ptr,
                &sns_dd_pah8002_if,
                dd_ptr->rising_falling);
        if(ret_val != SNS_DDF_SUCCESS)
        {
            PAH8002_MSG_0(MED, "###sns_ddf_signal_register Fail###");
            return ret_val;
        }
        dd_ptr->int_registered = true;
        PAH8002_MSG_0(MED, "###sns_ddf_signal_register###");
    }
    return ret_val;
}

/*===========================================================================
  FUNCTION      sns_dd_pixart_ppg_disable_interrupt
  ===========================================================================*/
static sns_ddf_status_e sns_dd_pixart_ppg_disable_interrupt(
        sns_ddf_handle_t  handle)
{
    sns_dd_pixart_ppg_state_t* dd_ptr = (sns_dd_pixart_ppg_state_t*)handle;
    sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;
    PAH8002_MSG_1(MED, "###sns_dd_pixart_ppg_disable_interrupt### %d",dd_ptr->int_registered);
    if(dd_ptr->int_registered)
    {
        ret_val = sns_ddf_signal_deregister(dd_ptr->interrupt_gpio);
        if(ret_val != SNS_DDF_SUCCESS)
        {
            PAH8002_MSG_1(MED, "###sns_ddf_signal_deregister Fail### %d",ret_val);
            return ret_val;
        }
        dd_ptr->int_registered = false;
        PAH8002_MSG_0(MED, "###sns_ddf_signal_deregister###");
    }
    return ret_val;
}

static sns_ddf_status_e sns_dd_pixart_report_touch_data(
        sns_ddf_handle_t dd_handle)
{
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    uint32_t gpio_status = 0;

    state->touch_data.sensor = SNS_DDF_SENSOR_HEART_RATE;
    state->touch_data.status = SNS_DDF_SUCCESS;
    state->touch_data.timestamp = sns_ddf_get_timestamp();
    state->touch_data.num_samples = 3;
    //pah8002_get_touch_flag(dd_handle, &state->pah8002_touch_flag);
    sns_dd_pixart_ppg_enable_interrupt(dd_handle); //if not registered, register once. Do not care "state->rising_falling" .
    if (sns_ddf_signal_gpio_read(state->interrupt_gpio, &gpio_status) != SNS_DDF_SUCCESS)
    {
        PAH8002_MSG_0(MED, "ERROR");
    }
    state->pah8002_touch_flag = gpio_status;
    PAH8002_MSG_1(MED, "###sns_dd_pixart_ppg_handle_irq ::: touch data %d###",state->pah8002_touch_flag);
    if(state->pah8002_touch_flag == 1)
    {
        state->touch_data.samples[0].sample = FX_FLTTOFIX_Q16(0);
        state->touch_data.samples[0].status = SNS_DDF_SUCCESS;
        state->touch_data.samples[1].sample = FX_FLTTOFIX_Q16(0);
        state->touch_data.samples[1].status = SNS_DDF_SUCCESS;
        state->touch_data.samples[2].sample = FX_FLTTOFIX_Q16(128);
        state->touch_data.samples[2].status = SNS_DDF_SUCCESS;
        state->rising_falling = SNS_DDF_SIGNAL_IRQ_FALLING;
    }
    else
    {
        state->touch_data.samples[0].sample = FX_FLTTOFIX_Q16(0);
        state->touch_data.samples[0].status = SNS_DDF_SUCCESS;
        state->touch_data.samples[1].sample = FX_FLTTOFIX_Q16(0);
        state->touch_data.samples[1].status = SNS_DDF_SUCCESS;
        state->touch_data.samples[2].sample = FX_FLTTOFIX_Q16(0);
        state->touch_data.samples[2].status = SNS_DDF_SUCCESS;
        state->rising_falling = SNS_DDF_SIGNAL_IRQ_RISING;
    }
    if( state->last_ppg_touch_status != state->pah8002_touch_flag)
    {
        status = sns_ddf_smgr_notify_data(state->smgr_hndl, &state->touch_data, 1);
        if(status != SNS_DDF_SUCCESS)
            return status;
        state->last_ppg_touch_status = state->pah8002_touch_flag;
    }
    sns_dd_pixart_ppg_disable_interrupt(dd_handle);
    sns_dd_pixart_ppg_enable_interrupt(dd_handle);
    PAH8002_MSG_1(MED, "###sns_dd_pixart_ppg_handle_irq ::: touch flag = %d ###",state->touch_data.samples[2].sample );
    return status;
}

#define SUB_SAMPLES 3
static void sns_dd_pixart_log_fifo_data(sns_ddf_sensor_data_s *sensor_data)
{
   sns_log_sensor_data_pkt_s *log_struct_ptr;
   sns_err_code_e err_code;
   int i = 0;
   log_pkt_t log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA;

   /* Allocate log packet */
   err_code = sns_logpkt_malloc(
                         log_pkt_type,
                         sizeof(sns_log_sensor_data_pkt_s)+ (sensor_data->num_samples -1)*sizeof(int32_t),
                         (void**)&log_struct_ptr);

   PAH8002_MSG_2(HIGH, ">>> sns_dd_pixart_log_fifo_data error_code %d %d", err_code,sensor_data->num_samples);
   if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
   {
     log_struct_ptr->version   = SNS_LOG_SENSOR_DATA_PKT_VERSION;
     log_struct_ptr->sensor_id = SNS_DDF_SENSOR_HEART_RATE_RAW;
     log_struct_ptr->vendor_id = SNS_DDF_VENDOR_PIXART;
     log_struct_ptr->timestamp = sensor_data->timestamp;
     log_struct_ptr->end_timestamp = sensor_data->end_timestamp;
     log_struct_ptr->num_data_types = SUB_SAMPLES;
     log_struct_ptr->num_samples = sensor_data->num_samples / SUB_SAMPLES;
     log_struct_ptr->data[0]   = sensor_data->samples[0].sample;
     log_struct_ptr->data[1]   = sensor_data->samples[1].sample;
     log_struct_ptr->data[2]   = sensor_data->samples[2].sample;

     for(i=0; i<sensor_data->num_samples; i++)
     {
       log_struct_ptr->samples[i] = sensor_data->samples[i].sample;
     }
     /* Commit log (also frees up the log packet memory) */
     sns_logpkt_commit(log_pkt_type, log_struct_ptr);
   }
}

static void sns_dd_pixart_report_fifo_data(sns_dd_pixart_ppg_state_t *state)
{
  sns_ddf_status_e status;
  sns_ddf_sensor_data_s*    sensor_data_report;
  sns_ddf_sensor_sample_s*  data_ptr;
  int i = 0 ;
  int j=0;
  q16_t sample2;
  sns_ddf_time_t sample_interval ;
  PAH8002_MSG_0(HIGH, ">>> sns_dd_pixart_report_fifo_data");
  /* allocate memory for sensor data */
  if ((status = sns_ddf_malloc_ex(
          (void **)&sensor_data_report,
          sizeof(sns_ddf_sensor_data_s) , state->smgr_hndl))
       != SNS_DDF_SUCCESS)
  {
    //sns_ddf_smgr_notify_data(state->smgr_hndl, NULL, 0);
    return;
  }
  /* allocate memory for sensor sub-samples */
  if ((status = sns_ddf_malloc_ex(
                 (void **)&data_ptr,
                 state->water_mark *SUB_SAMPLES* sizeof(sns_ddf_sensor_sample_s) , state->smgr_hndl))
       != SNS_DDF_SUCCESS)
  {
    sns_ddf_mfree_ex(sensor_data_report, state->smgr_hndl);
    return;
  }
  /* create a report */
  sensor_data_report->sensor      = SNS_DDF_SENSOR_HEART_RATE_RAW;
  sensor_data_report->status      = SNS_DDF_SUCCESS;
  sensor_data_report->samples     = data_ptr;
  sensor_data_report->num_samples = state->water_mark * SUB_SAMPLES;
  sensor_data_report->end_timestamp = sns_ddf_get_timestamp();
  sample_interval = (sensor_data_report->end_timestamp - state->pah8002_previous_timestamp) / state->water_mark ;
  sensor_data_report->timestamp   = state->pah8002_previous_timestamp + sample_interval;
  state->pah8002_previous_timestamp = sensor_data_report->end_timestamp;
  if(state->pah8002_touch_flag == 1)
  {
    sample2 = FX_FLTTOFIX_Q16(128);
  }
  else
  {
    sample2 = FX_FLTTOFIX_Q16(0);
  }
  while(i<sensor_data_report->num_samples)
  {
    PAH8002_MSG_1(MED, "FIFO %08x \n",  *(uint32_t *)(&state->pah8002_ppg_data[i*4]));
    i++;
  }
  for(i=0,j=0; i<sensor_data_report->num_samples; i+=3, j +=TOTAL_CHANNELS  )
  {
    sensor_data_report->samples[i].sample = *(uint32_t *)(&state->pah8002_ppg_data[j*4 + 8]);
    sensor_data_report->samples[i].status = SNS_DDF_SUCCESS;
    sensor_data_report->samples[i+1].sample = *(uint32_t *)(&state->pah8002_ppg_data[j*4 + 12]);
    sensor_data_report->samples[i+1].status = SNS_DDF_SUCCESS;
    sensor_data_report->samples[i+2].sample = sample2;
    sensor_data_report->samples[i+2].status = SNS_DDF_SUCCESS;
    PAH8002_MSG_4(MED, "X: %08x Y :%08x Z:%08x  Time:%u", sensor_data_report->samples[i].sample, sensor_data_report->samples[i+1].sample, sensor_data_report->samples[i+2].sample,sensor_data_report->timestamp);
  }
  
  sns_ddf_smgr_notify_data(state->smgr_hndl, sensor_data_report, 1);
  sns_dd_pixart_log_fifo_data(sensor_data_report);
  sns_ddf_mfree_ex(sensor_data_report, state->smgr_hndl);
  sns_ddf_mfree_ex(data_ptr, state->smgr_hndl);
}

static void sns_dd_pixart_report_data(sns_dd_pixart_ppg_state_t *state)
{
    uint32_t now_timestamp;
    uint32_t diff_time;
    uint8_t temp[4];
    int i = 0;
    int samples_per_read = state->odr_ppg == 20 ? HEART_RATE_MODE_SAMPLES_PER_READ : STRESS_MODE_SAMPLES_PER_READ;
    sns_ddf_status_e status ;

    now_timestamp = sns_ddf_get_timestamp();
    diff_time = (now_timestamp - state->pah8002_previous_timestamp)/ (samples_per_read/TOTAL_CHANNELS);
    PAH8002_MSG_0(HIGH, ">>> sns_dd_pixart_report_data");
    for(i=0; i<samples_per_read; i+=TOTAL_CHANNELS)
    {   //report raw data
        state->sensor_data.sensor = SNS_DDF_SENSOR_HEART_RATE_RAW;
        state->sensor_data.status = SNS_DDF_SUCCESS;
        state->sensor_data.num_samples = 3;
        state->sensor_data.timestamp = state->pah8002_previous_timestamp + diff_time * (i/TOTAL_CHANNELS+1);

        if(state->pah8002_touch_flag == 1)
        {
            state->sensor_data.samples[2].sample = FX_FLTTOFIX_Q16(128);
            state->sensor_data.samples[2].status = SNS_DDF_SUCCESS;
        }
        else
        {
            state->sensor_data.samples[2].sample = FX_FLTTOFIX_Q16(0);
            state->sensor_data.samples[2].status = SNS_DDF_SUCCESS;
        }

        temp[0] = state->pah8002_ppg_data[i*4 + 0];
        temp[1] = state->pah8002_ppg_data[i*4 + 1];
        temp[2] = state->pah8002_ppg_data[i*4 + 2];
        temp[3] = state->pah8002_ppg_data[i*4 + 3];
        state->sensor_data.samples[0].sample = ( *(uint32_t *)(&temp[0]));
        state->sensor_data.samples[0].status = SNS_DDF_SUCCESS;
        PAH8002_MSG_1(MED, "CH_0 = %d", ( *(uint32_t *)(&temp[0])));

        temp[0] = state->pah8002_ppg_data[i*4 + 4];
        temp[1] = state->pah8002_ppg_data[i*4 + 5];
        temp[2] = state->pah8002_ppg_data[i*4 + 6];
        temp[3] = state->pah8002_ppg_data[i*4 + 7];
        state->sensor_data.samples[1].sample = ( *(uint32_t *)(&temp[0]));
        state->sensor_data.samples[1].status = SNS_DDF_SUCCESS;
        PAH8002_MSG_1(MED, "CH_1 = %d", ( *(uint32_t *)(&temp[0])));

        sns_dd_pah8002_log(state, SNS_DDF_SENSOR_HEART_RATE_RAW, state->sensor_data.samples[0].sample, state->sensor_data.samples[1].sample, state->sensor_data.samples[2].sample);
        /* value[1] : Pixart algo Hr bpm */

        temp[0] = state->pah8002_ppg_data[i*4 + 8];
        temp[1] = state->pah8002_ppg_data[i*4 + 9];
        temp[2] = state->pah8002_ppg_data[i*4 + 10];
        temp[3] = state->pah8002_ppg_data[i*4 + 11];
        state->sensor_data.samples[0].sample = ( *(uint32_t *)(&temp[0]));
        state->sensor_data.samples[0].status = SNS_DDF_SUCCESS;
        PAH8002_MSG_1(MED, "CH_2 = %d", ( *(uint32_t *)(&temp[0])));

        //state->sensor_data.samples[1].sample = FX_FLTTOFIX_Q16((int)state->fHR);
        //state->sensor_data.samples[1].status = SNS_DDF_SUCCESS;

        /* value[2] : Flag touch, */
          temp[0] = state->pah8002_ppg_data[i*4 + 12];
        temp[1] = state->pah8002_ppg_data[i*4 + 13];
        temp[2] = state->pah8002_ppg_data[i*4 + 14];
        temp[3] = state->pah8002_ppg_data[i*4 + 15];
        state->sensor_data.samples[1].sample = ( *(uint32_t *)(&temp[0]));
        state->sensor_data.samples[1].status = SNS_DDF_SUCCESS;
        PAH8002_MSG_1(MED, "CH_3 = %d", ( *(uint32_t *)(&temp[0])));

        status = sns_ddf_smgr_notify_data(state->smgr_hndl, &state->sensor_data, 1);
        if(status != SNS_DDF_SUCCESS){
            PAH8002_MSG_1(ERROR, "sns_ddf_smgr_notify_data fail i = %d", i);
        }
        PAH8002_MSG_4(MED, "X: %08x Y :%08x Z:%08x  Time:%u", state->sensor_data.samples[0].sample, state->sensor_data.samples[1].sample, state->sensor_data.samples[2].sample,state->sensor_data.timestamp);
    }
    state->pah8002_previous_timestamp = now_timestamp;
}
static void sns_dd_pixart_ppg_irq_handler(sns_ddf_handle_t     dd_handle,
        uint32_t gpio_num,
        sns_ddf_time_t timestamp)
{
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    sns_ddf_status_e status ;

    PAH8002_MSG_0(HIGH, "irq handler");
    if(state->ppg_int_enable == 0) //interrupt is not enabled
        return ;

    if(state->odr_ppg != 0) //higher priority than touch mode
    {
        /* this is a sync driver, so try to read data now */
        if( !pah8002_task(dd_handle) )
        {
            return;
        }
        if(state->water_mark > 1)
        {
        sns_dd_pixart_report_fifo_data(state);
        }
        else
        {
          sns_dd_pixart_report_data(state);
        }

        if(state->odr_touch != 0)
        {
            //report touch status
            state->touch_data.sensor = SNS_DDF_SENSOR_HEART_RATE;
            state->touch_data.status = SNS_DDF_SUCCESS;
            state->touch_data.timestamp = sns_ddf_get_timestamp();
            state->touch_data.num_samples = 3;
            //state->pah8002_touch_flag is updated in pah8002_task;
            if(state->pah8002_touch_flag == 1)
            {
                state->touch_data.samples[0].sample = FX_FLTTOFIX_Q16(0);
                state->touch_data.samples[0].status = SNS_DDF_SUCCESS;
                state->touch_data.samples[1].sample = FX_FLTTOFIX_Q16(0);
                state->touch_data.samples[1].status = SNS_DDF_SUCCESS;
                state->touch_data.samples[2].sample = FX_FLTTOFIX_Q16(128);
                state->touch_data.samples[2].status = SNS_DDF_SUCCESS;
            }
            else
            {
                state->touch_data.samples[0].sample = FX_FLTTOFIX_Q16(0);
                state->touch_data.samples[0].status = SNS_DDF_SUCCESS;
                state->touch_data.samples[1].sample = FX_FLTTOFIX_Q16(0);
                state->touch_data.samples[1].status = SNS_DDF_SUCCESS;
                state->touch_data.samples[2].sample = FX_FLTTOFIX_Q16(0);
                state->touch_data.samples[2].status = SNS_DDF_SUCCESS;
            }
            if( state->last_ppg_touch_status != state->pah8002_touch_flag)
            {
                status = sns_ddf_smgr_notify_data(state->smgr_hndl, &state->touch_data, 1);
                if(status != SNS_DDF_SUCCESS)
                    return;    //status;
                state->last_ppg_touch_status = state->pah8002_touch_flag;
            }
        }
#ifdef PXIALG_INSIDE
        if(state->odr_ppg == 20)
        {
            //build up pah8002_report_data
            state->pah8002_data.time = 50 * HEART_RATE_MODE_SAMPLES_PER_CH_READ ;
            state->pah8002_data.touch_flag = state->pah8002_touch_flag;
            //---------------------------------------------------------------------
            do
            {
                float hr =  0;
                uint32_t alg_version = pah8002_version();
                uint8_t ret = 0 ;
                int16_t signal_grade ;
                PAH8002_MSG_1(HIGH, "frame_count = %d", state->pah8002_data.frame_count);
                PAH8002_MSG_1(HIGH, "time = %d", state->pah8002_data.time);
                PAH8002_MSG_1(HIGH, "touch_flag = %d", state->pah8002_data.touch_flag);
                PAH8002_MSG_1(HIGH, "nf_ppg_channel = %d", state->pah8002_data.nf_ppg_channel);
                PAH8002_MSG_1(HIGH, "nf_ppg_per_channel = %d", state->pah8002_data.nf_ppg_per_channel);
                PAH8002_MSG_2(HIGH, "ppg_data@ %x, %x", state->pah8002_data.ppg_data, state->pah8002_ppg_data);
                PAH8002_MSG_1(HIGH, "nf_mems = %d", state->pah8002_data.nf_mems);
                PAH8002_MSG_1(HIGH, "mems_data@ %x", state->pah8002_data.mems_data);
                ret = pah8002_entrance(&state->pah8002_data);
                pah8002_get_signal_grade(&signal_grade) ;
                pah8002_get_hr(&hr) ;
                PAH8002_MSG_1(HIGH, "ver = %d", alg_version);
                PAH8002_MSG_3(HIGH, "Ret = %x, sg = %d, hr = %d", ret, signal_grade, (int)hr );
            }while(0);
            state->pah8002_data.frame_count++;
        }
#endif
    }
    else if(state->odr_touch != 0)
    {
        //touch mode enable
        sns_dd_pixart_report_touch_data(dd_handle);
    }
}

sns_ddf_status_e sns_dd_pixart_enable_sched_data(
        sns_ddf_handle_t  dd_handle,
        sns_ddf_sensor_e  sensor,
        bool              enable)
{
    int errorcode;
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    PAH8002_MSG_3(HIGH, "sns_dd_pixart_enable_sched_data Sensor:%d Enable:%d INT:%d",sensor,enable,state->ppg_int_enable);

   /* this prevent watermark update with ODR change thus fifo mode operation didn't work
   if(state->op_mode == OP_MODE_NONE){
        return SNS_DDF_EFAIL;
    } */

    if(enable)
    {
        if(state->ppg_int_enable == 0 ) //if not enabled (touch, raw, stress)
        {
            PAH8002_MSG_0(HIGH, "sns_dd_pixart_enable_sched_data enable -1");
            sns_ddf_status_e stat = SNS_DDF_SUCCESS;

            if(!sns_ddf_signal_irq_enabled())
            {
                errorcode = SNS_DDF_PPG_ERROR_4;
                goto RTN;
            }

            state->ppg_int_enable = 1 ;
            if(state->odr_ppg == 20)
            {
                //Heart rate mode
                stat = sns_ddf_signal_register(state->interrupt_gpio, state, &sns_dd_pah8002_if, SNS_DDF_SIGNAL_IRQ_RISING);
                if(stat != SNS_DDF_SUCCESS)
                {
                    errorcode = SNS_DDF_PPG_ERROR_5;
                    goto RTN;
                }
                if( !pah8002_enter_normal_mode(dd_handle))
                {
                    errorcode = SNS_DDF_PPG_ERROR_6;
                    goto RTN;
                }

#ifdef PXIALG_INSIDE
                pah8002_close();
                pah8002_open();
#endif
            }
            else if(state->odr_ppg == 200)
            {
                //Stress mode
                stat = sns_ddf_signal_register(state->interrupt_gpio, state, &sns_dd_pah8002_if, SNS_DDF_SIGNAL_IRQ_RISING);
                if(stat != SNS_DDF_SUCCESS)
                {
                    errorcode = SNS_DDF_PPG_ERROR_7;
                    goto RTN;
                }
                if( !pah8002_enter_stress_mode(dd_handle))
                {
                    errorcode = SNS_DDF_PPG_ERROR_8;
                    goto RTN;
                }
            }
            else if(state->odr_touch != 0)
            {
                if( !pah8002_enter_touch_mode(dd_handle))
                {
                    errorcode = SNS_DDF_PPG_ERROR_9;
                    goto RTN;
                }
                PAH8002_MSG_1(FATAL, "CHIP_ID: %x",state->chip_id);
                sns_dd_pixart_report_touch_data(dd_handle);
            }
        }
        else    //if already enabled (touch, raw, stress)
        {
            PAH8002_MSG_0(HIGH, "sns_dd_pixart_enable_sched_data enable - 2");
            sns_ddf_status_e stat = SNS_DDF_SUCCESS;
            if(state->odr_ppg == 20)
            {
                //Heart rate mode
                stat = sns_ddf_signal_deregister(state->interrupt_gpio);
                state->int_registered = 0;
                if(stat != SNS_DDF_SUCCESS)
                {
                    errorcode = SNS_DDF_PPG_ERROR_12;
                    goto RTN;
                }
                stat = sns_ddf_signal_register(state->interrupt_gpio, state, &sns_dd_pah8002_if, SNS_DDF_SIGNAL_IRQ_RISING);
                if(stat != SNS_DDF_SUCCESS)
                    return stat;
                if( !pah8002_enter_normal_mode(dd_handle))
                {
                    errorcode = SNS_DDF_PPG_ERROR_13;
                    goto RTN;
                }

#ifdef PXIALG_INSIDE
                pah8002_close();
                pah8002_open();
#endif
            }
            else if(state->odr_ppg == 200)
            {
                //Stress mode
                stat = sns_ddf_signal_deregister(state->interrupt_gpio);
                state->int_registered = 0;
                if(stat != SNS_DDF_SUCCESS)
                {
                    errorcode = SNS_DDF_PPG_ERROR_14;
                    goto RTN;
                }
                stat = sns_ddf_signal_register(state->interrupt_gpio, state, &sns_dd_pah8002_if, SNS_DDF_SIGNAL_IRQ_RISING);
                if(stat != SNS_DDF_SUCCESS)
                {
                    errorcode = SNS_DDF_PPG_ERROR_15;
                    goto RTN;
                }
                if( !pah8002_enter_stress_mode(dd_handle))
                {
                    errorcode = SNS_DDF_PPG_ERROR_16;
                    goto RTN;
                }
            }
            else if(state->odr_touch != 0)
            {
                stat = sns_ddf_signal_deregister(state->interrupt_gpio);
                if(stat != SNS_DDF_SUCCESS)
                {
                    errorcode = SNS_DDF_PPG_ERROR_17;
                    goto RTN;
                }
                state->int_registered = 0;
                if( !pah8002_enter_touch_mode(dd_handle))
                {
                    errorcode = SNS_DDF_PPG_ERROR_18;
                    goto RTN;
                }
                sns_dd_pixart_report_touch_data(dd_handle);
            }
        }
    }
    else
    {
        PAH8002_MSG_2(HIGH, "sns_dd_pixart_enable_sched_data disable Sensor:%d - state->odr_touch%d",sensor,state->odr_touch);
        sns_ddf_status_e stat = SNS_DDF_SUCCESS;
        if( sensor == SNS_DDF_SENSOR_HEART_RATE_RAW) //close PPG
        {
            if(state->odr_touch != 0)       //close touch, but still touch
            {
                stat = sns_ddf_signal_deregister(state->interrupt_gpio);
                if(stat != SNS_DDF_SUCCESS)
                {
                    errorcode = SNS_DDF_PPG_ERROR_19;
                    goto RTN;
                }
                state->int_registered = 0;
                if( !pah8002_enter_touch_mode(dd_handle))
                {
                    errorcode = SNS_DDF_PPG_ERROR_20;
                    goto RTN;
                }
                sns_dd_pixart_report_touch_data(dd_handle);
            }
            else //if( (state->odr_touch == 0) && (state->odr_ppg == 0))
            {
                if(state->ppg_int_enable == 1)
                {
                    sns_ddf_signal_deregister(state->interrupt_gpio);
                    state->int_registered = false;
                    //pah8002_stop(dd_handle);
                    //pah8002_sw_reset(dd_handle);
                    state->ppg_int_enable = 0 ;
                    state->op_mode = OP_MODE_NONE;
                }
            }
        }
        else //close touch
        {
            if(state->odr_ppg == 20) //close touch, but still raw
            {
                //Heart rate mode
                stat = sns_ddf_signal_deregister(state->interrupt_gpio);
                state->int_registered = 0;
                if(stat != SNS_DDF_SUCCESS)
                {
                    errorcode = SNS_DDF_PPG_ERROR_21;
                    goto RTN;
                }
                stat = sns_ddf_signal_register(state->interrupt_gpio, state, &sns_dd_pah8002_if, SNS_DDF_SIGNAL_IRQ_RISING);
                if(stat != SNS_DDF_SUCCESS)
                {
                    errorcode = SNS_DDF_PPG_ERROR_22;
                    goto RTN;
                }
                if( !pah8002_enter_normal_mode(dd_handle))
                {
                    errorcode = SNS_DDF_PPG_ERROR_23;
                    goto RTN;
                }

#ifdef PXIALG_INSIDE
                pah8002_close();
                pah8002_open();
#endif
            }
            else if(state->odr_ppg == 200) //close touch, but still stress
            {
                //Stress mode
                stat = sns_ddf_signal_deregister(state->interrupt_gpio);
                state->int_registered = 0;
                if(stat != SNS_DDF_SUCCESS)
                {
                    errorcode = SNS_DDF_PPG_ERROR_24;
                    goto RTN;
                }
                stat = sns_ddf_signal_register(state->interrupt_gpio, state, &sns_dd_pah8002_if, SNS_DDF_SIGNAL_IRQ_RISING);
                if(stat != SNS_DDF_SUCCESS)
                {
                    errorcode = SNS_DDF_PPG_ERROR_25;
                    goto RTN;
                }
                if( !pah8002_enter_stress_mode(dd_handle))
                {
                    errorcode = SNS_DDF_PPG_ERROR_26;
                    goto RTN;
                }
            }
            else  //if( (state->odr_touch == 0) && (state->odr_ppg == 0))
            {
                if(state->ppg_int_enable == 1)
                {
                    sns_ddf_signal_deregister(state->interrupt_gpio);
                    state->int_registered = false;
                    //pah8002_stop(dd_handle);
                    //pah8002_sw_reset(dd_handle);
                    state->ppg_int_enable = 0 ;
                    state->op_mode = OP_MODE_NONE;
                }
            }
        }
    }
    return SNS_DDF_SUCCESS;

    RTN:
    PAH8002_MSG_1(HIGH, "<< FAIL!!! sns_dd_pixart_enable_sched_data error %d\r\n",errorcode);
    return SNS_DDF_EFAIL;
}
/***************************************************************************/
/***************************************************************************/
static sns_ddf_status_e pah8002_write_reg(sns_ddf_handle_t dd_handle, uint8_t addr, uint8_t data)
{
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    sns_ddf_status_e status;
    uint8_t reg, out;
    reg = data;
    status = sns_ddf_write_port(state->port_handle,
            addr,
            &reg,
            1,
            &out);
    // PAH8002_MSG_3(HIGH, "pah8002_write_reg addr = 0x%2x, data = 0x%2x, status =%d", addr, data, (int)status);
    return status;
}

static sns_ddf_status_e pah8002_read_reg(sns_ddf_handle_t dd_handle, uint8_t addr, uint8_t *data)
{
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    sns_ddf_status_e status;
    uint8_t out;

    status = sns_ddf_read_port(state->port_handle,
            addr,
            data,
            1,
            &out);
    //PAH8002_MSG_3(HIGH, "pah8002_read_reg addr = 0x%2x, data = 0x%2x, status = %d", addr, *data, (int)status);
    return status;
}

static sns_ddf_status_e pah8002_burst_read_reg(sns_ddf_handle_t dd_handle, uint8_t addr, uint8_t *data, uint16_t num)
{
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    sns_ddf_status_e status;
    uint32_t out;
    status = sns_ddf_read_port_u32_count(state->port_handle,
            addr,
            data,
            num,
            &out);
    return status ;
}

static bool pah8002_sw_reset(sns_ddf_handle_t dd_handle)
{
    uint8_t data ;
    PAH8002_MSG_0(HIGH, ">>> pah8002_sw_reset \r\n");
    if(pah8002_wakeup(dd_handle) == SNS_DDF_EFAIL)
        goto RTN;
    if(0 != pah8002_write_reg(dd_handle, 0x7f, 0x00))
    {
        goto RTN;
    }
    if(0 != pah8002_read_reg(dd_handle, 0, &data))
    {
        goto RTN;
    }
    PAH8002_MSG_1(HIGH, "ID = %d\r\n", data);
    if(data != 0x02)
    {
        goto RTN;
    }
    if(0 != pah8002_write_reg(dd_handle, 0xe1, 0))//write 0 to trigger Software Reset
    {
        goto RTN;
    }
    //delay 5ms
    sns_ddf_delay(5*1000);
    PAH8002_MSG_0(HIGH, "<<< pah8002_sw_reset \r\n");
    return true;
RTN:
    PAH8002_MSG_0(FATAL, "<<< FAIL!!! pah8002_sw_reset \r\n");
    return false;
}

static bool pah8002_start(sns_ddf_handle_t dd_handle)
{
    uint8_t data = 0;
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    int samples_per_read = state->odr_ppg == 20 ? HEART_RATE_MODE_SAMPLES_PER_READ : STRESS_MODE_SAMPLES_PER_READ ;
    if(state->water_mark > 1)
      samples_per_read = state->water_mark * TOTAL_CHANNELS ;
    PAH8002_MSG_1(HIGH, ">>> pah8002_start %d\r\n", samples_per_read);
    if(pah8002_wakeup(dd_handle) == SNS_DDF_EFAIL)
        goto RTN;
    if(0 != pah8002_write_reg(dd_handle, 0x7f, 0x01))
    {
        goto RTN;
    }
    else if(0 != pah8002_write_reg(dd_handle, 0xea, (samples_per_read+1)))
    {
        goto RTN;
    }
    else if(0 != pah8002_write_reg(dd_handle, 0xd5, 1))//TG enable. REQTIMER_ENABLE
    {
        goto RTN;
    }
    else if(0 != pah8002_read_reg(dd_handle, 0xd5, &data))//TG enable. REQTIMER_ENABLE
    {
        goto RTN;
    }
    state -> pah8002_previous_timestamp = sns_ddf_get_timestamp() ;
    PAH8002_MSG_1(HIGH, "<<< pah8002_start %d\r\n",data);
    return true;
RTN:
    PAH8002_MSG_0(FATAL, "<<< FAIL!!! pah8002_start \r\n");
    return false;
}
static bool pah8002_touch_mode_init(sns_ddf_handle_t dd_handle)
{
    int i =0 ;
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    PAH8002_MSG_0(HIGH, ">>> pah8002_touch_mode_init \r\n");
    if(pah8002_wakeup(dd_handle) == SNS_DDF_EFAIL)
        goto RTN;
    PAH8002_MSG_1(FATAL, "CHIP_ID: %x",state->chip_id);
    for(i = 0; i < REVD_INIT_TOUCH_REG_ARRAY_SIZE;i++)
    {
        if ( pah8002_write_reg(dd_handle,
                    revd_init_touch_register_array[i][0],
                    revd_init_touch_register_array[i][1]) != 0 )
        {
            goto RTN;
        }
    }
    PAH8002_MSG_0(HIGH, "<<< pah8002_touch_mode_init \r\n");
    return true;
RTN:
    PAH8002_MSG_0(FATAL, "<<< pah8002_touch_mode_init FAIL!!! \r\n");
    return false;
}

static bool pah8002_normal_mode_init(sns_ddf_handle_t dd_handle)
{
    int i =0 ;
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    PAH8002_MSG_0(HIGH, ">>> pah8002_normal_mode_init \r\n");
    if(pah8002_wakeup(dd_handle) == SNS_DDF_EFAIL)
        goto RTN;
    PAH8002_MSG_1(FATAL, "CHIP_ID: %x",state->chip_id);
    for(i = 0; i < REVD_INIT_PPG_REG_ARRAY_SIZE;i++)
    {
        if ( pah8002_write_reg(dd_handle,
                    revd_init_ppg_register_array[i][0],
                    revd_init_ppg_register_array[i][1]) != 0 )
        {
            PAH8002_MSG_2(FATAL, "pah8002_normal_mode_init init regiter write fail 0x%2x 0x%2x \r\n",
                    revd_init_ppg_register_array[i][0], revd_init_ppg_register_array[i][1]);
            goto RTN;
        }
    }
    PAH8002_MSG_0(HIGH, "<<< pah8002_normal_mode_init \r\n");
    return true;
RTN:
    PAH8002_MSG_0(FATAL, "<<< FAIL!! pah8002_normal_mode_init \r\n");
    return false;
}

static bool pah8002_stress_mode_init(sns_ddf_handle_t dd_handle)
{
    int i =0 ;
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    PAH8002_MSG_0(HIGH, ">>> pah8002_stress_mode_init \r\n");
    if(pah8002_wakeup(dd_handle) == SNS_DDF_EFAIL)
        goto RTN;
    PAH8002_MSG_1(FATAL, "CHIP_ID: %x",state->chip_id);
    for(i = 0; i < REVD_INIT_STRESS_REG_ARRAY_SIZE;i++)
    {
        if ( pah8002_write_reg(dd_handle,
                    revd_init_stress_register_array[i][0],
                    revd_init_stress_register_array[i][1]) != 0 )
        {
            goto RTN;
        }
    }
    PAH8002_MSG_0(HIGH, "<<< pah8002_stress_mode_init \r\n");
    return true;
RTN:
    return false;
}

static uint8_t pah8002_get_touch_flag_ppg_mode(sns_ddf_handle_t dd_handle)
{
    static uint8_t touch_sts_output = 1;
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    int32_t *s = (int32_t *)state->pah8002_ppg_data ;
    int32_t ch0 ;
    int32_t ch1 ;
    int64_t ir_rawdata;
    int i;
    static int touch_cnt = 0, no_touch_cnt = 0;
    int samples_per_read = state->odr_ppg == 20 ? HEART_RATE_MODE_SAMPLES_PER_READ : STRESS_MODE_SAMPLES_PER_READ;
    if(state->water_mark > 1)
      samples_per_read = state->water_mark * TOTAL_CHANNELS ;

    #define TouchDetection_Upper_TH (1648) //raw data = 200,000
    #define TouchDetection_Lower_TH (1565) //raw data = 190,000
    #define TouchDetection_Count_TH   (3)    //(3+1)*50ms = 200ms
    #define NoTouchDetection_Count_TH (3)    //(3+1)*50ms = 200ms

    for(i=0; i<samples_per_read; i+=TOTAL_CHANNELS)
    {
        ch0 = *s;
        ch1 = *(s+1);
        ir_rawdata = ch0 - ch1 ;
        ir_rawdata = (ir_rawdata * state->u8IRDAC * state->u8IRET)>>20 ;
        if( ir_rawdata > TouchDetection_Upper_TH)
        {
            touch_cnt++;
            no_touch_cnt = 0;
        }
        else if( ir_rawdata < TouchDetection_Lower_TH)
        {
            no_touch_cnt++;
            touch_cnt = 0 ;
        }
        else
        {
            touch_cnt = 0 ;
            no_touch_cnt = 0;
        }
        s+=TOTAL_CHANNELS;
    }
    if(touch_cnt > TouchDetection_Count_TH)
    {
        touch_sts_output = 1;
    }
    else if( no_touch_cnt > NoTouchDetection_Count_TH)
    {
        touch_sts_output = 0;
    }
    PAH8002_MSG_2(HIGH, "<<< pah8002_get_touch_flag_ppg_mode %d, %d\n",touch_cnt, no_touch_cnt);
    PAH8002_MSG_1(HIGH, "<<< pah8002_get_touch_flag_ppg_mode %d\n", touch_sts_output);
    return touch_sts_output;
}

static bool pah8002_task(sns_ddf_handle_t dd_handle)
{
    uint8_t cks[4] ;
    uint8_t int_req = 0;
    uint8_t reg0x25 = 0;
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    PAH8002_MSG_0(HIGH, ">>> pah8002_task\n");
    if(pah8002_wakeup(dd_handle) == SNS_DDF_EFAIL)
        goto RTN;
    if(0 != pah8002_write_reg(dd_handle, 0x7f, 0x02))
    {
        /* do nothing */
    }
    else if(0 != pah8002_read_reg(dd_handle, 0x25, &reg0x25))
    {
        /* do nothing */
    }
    else if(0 != pah8002_read_reg(dd_handle, 0x73, &int_req))
    {
        /* do nothing */
    }
    else
    {
        PAH8002_MSG_1(HIGH, "Bank2 Reg0x25 = %x\r\n", reg0x25);
        if( (int_req & 0x04) != 0)
        {
            //overflow
            PAH8002_MSG_0(HIGH, "overflow interrupt\n");
            //Flush FIFOs
            if(0 != pah8002_write_reg(dd_handle, 0x7f, 0x03))
            {}
            else
            {
                int k=0;
                int tmp;
                for(k=0; k<MAX_FIFO_READ_SIZE/4; k++)
                {
                    pah8002_burst_read_reg(dd_handle, 0, (uint8_t *)&tmp, 4);
                }
            }
            if(0 != pah8002_write_reg(dd_handle, 0x7f, 0x02))
            {}
            else if(0 != pah8002_write_reg(dd_handle, 0x8C, 0x01))//clear FIFO Overflow interrupt
            {}
            else if(0 != pah8002_write_reg(dd_handle, 0x8C, 0x00))
            {}
            else if(0 != pah8002_write_reg(dd_handle, 0x75, 0x01))//read fifo first, then clear SRAM FIFO interrupt
            {}
            else if(0 != pah8002_write_reg(dd_handle, 0x75, 0x00))
            {}
            goto RTN;
        }
        if( (int_req & 0x02) != 0)
        {
            //touch interrupt
            PAH8002_MSG_0(HIGH, "touch interrupt\n");
            //pah8002_enter_normal_mode(dd_handle);
            goto RTN;
        }
        if( (int_req & 0x08) != 0)
        {
            //underflow
            PAH8002_MSG_0(HIGH, "underflow interrupt\n");
            if(0 != pah8002_write_reg(dd_handle, 0x7f, 0x02))
            {}
            else if(0 != pah8002_write_reg(dd_handle, 0x8E, 0x01))//clear FIFO Underflow interrupt
            {}
            else if(0 != pah8002_write_reg(dd_handle, 0x8E, 0x00))
            {}
            goto RTN;
        }
        if( (int_req & 0x01) != 0)
        {
            int samples_per_read = state->odr_ppg == 20 ? HEART_RATE_MODE_SAMPLES_PER_READ : STRESS_MODE_SAMPLES_PER_READ ;
            if(state->water_mark > 1)
              samples_per_read = state->water_mark * TOTAL_CHANNELS ;

            PAH8002_MSG_0(HIGH, "FIFO interrupt\n");
            //pah8002_get_touch_flag(dd_handle, &state->pah8002_touch_flag);
            if(0 != pah8002_write_reg(dd_handle, 0x7f, 0x03))
            {}
            else if(0 != pah8002_burst_read_reg(dd_handle, 0, state->pah8002_ppg_data, samples_per_read*4))
            {}
            else if(0 != pah8002_write_reg(dd_handle, 0x7f, 0x02))
            {}
            else if(0 != pah8002_burst_read_reg(dd_handle, 0x80, cks, 4))
            {}
            else if(0 != pah8002_write_reg(dd_handle, 0x75, 0x01))//read fifo first, then clear SRAM FIFO interrupt
            {}
            else if(0 != pah8002_write_reg(dd_handle, 0x75, 0x00))
            {}
            else
            {
                uint32_t *s = (uint32_t *)state->pah8002_ppg_data ;
                uint32_t cks_cal = *s;
                uint32_t cks_rx = *((uint32_t *)cks) ;
                uint32_t i ;
                //checksum compare
                for(i=1; i<samples_per_read; i++)
                {
                    cks_cal = cks_cal ^ (*(s+i)) ;
                }
                if(cks_cal != cks_rx)
                {
                    PAH8002_MSG_0(HIGH, "checksum error\r\n");
                }
                else
                {
                    PAH8002_MSG_2(HIGH, "checksum OK %d, %d\r\n", cks_cal, *(uint32_t *)(state->pah8002_ppg_data + 4));
                }
                state->pah8002_touch_flag = pah8002_get_touch_flag_ppg_mode(dd_handle);
            }
        }
        else
        {
            PAH8002_MSG_1(HIGH, "not fifo interrupt%d\r\n", int_req);
        }
    }
    PAH8002_MSG_0(HIGH, "<<< pah8002_task\n");
    return true;
RTN:
    PAH8002_MSG_0(FATAL, "<<< pah8002_task FAIL!!!!\n");
    return false;
}

static bool pah8002_enter_normal_mode(sns_ddf_handle_t dd_handle)
{
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    PAH8002_MSG_0(HIGH, ">>> pah8002_enter_normal_mode\r\n");
    //1. software reset
    if( !pah8002_sw_reset(dd_handle))
        goto RTN;
    //2. load registers for normal mode
    if( !pah8002_normal_mode_init(dd_handle))
        goto RTN;
    pah8002_write_reg(dd_handle, 0x7f, 0x00);//Bank0
    pah8002_read_reg(dd_handle, 0x0D, &state->u8IRET);// IR Exposure Time
    pah8002_write_reg(dd_handle, 0x7f, 0x01);//Bank1
    pah8002_read_reg(dd_handle, 0xBA, &state->u8IRDAC);//IR Led DAC
    //3. enable sensor
    if( !pah8002_start(dd_handle))
        goto RTN;
    PAH8002_MSG_2(HIGH, "<<< pah8002_enter_normal_mode IRDAC %x, IRET %x\r\n", state->u8IRDAC ,state->u8IRET);
    return true;
RTN:
    PAH8002_MSG_0(FATAL, "<<< FAIL!!! pah8002_enter_normal_mode\r\n");
    return false ;
}

static bool pah8002_enter_stress_mode(sns_ddf_handle_t dd_handle)
{
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    PAH8002_MSG_0(HIGH, ">>> pah8002_enter_stress_mode\r\n");
    //1. software reset
    if( !pah8002_sw_reset(dd_handle))
        goto RTN;

    //2. load registers for normal mode
    if( !pah8002_stress_mode_init(dd_handle))
        goto RTN;

    pah8002_write_reg(dd_handle, 0x7f, 0x00);//Bank0
    pah8002_read_reg(dd_handle, 0x0D, &state->u8IRET);// IR Exposure Time
    pah8002_write_reg(dd_handle, 0x7f, 0x01);//Bank1
    pah8002_read_reg(dd_handle, 0xBA, &state->u8IRDAC);//IR Led DAC

    //3. enable sensor
    if( !pah8002_start(dd_handle))
        goto RTN;
    PAH8002_MSG_0(HIGH, "<<< pah8002_enter_stress_mode \r\n");
    return true;
RTN:
    return false;
}

static bool pah8002_enter_touch_mode(sns_ddf_handle_t dd_handle)
{
    PAH8002_MSG_0(HIGH, ">>> pah8002_enter_touch_mode\r\n");

    //1. software reset
    if( !pah8002_sw_reset(dd_handle) )
        goto RTN;

    //2. load registers for touch mode
    if( !pah8002_touch_mode_init(dd_handle))
        goto RTN;

    //3. enable sensor
    if( !pah8002_start(dd_handle))
        goto RTN;

    PAH8002_MSG_0(HIGH, "<<< pah8002_enter_touch_mode\r\n");
    return true;

RTN:
    PAH8002_MSG_0(FATAL, "<<< pah8002_enter_touch_mode FAIL!!!!!\r\n");
    return false;
}

static int pah8002_wakeup(sns_ddf_handle_t dd_handle)
{
    int retry = 0;
    int success = 0;
    uint8_t data = 0;
    sns_dd_pixart_ppg_state_t *state = (sns_dd_pixart_ppg_state_t *)dd_handle;
    //sns_ddf_status_e status;

    PAH8002_MSG_0(HIGH, ">>> pah8002_wakeup\r\n");
    pah8002_read_reg(dd_handle, 0, &data);
    pah8002_read_reg(dd_handle, 0, &data);

    do
    {
        pah8002_write_reg(dd_handle, 0x7f, 0x00);
        pah8002_read_reg(dd_handle, 0, &data);
        if(data == 0x02)
            success++;
        else
            success = 0 ;

        if(success >=2)
            break;
        retry ++;
    }while(retry < 20);//When Idle Time = 300ms, It only needs max 20 retry times

    if(retry == 20)
    {
        PAH8002_MSG_1(FATAL, ">>> pah8002_wakeup retry = %d \r\n", retry);
        return SNS_DDF_EFAIL;
    }

    if(state -> chip_id == 0)
    {
        pah8002_read_reg(dd_handle, 0x02, &data);
        state -> chip_id = data & 0xF0 ;
    }
    PAH8002_MSG_3(HIGH, "<<< pah8002_wakeup retry %d, Ver. chip_id = %x, drv ver. %d\r\n", retry, state -> chip_id, 21);

    if( (state -> chip_id == 0xD0) /*Version D*/ ||
        (state -> chip_id == 0xC0) /*Version C*/ )   //Get Chip Version.
    {
        pah8002_write_reg(dd_handle, 0x7f, 0x02);
        pah8002_write_reg(dd_handle, 0x70, 0x00);
    }
    else
    {
        return SNS_DDF_EFAIL;
    }

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION:   sns_dd_trigger_fifo_data

===========================================================================*/
/**
* @brief Retrieves a set of sensor data. Asynchronous API
*
* @note If a sensor has failed or
*       isn't available, @a sns_ddf_sensor_data_s.status must be used to
*       reflect this status.
*
* @param[in]  dd_handle    Handle to a driver instance.
* @param[in]  sensor      sensor for which data is requested.
*
* @param[in]  num_samples  number of samples to retrieve as available. Drain the FIFO if value is set to Zero.
* @param[in]  trigger now  trigger notify fifo data now or
*       later when trigger_now is set to true.
*
*
* @return SNS_DDF_SUCCESS if data was populated successfully.
*         via sns_ddf_smgr_data_notify() or if trigger_now is
*         set to false; Otherwise a specific error code is
*         returned.
*
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_pixart_trigger_fifo_data
(
    sns_ddf_handle_t         dd_handle,
    sns_ddf_sensor_e         sensor,
    uint16_t                 num_samples,
    bool                     trigger_now
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  PAH8002_MSG_2(HIGH, "sns_dd_pixart_trigger_fifo_data num_samples - %d, trigger_now %d", num_samples, trigger_now);

  return status;
}
