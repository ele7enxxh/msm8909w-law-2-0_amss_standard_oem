/*==============================================================================

    S E N S O R S   AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Implements the driver for the ltr55x ALS(Light) and Proximity Sensor
   This driver has 3 sub-modules:
   1. The common handler that provides the driver interface
   2. The light(ALS) driver that handles light data type
   3. The proximity driver that handles proximity data type

Copyright (c) 2013-2015 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_alsprx_ltr55x.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
03/10/15     MW      Fixed KW issue
03/12/14     MW      Fixed KW issue
08/21/13     cj      Add calibration code
07/24/13     vh      Eliminated compiler warnings
05/24/13     fp      Initial version of alsprx driver for ltr55x
==============================================================================*/

/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_signal.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_dd.h"
#include "sns_dd_alsprx_ltr55x_priv.h"

extern volatile boolean EnableI2C;

/*============================================================================
                            STATIC VARIABLE DEFINITIONS
============================================================================*/
/* high threshold in raw ADC for each range for ALS in milli-lux */
static const uint32_t sns_dd_ltr55x_als_rng_high_thresh_tb[] =
{
  320,   /* for range SNS_DD_LTR55X_ALSPRX_ALS_RANGE_LOW */
  64000  /* for range SNS_DD_LTR55X_ALSPRX_ALS_RANGE_HIGH */
};

#define SNS_DD_LTR55X_ALS_RNG_TB_SIZE sizeof(sns_dd_ltr55x_als_rng_high_thresh_tb)/sizeof(sns_dd_ltr55x_als_rng_high_thresh_tb[0])
/* tables with adjusted range */
static uint32_t sns_dd_ltr55x_als_rng_high_thresh_adj_tb[SNS_DD_LTR55X_ALS_RNG_TB_SIZE];

/*============================================================================
                           STATIC FUNCTION PROTOTYPES
============================================================================*/
static BOOLEAN  sns_dd_alsprx_prx_prepare_data_from_device
(
  sns_dd_alsprx_state_t* dd_ptr
);

static BOOLEAN sns_dd_alsprx_als_prepare_data_from_device
(
  sns_dd_alsprx_state_t* dd_ptr
);

extern sns_ddf_status_e sns_dd_alsprx_ltr55x_init
(
  sns_ddf_handle_t*        dd_handle_ptr,
  sns_ddf_handle_t         smgr_handle,
  sns_ddf_nv_params_s*     nv_params,
  sns_ddf_device_access_s  device_info[],
  uint32_t                 num_devices,
  sns_ddf_memhandler_s*    memhandler,
  sns_ddf_sensor_e*        sensors[],
  uint32_t*                num_sensors
);

static sns_ddf_status_e sns_dd_alsprx_ltr55x_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
);

static sns_ddf_status_e sns_dd_alsprx_ltr55x_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
);

extern sns_ddf_status_e sns_dd_alsprx_ltr55x_get_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**               value,
  uint32_t*            num_elems
);

static void sns_dd_alsprx_ltr55x_handle_timer
(
  sns_ddf_handle_t dd_handle,
  void* arg
);

static void sns_dd_alsprx_ltr55x_handle_irq
(
  sns_ddf_handle_t dd_handle,
  uint32_t         gpio_num,
  sns_ddf_time_t   timestamp
);

sns_ddf_status_e sns_dd_alsprx_ltr55x_reset
(
  sns_ddf_handle_t dd_handle
);

sns_ddf_status_e sns_dd_alsprx_ltr55x_enable_sched_data
(
  sns_ddf_handle_t  handle,
  sns_ddf_sensor_e  sensor,
  bool              enable
);

sns_ddf_status_e sns_dd_ltr55x_enable_interrupt
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e  sensor
);

sns_ddf_status_e sns_dd_ltr55x_disable_interrupt
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e  sensor
);
static sns_ddf_status_e sns_dd_alsprx_ltr55x_run_test
(
  sns_ddf_handle_t    handle,
  sns_ddf_sensor_e    sensor_type,
  sns_ddf_test_e      test,
  uint32_t*           err
);
static sns_ddf_status_e sns_dd_alsprx_ltr55x_probe
(
  sns_ddf_device_access_s* device_info,
  sns_ddf_memhandler_s*    memhandler,
  uint32_t*                num_sensors,
  sns_ddf_sensor_e**       sensors
);
sns_ddf_driver_if_s sns_alsprx_ltr55x_driver_fn_list =
{
  &sns_dd_alsprx_ltr55x_init,
  &sns_dd_alsprx_ltr55x_get_data,
  &sns_dd_alsprx_ltr55x_set_attrib,
  &sns_dd_alsprx_ltr55x_get_attrib,
  &sns_dd_alsprx_ltr55x_handle_timer,
  &sns_dd_alsprx_ltr55x_handle_irq,
  &sns_dd_alsprx_ltr55x_reset,
  &sns_dd_alsprx_ltr55x_run_test,
  &sns_dd_alsprx_ltr55x_enable_sched_data,
  &sns_dd_alsprx_ltr55x_probe
};

/*===========================================================================
FUNCTION      sns_dd_alsprx_log

DESCRIPTION   Log the latest sensor data

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None
===========================================================================*/
static void sns_dd_alsprx_log(
   sns_dd_alsprx_state_t* dd_ptr,
   sns_ddf_sensor_e       sensor_type,
   uint32_t               data1,
   uint32_t               data1_q16,
   uint32_t               data2,
   uint32_t               data2_q16,
   uint32_t               raw_data )
{
   sns_err_code_e err_code;
   sns_log_sensor_data_pkt_s* log_struct_ptr;

   DDF_MSG_0(HIGH, "LTR55xALS Log Data");

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
     log_struct_ptr->timestamp = sns_ddf_get_timestamp();

     /* Log the sensor data */
     if (sensor_type == SNS_DDF_SENSOR_AMBIENT)
     {
       log_struct_ptr->num_data_types = 3;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data1_q16;
       log_struct_ptr->data[2] = raw_data;
      }
     else
     {
       log_struct_ptr->num_data_types = 5;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data1_q16;
       log_struct_ptr->data[2] = data2;
       log_struct_ptr->data[3] = data2_q16;
       log_struct_ptr->data[4] = raw_data;
     }
     /* Commit log (also frees up the log packet memory) */
     err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
                                  log_struct_ptr);
   }
   else
   {
     dd_ptr->dropped_logs++;
   }
}

/*===========================================================================

FUNCTION      sns_dd_start_general_timer

DESCRIPTION   Starts a timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
static void sns_dd_start_general_timer
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e       sensor_type,
  uint32_t duration
)
{
  sns_ddf_status_e status;
  sns_dd_timer_type* timer_ptr;

  if (sensor_type == SNS_DDF_SENSOR_AMBIENT)
  {
    timer_ptr = &dd_ptr->sns_dd_alsprx_common_db.als_timer;
  }
  else
  {
    timer_ptr = &dd_ptr->sns_dd_alsprx_common_db.prx_timer;
  }

  if ( TRUE == timer_ptr->active )
  {
    return;
  }

#ifdef ALSPRX_DEBUG_TIMER
  SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_ALS_RES_INVALID,
                               100);
#endif

  status = sns_ddf_timer_start(timer_ptr->timer_obj, duration);
  if ( status != SNS_DDF_SUCCESS )
  {
    return;
  }
  timer_ptr->active = TRUE;
}

/*===========================================================================

FUNCTION      sns_dd_stop_general_timer

DESCRIPTION   Stops a timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
static void sns_dd_stop_general_timer
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e       sensor_type
)
{
  sns_dd_timer_type* timer_ptr;

  if (sensor_type == SNS_DDF_SENSOR_AMBIENT)
  {
    timer_ptr = &dd_ptr->sns_dd_alsprx_common_db.als_timer;
  }
  else
  {
    timer_ptr = &dd_ptr->sns_dd_alsprx_common_db.prx_timer;
  }

  if ( TRUE == timer_ptr->active )
  {
#ifdef ALSPRX_DEBUG_TIMER
    SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_ALS_RES_INVALID,
                                 500);
#endif

    sns_ddf_timer_cancel(timer_ptr->timer_obj); /* will return error if timer already stopped */
    timer_ptr->active = FALSE;
  }
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_req_data

