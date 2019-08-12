/******************** (C) COPYRIGHT 2014 STMicroelectronics ********************
*
* File Name         : sns_dd_press_lps25h.h
* Authors           : Jianjian Huo
* Version           : V 1.0.6
* Date              : 07/27/2014
* Description       : LPS25H pressure sensor driver header file
*
********************************************************************************
* Copyright (c) 2014, STMicroelectronics.
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
*******************************************************************************/

/*------------------------------------------------------------------------
 Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved
 Qualcomm Technologies Proprietary and Confidential
------------------------------------------------------------------------*/
  
#ifndef __LPS25H__
#define __LPS25H__

#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include <string.h>

#include "fixed_point.h"

#define STM_LPS25H_PRESS_RANGE_MIN    FX_CONV_Q16(260,0)
#define STM_LPS25H_PRESS_RANGE_MAX    FX_CONV_Q16(1260,0)
#define STM_LPS25H_TEMP_RANGE_MIN     FX_CONV_Q16(0,0)
#define STM_LPS25H_TEMP_RANGE_MAX     FX_CONV_Q16(80,0)

#define STM_LPS25H_MAX_RES_PRESS      FX_FLTTOFIX_Q16(0.0002)
#define STM_LPS25H_MAX_RES_TEMP       FX_FLTTOFIX_Q16(0.002)

               
// Enable this flag to build in HD22 build
//#define BUILD_DB8074_HD22
// Enable this flag to view driver debug messages
//#define LPS25H_DEBUG
#ifdef LPS25H_DEBUG
#ifdef BUILD_DB8074_HD22
// build for DragonBoard+HD22
#include "sns_log_types_public.h"
#include "sns_log_api_public.h"
#define SNS_LOG_CONVERTED_SENSOR_DATA	SNS_LOG_CONVERTED_SENSOR_DATA_PUBLIC
#else // BUILD_DB8074_HD22
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#endif // BUILD_DB8074_HD22
#define LPS25H_MSG_0(level,msg)             UMSG(MSG_SSID_SNS,DBG_##level##_PRIO, "LPS25H - " msg)
#define LPS25H_MSG_1(level,msg,p1)          UMSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "LPS25H - " msg,p1)
#define LPS25H_MSG_2(level,msg,p1,p2)       UMSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "LPS25H - " msg,p1,p2)
#define LPS25H_MSG_3(level,msg,p1,p2,p3)    UMSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "LPS25H - " msg,p1,p2,p3)
#define LPS25H_MSG_4(level,msg,p1,p2,p3,p4) UMSG_4(MSG_SSID_SNS,DBG_##level##_PRIO, "LPS25H - " msg,p1,p2,p3,p4)
#else
// Regular Builds
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#define LPS25H_MSG_0(level,msg)
#define LPS25H_MSG_1(level,msg,p1)
#define LPS25H_MSG_2(level,msg,p1,p2)
#define LPS25H_MSG_3(level,msg,p1,p2,p3)
#define LPS25H_MSG_4(level,msg,p1,p2,p3,p4)
#endif //LPS25H_DEBUG

#define AUTO_INCREMENT 0x80
#define STM_LPS25H_NUM_BYTES_PRESS 3
#define STM_LPS25H_NUM_BYTES_TEMP 2


/**
 * Pressure sensor LPS25H output data rate
 * * This is bits [ODR2:ODR0] ([6:4]) in register CTRL_REG1(20h)
 * * The values at the end of name indicate pressure output rate first, temperature output rate second
 */
typedef enum 
{
    //STM_LPS25H_ODR_0_0 =           0x00,  /*Pressure: one shot, Temperature: one shot*/
    STM_LPS25H_ODR_1_1 =             0x01,  /*Pressure: 1Hz, Temperature: 1Hz*/
    STM_LPS25H_ODR_7_7 =             0x02,  /*Pressure: 7Hz, Temperature: 7Hz*/
    STM_LPS25H_ODR_12p5_12p5 =       0x03,  /*Pressure: 12.5Hz, Temperature: 12.5Hz*/
    STM_LPS25H_ODR_25_25 =       	 0x04,  /*Pressure: 25Hz, Temperature: 25Hz*/
} stm_lps25h_odr;

#define STM_LPS25H_ODR_NUM      4
#define STM_LPS25H_MAX_ODR      STM_LPS25H_ODR_25_25
#define STM_LPS25H_MAX_ODR_HZ	25

/**
 * Pressure sensor LPS25H I2C address
 */
#define STM_LPS25H_I2C_ADDR         0x5D

/**
 * Pressure sensor LPS25H WHO AM I register
 */
#define STM_LPS25H_WHO_AM_I_VALUE   0xBD

/**
 * Pressure sensor LPS25H register addresses
 */
