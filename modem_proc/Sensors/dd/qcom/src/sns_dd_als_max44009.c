/********************************************************************************
* Copyright (c) 2013, Maxim Integrated Products, Inc.
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of Maxim Integrated nor the
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
*******************************************************************************/
/* compile flags */

/* Flag to use Qualcomm's Q16 format for reporting lux */
#define USE_QUALCOMM_RANGE_FOR_LUX 1

/* Flag to set if compile is for DDK test board */
/* DO NOT set this for real devices */
#define SNS_DD_COMPILE_FOR_DDK_TEST_BOARD 0

/* Flag to include calibration mode support.*/
#define SNS_DD_MAX44009_CALIBRATION_SUPPORT 0

/* Flag to turn debugging on - use only on DDK test board, not real device */
#define DEBUG_MAX44009  0

#if DEBUG_MAX44009
 #define DBGPRINT(...) printf(__VA_ARGS__)
#else
 #define DBGPRINT(...)
#endif


#include <stdio.h>
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_ddf_signal.h"
#include "sns_memmgr.h"
#endif

/* Register definitions */
#define SNS_DD_MAX44009_I2C0     	0x94
#define SNS_DD_MAX44009_I2C1     	0x96
#define SNS_DD_MAX44009_INT_STATUS  	0x00
#define SNS_DD_MAX44009_INT_ENABLE 	0x01
#define SNS_DD_MAX44009_CONFIG 		0x02
#define SNS_DD_MAX44009_LUX_HIGH	0x03
#define SNS_DD_MAX44009_LUX_LOW		0x04
#define SNS_DD_MAX44009_THRESH_HIGH	0x05
#define SNS_DD_MAX44009_THRESH_LOW	0x06
#define SNS_DD_MAX44009_THRESH_TIM	0x07
#define SNS_DD_MAX44009_CLK_TRIM_COARSE	0x09
#define SNS_DD_MAX44009_CLK_TRIM_FINE 	0x0A
#define SNS_DD_MAX44009_GREEN_TRIM	0x0B
#define SNS_DD_MAX44009_IR_TRIM		0x0C
#define SNS_DD_MAX44009_OTP_REG		0x0D
/* OTP select bit set to 1*/
#define SNS_DD_MAX44009_OTP_SEL         0x01

/* Part specific constants */
#define SNS_DD_NUM_LUX_REG_BYTES      2
/* (reg reading) * SNS_DD_MAX44009_MLUX_LSB = measurement in millilux*/
#define SNS_DD_MAX44009_MLUX_LSB      45
/* max lux reading, corresponds to exponent 1110, mantissa 1111 1111 */
/* value = 2^14 * 0xFF */
#define SNS_DD_MAX44009_MAX_READING   0x3FC000
/* max reading */
#define SNS_DD_MAX44009_MAX_MLUX (SNS_DD_MAX44009_MAX_READING * SNS_DD_MAX44009_MLUX_LSB)
#define SNS_DD_MAX44009_EXPONENT_OVER_RANGE             0x0F
#define SNS_DD_MAX44009_MASK_MANTISSA_LSB               0x0E
#define SNS_DD_MAX44009_MANTISSA_BYTE2_HALF_RANGE       0x07
/* Threshold timer, in 100 ms increments */
#define SNS_DD_MAX44009_THRESHOLD_TIMER         0x01
#define SNS_DD_MAX44009_INIT_THRESHOLD_TIMER    0x01
#define SNS_DD_MAX44009_MAX_THRESHOLD_TIMER     0xFF


/* Qualcomm's Q16 format ranges for reporting lux */
#define QUALCOMM_MAX_LUX 32767
#define QUALCOMM_MAX_MLUX (QUALCOMM_MAX_LUX * 1000)
/* Interrupt threshold exponent and mantissa for QUALCOMM_MAX_LUX
 * Correspond to SNS_DD_MAX44009_MLUX_LSB of 45 
 * Need to recompute these after calibration. 
 * (2^12)*178*0.045 = 32808.96, nearest value over 32767 (QUALCOMM_MAX_LUX)
 */
#define QUALCOMM_MAX_LUX_EXPONENT 12 
#define QUALCOMM_MAX_LUX_MANTISSA 178

/* Basic definitions */
sns_ddf_driver_if_s sns_als_max44009_driver_if;

/* data structure for NV items */
#define MAGIC_NUM  299792458 //0x11DE784A
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
typedef PACK(struct)
#else
typedef struct
#endif
{
  uint32_t      magic_num; /* magic number to check if below parameters are valid */
  uint8_t       version_num;                    /* Version of NV structure */
  uint8_t       green_scale_pct;                 /* green trim scale percentage */
  uint8_t       ir_scale_pct;                    /* IR trim scale percentage */
  uint8_t       calibration_requested;          /* 0 = no, 1 = yes */
  uint8_t       green_gain_for_calibration;     /* normally 0xFF */
  uint8_t       ir_gain_for_calibration;        /* normally 0x00 or 0x7F */
  uint8_t       reserved[64];                   /* for future use*/
} sns_dd_als_nv_t;

/* data structure for the driver instance, allocated on init */
typedef struct {
  sns_ddf_handle_t      smgr_handle;  /* notify_data handle */
  sns_ddf_handle_t      port_handle;  /* I2C bus handle */
  sns_ddf_powerstate_e  power_mode;   /* ACTIVE/LOW_POWER */
  sns_ddf_timer_s       timer_obj;    /* timer object */
  bool                  in_calibration_mode;    /* calibration mode*/ 
  sns_dd_als_nv_t       nvparams;
  uint16_t              nv_size;
  uint32_t              irq;
  uint32_t              odr;   
} sns_dd_als_state_t;

static sns_ddf_sensor_e sns_dd_als_sensor_types[2] =
{
  SNS_DDF_SENSOR__NONE,
  SNS_DDF_SENSOR_AMBIENT
};

/*===========================================================================
===========================================================================*/
/* function prototypes */
/*===========================================================================
===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44009_set_power_mode
(
  sns_dd_als_state_t*    state,
  sns_ddf_powerstate_e   op_mode
);

static sns_ddf_status_e sns_dd_als_max44009_reset
(
  sns_ddf_handle_t dd_handle
);

static sns_ddf_status_e sns_dd_als_max44009_init
(
  sns_ddf_handle_t*        dd_ptr,
  sns_ddf_handle_t         smgr_handle,
  sns_ddf_nv_params_s*     reg_params,
  sns_ddf_device_access_s  device_info[],
  uint32_t                 num_devices,
  sns_ddf_memhandler_s*    memhandler,
  sns_ddf_sensor_e*        sensors[],
  uint32_t*                num_sensors
);

static sns_ddf_status_e sns_dd_als_max44009_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s** data
);

static sns_ddf_status_e sns_dd_als_max44009_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
);

static sns_ddf_status_e sns_dd_als_max44009_get_attrib
(
  sns_ddf_handle_t      handle,
  sns_ddf_sensor_e      sensor,
  sns_ddf_attribute_e   attrib,
  sns_ddf_memhandler_s* memhandler,
  void**                value,
  uint32_t*             num_elems
);

static void sns_dd_als_max44009_handle_timer 
(
  sns_ddf_handle_t handle, 
  void* arg
);

#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
static void sns_dd_als_max44009_handle_irq
(
  sns_ddf_handle_t dd_handle, 
  uint32_t         irq,
  sns_ddf_time_t   timestamp
);
#else
/* NOTE: irqs not support on DDK test board. Included for compile only*/
static void sns_dd_als_max44009_handle_irq
(
  sns_ddf_handle_t dd_handle, 
  sns_ddf_irq_e irq
);
#endif


static sns_ddf_status_e sns_dd_als_max44009_init_normal
(
  sns_dd_als_state_t* state
);

static sns_ddf_status_e sns_dd_als_max44009_init_calibration
(
  sns_dd_als_state_t* state 
);

static sns_ddf_status_e sns_dd_als_max44009_init_lowpower
(
  sns_dd_als_state_t* state
);

