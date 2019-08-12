/*==============================================================================

    S E N S O R S   RGB, AMBIENT LIGHT AND PROXIMITY  D R I V E R

DESCRIPTION

   Implements the driver for Avago RGB, ALS and Proximity Sensor APDS-9950
   This driver has 3 sub-modules:
   1. The common handler that provides the driver interface
   2. The RGB and ALS driver that handles light data type
   3. The proximity driver that handles proximity data type

********************************************************************************
* Copyright (c) 2012, Avago Technologies.
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of Avago nor the
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

==============================================================================*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2012 - 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
  -----------------------------------------------------------------------------*/
/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     -----------------------------------------------------------
30-Sep-2012   KK      (ver 1.00) Avago Technologies, at Singapore, first draft, derived from apds99xx
09-Oct-2012   KK      (ver 1.01) Implement 64-bit int in sns_dd_apds9950_rgb_convert_to_mlux() and
                      sns_dd_apds9950_rgb_convert_to_cct()
11-Oct-2012   KK      (ver 1.02) Typo error in sns_dd_apds9950_async_notify(), use data_val2
18-Jun-2013   KK      (ver 1.03) Merge with latest sns_dd_apds99xx.c from Qcom
05-Aug-2013   KK      (ver 1.04) Added SNS_DDF_SENSOR_RGB and SSN_DDF_SENSOR_C_CT
14-Aug-2013   KK      (ver 1.05) Implement as Combo driver
21-Aug-2013   KK      (ver 1.06) Correct typo error
27-Aug-2013   MW      (ver 1.07) Use SSN_DDF_SENSOR_CT_C, swap sub samples for
								 SSN_DDF_SENSOR_CT_C type, DRI mode fix for ALS
29-Aug-2013   KK      (ver 1.08) Revise RGB_COE_X,Y and Z as indicated in App Note (Pg 13)
                                 Revise sns_dd_apds9950_rgb_convert_to_mlux as indicate in App Note (Pg 10)
03-Sep-2013   KK      (ver 1.09) Incorporate 2 sub-devices implementation independently

06-Sep-2013   KK      (ver 1.10) Incorporate primary and secondary sub-device in DRI or Polling
23-Oct-2013   MW      (ver 1.11) Update driver debug message type to MSG_SSID_SNS
26-Oct-2013   KK      (ver 1.12) Add auto gain control into RGB and CT_C sub device;
                                 Correct PROXMITY calibration parameter where PRX_ENA and WAIT_ENA can't be used together;
28-Oct-2013   MW      (ver 1.13) DRI mode fix
30-Oct-2013   KK      (ver 1.14) DO NOT alter ptime register vallue in apds9950_dd_update_intg_time() as it can work with 0xFF only;
                                 Always use the Power-On-Reset value (default).
12-Nov-2013   MW      (ver 1.15) Update prox Y data format to unsigned int
06-Dec-2013   MW      (ver 1.16) CR 582067 - attribute and data log packet fix
02-Jan-2014   KK      (ver 1.17) Convert ADC raw date to irradiance value
20-Jan-2014   MW      (ver 1.18) Fix 20Hz polling timing CR 590296
22-Jan-2014   MW      (ver 1.19) Fixed RGB and CT 0x14c1 log content CR 590470
10-Mar-2014   KK      (ver 1.20) Add RGBC calibration in irradiance value
13-Mar-2014   KK      (ver 1.21) Add prx_factor in PRX calibration
22-Jul-2014   MW      (ver 1.22) Eliminate compiler warning
22-Jul-2014   MW      (ver 1.23) Eliminate compiler warning
11-Nov-2014   ps      (ver 1.24) Replace memcpy from SNS_OS_MEMSCPY
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
#include "sns_dd_apds9950_priv.h"

#ifndef ALSPRX_OEM_BUILD
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_memmgr.h"
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#endif

#ifdef ALSPRX_OEM_BUILD
#define ALSPRX_MSG_0(mod_id,str_id)                         printf(str_id)
#define ALSPRX_MSG_1(mod_id,str_id,param1)                  printf(str_id, param1)
#define ALSPRX_MSG_2(mod_id,str_id,param1,param2)           printf(str_id, param1, param2)
#define ALSPRX_MSG_3(mod_id,str_id,param1,param2,param3)    printf(str_id, param1, param2, param3)
#else
#ifdef APDS9950_DEBUG
#if defined QDSP6
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define ALSPRX_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "APDS9950 - " msg)
#define ALSPRX_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "APDS9950 - " msg,p1)
#define ALSPRX_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "APDS9950 - " msg,p1,p2)
#define ALSPRX_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "APDS9950 - " msg,p1,p2,p3)
#else
#define MED MEDIUM
#include "sns_debug_str.h"
#define ALSPRX_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define ALSPRX_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
#define ALSPRX_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
#define ALSPRX_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
#endif
#else /* APDS9950_DEBUG */
#define ALSPRX_MSG_0(level,msg)
#define ALSPRX_MSG_1(level,msg,p1)
#define ALSPRX_MSG_2(level,msg,p1,p2)
#define ALSPRX_MSG_3(level,msg,p1,p2,p3)
#endif /* APDS9950_DEBUG */
#endif /* ALSPRX_OEM_BUILD */

#define CHECK_STATUS(status)        \
  if ( status != SNS_DDF_SUCCESS )  \
  {                                 \
    return status;                  \
  }

/*============================================================================
                            STATIC VARIABLE DEFINITIONS
============================================================================*/
#ifndef ALSPRX_OEM_BUILD
extern volatile boolean EnableI2C;  /* Avago's DDK platform doesn't support this */ 
#endif

static uint8_t sns_dd_als_atime_tb[] = { 0xF6, 0xEB, 0xD6 }; /* 24ms,  50.4ms , 100.8ms */
static uint32_t sns_dd_als_atime_us_tb[] = { 24000, 50400, 100800 }; /* 24ms,  50.4ms , 100.8ms */
static uint16_t sns_dd_als_res_tb[] = { 0x2800, 0x5400, 0xA800 }; /* 10240, 21504, 43008 */
static uint8_t sns_dd_als_adc_bit_tb[] = { 14, 15, 16 };
static uint32_t sns_dd_als_integration_time_tb[] = { 36000, 75000, 150000 };

static uint8_t sns_dd_als_again_tb[] = { 1, 4, 16, 64 };
static uint8_t sns_dd_als_again_bit_tb[] = {0x00, 0x01, 0x02, 0x03 };

static int32_t RGB_COE_X[3] = {-652, 6478, -5115}; 
static int32_t RGB_COE_Y[3] = {-998, 6788, -4959};  
static int32_t RGB_COE_Z[3] = {-1997, 5431, -2340}; 

static int32_t RGB_CIE_N1 = 332;
static int32_t RGB_CIE_N2 = 186;

static int32_t RGB_CIE_CCT1 = 449; 
static int32_t RGB_CIE_CCT2 = 3525;
static int32_t RGB_CIE_CCT3 = 6823;
static int32_t RGB_CIE_CCT4 = 5520;

static const sns_ddf_sensor_e apds9950_dd_first_sub_sensors[APDS9950_DD_NUM_SENSOR_TYPES] =
{
  SNS_DDF_SENSOR_PROXIMITY,
  SNS_DDF_SENSOR_AMBIENT,
};

static const sns_ddf_sensor_e apds9950_dd_second_sub_sensors[APDS9950_DD_NUM_SENSOR_TYPES] =
{
  SNS_DDF_SENSOR_RGB,
  SNS_DDF_SENSOR_CT_C,
};

static sns_ddf_sensor_e prx_timer = SNS_DDF_SENSOR_PROXIMITY;
static sns_ddf_sensor_e als_timer = SNS_DDF_SENSOR_AMBIENT;
static sns_ddf_sensor_e rgb_timer = SNS_DDF_SENSOR_RGB;
static sns_ddf_sensor_e cct_timer = SNS_DDF_SENSOR_CT_C; 

static bool first_init=FALSE;
static bool first_probe=FALSE;

static apds9950_dd_head_s apds9950_dd_head = {NULL, {"APDS-9950", 1}};

/*============================================================================
                           STATIC FUNCTION PROTOTYPES
============================================================================*/

static sns_ddf_status_e sns_dd_apds9950_init
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

static sns_ddf_status_e sns_dd_apds9950_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
);

static sns_ddf_status_e sns_dd_apds9950_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
);

static sns_ddf_status_e sns_dd_apds9950_get_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**               value,
  uint32_t*            num_elems
);

static void sns_dd_apds9950_handle_timer 
(
  sns_ddf_handle_t dd_handle, 
  void* arg
);

static void sns_dd_apds9950_handle_irq
(
  sns_ddf_handle_t dd_handle, 
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
);

static sns_ddf_status_e sns_dd_apds9950_reset
(
  sns_ddf_handle_t dd_handle
);

sns_ddf_status_e sns_dd_apds9950_enable_sched_data
(
  sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor,
  bool              enable
);

static sns_ddf_status_e sns_dd_apds9950_calibration
(
  sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor,
  sns_ddf_test_e    test,
  uint32_t*         err
);

static sns_ddf_status_e sns_dd_apds9950_probe
(
 sns_ddf_device_access_s* device_info,
 sns_ddf_memhandler_s*    memhandler,
 uint32_t*                num_sensors,
 sns_ddf_sensor_e**       sensors
);

sns_ddf_driver_if_s sns_dd_apds9950_driver_if =
{
  &sns_dd_apds9950_init,
  &sns_dd_apds9950_get_data,
  &sns_dd_apds9950_set_attrib, 
  &sns_dd_apds9950_get_attrib,
  &sns_dd_apds9950_handle_timer,
  &sns_dd_apds9950_handle_irq,
  &sns_dd_apds9950_reset,
  &sns_dd_apds9950_calibration, /* calibration test - ALS and PRX, RGB */
  &sns_dd_apds9950_enable_sched_data,
  &sns_dd_apds9950_probe
};

/*===========================================================================
FUNCTION      sns_dd_apds9950_log
  
DESCRIPTION   Log the latest sensor data

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 
===========================================================================*/
void sns_dd_apds9950_log(
   apds9950_dd_dev_s* dd_ptr,
   sns_ddf_sensor_e       sensor,
   uint32_t               data1, 
   uint32_t               data2,
   uint32_t               data3,
   uint32_t               data4,
   uint32_t               data5 )
{
#ifndef ALSPRX_OEM_BUILD  
   sns_err_code_e err_code;
   sns_log_sensor_data_pkt_s* log_struct_ptr;

   /* Allocate log packet */
   err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                                sizeof(sns_log_sensor_data_pkt_s),
                                (void**)&log_struct_ptr);

   if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
   {
     log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
     log_struct_ptr->sensor_id = sensor;
     log_struct_ptr->vendor_id = SNS_DDF_VENDOR_AVAGO;

     /* Timestamp the log with sample time */
     log_struct_ptr->timestamp = sns_ddf_get_timestamp();

     /* Log the sensor data */
     if (sensor == SNS_DDF_SENSOR_AMBIENT)
     {
       log_struct_ptr->num_data_types = 3;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data2;
       log_struct_ptr->data[2] = data3;
     }
     else if(sensor == SNS_DDF_SENSOR_PROXIMITY)
     {
       log_struct_ptr->num_data_types = 5;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data2;
       log_struct_ptr->data[2] = data3;
       log_struct_ptr->data[3] = data4;
       log_struct_ptr->data[4] = data5;
     }
		 else if(sensor == SNS_DDF_SENSOR_RGB)
     {
       log_struct_ptr->num_data_types = 3;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data2;
       log_struct_ptr->data[2] = data3;
     }
		 else if(sensor == SNS_DDF_SENSOR_CT_C)
     {
       log_struct_ptr->num_data_types = 2;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data2;
     }

     /* Commit log (also frees up the log packet memory) */
     err_code = sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA,
                                  log_struct_ptr);
   }
   else
   {
     dd_ptr->dropped_logs++;
     ALSPRX_MSG_3(ERROR,"log - %d dropped=%d err=%d", 
                  11, dd_ptr->dropped_logs, err_code);
   }
#endif
}

/*===========================================================================

  FUNCTION:   sns_dd_isl29028_int_disable

===========================================================================*/
/*!
  @brief Disable ISL29028 INT since it shares a pin with APDS99xx.
 
  @detail
  Resets  the driver state structure
 
  @param[in] handle  Handle to a driver instance.
 
  @return Success if the driver was able to reset its state and the device.
          Otherwise a specific error code is returned. 
*/
/*=========================================================================*/
#ifdef FEATURE_APDS99XX_INT_PIN_CONFIG 

#define ISL29028_I2C_ADDR 0x45
#define APDS9950_DD_ISL_ALSPRX_CONFIG_REG         0x01
#define APDS9950_DD_ISL_ALSPRX_INTERRUPT_REG      0x02
#define APDS9950_DD_ISL_ALSPRX_PROX_LT_REG        0x03
#define APDS9950_DD_ISL_ALSPRX_PROX_HT_REG        0x04
#define APDS9950_DD_ISL_ALSPRX_ALSIR_TH1_REG      0x05
#define APDS9950_DD_ISL_ALSPRX_ALSIR_TH2_REG      0x06
#define APDS9950_DD_ISL_ALSPRX_ALSIR_TH3_REG      0x07
#define APDS9950_DD_ISL_ALSPRX_ALSIR_TEST1_REG    0x0E
#define APDS9950_DD_ISL_ALSPRX_ALSIR_TEST2_REG    0x0F

typedef struct {
   sns_ddf_handle_t         smgr_handle;    /* SDDI handle used to notify_data */
   uint32_t                 dropped_logs;
   sns_ddf_handle_t         port_handle;    /* handle used to access the I2C bus */
   als_type       als;
   prx_type       prx;
   common_type sns_dd_alsprx_common_db;
} sns_dd_alsprx_state_t;

sns_ddf_status_e sns_dd_isl29028_int_disable()
{
  sns_ddf_status_e status;
  uint8_t data, out;
  sns_dd_alsprx_state_t* ptr;
  sns_ddf_i2c_config_s    i2c_config;
  sns_ddf_device_access_s device;

  device.device_select = 0;
  device.port_config.bus = SNS_DDF_BUS_I2C;
  device.port_config.bus_config.i2c  = &i2c_config;
  i2c_config.addr_type = SNS_DDF_I2C_ADDR_7BIT;
  i2c_config.bus_acq_timeout = -1;
  i2c_config.bus_freq = 400;
  i2c_config.dev_type = SNS_DDF_I2C_DEVICE_REGADDR;
  i2c_config.read_opt = SNS_DDF_I2C_START_BEFORE_RD;
  i2c_config.slave_addr = ISL29028_I2C_ADDR;
  i2c_config.xfer_timeout = -1;

  status = sns_ddf_malloc((void **)&ptr, sizeof(sns_dd_alsprx_state_t));
  if (status != SNS_DDF_SUCCESS)
  {
    return status;
  }
  status = sns_ddf_open_port(&(ptr->port_handle), 
                              &(device.port_config));

  if (status != SNS_DDF_SUCCESS)
  {
    sns_ddf_mfree(ptr);
    return status;
  }
    
  /* ISL29028a reset sequence */
  data =  0x29; 
  sns_ddf_write_port(ptr->port_handle,
                     APDS9950_DD_ISL_ALSPRX_ALSIR_TEST2_REG, 
                     &data, 1, &out);

  data =  0x0; 
  sns_ddf_write_port(ptr->port_handle,
                     APDS9950_DD_ISL_ALSPRX_ALSIR_TEST1_REG, 
                     &data, 1, &out);

  data =  0x0; 
  sns_ddf_write_port(ptr->port_handle,
                     APDS9950_DD_ISL_ALSPRX_ALSIR_TEST2_REG, 
                     &data, 1, &out);

  data =  0x0; 
  sns_ddf_write_port(ptr->port_handle,
                     APDS9950_DD_ISL_ALSPRX_CONFIG_REG, 
                     &data, 1, &out);

  /* write valid information to INTERRUPT register */
  data =  0x7; //16 conversions for ALS; AND  
  sns_ddf_write_port(ptr->port_handle,
                     APDS9950_DD_ISL_ALSPRX_INTERRUPT_REG, 
                     &data, 1, &out);

  /* update thresholds so interrupts are not generated */
  data =  0xFF; 
  sns_ddf_write_port(ptr->port_handle,
                     APDS9950_DD_ISL_ALSPRX_PROX_LT_REG, 
                     &data, 1, &out);

  data =  0xFF; 
  sns_ddf_write_port(ptr->port_handle,
                     APDS9950_DD_ISL_ALSPRX_PROX_HT_REG, 
                     &data, 1, &out);

  data =  0xFF; 
  sns_ddf_write_port(ptr->port_handle,
                     APDS9950_DD_ISL_ALSPRX_ALSIR_TH1_REG, 
                     &data, 1, &out);

  data =  0xFF; 
  sns_ddf_write_port(ptr->port_handle,
                     APDS9950_DD_ISL_ALSPRX_ALSIR_TH2_REG, 
                     &data, 1, &out);

  data =  0xFF; 
  sns_ddf_write_port(ptr->port_handle,
                     APDS9950_DD_ISL_ALSPRX_ALSIR_TH3_REG, 
                     &data, 1, &out);

  sns_ddf_close_port(ptr->port_handle);
  sns_ddf_mfree(ptr);
  if (status != SNS_DDF_SUCCESS)
  {
     return status;
  }

  return SNS_DDF_SUCCESS;
}
#endif

/*===========================================================================

FUNCTION      apds9950_dd_find_same_port_dev
  
DESCRIPTION   

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
apds9950_dd_dev_s* apds9950_dd_find_same_port_dev
(
  sns_ddf_port_config_s* port_config
)
{
  if (port_config == NULL) return NULL;
  if (port_config->bus_config.i2c == NULL) return NULL;
  
  if (port_config->bus_config.i2c->slave_addr == 0x39)
  {
    if (first_init == FALSE && apds9950_dd_head.next == NULL) 
    {
      first_init = TRUE;
      return apds9950_dd_head.next;
    }
    else
    {
      if (apds9950_dd_head.next != NULL)
      {
        first_init = TRUE;
        return apds9950_dd_head.next;
      }
      else 
      {
        first_init = FALSE;
        return apds9950_dd_head.next;
      }
    }
  }
  
  return NULL;
}

/*===========================================================================

FUNCTION      apds9950_dd_fill_sub_dev_slot
  
DESCRIPTION   

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
uint32_t apds9950_dd_fill_sub_dev_slot
(
  apds9950_dd_sub_dev_s* sub_dev,
  uint32_t num_sensors,
  sns_ddf_sensor_e* sensors
)
{
  uint32_t i;
  
  if (sub_dev == NULL) return 0;
  if (num_sensors > APDS9950_DD_NUM_SENSOR_TYPES) num_sensors = APDS9950_DD_NUM_SENSOR_TYPES;

  sub_dev->num_sensors = num_sensors;
  for (i=0; i<num_sensors;i++)
  {
    sub_dev->sensors[i] = *(sensors+i);
  }
  
  return num_sensors;
}

/*===========================================================================

FUNCTION      apds9950_find_free_sub_dev_slot
  
DESCRIPTION   

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
uint32_t apds9950_find_free_sub_dev_slot
(
  apds9950_dd_dev_s* dd_ptr
)
{
  if (dd_ptr == NULL) return -1;
  if (dd_ptr->sub_dev_cnt >= APDS9950_DD_MAX_SUB_DEV) return -1;

  return dd_ptr->sub_dev_cnt;
}

/*===========================================================================

FUNCTION      apds9950_dd_set_prx_thresh
  
DESCRIPTION   

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
sns_ddf_status_e apds9950_dd_set_prx_thresh
(
  apds9950_dd_dev_s* dd_ptr,
  uint16_t  pilt,
  uint16_t  piht
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t bytes_w;
  uint8_t thresh[4];

  thresh[0] = (pilt & 0xFF); /* PILTL */
  thresh[1] = (pilt >> 8);   /* PILTH */
  thresh[2] = (piht & 0xFF); /* PIHTL */
  thresh[3] = (piht >> 8);   /* PIHTH */

  status = sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9950_DD_CMD_WORD|APDS9950_DD_PILTL_ADDR,
                              (uint8_t*)&thresh[0], 4, &bytes_w);
  CHECK_STATUS(status);

#ifdef ALSPRX_DEBUG
  {
    uint8_t   i2c_read_data[4];

    status = sns_ddf_read_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_BYTE|APDS9950_DD_PILTL_ADDR,
                               (uint8_t*)&i2c_read_data[0], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_BYTE|APDS9950_DD_PILTH_ADDR,
                               (uint8_t*)&i2c_read_data[1], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_BYTE|APDS9950_DD_PIHTL_ADDR,
                               (uint8_t*)&i2c_read_data[2], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_BYTE|APDS9950_DD_PIHTH_ADDR,
                               (uint8_t*)&i2c_read_data[3], 1, &bytes_w);

    ALSPRX_MSG_2(HIGH, "set_prx_thresh - data0=%d data1=%d",
                 i2c_read_data[0], i2c_read_data[1]);
    ALSPRX_MSG_2(HIGH, "set_prx_thresh - data2=%d data3=%d",
                 i2c_read_data[2], i2c_read_data[3]);  }
#endif

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      apds9950_dd_set_als_thresh
  
DESCRIPTION   

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
sns_ddf_status_e apds9950_dd_set_als_thresh
(
  apds9950_dd_dev_s* dd_ptr,
  uint16_t  ailt,
  uint16_t  aiht
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t bytes_w;
  uint8_t thresh[4];

  thresh[0] = (ailt & 0xFF); /* AILTL */
  thresh[1] = (ailt >> 8);   /* AILTH */
  thresh[2] = (aiht & 0xFF); /* AIHTL */
  thresh[3] = (aiht >> 8);   /* AIHTH */


  status = sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9950_DD_CMD_WORD|APDS9950_DD_AILTL_ADDR,
                              (uint8_t*)&thresh[0], 4, &bytes_w);
  CHECK_STATUS(status);

#ifdef ALSPRX_DEBUG
  {
    uint8_t   i2c_read_data[4];

    status = sns_ddf_read_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_BYTE|APDS9950_DD_AILTL_ADDR,
                               (uint8_t*)&i2c_read_data[0], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_BYTE|APDS9950_DD_AILTH_ADDR,
                               (uint8_t*)&i2c_read_data[1], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_BYTE|APDS9950_DD_AIHTL_ADDR,
                               (uint8_t*)&i2c_read_data[2], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_BYTE|APDS9950_DD_AIHTH_ADDR,
                               (uint8_t*)&i2c_read_data[3], 1, &bytes_w);

    ALSPRX_MSG_2(HIGH, "set_als_thresh - data0=%d data1=%d",
                 i2c_read_data[0], i2c_read_data[1]);
    ALSPRX_MSG_2(HIGH, "set_als_thresh - data2=%d data3=%d",
                 i2c_read_data[2], i2c_read_data[3]);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_BYTE|APDS9950_DD_PILTL_ADDR,
                               (uint8_t*)&i2c_read_data[0], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_BYTE|APDS9950_DD_PILTH_ADDR,
                               (uint8_t*)&i2c_read_data[1], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_BYTE|APDS9950_DD_PIHTL_ADDR,
                               (uint8_t*)&i2c_read_data[2], 1, &bytes_w);
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_BYTE|APDS9950_DD_PIHTH_ADDR,
                               (uint8_t*)&i2c_read_data[3], 1, &bytes_w);
    
    ALSPRX_MSG_2(HIGH, "set_als_thresh - data0=%d data1=%d",
                 i2c_read_data[0], i2c_read_data[1]);
    ALSPRX_MSG_2(HIGH, "set_als_thresh - data2=%d data3=%d",
                 i2c_read_data[2], i2c_read_data[3]);
  }