DESCRIPTION   function that handles data request from sub-drivers

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_req_data
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e       data_type,
  uint8_t                rng_sel
)
{
  uint8_t i2c_config_reg, bytes_rw;
  sns_ddf_status_e status=SNS_DDF_SUCCESS;
  uint32_t data_ready_us;

  if ( data_type == SNS_DDF_SENSOR_AMBIENT )
  {
    /* Read current config before writing */
    status = sns_ddf_read_port(dd_ptr->port_handle,
        LTR55X_ALS_CONTR,
        (uint8_t*)&i2c_config_reg,
        1,
        &bytes_rw);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
          DBG_DD_ALSPRX_I2C_RD_ERR,
          LTR55X_ALS_CONTR,
          status);
      return status;
    }

    data_ready_us = SNS_DD_ALSPRX_ALS_DATA_READY_US;
      if( dd_ptr->ltr_type == SNS_DD_LTR558)
      {
             i2c_config_reg |= SNS_DD_LTR55X_ALSPRX_ALS_MODE_EN;
      }
      else if(dd_ptr->ltr_type == SNS_DD_LTR559)
      {
      i2c_config_reg |= SNS_DD_LTR559_ALSPRX_ALS_MODE_EN;
      }
    dd_ptr->sns_dd_als_db.als_req_cnt++;

    status = sns_ddf_write_port(dd_ptr->port_handle,
        LTR55X_ALS_CONTR,
        &i2c_config_reg,
        1,
        &bytes_rw);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
          DBG_DD_ALSPRX_I2C_WR_ERR,
          LTR55X_ALS_CONTR,
          status);
      return status;
    }
  }
  else
  {
    /* Read current config before writing */
    status = sns_ddf_read_port(dd_ptr->port_handle,
        LTR55X_PS_CONTR,
        (uint8_t*)&i2c_config_reg,
        1,
        &bytes_rw);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
          DBG_DD_ALSPRX_I2C_RD_ERR,
          LTR55X_PS_CONTR,
          status);
      return status;
    }

    data_ready_us = SNS_DD_ALSPRX_PRX_DATA_READY_US;

      dd_ptr->sns_dd_prx_db.prx_req_cnt++;

    if(dd_ptr->ltr_type == SNS_DD_LTR558)
      {
      i2c_config_reg |= SNS_DD_LTR55X_ALSPRX_PROX_MODE_EN | 0x0c;
            status = sns_ddf_write_port(dd_ptr->port_handle,
        LTR55X_PS_CONTR,
        &i2c_config_reg,
        1,
        &bytes_rw);
      }
      else if(dd_ptr->ltr_type == SNS_DD_LTR559)
      {
      i2c_config_reg |= SNS_DD_LTR55X_ALSPRX_PROX_MODE_EN;
      status = sns_ddf_write_port(dd_ptr->port_handle,
        LTR55X_PS_CONTR,
        &i2c_config_reg,
        1,
        &bytes_rw);
      }
  }

  sns_dd_start_general_timer (dd_ptr,
                              data_type,
                              data_ready_us);
  return status;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_als_convert_to_mlux

DESCRIPTION   Convert a raw data to a real milli lux

DEPENDENCIES  None

RETURN VALUE  milli lux value or 0 if there was a error

SIDE EFFECT   None

===========================================================================*/
static uint32_t sns_dd_alsprx_als_convert_to_mlux
(
  sns_dd_alsprx_state_t* dd_ptr,
  uint8_t rng,
  uint16_t* raw_data
)
{
  int32_t luxdata_int = 0;
  int32_t ratio;
  int32_t alsval_ch1 = raw_data[1];
  int32_t alsval_ch0 = raw_data[0];
  int32_t ch0_coeff = 0;
  int32_t ch1_coeff = 0;

  if(0 == alsval_ch1 + alsval_ch0)
  {
    ratio = 1000;
    DDF_MSG_1(HIGH, "LTR55X als read data error!!\n", ratio);
  }
  else
  {
    ratio = (1000*alsval_ch1) /(alsval_ch1 + alsval_ch0);
  }
  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
      DBG_DD_ALSPRX_ALS_NXT_RNG_SEL,
      ratio);
  if(ratio<450)
  {
    ch0_coeff = 17743;
    ch1_coeff = -11059;
    luxdata_int  = ((alsval_ch0*ch0_coeff) - (alsval_ch1*ch1_coeff))/100000;
    luxdata_int = luxdata_int + 1;
  }
  else if((ratio >=450)&&(ratio<640))
  {
    ch0_coeff = 42785;
    ch1_coeff = 19548;
    luxdata_int  = ((alsval_ch0*ch0_coeff) - (alsval_ch1*ch1_coeff))/100000;
    luxdata_int = luxdata_int + 1;
  }
  else if((ratio >=640)&&(ratio<850))
  {
    ch0_coeff = 5926;
    ch1_coeff = -1185;
    luxdata_int  = ((alsval_ch0*ch0_coeff) - (alsval_ch1*ch1_coeff))/100000;
    luxdata_int = luxdata_int + 1;
  }
  else //if(ratio >= 850)
  {
    ch0_coeff = 0;
    ch1_coeff = 0;
    luxdata_int  = ((alsval_ch0*ch0_coeff) - (alsval_ch1*ch1_coeff))/100000;
  }

  /* use the NV item als factor and visible ratio */
  luxdata_int *= (dd_ptr->sns_dd_alsprx_common_db.nv_db.als_factor / dd_ptr->sns_dd_alsprx_common_db.nv_db.visible_ratio);

/*  Vendor may reuqire this code
  if( rng == (ALS_RANGE1_320-1))
  {
    luxdata_int = luxdata_int/15;
  }
  if(luxdata_int <= 10)
    luxdata_int = 10;
  else if(luxdata_int <=200)
    luxdata_int = 100;
  else if(luxdata_int <=2000)
    luxdata_int = 1000;
  else if(luxdata_int <=10000)
    luxdata_int = 5000;
  else
    luxdata_int = 10000;
*/
#ifdef ALSPRX_DEBUG_REG
  SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
      DBG_DD_ALSPRX_ALS_CONVERSION1,
      raw_data[0],
      raw_data[1]);

  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
      DBG_DD_ALSPRX_ALS_CONVERSION2,
      luxdata_int);

#endif /* ALSPRX_DEBUG_REG */

  return luxdata_int;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_async_notify

DESCRIPTION   Notify to SNSM with a sensor data.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
static void sns_dd_alsprx_async_notify
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e       sensor_type,
  uint32_t               data_val,
  uint32_t               prx_bin ,
  uint32_t               raw_data
)
{
  uint8_t num_sensors = 1, num_samples=2;
  uint16_t size;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  static sns_ddf_sensor_sample_s *buf_ptr;
  static sns_ddf_sensor_data_s *sensor_data;
  uint8_t sensor_idx=0;
  uint32_t data_lux=0;

  /* allocate memory for one or both sensors as needed */
  size = num_sensors*sizeof(sns_ddf_sensor_data_s);
  if ((status = sns_ddf_malloc((void **)&sensor_data, size)) != SNS_DDF_SUCCESS )
  {
    /* Failed miserably. Can't even notify SMGR */
    return;
  }

  size = num_samples*sizeof(sns_ddf_sensor_sample_s);
  if ((status = sns_ddf_malloc((void **)&buf_ptr, size)) != SNS_DDF_SUCCESS )
  {
    /* Failed miserably. Can't even notify SMGR */
    return;
  }

  if ( sensor_type == SNS_DDF_SENSOR_AMBIENT )
  {
    /* saturate */
    data_lux = (data_val > 10000) ? 10000 : data_val;

    /* related to floating point library removal; change to FX_CONV_Q16(val,0) */
    buf_ptr[0].sample = FX_FLTTOFIX_Q16(data_lux);
    buf_ptr[0].status = SNS_DDF_SUCCESS;
    buf_ptr[1].sample = raw_data;                 /* ADC output */
    buf_ptr[1].status = SNS_DDF_SUCCESS;

    sns_dd_alsprx_log(dd_ptr, sensor_type, data_val, buf_ptr[0].sample, 0, 0, raw_data);
  }
  else
  {
    buf_ptr[0].sample = FX_CONV_Q16(prx_bin, 0);  /* proximity binary */
    buf_ptr[0].status = SNS_DDF_SUCCESS;
    buf_ptr[1].sample = raw_data;                 /* ADC output */
    buf_ptr[1].status = SNS_DDF_SUCCESS;
    sns_dd_alsprx_log(dd_ptr, sensor_type, prx_bin, buf_ptr[0].sample, 0, 0, raw_data);
  }

  sensor_data[sensor_idx].status = status;
  sensor_data[sensor_idx].sensor = sensor_type;
  sensor_data[sensor_idx].num_samples = num_samples;
  sensor_data[sensor_idx].timestamp = sns_ddf_get_timestamp();
  sensor_data[sensor_idx].samples = buf_ptr;

  sns_ddf_smgr_notify_data(dd_ptr->smgr_handle, sensor_data, num_sensors);

  sns_ddf_mfree(buf_ptr);
  sns_ddf_mfree(sensor_data);
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_als_calc_new_rng

DESCRIPTION   This function is called to calculate the range to use for the
              passed in milli-lux

DEPENDENCIES  None

RETURN VALUE  The range corresponding to the milli-lux

SIDE EFFECT   None

===========================================================================*/
static uint8_t sns_dd_alsprx_als_calc_new_rng (uint32_t data_mlux)
{
  uint8_t ret = SNS_DD_LTR55X_ALSPRX_ALS_RANGE_HIGH;

  if ( data_mlux <= sns_dd_ltr55x_als_rng_high_thresh_adj_tb[0] )
  {
    ret = SNS_DD_LTR55X_ALSPRX_ALS_RANGE_LOW;
  }

  return ret;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_als_data_ready

DESCRIPTION   This function is called when the ALS raw data is ready

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
static void sns_dd_alsprx_als_data_ready
(
  sns_dd_alsprx_state_t* dd_ptr
)
{
  uint32_t data_mlux;
  uint8_t  bytes_rw;
  uint8_t  i2c_data[2];
  uint16_t raw_data[2];
  uint32_t raw_data_full = 0;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  if ( dd_ptr->sns_dd_als_db.pend_state != SNS_DD_PEND_STATE_PEND && dd_ptr->dri_enable == false)
  {
    /* Ignore the response */
    SNS_PRINTF_STRING_ID_MEDIUM_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                  DBG_DD_ALSPRX_ALS_DATA_READY_ERR);
    return;
  }

   status = sns_ddf_read_port(dd_ptr->port_handle,
        LTR55X_PS_LED,
        (uint8_t*)&i2c_data[0],
        1,
        &bytes_rw);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
          DBG_DD_ALSPRX_I2C_RD_ERR,
          LTR55X_PS_LED,
          status);
      return;
    }

   status = sns_ddf_read_port(dd_ptr->port_handle,
        LTR55X_PS_MEAS_RATE,
        (uint8_t*)&i2c_data[0],
        1,
        &bytes_rw);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
          DBG_DD_ALSPRX_I2C_RD_ERR,
          LTR55X_PS_MEAS_RATE,
          status);
      return;
    }

  dd_ptr->sns_dd_als_db.pend_state = SNS_DD_PEND_STATE_IDLE;