static sns_ddf_status_e sns_dd_als_max44009_read
(
  sns_dd_als_state_t* state,
  uint8_t * reg_data
);

static sns_ddf_status_e sns_dd_als_max44009_prepare_data
(
  sns_dd_als_state_t* state,
  const uint8_t * regdata,
  const sns_ddf_sensor_e sensor,
  sns_ddf_sensor_data_s*    s_data,
  sns_ddf_sensor_sample_s*  samples,
  bool from_irq
);

static sns_ddf_status_e sns_als_max44009_driver_update_int_thresh
(
  sns_dd_als_state_t* state,
  const uint8_t exponent,
  const uint8_t mantissa,
  const q16_t lux
);

static sns_ddf_status_e read_settings_reg_factory_defaults
(
  sns_dd_als_state_t* state,
  uint8_t* clk_trim_coarse,
  uint8_t* clk_trim_fine,
  uint8_t* green_trim,
  uint8_t* ir_trim
);

static sns_ddf_status_e write_settings_reg
(
  sns_dd_als_state_t* state,
  uint8_t clk_trim_coarse,
  uint8_t clk_trim_fine,
  uint8_t green_trim,
  uint8_t ir_trim
);
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
static sns_ddf_status_e sns_dd_als_max44009_enable_sched_data(
    sns_ddf_handle_t  handle,
    sns_ddf_sensor_e  sensor,
    bool              enable);

static sns_ddf_status_e sns_dd_als_max44009_enable_interrupt
(
  sns_dd_als_state_t* state
);

static sns_ddf_status_e sns_dd_als_max44009_disable_interrupt
(
  sns_dd_als_state_t* state
);

static void sns_dd_als_max44009_log_nvparams
(
  sns_dd_als_state_t* state
);
#endif

static sns_ddf_status_e sns_dd_als_max44009_test(
        sns_ddf_handle_t  dd_handle,
        sns_ddf_sensor_e  sensor,
        sns_ddf_test_e    test,
        uint32_t*         err);

/*===========================================================================*/
/* inline functions */
/*===========================================================================*/
/*
 * @brief sns_dd_als_max44009_write_one_byte: Write one byte and 
          confirm it by a read.
 * 
 * @param[in] handle    I2C bus handle given to this driver at init time. 
 * @param[in] reg_addr  Address to write to
 * @param[in] buffer    Pointer to buffer containing one byte of write data
 * 
 * @return SNS_DDF_SUCCESS on succcessful write and confirmed by a 
           subsequent read, error code otherwise.
 */
/*===========================================================================*/
static inline sns_ddf_status_e sns_dd_als_max44009_write_one_byte(
    sns_ddf_handle_t  handle,
    uint8_t           reg_addr,
    uint8_t*          buffer)
{
    uint8_t nbytes, confirm;
    sns_ddf_status_e ret;
    ret = sns_ddf_write_port(handle, reg_addr, buffer, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
      return SNS_DDF_EFAIL;
    
    ret = sns_ddf_read_port(handle, reg_addr, &confirm, 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
      return SNS_DDF_EFAIL;
    
    switch (reg_addr)
    {
    case SNS_DD_MAX44009_CLK_TRIM_COARSE:
    case SNS_DD_MAX44009_CLK_TRIM_FINE:
    case SNS_DD_MAX44009_GREEN_TRIM:
    case SNS_DD_MAX44009_IR_TRIM:
      confirm = ~confirm;
      break;
    
    default: /*do nothing*/
      break;    
    }
    if (*buffer != confirm) 
      return SNS_DDF_EFAIL;
    return SNS_DDF_SUCCESS;
}

/*===========================================================================*/
/* function definitions */
/*===========================================================================*/
/*
 * @brief sns_dd_als_max44009_set_power_mode: sets the chip to normal and 
 *        low power states.
 * 
 * @param[in] state    Handle to driver instance.
 * @param[in] op_mode  Power mode desired
 * 
 * @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44009_set_power_mode
(
  sns_dd_als_state_t*    state,
  sns_ddf_powerstate_e   op_mode
)
{
  sns_ddf_status_e ret = SNS_DDF_SUCCESS;
  DBGPRINT("%s, line %d\r\n",__func__,__LINE__);
  
  if ((op_mode == SNS_DDF_POWERSTATE_ACTIVE) && 
      (state->power_mode != SNS_DDF_POWERSTATE_ACTIVE)) {
        /* normal init or calibrate mode init*/
        if (!state->in_calibration_mode)
          ret = sns_dd_als_max44009_init_normal(state);
        else
          ret = sns_dd_als_max44009_init_calibration(state);
  }
  else if ((op_mode == SNS_DDF_POWERSTATE_LOWPOWER) && 
           (state->power_mode != SNS_DDF_POWERSTATE_LOWPOWER)) {
    ret = sns_dd_als_max44009_init_lowpower(state);
  }
  return ret;  
}

/*===========================================================================*/
/*
 * @brief sns_dd_als_max44009_reset: sets the chip to low power state. 
 *        Please note that MAX44009 power consumption even in normal state 
 *        is comparable to shutdown power of most parts.
 *
 * @param[in] dd_handle Handle to driver instance.
 * 
 * @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44009_reset
(
  sns_ddf_handle_t dd_handle
)
{
  sns_dd_als_state_t* state = (sns_dd_als_state_t *)dd_handle;

  /* basic sanity check */
  if (dd_handle == NULL) 
    return SNS_DDF_EINVALID_PARAM;

  /* reset into low-power mode */
  return sns_dd_als_max44009_set_power_mode(state, SNS_DDF_POWERSTATE_LOWPOWER);
}

/*===========================================================================*/
/*
 * @brief sns_dd_als_max44009_init Initializes the MAX44009 device driver
 *
 * @detail Allocates handle for this driver instance.
 *         Opens I2C communication port. 
 *         Places device in default power state.
 *         This function will be called at init time.
 *   
 *  @param[out] dd_handle_ptr  Pointer that this function must malloc and 
 *                             populate. This is a handle to the driver
 *                             instance that will be passed in to all other
 *                             functions. NB: Do not use @a memhandler to
 *                             allocate this memory.
 *  @param[in]  smgr_handle    Handle used to identify this driver when it 
 *                             calls into Sensors Manager functions.
 *  @param[in]  nv_params      NV parameters retrieved for the driver.
 *  @param[in]  device_info    Access info for physical devices controlled by 
 *                             this driver. Used to configure the bus
 *                             and talk to the devices.
 *  @param[in]  num_devices    Number of elements in @a device_info. 
 *  @param[in]  memhandler     Memory handler used to dynamically allocate 
 *                             output parameters, if applicable. NB: Do not
 *                             use memhandler to allocate memory for
 *                             @a dd_handle_ptr.
 *  @param[out] sensors        List of supported sensors, allocated, 
 *                             populated, and returned by this function.
 *  @param[out] num_sensors    Number of elements in @a sensors.
 * 
 *  @return Success if @a dd_handle_ptr was allocated and the driver was 
 *          configured properly. Otherwise a specific error code is returned.
 */
/*===========================================================================*/