#endif

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      apds9950_dd_set_cycle_time
  
DESCRIPTION   

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
void apds9950_dd_set_cycle_time
(
  apds9950_dd_dev_s* dd_ptr
)
{
  uint32_t max_odr;
  //uint32_t odr;

  max_odr = MAX(dd_ptr->common.als_req_odr,
                dd_ptr->common.prx_req_odr);

  if (max_odr >= 17) 
  {
    /* resulting freq = 20Hz */
    //odr = 20; /* Hz */
    dd_ptr->common.cycle_time = 40000; /* us */
  }
  else if (max_odr >= 13) 
  {
    /* resulting freq = 16.8Hz */
    //odr = 16;
    dd_ptr->common.cycle_time = 50000;
  }
  else if (max_odr >= 9) 
  {
    /* resulting freq = 12.8Hz */
    //odr = 12;
    dd_ptr->common.cycle_time = 66667;
  }
  else if (max_odr >= 5) 
  {
    /* resulting freq = 8.9Hz */
    //odr = 8;
    dd_ptr->common.cycle_time = 100000;
  }
  else if (max_odr > 0) 
  {
    /* resulting freq = 4.7Hz */
    /* (max_odr > 0) && (max_odr <=5) */
    //odr = 4;
    dd_ptr->common.cycle_time = 200000;
  }
  else 
  {
    //odr = 0;
    dd_ptr->common.cycle_time = 0;
  }

  dd_ptr->common.als_odr = dd_ptr->common.als_req_odr; /* Hz */
  dd_ptr->common.prx_odr = dd_ptr->common.prx_req_odr; /* Hz */
}

/*===========================================================================

FUNCTION      apds9950_dd_update_intg_time
  
DESCRIPTION   

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
sns_ddf_status_e apds9950_dd_update_intg_time
(
  apds9950_dd_dev_s* dd_ptr,
  bool prx_enabled,
  bool als_enabled,
  bool static_setting
)
{
  uint8_t          atime=0;
//  uint8_t          ptime=0xFF; // kk fixed it or don't alter it at all
  uint8_t          wtime=0;
  uint32_t         cycle_time_us;
  uint32_t         atime_us=0;
  uint32_t         ptime_us=2400;
  int32_t          wtime_us=0;
  int32_t          wtime_32=0;
  uint8_t          bytes_w;
  int8_t           res_idx;
  sns_ddf_status_e status;

  if (static_setting)
  {
    /* ATIME */
    atime  = sns_dd_als_atime_tb[dd_ptr->als.als_res_index];
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                APDS9950_DD_CMD_BYTE|APDS9950_DD_ATIME_ADDR,
                                (uint8_t*)&atime, 1, &bytes_w);
    CHECK_STATUS(status);
    
    /* WTIME */
    wtime  = APDS9950_DD_2_4MS_ADC_TIME;  /* reset to default so that prx calibration can use it */
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                APDS9950_DD_CMD_BYTE|APDS9950_DD_WTIME_ADDR,
                                (uint8_t*)&wtime, 1, &bytes_w);
    CHECK_STATUS(status);    

    /* PTIME */
    /* kk 30-Oct-2013
    ptime  = APDS9950_DD_2_4MS_ADC_TIME;
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                APDS9950_DD_CMD_BYTE|APDS9950_DD_PTIME_ADDR,
                                (uint8_t*)&ptime, 1, &bytes_w);
    CHECK_STATUS(status);
    */
  }
  else 
  {
    /* Set ATIME, PTIME, WTIME */
    cycle_time_us = dd_ptr->common.cycle_time;

    /* PTIME */
    if (prx_enabled) 
    {
      //ptime  = APDS9950_DD_2_4MS_ADC_TIME;
      ptime_us = 2400;
    }
	
    /* ATIME */
    if (als_enabled) 
    {
      for (res_idx=dd_ptr->als.als_res_index; res_idx >=0; res_idx--) 
      {
        atime_us = sns_dd_als_atime_us_tb[res_idx];
        if (cycle_time_us > atime_us) 
        {
          ALSPRX_MSG_3(HIGH, "update_intg_time - idx=%d cycle_time=%d atime=%d", res_idx, cycle_time_us, atime_us);
          
          break; /* current atime works */
        }
      } /* for */
      ALSPRX_MSG_2(HIGH, "update_intg_time - %d idx=%d", 600, res_idx);

      if (res_idx >=0) 
      {
        if (res_idx != dd_ptr->als.als_res_index) 
        {
          /* reducing resolution and atime to get the right ODR */
          dd_ptr->als.als_res_index = res_idx;
        }
      }
      else 
      {
        dd_ptr->als.als_res_index = 0; /* pick smallest atime */
        ALSPRX_MSG_1(ERROR, "update_intg_time - %d", 666);
      }
      atime = sns_dd_als_atime_tb[dd_ptr->als.als_res_index];
      atime_us = sns_dd_als_atime_us_tb[dd_ptr->als.als_res_index];
    }
    else 
    {
      /* ALS will be ON for PRX support */
      atime = 0xFC; /* try reducing atime for prx only to 10.88ms */
      atime_us = 10880;
    }

    /* WTIME */
    wtime_us = (cycle_time_us - (atime_us + ptime_us));
    if (wtime_us < 0) 
    {
      /* This should not happen. Make sure cycle time is computed correctly
         and atime is picked correctly. */
      wtime = 0xFF; /* minumum one cycle */
      ALSPRX_MSG_1(ERROR, "update_intg_time - wtime=%d", wtime);
    }
    else 
    {
      wtime_32 = (int32_t)(256 - (wtime_us / 2400)); /* 32 bit math */
      wtime = (uint8_t)wtime_32; /* convert to 8 bits */
    }

    ALSPRX_MSG_2(ERROR, "update_intg_time - atime=%d wtime=%d", 
                 atime, wtime);
    ALSPRX_MSG_3(ERROR, "update_intg_time - ptime_us=%d atime_us=%d wtime_us=%d", 
                 ptime_us, atime_us, wtime_us);

    /* Program */
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                APDS9950_DD_CMD_BYTE|APDS9950_DD_ATIME_ADDR,
                                &atime, 1, &bytes_w);
    CHECK_STATUS(status);

    /* kk 30-Oct-2013
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                  APDS9950_DD_CMD_BYTE|APDS9950_DD_PTIME_ADDR,
                                  &ptime, 1, &bytes_w);
    CHECK_STATUS(status);
    */

    status = sns_ddf_write_port(dd_ptr->port_handle,
                                  APDS9950_DD_CMD_BYTE|APDS9950_DD_WTIME_ADDR,
                                  &wtime, 1, &bytes_w);
    CHECK_STATUS(status);
  }

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      apds9950_dd_clear_int_reg

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  sns_ddf_status_e

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e apds9950_dd_clear_int_reg
(
  apds9950_dd_dev_s* dd_ptr,
  uint8_t reg_addr
)
{
  uint8_t written = 0;
  sns_ddf_status_e status;
#ifndef QDSP6
  status = sns_ddf_write_port(dd_ptr->ZBW_port_handle, 0, &reg_addr, 1, &written);
#else
  status = sns_ddf_write_port(dd_ptr->port_handle, reg_addr, NULL, 0, &written);
#endif
  ALSPRX_MSG_1(MED, "clear_int_reg - reg_addr=0x%x", reg_addr);
  return status;
}

/*===========================================================================

FUNCTION      apds9950_dd_enable_interrupt
  
DESCRIPTION   

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
sns_ddf_status_e apds9950_dd_enable_interrupt
(
  sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t i2c_data = 0x0;
  uint8_t bytes_rw;
  uint8_t i2c_pon_data = APDS9950_DD_PWR_ON;
  apds9950_dd_dev_s* dd_ptr;
  uint32_t sub_dev_idx;
  uint8_t enable_reg;

  sub_dev_idx = (uint32_t)dd_handle & (APDS9950_DD_HANDLE_ALIGN-1);
  dd_ptr = (apds9950_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9950_DD_HANDLE_ALIGN-1)));
  
  ALSPRX_MSG_1(HIGH, "ENABLE INTERRUPT START sub device ID=%d", sub_dev_idx);

  ALSPRX_MSG_3(MED, "ENABLE INTERRUPT START    enable_interrupt - sensor=%d prx=%d als=%d",
               sensor, dd_ptr->prx.enable, dd_ptr->als.enable);
  
  status = sns_ddf_read_port(dd_ptr->port_handle,
                             APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                             &i2c_data, 1, &bytes_rw);
  
  CHECK_STATUS(status);

  enable_reg = i2c_data;
    
  if ((sensor != SNS_DDF_SENSOR_PROXIMITY) && (sensor != SNS_DDF_SENSOR_AMBIENT) &&
      (sensor != SNS_DDF_SENSOR_RGB) && (sensor != SNS_DDF_SENSOR_CT_C) ) 
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  if (SNS_DDF_SENSOR_PROXIMITY == sensor) 
  {
    if (dd_ptr->prx.enable == TRUE) 
    {
      ALSPRX_MSG_0(ERROR, "ENABLE INTERRUPT END    proximity already enabled");
      return SNS_DDF_SUCCESS;
    }
    
    /* ENABLE=PON */
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                                &i2c_pon_data, 1, &bytes_rw);
    CHECK_STATUS(status);

    /* Clear interrupts */
    status = apds9950_dd_clear_int_reg(dd_ptr, APDS9950_DD_CMD_CLR_PS_ALS_INT);
    CHECK_STATUS(status);

    status = apds9950_dd_update_intg_time(dd_ptr, TRUE, (dd_ptr->als.enable|dd_ptr->rgb.enable|dd_ptr->cct.enable), FALSE);
    CHECK_STATUS(status);

    status = apds9950_dd_set_prx_thresh(dd_ptr, 0x3FF, 0);  // force an interrupt
    CHECK_STATUS(status);
  
    /* ENABLE */
    i2c_data = enable_reg | (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|APDS9950_DD_PRX_ENABLE|
                APDS9950_DD_PRX_INT_ENABLE | APDS9950_DD_WAIT_ENABLE); 
    
    dd_ptr->prx.enable = TRUE;
  }
  else if (SNS_DDF_SENSOR_AMBIENT == sensor) 
  {
    if (dd_ptr->als.enable == TRUE) 
    {
      return SNS_DDF_SUCCESS;
    }

    /* ENABLE=PON */
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                                &i2c_pon_data, 1, &bytes_rw);
    CHECK_STATUS(status);

    /* Clear interrupts */
    status = apds9950_dd_clear_int_reg(dd_ptr, APDS9950_DD_CMD_CLR_PS_ALS_INT);      
    CHECK_STATUS(status);
  
    if (!dd_ptr->sub_dev[1].dri_enabled ||
        (dd_ptr->sub_dev[1].dri_enabled && (!dd_ptr->rgb.enable && !dd_ptr->cct.enable)))
    {
      // do not need to disturb if other sub-dev sensors are usig it
      status = apds9950_dd_update_intg_time(dd_ptr, dd_ptr->prx.enable, TRUE, FALSE);
      CHECK_STATUS(status);     
    }
    
    status = apds9950_dd_set_als_thresh(dd_ptr, 0xFFFF, 0); // force AMBIENT interrupt
      CHECK_STATUS(status);

    /* ENABLE */
    i2c_data = enable_reg | (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|APDS9950_DD_ALS_INT_ENABLE|
                APDS9950_DD_WAIT_ENABLE);

    dd_ptr->als.enable = TRUE;
  }
  else if (SNS_DDF_SENSOR_RGB == sensor) 
  {
    if (dd_ptr->rgb.enable == TRUE) 
    {
      return SNS_DDF_SUCCESS;
    }
    
    /* ENABLE=PON */
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                                &i2c_pon_data, 1, &bytes_rw);
    CHECK_STATUS(status);
  
    /* Clear interrupts */
    status = apds9950_dd_clear_int_reg(dd_ptr, APDS9950_DD_CMD_CLR_PS_ALS_INT);
    CHECK_STATUS(status);
  
    if ((!dd_ptr->sub_dev[0].dri_enabled && !dd_ptr->als.enable) ||
        (dd_ptr->sub_dev[0].dri_enabled && !dd_ptr->als.enable))
    {
      if (!dd_ptr->cct.enable)
      {
        status = apds9950_dd_update_intg_time(dd_ptr, dd_ptr->prx.enable, TRUE, FALSE);
        CHECK_STATUS(status);
      }
    }

    status = apds9950_dd_set_als_thresh(dd_ptr, 0xFFFF, 0); // force AMBIENT interrupt
        CHECK_STATUS(status);
  
    /* ENABLE */
    i2c_data = enable_reg | (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|APDS9950_DD_ALS_INT_ENABLE|
                  APDS9950_DD_WAIT_ENABLE);
      
    dd_ptr->rgb.enable = TRUE;
  }
  else if (SNS_DDF_SENSOR_CT_C == sensor) 
  {
    if (dd_ptr->cct.enable == TRUE) 
    {
      return SNS_DDF_SUCCESS;
    }

    /* ENABLE=PON */
    status = sns_ddf_write_port(dd_ptr->port_handle,
                                APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                                &i2c_pon_data, 1, &bytes_rw);
    CHECK_STATUS(status);

    /* Clear interrupts */
    status = apds9950_dd_clear_int_reg(dd_ptr, APDS9950_DD_CMD_CLR_PS_ALS_INT);
    CHECK_STATUS(status);

    if ((!dd_ptr->sub_dev[0].dri_enabled && !dd_ptr->als.enable) ||
        (dd_ptr->sub_dev[0].dri_enabled && !dd_ptr->als.enable))
    {
      if (!dd_ptr->rgb.enable)
      {
        status = apds9950_dd_update_intg_time(dd_ptr, dd_ptr->prx.enable, TRUE, FALSE);
        CHECK_STATUS(status);
      }
    }

    status = apds9950_dd_set_als_thresh(dd_ptr, 0xFFFF, 0); // force AMBIENT interrupt
        CHECK_STATUS(status);

    /* ENABLE */
    i2c_data = enable_reg | (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|APDS9950_DD_ALS_INT_ENABLE|
                APDS9950_DD_WAIT_ENABLE);
    
    dd_ptr->cct.enable = TRUE;
  }

  /* Update ENABLE bits*/
  status = sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                              &i2c_data, 1, &bytes_rw);
  CHECK_STATUS(status);

  ALSPRX_MSG_1(MED, "dd_ptr->sub_dev[1].dri_enabled = %d", dd_ptr->sub_dev[1].dri_enabled);
  ALSPRX_MSG_1(MED, "dd_ptr->rgb.enable = %d", dd_ptr->rgb.enable);
  ALSPRX_MSG_1(MED, "dd_ptr->cct.enable = %d", dd_ptr->cct.enable);

  if (sub_dev_idx == 0)
  {
    if ( !dd_ptr->sub_dev[1].dri_enabled ||
         (dd_ptr->sub_dev[1].dri_enabled && (!dd_ptr->rgb.enable && !dd_ptr->cct.enable)) )
    {
      dd_ptr->sub_dev[sub_dev_idx].dri_registration = TRUE;
      status = sns_ddf_signal_register(dd_ptr->sub_dev[sub_dev_idx].interrupt_gpio, dd_handle, 
                                       &sns_dd_apds9950_driver_if, SNS_DDF_SIGNAL_IRQ_FALLING);
	  ALSPRX_MSG_2(MED, "sub device %d interrupt signal registered, status=%d", sub_dev_idx, status);
    }
  }
  else
  {
    if ( !dd_ptr->sub_dev[0].dri_enabled ||
         (dd_ptr->sub_dev[0].dri_enabled && (!dd_ptr->prx.enable && !dd_ptr->als.enable)) )
    {
      dd_ptr->sub_dev[sub_dev_idx].dri_registration = TRUE;
      status = sns_ddf_signal_register(dd_ptr->sub_dev[sub_dev_idx].interrupt_gpio, dd_handle, 
                                       &sns_dd_apds9950_driver_if, SNS_DDF_SIGNAL_IRQ_FALLING);
	  ALSPRX_MSG_2(MED, "sub device %d interrupt signal registered, status=%d", sub_dev_idx, status);
    }    
  }

  ALSPRX_MSG_2(MED, "ENABLE INTERRUPT END enable_int - data=%d status=%d", i2c_data, status);

  return status;
}