/* read chan 1_0 data */
  status = sns_ddf_read_port(dd_ptr->port_handle,
                              LTR55X_ALS_DATA_CH1_0,
                              &i2c_data[0],
                              1,
                              &bytes_rw);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_I2C_RD_ERR,
                                 LTR55X_ALS_DATA_CH1_0,
                                 status);
    return;
  }

  /* read chan 1_1 data */
  status = sns_ddf_read_port(dd_ptr->port_handle,
                              LTR55X_ALS_DATA_CH1_1,
                              &i2c_data[1],
                              1,
                              &bytes_rw);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_I2C_RD_ERR,
                                 LTR55X_ALS_DATA_CH1_1,
                                 status);
    return;
  }
  raw_data[1] = (i2c_data[1]<<8 | i2c_data[0]);

/* read chan 0_0 data */
  status = sns_ddf_read_port(dd_ptr->port_handle,
                              LTR55X_ALS_DATA_CH0_0,
                              &i2c_data[0],
                              1,
                              &bytes_rw);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_I2C_RD_ERR,
                                 LTR55X_ALS_DATA_CH0_0,
                                 status);
    return;
  }

  /* read chan 0_1 data */
  status = sns_ddf_read_port(dd_ptr->port_handle,
                              LTR55X_ALS_DATA_CH0_1,
                              &i2c_data[1],
                              1,
                              &bytes_rw);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_I2C_RD_ERR,
                                 LTR55X_ALS_DATA_CH0_1,
                                 status);
    return;
  }
  raw_data[0] = (i2c_data[1]<<8 | i2c_data[0]);

  /* convert reading to mlux */
  data_mlux = sns_dd_alsprx_als_convert_to_mlux(dd_ptr,
                                                dd_ptr->sns_dd_als_db.req_rng_sel,
                                                raw_data);

  *(uint32_t *)&dd_ptr->sns_dd_als_db.als_data_cache = data_mlux;

  SNS_PRINTF_STRING_ID_LOW_3(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_ALS_RSP_READY,
                             raw_data_full,
                             *(uint32_t *)&dd_ptr->sns_dd_als_db.als_data_cache,
                             dd_ptr->sns_dd_als_db.req_rng_sel);

  sns_dd_alsprx_async_notify(dd_ptr, SNS_DDF_SENSOR_AMBIENT,
                             *(uint32_t *)&dd_ptr->sns_dd_als_db.als_data_cache, 0,
                             (uint32_t) raw_data[0]);

  dd_ptr->sns_dd_als_db.last_rng_sel = dd_ptr->sns_dd_als_db.req_rng_sel;
  /* Range adaption */
  dd_ptr->sns_dd_als_db.next_rng_sel = sns_dd_alsprx_als_calc_new_rng(data_mlux);
  dd_ptr->sns_dd_als_db.last_get_time = (uint32_t)sns_em_convert_dspstick_to_usec(sns_ddf_get_timestamp());
  dd_ptr->sns_dd_als_db.last_mlux  = data_mlux;

}

/*===========================================================================

FUNCTION      sns_dd_alsprx_prx_binary

DESCRIPTION   This function is called by the proximity common handler when proximity data
              is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
static void sns_dd_alsprx_prx_binary
(
  sns_dd_alsprx_state_t* dd_ptr,
  uint16_t               raw_data_unsigned
)
{
  uint8_t  i2c_data;
  uint8_t bytes_w;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
				   
  if ( raw_data_unsigned >= dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near )
  {
    if ( dd_ptr->sns_dd_prx_db.last_nearby != SNS_PRX_NEAR_BY )
    {
      SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_PRX_BINARY_NEAR);
      *(uint32_t *)&dd_ptr->sns_dd_prx_db.prx_data_cache = (uint32_t)SNS_PRX_NEAR_BY;
    }
    dd_ptr->sns_dd_prx_db.last_nearby = SNS_PRX_NEAR_BY;

  i2c_data = 0xff;
  status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_THRES_UP_0,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
   i2c_data = 0x07;
  status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_THRES_UP_1,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
   i2c_data = dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far & 0xff;
  status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_THRES_LOW_0,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
   i2c_data = (dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far >>8) & 0x07;
  status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_THRES_LOW_1,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
  }
  else if ( raw_data_unsigned < dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far )
  {
    if ( dd_ptr->sns_dd_prx_db.last_nearby != SNS_PRX_FAR_AWAY )
    {
      SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_PRX_BINARY_FAR);
      *(uint32_t *)&dd_ptr->sns_dd_prx_db.prx_data_cache = SNS_PRX_FAR_AWAY;
    }
    dd_ptr->sns_dd_prx_db.last_nearby = SNS_PRX_FAR_AWAY;

  i2c_data = dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near & 0xff;
  status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_THRES_UP_0,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
   i2c_data = (dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near >>8) & 0x07;
  status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_THRES_UP_1,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
   i2c_data = 0x0;
  status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_THRES_LOW_0,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
   i2c_data = 0x0;
  status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_THRES_LOW_1,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
  }

  if(status != SNS_DDF_SUCCESS)
  {
    DDF_MSG_1(ERROR, "sns_ddf_write_port error %d", status);
  }
}
/*===========================================================================

FUNCTION      sns_dd_alsprx_prx_data_ready

DESCRIPTION   This function is called when proximity data is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
static void sns_dd_alsprx_prx_data_ready
(
  sns_dd_alsprx_state_t* dd_ptr
)
{
  uint16_t raw_data_unsigned;
  uint8_t  raw_data[2], bytes_rw;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  /* read data */
  status = sns_ddf_read_port(dd_ptr->port_handle,
                              LTR55X_PS_DATA_0,
                              (uint8_t*)&raw_data[0],
                              1,
                              &bytes_rw);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_I2C_RD_ERR,
                                 LTR55X_PS_DATA_0,
                                 status);
    return;
  }

 status = sns_ddf_read_port(dd_ptr->port_handle,
                              LTR55X_PS_DATA_1,
                              (uint8_t*)&raw_data[1],
                              1,
                              &bytes_rw);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_I2C_RD_ERR,
                                 LTR55X_PS_DATA_0,
                                 status);
    return;
  }

  raw_data_unsigned = ((raw_data[1] & 7) << 0x8) | raw_data[0];

  /* TODO - Add proximity distance processing here */
  *(uint32_t *)&dd_ptr->sns_dd_prx_db.prxdist_data_cache = 0;

  if ( dd_ptr->sns_dd_prx_db.pend_state != SNS_DD_PEND_STATE_PEND && (dd_ptr->dri_enable == false))
  {
    /* Ignore the response */
    SNS_PRINTF_STRING_ID_MEDIUM_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                  DBG_DD_ALSPRX_PRX_NOT_PENDING);
  }
  else
  {
   /*TODO need add specail deal with polling mode*/
    dd_ptr->sns_dd_prx_db.pend_state = SNS_DD_PEND_STATE_IDLE;

    sns_dd_alsprx_prx_binary(dd_ptr, raw_data_unsigned); /* binary decision */

    sns_dd_alsprx_async_notify(dd_ptr,
                               SNS_DDF_SENSOR_PROXIMITY,
                               *(uint32_t *)&dd_ptr->sns_dd_prx_db.prxdist_data_cache,
                               dd_ptr->sns_dd_prx_db.prx_data_cache,
                               (uint32_t) raw_data_unsigned);

    SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_PRX_RSP_READY,
                               raw_data_unsigned,
                               *(uint32_t *)&dd_ptr->sns_dd_prx_db.prx_data_cache);
  }
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_data_ready