static sns_ddf_status_e sns_dd_als_max44009_init
(
  sns_ddf_handle_t*        dd_ptr,
  sns_ddf_handle_t         smgr_handle,
  sns_ddf_nv_params_s*     reg_params,
  sns_ddf_device_access_s  device_info[],
  uint32_t                 num_devices,
  sns_ddf_memhandler_s*    memhandler,
  sns_ddf_sensor_e*        sensors[],
  uint32_t*                num_sensors
)
{
  sns_dd_als_state_t*  state;
  sns_ddf_status_e     ret;
  bool data_from_registry = false;

  /* basic sanity check */
  if (dd_ptr == NULL || sensors == NULL || num_sensors == NULL)
    return SNS_DDF_EINVALID_PARAM;

  /* allocate state memory */
  if (sns_ddf_malloc((void **)&state, sizeof(*state)) != SNS_DDF_SUCCESS)
    return SNS_DDF_ENOMEM;
  state->smgr_handle = smgr_handle;

  /* open I2C port */
  ret = sns_ddf_open_port(&state->port_handle, &device_info->port_config);
  if (ret != SNS_DDF_SUCCESS) 
    goto error;

  /* default for calibration = not on */ 
  state->in_calibration_mode = false;
  /* ODR is between 1 and 2hz (800 ms period). setting it to 2 */
  state->odr = 2;
  state->nv_size = 0;
  
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD) 
  /* in real device, replace defaults with nv items, if they are available*/
  if (reg_params) {
    if ( (reg_params->status == SNS_DDF_SUCCESS) &&
         (reg_params->data_len >= sizeof(sns_dd_als_nv_t)) ) {
      sns_dd_als_nv_t* nv_ptr = (sns_dd_als_nv_t *)reg_params->data;
      if ( (nv_ptr->magic_num == MAGIC_NUM) &&
           (reg_params->data_len >= sizeof(sns_dd_als_nv_t)) ) {
        /* update from nv items */
        data_from_registry = true;
        state->nv_size = reg_params->data_len;
        SNS_OS_MEMCOPY(&state->nvparams, nv_ptr, sizeof(sns_dd_als_nv_t));
      }
    }
    if (!data_from_registry) {
      /* set defaults for the nv params */
      state->nv_size = 128;
      state->nvparams.magic_num = MAGIC_NUM;
      state->nvparams.version_num = 0;
      state->nvparams.green_scale_pct = 100;
      state->nvparams.ir_scale_pct = 100;
      state->nvparams.calibration_requested = 0;
      state->nvparams.green_gain_for_calibration = 0;
      state->nvparams.ir_gain_for_calibration = 0;
      SNS_OS_MEMZERO(state->nvparams.reserved, sizeof(state->nvparams.reserved));
    }
  }
        
  /* set calibration and interrupt flags */
#if SNS_DD_MAX44009_CALIBRATION_SUPPORT
  if (state->nvparams.calibration_requested) {
    SNS_PRINTF_STRING_ID_FATAL_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,4414);
    state->in_calibration_mode = true;
  }
#endif /*SNS_DD_MAX44009_CALIBRATION_SUPPORT*/
  state->irq = device_info->first_gpio;  
#endif /* (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD) */

  /* low power init or calibrate mode init*/
  if (!state->in_calibration_mode) {
    ret = sns_dd_als_max44009_init_lowpower(state);
  } else {
    ret = sns_dd_als_max44009_init_calibration(state);
  }
        
  if (ret != SNS_DDF_SUCCESS) 
    goto error;
  
  /* initialize timer */
  ret = sns_ddf_timer_init(&state->timer_obj, state,
                           &sns_als_max44009_driver_if, NULL, true);
  if (ret != SNS_DDF_SUCCESS) 
    goto error;
 
  /* all done */
  *dd_ptr = (sns_ddf_handle_t)state;
  *sensors = sns_dd_als_sensor_types;
  *num_sensors = 2;

  /* request for registry update when needed;
     this needs to happen after dd_ptr is populated properly */
  if ( (reg_params) && (!data_from_registry) ) {
    sns_ddf_smgr_notify_event(smgr_handle,
                              SNS_DDF_SENSOR_AMBIENT,
                              SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
  }

#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)  
  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,4400);
#endif
  return SNS_DDF_SUCCESS;
error:
  DBGPRINT("Error, function %s line %d*****\r\n", __func__, __LINE__);
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)  
  SNS_PRINTF_STRING_ID_FATAL_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING2,5500,ret);
#endif  
  return ret;
}

/*===========================================================================*/
/*
 * @brief sns_dd_als_max44009_get_data: Requests a single sample of sensor data. 
 *       Data is returned immediately after being read from the sensor.
 *
 * @param[in]  dd_handle    Handle to driver instance.
 * @param[in]  sensors      List of sensors for which data is requested.
 * @param[in]  num_sensors  Length of @a sensors.
 * @param[in]  memhandler   Memory handler used to dynamically allocate 
 *                          output parameters, if applicable.
 * @param[out] data         Sampled sensor data.
 
 * @return SNS_DDF_SUCCESS if data was populated successfully, error code 
 *         otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44009_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s** data
)
{
  sns_dd_als_state_t        *state = (sns_dd_als_state_t *)dd_handle;
  sns_ddf_status_e          ret;
  uint8_t                   reg_data[2];
  sns_ddf_sensor_data_s*    s_data;
  sns_ddf_sensor_sample_s*  samples;
  
  /* basic sanity check */
  if (dd_handle == NULL || sensors == NULL || num_sensors != 1 ||
      memhandler == NULL || data == NULL) return SNS_DDF_EINVALID_PARAM;

  /* allocate sensor data structure */
  s_data = sns_ddf_memhandler_malloc(memhandler, sizeof(*s_data));
  if (s_data == NULL) 
    return SNS_DDF_ENOMEM;
  *data = s_data;
  
  /* allocate samples buffer */
  samples = sns_ddf_memhandler_malloc(memhandler, 2 * sizeof(*samples));
  
  if (samples == NULL) 
    return SNS_DDF_ENOMEM;

  /* read samples from device */
  ret = sns_dd_als_max44009_read(state, reg_data);
  
  if (ret != SNS_DDF_SUCCESS) 
    return ret;
  
  DBGPRINT("regdata[0] %d\t, regdata[1] %d\r\n", reg_data[0], reg_data[1]);
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)  
  SNS_PRINTF_STRING_ID_LOW_3(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING3,
                             4415,reg_data[0],reg_data[1] );
#endif  
  ret = sns_dd_als_max44009_prepare_data(state, reg_data,
                                         sensors[0], s_data, samples, false );
  if (ret != SNS_DDF_SUCCESS) 
    return ret;
  return SNS_DDF_SUCCESS;
}

/*===========================================================================*/
/*
 * @brief sns_dd_als_max44009_set_attrib: Sets certain device attributes that 
         are programmable. Currently only power mode is a settable attribute.
 *
 * @param[in] dd_handle  Handle to driver instance.
 * @param[in] sensor     Sensor for which this attribute is to be set.
 * @param[in] attrib     Attribute to be set.
 * @param[in] value      Value to set this attribute.
 *
 *  @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44009_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
  /* basic sanity check */
  if (dd_handle == NULL || value == NULL) return SNS_DDF_EINVALID_PARAM;

  /* set attribute */
  switch (attrib)
  {
    case SNS_DDF_ATTRIB_POWER_STATE:
      return sns_dd_als_max44009_set_power_mode((sns_dd_als_state_t *)dd_handle,
                                       *(sns_ddf_powerstate_e *)value);
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)    
   
case SNS_DDF_ATTRIB_ODR:
      /*Noop, ODR is fixed at 2*/
      return SNS_DDF_SUCCESS;
    case SNS_DDF_ATTRIB_RANGE:
      /*Noop, just return success*/
      return SNS_DDF_SUCCESS;
#endif      
    default:
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)  
    SNS_PRINTF_STRING_ID_FATAL_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_STRING2,5506,attrib);
#endif
      return SNS_DDF_EINVALID_PARAM;
  }
}