/*===========================================================================

FUNCTION      apds9950_dd_disable_interrupt
  
DESCRIPTION   

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
sns_ddf_status_e apds9950_dd_disable_interrupt
(
  sns_ddf_handle_t  dd_handle,
  sns_ddf_sensor_e  sensor
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t i2c_data = APDS9950_DD_PWR_DOWN;
  uint8_t i2c_pon_data = APDS9950_DD_PWR_ON;
  uint8_t bytes_w;
  apds9950_dd_dev_s* dd_ptr;
  uint32_t sub_dev_idx;

  sub_dev_idx = (uint32_t)dd_handle & (APDS9950_DD_HANDLE_ALIGN-1);
  dd_ptr = (apds9950_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9950_DD_HANDLE_ALIGN-1)));

  if ((sensor != SNS_DDF_SENSOR_PROXIMITY) && (sensor != SNS_DDF_SENSOR_AMBIENT) &&
      (sensor != SNS_DDF_SENSOR_RGB) && (sensor != SNS_DDF_SENSOR_CT_C) ) 
  {
    return SNS_DDF_EINVALID_PARAM;
  }
  
  ALSPRX_MSG_1(LOW, "DISABLE INTERRUPT START    disable_int - sensor=%d", sensor);
  
  if (SNS_DDF_SENSOR_PROXIMITY == sensor) 
  {
    if (!dd_ptr->prx.enable) 
    {
      return SNS_DDF_SUCCESS;
    }

    if (dd_ptr->als.enable) 
    {
      i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|APDS9950_DD_ALS_INT_ENABLE|
                  APDS9950_DD_WAIT_ENABLE);        
    }
    else if (dd_ptr->rgb.enable || dd_ptr->cct.enable)
    {
      if (!dd_ptr->sub_dev[1].dri_enabled)
      {
        // Polling
        i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE);
      }
      else
      {
        // DRI
        i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|APDS9950_DD_ALS_INT_ENABLE|
                    APDS9950_DD_WAIT_ENABLE);        
      }
    }

    dd_ptr->prx.enable = FALSE;
    dd_ptr->common.prx_req_odr = 0;
    dd_ptr->common.prx_odr = 0;
  }
  else if ( SNS_DDF_SENSOR_AMBIENT == sensor ) 
  {
    if (!dd_ptr->als.enable) 
    {
      return SNS_DDF_SUCCESS;
    }
    
    if (dd_ptr->prx.enable) 
    {
      i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_PRX_ENABLE|APDS9950_DD_ALS_ENABLE|
                  APDS9950_DD_PRX_INT_ENABLE|APDS9950_DD_WAIT_ENABLE);        
	
	  // check if ALS interrupt is needed in case RGB/CT are in DRI
	  if(dd_ptr->sub_dev[1].dri_enabled && (dd_ptr->rgb.enable || dd_ptr->cct.enable))
	  {
		i2c_data |= APDS9950_DD_ALS_INT_ENABLE;
	  }
	}
    else if (dd_ptr->rgb.enable || dd_ptr->cct.enable)
    {
      if (!dd_ptr->sub_dev[1].dri_enabled)
      {
        // Polling
        i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE);
      }
      else
      {
        // DRI
        i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|
                    APDS9950_DD_ALS_INT_ENABLE|APDS9950_DD_WAIT_ENABLE);
      } 
    }

    dd_ptr->als.enable = FALSE;
    dd_ptr->common.als_req_odr = 0;
    dd_ptr->common.als_odr = 0;
  }
  else if ( SNS_DDF_SENSOR_RGB == sensor ) 
  {
    if (!dd_ptr->rgb.enable) 
    {
      return SNS_DDF_SUCCESS;
    }
    
    if (!dd_ptr->prx.enable && !dd_ptr->als.enable && !dd_ptr->cct.enable) 
    {
      i2c_data = APDS9950_DD_PWR_ON;        
    }
    else if (!dd_ptr->cct.enable && (dd_ptr->prx.enable || dd_ptr->als.enable))
    {
      if (!dd_ptr->sub_dev[0].dri_enabled)
      {
        // Polling
        if (!dd_ptr->prx.enable && dd_ptr->als.enable)
        {
          i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE);
        }
        else
        {
          i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_PRX_ENABLE|APDS9950_DD_ALS_ENABLE);          
        }
      }
      else
      {
        // DRI
        if (!dd_ptr->prx.enable && dd_ptr->als.enable)
        {
          i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|
                      APDS9950_DD_ALS_INT_ENABLE|APDS9950_DD_WAIT_ENABLE);          
        }
        else if (dd_ptr->prx.enable && !dd_ptr->als.enable)
        {
          i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|APDS9950_DD_PRX_ENABLE|
                      APDS9950_DD_PRX_INT_ENABLE|APDS9950_DD_WAIT_ENABLE);
        }
        else if (dd_ptr->prx.enable && dd_ptr->als.enable)
        {
          i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|APDS9950_DD_PRX_ENABLE|
                      APDS9950_DD_ALS_INT_ENABLE|APDS9950_DD_PRX_INT_ENABLE|APDS9950_DD_WAIT_ENABLE);          
        }
      } 
    }
    else if (dd_ptr->cct.enable)
    {
        i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|
                    APDS9950_DD_ALS_INT_ENABLE|APDS9950_DD_WAIT_ENABLE);      
    }

    dd_ptr->rgb.enable = FALSE;
    dd_ptr->common.rgb_req_odr = 0;
    dd_ptr->common.rgb_odr = 0;
  }
  else if ( SNS_DDF_SENSOR_CT_C == sensor ) 
  {
    if (!dd_ptr->cct.enable) 
    {
      return SNS_DDF_SUCCESS;
    }
    
    if (!dd_ptr->prx.enable && !dd_ptr->als.enable && !dd_ptr->rgb.enable) 
    {
      i2c_data = APDS9950_DD_PWR_ON;        
    }
    else if (!dd_ptr->rgb.enable && (dd_ptr->prx.enable || dd_ptr->als.enable))
    {
      if (!dd_ptr->sub_dev[0].dri_enabled)
      {
        // Polling
        if (!dd_ptr->prx.enable && dd_ptr->als.enable)
        {
          i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE);
        }
        else
        {
          i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_PRX_ENABLE|APDS9950_DD_ALS_ENABLE);          
        }
      }
      else
      {
        // DRI
        if (!dd_ptr->prx.enable && dd_ptr->als.enable)
        {
          i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|
                      APDS9950_DD_ALS_INT_ENABLE|APDS9950_DD_WAIT_ENABLE);          
        }
        else if (dd_ptr->prx.enable && !dd_ptr->als.enable)
        {
          i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|APDS9950_DD_ALS_ENABLE|
                      APDS9950_DD_PRX_INT_ENABLE|APDS9950_DD_WAIT_ENABLE);          
        }
        else
        {
          i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|APDS9950_DD_ALS_ENABLE|
                      APDS9950_DD_ALS_INT_ENABLE|APDS9950_DD_PRX_INT_ENABLE|APDS9950_DD_WAIT_ENABLE);
        }
      } 
    }
    else if (dd_ptr->rgb.enable)
    {
        i2c_data = (APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|
                    APDS9950_DD_ALS_INT_ENABLE|APDS9950_DD_WAIT_ENABLE);      
    }

    dd_ptr->cct.enable = FALSE;
    dd_ptr->common.cct_req_odr = 0;
    dd_ptr->common.cct_odr = 0;
  }

  /* Update Integration times */
  if (sub_dev_idx == 0)
  {
    if (!dd_ptr->prx.enable && !dd_ptr->als.enable)
    {
      if (dd_ptr->sub_dev[1].dri_enabled && (!dd_ptr->rgb.enable && !dd_ptr->cct.enable))
      {
        /* Clear ENABLE bits*/
        i2c_data = APDS9950_DD_PWR_ON;
        status = sns_ddf_write_port(dd_ptr->port_handle,
                                    APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                                    &i2c_data, 1, &bytes_w);
        CHECK_STATUS(status);
        
        /* Clear interrupts */
        status = apds9950_dd_clear_int_reg(dd_ptr, APDS9950_DD_CMD_CLR_PS_ALS_INT);
        CHECK_STATUS(status);
    
        if (dd_ptr->sub_dev[0].dri_registration)
        {
          /* De-register INT notification */
          status = sns_ddf_signal_deregister(dd_ptr->sub_dev[0].interrupt_gpio);
          CHECK_STATUS(status);
        }
        else if (dd_ptr->sub_dev[1].dri_registration)
        {
          /* De-register INT notification */
          status = sns_ddf_signal_deregister(dd_ptr->sub_dev[1].interrupt_gpio);
          CHECK_STATUS(status);          
        }
      }
    }
    else if (dd_ptr->als.enable || dd_ptr->prx.enable)
    {
      /* ENABLE=PON */
      status = sns_ddf_write_port(dd_ptr->port_handle,
                                  APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                                  (uint8_t*)&i2c_pon_data, 1, &bytes_w);
      CHECK_STATUS(status);
  
      /* Clear interrupts */
      status = apds9950_dd_clear_int_reg(dd_ptr, APDS9950_DD_CMD_CLR_PS_ALS_INT);
      CHECK_STATUS(status);
  
      status = apds9950_dd_update_intg_time(dd_ptr, dd_ptr->prx.enable, 
                                            (dd_ptr->als.enable|dd_ptr->rgb.enable|dd_ptr->cct.enable), FALSE);
      CHECK_STATUS(status);
  
      /* Update ENABLE bits*/
      status = sns_ddf_write_port(dd_ptr->port_handle,
                                  APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                                  &i2c_data, 1, &bytes_w);
      CHECK_STATUS(status);      
    }
  }
  else
  {
    if (!dd_ptr->rgb.enable && !dd_ptr->cct.enable)
    {
      if (dd_ptr->sub_dev[0].dri_enabled && (!dd_ptr->prx.enable && !dd_ptr->als.enable))
      {
        /* Clear ENABLE bits*/
        i2c_data = APDS9950_DD_PWR_ON;
        status = sns_ddf_write_port(dd_ptr->port_handle,
                                    APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                                    &i2c_data, 1, &bytes_w);
        CHECK_STATUS(status);
        
        /* Clear interrupts */
        status = apds9950_dd_clear_int_reg(dd_ptr, APDS9950_DD_CMD_CLR_PS_ALS_INT);
        CHECK_STATUS(status);
    
        if (dd_ptr->sub_dev[0].dri_registration)
        {
          /* De-register INT notification */
          status = sns_ddf_signal_deregister(dd_ptr->sub_dev[0].interrupt_gpio);
          CHECK_STATUS(status);
        }
        else if (dd_ptr->sub_dev[1].dri_registration)
        {
          /* De-register INT notification */
          status = sns_ddf_signal_deregister(dd_ptr->sub_dev[1].interrupt_gpio);
          CHECK_STATUS(status);          
        }
      }
    }
    else if (dd_ptr->rgb.enable || dd_ptr->cct.enable)
    {
      /* ENABLE=PON */
      status = sns_ddf_write_port(dd_ptr->port_handle,
                                  APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                                  (uint8_t*)&i2c_pon_data, 1, &bytes_w);
      CHECK_STATUS(status);
  
      /* Clear interrupts */
      status = apds9950_dd_clear_int_reg(dd_ptr, APDS9950_DD_CMD_CLR_PS_ALS_INT);
      CHECK_STATUS(status);
  
      status = apds9950_dd_update_intg_time(dd_ptr, dd_ptr->prx.enable, 
                                            (dd_ptr->als.enable|dd_ptr->rgb.enable|dd_ptr->cct.enable), FALSE);
      CHECK_STATUS(status);
  
      /* Update ENABLE bits*/
      status = sns_ddf_write_port(dd_ptr->port_handle,
                                  APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                                  &i2c_data, 1, &bytes_w);
      CHECK_STATUS(status);      
    }    
  }

  ALSPRX_MSG_1(ERROR, "DISABLE INTERRUPT END    status=%d", status);
  return status;
}

/*===========================================================================

FUNCTION      sns_dd_apds9950_enable_sched_data
  
DESCRIPTION   

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
sns_ddf_status_e sns_dd_apds9950_enable_sched_data
(
  sns_ddf_handle_t dd_handle,
  sns_ddf_sensor_e sensor,
  bool enable
)
{
  ALSPRX_MSG_2(ERROR, "sched - sensor=%d enable=%d", sensor, enable);

  if(enable) 
  {
    return apds9950_dd_enable_interrupt(dd_handle, sensor);
  }
  else 
  {
    return apds9950_dd_disable_interrupt(dd_handle, sensor);
  }
}

/*===========================================================================

FUNCTION      apds9950_dd_set_als_change_pcnt
  
DESCRIPTION   Sets the ALS change [ercentage

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None 

===========================================================================*/
void apds9950_dd_set_als_change_pcnt
(
  apds9950_dd_dev_s* dd_ptr,
  uint32_t als_change_pcnt
)
{
  if ((als_change_pcnt <= APDS9950_DD_ALS_CHANGE_MIN_PCNT) ||
      (als_change_pcnt >= APDS9950_DD_ALS_CHANGE_MAX_PCNT)) 
  {
    dd_ptr->common.nv_db.als_change_pcnt = APDS9950_DD_DEFAULT_ALS_CHANGE_PCNT;
  }
  else 
  {
    dd_ptr->common.nv_db.als_change_pcnt = als_change_pcnt;
  }
}

/*===========================================================================

FUNCTION      apds9950_dd_set_prx_factor

DESCRIPTION   Sets the Proximity calibration factor

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e apds9950_dd_set_prx_factor
(
  apds9950_dd_dev_s* dd_ptr,
  sns_ddf_sensor_e sensor_type,
  uint16_t prx_factor
)
{
  sns_ddf_status_e status=SNS_DDF_SUCCESS;

  if (sensor_type == SNS_DDF_SENSOR_PROXIMITY)
  {
    ALSPRX_MSG_2(LOW, "set_prx_factor - sensor=%d factor=%d", sensor_type, prx_factor);

    dd_ptr->common.nv_db.prx_factor = prx_factor;
    dd_ptr->common.nv_db.thresh_near = dd_ptr->common.nv_db.prx_factor + APDS9950_DD_PRX_FACTOR;
    dd_ptr->common.nv_db.thresh_far = dd_ptr->common.nv_db.thresh_near - 100;

    dd_ptr->prx.thresh_near = dd_ptr->common.nv_db.thresh_near;
    dd_ptr->prx.thresh_far = dd_ptr->common.nv_db.thresh_far;
  }
	
  return status;
}

/*===========================================================================

FUNCTION      apds9950_dd_start_general_timer
  
DESCRIPTION   Starts a timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
void apds9950_dd_start_general_timer 
(
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx,
  uint32_t sensor,
  uint32_t duration
)
{
  sns_ddf_status_e status;

  ALSPRX_MSG_2(LOW, "apds9950_dd_start_general_timer - sensor=%d, duration=%d ", sensor, duration);

  if (sensor == SNS_DDF_SENSOR_AMBIENT) 
  {
    if ( dd_ptr->als.timer_active ) 
    {
        ALSPRX_MSG_0(MED, "AMBIENT timer already active");
        return;
    }
  
    status = sns_ddf_timer_start(dd_ptr->als.timer, duration);
    
    if ( status != SNS_DDF_SUCCESS ) 
    {
      ALSPRX_MSG_0(ERROR, "apds9950_dd_start_general_timer - timer start failed ");
      return;
    }

    dd_ptr->als.timer_active = TRUE;
  }
  else if (sensor == SNS_DDF_SENSOR_RGB)
  {
    if ( dd_ptr->rgb.timer_active ) 
    {
        ALSPRX_MSG_0(MED, "RGB timer already active");             
        return;
    }
  
    status = sns_ddf_timer_start(dd_ptr->rgb.timer, duration);
    
    if ( status != SNS_DDF_SUCCESS ) 
    {
      ALSPRX_MSG_0(ERROR, "apds9950_dd_start_general_timer - timer start failed ");
      return;
    }

    dd_ptr->rgb.timer_active = TRUE;    
  }
  else if (sensor == SNS_DDF_SENSOR_CT_C)
  {
    if ( dd_ptr->cct.timer_active ) 
    {
        ALSPRX_MSG_0(MED, "CT_C timer already active");             
        return;
    }
  
    status = sns_ddf_timer_start(dd_ptr->cct.timer, duration);
    
    if ( status != SNS_DDF_SUCCESS ) 
    {
      ALSPRX_MSG_0(ERROR, "apds9950_dd_start_general_timer - timer start failed ");
      return;
    }

    dd_ptr->cct.timer_active = TRUE;
  }
  else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
  {
    if ( dd_ptr->prx.timer_active ) 
    {
      ALSPRX_MSG_0(MED, "PROXIMITY timer already active");             
      return;
    }

    status = sns_ddf_timer_start(dd_ptr->prx.timer, duration);
    if ( status != SNS_DDF_SUCCESS ) 
    {
      return;
    }
  
    dd_ptr->prx.timer_active = TRUE;        
  }
}

/*===========================================================================

FUNCTION      apds9950_dd_stop_general_timer
  
DESCRIPTION   Stops a timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
void apds9950_dd_stop_general_timer
( 
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx,
  uint32_t sensor 
)
{
  if (sensor == SNS_DDF_SENSOR_AMBIENT) 
  {        
    if ( TRUE == dd_ptr->als.timer_active ) 
    {
      sns_ddf_timer_cancel(dd_ptr->als.timer);
      dd_ptr->als.timer_active = FALSE;
    }    
  }
  else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
  {
    if ( TRUE == dd_ptr->prx.timer_active ) 
    {
      sns_ddf_timer_cancel(dd_ptr->prx.timer);
      dd_ptr->prx.timer_active = FALSE;
    }
  }
  else if (sensor == SNS_DDF_SENSOR_RGB)
  {
    if ( TRUE == dd_ptr->rgb.timer_active ) 
    {
      sns_ddf_timer_cancel(dd_ptr->rgb.timer);
      dd_ptr->rgb.timer_active = FALSE;
    }    
  }
  else if (sensor == SNS_DDF_SENSOR_CT_C)
  {
    if ( TRUE == dd_ptr->cct.timer_active ) 
    {
      sns_ddf_timer_cancel(dd_ptr->cct.timer);
      dd_ptr->cct.timer_active = FALSE;
    }        
  }
}

/*===========================================================================

FUNCTION      apds9950_dd_req_data
  
DESCRIPTION   A function that handles data request from sub-drivers

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
sns_ddf_status_e apds9950_dd_req_data
(
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx,
  uint32_t sensor
)
{
  sns_ddf_status_e status=SNS_DDF_SUCCESS;
  ALSPRX_MSG_1(ERROR, "apds9950_dd_req_data - sensor=%d", sensor);
  
  if ( (sensor == SNS_DDF_SENSOR_AMBIENT) || (sensor == SNS_DDF_SENSOR_RGB) ||
       (sensor == SNS_DDF_SENSOR_CT_C) || (sensor == SNS_DDF_SENSOR_PROXIMITY) ) 
  {
    apds9950_dd_start_general_timer(dd_ptr, sub_dev_idx, sensor, 
                                sns_dd_als_integration_time_tb[dd_ptr->als.als_res_index]);
  }
  else 
  {
    status = SNS_DDF_EFAIL;
  }

  return status;
}

/*===========================================================================

FUNCTION      apds9950_convert_to_irradiance
  
DESCRIPTION   Convert adc raw data to irradiance value

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
void apds9950_convert_to_irradiance 
(
  apds9950_dd_dev_s* dd_ptr,
  sns_ddf_sensor_e sensor_type,
  uint16_t cdata,
  uint16_t rdata,
  uint16_t gdata,
  uint16_t bdata,
  bool calibration
)
{
  uint32_t irradiance_c=0;
  uint32_t irradiance_r=0;
  uint32_t irradiance_g=0;
  uint32_t irradiance_b=0;
  uint32_t normalized_factor;

  normalized_factor = (sns_dd_als_atime_us_tb[dd_ptr->als.als_res_index]*sns_dd_als_again_tb[dd_ptr->als.als_gain_index])/1000;

  if (cdata > 0) {
    if( ((rdata*100) /cdata) > 65 ) {  // Incandescent 2600K
      irradiance_c=(cdata*APDS9950_DD_IRRADIANCE_C_INCAND)/(normalized_factor);
      irradiance_r=(rdata*APDS9950_DD_IRRADIANCE_R_INCAND)/(normalized_factor);
      irradiance_g=(gdata*APDS9950_DD_IRRADIANCE_G_INCAND)/(normalized_factor);
      irradiance_b=(bdata*APDS9950_DD_IRRADIANCE_B_INCAND)/(normalized_factor);
    }
    else {  // Fluorescent Daylight 6500K
      irradiance_c=(cdata*APDS9950_DD_IRRADIANCE_C_NON_INCAND)/(normalized_factor);
      irradiance_r=(rdata*APDS9950_DD_IRRADIANCE_R_NON_INCAND)/(normalized_factor);
      irradiance_g=(gdata*APDS9950_DD_IRRADIANCE_G_NON_INCAND)/(normalized_factor);
      irradiance_b=(bdata*APDS9950_DD_IRRADIANCE_B_NON_INCAND)/(normalized_factor);
    }
  }
  
  if (calibration)
  {
      dd_ptr->als.cdata_irradiance = irradiance_c;
      dd_ptr->cct.cdata_irradiance = irradiance_c;
	  dd_ptr->rgb.rdata_irradiance = irradiance_r;
	  dd_ptr->rgb.gdata_irradiance = irradiance_g;
	  dd_ptr->rgb.bdata_irradiance = irradiance_b;
	
	  return;
  }

  if (sensor_type == SNS_DDF_SENSOR_AMBIENT)
  {
    dd_ptr->als.cdata_irradiance = (irradiance_c*dd_ptr->common.nv_db.rgb_c_factor)/100;
  }
  else if (sensor_type == SNS_DDF_SENSOR_RGB)
  {
    dd_ptr->rgb.rdata_irradiance = (irradiance_r*dd_ptr->common.nv_db.rgb_r_factor)/100;
    dd_ptr->rgb.gdata_irradiance = (irradiance_g*dd_ptr->common.nv_db.rgb_g_factor)/100;
    dd_ptr->rgb.bdata_irradiance = (irradiance_b*dd_ptr->common.nv_db.rgb_b_factor)/100;
  }
  else if (sensor_type == SNS_DDF_SENSOR_CT_C)
  {
    dd_ptr->cct.cdata_irradiance = (irradiance_c*dd_ptr->common.nv_db.rgb_c_factor)/100;;
  } 
}

/*===========================================================================

FUNCTION      apds9950_dd_rgb_convert_to_mlux
  
DESCRIPTION   Convert a raw data to a real milli lux

DEPENDENCIES  None

RETURN VALUE  milli lux value or 0 if there was a error

SIDE EFFECT   None 

===========================================================================*/
uint32_t apds9950_dd_rgb_convert_to_mlux 
(
  apds9950_dd_dev_s* dd_ptr,
  uint16_t cdata,
  uint16_t rdata,
  uint16_t gdata,
  uint16_t bdata,
  bool calibration
)
{
  uint32_t lux;
 
  ALSPRX_MSG_3(ERROR, "cdata %d atime %d again %d\r\n", 
               cdata,
               sns_dd_als_atime_us_tb[dd_ptr->als.als_res_index],
               sns_dd_als_again_tb[dd_ptr->als.als_gain_index]);    

  lux = ((cdata*100800)/(sns_dd_als_atime_us_tb[dd_ptr->als.als_res_index]*sns_dd_als_again_tb[dd_ptr->als.als_gain_index]));
  lux = lux * 1000; // convert to mlux

  ALSPRX_MSG_1(MED, "Lux=%d\r\n", (uint32_t)lux);      
  
  if (cdata > 0) 
  {
    if( ((rdata*100) /cdata) > 65 ) 
    {  // Incandescent 2600K
      lux=(lux*APDS9950_DD_LUX_GA2)/1000;
    }
    else 
    {  // Fluorescent Daylight 6500K
      lux=(lux*APDS9950_DD_LUX_GA1)/1000;
    }
  }

  if (calibration)
  {
  	ALSPRX_MSG_1(MED, "Lux=%d\r\n", (uint32_t)lux);
  	return ((uint32_t)lux);
  }

  // for ALS calibration
  lux = (lux * dd_ptr->common.nv_db.als_factor)/100;

  ALSPRX_MSG_1(MED, "Lux=%d\r\n", (uint32_t)lux);      
  
  return ((uint32_t)lux);
}

/*===========================================================================

FUNCTION      apds9950_dd_rgb_convert_to_cct
  
DESCRIPTION   Convert a raw data to a real cct (correlated color temperature)

DEPENDENCIES  None

RETURN VALUE  milli lux value or 0 if there was a error

SIDE EFFECT   None 

===========================================================================*/
uint32_t apds9950_dd_rgb_convert_to_cct 
(
  apds9950_dd_dev_s* dd_ptr,
  uint16_t cdata,
  uint16_t rdata,
  uint16_t gdata,
  uint16_t bdata
)
{
  int64_t X1, Y1, Z1;
  int64_t x1, y1;
  int64_t n;
  int64_t cct;
  int64_t sum_XYZ=0;

  X1 = (RGB_COE_X[0]*rdata) + (RGB_COE_X[1]*gdata) + (RGB_COE_X[2]*bdata);
  Y1 = (RGB_COE_Y[0]*rdata) + (RGB_COE_Y[1]*gdata) + (RGB_COE_Y[2]*bdata);
  Z1 = (RGB_COE_Z[0]*rdata) + (RGB_COE_Z[1]*gdata) + (RGB_COE_Z[2]*bdata);
 
  if ( (X1==0) && (Y1==0) && (Z1==0) ) 
  {
    x1 = y1 = 0;
  }
  else 
  {
    sum_XYZ = (X1 + Y1 + Z1);	

    x1 = (X1*1000)/sum_XYZ;
    y1 = (Y1*1000)/sum_XYZ;
  }
 
  if (cdata > 10) 
  {
    n = ((x1 - RGB_CIE_N1)*1000)/(RGB_CIE_N2 - y1);
    cct = (((RGB_CIE_CCT1*(n*n*n))/1000000000) + ((RGB_CIE_CCT2*(n*n))/1000000) + ((RGB_CIE_CCT3*n)/1000) + RGB_CIE_CCT4);
  }
  else 
  {
    n = 0;
    cct = 0;
  }

  ALSPRX_MSG_2(MED, "again %d atime %d\r\n", 
                   sns_dd_als_again_tb[dd_ptr->als.als_gain_index], 
                   sns_dd_als_atime_us_tb[dd_ptr->als.als_res_index]);    
    
  ALSPRX_MSG_1(MED, "CCT=%d\r\n", (uint32_t)cct);      
  
  if (cdata > 0) 
  {
    if( ((rdata*100) /cdata) > 65 ) 
    {  // Incandescent 2600K
      cct=(cct*APDS9950_DD_CCT_GA2)/1000;
    }
    else 
    {  // Fluorescent Daylight 6500K
      cct=(cct*APDS9950_DD_CCT_GA1)/1000;
    }
  }

  return ((uint32_t)cct);
}