DESCRIPTION   This function is called when data ready indication(timer/irq)
              is received from SMGR

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
static void sns_dd_alsprx_data_ready
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e       sensor_type
)
{
  uint8_t     i2c_int_reg = 0;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t     bytes_rw;

if(dd_ptr->dri_enable == false) {
    if (sensor_type == SNS_DDF_SENSOR_AMBIENT)
    {
      /* indicate the data to the light(ALS) sub-driver */
      sns_dd_alsprx_als_data_ready(dd_ptr);
    }
  else if (sensor_type == SNS_DDF_SENSOR_PROXIMITY)
    {
      /* indicate the data to the proximity common handler */
      sns_dd_alsprx_prx_data_ready (dd_ptr);
    }
    else
    {
      SNS_PRINTF_STRING_ID_HIGH_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                  DBG_DD_ALSPRX_STRING2,
                                  500,
                                  sensor_type);
    }
}
else
{
  /* Read prox/als flag from interrupt register */
   status = sns_ddf_read_port(dd_ptr->port_handle,
                             LTR55X_ALS_PS_STATUS,
                             (uint8_t*)&i2c_int_reg,
                             1,
                             &bytes_rw);
   if ( status != SNS_DDF_SUCCESS )
   {
     SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_I2C_RD_ERR,
                                 LTR55X_ALS_PS_STATUS,
                                 i2c_int_reg);
     return;
  }

   if((i2c_int_reg & 0x0c) == 0x0c)
   {
       /* indicate the data to the light(ALS) sub-driver */
      sns_dd_alsprx_als_data_ready(dd_ptr);
   }
   if((i2c_int_reg & 0x03) == 0x03)
   {
       /* indicate the data to the proximity common handler */
       sns_dd_alsprx_prx_data_ready(dd_ptr);
   }
}
#ifdef ALSPRX_DEBUG_BASIC
  SNS_PRINTF_STRING_ID_HIGH_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                              DBG_DD_ALSPRX_STRING2,
                              700,
                              i2c_int_reg);
#endif /* ALSPRX_DEBUG_BASIC */
}
/*===========================================================================

  FUNCTION:   sns_dd_alsprx_ltr55x_handle_irq

===========================================================================*/
/*!
  @brief ltr55x interrupt handler

  @detail
  This function will be called within the context of the Interrupt
  Service Task (IST), *not* the ISR.

  @param[in] dd_handle  Handle to a driver instance.
  @param[in] irq        The IRQ representing the interrupt that occured.

  @return None
*/
/*=========================================================================*/
static void sns_dd_alsprx_ltr55x_handle_irq
(
  sns_ddf_handle_t handle,
  uint32_t         gpio_num,
  sns_ddf_time_t   timestamp
)
{
  sns_dd_alsprx_state_t* dd_ptr = (sns_dd_alsprx_state_t*)handle;

  if(dd_ptr->dri_enable == true)
  {
#ifdef ALSPRX_DEBUG_IRQ
  SNS_PRINTF_STRING_ID_MEDIUM_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                DBG_DD_ALSPRX_HANDLE_IRQ);
#endif
  sns_dd_alsprx_data_ready(dd_ptr, SNS_DDF_SENSOR__NONE);
  }
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_ltr55x_handle_timer

===========================================================================*/
/*!
  @brief Called when the timer set by this driver has expired. This is
         the callback function submitted when initializing a timer.

  @detail
  This will be called within the context of the Sensors Manager task.
  It indicates that sensor data is ready

  @param[in] dd_handle  Handle to a driver instance.
  @param[in] arg        The argument submitted when the timer was set.

  @return None
*/
/*=========================================================================*/
static void sns_dd_alsprx_ltr55x_handle_timer
(
  sns_ddf_handle_t handle,
  void* arg
)
{
  sns_dd_alsprx_state_t* dd_ptr = (sns_dd_alsprx_state_t*)handle;
  sns_ddf_sensor_e       sensor_type=*(sns_ddf_sensor_e*)arg;

  if(dd_ptr->dri_enable == false || dd_ptr->sns_dd_alsprx_common_db.als_calibration || dd_ptr->sns_dd_alsprx_common_db.prx_calibration)
  {

#ifdef ALSPRX_DEBUG_TIMER
  SNS_PRINTF_STRING_ID_MEDIUM_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                DBG_DD_ALSPRX_HANDLE_TIMER);
#endif

  if ((sensor_type != SNS_DDF_SENSOR_AMBIENT) &&
      (sensor_type != SNS_DDF_SENSOR_PROXIMITY))
  {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_STRING2,
                                 800,
                                 sensor_type);
    return;
  }

  sns_dd_stop_general_timer(dd_ptr, sensor_type);
  sns_dd_alsprx_data_ready(dd_ptr, sensor_type);
  }
}

/*==============================================================================

FUNCTION      sns_dd_alsprx_als_init

DESCRIPTION   Initializes the light(ALS) driver

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

==============================================================================*/
static void sns_dd_alsprx_als_init (sns_dd_alsprx_state_t* dd_ptr)
{
  sns_dd_als_db_type *als_db_ptr = &dd_ptr->sns_dd_als_db;
  sns_dd_alsprx_common_db_type *common_db_ptr = &dd_ptr->sns_dd_alsprx_common_db;
  int8_t i;

  SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_ALS_INIT);
  als_db_ptr->pend_state        = SNS_DD_PEND_STATE_IDLE;
  als_db_ptr->last_rng_sel      = SNS_DD_LTR55X_ALS_DEF_RNG;
  als_db_ptr->next_rng_sel      = SNS_DD_LTR55X_ALS_DEF_RNG;
  als_db_ptr->req_rng_sel       = SNS_DD_LTR55X_ALS_DEF_RNG;
  als_db_ptr->cycle_time     = SNS_DD_LTR55X_ALS_CYCLE_TIME;
  als_db_ptr->last_get_time     = (uint32_t)sns_em_convert_dspstick_to_usec(sns_ddf_get_timestamp());
  als_db_ptr->last_mlux         = SNS_DD_ALS_DFLT_MILLI_LUX;

#define ALS_CONVERSION  (common_db_ptr->nv_db.als_factor / common_db_ptr->nv_db.visible_ratio)
  als_db_ptr->als_conversion_lo = (uint32_t)SNS_DD_ALSPRX_ALPHA_LO * ALS_CONVERSION / 10;
  als_db_ptr->als_conversion_hi = (uint32_t)SNS_DD_ALSPRX_ALPHA_HI * ALS_CONVERSION / 10;

  /* adjust the high threshold, min, and max mlux tables for Light by the light trans ratio */
  for ( i = 0; i < SNS_DD_LTR55X_ALS_RNG_TB_SIZE; i++ )
  {
    sns_dd_ltr55x_als_rng_high_thresh_adj_tb[i] = sns_dd_ltr55x_als_rng_high_thresh_tb[i];
  }

}

/*==============================================================================

FUNCTION      sns_dd_alsprx_prx_init

DESCRIPTION   initializes the LTR55x ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

==============================================================================*/
static void sns_dd_alsprx_prx_init ( sns_dd_alsprx_state_t* dd_ptr )
{
  sns_dd_prx_db_type     *prx_db_ptr     = &dd_ptr->sns_dd_prx_db;

  SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_PRX_INIT);
  prx_db_ptr->pend_state = SNS_DD_PEND_STATE_IDLE;
  prx_db_ptr->last_nearby = SNS_PRX_NEAR_BY_UNKNOWN;
  prx_db_ptr->cycle_time = SNS_DD_LTR55X_PRX_CYCLE_TIME;
}

/*==============================================================================

FUNCTION      sns_dd_alsprx_common_init

DESCRIPTION   initializes the LTR55x ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

==============================================================================*/
static void sns_dd_alsprx_common_init ( sns_dd_alsprx_state_t* dd_ptr )
{
  dd_ptr->dropped_logs = 0;
  dd_ptr->sns_dd_alsprx_common_db.state = SNS_DD_DEV_STOPPED;
  if(dd_ptr->dri_enable == false) {

  dd_ptr->sns_dd_alsprx_common_db.als_timer.active = FALSE;
  dd_ptr->sns_dd_alsprx_common_db.als_timer.defined = FALSE;
  dd_ptr->sns_dd_alsprx_common_db.prx_timer.active = FALSE;
  dd_ptr->sns_dd_alsprx_common_db.prx_timer.defined = FALSE;
    }
  /* initialize NV item db to default values if needed */
  if (dd_ptr->sns_dd_alsprx_common_db.nv_source == SNS_DD_ALSPRX_NV_SOURCE_DEFAULT)
  {
    dd_ptr->sns_dd_alsprx_common_db.prx_calibration = false;
  dd_ptr->sns_dd_alsprx_common_db.als_calibration = false;
    dd_ptr->sns_dd_alsprx_common_db.nv_db.visible_ratio = SNS_DD_VISIBLE_TRANS_RATIO;
    dd_ptr->sns_dd_alsprx_common_db.nv_db.ir_ratio = SNS_DD_IR_TRANS_RATIO;
    dd_ptr->sns_dd_alsprx_common_db.nv_db.dc_offset = SNS_DD_DC_OFFSET;
    dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near = SNS_DD_PRX_THRESH_NEAR;
    dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far  = SNS_DD_PRX_THRESH_FAR;
    dd_ptr->sns_dd_alsprx_common_db.nv_db.prx_factor  = SNS_DD_PRX_FACTOR;
    dd_ptr->sns_dd_alsprx_common_db.nv_db.als_factor  = SNS_DD_ALS_FACTOR;
    dd_ptr->sns_dd_alsprx_common_db.nv_db.version_num = 1;
    dd_ptr->sns_dd_alsprx_common_db.nv_db.id = LTR55X_MANUFACTURER_ID;

  sns_ddf_smgr_notify_event(dd_ptr->smgr_handle, SNS_DDF_SENSOR_PROXIMITY, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
#ifdef ALSPRX_DEBUG_REG
    SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_NV_USE_DRIVER_DEFAULTS);
#endif
  }

#ifdef ALSPRX_DEBUG_REG
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_NV_PARAMS,
                               dd_ptr->sns_dd_alsprx_common_db.nv_source);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_NV_PARAMS,
                               dd_ptr->sns_dd_alsprx_common_db.nv_db.visible_ratio);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_NV_PARAMS,
                               dd_ptr->sns_dd_alsprx_common_db.nv_db.ir_ratio);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_NV_PARAMS,
                               dd_ptr->sns_dd_alsprx_common_db.nv_db.dc_offset);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_NV_PARAMS,
                               dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_NV_PARAMS,
                               dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_NV_PARAMS,
                               dd_ptr->sns_dd_alsprx_common_db.nv_db.prx_factor);
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_NV_PARAMS,
                               dd_ptr->sns_dd_alsprx_common_db.nv_db.als_factor);