/*===========================================================================*/
/*
 * @brief sns_dd_als_max44009_get_attrib: Retrieves the value of an attribute.
 *
 * @param[in] dd_handle   Handle to driver instance.
 * @param[in] sensor      Sensor for which this attribute is to be retrieved.
 * @param[in] attrib      Attribute to be retrieved.
 * @param[in] memhandler  Memory handler used to dynamically allocate 
                          output parameters, if applicable.
 * @param[out] value      Pointer that this function will allocate or set 
 *                        to the attribute's value.
 * @param[out] num_elems   Number of elements in @a value.
 *
 * @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44009_get_attrib
(
  sns_ddf_handle_t      handle,
  sns_ddf_sensor_e      sensor,
  sns_ddf_attribute_e   attrib,
  sns_ddf_memhandler_s* memhandler,
  void**                value,
  uint32_t*             num_elems
)
{
  sns_dd_als_state_t        *state = (sns_dd_als_state_t *)handle;
  sns_ddf_driver_info_s     *driver_info;
  sns_ddf_device_info_s     *device_info;
  sns_ddf_range_s           *device_ranges;
  sns_ddf_resolution_t      *resolution;
  sns_ddf_resolution_adc_s  *resolution_adc;
  sns_ddf_power_info_s      *power;
  sns_ddf_powerstate_e      *power_state;
  sns_ddf_registry_group_s  *reg_group;
  uint8_t                   *reg_group_data;
  uint32_t                  *odr;

  /* basic sanity check */
  if (handle == NULL || memhandler == NULL || value == NULL ||
      num_elems == NULL) return SNS_DDF_EINVALID_PARAM;

  /* return attribute */
  switch (attrib)
  {
    case SNS_DDF_ATTRIB_DRIVER_INFO:
      driver_info = sns_ddf_memhandler_malloc(memhandler,
                                              sizeof(*driver_info));
      if (driver_info == NULL) return SNS_DDF_ENOMEM;
      driver_info->name = "MAX44009";
      driver_info->version = 1;
      *value = driver_info;
      *num_elems = 1;
      return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_DEVICE_INFO:
    
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
      /*logging nv params here for utility purposes*/    
      sns_dd_als_max44009_log_nvparams(state);
#endif
      device_info = sns_ddf_memhandler_malloc(memhandler,
                                              sizeof(*device_info));
      if (device_info == NULL) return SNS_DDF_ENOMEM;
      device_info->name = "MAX44009";
      device_info->vendor = "Maxim";
      device_info->model = "MAX44009";
      device_info->version = 1;
      *value = device_info;
      *num_elems = 1;
      return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_RANGE:
      device_ranges = sns_ddf_memhandler_malloc(memhandler,
                                                sizeof(*device_ranges));
      if (device_ranges == NULL) return SNS_DDF_ENOMEM;
      device_ranges->min = 0;
#if USE_QUALCOMM_RANGE_FOR_LUX
      device_ranges->max = FX_FLTTOFIX_Q16(QUALCOMM_MAX_LUX);
      
#else      
      device_ranges->max = SNS_DD_MAX44009_MAX_MLUX; // 188006400 milli lux
#endif      
      *value = device_ranges;
      *num_elems = 1;       
      return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_RESOLUTION:
      resolution = sns_ddf_memhandler_malloc(memhandler, sizeof(*resolution));
      if (resolution == NULL) return SNS_DDF_ENOMEM;
#if USE_QUALCOMM_RANGE_FOR_LUX
      *resolution = FX_FLTTOFIX_Q16(1);
#else      
      *resolution = SNS_DD_MAX44009_MLUX_LSB; /* resolution in milli lux*/
#endif      
      *value = resolution;
      *num_elems = 1;
      return SNS_DDF_SUCCESS;
 
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
      resolution_adc = sns_ddf_memhandler_malloc(memhandler,
                                                 sizeof(*resolution_adc));
      if (resolution_adc == NULL) return SNS_DDF_ENOMEM;
      resolution_adc->bit_len = 16;
      resolution_adc->max_freq = 2;
	  *value = resolution_adc;
      *num_elems = 1;
      return SNS_DDF_SUCCESS;
      
    case SNS_DDF_ATTRIB_POWER_INFO:
      power = sns_ddf_memhandler_malloc(memhandler, sizeof(*power));
      if (power == NULL) return SNS_DDF_ENOMEM;
      power->active_current = 1;
      power->lowpower_current = 1;
      *(sns_ddf_power_info_s **)value = power;
      *num_elems = 1;
      return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_POWER_STATE:
      power_state = sns_ddf_memhandler_malloc(memhandler,
                                              sizeof(*power_state));
      if (power_state == NULL) return SNS_DDF_ENOMEM;
      *power_state = state->power_mode;
      *value = power_state;
      *num_elems = 1;
      return SNS_DDF_SUCCESS;

#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)    
    case SNS_DDF_ATTRIB_ODR:
      odr = sns_ddf_memhandler_malloc(memhandler,
                                              sizeof(*odr));
      if (odr == NULL) return SNS_DDF_ENOMEM;
      *odr = state->odr;
      *value = odr;
      return SNS_DDF_SUCCESS;

    case SNS_DDF_ATTRIB_REGISTRY_GROUP:
      reg_group = sns_ddf_memhandler_malloc(memhandler,
                                            sizeof(*reg_group));
      if (reg_group == NULL) return SNS_DDF_ENOMEM;

      if (state->nv_size == 0) {
        reg_group->group_data = NULL;
        reg_group->size = 0;
      } else {
        reg_group_data = sns_ddf_memhandler_malloc(memhandler, state->nv_size);
        if (reg_group_data == NULL) return SNS_DDF_ENOMEM;

        SNS_OS_MEMCOPY(reg_group_data, &state->nvparams, sizeof(sns_dd_als_nv_t));
        reg_group->group_data = reg_group_data;
        reg_group->size = state->nv_size;
      }
      *value = reg_group;
      *num_elems = 1;
      return SNS_DDF_SUCCESS;
#endif /* (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD) */

    default:
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)  
      SNS_PRINTF_STRING_ID_FATAL_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_STRING2,5505,attrib);
#endif       
      return SNS_DDF_EINVALID_ATTR;
  }
}

/*===========================================================================*/
/*
 * @brief sns_dd_als_max44009_handle_timer: Called when the timer set by this 
 *        driver has expired. This is the callback function submitted when 
 *        initializing a timer.
 * 
 * @detail This will be called within the context of the Sensors Manager task.
 *         It indicates that sensor data is ready.
 *
 * @param[in] dd_handle  Handle to driver instance. 
 * @param[in] arg        The argument submitted when the timer was set. 
 * 
 * @return None 
 */
/*===========================================================================*/
static void sns_dd_als_max44009_handle_timer 
(
  sns_ddf_handle_t handle, 
  void* arg
)
{
  /* nothing to do */
}

/*===========================================================================*/
/*
 * @brief sns_dd_als_max44009_handle_irq: interrupt handler
 * 
 * @detail This function will be called within the context of the Interrupt 
 *         Service Task (IST), *not* the ISR.
 *
 * @param[in] dd_handle  Handle to driver instance. 
 * @param[in] irq        The IRQ representing the interrupt that occured.  
 * 
 * @return None 
 */
/*===========================================================================*/
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
static void sns_dd_als_max44009_handle_irq
(
  sns_ddf_handle_t dd_handle, 
  uint32_t         irq,
  sns_ddf_time_t   timestamp
)
#else
/* NOTE: irqs not support on DDK test board. Included for compile only*/
static void sns_dd_als_max44009_handle_irq
(
  sns_ddf_handle_t dd_handle, 
  sns_ddf_irq_e irq
)
#endif
{

  static sns_ddf_sensor_sample_s        *samples;
  static sns_ddf_sensor_data_s          *s_data;
  uint8_t                               reg_data[2];
  uint8_t intstatus_reg;
  uint8_t regval, nbytes;
  sns_ddf_status_e ret;

  sns_dd_als_state_t *state = (sns_dd_als_state_t *)dd_handle;  
  /* basic sanity check */
  if (dd_handle == NULL) {
    ret = SNS_DDF_EINVALID_PARAM;
    SNS_PRINTF_STRING_ID_FATAL_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING2,5502, ret);
    return;
  }

#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
  /* TODO: verify irq number */