/*===========================================================================

FUNCTION      apds9950_dd_async_notify
  
DESCRIPTION   Notify to SNSM with a sensor data.

DEPENDENCIES  None

RETURN VALUE  None 

SIDE EFFECT   None 

===========================================================================*/
void apds9950_dd_async_notify 
(
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx,
  sns_ddf_sensor_e sensor
)
{
  uint8_t num_sensors = 1, num_samples=2;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint32_t data_lux;
  uint32_t data_cct;

  ALSPRX_MSG_2(ERROR, "async_notify - %d sensor=%d", 700, sensor);

  if ( sensor == SNS_DDF_SENSOR_AMBIENT ) 
  {
    /* convert to lux */ 
    data_lux = (uint32_t) (dd_ptr->als.data_mlux / 1000);

    /* saturate */
    data_lux = (data_lux > 10000) ? 10000 : data_lux;
    
    /* related to floating point library removal; change to FX_CONV_Q16(val,0) */
    dd_ptr->als.sensor_sample[0].sample = FX_CONV_Q16(data_lux, 0);  /* Lux Output */
    dd_ptr->als.sensor_sample[0].status = SNS_DDF_SUCCESS;
    
    dd_ptr->als.sensor_sample[1].sample = dd_ptr->als.cdata;  /* ADC output - cdata */
    dd_ptr->als.sensor_sample[1].status = SNS_DDF_SUCCESS;

    sns_dd_apds9950_log(dd_ptr, sensor, dd_ptr->als.data_mlux, dd_ptr->als.sensor_sample[0].sample, dd_ptr->als.cdata, 0, 0);

    ALSPRX_MSG_2(MED, "async_notify - als data Lux=%d cdata=%x\r\n", data_lux, dd_ptr->als.cdata); 
    
    dd_ptr->common.sensor_data[0].status = status;  
    dd_ptr->common.sensor_data[0].sensor = sensor;
    dd_ptr->common.sensor_data[0].num_samples = num_samples;
    
    if (dd_ptr->sub_dev[sub_dev_idx].dri_enabled) 
    {
      dd_ptr->common.sensor_data[0].timestamp = dd_ptr->int_timestamp;
    }
    else 
    {
      dd_ptr->common.sensor_data[0].timestamp = sns_ddf_get_timestamp();
    }
    
    dd_ptr->common.sensor_data[0].samples = dd_ptr->als.sensor_sample;
  
    sns_ddf_smgr_notify_data(dd_ptr->sub_dev[0].smgr_handle, dd_ptr->common.sensor_data, num_sensors);    
  }
  else if (sensor == SNS_DDF_SENSOR_PROXIMITY) 
  {
    dd_ptr->prx.sensor_sample[0].sample = FX_CONV_Q16(dd_ptr->prx.detection_state, 0);  /* proximity binary */
    dd_ptr->prx.sensor_sample[0].status = SNS_DDF_SUCCESS;
    
    dd_ptr->prx.sensor_sample[1].sample = dd_ptr->prx.pdata;  /* ADC output - pdata */
    dd_ptr->prx.sensor_sample[1].status = SNS_DDF_SUCCESS;

    sns_dd_apds9950_log(dd_ptr, sensor, dd_ptr->prx.detection_state, dd_ptr->prx.sensor_sample[0].sample, 0, 0, dd_ptr->prx.pdata);

    ALSPRX_MSG_2(MED, "prx data %d %d\r\n", dd_ptr->prx.detection_state, dd_ptr->prx.pdata);    

    dd_ptr->common.sensor_data[0].status = status;  
    dd_ptr->common.sensor_data[0].sensor = sensor;
    dd_ptr->common.sensor_data[0].num_samples = num_samples;
    
    if (dd_ptr->sub_dev[sub_dev_idx].dri_enabled) 
    {
      dd_ptr->common.sensor_data[0].timestamp = dd_ptr->int_timestamp;
    }
    else 
    {
      dd_ptr->common.sensor_data[0].timestamp = sns_ddf_get_timestamp();
    }
    
    dd_ptr->common.sensor_data[0].samples = dd_ptr->prx.sensor_sample;
  
    sns_ddf_smgr_notify_data(dd_ptr->sub_dev[0].smgr_handle, dd_ptr->common.sensor_data, num_sensors);
  }
  else if (sensor == SNS_DDF_SENSOR_RGB) 
  {
    /* convert adc raw data to irradiance value */
    apds9950_convert_to_irradiance(dd_ptr, sensor, dd_ptr->rgb.cdata, dd_ptr->rgb.rdata, dd_ptr->rgb.gdata, dd_ptr->rgb.bdata, FALSE);

    ALSPRX_MSG_3(HIGH, "R_raw=%d G_raw=%d B_raw=%d", dd_ptr->rgb.rdata, dd_ptr->rgb.gdata, dd_ptr->rgb.bdata);
    ALSPRX_MSG_3(HIGH, "R_rad=%d G_rad=%d B_rad=%d", dd_ptr->rgb.rdata_irradiance, dd_ptr->rgb.gdata_irradiance, dd_ptr->rgb.bdata_irradiance);

    // RData
    if (dd_ptr->rgb.rdata_irradiance > 65535)
    {
      dd_ptr->rgb.sensor_sample[0].sample = FX_CONV_Q16(dd_ptr->rgb.rdata_irradiance/1000, 0);  /* ADC output - was dd_ptr->rgb.rdata */
    }
    else
    {
      dd_ptr->rgb.sensor_sample[0].sample = FX_CONV_Q16(dd_ptr->rgb.rdata_irradiance, 0)/1000;  /* ADC output - was dd_ptr->rgb.rdata */
    }
    dd_ptr->rgb.sensor_sample[0].status = SNS_DDF_SUCCESS;

    // GData
    if (dd_ptr->rgb.gdata_irradiance > 65535)
    {
      dd_ptr->rgb.sensor_sample[1].sample = FX_CONV_Q16(dd_ptr->rgb.gdata_irradiance/1000, 0);  /* ADC output - was dd_ptr->rgb.gdata */
    }
    else
    {
      dd_ptr->rgb.sensor_sample[1].sample = FX_CONV_Q16(dd_ptr->rgb.gdata_irradiance, 0)/1000;  /* ADC output - was dd_ptr->rgb.gdata */
    }
    dd_ptr->rgb.sensor_sample[1].status = SNS_DDF_SUCCESS;

    // BData
    if (dd_ptr->rgb.bdata_irradiance > 65535)
    {
      dd_ptr->rgb.sensor_sample[2].sample = FX_CONV_Q16(dd_ptr->rgb.bdata_irradiance/1000, 0);  /* ADC output - was dd_ptr->rgb.bdata */
    }
    else
    {
      dd_ptr->rgb.sensor_sample[2].sample = FX_CONV_Q16(dd_ptr->rgb.bdata_irradiance, 0)/1000;  /* ADC output - was dd_ptr->rgb.bdata */
    }
    dd_ptr->rgb.sensor_sample[2].status = SNS_DDF_SUCCESS;

    ALSPRX_MSG_3(HIGH, "R_sam=%d G_sam=%d B_sam=%d", dd_ptr->rgb.sensor_sample[0].sample, dd_ptr->rgb.sensor_sample[1].sample, dd_ptr->rgb.sensor_sample[2].sample);

    num_samples = 3;  /* special case */
    
    sns_dd_apds9950_log(dd_ptr, sensor, dd_ptr->rgb.sensor_sample[0].sample, dd_ptr->rgb.sensor_sample[1].sample, dd_ptr->rgb.sensor_sample[2].sample, 0, 0);
    
    dd_ptr->common.sensor_data[0].status = status;  
    dd_ptr->common.sensor_data[0].sensor = sensor;
    dd_ptr->common.sensor_data[0].num_samples = num_samples;

    dd_ptr->common.sensor_data[0].timestamp = sns_ddf_get_timestamp();
    
    dd_ptr->common.sensor_data[0].samples = dd_ptr->rgb.sensor_sample;
  
    sns_ddf_smgr_notify_data(dd_ptr->sub_dev[sub_dev_idx].smgr_handle, dd_ptr->common.sensor_data, num_sensors);
  }
  else if (sensor == SNS_DDF_SENSOR_CT_C) 
  {
    data_cct = (dd_ptr->cct.data_cct > 10000) ? 10000 : dd_ptr->cct.data_cct;
    
    dd_ptr->cct.sensor_sample[0].sample = FX_CONV_Q16(data_cct, 0);  /* CCT Output */
    dd_ptr->cct.sensor_sample[0].status = SNS_DDF_SUCCESS;

    /* convert adc raw data to irradiance value */
    apds9950_convert_to_irradiance(dd_ptr, sensor, dd_ptr->cct.cdata, dd_ptr->cct.rdata, dd_ptr->cct.gdata, dd_ptr->cct.bdata, FALSE);  

    if (dd_ptr->cct.cdata_irradiance > 65535)
    {
      dd_ptr->cct.sensor_sample[1].sample = FX_CONV_Q16(dd_ptr->cct.cdata_irradiance/1000, 0);  /* ADC output - was dd_ptr->cct.cdata */
    }
    else
    {
      dd_ptr->cct.sensor_sample[1].sample = FX_CONV_Q16(dd_ptr->cct.cdata_irradiance, 0)/1000;  /* ADC output - was dd_ptr->als.cdata */
    }    
    dd_ptr->cct.sensor_sample[1].status = SNS_DDF_SUCCESS;

    sns_dd_apds9950_log(dd_ptr, sensor, dd_ptr->cct.sensor_sample[0].sample, dd_ptr->cct.sensor_sample[1].sample, 0, 0, 0);

    ALSPRX_MSG_2(MED, "async_notify - CCT=%d cdata=%x\r\n", data_cct, dd_ptr->cct.cdata);   
   
    dd_ptr->common.sensor_data[0].status = status;  
    dd_ptr->common.sensor_data[0].sensor = sensor;
    dd_ptr->common.sensor_data[0].num_samples = num_samples;
    
    dd_ptr->common.sensor_data[0].timestamp = sns_ddf_get_timestamp();

    dd_ptr->common.sensor_data[0].samples = dd_ptr->cct.sensor_sample;
  
    sns_ddf_smgr_notify_data(dd_ptr->sub_dev[sub_dev_idx].smgr_handle, dd_ptr->common.sensor_data, num_sensors);    
  }
}

/*===========================================================================

FUNCTION      apds9950_dd_als_rsp
  
DESCRIPTION   This function is called by the arbitration manager when ALS(light) data
              is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
void apds9950_dd_als_rsp 
(
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx,
  sns_ddf_sensor_e sensor
)
{
  uint16_t ailt;
  uint16_t aiht;
  uint16_t diff;

  if (dd_ptr->sub_dev[sub_dev_idx].dri_enabled) 
  {
    if ( sensor == SNS_DDF_SENSOR_AMBIENT )
    {
      diff = PCNT_OF_RAW(dd_ptr->common.nv_db.als_change_pcnt, 
                         dd_ptr->als.cdata);
      ailt = (dd_ptr->als.cdata - diff);
      aiht = (dd_ptr->als.cdata + diff);
      apds9950_dd_set_als_thresh(dd_ptr, ailt, aiht);
      ALSPRX_MSG_3(HIGH, "als_rsp - raw_data=%d ailt=%d aiht=%d", 
                   dd_ptr->als.cdata, ailt, aiht);
    }
  }

  if (sensor == SNS_DDF_SENSOR_AMBIENT) 
  {
    apds9950_dd_async_notify(dd_ptr, sub_dev_idx, sensor);
    dd_ptr->als.last_mlux  = dd_ptr->als.data_mlux;
  }
}

/*===========================================================================

FUNCTION      apds9950_dd_cct_raw_data_ready
  
DESCRIPTION   This function is called when the ALS raw data is ready

DEPENDENCIES  None

RETURN VALUE  int32_t	0 success, 1 pending, -1 error

SIDE EFFECT   None 

===========================================================================*/
bool apds9950_dd_cct_raw_data_ready 
(
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx,
  sns_ddf_sensor_e sensor,
  uint16_t cdata,
  uint16_t rdata,
  uint16_t gdata,
  uint16_t bdata
)
{
  ALSPRX_MSG_1(ERROR, "apds9950_dd_cct_raw_data_ready sensor=%d\r\n", sensor);

  /* convert reading to cct */
  if (sensor == SNS_DDF_SENSOR_CT_C) 
  {      
    dd_ptr->cct.data_cct = apds9950_dd_rgb_convert_to_cct(dd_ptr, 
                                                          cdata,
                                                          rdata,
                                                          gdata,
                                                          bdata);
    ALSPRX_MSG_1(MED, "data_cct=%d\r\n", dd_ptr->cct.data_cct);
  }

  return TRUE;
}

/*===========================================================================

FUNCTION      apds9950_dd_als_raw_data_ready
  
DESCRIPTION   This function is called when the ALS raw data is ready

DEPENDENCIES  None

RETURN VALUE  int32_t	0 success, 1 pending, -1 error

SIDE EFFECT   None 

===========================================================================*/
bool apds9950_dd_als_raw_data_ready 
(
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx,
  sns_ddf_sensor_e sensor,
  uint16_t cdata,
  uint16_t rdata,
  uint16_t gdata,
  uint16_t bdata
)
{
  uint32_t adc_saturation_data;
  uint32_t adc_low_data;
  bool change_gain=FALSE;
  uint8_t i2c_data, i2c_read_data;
  uint8_t bytes_w, bytes_rw;

  ALSPRX_MSG_1(ERROR, "apds9950_dd_als_raw_data_ready sensor=%d\r\n", sensor);
  
  if (sensor == SNS_DDF_SENSOR_AMBIENT)  // polling mode for ALS/PRX 
  {
    /* convert reading to mlux */
    dd_ptr->als.data_mlux = apds9950_dd_rgb_convert_to_mlux(dd_ptr, 
                                                            cdata,
                                                            rdata,
                                                            gdata,
                                                            bdata,
															FALSE);
    ALSPRX_MSG_1(MED, "data_mlux=%d\r\n", dd_ptr->als.data_mlux);
  }

  /********************************************************
   * check lux value to see if a different range is needed
   *********************************************************/
    
  adc_saturation_data = sns_dd_als_res_tb[dd_ptr->als.als_res_index] * 99;
  adc_saturation_data = adc_saturation_data /100;

  adc_low_data = sns_dd_als_res_tb[dd_ptr->als.als_res_index] * 1;
  adc_low_data = adc_low_data /100;
 
  ALSPRX_MSG_3(HIGH, "apds9950_dd_als_raw_data_ready cdata=%d, adc_saturation_data=%d, adc_low_data=%d", cdata, adc_saturation_data, adc_low_data);

  if ( cdata >= adc_saturation_data ) 
  {
    /* lower gain if possible */
    if (dd_ptr->als.als_gain_index != APDS9950_DD_ALS_GAIN_1X) 
    {
      dd_ptr->als.als_gain_index--;
      change_gain = TRUE;

      ALSPRX_MSG_1(MED, "gain decrease to %d\r\n", sns_dd_als_again_tb[dd_ptr->als.als_gain_index]);      
    }
  }
  else if (cdata < adc_low_data) 
  {
    /* increase gain if possible */
    if (dd_ptr->als.als_gain_index != APDS9950_DD_ALS_GAIN_64X) 
    {
      dd_ptr->als.als_gain_index++;
      change_gain = TRUE; // changing GAIN, don't inform SMGR

      ALSPRX_MSG_1(MED, "gain increase to %d\r\n", sns_dd_als_again_tb[dd_ptr->als.als_gain_index]);  
    }
  }

  if (change_gain) 
  {
     sns_ddf_read_port(dd_ptr->port_handle,
                             APDS9950_DD_CMD_BYTE|APDS9950_DD_CONTROL_ADDR,
                             (uint8_t*)&i2c_read_data, 1, &bytes_rw);
    
    i2c_data  = (i2c_read_data & 0xFC);
    i2c_data  = (i2c_data | sns_dd_als_again_bit_tb[dd_ptr->als.als_gain_index]);
    
    sns_ddf_write_port(dd_ptr->port_handle,
                                APDS9950_DD_CMD_BYTE|APDS9950_DD_CONTROL_ADDR,
                                (uint8_t*)&i2c_data, 1, &bytes_w);
    
    ALSPRX_MSG_2(ERROR, "raw_data_ready - %d %d", 850, 600);
    
    return FALSE;	// changing GAIN, don't update SMGR first, wait for next cycle
  }
  else 
  {
    if (sensor == SNS_DDF_SENSOR_AMBIENT)
    {
      apds9950_dd_als_rsp(dd_ptr, sub_dev_idx, sensor);
    }
  }

  return TRUE;	// ok, already update SMGR
}

