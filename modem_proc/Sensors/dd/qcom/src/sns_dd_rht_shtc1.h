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
01-15-2014   MW      Add on-change data notify behavior CR 581550
12-03-2013   MW      Attribute value fixes for CR 582067
07-05-2013	 --		   Original version from Sensirion
==============================================================================*/

#ifndef _SNS_DD_RHT_SHTC1_H
#define _SNS_DD_RHT_SHTC1_H

/* Enable this macro to see debug messages */
//#define SHTC1_DEBUG

/* driver attributes */
#define SHTC1_DEVICE_VERSION        1
#define SHTC1_DRIVER_VERSION        1
#define SHTC1_MEAS_DUR_LPM_US       940
#define SHTC1_MEAS_DUR_HPM_US       14400
#define SHTC1_CURRENT_IDLE_UA       1
#define SHTC1_CURRENT_MEASURING_UA  250
#define SHTC1_TIME_TO_ACTIVE_US     0
#define SHTC1_MAX_READ_FREQ_LPM_HZ  (1000000/SHTC1_MEAS_DUR_LPM_US)
#define SHTC1_MAX_READ_FREQ_HPM_HZ  (1000000/SHTC1_MEAS_DUR_HPM_US)
#define SHTC1_ADC_RES_TEMP_LPM      11
#define SHTC1_ADC_RES_TEMP_HPM      14
#define SHTC1_ADC_RES_HUMI_LPM      9
#define SHTC1_ADC_RES_HUMI_HPM      12

/* driver specific */
#define SHTC1_MAX_NUM_SENSORS       2
#define SHTC1_NUM_SAMPLES           1
#define SHTC1_NUM_MODES             2

/* device communication */
// read temperature first then humidity , no clock stretching, low power
#define SHTC1_CMD_MEASURE_LPM       {0x60,0x9c}
// read temperature first then humidity , no clock stretching, high resolution
#define SHTC1_CMD_MEASURE_HPM       {0x78,0x66}
#define SHTC1_CMD_SOFT_RESET        {0x80,0x5d}
#define SHTC1_CMD_READ_ID_REG       {0xef,0xc8}
#define SHTC1_CMD_SIZE_BYTES        2
#define SHTC1_READ_SIZE_BYTES       6
#define SHTC1_ID_REG_MASK           0x3F
#define SHTC1_ID_REG_CONTENT        0x07
#define SHTC1_CRC_POLYNOMIAL        0x31
#define SHTC1_CRC_INIT              0xFF
/* calculations */
#define SHTC1_ADC_TICKS_SENT        16
#define SHTC1_TICKSTOTEMP_Q16(a)    \
  ((int32_t)(a)*175-45*(1<<SHTC1_ADC_TICKS_SENT))
#define SHTC1_TICKSTOHUMI_Q16(a)    ((int32_t)(a)*100)
#define SHTC1_RES_TEMP_Q16          175    // 175/65535 in decimal
#define SHTC1_RES_HUMI_Q16          100    // 100/65535 in decimal

#define SHTC1_TEMP_CHANGE           0.5
#define SHTC1_HUMIDITY_CHANGE       0.5

#ifdef SHTC1_DEBUG
#if defined QDSP6
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define SHTC1_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "SHTC1 - " msg)
#define SHTC1_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "SHTC1 - " msg,p1)
#define SHTC1_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "SHTC1 - " msg,p1,p2)
#define SHTC1_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "SHTC1 - " msg,p1,p2,p3)
#else
#define MED MEDIUM
#include "sns_debug_str.h"
#define SHTC1_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define SHTC1_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
#define SHCT1_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
#define SHTC1_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
#endif
#else
#define SHTC1_MSG_0(level,msg)
#define SHTC1_MSG_1(level,msg,p1)
#define SHTC1_MSG_2(level,msg,p1,p2)
#define SHTC1_MSG_3(level,msg,p1,p2,p3)
#endif

typedef struct
{
   uint32_t  ambient_temp_bias;
   uint32_t  ambient_temp_scale;
   uint32_t  ambient_temp_ds;
   uint32_t  humidity_bias;
   uint32_t  humidity_scale;
   uint32_t  humidity_ds;
}sns_dd_rht_shtc1_nv_t;


/* State variables for this driver */
typedef struct 
{
  sns_ddf_handle_t        smgr_hndl; /**< Handle used with sns_ddf_notify_data*/
  sns_ddf_handle_t 	      port_handle; /**< Handle used to access the I2C bus */
  sns_ddf_timer_s         timer_obj; /**< Pointer to the timer */
  sns_ddf_sensor_data_s   sensor_data[SHTC1_MAX_NUM_SENSORS]; /**< sensor sample
                                                                   sources */
  sns_ddf_sensor_sample_s sample_cache[SHTC1_MAX_NUM_SENSORS]; /**< cache for
                                                                    measurement
                                                                    data */
  bool 			  mode_use_LPM; /**< mode selection */
  bool 			  measurement_running; /**< true if measurement is
                                                    running */
  bool        enable_humidity;
  bool        enable_temp;
  q16_t       prev_humidity_value;
  q16_t       prev_temp_value;
  uint32_t    humidity_odr;
  uint32_t    temp_odr;
  
  sns_dd_rht_shtc1_nv_t  nv_source;
  uint8_t     device_selection;  /**< Device select when a family of devices
                                      is controlled by one driver.
                                      0 -> SHTC1
                                      1 -> SHTW1 */
  
} sns_dd_rht_shtc1_state_t;


#endif /* _SNS_DD_RHT_SHTC1_H */