#endif  
  
  /* disable interrupt */
  regval = 0x00;
  ret = sns_dd_als_max44009_write_one_byte(state->port_handle, 
                                           SNS_DD_MAX44009_INT_ENABLE,
                                           &regval);
  if (ret != SNS_DDF_SUCCESS)
    goto error;  

  /* read the interrupt status register to clear the interrupt */
  ret = sns_ddf_read_port(state->port_handle,SNS_DD_MAX44009_INT_STATUS,
                               &intstatus_reg, 1, &nbytes);
  if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
    goto error;
  DBGPRINT("Interrupt status bit is %d*****\r\n",intstatus_reg);

  if (SNS_DDF_SUCCESS != sns_ddf_malloc((void **)&s_data, sizeof(*state)))
    goto error;
  if (SNS_DDF_SUCCESS != sns_ddf_malloc((void **)&samples, 2 * sizeof(*samples)))
    goto error;
      
  if (SNS_DDF_SUCCESS != sns_dd_als_max44009_read(state, reg_data))
    goto error;
  
  DBGPRINT("** regdata[0] %d\t, regdata[1] %d\r\n", reg_data[0], reg_data[1]);

  if (!s_data || !samples)
  {
  	goto error;
  }

  if (SNS_DDF_SUCCESS != sns_dd_als_max44009_prepare_data
                        (state, reg_data, SNS_DDF_SENSOR_AMBIENT,
                         s_data, samples, true)) {
    goto error;
  }
  sns_ddf_smgr_notify_data(state->smgr_handle, s_data, 1);
  /* enable interrupt */
  regval = 0x01;
  ret = sns_dd_als_max44009_write_one_byte(state->port_handle, 
                                           SNS_DD_MAX44009_INT_ENABLE,
                                           &regval);
  if (ret != SNS_DDF_SUCCESS) {
    DBGPRINT("Error, function %s line %d*****\r\n", __func__, __LINE__); 
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
    SNS_PRINTF_STRING_ID_FATAL_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING2,5501, ret);
#endif
  }
  if (samples)
    sns_ddf_mfree(samples);
  if (s_data)
    sns_ddf_mfree(s_data);
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
  SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,4401);
#endif
  return;
 
error:
  DBGPRINT("Error, function %s line %d*****\r\n", __func__, __LINE__); 
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
    SNS_PRINTF_STRING_ID_FATAL_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING2,5502, ret);
#endif  
  /* enable interrupt */
  regval = 0x01;
  sns_dd_als_max44009_write_one_byte(state->port_handle, 
                                           SNS_DD_MAX44009_INT_ENABLE,
                                           &regval);
  if (samples)
    sns_ddf_mfree(samples);
  if (s_data)
    sns_ddf_mfree(s_data);
  return;
    
}

/*==========================================================================*/
/*
 * @brief sns_dd_als_max44009_init_normal: sets the chip to normal state.
 * 
 * @param[in] state    Handle to driver instance.
 * 
 * @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44009_init_normal
(
  sns_dd_als_state_t* state
)
{
  /* read reg ox0D, set last bit to 0 
   * read reg Ox09 to Ox0C, bitwise complement
   * write 0x09 and 0x0A back
   * scale and write 0x0b and 0x0C back
   */
   
  uint8_t               clk_trim_coarse, clk_trim_fine;
  uint8_t               green_trim, ir_trim;
  sns_ddf_status_e      ret;

  /* read factory default reg values */
  ret = read_settings_reg_factory_defaults(state,
                                           &clk_trim_coarse,&clk_trim_fine,
                                           &green_trim,&ir_trim);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
  
  /* scale green and ir trim */
  green_trim = (green_trim * state->nvparams.green_scale_pct) / 100; 
  ir_trim = (ir_trim * state->nvparams.ir_scale_pct) / 100;
  
  /*write reg values back */
  ret = write_settings_reg(state,
                          clk_trim_coarse,clk_trim_fine,
                          green_trim,ir_trim);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
 
  state->power_mode = SNS_DDF_POWERSTATE_ACTIVE;
  DBGPRINT("********successful normal init*****\r\n");
  return SNS_DDF_SUCCESS;
  
error:
  DBGPRINT("Error, function %s line %d*****\r\n", __func__, __LINE__);
  return SNS_DDF_EFAIL;
}

/*===========================================================================*/
/*
 * @brief sns_dd_als_max44009_init_calibration: sets the chip to calibration 
 *         state.
 * 
 * @param[in] state    Handle to driver instance.
 * 
 * @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44009_init_calibration
(
  sns_dd_als_state_t* state 
)
{
  sns_ddf_status_e      ret;
  uint8_t               green_trim, ir_trim;
  uint8_t               clk_trim_coarse, clk_trim_fine;

  /* read factory default reg values */
  ret = read_settings_reg_factory_defaults(state,
                                           &clk_trim_coarse,&clk_trim_fine,
                                           &green_trim,&ir_trim);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
  
  /* output green and ir trim as log messages */ 
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)  
  SNS_PRINTF_STRING_ID_FATAL_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING2,4408,green_trim);
  SNS_PRINTF_STRING_ID_FATAL_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING2,4409,ir_trim);
#else
  DBGPRINT("Calibration,factory defaults, green trim %d ir trim %d \r\n", green_trim, ir_trim);
#endif  
  /* set green and ir trim to calibration values */
  green_trim = state->nvparams.green_gain_for_calibration;
  ir_trim = state->nvparams.ir_gain_for_calibration;
  
  /*write reg values back */
  ret = write_settings_reg(state,
                           clk_trim_coarse,clk_trim_fine,
                           green_trim,ir_trim);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
  
  state->power_mode = SNS_DDF_POWERSTATE_ACTIVE;
  DBGPRINT("********successful calibrate init*****\r\n");
  return SNS_DDF_SUCCESS;
  
error:
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,5508);
#endif    
  DBGPRINT("Error, function %s line %d*****\r\n", __func__, __LINE__);
  return SNS_DDF_EFAIL;
}
/*===========================================================================*/
/*
 * @brief sns_dd_als_max44009_init_lowpower: sets the chip to low power 
 *        state. Please note that MAX44009 power consumption even in normal state 
 *        is comparable to shutdown power of most parts.
 *
 * @param[in] state    Handle to driver instance.
 * 
 * @return SNS_DDF_SUCCESS
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44009_init_lowpower
(
  sns_dd_als_state_t* state
)
{
  state->power_mode = SNS_DDF_POWERSTATE_LOWPOWER;
  DBGPRINT("********successful low power mode*****\r\n");
  return SNS_DDF_SUCCESS;  
}
/*===========================================================================*/
/*
 * @brief  sns_dd_als_max44009_read: Tries to read two data bytes from the 
 *         sensor as two separate I2C reads. 
 *
 * @detail This function is a workaround for separate reads with a continued 
 *         start not being supported on Qualcomm chipsets.This function tries 
 *         to read 2 bytes of data. If mantissa differs only by the last bit 
 *         for two consecutive reads, then the function returns both the bytes 
 *         from the latest read. Otherwise, it returns the first byte from 
 *         latest read (4 exponent + 4 mantissa bits), sets the next 4 bits of
 *         mantissa to half its range value
 *         (SNS_DD_MAX44009_MANTISSA_BYTE2_HALF_RANGE), 
 *         and returns that as the second byte.
 *          
 * @param[in]  state     State memory allocated by driver at init time. 
 * @param[out] reg_data  Two bytes of data read  
 * 
 * @return SNS_DDF_SUCCESS if successful, error code otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44009_read
(
  sns_dd_als_state_t* state,
  uint8_t* reg_data
)
{
  sns_ddf_status_e          ret;
  uint8_t  nbytes;

  /* 2 byte read, repeated start I2C read function is now supported by Qualcomm*/  
  ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44009_LUX_HIGH,
                          reg_data, 2, &nbytes); 
  
  if ((ret != SNS_DDF_SUCCESS) || (nbytes != 2)) {
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)  
    SNS_PRINTF_STRING_ID_LOW_3(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                               DBG_DD_ALSPRX_STRING3,5507,ret,nbytes);