#endif /* ALSPRX_DEBUG_REG */

}

/*===========================================================================

FUNCTION      sns_dd_alsprx_init_data_struct

DESCRIPTION   Initialize data structures for ALS and PRX

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
static void sns_dd_alsprx_init_data_struct
(
  sns_dd_alsprx_state_t* dd_ptr
)
{
  /* Initialize the common database */
  sns_dd_alsprx_common_init(dd_ptr);

  /* Initialize ALS database */
  sns_dd_alsprx_als_init(dd_ptr);

  /* Initialize PRX database */
  sns_dd_alsprx_prx_init(dd_ptr);
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_set_powerstate

DESCRIPTION   Sets the power state of the LTR55X ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

===========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_set_powerstate
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_powerstate_e   op_mode,
  boolean                init_data
)
{
  uint16_t   i2c_data;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t          bytes_w;

  if (EnableI2C != TRUE)
    return SNS_DDF_SUCCESS;

  if ( op_mode == SNS_DDF_POWERSTATE_ACTIVE )
  {

  if(dd_ptr->dri_enable == true)
  {
    /* Set thresholds such that interrupt is generated after every Mux ADC conversion.
    and PX bigger than the High threshold,
    High threshold < Low threshold will do this. */
    i2c_data = dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near & 0xff;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_THRES_UP_0,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   LTR55X_PS_THRES_UP_0,
                                   status);
    return status;
    }
    i2c_data = (dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near >> 8) & 0xff;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_THRES_UP_1,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   LTR55X_PS_THRES_UP_1,
                                   status);
      return status;
    }

    i2c_data = 0x0;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_THRES_LOW_0,
                                (uint8_t*)&i2c_data,
                                2,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   LTR55X_PS_THRES_LOW_0,
                                   status);
      return status;
    }

    i2c_data = 0x0;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_THRES_LOW_1,
                                (uint8_t*)&i2c_data,
                                2,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   LTR55X_PS_THRES_LOW_1,
                                   status);
    return status;
    }

    i2c_data = 0x0;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_ALS_THRES_UP_0,
                                (uint8_t*)&i2c_data,
                                2,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
      {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   LTR55X_ALS_THRES_UP_0,
                                   status);
    return status;
    }

    i2c_data = 0x0;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_ALS_THRES_UP_1,
                                (uint8_t*)&i2c_data,
                                2,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   LTR55X_ALS_THRES_UP_1,
                                   status);
      return status;
    }

    i2c_data = 0xFF;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_ALS_THRES_LOW_0,
                                (uint8_t*)&i2c_data,
                                2,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   LTR55X_ALS_THRES_LOW_0,
                                   status);
      return status;
    }

    i2c_data = 0xFF;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_ALS_THRES_LOW_1,
                                (uint8_t*)&i2c_data,
                                2,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   LTR55X_ALS_THRES_LOW_1,
                                   status);
      return status;
    }
    }

  /*check the device is ltr558 or ltr559 */
  status = sns_ddf_read_port(dd_ptr->port_handle,
        LTR55X_PART_ID,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
          DBG_DD_ALSPRX_I2C_RD_ERR,
          LTR55X_PART_ID,
          status);
    return status;
  }
  if((i2c_data & 0xFF) == SNS_DD_LTR558_ID)
  {
    dd_ptr->ltr_type = SNS_DD_LTR558;
  }
  else if((i2c_data & 0xFF) == SNS_DD_LTR559_ID)
  {
    dd_ptr->ltr_type = SNS_DD_LTR559;
  }
  else
    return SNS_DDF_EFAIL;

  /*Initilize the sensor.*/
    i2c_data = 0x08;
  status = sns_ddf_write_port(dd_ptr->port_handle,
        LTR55X_PS_N_PULSES,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);

  i2c_data = 0x0;
  status = sns_ddf_write_port(dd_ptr->port_handle,
        LTR55X_PS_MEAS_RATE,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);

    i2c_data = SNS_DD_LTR55X_ALSPRX_PWR_DOWN;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_ALS_CONTR,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   LTR55X_ALS_CONTR,
                                   status);
      return status;
    }
  }
  else /* when op_mode is SNS_DDF_POWERSTATE_LOWPOWER */
  {
  if(dd_ptr->dri_enable == false) {

    /* stop any pending timers */
    sns_dd_stop_general_timer( dd_ptr, SNS_DDF_SENSOR_AMBIENT );
    sns_dd_stop_general_timer( dd_ptr, SNS_DDF_SENSOR_PROXIMITY );
  }

    if (init_data == TRUE)
    {
      /* cleanup */
      sns_dd_alsprx_init_data_struct(dd_ptr);
    }
#ifdef ALSPRX_DEBUG_BASIC
  /* Read current config before writing */
  status = sns_ddf_read_port(dd_ptr->port_handle,
                             LTR55X_ALS_CONTR,
                             (uint8_t*)&i2c_data,
                             1,
                             &bytes_w);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_I2C_RD_ERR,
                                 LTR55X_ALS_CONTR,
                                 status);
    return status;
  }
#endif

  i2c_data  = SNS_DD_LTR55X_ALSPRX_PWR_DOWN;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_ALS_CONTR,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   LTR55X_ALS_CONTR,
                                   status);
      return status;
    }
   }

  dd_ptr->sns_dd_alsprx_common_db.pwr_mode = op_mode;
  dd_ptr->sns_dd_alsprx_common_db.state = SNS_DD_DEV_CONFIGURED;

  return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_ltr55x_reset

===========================================================================*/
/*!
  @brief Resets the driver and device so they return to the state they were
         in after init() was called.

  @detail
  Resets  the driver state structure

  @param[in] handle  Handle to a driver instance.

  @return Success if the driver was able to reset its state and the device.
          Otherwise a specific error code is returned.
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_alsprx_ltr55x_reset
(
  sns_ddf_handle_t dd_handle
)
{
  sns_dd_alsprx_state_t* dd_ptr = (sns_dd_alsprx_state_t *)dd_handle;
  sns_ddf_status_e init_status = SNS_DDF_SUCCESS;

#ifdef ALSPRX_DEBUG_BASIC
  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_STRING1,
                             200);
#endif /* ALSPRX_DEBUG_BASIC */

  if ( dd_handle == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  sns_dd_alsprx_init_data_struct(dd_ptr);

  if ( (init_status = sns_dd_alsprx_set_powerstate(dd_ptr,
                          SNS_DDF_POWERSTATE_LOWPOWER, FALSE)) != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_SET_PWR_ST_ERR);
    return init_status;
  }

  return init_status;
}