/*===========================================================================

FUNCTION      apds9950_dd_prx_rsp
  
DESCRIPTION   This function is called by the proximity common handler when proximity data
              is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
void apds9950_dd_prx_rsp 
(
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx,
  uint16_t pdata
)
{
  ALSPRX_MSG_3(MED, "thresh_far=%d, thresh_near=%d, pdata=%d\r\n", 
                   dd_ptr->prx.thresh_far, 
                   dd_ptr->prx.thresh_near, 
                   pdata);
  
  if ( pdata >= dd_ptr->prx.thresh_near ) 
  {
    ALSPRX_MSG_2(LOW, "prx_rsp - %d %d", 900, 1);
    if ( dd_ptr->als.cdata < (sns_dd_als_res_tb[dd_ptr->als.als_res_index]) ) 
    {
      if ( dd_ptr->prx.last_nearby != APDS9950_DD_PRX_NEAR_BY ) 
      {
        dd_ptr->prx.detection_state = APDS9950_DD_PRX_NEAR_BY;
      }
      dd_ptr->prx.last_nearby = APDS9950_DD_PRX_NEAR_BY;

      if (dd_ptr->sub_dev[sub_dev_idx].dri_enabled) 
      {
        ALSPRX_MSG_2(LOW, "prx_rsp - %d %d", 900, 3);
        apds9950_dd_set_prx_thresh(dd_ptr, 
                                       dd_ptr->prx.thresh_far,
				       APDS9950_DD_PIHT_NEAR);
      }
    }
    else 
    {
      if ( dd_ptr->prx.last_nearby != APDS9950_DD_PRX_FAR_AWAY ) 
      {
        dd_ptr->prx.detection_state = APDS9950_DD_PRX_FAR_AWAY;
        dd_ptr->prx.last_nearby = APDS9950_DD_PRX_FAR_AWAY;
        apds9950_dd_set_prx_thresh(dd_ptr, 
                                       APDS9950_DD_PILT_FAR,
                                       dd_ptr->prx.thresh_near);
      }
    }
  }
  else if ( pdata < dd_ptr->prx.thresh_far ) {
    ALSPRX_MSG_2(LOW, "prx_rsp - %d %d", 900, 2);
    if ( dd_ptr->prx.last_nearby != APDS9950_DD_PRX_FAR_AWAY ) 
    {
      dd_ptr->prx.detection_state = APDS9950_DD_PRX_FAR_AWAY;
    }
    dd_ptr->prx.last_nearby = APDS9950_DD_PRX_FAR_AWAY;

    if (dd_ptr->sub_dev[sub_dev_idx].dri_enabled) 
    {
       ALSPRX_MSG_2(LOW, "prx_rsp - %d %d", 900, 4);
       apds9950_dd_set_prx_thresh(dd_ptr, 
                                      APDS9950_DD_PILT_FAR,
                                      dd_ptr->prx.thresh_near);
    }
  }
}

/*===========================================================================

FUNCTION      apds9950_dd_prxcomm_rsp
  
DESCRIPTION   This function is called by the arbitration manager when proximity 
              data is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
void apds9950_dd_prxcomm_rsp 
(
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx
)
{
  apds9950_dd_prx_rsp(dd_ptr, sub_dev_idx, dd_ptr->prx.pdata);
  apds9950_dd_async_notify(dd_ptr, sub_dev_idx, SNS_DDF_SENSOR_PROXIMITY);
}

/*===========================================================================

FUNCTION      apds9950_dd_handle_device_indication
  
DESCRIPTION   This function is called by the arbitration manager when proximity 
              data is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
void apds9950_dd_handle_device_indication
(
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx,
  sns_ddf_sensor_e sensor
)
{
  uint8_t   i2c_read_data[10];
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t   bytes_rw;
  int32_t   als_update_flag=0;
  uint32_t  data_mlux;
  int32_t	calibrated_irradiance;
  bool	registry_update=TRUE;

  ALSPRX_MSG_1(ERROR, "apds9950_dd_handle_device_indication - sensor=%d", sensor);
  
  if (sensor == SNS_DDF_SENSOR_AMBIENT)  
  {
    /* read cdata, rdata, gdata and bdata */
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_WORD|APDS9950_DD_CDATAL_ADDR,
                               (uint8_t*)&i2c_read_data, 8, &bytes_rw);
      
    if ( status != SNS_DDF_SUCCESS ) 
    {
      ALSPRX_MSG_0(LOW, "handle_dev_ind - error read");
      return; 
    }  

    dd_ptr->als.cdata = (int16_t)(((int16_t)i2c_read_data[1] << 8) | i2c_read_data[0]);    
    dd_ptr->als.rdata = (int16_t)(((int16_t)i2c_read_data[3] << 8) | i2c_read_data[2]);  
    dd_ptr->als.gdata = (int16_t)(((int16_t)i2c_read_data[5] << 8) | i2c_read_data[4]);  
    dd_ptr->als.bdata = (int16_t)(((int16_t)i2c_read_data[7] << 8) | i2c_read_data[6]);  

    ALSPRX_MSG_2(MED, "AMBIENT: cdata %d rdata %d\r\n", dd_ptr->als.cdata, dd_ptr->als.rdata);
    ALSPRX_MSG_2(MED, "AMBIENT: gdata %d bdata %d\r\n", dd_ptr->als.gdata, dd_ptr->als.bdata);
    
    if (dd_ptr->als.calibration) 
    {
      dd_ptr->als.calibration = FALSE;     
	  als_update_flag = 1;
	  sensor = dd_ptr->common.calibrating_sensor;
	
      // complete
      data_mlux = apds9950_dd_rgb_convert_to_mlux(dd_ptr, dd_ptr->als.cdata, 
                                                      dd_ptr->als.rdata,
                                                      dd_ptr->als.gdata,
                                                      dd_ptr->als.bdata,
													  TRUE);
      data_mlux = data_mlux/1000; // convert mlux to lux

      ALSPRX_MSG_1(LOW, "handle_dev_ind - lux =%d\r\n", data_mlux);
	  
      apds9950_convert_to_irradiance(dd_ptr,
									 sensor,
									 dd_ptr->als.cdata,
									 dd_ptr->als.rdata,
									 dd_ptr->als.gdata,
									 dd_ptr->als.bdata,
									 TRUE);

      ALSPRX_MSG_2(MED, "c=%d, r=%d\r\n", dd_ptr->als.cdata_irradiance, dd_ptr->rgb.rdata_irradiance);
      ALSPRX_MSG_2(MED, "g=%d, b=%d\r\n", dd_ptr->rgb.gdata_irradiance, dd_ptr->rgb.bdata_irradiance);

	  /* scale up 100 times */
	  calibrated_irradiance = (APDS9950_DD_ALS_CALIBRATED_LUX*146);

      ALSPRX_MSG_2(MED, "als_c=%d, irradiance_c=%d\r\n",
				   APDS9950_DD_ALS_CALIBRATED_LUX,
				   calibrated_irradiance);	
	
	  /* CLEAR irradiance */
	  if (((dd_ptr->als.cdata_irradiance) <= (calibrated_irradiance*110)/100) &&
          ((dd_ptr->als.cdata_irradiance) >= (calibrated_irradiance*90)/100))
      {
		dd_ptr->common.nv_db.rgb_c_factor = 100;
      }
	  else if (((dd_ptr->als.cdata_irradiance) <= (calibrated_irradiance*140)/100) &&
          		((dd_ptr->als.cdata_irradiance) >= (calibrated_irradiance*60)/100))
      {
        dd_ptr->common.nv_db.rgb_c_factor = (calibrated_irradiance*100)/dd_ptr->als.cdata_irradiance;
      }
      else
      {
        registry_update = FALSE;
      }

	  /* RED irradiance */
	  if (((dd_ptr->rgb.rdata_irradiance) <= (calibrated_irradiance*110)/100) &&
          ((dd_ptr->rgb.rdata_irradiance) >= (calibrated_irradiance*90)/100))
      {
		dd_ptr->common.nv_db.rgb_r_factor = 100;
      }
	  else if (((dd_ptr->rgb.rdata_irradiance) <= (calibrated_irradiance*140)/100) &&
          		((dd_ptr->rgb.rdata_irradiance) >= (calibrated_irradiance*60)/100))
      {
        dd_ptr->common.nv_db.rgb_r_factor = (calibrated_irradiance*100)/dd_ptr->rgb.rdata_irradiance;
      }
      else
      {
        registry_update = FALSE;
      }

	  /* GREEN irradiance */
	  if (((dd_ptr->rgb.gdata_irradiance) <= (calibrated_irradiance*110)/100) &&
          ((dd_ptr->rgb.gdata_irradiance) >= (calibrated_irradiance*90)/100))
      {
		dd_ptr->common.nv_db.rgb_g_factor = 100;
      }
	  else if (((dd_ptr->rgb.gdata_irradiance) <= (calibrated_irradiance*140)/100) &&
          		((dd_ptr->rgb.gdata_irradiance) >= (calibrated_irradiance*60)/100))
      {
        dd_ptr->common.nv_db.rgb_g_factor = (calibrated_irradiance*100)/dd_ptr->rgb.gdata_irradiance;
      }
      else
      {
        registry_update = FALSE;
      }

	  /* BLUE irradiance */
	  if (((dd_ptr->rgb.bdata_irradiance) <= (calibrated_irradiance*110)/100) &&
          ((dd_ptr->rgb.bdata_irradiance) >= (calibrated_irradiance*90)/100))
      {
		dd_ptr->common.nv_db.rgb_b_factor = 100;
      }
	  else if (((dd_ptr->rgb.bdata_irradiance) <= (calibrated_irradiance*140)/100) &&
          		((dd_ptr->rgb.bdata_irradiance) >= (calibrated_irradiance*60)/100))
      {
        dd_ptr->common.nv_db.rgb_b_factor = (calibrated_irradiance*100)/dd_ptr->rgb.bdata_irradiance;
      }
      else
      {
        registry_update = FALSE;
      }
	
      if ( (data_mlux <= (APDS9950_DD_ALS_CALIBRATED_LUX*110)/100) &&
          (data_mlux >= (APDS9950_DD_ALS_CALIBRATED_LUX*90)/100)) 
      {   
        dd_ptr->common.nv_db.als_factor = 100;            
      }      
      else if ( (data_mlux >= (APDS9950_DD_ALS_CALIBRATED_LUX*60)/100) && 
                 (data_mlux <= (APDS9950_DD_ALS_CALIBRATED_LUX*140)/100) ) 
      {
                   
        dd_ptr->common.nv_db.als_factor = (APDS9950_DD_ALS_CALIBRATED_LUX * 100)/data_mlux;          
      }
      else
      {
        registry_update = FALSE;
      }

      ALSPRX_MSG_1(MED, "als_f=%d\r\n", dd_ptr->common.nv_db.als_factor);
      ALSPRX_MSG_2(MED, "c_f=%d, r_f=%d\r\n", dd_ptr->common.nv_db.rgb_c_factor, dd_ptr->common.nv_db.rgb_r_factor);
      ALSPRX_MSG_2(MED, "g_f=%d, b_f=%d\r\n", dd_ptr->common.nv_db.rgb_g_factor, dd_ptr->common.nv_db.rgb_b_factor);

	  if (registry_update)
	  {
        sns_ddf_smgr_notify_test_complete(dd_ptr->sub_dev[dd_ptr->common.calibrating_sub_dev].smgr_handle, sensor, SNS_DDF_SUCCESS, 0);      
        sns_ddf_smgr_notify_event(dd_ptr->sub_dev[dd_ptr->common.calibrating_sub_dev].smgr_handle, sensor, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);                      
	  }      
	  else 
	  {
        sns_ddf_smgr_notify_test_complete(dd_ptr->sub_dev[dd_ptr->common.calibrating_sub_dev].smgr_handle, sensor, SNS_DDF_EFAIL, APDS9950_DD_ALS_CAL_FAILED);
	  }            
	
	  dd_ptr->common.calibrating_sensor = SNS_DDF_SENSOR__NONE;
	  dd_ptr->common.calibrating_sub_dev = 0;
      dd_ptr->als.enable = FALSE;
	  return;
    }  
    else if ((dd_ptr->sub_dev[sub_dev_idx].dri_enabled) ||
             ((!(dd_ptr->sub_dev[sub_dev_idx].dri_enabled)) && (dd_ptr->als.polling_data))) // do I have to check these conditions???
    {
        /* indicate the data to the light(ALS) sub-driver */      
        als_update_flag = apds9950_dd_als_raw_data_ready(dd_ptr,
                                                         sub_dev_idx,
                                                         sensor,
                                                         dd_ptr->als.cdata, 
                                                         dd_ptr->als.rdata,
                                                         dd_ptr->als.gdata,
                                                         dd_ptr->als.bdata);
        if (als_update_flag) 
        {
          // if this flag is set, it has already sent notification to SMGR
          dd_ptr->als.polling_data = FALSE;
        }
    }
  }
  else if (sensor == SNS_DDF_SENSOR_RGB) 
  {  
    /* read cdata, rdata, gdata and bdata */
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_WORD|APDS9950_DD_CDATAL_ADDR,
                               (uint8_t*)&i2c_read_data, 8, &bytes_rw);
      
    if ( status != SNS_DDF_SUCCESS ) 
    {
      ALSPRX_MSG_0(LOW, "handle_dev_ind - error read");
      return; 
    }  

    dd_ptr->rgb.cdata = (int16_t)(((int16_t)i2c_read_data[1] << 8) | i2c_read_data[0]);    
    dd_ptr->rgb.rdata = (int16_t)(((int16_t)i2c_read_data[3] << 8) | i2c_read_data[2]);  
    dd_ptr->rgb.gdata = (int16_t)(((int16_t)i2c_read_data[5] << 8) | i2c_read_data[4]);  
    dd_ptr->rgb.bdata = (int16_t)(((int16_t)i2c_read_data[7] << 8) | i2c_read_data[6]);  

    ALSPRX_MSG_3(MED, "RGB: rdata %d gdata %d bdata %d\r\n", dd_ptr->rgb.rdata, dd_ptr->rgb.gdata, dd_ptr->rgb.bdata);

    /* kk 26-Oct-2013 : check if gain need to adjust before reporting to SMGR */
    als_update_flag = apds9950_dd_als_raw_data_ready(dd_ptr,
                                                     sub_dev_idx,
                                                     sensor,
                                                     dd_ptr->rgb.cdata, 
                                                     dd_ptr->rgb.rdata,
                                                     dd_ptr->rgb.gdata,
                                                     dd_ptr->rgb.bdata);    

    if (als_update_flag) 
    {    
      apds9950_dd_async_notify(dd_ptr, sub_dev_idx, sensor);
  
      if (dd_ptr->rgb.polling_data)
      {
         dd_ptr->rgb.polling_data = FALSE;
      }
    }
  }
  else if (sensor == SNS_DDF_SENSOR_CT_C) 
  {  
    /* read cdata, rdata, gdata and bdata */
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_WORD|APDS9950_DD_CDATAL_ADDR,
                               (uint8_t*)&i2c_read_data, 8, &bytes_rw);
      
    if ( status != SNS_DDF_SUCCESS ) 
    {
      ALSPRX_MSG_0(LOW, "handle_dev_ind - error read");
      return; 
    }  

    dd_ptr->cct.cdata = (int16_t)(((int16_t)i2c_read_data[1] << 8) | i2c_read_data[0]);    
    dd_ptr->cct.rdata = (int16_t)(((int16_t)i2c_read_data[3] << 8) | i2c_read_data[2]);  
    dd_ptr->cct.gdata = (int16_t)(((int16_t)i2c_read_data[5] << 8) | i2c_read_data[4]);  
    dd_ptr->cct.bdata = (int16_t)(((int16_t)i2c_read_data[7] << 8) | i2c_read_data[6]);  

    ALSPRX_MSG_3(MED, "CT_C: rdata %d gdata %d bdata %d\r\n", dd_ptr->cct.rdata, dd_ptr->cct.gdata, dd_ptr->cct.bdata);

    /* kk 26-Oct-2013 : check if gain need to adjust before reporting to SMGR */
    als_update_flag = apds9950_dd_als_raw_data_ready(dd_ptr,
                                                     sub_dev_idx,
                                                     sensor,
                                                     dd_ptr->cct.cdata, 
                                                     dd_ptr->cct.rdata,
                                                     dd_ptr->cct.gdata,
                                                     dd_ptr->cct.bdata);    

    if (als_update_flag) 
    {        
      apds9950_dd_cct_raw_data_ready(dd_ptr, sub_dev_idx, sensor,
                                         dd_ptr->cct.cdata, dd_ptr->cct.rdata, dd_ptr->cct.gdata, dd_ptr->cct.bdata);
      apds9950_dd_async_notify(dd_ptr, sub_dev_idx, sensor);
  
      if (dd_ptr->cct.polling_data)
      {
        dd_ptr->cct.polling_data = FALSE;
      }
    }
  }
  else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
  {     
    /* read pdata */
    status = sns_ddf_read_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_WORD|APDS9950_DD_PDATAL_ADDR,
                               (uint8_t*)&i2c_read_data, 2, &bytes_rw);
      
    if ( status != SNS_DDF_SUCCESS ) 
    {
      ALSPRX_MSG_0(LOW, "handle_dev_ind - error read");
      return; 
    }  

    dd_ptr->prx.pdata = (int16_t)(((int16_t)i2c_read_data[1] << 8) | i2c_read_data[0]);    

    ALSPRX_MSG_1(MED, "PROXIMITY: pdata %d\r\n", dd_ptr->prx.pdata);
    
    if (dd_ptr->prx.calibration) 
    {
      dd_ptr->prx.calibration = FALSE;
      
      if (dd_ptr->prx.calibration_loop < 10) 
      {
        dd_ptr->prx.calibration_pdata += dd_ptr->prx.pdata;          
        dd_ptr->prx.calibration = TRUE;
        dd_ptr->prx.calibration_loop++;
        apds9950_dd_req_data (dd_ptr, sub_dev_idx, sensor);
        return; // continue getting data
      }
      else 
      {
        // complete
        dd_ptr->prx.calibration_pdata /= dd_ptr->prx.calibration_loop;
        if (dd_ptr->prx.calibration_pdata > APDS9950_DD_PRX_CAL_THRESHOLD) 
        {
          sns_ddf_smgr_notify_test_complete(dd_ptr->sub_dev[sub_dev_idx].smgr_handle, sensor, SNS_DDF_EFAIL, APDS9950_DD_PRX_CAL_FAILED);
        }
        else 
        {
#if 0
          dd_ptr->common.nv_db.thresh_near = dd_ptr->prx.calibration_pdata + APDS9950_DD_PRX_FACTOR;
          dd_ptr->common.nv_db.thresh_far = dd_ptr->common.nv_db.thresh_near - 100;
          dd_ptr->prx.thresh_near = dd_ptr->common.nv_db.thresh_near;
          dd_ptr->prx.thresh_far = dd_ptr->common.nv_db.thresh_far;
#endif		
          apds9950_dd_set_prx_factor(dd_ptr, sensor, dd_ptr->prx.calibration_pdata);
          
          sns_ddf_smgr_notify_test_complete(dd_ptr->sub_dev[sub_dev_idx].smgr_handle, sensor, SNS_DDF_SUCCESS, 0);
          sns_ddf_smgr_notify_event(dd_ptr->sub_dev[sub_dev_idx].smgr_handle, sensor, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
        }
      }      
    }
    else if ((dd_ptr->sub_dev[sub_dev_idx].dri_enabled) || 
             ((!dd_ptr->sub_dev[sub_dev_idx].dri_enabled) && (dd_ptr->prx.polling_data))) 
    {
        /* indicate the data to the proximity common handler */
        apds9950_dd_prxcomm_rsp (dd_ptr, sub_dev_idx);  

        if(dd_ptr->prx.polling_data)
		{
			dd_ptr->prx.polling_data = FALSE;
		}
    }
  }

  if (!dd_ptr->sub_dev[sub_dev_idx].dri_enabled) // polling sub-device
  {
    if (sensor == SNS_DDF_SENSOR_AMBIENT) 
    {
      if (!als_update_flag) 
      {
        apds9950_dd_start_general_timer (dd_ptr, sub_dev_idx, sensor, 
                                         sns_dd_als_integration_time_tb[dd_ptr->als.als_res_index]);
        return;
      }

      dd_ptr->als.enable = FALSE;
    }
    else if (sensor == SNS_DDF_SENSOR_RGB)
    {
      if (!als_update_flag) 
      {
        apds9950_dd_start_general_timer (dd_ptr, sub_dev_idx, sensor, 
                                         sns_dd_als_integration_time_tb[dd_ptr->als.als_res_index]);
        return;
      }
	  
	  dd_ptr->rgb.enable = FALSE;
    }
    else if (sensor == SNS_DDF_SENSOR_CT_C)
    {
      if (!als_update_flag) 
      {
        apds9950_dd_start_general_timer (dd_ptr, sub_dev_idx, sensor, 
                                         sns_dd_als_integration_time_tb[dd_ptr->als.als_res_index]);
        return;
      }

	  dd_ptr->cct.enable = FALSE;      
    }
    else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
    {
      dd_ptr->prx.enable = FALSE;
    }
  }
}

/*===========================================================================

  FUNCTION:   sns_dd_apds9950_handle_irq

===========================================================================*/
/*!
  @brief Interrupt handler
 
  @detail
  Called in response to an interrupt for this driver.
 
  @see sns_ddf_driver_if_s.handle_irq()
 
  @param[in] dd_handle  Handle to a driver instance. 
  @param[in] irq        The IRQ representing the interrupt that occured. 
  
  @return None 
*/
/*=========================================================================*/
static void sns_dd_apds9950_handle_irq
(
  sns_ddf_handle_t  dd_handle,
  uint32_t          gpio_num,
  sns_ddf_time_t    timestamp
)
{
  apds9950_dd_dev_s* dd_ptr;
  /*uint32_t sub_dev_idx;*/
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t bytes_rw;
  uint8_t i2c_data = 0x0;
  uint8_t disable_data=APDS9950_DD_PWR_ON;
  uint8_t reg_addr=0;
  uint8_t status_reg=0, enable_reg=0;

  ALSPRX_MSG_1(MED, "HANDLE IRQ START - gpio_num=%d", gpio_num);

  /*sub_dev_idx = (uint32_t)dd_handle & (APDS9950_DD_HANDLE_ALIGN-1);*/
  dd_ptr = (apds9950_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9950_DD_HANDLE_ALIGN-1)));
  
  /* STATUS */  
  status = sns_ddf_read_port(dd_ptr->port_handle,
                             APDS9950_DD_CMD_BYTE|APDS9950_DD_STATUS_ADDR,
                             (uint8_t*)&i2c_data, 1, &bytes_rw);
  
  if(status != SNS_DDF_SUCCESS) 
  {
    ALSPRX_MSG_1(ERROR, "irq - error read =%d", status);
    return;
  }

  //ALSPRX_MSG_1(MED, "irq - i2c_data=%x", i2c_data);
  
  status_reg = i2c_data;
  
  status = sns_ddf_read_port(dd_ptr->port_handle,
                             APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                             (uint8_t*)&i2c_data, 1, &bytes_rw);
  
  if(status != SNS_DDF_SUCCESS) 
  {
    ALSPRX_MSG_1(ERROR, "irq - error read =%d", status);
    return;
  }

  ALSPRX_MSG_1(MED, "irq - enable=%x", i2c_data);

  enable_reg = i2c_data;
  
  dd_ptr->int_timestamp = timestamp;

  /* is this really needed? */
  /* Turn OFF all ENABLE bits first */
  status = sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                              (uint8_t*)&disable_data, 
                              1,
                              &bytes_rw);
  
  if ( status != SNS_DDF_SUCCESS ) 
  {
    return;
  }  

   ALSPRX_MSG_1(HIGH, "sns_dd_apds9950_handle_irq status_reg=%d", status_reg);

  /* Check which interrupts occured */
  if ((status_reg & APDS9950_DD_PINT_STATUS ) && dd_ptr->prx.enable && (dd_ptr->sub_dev[0].dri_enabled))
  {
    /* Proximity Interrupt */
    ALSPRX_MSG_0(MED, "irq - prox");
    apds9950_dd_handle_device_indication(dd_ptr,
                                         0,
                                         SNS_DDF_SENSOR_PROXIMITY);
    
    reg_addr = APDS9950_DD_CMD_CLR_PS_INT;
  }

  if ((status_reg & APDS9950_DD_AINT_STATUS) && dd_ptr->als.enable && (dd_ptr->sub_dev[0].dri_enabled)) 
  {
    /* ALS Interrupt */
	ALSPRX_MSG_0(MED, "irq - als");
    
    dd_ptr->als.polling_data = TRUE;
    //dd_ptr->rgb.polling_data = FALSE;
    //dd_ptr->cct.polling_data = FALSE;
    
    apds9950_dd_handle_device_indication(dd_ptr,
                                         0,
                                         SNS_DDF_SENSOR_AMBIENT);
    
    if (reg_addr != 0) 
    {
      reg_addr = APDS9950_DD_CMD_CLR_PS_ALS_INT;
    }
    else 
    {
      reg_addr = APDS9950_DD_CMD_CLR_ALS_INT;
    }
  } 

  if((status_reg & APDS9950_DD_AINT_STATUS) && dd_ptr->rgb.enable && (dd_ptr->sub_dev[1].dri_enabled))
  {
      /* RGB Interrupt */
      ALSPRX_MSG_0(MED, "irq - rgb");

      //dd_ptr->als.polling_data = FALSE;
      dd_ptr->rgb.polling_data = TRUE;
      //dd_ptr->cct.polling_data = FALSE;

      apds9950_dd_handle_device_indication(dd_ptr,
          1,
          SNS_DDF_SENSOR_RGB);

      reg_addr |= APDS9950_DD_CMD_CLR_ALS_INT;
      // Update ALS thresholds
      if(!dd_ptr->als.enable)
      {
          uint16_t ailt;
          uint16_t aiht;
          uint16_t diff;

          if (dd_ptr->sub_dev[1].dri_enabled) 
          {

              diff = PCNT_OF_RAW(dd_ptr->common.nv_db.als_change_pcnt, 
                  dd_ptr->rgb.cdata);
              ailt = (dd_ptr->rgb.cdata - diff);
              aiht = (dd_ptr->rgb.cdata + diff);
              apds9950_dd_set_als_thresh(dd_ptr, ailt, aiht);
              ALSPRX_MSG_3(HIGH, "Update thesh - raw_data=%d ailt=%d aiht=%d", 
                  dd_ptr->rgb.cdata, ailt, aiht);

          }
      }
  }

  if((status_reg & APDS9950_DD_AINT_STATUS) && dd_ptr->cct.enable && (dd_ptr->sub_dev[1].dri_enabled))
  {
      /* CCT Interrupt */
      ALSPRX_MSG_0(MED, "irq - cct");

      //dd_ptr->als.polling_data = FALSE;
      //dd_ptr->rgb.polling_data = FALSE;
      dd_ptr->cct.polling_data = TRUE;

      apds9950_dd_handle_device_indication(dd_ptr,
          1,
          SNS_DDF_SENSOR_CT_C);

      reg_addr |= APDS9950_DD_CMD_CLR_ALS_INT;
      // Update ALS thresholds
      if(!dd_ptr->als.enable)
      {
          uint16_t ailt;
          uint16_t aiht;
          uint16_t diff;

          if (dd_ptr->sub_dev[1].dri_enabled) 
          {

              diff = PCNT_OF_RAW(dd_ptr->common.nv_db.als_change_pcnt, 
                  dd_ptr->cct.cdata);
              ailt = (dd_ptr->cct.cdata - diff);
              aiht = (dd_ptr->cct.cdata + diff);
              apds9950_dd_set_als_thresh(dd_ptr, ailt, aiht);
              ALSPRX_MSG_3(HIGH, "Update thesh - raw_data=%d ailt=%d aiht=%d", 
                  dd_ptr->cct.cdata, ailt, aiht);

          }
      }
  }

#ifdef ALSPRX_DEBUG
    if ((status_reg & APDS9950_DD_PINT_STATUS ) && !(dd_ptr->prx.enable))
      MSG(MSG_SSID_SNS,DBG_ERROR_PRIO, "ALSPRX - PINT set, but PRX not enabled");

    if ((status_reg & APDS9950_DD_AINT_STATUS) && !(dd_ptr->als.enable))
      MSG(MSG_SSID_SNS,DBG_ERROR_PRIO, "ALSPRX - AINT set, but ALS not enabled");
#endif

  /* Clear interrupts */
  if (reg_addr == 0) 
  {
    i2c_data = APDS9950_DD_CMD_CLR_PS_ALS_INT;
  }
  
  status = apds9950_dd_clear_int_reg(dd_ptr, reg_addr);

  ALSPRX_MSG_3(MED, "irq - clear int - %d addr=%d result=%d", 
               600, reg_addr, status);

  /* Enable Sensor */
  i2c_data = enable_reg;  // write back the previous value
  status = sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                              (uint8_t*)&i2c_data, 
                              1,
                              &bytes_rw);
  ALSPRX_MSG_2(MED, "irq - enable - data=%x result=%d", i2c_data, status);
  
  if ( status != SNS_DDF_SUCCESS ) 
  {
    return;
  }

  ALSPRX_MSG_0(ERROR, "HANDLE IRQ END     irq - done");
}