#define STM_LPS25H_REF_P_XL             0x08
#define STM_LPS25H_REF_P_L              0x09
#define STM_LPS25H_REF_P_H              0x0A
#define STM_LPS25H_WHO_AM_I             0x0F
#define STM_LPS25H_RES_CONF             0x10
#define STM_LPS25H_RES_ALLOW            0x16
#define STM_LPS25H_CTRL_REG1            0x20
#define STM_LPS25H_CTRL_REG2            0x21
#define STM_LPS25H_CTRL_REG3            0x22
#define STM_LPS25H_CTRL_REG4            0x23
#define STM_LPS25H_INT_CFG              0x24
#define STM_LPS25H_INT_SOURCE           0x25
#define STM_LPS25H_STATUS_REG           0x27
#define STM_LPS25H_PRESS_OUT_XL         0x28
#define STM_LPS25H_PRESS_OUT_L          0x29
#define STM_LPS25H_PRESS_OUT_H          0x2A
#define STM_LPS25H_TEMP_OUT_L           0x2B
#define STM_LPS25H_TEMP_OUT_H           0x2C
#define STM_LPS25H_FIFO_CTRL            0x2E
#define STM_LPS25H_FIFO_STATUS          0x2F

#define STMERR_TEST_OK					0
#define STMERR_TEST_UNKNOWN				-1
#define STMERR_TEST_1ST_SENSOR_FAIL		1
#define STMERR_TEST_2ND_SENSOR_FAIL		2
#define STMERR_TEST_I2C_FAIL		2


/**
 * Pressure sensor LPS25H Driver State Information Structure
 */
typedef struct {
    /**< Handle used with sns_ddf_notify_data. */
    sns_ddf_handle_t smgr_hndl; 

    /**< Handle used to access the I2C bus. */
    sns_ddf_handle_t port_handle;

    /**< Current sensor sampling frequency. */
    stm_lps25h_odr data_rate;

    /**< Current power state: ACTIVE or LOWPOWER */
    sns_ddf_powerstate_e power_state;

    /**< Current FIFO status: 0 for disabled; 1 for enabled. */
    uint8_t FIFO_enable;

    /**< Self-test status: 0 for disabled; 1 for enabled. */
    uint8_t self_test_enable;

    /**< Timer object for self-test */
    sns_ddf_timer_s			timer_obj;

} sns_dd_press_lps25h_state_t;



/**
 * Function prototypes
 */

//.init       = &sns_dd_press_lps25h_init,
sns_ddf_status_e sns_dd_press_lps25h_init(
   sns_ddf_handle_t *dd_handle_ptr,
   sns_ddf_handle_t smgr_handle,
   sns_ddf_nv_params_s *nv_params,
   sns_ddf_device_access_s device_info[],
   uint32_t num_devices,
   sns_ddf_memhandler_s *memhandler,
   sns_ddf_sensor_e *sensors[],
   uint32_t *num_sensors);


//.get_data   = &sns_dd_press_lps25h_get_data,
sns_ddf_status_e sns_dd_press_lps25h_get_data(
   sns_ddf_handle_t dd_handle,
   sns_ddf_sensor_e sensors[],
   uint32_t num_sensors,
   sns_ddf_memhandler_s *memhandler,
   sns_ddf_sensor_data_s **data);


//.set_attrib = &sns_dd_press_lps25h_set_attr,
sns_ddf_status_e sns_dd_press_lps25h_set_attr(
   sns_ddf_handle_t dd_handle,
   sns_ddf_sensor_e sensor,
   sns_ddf_attribute_e attrib,
   void *value);


//.get_attrib = &sns_dd_press_lps25h_get_attr,
sns_ddf_status_e sns_dd_press_lps25h_get_attr(
   sns_ddf_handle_t dd_handle,
   sns_ddf_sensor_e sensor,
   sns_ddf_attribute_e attrib,
   sns_ddf_memhandler_s *memhandler,
   void **value,
   uint32_t *num_elems);


//.handle_timer = &sns_dd_press_lps25h_handle_timer,
void sns_dd_press_lps25h_handle_timer(
   sns_ddf_handle_t dd_handle,
   void *arg);


//.reset      = &sns_dd_press_lps25h_reset,
sns_ddf_status_e sns_dd_press_lps25h_reset(
   sns_ddf_handle_t dd_handle);


//.run_test   = &sns_dd_press_lps25h_run_test,
sns_ddf_status_e sns_dd_press_lps25h_run_test
(
   sns_ddf_handle_t    dd_handle,
   sns_ddf_sensor_e    sensor_type,
   sns_ddf_test_e      test,
   uint32_t *err);


//.probe      = &sns_dd_press_lps25h_probe
sns_ddf_status_e sns_dd_press_lps25h_probe(
   sns_ddf_device_access_s *device_info,
   sns_ddf_memhandler_s *memhandler,
   uint32_t *num_sensors,
   sns_ddf_sensor_e **sensors);


sns_ddf_status_e sns_dd_press_enable_FIFO(
   sns_ddf_handle_t dd_handle);


#endif  /* __LPS25H__ */