/*===========================================================================

FUNCTION      sns_dd_ltr55x_enable_interrupt

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_ltr55x_enable_interrupt
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e  sensor
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t   i2c_data = 0x0;
  uint8_t   bytes_w;
  uint8_t  interrupt = 0x0;

  if ((sensor != SNS_DDF_SENSOR_PROXIMITY) && (sensor != SNS_DDF_SENSOR_AMBIENT))
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  status = sns_ddf_signal_register(dd_ptr->interrupt_gpio, dd_ptr,
                                   &sns_alsprx_ltr55x_driver_fn_list, SNS_DDF_SIGNAL_IRQ_FALLING);
  if(dd_ptr->dri_enable == true)
  {
    i2c_data = 0x0;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_ALS_THRES_UP_0,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   LTR55X_ALS_THRES_UP_0,
                                   status);
      return status;
    }

    i2c_data = 0x0;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_ALS_THRES_UP_1,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   LTR55X_ALS_THRES_UP_1,
                                   status);
      return status;
    }

    i2c_data = 0xFF;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_ALS_THRES_LOW_0,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   LTR55X_ALS_THRES_LOW_0,
                                   status);
      return status;
    }

    i2c_data = 0xFF;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_ALS_THRES_LOW_1,
                                (uint8_t*)&i2c_data,
                                1,
                                &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_I2C_WR_ERR,
                                   LTR55X_ALS_THRES_LOW_1,
                                   status);
      return status;
    }
  } /* end of dri == true */

  /* configre the INTERRUPT register*/
  status = sns_ddf_read_port(dd_ptr->port_handle,
        LTR55X_INTERRUPT,
        (uint8_t*)&interrupt,
        1,
        &bytes_w);

  if (sensor == SNS_DDF_SENSOR_PROXIMITY)
  {
    i2c_data = interrupt | 0x01;
  }
  else if (sensor == SNS_DDF_SENSOR_AMBIENT)
  {
    i2c_data = interrupt | 0x02;
  }
    status = sns_ddf_write_port(dd_ptr->port_handle,
        LTR55X_INTERRUPT,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);
    if ( status != SNS_DDF_SUCCESS )
    {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
          DBG_DD_ALSPRX_I2C_RD_ERR,
          LTR55X_INTERRUPT,
          status);
      return status;
    }

  if(  dd_ptr->ltr_type == SNS_DD_LTR558)
  {
  if (sensor == SNS_DDF_SENSOR_AMBIENT)
  {
    i2c_data = SNS_DD_LTR55X_ALSPRX_ALS_MODE_EN;
    status = sns_ddf_write_port(dd_ptr->port_handle,
        LTR55X_ALS_CONTR,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);
     status = sns_ddf_read_port(dd_ptr->port_handle,
        LTR55X_ALS_CONTR,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
          DBG_DD_ALSPRX_I2C_RD_ERR,
          LTR55X_ALS_CONTR,
          i2c_data);
    }
  else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
  {
    i2c_data= SNS_DD_LTR55X_ALSPRX_PROX_MODE_EN;
    status = sns_ddf_write_port(dd_ptr->port_handle,
        LTR55X_PS_CONTR,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
        LTR55X_PS_CONTR,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
          DBG_DD_ALSPRX_I2C_RD_ERR,
          LTR55X_PS_CONTR,
          i2c_data);
  }
  }
  else  /* LTR559 */
  {
    if (sensor == SNS_DDF_SENSOR_AMBIENT)
  {
    i2c_data= SNS_DD_LTR559_ALSPRX_ALS_MODE_EN;
    status = sns_ddf_write_port(dd_ptr->port_handle,
        LTR55X_ALS_CONTR,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);
     status = sns_ddf_read_port(dd_ptr->port_handle,
        LTR55X_ALS_CONTR,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
          DBG_DD_ALSPRX_I2C_RD_ERR,
          LTR55X_ALS_CONTR,
          i2c_data);
    }
  else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
  {
    i2c_data= SNS_DD_LTR55X_ALSPRX_PROX_MODE_EN;
    status = sns_ddf_write_port(dd_ptr->port_handle,
        LTR55X_PS_CONTR,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);
      status = sns_ddf_read_port(dd_ptr->port_handle,
        LTR55X_PS_CONTR,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
          DBG_DD_ALSPRX_I2C_RD_ERR,
          LTR55X_PS_CONTR,
          i2c_data);
  }
  }
  return status;
}

/*===========================================================================

FUNCTION      sns_dd_ltr55x_disable_interrupt

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_ltr55x_disable_interrupt
(
  sns_dd_alsprx_state_t* dd_ptr,
  sns_ddf_sensor_e  sensor
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t     i2c_data = 0x0;
  uint8_t     bytes_w;
  uint8_t    interrupt = 0x0;

/* configre the INTERRUPT register*/
  status = sns_ddf_read_port(dd_ptr->port_handle,
        LTR55X_INTERRUPT,
        (uint8_t*)&interrupt,
        1,
        &bytes_w);

  if (sensor == SNS_DDF_SENSOR_PROXIMITY)
  {
    i2c_data = (interrupt & ~0x01);
  }
  else if (sensor == SNS_DDF_SENSOR_AMBIENT)
  {
    i2c_data = (interrupt & ~0x02);
  }
    status = sns_ddf_write_port(dd_ptr->port_handle,
        LTR55X_INTERRUPT,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);
  if ( status != SNS_DDF_SUCCESS )
  {
      SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
          DBG_DD_ALSPRX_I2C_RD_ERR,
          LTR55X_INTERRUPT,
          status);
      return status;
  }

  if(!(i2c_data & 0x03))
  {
    /* De-register INT notification */
    status = sns_ddf_signal_deregister(dd_ptr->interrupt_gpio);
  }

  if (sensor == SNS_DDF_SENSOR_AMBIENT)
  {
    i2c_data= 0x0;
    status = sns_ddf_write_port(dd_ptr->port_handle,
        LTR55X_ALS_CONTR,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);
  }
  else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
  {
    i2c_data= 0x0;
    status = sns_ddf_write_port(dd_ptr->port_handle,
        LTR55X_PS_CONTR,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);
  }
  return status;

}

/*===========================================================================

FUNCTION      sns_dd_alsprx_ltr55x_enable_sched_data

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_alsprx_ltr55x_enable_sched_data
(
  sns_ddf_handle_t  handle,
  sns_ddf_sensor_e  sensor,
  bool              enable
)
{
  sns_dd_alsprx_state_t* dd_ptr = (sns_dd_alsprx_state_t*)handle;

  if(enable)
  {
      return sns_dd_ltr55x_enable_interrupt(dd_ptr, sensor);
  }
  else
  {
     return sns_dd_ltr55x_disable_interrupt(dd_ptr, sensor);
  }

}

/*===========================================================================

FUNCTION      sns_dd_ltr55x_prx_set_cycle_time

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_ltr55x_prx_set_cycle_time
(
  sns_dd_alsprx_state_t* dd_ptr
)
{
  uint32_t max_odr;
  uint32_t odr;
  uint8_t i2c_data = 0x2;
  uint8_t bytes_w;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  max_odr = dd_ptr->sns_dd_prx_db.prx_odr;

  if (max_odr >= 17)
  {
    /* resulting freq = 20Hz */
    odr = 20; /* Hz */
    dd_ptr->sns_dd_prx_db.cycle_time = 50000; /* us */
  i2c_data = 0x0;
  }
  else if (max_odr >= 12)
  {
    /* resulting freq = 16.8Hz */
    odr = 14;
    dd_ptr->sns_dd_prx_db.cycle_time = 70000;
  i2c_data = 0x1;
  }
  else if (max_odr >= 8)
  {
    /* resulting freq = 12.8Hz */
    odr = 10;
    dd_ptr->sns_dd_prx_db.cycle_time = 100000;
  i2c_data = 0x2;
  }
  else if (max_odr >= 4)
  {
    /* resulting freq = 8.9Hz */
    odr = 5;
    dd_ptr->sns_dd_prx_db.cycle_time = 200000;
  i2c_data = 0x3;
  }
  else if (max_odr >= 2)
  {
    /* resulting freq = 4.7Hz */
    /* (max_odr > 0) && (max_odr <=5) */
    odr = 2;
    dd_ptr->sns_dd_prx_db.cycle_time = 500000;
  i2c_data = 0x4;
  }
  else if (max_odr >= 1)
  {
    /* resulting freq = 4.7Hz */
    /* (max_odr > 0) && (max_odr <=5) */
    odr = 1;
    dd_ptr->sns_dd_prx_db.cycle_time = 1000000;
  i2c_data = 0x5;
  }
  else
  {
    odr = 0;
    dd_ptr->sns_dd_prx_db.cycle_time = 2000000;
  i2c_data = 0x6;
  }

  status = sns_ddf_write_port(dd_ptr->port_handle,
         LTR55X_PS_MEAS_RATE,
         (uint8_t*)&i2c_data,
         1,
         &bytes_w);
  if ( status != SNS_DDF_SUCCESS )
  {
       SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
           DBG_DD_ALSPRX_I2C_RD_ERR,
           LTR55X_PS_MEAS_RATE,
           i2c_data);
       return status;
  }

  dd_ptr->sns_dd_prx_db.prx_odr = odr; /* Hz */
  return status;
}