/*===========================================================================

  FUNCTION:   sns_dd_apds9950_handle_timer

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
static void sns_dd_apds9950_handle_timer 
(
  sns_ddf_handle_t dd_handle, 
  void* arg
)
{
  apds9950_dd_dev_s* dd_ptr;
  uint32_t sub_dev_idx;
  sns_ddf_sensor_e sensor=*(sns_ddf_sensor_e*)arg;

  ALSPRX_MSG_1(HIGH, "handle_timer - sensor=%d", sensor);

  sub_dev_idx = (uint32_t)dd_handle & (APDS9950_DD_HANDLE_ALIGN-1);
  dd_ptr = (apds9950_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9950_DD_HANDLE_ALIGN-1)));
  
  if (sensor == SNS_DDF_SENSOR_AMBIENT) 
  {
    if ( FALSE == dd_ptr->als.timer_active ) return; // when sensor is als
  }
  else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
  { 
    if ( FALSE == dd_ptr->prx.timer_active ) return; // when sensor is prx    
  }
  else if (sensor == SNS_DDF_SENSOR_RGB)
  {
    if ( FALSE == dd_ptr->rgb.timer_active ) return; // when sensor is rgb        
  }
  else if (sensor == SNS_DDF_SENSOR_CT_C)
  {
    if ( FALSE == dd_ptr->cct.timer_active ) return; // when sensor is cct        
  }

  ALSPRX_MSG_2(HIGH, "handle_timer - sensor=%d sub_dev_id=%d", sensor, sub_dev_idx);

  apds9950_dd_stop_general_timer(dd_ptr, sub_dev_idx, sensor);

  apds9950_dd_handle_device_indication(dd_ptr, sub_dev_idx, sensor);
}

/*==============================================================================

FUNCTION      apds9950_dd_rgb_init
  
DESCRIPTION   Initializes the light(ALS) driver

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e apds9950_dd_rgb_init 
(
  apds9950_dd_dev_s* dd_ptr
)
{
  apds9950_dd_rgb_s *rgb_ptr = &dd_ptr->rgb;

  rgb_ptr->als_res_index = APDS9950_DD_RES_14BIT;
  rgb_ptr->als_gain_index = APDS9950_DD_ALS_GAIN_4X;

  rgb_ptr->enable = FALSE;
  rgb_ptr->timer_active = FALSE;

  rgb_ptr->polling_data = FALSE;
  rgb_ptr->dri_data = FALSE;
  
  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      apds9950_dd_cct_init
  
DESCRIPTION   initializes the ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e apds9950_dd_cct_init 
( 
  apds9950_dd_dev_s* dd_ptr
)
{
  apds9950_dd_cct_s *cct_ptr = &dd_ptr->cct;

  cct_ptr->als_res_index = APDS9950_DD_RES_14BIT;
  cct_ptr->als_gain_index = APDS9950_DD_ALS_GAIN_4X;

  cct_ptr->enable = FALSE;
  cct_ptr->timer_active = FALSE;
  cct_ptr->polling_data = FALSE;
  cct_ptr->dri_data = FALSE;
  
  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      apds9950_dd_als_init
  
DESCRIPTION   Initializes the light(ALS) driver

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e apds9950_dd_als_init 
(
  apds9950_dd_dev_s* dd_ptr
)
{
  apds9950_dd_als_s *als_ptr = &dd_ptr->als;

  als_ptr->last_mlux = APDS9950_DD_ALS_DFLT_MILLI_LUX;

  ALSPRX_MSG_2(HIGH, "rgb.enable=%d cct.enable=%d", dd_ptr->rgb.enable, dd_ptr->cct.enable);
  
  if(!dd_ptr->rgb.enable && !dd_ptr->cct.enable)
  {
    als_ptr->als_res_index = APDS9950_DD_RES_14BIT;
    als_ptr->als_gain_index = APDS9950_DD_ALS_GAIN_4X;
    ALSPRX_MSG_0(HIGH, "res and gain index reset");
  }

  als_ptr->enable = FALSE;
  als_ptr->timer_active = FALSE;

  als_ptr->polling_data = FALSE;  // polling enable flag
  als_ptr->dri_data = FALSE;  // polling enable flag
  als_ptr->calibration = FALSE;

  dd_ptr->als.saturated_lux = APDS9950_DD_ALS_MAX_LUX;
    
  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      apds9950_dd_prx_init
  
DESCRIPTION   initializes the ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e apds9950_dd_prx_init 
( 
  apds9950_dd_dev_s* dd_ptr
)
{
  apds9950_dd_prx_s *prx_ptr = &dd_ptr->prx;

  prx_ptr->last_nearby = APDS9950_DD_PRX_NEAR_BY_UNKNOWN;
  prx_ptr->thresh_near = dd_ptr->common.nv_db.thresh_near;
  prx_ptr->thresh_far = dd_ptr->common.nv_db.thresh_far;

  prx_ptr->polling_data = FALSE;
  prx_ptr->dri_data = FALSE;
  prx_ptr->calibration = FALSE;
  prx_ptr->calibration_loop = 0;
  
  prx_ptr->enable = FALSE;
  prx_ptr->timer_active = FALSE;
  
  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      apds9950_dd_sub_dev_init
  
DESCRIPTION   initializes the ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e apds9950_dd_sub_dev_init 
( 
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx
)
{
  apds9950_dd_sub_dev_s *sub_dev_ptr = &dd_ptr->sub_dev[sub_dev_idx];
  
  sub_dev_ptr->pwr_mode = SNS_DDF_POWERSTATE_LOWPOWER;
  sub_dev_ptr->state = APDS9950_DD_DEV_STOPPED;
  
  sub_dev_ptr->dri_registration = FALSE;

  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      apds9950_dd_common_init
  
DESCRIPTION   initializes the ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e apds9950_dd_common_init 
( 
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx
)
{
  apds9950_dd_common_s *common_ptr = &dd_ptr->common;
  
  if (sub_dev_idx == 0) 
  {
    common_ptr->prx_odr = 0;
    common_ptr->als_odr = 0;
    common_ptr->prx_req_odr = 0;
    common_ptr->als_req_odr = 0; 
  }
  else 
  {
    common_ptr->rgb_odr = 0;
    common_ptr->cct_odr = 0;
    common_ptr->rgb_req_odr = 0;
    common_ptr->cct_req_odr = 0; 
  }
  
  common_ptr->calibrating_sensor = SNS_DDF_SENSOR__NONE;
  dd_ptr->common.calibrating_sub_dev = 0;

  return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      apds9950_dd_init_data_struct
  
DESCRIPTION   Initialize data structures for ALS and PRX

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None 

===========================================================================*/
static sns_ddf_status_e apds9950_dd_init_data_struct
(
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx
)
{
  sns_ddf_status_e init_status = SNS_DDF_SUCCESS;

  /* Initialize the common database */
  if ( (init_status = apds9950_dd_common_init(dd_ptr, sub_dev_idx)) != SNS_DDF_SUCCESS )
  {
    return init_status;
  }

  if ( (init_status = apds9950_dd_sub_dev_init(dd_ptr, sub_dev_idx)) != SNS_DDF_SUCCESS )
  {
    return init_status;
  }

  if (sub_dev_idx == 0) 
  {
    if ( (init_status = apds9950_dd_als_init(dd_ptr)) != SNS_DDF_SUCCESS )
    {
      return init_status;
    }

    if ( (init_status = apds9950_dd_prx_init(dd_ptr)) != SNS_DDF_SUCCESS )
    {
      return init_status;
    }
  }
  else 
  {
    if ( (init_status = apds9950_dd_rgb_init(dd_ptr)) != SNS_DDF_SUCCESS )
    {
      return init_status;
    }

    if ( (init_status = apds9950_dd_cct_init(dd_ptr)) != SNS_DDF_SUCCESS )
    {
      return init_status;
    }    
  }

  return init_status;
}

/*===========================================================================

FUNCTION      apds9950_dd_set_powerstate
  
DESCRIPTION   Sets the power state of the ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None 

===========================================================================*/
sns_ddf_status_e apds9950_dd_set_powerstate
(
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx,
  sns_ddf_sensor_e sensor,
  sns_ddf_powerstate_e op_mode,
  bool init_data
)
{
  uint8_t i2c_data = APDS9950_DD_PWR_DOWN;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  uint8_t bytes_rw;

#ifndef ALSPRX_OEM_BUILD
  if (EnableI2C != TRUE)  /* Avago's DDK platform doesn't support this */ 
    return SNS_DDF_SUCCESS;
#endif

  if ( op_mode == SNS_DDF_POWERSTATE_ACTIVE )
  {
    if (dd_ptr->sub_dev[sub_dev_idx].pwr_mode == SNS_DDF_POWERSTATE_ACTIVE) 
    {
      // don't have to do this if other sensor type already did it
      return status;
    }
    
    if ( dd_ptr->sub_dev[0].pwr_mode == SNS_DDF_POWERSTATE_LOWPOWER && 
         dd_ptr->sub_dev[1].pwr_mode == SNS_DDF_POWERSTATE_LOWPOWER )
    {
      // if another sub-device is already enabled, don't disturb the enable register
      /* ENABLE=PON */    
      i2c_data = APDS9950_DD_PWR_ON;
      status = sns_ddf_write_port(dd_ptr->port_handle,
                                  APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                                  (uint8_t*)&i2c_data, 
                                  1,
                                  &bytes_rw);
      CHECK_STATUS(status);
    }
    
    dd_ptr->sub_dev[sub_dev_idx].pwr_mode = op_mode;
    dd_ptr->sub_dev[sub_dev_idx].state = APDS9950_DD_DEV_CONFIGURED;    
  }
  else /* when op_mode is SNS_DDF_POWERSTATE_LOWPOWER */
  {    
    if (sub_dev_idx == 0)
    {
      // SNS_DDF_SENSOR_AMBIENT or SNS_DDF_SENSOR_PROXIMITY or SNS_DDF_SENSOR__ALL      
      if (sensor == SNS_DDF_SENSOR__ALL) 
      {
        if (!dd_ptr->rgb.enable && !dd_ptr->cct.enable)
        {
          // we can power it down as no other sensor type is using it
          ALSPRX_MSG_0(HIGH, "APDS9950_DD_PWR_DOWN");
          i2c_data = APDS9950_DD_PWR_DOWN;
        }
        else if (dd_ptr->rgb.enable || dd_ptr->cct.enable)
        {
            // RGB or CCT is active
            ALSPRX_MSG_0(HIGH, "APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE");
            i2c_data = APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE; 
			if (dd_ptr->sub_dev[1].dri_enabled)
            {
              i2c_data |= (APDS9950_DD_ALS_INT_ENABLE|APDS9950_DD_WAIT_ENABLE);
            }
        }
        
        if (init_data == TRUE) 
        {
          /* cleanup */         
          status = apds9950_dd_init_data_struct(dd_ptr, sub_dev_idx);
          CHECK_STATUS(status);
        }
        
        dd_ptr->sub_dev[sub_dev_idx].pwr_mode = op_mode;
        dd_ptr->sub_dev[sub_dev_idx].state = APDS9950_DD_DEV_CONFIGURED;        
      }
      else 
      { 
        return SNS_DDF_EINVALID_PARAM;
      }      
    }
    else 
    {
      // SNS_DDF_SENSOR_RGB or SNS_DDF_SENSOR_CCT or SNS_DDF_SENSOR__ALL
      if (sensor == SNS_DDF_SENSOR__ALL) 
      {
        if (!dd_ptr->prx.enable && !dd_ptr->als.enable) // cannot power down if other sensor type is active
        {
           ALSPRX_MSG_0(HIGH, "APDS9950_DD_PWR_DOWN");
           i2c_data = APDS9950_DD_PWR_DOWN;
        }
        else if (dd_ptr->prx.enable || dd_ptr->als.enable)
        {
          if (dd_ptr->prx.enable && dd_ptr->als.enable)
          {
            i2c_data = APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|APDS9950_DD_PRX_ENABLE;
            if (dd_ptr->sub_dev[0].dri_enabled)
            {
              i2c_data |= (APDS9950_DD_PRX_INT_ENABLE|APDS9950_DD_ALS_INT_ENABLE|APDS9950_DD_WAIT_ENABLE);
            }
          }
          else if (dd_ptr->prx.enable && !dd_ptr->als.enable)
          {
            // still need als function in PS
            i2c_data = APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|APDS9950_DD_PRX_ENABLE;
            if (dd_ptr->sub_dev[0].dri_enabled)
            {
              i2c_data |= (APDS9950_DD_PRX_INT_ENABLE|APDS9950_DD_WAIT_ENABLE);
            }            
          }
          else if (!dd_ptr->prx.enable && dd_ptr->als.enable)
          {
            i2c_data = APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE;
            if (dd_ptr->sub_dev[0].dri_enabled)
            {
              i2c_data |= (APDS9950_DD_ALS_INT_ENABLE|APDS9950_DD_WAIT_ENABLE);
            }
          }
        }
        
        if (init_data == TRUE)
        {
          /* cleanup */
          status = apds9950_dd_init_data_struct(dd_ptr, sub_dev_idx);
          CHECK_STATUS(status);
        }
        
        dd_ptr->sub_dev[sub_dev_idx].pwr_mode = op_mode;
        dd_ptr->sub_dev[sub_dev_idx].state = APDS9950_DD_DEV_CONFIGURED;        
      }
      else 
      { 
         return SNS_DDF_EINVALID_PARAM;
      }
    }

    if  ((!dd_ptr->sub_dev[sub_dev_idx].dri_enabled) && (op_mode == SNS_DDF_POWERSTATE_LOWPOWER))
    {
      apds9950_dd_stop_general_timer( dd_ptr, sub_dev_idx, sensor ); /* stop any pending timers */
    }

    status = sns_ddf_write_port(dd_ptr->port_handle,
                                APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                                (uint8_t*)&i2c_data, 
                                1,
                                &bytes_rw);
    CHECK_STATUS(status);
  }

  return status;
}

/*===========================================================================

FUNCTION      apds9950_dd_set_odr
  
DESCRIPTION   Sets ODR of the ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None 

===========================================================================*/
sns_ddf_status_e apds9950_dd_set_odr
(
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx,
  sns_ddf_sensor_e sensor,
  uint32_t odr
)
{
  sns_ddf_status_e status=SNS_DDF_SUCCESS;
  bool change=TRUE;
  
  switch ( sensor )
  {
    case SNS_DDF_SENSOR_AMBIENT:
      if (dd_ptr->common.als_req_odr == odr)
      {
        change = FALSE;
      }
      dd_ptr->common.als_req_odr = odr;
      break;

	case SNS_DDF_SENSOR_PROXIMITY:
		if (dd_ptr->common.prx_req_odr == odr)
		{
			change = FALSE;
		}    
		dd_ptr->common.prx_req_odr = odr;
		break;
	
	case SNS_DDF_SENSOR_RGB:
		change = FALSE;		
		dd_ptr->common.rgb_req_odr = odr;
		dd_ptr->common.rgb_odr = odr;
		break;

	case SNS_DDF_SENSOR_CT_C:
		change = FALSE;		
		dd_ptr->common.cct_req_odr = odr;
		dd_ptr->common.cct_odr = odr;
		break;

    default:
      status = SNS_DDF_EFAIL;
  }

  if (change)
  {
    apds9950_dd_set_cycle_time(dd_ptr);
  }

  return status;
}

/*===========================================================================

FUNCTION      apds9950_dd_set_resolution
  
DESCRIPTION   Sets the resolution of the ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None 

===========================================================================*/
sns_ddf_status_e apds9950_dd_set_resolution
(
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx,
  sns_ddf_sensor_e  sensor,
  apds9950_dd_res_e resolution
)
{
  uint16_t i2c_data = 0x0;
  uint8_t bytes_w;
  sns_ddf_status_e status=SNS_DDF_EFAIL;
  
  switch ( sensor )
  {
    case SNS_DDF_SENSOR_AMBIENT:
    case SNS_DDF_SENSOR_RGB:
    case SNS_DDF_SENSOR_CT_C:
      if (resolution == sns_dd_als_adc_bit_tb[APDS9950_DD_RES_14BIT]) 
      {    
        i2c_data  = sns_dd_als_atime_tb[APDS9950_DD_RES_14BIT];
        dd_ptr->als.als_res_index = APDS9950_DD_RES_14BIT;
      }
      else if (resolution == sns_dd_als_adc_bit_tb[APDS9950_DD_RES_15BIT]) 
      {
        i2c_data  = sns_dd_als_atime_tb[APDS9950_DD_RES_15BIT];
        dd_ptr->als.als_res_index = APDS9950_DD_RES_15BIT;
      }
      else 
      {  // APDS9950_DD_RES_16BIT
        i2c_data  = sns_dd_als_atime_tb[APDS9950_DD_RES_16BIT];
        dd_ptr->als.als_res_index = APDS9950_DD_RES_16BIT;
      }
      
      status = sns_ddf_write_port(dd_ptr->port_handle,
                               APDS9950_DD_CMD_BYTE|APDS9950_DD_ATIME_ADDR,
                               (uint8_t*)&i2c_data, 
                               1,
                               &bytes_w);
      
      break;

  default:
      status = SNS_DDF_EFAIL;
  }
  
  return status;
}

/*===========================================================================

FUNCTION      apds9950_reset_registers
  
DESCRIPTION   Load pre-defined value to all registers

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None 

===========================================================================*/
static sns_ddf_status_e apds9950_reset_registers
(
  apds9950_dd_dev_s* dd_ptr
)
{
  uint8_t i2c_data = 0x0;
  uint8_t bytes_w;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  /* DO NOT allow to reset registers if one of the sub dev is active */
  if (dd_ptr->prx.enable || dd_ptr->als.enable || dd_ptr->rgb.enable || dd_ptr->cct.enable)
  {
    return status;
  }
      
  /* Initialize registers */
  /* ENABLE */
  i2c_data = APDS9950_DD_PWR_DOWN;    
  status = sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                              &i2c_data, 1, &bytes_w);
  CHECK_STATUS(status);  
  
  status = apds9950_dd_clear_int_reg(dd_ptr, APDS9950_DD_CMD_CLR_PS_ALS_INT);
  CHECK_STATUS(status);

  /* PERS */
  i2c_data  = APDS9950_DD_PPERS_2|APDS9950_DD_APERS_2;
  status = sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9950_DD_CMD_BYTE|APDS9950_DD_PERS_ADDR,
                              (uint8_t*)&i2c_data, 1, &bytes_w);
  CHECK_STATUS(status);

  /* PPCOUNT */
  i2c_data  = APDS9950_DD_PRX_PULSE_NUMBER;
  status = sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9950_DD_CMD_BYTE|APDS9950_DD_PPCOUNT_ADDR,
                              (uint8_t*)&i2c_data, 1, &bytes_w);
  CHECK_STATUS(status);
  
  /* CONTROL */
  i2c_data  = (sns_dd_als_again_bit_tb[dd_ptr->als.als_gain_index]|APDS9950_DD_PRX_IR_DIOD|APDS9950_DD_PDRVIE);

  status = sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9950_DD_CMD_BYTE|APDS9950_DD_CONTROL_ADDR,
                              (uint8_t*)&i2c_data, 1, &bytes_w);    
  CHECK_STATUS(status);
  
  status = apds9950_dd_update_intg_time(dd_ptr, FALSE, FALSE, TRUE);
  CHECK_STATUS(status);
  
  return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_apds9950_reset

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
static sns_ddf_status_e sns_dd_apds9950_reset
(
  sns_ddf_handle_t dd_handle
)
{
  apds9950_dd_dev_s* dd_ptr;
  sns_ddf_status_e init_status = SNS_DDF_SUCCESS;
  uint32_t sub_dev_idx;

  if ( dd_handle == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }
  
  sub_dev_idx = (uint32_t)dd_handle & (APDS9950_DD_HANDLE_ALIGN-1);
  dd_ptr = (apds9950_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9950_DD_HANDLE_ALIGN-1)));

  ALSPRX_MSG_1(HIGH, "reset sub dev ID = %d", sub_dev_idx);

  if ( (init_status = apds9950_dd_init_data_struct(dd_ptr, sub_dev_idx)) != SNS_DDF_SUCCESS )
  {
    return init_status;
  }

  /* init all registers with pre-defined value */
  if ( (init_status = apds9950_reset_registers(dd_ptr)) != SNS_DDF_SUCCESS ) 
  {
    return init_status; 
  }
  
#ifdef FEATURE_APDS99XX_INT_PIN_CONFIG
  init_status = sns_dd_isl29028_int_disable();
  ALSPRX_MSG_2(LOW, "reset - %d int disable result=%d", 0, init_status);
#endif

  
  if ( (init_status = apds9950_dd_set_powerstate(dd_ptr, sub_dev_idx, SNS_DDF_SENSOR__ALL, 
                          SNS_DDF_POWERSTATE_LOWPOWER, FALSE)) != SNS_DDF_SUCCESS )
  {
    return init_status;
  }

  return init_status;
}