#endif
    return SNS_DDF_EFAIL;
  }
  
  return SNS_DDF_SUCCESS;
  /* older work around when 2 byte reads were not supported by Qualcomm */
  /*
  uint8_t prev_reg_data[2];
  bool bmatch = false;
  int i;
  
  ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44009_LUX_HIGH,
                    &prev_reg_data[0], 1, &nbytes);
  if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    goto error;
  
  ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44009_LUX_LOW,
                    &prev_reg_data[1], 1, &nbytes);
  if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
    goto error;
  
  for (i = 0; i < 2; ++i) {
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44009_LUX_HIGH,
                    &reg_data[0], 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
      goto error;
    
    ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44009_LUX_LOW,
                    &reg_data[1], 1, &nbytes);
    if ((ret != SNS_DDF_SUCCESS) || (nbytes != 1))
      goto error;   

    DBGPRINT("byte1 prev %d current%d\r\n",prev_reg_data[0], reg_data[0]);
    DBGPRINT("byte2 prev %d current%d\r\n",prev_reg_data[1], reg_data[1]);

    if ((prev_reg_data[0] == reg_data[0]) && 
        ((prev_reg_data[1] & SNS_DD_MAX44009_MASK_MANTISSA_LSB) == 
         (reg_data[1] & SNS_DD_MAX44009_MASK_MANTISSA_LSB))) {
      DBGPRINT("**Found matching reads**\r\n");
      bmatch = true;
      break;     
    } else {
      prev_reg_data[0] = reg_data[0];
      prev_reg_data[1] = reg_data[1];
    }
  }
  if (!bmatch) {
    DBGPRINT("**No matching reads**\r\n");
    reg_data[1] = SNS_DD_MAX44009_MANTISSA_BYTE2_HALF_RANGE; 
  }
  return SNS_DDF_SUCCESS;

error:
  DBGPRINT("Error, function %s line %d*****\r\n", __func__, __LINE__);
  return SNS_DDF_EFAIL;
  */
}

/*===========================================================================*/
/*
 * @brief sns_dd_als_max44009_prepare_data: Utility function used by callers 
 * prepare sensor data to return to caller. Also sets interrupt thresholds if 
 * interrupts are enabled.
 *
 * @param[in]  state    State memory allocated by driver at init time.
 * @param[in]  reg_data 2 bytes of data, lux hi and lux low register values
 * @param[in]  sensors  List of sensors for which data is requested.
 * @param[out] s_data   Sampled sensor data.
 * @param[out] samples  The data samples themselves.
 * @param[in]  from_irq Indicates if the call is from irq or get_data
 *
 * @return SNS_DDF_SUCCESS if data was prepared successfully, error code 
 *         otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44009_prepare_data
(
  sns_dd_als_state_t* state,
  const uint8_t* reg_data,
  const sns_ddf_sensor_e sensor,
  sns_ddf_sensor_data_s* s_data,
  sns_ddf_sensor_sample_s* samples,
  bool from_irq
)
{
  uint8_t       exponent, mantissa;  
  q16_t         luxvalue;
  q16_t         luxvalue_before_qcom_range;
  
  /* get exponent and mantissa, convert to milli lux */
  mantissa = (reg_data[0] << 4) & 0xF0; // 4 MSB of mantissa
  mantissa |= (reg_data[1] & 0x0F);     //4 LSB of mantissa 
  exponent = (reg_data[0] >> 4) & 0x0F; 
  
   /* check for over range condition*/
  if (exponent != SNS_DD_MAX44009_EXPONENT_OVER_RANGE) {
    luxvalue = (1 << exponent) * mantissa * 
                                SNS_DD_MAX44009_MLUX_LSB; 
    /* check for lux max */
    if (luxvalue > SNS_DD_MAX44009_MAX_MLUX)
      luxvalue = SNS_DD_MAX44009_MAX_MLUX;
    
  } else { 
    /* exponent over range, set lux to lux max*/
    luxvalue = SNS_DD_MAX44009_MAX_MLUX;
  }
  luxvalue_before_qcom_range = luxvalue;

#if USE_QUALCOMM_RANGE_FOR_LUX
  if (luxvalue > QUALCOMM_MAX_MLUX) {
    luxvalue = QUALCOMM_MAX_MLUX;
  }
  luxvalue = luxvalue/1000;
  samples[0].sample = FX_FLTTOFIX_Q16(luxvalue);
  DBGPRINT("milli lux %d, \t lux %d, Q16 %d\r\n",
           luxvalue * 1000, luxvalue, samples[0].sample);
#else
  samples[0].sample = luxvalue;
  DBGPRINT("milli lux %d, \t lux %d\r\n",luxvalue,
           luxvalue/1000);
#endif
  samples[0].status = SNS_DDF_SUCCESS;
  
  /* calculate raw counts, check for over range condition */
  if (exponent != SNS_DD_MAX44009_EXPONENT_OVER_RANGE) {
    samples[1].sample = (1 << exponent) * mantissa;
  } else {
    samples[1].sample = SNS_DD_MAX44009_MAX_READING;
  }
  DBGPRINT("raw counts %d\r\n",samples[1].sample); 
  samples[1].status = SNS_DDF_SUCCESS;
  
  s_data[0].num_samples = 2;
  s_data[0].sensor = sensor;
  s_data[0].status = SNS_DDF_SUCCESS;
  s_data[0].timestamp = sns_ddf_get_timestamp();
  s_data[0].samples = samples;  
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)    
  if (state->in_calibration_mode) {
        SNS_PRINTF_STRING_ID_FATAL_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                     DBG_DD_ALSPRX_STRING2,4404,
                                     luxvalue_before_qcom_range);
        SNS_PRINTF_STRING_ID_FATAL_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                     DBG_DD_ALSPRX_STRING2,4405,
                                     luxvalue);
        SNS_PRINTF_STRING_ID_FATAL_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                                     DBG_DD_ALSPRX_STRING2,4406,
                                     samples[1].sample);     
  }  
#endif 
  /* if called from irq, we need to set interrupt thresholds */
  if (from_irq) {
    sns_als_max44009_driver_update_int_thresh(state, exponent,mantissa,
                                              luxvalue);   
  }
  return SNS_DDF_SUCCESS;  
}

/*===========================================================================*/
/*
 * @brief sns_als_max44009_driver_update_int_thresh: Sets interrupt thresholds
 * based on exponent and mantisaa. Please note that interrupts are disabled 
 * before calling this function.
 *
 * @param[in]  state    State memory allocated by driver at init time.  
 * @param[in]  exponent List of sensors for which data is requested.
 * @param[in]  mantissa Indicates if the call is from irq or get_data
 * @param[out] lux      Sampled sensor data.
 * @param[out] samples  The data samples themselves.
 *
 * @return SNS_DDF_SUCCESS on success, error code otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e sns_als_max44009_driver_update_int_thresh
(
  sns_dd_als_state_t* state,
  uint8_t exponent,
  uint8_t mantissa,
  const q16_t lux
)

{
  uint8_t regval;
  sns_ddf_status_e ret;
  
  /* set upper threshold to exponent+1, same mantissa
   * also account for out of range and boundary conditions 
   */
  if ((exponent == SNS_DD_MAX44009_EXPONENT_OVER_RANGE) ||
      (exponent == (SNS_DD_MAX44009_EXPONENT_OVER_RANGE - 1))) {
    regval = 0xFF;
  } else {
    exponent = exponent + 1;
    regval = (exponent << 4) & 0xF0;
    regval |= (mantissa >> 4) & 0x0F;
#if USE_QUALCOMM_RANGE_FOR_LUX
    if (lux >= QUALCOMM_MAX_LUX) {
      regval = 0xFF;
    }
#endif    
  }
  
  ret = sns_dd_als_max44009_write_one_byte(state->port_handle,
                                           SNS_DD_MAX44009_THRESH_HIGH,
                                           &regval);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
  
  /* set lower threshold to exponent-1, same mantissa
   * also account for out of range and boundary conditions 
   */
  if (exponent == 0) {
    regval = 0x00;
  } else {
    exponent = exponent - 1;
    regval = (exponent << 4) & 0xF0;
    regval |= (mantissa >> 4) & 0x0F;    
#if USE_QUALCOMM_RANGE_FOR_LUX
    if (lux >= QUALCOMM_MAX_LUX) {
      regval = (QUALCOMM_MAX_LUX_EXPONENT << 4) & 0xF0;
      regval |= (QUALCOMM_MAX_LUX_MANTISSA >> 4)& 0x0F;   
    }
#endif      
  }
  ret = sns_dd_als_max44009_write_one_byte(state->port_handle,
                                           SNS_DD_MAX44009_THRESH_LOW,
                                           &regval);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
  
  /* threshold timer */
  regval = SNS_DD_MAX44009_THRESHOLD_TIMER;
  ret = sns_dd_als_max44009_write_one_byte(state->port_handle,
                                           SNS_DD_MAX44009_THRESH_TIM,
                                           &regval);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
  
  if (ret != SNS_DDF_SUCCESS)
    goto error;    
  return SNS_DDF_SUCCESS;
  