/*===========================================================================

FUNCTION      sns_dd_ltr55x_als_set_cycle_time

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_ltr55x_als_set_cycle_time
(
  sns_dd_alsprx_state_t* dd_ptr
)
{
  uint32_t max_odr;
  uint32_t odr;
  uint8_t i2c_data = 0x3;
  uint8_t bytes_w;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  max_odr = dd_ptr->sns_dd_als_db.als_odr;

  if (max_odr >= 15)
  {
    /* resulting freq = 20Hz */
    odr = 20; /* Hz */
    dd_ptr->sns_dd_als_db.cycle_time = 50000; /* us */
  i2c_data = 0x0;
  }
  else if (max_odr >= 8)
  {
    /* resulting freq = 16.8Hz */
    odr = 10;
    dd_ptr->sns_dd_als_db.cycle_time = 100000;
  i2c_data = 0x1;
  }
  else if (max_odr >= 7)
  {
    /* resulting freq = 12.8Hz */
    odr = 7;
    dd_ptr->sns_dd_als_db.cycle_time = 150000;
  i2c_data = 0x22;
  }
  else if (max_odr >= 5)
  {
    /* resulting freq = 8.9Hz */
    odr = 5;
    dd_ptr->sns_dd_als_db.cycle_time = 200000;
  i2c_data = 0x12;
  }
  else if (max_odr >= 4)
  {
    /* resulting freq = 8.9Hz */
    odr = 4;
    dd_ptr->sns_dd_als_db.cycle_time = 250000;
  i2c_data = 0x2b;
  }
  else if (max_odr >= 3)
  {
    /* resulting freq = 8.9Hz */
    odr = 3;
    dd_ptr->sns_dd_als_db.cycle_time = 300000;
  i2c_data = 0x33;
  }
    else if (max_odr >= 2)
  {
    /* resulting freq = 8.9Hz */
    odr = 2;
    dd_ptr->sns_dd_als_db.cycle_time = 400000;
  i2c_data = 0x1b;
  }
  else
  {
    odr = 0;
    dd_ptr->sns_dd_als_db.cycle_time = 0;
  i2c_data = 0x3;
  }
  status = sns_ddf_write_port(dd_ptr->port_handle,
        LTR55X_ALS_MEAS_RATE,
        (uint8_t*)&i2c_data,
        1,
        &bytes_w);
  if ( status != SNS_DDF_SUCCESS )
  {
    SNS_PRINTF_STRING_ID_ERROR_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
        DBG_DD_ALSPRX_I2C_RD_ERR,
        LTR55X_ALS_MEAS_RATE,
        status);
    return status;
  }

  dd_ptr->sns_dd_als_db.als_odr = odr; /* Hz */
  return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_ltr55x_set_attrib

===========================================================================*/
/*!
  @brief Sets an attribute of the LTR55X ALS/Prx sensor

  @detail
  Called by SMGR to set certain device attributes that are
  programmable. Curently its the power mode and range.

  @param[in] dd_handle   Handle to a driver instance.
  @param[in] sensor Sensor for which this attribute is to be set.
  @param[in] attrib      Attribute to be set.
  @param[in] value      Value to set this attribute.

  @return
    The error code definition within the DDF
    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
    SNS_DDF_EINVALID_PARAM

*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_ltr55x_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  sns_dd_alsprx_state_t* dd_ptr = dd_handle;
  bool             change=TRUE;

#ifdef ALSPRX_DEBUG_BASIC
  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_STRING1,
                             300);
#endif /* ALSPRX_DEBUG_BASIC */

  if ( (dd_handle == NULL) || (value == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_POWER_STATE:
      /* set power mode */
      status = sns_dd_alsprx_set_powerstate((sns_dd_alsprx_state_t *)dd_handle,
                                            *(sns_ddf_powerstate_e *)value,
                                            TRUE);
      break;
  case SNS_DDF_ATTRIB_ODR:
      if(sensor == SNS_DDF_SENSOR_PROXIMITY)
      {
    if(dd_ptr->sns_dd_prx_db.prx_odr == *(uint32_t*)value)
    {
      change = FALSE;
        }
        dd_ptr->sns_dd_prx_db.prx_odr = *(uint32_t*)value;
    if (change)
      {
      status = sns_dd_ltr55x_prx_set_cycle_time(dd_ptr);
    }
      }
      else if(sensor == SNS_DDF_SENSOR_AMBIENT)
      {
        if(dd_ptr->sns_dd_als_db.als_odr == *(uint32_t*)value)
        {
      change = FALSE;
        }
        dd_ptr->sns_dd_als_db.als_odr = *(uint32_t*)value;
      if (change)
      {
      status = sns_dd_ltr55x_als_set_cycle_time(dd_ptr);
      }
      }
      else
      {
          return SNS_DDF_EINVALID_ATTR;
      }
    break;
    default:
      return SNS_DDF_EINVALID_PARAM;
  }
  return(status);
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_ltr55x_run_test

===========================================================================*/
/*!
  @ Called by the SMGR to run calibration test for ALS or PRX.

  @detail
  Returns the cal_factor in error, to be stored in NV memory

  @param[in]  handle      Handle to a driver instance.
  @param[in]  sensor_type Sensor whose attribute is to be retrieved.
  @param[in] test         Test ID used to decide what to test.
  @param[out] err         prx_factor or als_factor.

  @return
    Success if no error. Otherwise a specific error code is returned.
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_alsprx_ltr55x_run_test
(
  sns_ddf_handle_t    handle,
  sns_ddf_sensor_e    sensor_type,
  sns_ddf_test_e      test,
  uint32_t*           err
)
{
  uint8_t bytes_rw;
  uint8_t i2c_data;
  uint16_t raw_data_unsigned=0, raw_data_unsigned1=0;
  uint8_t  raw_data[2] = {0}, i2c_reg = 0, i2c_config_reg = 0;
  int i = 0;
  sns_dd_alsprx_state_t* dd_ptr = (sns_dd_alsprx_state_t *)handle;
  sns_ddf_status_e status, status2;
  *err = 0;
  status = SNS_DDF_EFAIL;
  status2  = SNS_DDF_EFAIL;


  switch (test) {
  case SNS_DDF_TEST_SELF:         /**< Hardware self test. */
    
    status = sns_ddf_read_port(dd_ptr->port_handle,
                              LTR55X_PS_CONTR,
                              (uint8_t*)&i2c_reg,
                              1,
                              &bytes_rw);
    SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX, DBG_DD_ALSPRX_I2C_RD_ERR,  0x00, i2c_reg);

  if (0 == i2c_reg)
  {	
    i2c_config_reg = 0x08;  //0x83
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_N_PULSES,
                                &i2c_config_reg,
                                1,
                                &bytes_rw);

    i2c_config_reg = 0x7f;    //0x82
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_LED,
                                &i2c_config_reg,
                                1,
                                &bytes_rw);

    i2c_config_reg = 0x00;   //0x84
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_MEAS_RATE,
                                &i2c_config_reg,
                                1,
                                &bytes_rw);

    i2c_config_reg = SNS_DD_LTR55X_ALSPRX_PROX_MODE_EN;   //0x81
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_CONTR,
                                &i2c_config_reg,
                                1,
                                &bytes_rw);
    
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               LTR55X_PS_CONTR,
                               (uint8_t*)&i2c_reg,
                               1,
                               &bytes_rw);

    while (1)
    {
      status = sns_ddf_read_port(dd_ptr->port_handle,
                                 LTR55X_PS_DATA_0, 
				 (uint8_t*)&raw_data[0], 
				 1, 
				 &bytes_rw);
				 
      status = sns_ddf_read_port(dd_ptr->port_handle, 
                                 LTR55X_PS_DATA_1, 
				 (uint8_t*)&raw_data[1], 
				 1, 
				 &bytes_rw);
      raw_data_unsigned = ((raw_data[1] & 7) << 0x8) | raw_data[0];

      if (0 == raw_data_unsigned)
	continue;

      raw_data_unsigned1 += raw_data_unsigned;
      i++;
      if (8 == i)
        break;
    }
    raw_data_unsigned = raw_data_unsigned1 >> 3;

    if (raw_data_unsigned <= 200) {
      dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near = raw_data_unsigned << 1;
      dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far  = raw_data_unsigned << 1;
    } else if (raw_data_unsigned <= 600) {
      dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near = raw_data_unsigned + 150;
      dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far  = raw_data_unsigned + 150;
    } else {
      dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_near = 800;
      dd_ptr->sns_dd_alsprx_common_db.nv_db.thresh_far  = 800;
    }

    sns_ddf_smgr_notify_event(dd_ptr->smgr_handle,
                              SNS_DDF_SENSOR_PROXIMITY,
                              SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);

    i2c_config_reg = 0;   //0x81
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                LTR55X_PS_CONTR,
                                &i2c_config_reg,
                                1,
                                &bytes_rw);
  }


    status2 = SNS_DDF_SUCCESS;
    break;
  case SNS_DDF_TEST_IRQ:          /**< Interrupt test. */
    status2 = SNS_DDF_EINVALID_TEST;
    break;
  case SNS_DDF_TEST_CONNECTIVITY:  /**< Basic connectivity test. */
    {
      status = sns_ddf_read_port(dd_ptr->port_handle,
        LTR55X_MANUFACTURER_ID,
        (uint8_t*)&i2c_data,
        1,
        &bytes_rw);
      if (status != SNS_DDF_SUCCESS) break;
      if (i2c_data != 0x05) break;
      status2 = SNS_DDF_SUCCESS;
    }
    break;
  default:
    status = SNS_DDF_EINVALID_ATTR;
    break;
  }

  if (status == SNS_DDF_SUCCESS)
    return status2;
  else
    return status;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_prx_prepare_data_from_device

DESCRIPTION   For preparing a sensor data, this function issues a request to
              the proximity common handler

DEPENDENCIES  None

RETURN VALUE  if error, return TRUE, else FALSE

SIDE EFFECT   None