/*===========================================================================

  FUNCTION:   sns_dd_apds9950_init

===========================================================================*/
/*!
  @brief Initializes the Ambient Light Sensing and Proximity device driver
   Allocates a handle to a driver instance, opens a communication port to 
   associated devices, configures the driver and devices, and places 
   the devices in the default power state. Returns the instance handle along 
   with a list of supported sensors. This function will be called at init 
   time.
    
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
 
*/
/*=========================================================================*/                                             
static sns_ddf_status_e sns_dd_apds9950_init
(
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

  sns_ddf_status_e stat = SNS_DDF_SUCCESS;
  apds9950_dd_dev_s* apds9950_dd_dev_ptr;
  apds9950_dd_nv_s* nv_ptr;

  ALSPRX_MSG_1(ERROR, "==> sns_dd_apds9950_init %x\r\n", device_info->device_select);
  ALSPRX_MSG_2(ERROR, "==> sns_dd_apds9950_init sensor[0]=%d num_sensor=%d\r\n", sensors[0],*num_sensors);

  if ( (dd_handle_ptr == NULL)      || 
       (num_sensors == NULL) || 
       (sensors == NULL) ) 
  {  
    return SNS_DDF_EINVALID_PARAM;
  }

  apds9950_dd_dev_ptr = apds9950_dd_find_same_port_dev(&device_info->port_config);
  
  if (apds9950_dd_dev_ptr==NULL) 
  {
    if (sns_ddf_malloc((void **)&apds9950_dd_dev_ptr, sizeof(apds9950_dd_dev_s)) != SNS_DDF_SUCCESS) 
    {
      return SNS_DDF_ENOMEM;
    }

    memset(apds9950_dd_dev_ptr, 0, sizeof(apds9950_dd_dev_s));
    
    apds9950_dd_dev_ptr->next = NULL;
    apds9950_dd_head.next = apds9950_dd_dev_ptr;
        
#ifndef ALSPRX_OEM_BUILD  
    sns_ddf_get_port_id(&device_info->port_config, &apds9950_dd_dev_ptr->port_id);
#endif
    
    ALSPRX_MSG_0(ERROR, "sns_dd_apds9950_init FIRST INIT\r\n");
    
    // first init always load with PROXIMITY and AMBIENT
    
    /* Fill out supported sensor info */
    *num_sensors = APDS9950_DD_NUM_SENSOR_TYPES;
    *sensors = (sns_ddf_sensor_e *)apds9950_dd_first_sub_sensors;    
    
    apds9950_dd_dev_ptr->sub_dev[0].smgr_handle = smgr_handle;
    apds9950_dd_fill_sub_dev_slot(&apds9950_dd_dev_ptr->sub_dev[0], *num_sensors, *sensors);
    apds9950_dd_dev_ptr->sub_dev_cnt++;

    *dd_handle_ptr = (sns_ddf_handle_t)apds9950_dd_dev_ptr;
    
    /* open normal port */
    if ((stat = sns_ddf_open_port(&apds9950_dd_dev_ptr->port_handle, &device_info->port_config))!= SNS_DDF_SUCCESS) 
    {
      sns_ddf_mfree(apds9950_dd_dev_ptr); // free the memory
      return stat;
    }

#ifndef ALSPRX_OEM_BUILD  
    /* open additional port for zero byte write(ZBW) */
    device_info->port_config.bus_config.i2c->dev_type = SNS_DDF_I2C_DEVICE_DEFAULT;
    if ( (stat = sns_ddf_open_port(&apds9950_dd_dev_ptr->ZBW_port_handle, &device_info->port_config))!= SNS_DDF_SUCCESS ) 
    {
      sns_ddf_mfree(apds9950_dd_dev_ptr); // free the memory
      return stat;
    }
#endif
 
#ifndef QDSP6
    /* open additional port for zero byte write(ZBW) */
    device_info->port_config.bus_config.i2c->dev_type = SNS_DDF_I2C_DEVICE_DEFAULT;
    if ((stat = sns_ddf_open_port(&apds9950_dd_dev_ptr->ZBW_port_handle, &device_info->port_config))!= SNS_DDF_SUCCESS) 
    {
      sns_ddf_mfree(apds9950_dd_dev_ptr); // free the memory
      return stat;
    }
#endif
  
    apds9950_dd_dev_ptr->sub_dev[0].interrupt_gpio = device_info->first_gpio;
    
    if (apds9950_dd_dev_ptr->sub_dev[0].interrupt_gpio != 0xffff) 
    {
      apds9950_dd_dev_ptr->sub_dev[0].dri_enabled = TRUE;
    }
    else 
    {
      apds9950_dd_dev_ptr->sub_dev[0].dri_enabled = FALSE;
    }

#ifdef ALSPRX_OEM_BUILD  
    apds9950_dd_dev_ptr->sub_dev[0].dri_enabled = FALSE;  /* Avago's DDK platform doesn't support DRI */  
#endif
  
    /* timer for AMBIENT*/
    stat = sns_ddf_timer_init(&apds9950_dd_dev_ptr->als.timer, 
                            apds9950_dd_dev_ptr, 
                            &sns_dd_apds9950_driver_if, 
                            (void *)&als_timer, 
                            FALSE);
  
    if ( stat != SNS_DDF_SUCCESS ) 
    {
      sns_ddf_mfree(apds9950_dd_dev_ptr);
      return stat;
    }
  
    /* timer for PROXIMITY */
    stat = sns_ddf_timer_init(&apds9950_dd_dev_ptr->prx.timer, 
                            apds9950_dd_dev_ptr, 
                            &sns_dd_apds9950_driver_if, 
                            (void *)&prx_timer, 
                            FALSE);
  
    if ( stat != SNS_DDF_SUCCESS ) 
    {
      sns_ddf_mfree(apds9950_dd_dev_ptr);
      return stat;
    }
    
    /* Recall NV parameters if any */
    if (nv_params) 
    {
      bool data_from_registry = FALSE;

      if ((nv_params->status == SNS_DDF_SUCCESS) && (nv_params->data_len >= sizeof(apds9950_dd_nv_s))) 
      {
        nv_ptr = (apds9950_dd_nv_s *)nv_params->data;
        if ((nv_ptr->version_num != 0) && 
            (nv_ptr->id != 0) &&
            (nv_params->data_len != 0) ) 
        {
          // update value from NV parameters
          data_from_registry = TRUE;
          apds9950_dd_dev_ptr->common.nv_size = nv_params->data_len;
          apds9950_dd_dev_ptr->common.nv_db.version_num = nv_ptr->version_num;
          apds9950_dd_dev_ptr->common.nv_db.id = nv_ptr->id;
          
          apds9950_dd_dev_ptr->common.nv_db.visible_ratio = nv_ptr->visible_ratio;
          apds9950_dd_dev_ptr->common.nv_db.ir_ratio = nv_ptr->ir_ratio;
          apds9950_dd_dev_ptr->common.nv_db.als_factor = nv_ptr->als_factor;          

          if(nv_ptr->prx_factor == 0)
          {         
            // if prx_factor is 0 then prox is not calibrated.
            // use registry values as is.
            apds9950_dd_dev_ptr->common.nv_db.thresh_near = nv_ptr->thresh_near;
            apds9950_dd_dev_ptr->common.nv_db.thresh_far = nv_ptr->thresh_far;
            apds9950_dd_dev_ptr->common.nv_db.prx_factor = nv_ptr->prx_factor;
          }
          else
          {
            // update prx factor and thresh through this
            apds9950_dd_set_prx_factor(apds9950_dd_dev_ptr, SNS_DDF_SENSOR_PROXIMITY, nv_ptr->prx_factor);
          }
          
          /* RGBC factor */
          apds9950_dd_dev_ptr->common.nv_db.rgb_c_factor = nv_ptr->rgb_c_factor;
          apds9950_dd_dev_ptr->common.nv_db.rgb_r_factor = nv_ptr->rgb_r_factor;
          apds9950_dd_dev_ptr->common.nv_db.rgb_g_factor = nv_ptr->rgb_g_factor;
          apds9950_dd_dev_ptr->common.nv_db.rgb_b_factor = nv_ptr->rgb_b_factor;

          ALSPRX_MSG_1(MED, "prx crosstalk=%d\r\n", nv_ptr->prx_factor );
          
          // update als change pcnt
          apds9950_dd_set_als_change_pcnt(apds9950_dd_dev_ptr, nv_ptr->als_change_pcnt);
          ALSPRX_MSG_1(LOW, "als factor=%d\r\n", nv_ptr->als_factor );
        }
      }
    
      if (!data_from_registry) 
      {
        // default value of NV parameters
        apds9950_dd_dev_ptr->common.nv_size = 128;
        apds9950_dd_dev_ptr->common.nv_db.version_num = 1;
        apds9950_dd_dev_ptr->common.nv_db.id = 0x50|SNS_DDF_VENDOR_AVAGO;  /* 0001xxxx = APDS-9900, 0010xxxx = APDS-9930, 0011xxx = APDS-9190, 0100xxxx = APDS-9130, 0x0101xxxx = APDS-9950 */ 
        
        apds9950_dd_dev_ptr->common.nv_db.visible_ratio = 20; // 20% visible transmittance
        apds9950_dd_dev_ptr->common.nv_db.ir_ratio = 80; // 80% ir transmittance
        apds9950_dd_dev_ptr->common.nv_db.als_factor = 100; // factor of 1
        
        apds9950_dd_dev_ptr->common.nv_db.thresh_near = APDS9950_DD_PRX_NEAR_THRESHOLD;
        apds9950_dd_dev_ptr->common.nv_db.thresh_far = APDS9950_DD_PRX_FAR_THRESHOLD;
        apds9950_dd_dev_ptr->common.nv_db.prx_factor = 0;
  
		  /* RGBC factor */
        apds9950_dd_dev_ptr->common.nv_db.rgb_c_factor = 100;
        apds9950_dd_dev_ptr->common.nv_db.rgb_r_factor = 100;
        apds9950_dd_dev_ptr->common.nv_db.rgb_g_factor = 100;
        apds9950_dd_dev_ptr->common.nv_db.rgb_b_factor = 100;
		
        // update als change pcnt
        apds9950_dd_set_als_change_pcnt(apds9950_dd_dev_ptr, APDS9950_DD_DEFAULT_ALS_CHANGE_PCNT);
  
        sns_ddf_smgr_notify_event(apds9950_dd_dev_ptr->sub_dev[0].smgr_handle, SNS_DDF_SENSOR_PROXIMITY, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP); 
      }
    }
    
    /* Resets the driver */ /* reset parameters after NV */
    stat = sns_dd_apds9950_reset((sns_ddf_handle_t) apds9950_dd_dev_ptr);
    ALSPRX_MSG_0(ERROR, "sns_dd_apds9950_init FIRST INIT done\r\n");
  }
  else 
  {
    sns_ddf_handle_t  apds9950_dd_handle;
    uint32_t  sub_dev_idx;
    
    ALSPRX_MSG_0(ERROR, "sns_dd_apds9950_init SECOND INIT\r\n");
    
    sub_dev_idx = apds9950_find_free_sub_dev_slot(apds9950_dd_dev_ptr);
    
    if (sub_dev_idx == -1)
    {
      return SNS_DDF_ENOMEM;
    }
    else
    {
      // load with second sub-device 
      // second init load with RGB and CT_C
      /* Fill out supported sensor info */
      *num_sensors = APDS9950_DD_NUM_SENSOR_TYPES;
      *sensors = (sns_ddf_sensor_e *)apds9950_dd_second_sub_sensors;
      
      apds9950_dd_dev_ptr->sub_dev[sub_dev_idx].smgr_handle = smgr_handle;
      
      apds9950_dd_fill_sub_dev_slot(&apds9950_dd_dev_ptr->sub_dev[sub_dev_idx], *num_sensors, *sensors);
      apds9950_dd_dev_ptr->sub_dev_cnt++;
      
      apds9950_dd_handle = (sns_ddf_handle_t)((uint32_t)apds9950_dd_dev_ptr | sub_dev_idx);
      *dd_handle_ptr = apds9950_dd_handle;

      apds9950_dd_dev_ptr->sub_dev[sub_dev_idx].interrupt_gpio = device_info->first_gpio;
      
      if (apds9950_dd_dev_ptr->sub_dev[sub_dev_idx].interrupt_gpio != 0xffff) 
      {
        apds9950_dd_dev_ptr->sub_dev[sub_dev_idx].dri_enabled = TRUE;
      }
      else 
      {
        apds9950_dd_dev_ptr->sub_dev[sub_dev_idx].dri_enabled = FALSE;
      }

#ifdef ALSPRX_OEM_BUILD  
      apds9950_dd_dev_ptr->sub_dev[sub_dev_idx].dri_enabled = FALSE;  /* Avago's DDK platform doesn't support DRI */  
#endif      
      /* timer for RGB */
      stat = sns_ddf_timer_init(&apds9950_dd_dev_ptr->rgb.timer, 
                              apds9950_dd_handle, 
                              &sns_dd_apds9950_driver_if, 
                              (void *)&rgb_timer, 
                              FALSE);
    
      if ( stat != SNS_DDF_SUCCESS ) 
      {
        return stat;
      }
    
      /* timer for CT_C */
      stat = sns_ddf_timer_init(&apds9950_dd_dev_ptr->cct.timer, 
                              apds9950_dd_handle, 
                              &sns_dd_apds9950_driver_if, 
                              (void *)&cct_timer, 
                              FALSE);
    
      if ( stat != SNS_DDF_SUCCESS ) 
      {
        return stat;
      }
      
      sns_dd_apds9950_reset((sns_ddf_handle_t) apds9950_dd_handle);

      ALSPRX_MSG_0(ERROR, "sns_dd_apds9950_init SECOND INIT done\r\n");
    }
  }
  
  return stat;  
}

/*===========================================================================

  FUNCTION:   sns_dd_apds9950_set_attrib

===========================================================================*/
/*!
  @brief Sets an attribute of the ALS/Prx sensor
 
  @detail
  Called by SMGR to set certain device attributes that are
  programmable. Curently its the power mode, resolution and ODR.
 
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
static sns_ddf_status_e sns_dd_apds9950_set_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  void*                value
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  apds9950_dd_dev_s* dd_ptr;
  uint32_t sub_dev_idx;
#ifdef ALSPRX_OEM_BUILD
  uint32_t err;
#endif // ALSPRX_OEM_BUILD
  
  ALSPRX_MSG_3(HIGH, "set_attr - %d sensor=%d attr=%d", 300, sensor, attrib);

  if ( (dd_handle == NULL) || 
       (value == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }
  
  sub_dev_idx = (uint32_t)dd_handle & (APDS9950_DD_HANDLE_ALIGN-1);
  dd_ptr = (apds9950_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9950_DD_HANDLE_ALIGN-1)));
  
  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_POWER_STATE:
      /* set power mode */
      /* don't check if als or prx, either one will do */
      status = apds9950_dd_set_powerstate(dd_ptr,
                                          sub_dev_idx,
                                          sensor,
                                          *(sns_ddf_powerstate_e *)value,
                                          TRUE);
      break;

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
      /* set resolution */
      status = apds9950_dd_set_resolution(dd_ptr,
                                          sub_dev_idx,
                                          sensor,
                                          *(apds9950_dd_res_e *)value); 
      break;
      
    case SNS_DDF_ATTRIB_ODR:
      /* output data rate */
      status = apds9950_dd_set_odr(dd_ptr,
                                   sub_dev_idx,
                                   sensor, 
                                   *(uint32_t *)value);
      break;

#ifdef ALSPRX_OEM_BUILD
	  /* to test calibration from DDK */
  	case SNS_DDF_ATTRIB_LOWPASS:
		
		status =  sns_dd_apds9950_calibration(dd_ptr,
											  sensor,
											  SNS_DDF_TEST_OEM,
											  &err);
											
		break;
#endif

    default:
      return SNS_DDF_EINVALID_PARAM;
  }

  ALSPRX_MSG_3(HIGH, "set_attr - sensor=%d attr=%d result=%d - end", sensor, attrib, status);

  return(status);
}

/*===========================================================================

FUNCTION      apds9950_dd_common_request_data
  
DESCRIPTION   As a async driver, this driver needs preparing a sensor data. 
              This function calls an appropriate preparing function according to
              the data type.

DEPENDENCIES  None

RETURN VALUE  FALSE if successful, TRUE otherwise

SIDE EFFECT   None 
===========================================================================*/
bool apds9950_dd_common_request_data
(
  apds9950_dd_dev_s* dd_ptr,
  uint32_t sub_dev_idx,
  sns_ddf_sensor_e sensor
)
{
  uint8_t i2c_data = 0x0;
  uint8_t enable_reg;
  uint8_t bytes_w;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
    
  ALSPRX_MSG_2(MED, "apds9950_dd_common_request_data - sensor=%d idx=%d", sensor, sub_dev_idx);

  /* make sure device is in the right state */  
  if ( dd_ptr->sub_dev[sub_dev_idx].state != APDS9950_DD_DEV_CONFIGURED ||
      dd_ptr->sub_dev[sub_dev_idx].pwr_mode != SNS_DDF_POWERSTATE_ACTIVE )
  {
    /* device state is stopped OR invalid */
    /* make sure device is in the right state */
    ALSPRX_MSG_0(ERROR, "apds9950_dd_common_request_data - device not configured ");
    apds9950_dd_set_powerstate(dd_ptr, 0, SNS_DDF_SENSOR__ALL, 
                                    SNS_DDF_POWERSTATE_ACTIVE, FALSE);    
  }
    
  status = sns_ddf_read_port(dd_ptr->port_handle,
                             APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                             &i2c_data, 1, &bytes_w);
  
  CHECK_STATUS(status);

  enable_reg = i2c_data;

  ALSPRX_MSG_1(HIGH, "before change enable_reg=%X", enable_reg);
  
  /* Request data from device */
  if (sensor == SNS_DDF_SENSOR_AMBIENT)
  {
    dd_ptr->als.polling_data = TRUE;
   
    if (dd_ptr->als.enable == FALSE) 
    {
      i2c_data = enable_reg|APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE;
  
      dd_ptr->als.enable = TRUE;

      sns_ddf_write_port(dd_ptr->port_handle,
                         APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                         (uint8_t*)&i2c_data, 
                         1, 
                         &bytes_w);
    }

    apds9950_dd_req_data(dd_ptr, sub_dev_idx, sensor);
  }
  else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
  {
    dd_ptr->prx.polling_data = TRUE;

    if (dd_ptr->prx.enable == FALSE) 
    {
      /* need to make use of ALS function as supplementary support for Prox */
      i2c_data = enable_reg|APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE|APDS9950_DD_PRX_ENABLE;

      dd_ptr->prx.enable = TRUE;
            
      ALSPRX_MSG_1(HIGH, "before change enable_reg=%X", i2c_data);

	  sns_ddf_write_port(dd_ptr->port_handle,
                       APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                       (uint8_t*)&i2c_data, 
                       1, 
                       &bytes_w);
    }    
    
    apds9950_dd_req_data (dd_ptr, sub_dev_idx, SNS_DDF_SENSOR_PROXIMITY);
  }
  else if (sensor == SNS_DDF_SENSOR_RGB || sensor == SNS_DDF_SENSOR_CT_C)
  {
    if (sensor == SNS_DDF_SENSOR_RGB)
    {
      dd_ptr->rgb.polling_data = TRUE;
    }
    else
    {
      dd_ptr->cct.polling_data = TRUE;      
    }
   
    if ((dd_ptr->rgb.enable == FALSE) || (dd_ptr->cct.enable == FALSE)) 
    {
      i2c_data = enable_reg|APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE;
   
      if (sensor == SNS_DDF_SENSOR_RGB)
      {
        dd_ptr->rgb.enable = TRUE;
      }
      else
      {
        dd_ptr->cct.enable = TRUE;
      }

      sns_ddf_write_port(dd_ptr->port_handle,
                         APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                         (uint8_t*)&i2c_data, 
                         1, 
                         &bytes_w); 
    }

    apds9950_dd_req_data(dd_ptr, sub_dev_idx, sensor);
  }

  return FALSE;
}

/*===========================================================================

  FUNCTION:   sns_dd_apds9950_get_data

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
static sns_ddf_status_e sns_dd_apds9950_get_data
(
  sns_ddf_handle_t        dd_handle,
  sns_ddf_sensor_e        sensors[],
  uint32_t                num_sensors,
  sns_ddf_memhandler_s*   memhandler,
  sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
)
{
  sns_ddf_status_e status = SNS_DDF_PENDING;
  uint8_t i;
  apds9950_dd_dev_s* dd_ptr;
  uint32_t sub_dev_idx;

  ALSPRX_MSG_3(HIGH, "polling_data - %d sensor=%d num=%d", 
               200, sensors[0], num_sensors);

  if ( (dd_handle == NULL) || (sensors == NULL) ||
       (num_sensors < 1) || (num_sensors > 2) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }
  
  sub_dev_idx = (uint32_t)dd_handle & (APDS9950_DD_HANDLE_ALIGN-1);
  dd_ptr = (apds9950_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9950_DD_HANDLE_ALIGN-1)));

  ALSPRX_MSG_1(MED, "polling_data - sub dev ID=%d", sub_dev_idx);
  
#ifndef ALSPRX_OEM_BUILD
  if (EnableI2C == FALSE) /* Avago's DDK platform doesn't support this */ 
  {
      static uint8_t toggle[2] = {0};
      uint8_t num_samples=2;
      sns_ddf_sensor_data_s *data_ptr;
      sns_ddf_sensor_sample_s *sample_ptr;

     *data = sns_ddf_memhandler_malloc(
         memhandler, num_sensors * sizeof(sns_ddf_sensor_data_s));

     if(*data == NULL)
         return SNS_DDF_ENOMEM;

     for (i=0; i < num_sensors; i++)
     {
       data_ptr = &((*data)[i]);

       if( (sample_ptr = sns_ddf_memhandler_malloc(memhandler,
                           (3*sizeof(sns_ddf_sensor_sample_s)))) == NULL)
       {
         return SNS_DDF_ENOMEM;
       }

       sns_ddf_delay(50);

       if (sensors[i] == SNS_DDF_SENSOR_PROXIMITY)
       {
         data_ptr->sensor = SNS_DDF_SENSOR_PROXIMITY;
         if (toggle[i])
         {
           num_samples = 2;
           sample_ptr[0].sample = FX_CONV_Q16(0, 0); //far = 0; near = 1
           sample_ptr[0].status = SNS_DDF_SUCCESS;
           sample_ptr[1].sample = 100; //ADC
           sample_ptr[1].status = SNS_DDF_SUCCESS;
         }
         else
         {
           num_samples = 2;
           sample_ptr[0].sample = FX_CONV_Q16(1, 0); //far = 0; near = 1
           sample_ptr[0].status = SNS_DDF_SUCCESS;
           sample_ptr[1].sample = 500; //ADC
           sample_ptr[1].status = SNS_DDF_SUCCESS;
         }
       }
       else if (sensors[i] == SNS_DDF_SENSOR_AMBIENT)
       {
         data_ptr->sensor = SNS_DDF_SENSOR_AMBIENT;
         if (toggle[i])
         {
           num_samples = 2;
           sample_ptr[0].sample = FX_CONV_Q16(20, 0); //20 lux
           sample_ptr[0].status = SNS_DDF_SUCCESS;
           sample_ptr[1].sample = 60; //ADC
           sample_ptr[1].status = SNS_DDF_SUCCESS;
         }
         else
         {
           num_samples = 2;
           sample_ptr[0].sample = FX_CONV_Q16(100, 0); //100 lux
           sample_ptr[0].status = SNS_DDF_SUCCESS;
           sample_ptr[1].sample = 300; //ADC
           sample_ptr[1].status = SNS_DDF_SUCCESS;
         }
       }
       else if (sensors[i] == SNS_DDF_SENSOR_RGB) {
         data_ptr->sensor = SNS_DDF_SENSOR_RGB;
         if (toggle[i])
         {
           num_samples = 3;
           sample_ptr[0].sample = 60; //R
           sample_ptr[0].status = SNS_DDF_SUCCESS;
           sample_ptr[1].sample = 60; //G
           sample_ptr[1].status = SNS_DDF_SUCCESS;
           sample_ptr[2].sample = 60; //B
           sample_ptr[2].status = SNS_DDF_SUCCESS;
         }
         else
         {
           num_samples = 3;
           sample_ptr[0].sample = 150; //R
           sample_ptr[0].status = SNS_DDF_SUCCESS;
           sample_ptr[1].sample = 150; //G
           sample_ptr[1].status = SNS_DDF_SUCCESS;
           sample_ptr[2].sample = 150; //B
           sample_ptr[2].status = SNS_DDF_SUCCESS;
         }         
       }
       else {
         data_ptr->sensor = SNS_DDF_SENSOR_CT_C;
         if (toggle[i])
         {
           num_samples = 2;
           sample_ptr[0].sample = 100; //cdata
           sample_ptr[0].status = SNS_DDF_SUCCESS;
           sample_ptr[1].sample = FX_CONV_Q16(2600, 0); //2600K
           sample_ptr[1].status = SNS_DDF_SUCCESS;
         }
         else
         {
           num_samples = 2;
           sample_ptr[0].sample = 300; // cdata
           sample_ptr[0].status = SNS_DDF_SUCCESS;
           sample_ptr[1].sample = FX_CONV_Q16(2600, 0); //2600K
           sample_ptr[1].status = SNS_DDF_SUCCESS;
         }                  
       }
       
       data_ptr->status = SNS_DDF_SUCCESS;
       data_ptr->num_samples = num_samples;
       data_ptr->timestamp = sns_ddf_get_timestamp();
       data_ptr->samples = sample_ptr;
       toggle[i] = ~toggle[i];
     }

     return SNS_DDF_SUCCESS;
  }
#endif
  
  /* Look at all the requests */
  for (i=0; i < num_sensors; i++) {
    if ( sensors[i] != SNS_DDF_SENSOR_AMBIENT &&
         sensors[i] != SNS_DDF_SENSOR_PROXIMITY &&
         sensors[i] != SNS_DDF_SENSOR_RGB &&
         sensors[i] != SNS_DDF_SENSOR_CT_C ) {
      status = SNS_DDF_EINVALID_PARAM;
      break;
    }

    if ( apds9950_dd_common_request_data(dd_ptr, sub_dev_idx, sensors[i]) == TRUE ) {
      status = SNS_DDF_EFAIL;
      break;
    }
  }
  
  return status;
}