error:
  DBGPRINT("Error, function %s line %d*****\r\n", __func__, __LINE__);
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
  SNS_PRINTF_STRING_ID_FATAL_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,5512);
#endif
  return SNS_DDF_EFAIL;
  
}

/*===========================================================================*/
/*
 * @brief read_settings_reg_factory_defaults: Utility function
 * to read factory default register settings
 *
 * @param[in]  state            State memory allocated by driver at init time.  
 * @param[out] clk_trim_coarse  Coarse clock trim 
 * @param[out] clk_trim_fine    Fine clock trim 
 * @param[out] green_trim       Green trim 
 * @param[out] ir_trim          IR trim 
 *
 * @return SNS_DDF_SUCCESS on success, error code otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e read_settings_reg_factory_defaults
(
  sns_dd_als_state_t* state,
  uint8_t* clk_trim_coarse,
  uint8_t* clk_trim_fine,
  uint8_t* green_trim,
  uint8_t* ir_trim
)
{
  sns_ddf_status_e      ret;
  uint8_t               nbytes;
  uint8_t               otp_reg;
  
  /* set otp sel bit to 0 - to get factory defaults */
  otp_reg = 0x00;   
  ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44009_OTP_REG,
                               &otp_reg, 1, &nbytes);
  if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
    goto error;
  
  /* read register values. All register values are bitwise complemented*/
  sns_ddf_read_port(state->port_handle, SNS_DD_MAX44009_CLK_TRIM_COARSE,
                          clk_trim_coarse, 1, &nbytes);
  if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
    goto error;
  *clk_trim_coarse = ~(*clk_trim_coarse);
  
  ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44009_CLK_TRIM_FINE,
                          clk_trim_fine, 1, &nbytes);
  if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
    goto error;
  *clk_trim_fine = ~(*clk_trim_fine);
  
  ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44009_GREEN_TRIM,
                          green_trim, 1, &nbytes);
  if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
    goto error;
  *green_trim = ~(*green_trim);
  
  ret = sns_ddf_read_port(state->port_handle, SNS_DD_MAX44009_IR_TRIM,
                          ir_trim, 1, &nbytes);
  if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
    goto error;
  *ir_trim = ~(*ir_trim);

  return SNS_DDF_SUCCESS;
error:
  DBGPRINT("Error, function %s line %d*****\r\n", __func__, __LINE__);
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
  SNS_PRINTF_STRING_ID_FATAL_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,5514);
#endif  
  return SNS_DDF_EFAIL;
}
/*===========================================================================*/
/*
 * @brief write_settings_reg : Utility function
 * to write register settings
 *
 * @param[in]  state           State memory allocated by driver at init time.  
 * @param[in] clk_trim_coarse  Coarse clock trim 
 * @param[in] clk_trim_fine    Fine clock trim 
 * @param[in] green_trim       Green trim 
 * @param[in] ir_trim          IR trim 
 *
 * @return SNS_DDF_SUCCESS on success, error code otherwise.
 */
/*===========================================================================*/
static sns_ddf_status_e write_settings_reg
(
  sns_dd_als_state_t* state,
  uint8_t clk_trim_coarse,
  uint8_t clk_trim_fine,
  uint8_t green_trim,
  uint8_t ir_trim
)
{
  sns_ddf_status_e      ret;
  uint8_t               nbytes;
  uint8_t               otp_reg;
  
  /* set OTP sel bit in OTP reg, and write values back */
  otp_reg = SNS_DD_MAX44009_OTP_SEL;  
  ret = sns_ddf_write_port(state->port_handle, SNS_DD_MAX44009_OTP_REG,
                               &otp_reg, 1, &nbytes);
  if ((ret != SNS_DDF_SUCCESS) || (1 != nbytes))
    goto error;

  ret = sns_dd_als_max44009_write_one_byte(state->port_handle, 
                                           SNS_DD_MAX44009_CLK_TRIM_COARSE,
                                           &clk_trim_coarse);
  if (ret != SNS_DDF_SUCCESS)
    goto error;

  ret = sns_dd_als_max44009_write_one_byte(state->port_handle, 
                                           SNS_DD_MAX44009_CLK_TRIM_FINE,
                                           &clk_trim_fine);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
    
  ret = sns_dd_als_max44009_write_one_byte(state->port_handle, 
                                           SNS_DD_MAX44009_GREEN_TRIM,
                                           &green_trim);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
  
  ret = sns_dd_als_max44009_write_one_byte(state->port_handle, 
                                           SNS_DD_MAX44009_IR_TRIM,
                                           &ir_trim);
  if (ret != SNS_DDF_SUCCESS)
    goto error;  
  #if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
  SNS_PRINTF_STRING_ID_FATAL_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING2,4410,green_trim);
  SNS_PRINTF_STRING_ID_FATAL_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING2,4411,ir_trim);
  SNS_PRINTF_STRING_ID_FATAL_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING2,4412,clk_trim_coarse);
  SNS_PRINTF_STRING_ID_FATAL_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING2,4413,clk_trim_fine);
#endif
  
  return SNS_DDF_SUCCESS;
  
error:
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
  SNS_PRINTF_STRING_ID_FATAL_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,5515);
#endif  
  DBGPRINT("Error, function %s line %d*****\r\n", __func__, __LINE__);
  return SNS_DDF_EFAIL;  
}
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
/*===========================================================================
===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44009_enable_sched_data(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    bool              enable)
{
  sns_ddf_status_e      ret;
  sns_dd_als_state_t    *state = (sns_dd_als_state_t *)dd_handle;
  if (enable) {
    ret = sns_dd_als_max44009_enable_interrupt(state);
  } else {
    ret = sns_dd_als_max44009_disable_interrupt(state);
  }
  return ret;
    
}
/*===========================================================================
===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44009_enable_interrupt
(
  sns_dd_als_state_t* state
)
{
  sns_ddf_status_e      ret;
  uint8_t regval;
  /* set topsy-turvy upper and lower thresholds so that interrupt always
   * fires the first time. We adjust thresholds at that time
   */
  
  /* upper threshold = min */
  regval = 0x00;
  ret = sns_dd_als_max44009_write_one_byte(state->port_handle,
                                           SNS_DD_MAX44009_THRESH_HIGH,
                                           &regval);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
  
  /* lower threshold = max */
  regval = 0xFF;
  ret = sns_dd_als_max44009_write_one_byte(state->port_handle,
                                           SNS_DD_MAX44009_THRESH_LOW,
                                           &regval);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
  
  /* initial threshold timer */
  regval = SNS_DD_MAX44009_INIT_THRESHOLD_TIMER;
  ret = sns_dd_als_max44009_write_one_byte(state->port_handle,
                                           SNS_DD_MAX44009_THRESH_TIM,
                                           &regval);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
  
  /* interrupt enable */
  regval = 0x01;
  ret = sns_dd_als_max44009_write_one_byte(state->port_handle, 
                                           SNS_DD_MAX44009_INT_ENABLE,
                                           &regval);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
  
   ret = sns_ddf_signal_register(
            state->irq,
            state,
            &sns_als_max44009_driver_if,
            SNS_DDF_SIGNAL_IRQ_FALLING);
   if (ret != SNS_DDF_SUCCESS)
    goto error; 
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
    SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING2,4402, ret);