===========================================================================*/
static BOOLEAN sns_dd_alsprx_prx_prepare_data_from_device(sns_dd_alsprx_state_t* dd_ptr)
{
  BOOLEAN err=TRUE;

  if ( dd_ptr->sns_dd_prx_db.pend_state == SNS_DD_PEND_STATE_IDLE )
  {
    if(sns_dd_alsprx_req_data (dd_ptr,SNS_DDF_SENSOR_PROXIMITY,0) == SNS_DDF_SUCCESS)
    {
      dd_ptr->sns_dd_prx_db.pend_state = SNS_DD_PEND_STATE_PEND;
      err = FALSE;
    }
  }
  else
  {
    /* Already PEND state, so nothing is to be done in here */
    SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_PRX_STATE_PEND);
  }
  return err;
}

/*===========================================================================

FUNCTION      sns_dd_alsprx_als_prepare_data_from_device

DESCRIPTION   For preparing a sensor data, this function issues a request to
              the arbitration manager

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
static BOOLEAN  sns_dd_alsprx_als_prepare_data_from_device (sns_dd_alsprx_state_t* dd_ptr)
{
  BOOLEAN err=TRUE;
  uint8_t rng_sel;

  if ( dd_ptr->sns_dd_als_db.pend_state == SNS_DD_PEND_STATE_IDLE )
  {
    /* if the last sampling time is not long time ago, use the same range value */
    if ( ((uint32_t)sns_em_convert_dspstick_to_usec(sns_ddf_get_timestamp()) -
          dd_ptr->sns_dd_als_db.last_get_time) > SNS_DD_ALS_LONG_TIME_AGO_US )
    {
      /* start with default range */
      rng_sel = SNS_DD_LTR55X_ALS_DEF_RNG;
    }
    else
    {
      /* use the next range selection */
      rng_sel = dd_ptr->sns_dd_als_db.next_rng_sel;
    }
    if (sns_dd_alsprx_req_data(dd_ptr, SNS_DDF_SENSOR_AMBIENT, rng_sel) == SNS_DDF_SUCCESS)
    {
      dd_ptr->sns_dd_als_db.req_rng_sel = rng_sel;
      dd_ptr->sns_dd_als_db.pend_state = SNS_DD_PEND_STATE_PEND;
      err = FALSE;
    }
  }
  else
  {
    SNS_PRINTF_STRING_ID_LOW_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_DEV_STATE_PEND);
  }
  return err;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_ltr55x_get_data

===========================================================================*/
/*!
  @brief Called by the SMGR to get data

  @detail
  Requests a single sample of sensor data from each of the specified
  sensors. Data is returned immediately after being read from the
  sensor, in which case data[] is populated in the same order it was
  requested

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

  @return SNS_DDF_SUCCESS if data was populated successfully. If any of the
          sensors queried are to be read asynchronously SNS_DDF_PENDING is
          returned and data is via @a sns_ddf_smgr_data_notify() when
          available. Otherwise a specific error code is returned.

*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_alsprx_ltr55x_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
)
{
  sns_ddf_status_e status = SNS_DDF_PENDING;
  uint8_t          i;
  sns_dd_alsprx_state_t* dd_ptr = (sns_dd_alsprx_state_t *)dd_handle;
  BOOLEAN          err=FALSE;

#ifdef ALSPRX_DEBUG_BASIC
  SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                DBG_DD_ALSPRX_GET_DATA_REQ,
                                num_sensors);
#endif

  if ( (dd_handle == NULL) || (sensors == NULL) ||
       (num_sensors < 1) || (num_sensors > 2) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if (EnableI2C == FALSE)
  {
    static uint8_t toggle[2] = {0};
    sns_ddf_sensor_data_s *data_ptr;
    sns_ddf_sensor_sample_s *sample_ptr;
    int8 i;

    *data = sns_ddf_memhandler_malloc(
        memhandler, num_sensors * sizeof(sns_ddf_sensor_data_s));

    if(*data == NULL)
        return SNS_DDF_ENOMEM;

    for (i = 0; i < num_sensors; i++)
    {
      data_ptr = &((*data)[i]);

      if( (sample_ptr = sns_ddf_memhandler_malloc(memhandler,
                          (2*sizeof(sns_ddf_sensor_sample_s)))) == NULL)
      {
        return SNS_DDF_ENOMEM;
      }

      sns_ddf_delay(50);

      if (sensors[i] == SNS_DDF_SENSOR_PROXIMITY)
      {
        data_ptr->sensor = SNS_DDF_SENSOR_PROXIMITY;
        if (toggle[i])
        {
          sample_ptr[0].sample = FX_CONV_Q16(0, 0); //far = 0; near = 1
          sample_ptr[0].status = SNS_DDF_SUCCESS;
          sample_ptr[1].sample = 100; //ADC
          sample_ptr[1].status = SNS_DDF_SUCCESS;
        }
        else
        {
          sample_ptr[0].sample = FX_CONV_Q16(1, 0); //far = 0; near = 1
          sample_ptr[0].status = SNS_DDF_SUCCESS;
          sample_ptr[1].sample = 5; //ADC
          sample_ptr[1].status = SNS_DDF_SUCCESS;
        }
      }
      else
      {
        data_ptr->sensor = SNS_DDF_SENSOR_AMBIENT;
        if (toggle[i])
        {
          sample_ptr[0].sample = (int32_t)(20<<16); //20 lux
          sample_ptr[0].status = SNS_DDF_SUCCESS;
          sample_ptr[1].sample = 60; //ADC
          sample_ptr[1].status = SNS_DDF_SUCCESS;
        }
        else
        {
          sample_ptr[0].sample = (int32_t)(50<<16); //50 lux
          sample_ptr[0].status = SNS_DDF_SUCCESS;
          sample_ptr[1].sample = 150; //ADC
          sample_ptr[1].status = SNS_DDF_SUCCESS;
        }
      }
      data_ptr->status = SNS_DDF_SUCCESS;
      data_ptr->num_samples = 2;
      data_ptr->timestamp = sns_ddf_get_timestamp();
      data_ptr->samples = sample_ptr;
      toggle[i] = ~toggle[i];
    }

    return SNS_DDF_SUCCESS;
  }

  /* make sure device is in the right state */
  if ( dd_ptr->sns_dd_alsprx_common_db.state != SNS_DD_DEV_CONFIGURED )
  {
    /* device state is stopped OR invalid */
    SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                 DBG_DD_ALSPRX_GET_DATA_STATE_ERR,
                                 dd_ptr->sns_dd_alsprx_common_db.state);
    return SNS_DDF_EFAIL;
  }

  /* Look at all the requests */
  for ( i = 0; i < num_sensors; i++ )
  {
#ifdef ALSPRX_DEBUG_BASIC
    SNS_PRINTF_STRING_ID_MEDIUM_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                  DBG_DD_ALSPRX_GET_DATA_REQ,
                                  sensors[i]);
#endif

    switch ( sensors[i] )
    {
      case SNS_DDF_SENSOR_AMBIENT:
        err = sns_dd_alsprx_als_prepare_data_from_device(dd_ptr);
        break;
      case SNS_DDF_SENSOR_PROXIMITY:
        err = sns_dd_alsprx_prx_prepare_data_from_device(dd_ptr);
        break;
      default:
        SNS_PRINTF_STRING_ID_ERROR_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                     DBG_DD_ALSPRX_GET_DATA_INVALID_SENSOR_ERR,
                                     sensors[i]);
        err = SNS_DDF_EINVALID_PARAM;
    }

    if ( err == TRUE )
    {
      SNS_PRINTF_STRING_ID_ERROR_0(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                   DBG_DD_ALSPRX_GET_DATA_REQUEST_ERR);
      status = SNS_DDF_EFAIL;
      break;
    }
  }

  return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_alsprx_ltr55x_probe

===========================================================================*/
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
static sns_ddf_status_e sns_dd_alsprx_ltr55x_probe
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
  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }

 /* Read & Verify manufacture ID */
  status = sns_ddf_read_port( port_handle,
                              LTR55X_MANUFACTURER_ID,
                              &i2c_buff,
                              1,
                              &bytes_read );

  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }
  if( i2c_buff != SNS_DD_MANUFAC_ID )
  {
    /* Incorrect value. Return now with nothing detected */
    sns_ddf_close_port( port_handle );
    return SNS_DDF_SUCCESS;
  }

  /* Read & Verify Device ID */
  status = sns_ddf_read_port( port_handle,
                              LTR55X_PART_ID,
                              &i2c_buff,
                              1,
                              &bytes_read );

  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }
  if( i2c_buff != SNS_DD_LTR558_ID /* ltr558 */ &&
      i2c_buff != SNS_DD_LTR559_ID /* ltr559 */ )
  {
    /* Incorrect value. Return now with nothing detected */
    sns_ddf_close_port( port_handle );
    return SNS_DDF_SUCCESS;
  }

  /* Registers registers are correct. This is probably an ltr558 or ltr558 */
  *num_sensors = 2;
  *sensors = sns_ddf_memhandler_malloc( memhandler,
                                        sizeof(sns_ddf_sensor_e) * *num_sensors );
  if( *sensors != NULL )
  {
    (*sensors)[0] = SNS_DDF_SENSOR_PROXIMITY;
    (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
    status = SNS_DDF_SUCCESS;
  } else {
    status = SNS_DDF_ENOMEM;
  }
    sns_ddf_close_port( port_handle );
  return status;
}