/*==============================================================================

FUNCTION      apds9950_dd_prx_query
  
DESCRIPTION   Called by sns_dd_apds9950_get_attrib to get an attribute value for proximitydistance data type 

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e apds9950_dd_prx_query
(
  apds9950_dd_dev_s* dd_ptr,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  uint16_t* thresh_ptr;
  uint16_t* accu_ptr;
  sns_ddf_power_info_s* power_ptr;
  
  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if( (*value = sns_ddf_memhandler_malloc(memhandler, 
                      sizeof(sns_ddf_range_s)))  == NULL)
      {
          return SNS_DDF_ENOMEM;
      }
      device_ranges = *value;
      *num_elems = 1;       
      device_ranges->min = FX_FLTTOFIX_Q16(0.00); /* From 0 mm*/
      device_ranges->max = FX_FLTTOFIX_Q16(0.05);  /* to 50 mm*/
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      sns_ddf_resolution_adc_s* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      resp->bit_len = 10;
      resp->max_freq = 20;
      *value = resp;
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
      *resp = APDS9950_DD_PRX_RES;
       *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_POWER_INFO:
      if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      power_ptr->active_current = APDS9950_DD_PRX_PWR;
      power_ptr->lowpower_current = APDS9950_DD_ALS_LO_PWR;
      *(sns_ddf_power_info_s **)value = power_ptr;
      break;

    case SNS_DDF_ATTRIB_ACCURACY:
      if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *accu_ptr = APDS9950_DD_PRX_ACCURACY;
      *(uint16_t **)value = accu_ptr;
      break;

    case SNS_DDF_ATTRIB_THRESHOLD:
      if ( (thresh_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *thresh_ptr = dd_ptr->prx.thresh_near;
      *(uint16_t **)value = thresh_ptr;
      break;

    case SNS_DDF_ATTRIB_ODR:
    {
      uint32_t* odr_ptr;

      if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *odr_ptr = dd_ptr->common.prx_odr;
      *(uint32_t **)value = odr_ptr;
      ALSPRX_MSG_2(LOW, "prx_query - %d odr=%d", 100, *odr_ptr);
      break;
    }

    default:
      return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      apds9950_dd_als_query
  
DESCRIPTION   Called by sns_dd_apds9950_get_attrib to get an attribute value for ALS(light) data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e apds9950_dd_als_query
(
  apds9950_dd_dev_s* dd_ptr,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  sns_ddf_power_info_s* power_ptr;
  uint16_t* accu_ptr;

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if( (*value = sns_ddf_memhandler_malloc(memhandler, 
                      sizeof(sns_ddf_range_s)))  == NULL) {
          return SNS_DDF_ENOMEM;
      }
      
      device_ranges = *value;
      *num_elems = 1;       
      device_ranges->min = FX_FLTTOFIX_Q16(0.0); 
      device_ranges->max = FX_FLTTOFIX_Q16(10000.0);
      break;
    }
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      sns_ddf_resolution_adc_s* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, 3 * sizeof(sns_ddf_resolution_adc_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      
      *num_elems = 3;
      
      resp[0].bit_len = sns_dd_als_adc_bit_tb[APDS9950_DD_RES_14BIT];
      resp[0].max_freq = 20;  
      resp[1].bit_len = sns_dd_als_adc_bit_tb[APDS9950_DD_RES_15BIT];
      resp[1].max_freq = 10;  
      resp[2].bit_len = sns_dd_als_adc_bit_tb[APDS9950_DD_RES_16BIT];
      resp[2].max_freq = 5;  
      
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;

      *resp = APDS9950_DD_ALS_RES;
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_POWER_INFO:
      if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      power_ptr->active_current = APDS9950_DD_ALS_PWR;
      power_ptr->lowpower_current = APDS9950_DD_ALS_LO_PWR;
      *(sns_ddf_power_info_s **)value = power_ptr;
      break;

    case SNS_DDF_ATTRIB_ACCURACY:
      if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
     *accu_ptr = APDS9950_DD_ALS_ACCURACY;
      *(uint16_t **)value = accu_ptr;
      break;

    case SNS_DDF_ATTRIB_ODR:
    {
      uint32_t* odr_ptr;

      if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *odr_ptr = dd_ptr->common.als_odr;
      *(uint32_t **)value = odr_ptr;
      ALSPRX_MSG_2(LOW, "als_query - %d odr=%d", 100, *odr_ptr);
      break;
    }

    default:
      return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      apds9950_dd_rgb_query
  
DESCRIPTION   Called by sns_dd_apds9950_get_attrib to get an attribute value for RGB data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e apds9950_dd_rgb_query
(
  apds9950_dd_dev_s* dd_ptr,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  sns_ddf_power_info_s* power_ptr;
  uint16_t* accu_ptr;

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if( (*value = sns_ddf_memhandler_malloc(memhandler, 
                      sizeof(sns_ddf_range_s)))  == NULL) {
          return SNS_DDF_ENOMEM;
      }
      
      device_ranges = *value;
      *num_elems = 1;       
      device_ranges->min = FX_FLTTOFIX_Q16(0.0); // minimum ADC count
      device_ranges->max = FX_FLTTOFIX_Q16(43007.0);  // maximum ADC count
      break;
    }
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      sns_ddf_resolution_adc_s* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, 3 * sizeof(sns_ddf_resolution_adc_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      
      *num_elems = 3;
      
      resp[0].bit_len = sns_dd_als_adc_bit_tb[APDS9950_DD_RES_14BIT];
      resp[0].max_freq = 20;  
      resp[1].bit_len = sns_dd_als_adc_bit_tb[APDS9950_DD_RES_15BIT];
      resp[1].max_freq = 10;  
      resp[2].bit_len = sns_dd_als_adc_bit_tb[APDS9950_DD_RES_16BIT];
      resp[2].max_freq = 5;  
      
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;

      *resp = APDS9950_DD_RGB_RES;
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_POWER_INFO:
      if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      power_ptr->active_current = APDS9950_DD_RGB_PWR;
      power_ptr->lowpower_current = APDS9950_DD_RGB_LO_PWR;
      *(sns_ddf_power_info_s **)value = power_ptr;
      break;

    case SNS_DDF_ATTRIB_ACCURACY:
      if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *accu_ptr = APDS9950_DD_ALS_ACCURACY;
      *(uint16_t **)value = accu_ptr;
      break;

    case SNS_DDF_ATTRIB_ODR:
    {
      uint32_t* odr_ptr;

      if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *odr_ptr = dd_ptr->common.rgb_odr;
      *(uint32_t **)value = odr_ptr;
      ALSPRX_MSG_2(LOW, "rgb_query - %d odr=%d", 100, *odr_ptr);
      break;
    }

    default:
      return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      apds9950_dd_cct_query
  
DESCRIPTION   Called by sns_dd_apds9950_get_attrib to get an attribute value for CT_C data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None 

==============================================================================*/
sns_ddf_status_e apds9950_dd_cct_query
(
  apds9950_dd_dev_s* dd_ptr,
  sns_ddf_memhandler_s*  memhandler,
  sns_ddf_attribute_e    attrib,
  void**                 value,
  uint32_t*              num_elems
)
{
  sns_ddf_power_info_s* power_ptr;
  uint16_t* accu_ptr;

  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_RANGE:
    {
      sns_ddf_range_s *device_ranges;
      if( (*value = sns_ddf_memhandler_malloc(memhandler, 
                      sizeof(sns_ddf_range_s)))  == NULL) {
          return SNS_DDF_ENOMEM;
      }
      
      device_ranges = *value;
      *num_elems = 1;       
      device_ranges->min = FX_FLTTOFIX_Q16(0.0);
      device_ranges->max = FX_FLTTOFIX_Q16(10000.0);
      break;
    }
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
    {
      sns_ddf_resolution_adc_s* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, 3 * sizeof(sns_ddf_resolution_adc_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      
      *num_elems = 3;
      
      resp[0].bit_len = sns_dd_als_adc_bit_tb[APDS9950_DD_RES_14BIT];
      resp[0].max_freq = 20;  
      resp[1].bit_len = sns_dd_als_adc_bit_tb[APDS9950_DD_RES_15BIT];
      resp[1].max_freq = 10;  
      resp[2].bit_len = sns_dd_als_adc_bit_tb[APDS9950_DD_RES_16BIT];
      resp[2].max_freq = 5;  
      
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_RESOLUTION:
    {
      sns_ddf_resolution_t* resp;
      if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;

      *resp = APDS9950_DD_CCT_RES;
      *value = resp;
      break;
    }

    case SNS_DDF_ATTRIB_POWER_INFO:
      if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      power_ptr->active_current = APDS9950_DD_CCT_PWR;
      power_ptr->lowpower_current = APDS9950_DD_CCT_LO_PWR;
      *(sns_ddf_power_info_s **)value = power_ptr;
      break;

    case SNS_DDF_ATTRIB_ACCURACY:
      if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;
      *accu_ptr = APDS9950_DD_CCT_ACCURACY;
      *(uint16_t **)value = accu_ptr;
      break;


    case SNS_DDF_ATTRIB_ODR:
    {
      uint32_t* odr_ptr;

      if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      *num_elems = 1;      
	  *odr_ptr = dd_ptr->common.cct_odr;
      *(uint32_t **)value = odr_ptr;
      ALSPRX_MSG_2(MED, "cct_query - %d odr=%d", 100, *odr_ptr);
      break;
    }

    default:
      return SNS_DDF_EINVALID_ATTR;
  }
  return SNS_DDF_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_dd_apds9950_get_attrib

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.
 
  @detail
  Returns the requested attribute
 
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

static sns_ddf_status_e sns_dd_apds9950_get_attrib
(
  sns_ddf_handle_t     dd_handle,
  sns_ddf_sensor_e     sensor,
  sns_ddf_attribute_e  attrib,
  sns_ddf_memhandler_s* memhandler,
  void**                value,
  uint32_t*             num_elems
)
{
  apds9950_dd_dev_s* dd_ptr;
#ifdef APDS9950_DEBUG
  uint32_t sub_dev_idx;
#endif // APDS9950_DEBUG
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  bool generic_attrib = FALSE;
  sns_ddf_driver_info_s *driver_info_ptr;
  sns_ddf_driver_info_s driver_info_l = {
    "APDS-9950 Proximity & Ambient",  /* name */
    1                                     /* version */
  };
  sns_ddf_driver_info_s driver_info_2 = {
    "APDS-9950 RGB & CCT",          /* name */
    1                               /* version */
  };
  sns_ddf_device_info_s *device_info_ptr;
  sns_ddf_device_info_s device_info_l = {
    "Proximity & Ambient",        /* name */
    "Avago",                      /* vendor */
    "APDS-9950",                  /* model */
    1                             /* version */
  };
  sns_ddf_device_info_s device_info_2 = {
    "RGB & CCT",                  /* name */
    "Avago",                      /* vendor */
    "APDS-9950",                  /* model */
    1                             /* version */
  };
  sns_ddf_registry_group_s *reg_group_ptr;
  uint8_t *reg_group_data_ptr;

  if ( (dd_handle == NULL)      || 
       (memhandler == NULL) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }
 
  ALSPRX_MSG_2(HIGH, "get_attr - sensor=%d attr=%d", sensor, attrib);

  dd_ptr = (apds9950_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9950_DD_HANDLE_ALIGN-1)));  

#ifdef APDS9950_DEBUG
  sub_dev_idx = (uint32_t)dd_handle & (APDS9950_DD_HANDLE_ALIGN-1);
  ALSPRX_MSG_1(MED, "get_attr - gpio=%x", dd_ptr->sub_dev[sub_dev_idx].interrupt_gpio);
#endif // APDS9950_DEBUG
 
  /* check first if the query is for generic attributes */
  switch ( attrib )
  {
    case SNS_DDF_ATTRIB_DRIVER_INFO:
      if ( (driver_info_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_driver_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }

      if (sensor == SNS_DDF_SENSOR_AMBIENT || sensor == SNS_DDF_SENSOR_PROXIMITY) {
        *driver_info_ptr = driver_info_l; /* Is memcopy needed because of strings */
      }
      else if (sensor == SNS_DDF_SENSOR_RGB || sensor == SNS_DDF_SENSOR_CT_C) {
        *driver_info_ptr = driver_info_2; /* Is memcopy needed because of strings */        
      }
      else {
         return SNS_DDF_EINVALID_ATTR;       
      }
      
      *(sns_ddf_driver_info_s**)value = driver_info_ptr;
      *num_elems = 1;
      generic_attrib = TRUE;
      break;

    case SNS_DDF_ATTRIB_DEVICE_INFO:
      if ( (device_info_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_device_info_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
           
      if (sensor == SNS_DDF_SENSOR_AMBIENT || sensor == SNS_DDF_SENSOR_PROXIMITY) {
        *device_info_ptr = device_info_l;
      }
      else if (sensor == SNS_DDF_SENSOR_RGB || sensor == SNS_DDF_SENSOR_CT_C) {
        *device_info_ptr = device_info_2;        
      }
      else {
         return SNS_DDF_EINVALID_ATTR;       
      }
      
      *(sns_ddf_device_info_s**)value = device_info_ptr; /* Is memcopy needed instead? */
      *num_elems = 1;
      generic_attrib = TRUE;
      break;
      
    case SNS_DDF_ATTRIB_REGISTRY_GROUP:
      if ( (reg_group_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_registry_group_s))) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
                        
      if ( (reg_group_data_ptr = sns_ddf_memhandler_malloc(memhandler, dd_ptr->common.nv_size)) == NULL )
      {
        return SNS_DDF_ENOMEM;
      }
      
#ifndef ALSPRX_OEM_BUILD      
      SNS_OS_MEMCOPY(reg_group_data_ptr, &dd_ptr->common.nv_db, sizeof(apds9950_dd_nv_s));
#else
      SNS_OS_MEMSCPY(reg_group_data_ptr, sizeof(apds9950_dd_nv_s), &dd_ptr->common.nv_db, sizeof(apds9950_dd_nv_s));
#endif
      
      reg_group_ptr->group_data = reg_group_data_ptr;
      reg_group_ptr->size = dd_ptr->common.nv_size;    
      *(sns_ddf_registry_group_s**)value = reg_group_ptr;

#ifndef ALSPRX_OEM_BUILD      
      ALSPRX_MSG_2(LOW, "get_attrib - size=%d data=%d",
                   reg_group_ptr->size, (uint8_t) *reg_group_ptr->group_data);
      
#endif
      
      ALSPRX_MSG_3(LOW, "get_attrib - ratio=%d factor=%d id=%d",                   
                   dd_ptr->common.nv_db.visible_ratio,                   
                   dd_ptr->common.nv_db.als_factor,                   
                   dd_ptr->common.nv_db.id );

      *num_elems = 1;
      generic_attrib = TRUE;      
      break;      

    default:
      /* do nothing */
      break;
  }

  /* The query is not for generic attribute but is for specific data type */
  if ( generic_attrib != TRUE )
  {
    switch ( sensor )
    {
      case SNS_DDF_SENSOR_AMBIENT:
        status = apds9950_dd_als_query(dd_ptr, memhandler, attrib, value, num_elems);
        break;
        
      case SNS_DDF_SENSOR_RGB:
        status = apds9950_dd_rgb_query(dd_ptr, memhandler, attrib, value, num_elems);
        break;
      case SNS_DDF_SENSOR_CT_C:
        status = apds9950_dd_cct_query(dd_ptr, memhandler, attrib, value, num_elems);
		break;

      case SNS_DDF_SENSOR_PROXIMITY:
        status = apds9950_dd_prx_query(dd_ptr, memhandler, attrib, value, num_elems);
        break;

      default:
        status = SNS_DDF_EINVALID_ATTR;
    }
  }

  ALSPRX_MSG_3(HIGH, "get_attr - sensor=%d attr=%d result=%d- end", sensor, attrib, status);

  return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_apds9950_calibration

===========================================================================*/
/*!
  @ Called by the SMGR to run calibration test for ALS or PRX.
 
  @detail
  Returns the cal_factor in error, to be stored in NV memory
 
  @param[in]  handle      Handle to a driver instance.
  @param[in]  sensor Sensor whose attribute is to be retrieved.
  @param[in] test         Test ID used to decide what to test.
  @param[out] err         prx_factor or als_factor.

  @return
    Success if no error. Otherwise a specific error code is returned.
*/
/*=========================================================================*/

static sns_ddf_status_e sns_dd_apds9950_calibration
(
  sns_ddf_handle_t dd_handle,
  sns_ddf_sensor_e sensor,
  sns_ddf_test_e test,
  uint32_t* err
)
{
  apds9950_dd_dev_s* dd_ptr = (apds9950_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9950_DD_HANDLE_ALIGN-1)));
  sns_ddf_status_e status = SNS_DDF_PENDING;
#ifdef FEATURE_APDS99XX_INT_PIN_CONFIG
  sns_ddf_status_e init_status = SNS_DDF_EFAIL;
#endif
  uint8_t   i2c_data = 0x0;
  uint8_t   bytes_w;

  if (dd_handle == NULL)
  {
    return SNS_DDF_ENOMEM;
  }
  
  ALSPRX_MSG_2(HIGH, "self_test - sensor %d test %d\r\n", sensor, test);
  /* check what is the test type */
  
  switch ( test )
  {
    case SNS_DDF_TEST_OEM:

      if (sensor == SNS_DDF_SENSOR_PROXIMITY)
	  {
#ifdef FEATURE_APDS99XX_INT_PIN_CONFIG
          init_status = sns_dd_isl29028_int_disable();
          ALSPRX_MSG_2(LOW, "calib - %d prox dis stat=%d", 0, init_status);
#endif        
          apds9950_dd_set_powerstate(dd_ptr, 0, SNS_DDF_SENSOR__ALL, 
                                     SNS_DDF_POWERSTATE_ACTIVE, FALSE);

          if (dd_ptr->prx.enable == FALSE) {

            dd_ptr->prx.enable = TRUE;
            i2c_data = APDS9950_DD_PWR_ON|APDS9950_DD_PRX_ENABLE|APDS9950_DD_ALS_ENABLE;	// kk 26-Oct-2013 DO NOT use PRX and WAIT enable together
            
            sns_ddf_write_port(dd_ptr->port_handle,
                             APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                             (uint8_t*)&i2c_data, 
                             1, 
                             &bytes_w);
  
          }
          else if (dd_ptr->prx.polling_data == TRUE) {
            return SNS_DDF_EDEVICE_BUSY; 
          }
    
          dd_ptr->prx.calibration = TRUE;
          dd_ptr->prx.calibration_loop = 0;
          dd_ptr->prx.calibration_pdata = 0;
          
          ALSPRX_MSG_1(LOW, "prx data %d %d\r\n", 100);

          apds9950_dd_req_data (dd_ptr, 0, sensor); 
      }
      else if (sensor == SNS_DDF_SENSOR_AMBIENT ||
			   sensor == SNS_DDF_SENSOR_RGB)
	  {
#ifdef FEATURE_APDS99XX_INT_PIN_CONFIG
        init_status = sns_dd_isl29028_int_disable();
        SNS_PRINTF_LOW_2(0, "", 0, init_status); 
#endif   

		dd_ptr->common.calibrating_sensor = sensor;
		dd_ptr->common.calibrating_sub_dev = (uint32_t)dd_handle & (APDS9950_DD_HANDLE_ALIGN-1);
		
		sensor = SNS_DDF_SENSOR_AMBIENT;

        apds9950_dd_set_powerstate(dd_ptr, 0, SNS_DDF_SENSOR__ALL, 
                                   SNS_DDF_POWERSTATE_ACTIVE, FALSE);

        if (dd_ptr->als.enable == FALSE)
		{
        
          dd_ptr->als.enable = TRUE;
          i2c_data = APDS9950_DD_PWR_ON|APDS9950_DD_ALS_ENABLE;
            
          sns_ddf_write_port(dd_ptr->port_handle,
                           APDS9950_DD_CMD_BYTE|APDS9950_DD_ENABLE_ADDR,
                           (uint8_t*)&i2c_data, 
                           1, 
                           &bytes_w);
        }
        else if (dd_ptr->als.polling_data == TRUE)
		{
          return SNS_DDF_EDEVICE_BUSY;          
        }

        dd_ptr->als.calibration = TRUE;
        ALSPRX_MSG_0(MED, "Requesting data for calibration");          
        apds9950_dd_req_data (dd_ptr, 0, sensor);                        
      }
      else
      {
        return SNS_DDF_EINVALID_PARAM;
      }

      break;

    default:
      /* do nothing */
      return SNS_DDF_EINVALID_TEST;
  }
  ALSPRX_MSG_1(HIGH, "self_test exit status=%d", status);
  return status;
}

/*===========================================================================

  FUNCTION:   sns_dd_apds9950_probe

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
 * @param[out] sensor Array of sensor types supported, with num_sensor
 *                         elements. Allocated by this function.
 *
 * @return SNS_DDF_SUCCESS if the part was probed function completed, even
 *         if no device was found (in which case num_sensors will be set to
 *         0).
 */
static sns_ddf_status_e sns_dd_apds9950_probe
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

  /* Read & Verify Device ID */
  status = sns_ddf_read_port( port_handle,
                              APDS9950_DD_CMD_BYTE|APDS9950_DD_ID_ADDR,
                              &i2c_buff,
                              1,
                              &bytes_read );
                             
  if (status != SNS_DDF_SUCCESS || bytes_read != 1)  
  {
    sns_ddf_close_port( port_handle );
    return status;
  }
  
  if (i2c_buff != 0x69) { /* APDS-9950 */
    /* Incorrect value. Return now with nothing detected */
    sns_ddf_close_port( port_handle );
    return SNS_DDF_SUCCESS;
  }
   
  /* Registers registers are correct. This is APDS-9950 */
  *num_sensors = APDS9950_DD_NUM_SENSOR_TYPES;
  *sensors = sns_ddf_memhandler_malloc( memhandler,
                                          sizeof(sns_ddf_sensor_e) * *num_sensors );
  if (first_probe == FALSE) 
  {    
    if ( *sensors != NULL )
    {
      (*sensors)[0] = SNS_DDF_SENSOR_PROXIMITY;
      (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
            
      status = SNS_DDF_SUCCESS;
      first_probe = TRUE;
    } 
    else 
    {
      status = SNS_DDF_ENOMEM;
    }
  }
  else 
  {
    if ( *sensors != NULL )  
    {
      (*sensors)[0] = SNS_DDF_SENSOR_RGB;
      (*sensors)[1] = SNS_DDF_SENSOR_CT_C;
            
      status = SNS_DDF_SUCCESS;
    } 
    else 
    {
      status = SNS_DDF_ENOMEM;
    }    
  }
    
  sns_ddf_close_port( port_handle );
  return status;
}