#endif
  DBGPRINT("********successful enable interrupt*****\r\n");
  return SNS_DDF_SUCCESS;
error:
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
    SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING2,5503, ret);
#endif  
  DBGPRINT("Error, function %s line %d*****\r\n", __func__, __LINE__);
  return ret;
}
/*===========================================================================
===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44009_disable_interrupt
(
  sns_dd_als_state_t* state
)
{
  sns_ddf_status_e      ret;
  uint8_t regval;

  /* set upper and lower thresholds to widest values and then disable interrupt*/
  /* upper threshold = max */
  regval = 0xFF;
  ret = sns_dd_als_max44009_write_one_byte(state->port_handle,
                                           SNS_DD_MAX44009_THRESH_HIGH,
                                           &regval);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
  
  /* lower threshold = min */
  regval = 0x00;
  ret = sns_dd_als_max44009_write_one_byte(state->port_handle,
                                           SNS_DD_MAX44009_THRESH_LOW,
                                           &regval);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
  
  /* threshold timer = max */
  regval = 0xFF;
  ret = sns_dd_als_max44009_write_one_byte(state->port_handle,
                                           SNS_DD_MAX44009_THRESH_TIM,
                                           &regval);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
  
  /* interrupt disable */
  regval = 0x00;
  ret = sns_dd_als_max44009_write_one_byte(state->port_handle, 
                                           SNS_DD_MAX44009_INT_ENABLE,
                                           &regval);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
  
  ret = sns_ddf_signal_deregister(state->irq);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
 #if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,4403);
#endif 
  DBGPRINT("********successful disable interrupt *****\r\n");
  return SNS_DDF_SUCCESS;
  
error:
 #if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
    SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING2,5504, ret);
#endif   
  DBGPRINT("Error, function %s line %d*****\r\n", __func__, __LINE__);
  return ret;  
}

 /*===========================================================================
===========================================================================*/
static void sns_dd_als_max44009_log_nvparams
(
  sns_dd_als_state_t* state
)
{
       
  SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_STRING2,4444,
                             state->nvparams.magic_num);
  SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_STRING2,4444,
                             state->nvparams.version_num);
  SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_STRING2,4444,
                             state->nvparams.green_scale_pct);
  SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_STRING2,4444,
                             state->nvparams.ir_scale_pct);        
  SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_STRING2,4444,
                             state->nvparams.calibration_requested);
  SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_STRING2,4444,
                             state->nvparams.green_gain_for_calibration);
  SNS_PRINTF_STRING_ID_LOW_2(SNS_DBG_MOD_DSPS_DD_ALSPRX,
                             DBG_DD_ALSPRX_STRING2,4444,
                             state->nvparams.ir_gain_for_calibration);
}
#endif // (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
 /*===========================================================================
NOTE: Stop get_data before calling test functions to adjust green and ir trim
===========================================================================*/
static sns_ddf_status_e sns_dd_als_max44009_test(
        sns_ddf_handle_t  dd_handle,
        sns_ddf_sensor_e  sensor,
        sns_ddf_test_e    test,
        uint32_t*         err)
{

  sns_ddf_status_e      ret;
  uint8_t               green_trim, ir_trim;
  uint8_t               clk_trim_coarse, clk_trim_fine;
  uint32_t custom_trim;
  

  
  sns_dd_als_state_t    *state = (sns_dd_als_state_t *)dd_handle;
  if (!state) {
    *err = 0; /*using only return error codes for now*/
    return SNS_DDF_EINVALID_PARAM;    
  }
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)  
  sns_dd_als_max44009_log_nvparams(state);
#endif
  if (!state->in_calibration_mode) {
 #if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
    SNS_PRINTF_STRING_ID_FATAL_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,5509);
#endif   
     DBGPRINT("Function %s line %d, not in calibrate mode*****\r\n", __func__, __LINE__);    
     return SNS_DDF_EFAIL;
  }
  /* read factory default reg values */
  ret = read_settings_reg_factory_defaults(state,
                                           &clk_trim_coarse,&clk_trim_fine,
                                           &green_trim,&ir_trim);
  if (ret != SNS_DDF_SUCCESS)
    goto error;
  
  DBGPRINT("Function %s test %d*****\r\n", __func__, test); 
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
  switch(test) {
  case SNS_DDF_TEST_SELF:
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,4490); 
    /* set green and ir trim to FF and 00 */
    green_trim = 0xFF;
    ir_trim = 0x00;
    ret = write_settings_reg(state,
                           clk_trim_coarse,clk_trim_fine,
                           green_trim,ir_trim);
    if (ret != SNS_DDF_SUCCESS)
      goto error;  
    break;
  case SNS_DDF_TEST_IRQ:
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,4491);
    /* set green and ir trim to FF and 7F */
    green_trim = 0xFF;
    ir_trim = 0x7F;
    ret = write_settings_reg(state,
                           clk_trim_coarse,clk_trim_fine,
                           green_trim,ir_trim);
    if (ret != SNS_DDF_SUCCESS)
      goto error;    
    break;
  case SNS_DDF_TEST_CONNECTIVITY:
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,4492);
    /* set green and ir trim to calibration values */
    green_trim = state->nvparams.green_gain_for_calibration;
    ir_trim = state->nvparams.ir_gain_for_calibration;
    ret = write_settings_reg(state,
                           clk_trim_coarse,clk_trim_fine,
                           green_trim,ir_trim);
    if (ret != SNS_DDF_SUCCESS)
      goto error;     
    break;
  case SNS_DDF_TEST_SELF_HW:
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,4493);  
    /* set green and ir trim to factory defaults */
    ret = write_settings_reg(state,
                           clk_trim_coarse,clk_trim_fine,
                           green_trim,ir_trim);
    if (ret != SNS_DDF_SUCCESS)
      goto error;     
    break;     
  case SNS_DDF_TEST_SELF_SW:
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,4494);  
    /* set green and ir trim to values passed in - if it can be passed in*/
    custom_trim = *err;
    green_trim = (custom_trim >> 16) & 0xFF;
    ir_trim = (custom_trim) & 0xFF;
    ret = write_settings_reg(state,
                           clk_trim_coarse,clk_trim_fine,
                           green_trim,ir_trim);
    if (ret != SNS_DDF_SUCCESS)
      goto error;     
    break;    
  default:
    SNS_PRINTF_STRING_ID_LOW_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,5510);
    break;
  }
#endif
  *err = 0; /*using only return error codes for now*/
  return SNS_DDF_SUCCESS;
error:
 #if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
    SNS_PRINTF_STRING_ID_FATAL_1(SNS_DBG_MOD_DSPS_DD_ALSPRX,DBG_DD_ALSPRX_STRING1,5511);
#endif     
  DBGPRINT("Error, function %s line %d*****\r\n", __func__, __LINE__);
  *err = 0; /*using only return error codes for now*/
  return SNS_DDF_EFAIL;
  
}
/*===========================================================================
===========================================================================*/

sns_ddf_driver_if_s sns_als_max44009_driver_if =
{
  &sns_dd_als_max44009_init,
  &sns_dd_als_max44009_get_data,
  &sns_dd_als_max44009_set_attrib, 
  &sns_dd_als_max44009_get_attrib,
  &sns_dd_als_max44009_handle_timer,
  &sns_dd_als_max44009_handle_irq,
  &sns_dd_als_max44009_reset,
  &sns_dd_als_max44009_test, /* test func, used to set calibration registers */
#if (!SNS_DD_COMPILE_FOR_DDK_TEST_BOARD)
  sns_dd_als_max44009_enable_sched_data 
#endif    
};
/*===========================================================================
===========================================================================*/
