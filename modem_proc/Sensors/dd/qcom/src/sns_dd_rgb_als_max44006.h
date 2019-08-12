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

/******************************************************************************
* Copyright (c) 2010-2013 by Qualcomm Technologies, Inc.  All Rights Reserved
* Qualcomm Technologies Proprietary and Confidential
******************************************************************************/


#ifndef __SNS_DD_MAX44006_H
#define __SNS_DD_MAX44006_H


#include <stdio.h>
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"


#define SNS_DD_MAX44006_I2C_ADDRESS			0x45

#define MAX44006_DD_MAX_SENSOR_TYPES	2

/* Register definitions */
#define SNS_DD_MAX44006_STATUS_REG			0x00
#define SNS_DD_MAX44006_MAIN_CFG			0x01
#define SNS_DD_MAX44006_AMB_CFG_REG			0x02
#define SNS_DD_MAX44006_AMB_CLR_HI_REG		0x04
#define SNS_DD_MAX44006_AMB_CLR_LO_REG		0x05
#define SNS_DD_MAX44006_AMB_RED_HI_REG		0x06
#define SNS_DD_MAX44006_AMB_RED_LO_REG		0x07

#define SNS_DD_MAX44006_AMB_GREEN_HI_REG	0x08
#define SNS_DD_MAX44006_AMB_GREEN_LO_REG	0x09

#define SNS_DD_MAX44006_AMB_BLUE_HI_REG		0x0A
#define SNS_DD_MAX44006_AMB_BLUE_LO_REG		0x0B

#define SNS_DD_MAX44006_AMB_IR_HI_REG		0x0C
#define SNS_DD_MAX44006_AMB_IR_LO_REG		0x0D

#define SNS_DD_MAX44006_AMB_COMP_HI_REG		0x0E
#define SNS_DD_MAX44006_AMB_COMP_LO_REG		0x0F

#define SNS_DD_MAX44006_TEMP_HI_REG			0x12
#define SNS_DD_MAX44006_TEMP_LO_REG			0x13


/* Interrupt Thresholds */
#define SNS_DD_MAX44006_AMB_UPTHR_HI_REG	0x14
#define SNS_DD_MAX44006_AMB_UPTHR_LO_REG	0x15

#define SNS_DD_MAX44006_AMB_LOTHR_HI_REG	0x16
#define SNS_DD_MAX44006_AMB_LOTHR_LO_REG	0x17
#define SNS_DD_MAX44006_AMB_THR_PST_TMR_REG	0x18


/* Ambient ADC Gains */
#define SNS_DD_MAX44006_TRIM_GAIN_CLR_REG	0x1D
#define SNS_DD_MAX44006_TRIM_GAIN_RED_REG	0x1E
#define SNS_DD_MAX44006_TRIM_GAIN_GREEN_REG	0x1F
#define SNS_DD_MAX44006_TRIM_GAIN_BLUE_REG	0x20
#define SNS_DD_MAX44006_TRIM_GAIN_IR_REG	0x21

/* Register values */
#define MAX44006_AMB_THR_PST_TMR			0x01
#define MAX44006_AMB_TRESH_HI				0x0000
#define MAX44006_AMB_TRESH_LO				0x0000


/* Ambient bit configuration */

#define MAX44006_AMB_PGA_FACTOR_BITS		0x02 /* If this default value changes then re tuning of ALS factor is needed, AMPGA is 0b10 */
#define MAX44006_AMB_TIM_BITS			0x00
#define MAX44006_TEMPEN_BIT					0x01
#define MAX44006_COMPEN_BIT					0x00
#define MAX44006_TRIM_BIT					  0x00

#define MAX44006_AMB_PGA_FACTOR_BITS_OEM_TEST   0x01
#define MAX44006_AMB_TIM_BITS_OEM_TEST				      0x00 // 100 ms integ time
//#define MAX44006_AMB_TIM_BITS_OEM_TEST				      0x01 // 25 ms integ time
//#define MAX44006_AMB_TIM_BITS_OEM_TEST				      0x02 // 6.25 ms integ time

/* shifts and masks*/
#define MAX44006_AMB_TIM_BITS_SHIFT     2
#define MAX44006_AMB_TIM_BITS_MASK      0x07
#define MAX44006_AMB_TRIM_BIT_ON        0x80

#define MAX44006_MAIN_CFG_CL_RGB_IR             0x20
#define MAX44006_MAIN_CFG_AMB_INT_ENABLE_BIT    0x01

/* gain trim bits */
#define MAX44006_GAIN_TRIM_BIT6				 0x40
#define MAX44006_GAIN_TRIM_BIT5				 0x20
#define MAX44006_GAIN_TRIM_BIT4				 0x10
#define MAX44006_GAIN_TRIM_BIT3				 0x08
#define MAX44006_GAIN_TRIM_BIT2				 0x04
#define MAX44006_GAIN_TRIM_BIT1				 0x02
#define MAX44006_GAIN_TRIM_BIT0				 0x01
#define MAX44006_GAIN_TRIM_MASK				 0x7F
#define MAX44006_GAIN_TRIM_MAX				  0x7F
#define MAX44006_GAIN_TRIM_MIN      0x00





/*******************************************************************
uW/cdm^2 for 0b000 (100 ms) and AMPGA 0b10

CLR		128/1000.0
R		128/1000.0
G 		128/1000.0
B 		256/1000.0
IR 		128/1000.0
IR_C	128/1000.0

Please see the following datasheet, page 14
http://datasheets.maximintegrated.com/en/ds/MAX44006-MAX44008.pdf
*******************************************************************/

#define CLR_U_WATT_PER_LSB			((float)(128.0/1000.0))
#define RED_U_WATT_PER_LSB			((float)(128.0/1000.0))
#define GREEN_U_WATT_PER_LSB		((float)(128.0/1000.0))
#define BLUE_U_WATT_PER_LSB			((float)(256.0/1000.0))
#define IR_U_WATT_PER_LSB			((float)(128.0/1000.0))
#define IR_COMP_U_WATT_PER_LSB		((float)(128.0/1000.0))


//#define MAX44006_DEBUG
//#define MAX44006_LOG_ALL_REG_IN_OEM_TEST //logs some more data in low and high gain OEM test to aid tester in deciding threshold values for registry 


#ifdef MAX44006_DEBUG
#if defined QDSP6
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define MAX44006_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "MAX44006 - " msg)
#define MAX44006_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "MAX44006 - " msg,p1)
#define MAX44006_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "MAX44006 - " msg,p1,p2)
#define MAX44006_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "MAX44006 - " msg,p1,p2,p3)
#else
#define MED MEDIUM
#include "sns_debug_str.h"
#define MAX44006_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define MAX44006_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
#define MAX44006_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
#define MAX44006_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
#endif
#else
#define MAX44006_MSG_0(level,msg)
#define MAX44006_MSG_1(level,msg,p1)
#define MAX44006_MSG_2(level,msg,p1,p2)
#define MAX44006_MSG_3(level,msg,p1,p2,p3)
#endif

typedef struct
{
    sns_ddf_dev_list_t next; /* the next device instance */
    sns_ddf_driver_info_s dd_info; /* the driver info */
} my_dd_head_s;

typedef struct
{
    sns_ddf_powerstate_e 	powerstate;
    sns_ddf_handle_t 		   smgr_handle;
    uint32_t              odr;
    uint16_t              irq;
    bool                  dri_enable;
    bool                  enable;
} max44006_sub_dev_s;

#define MAX44006_MAGIC_NUM     0x5A5A5A5A

typedef struct
{
    uint8_t  visible_ratio;      /* visible light transmission ratio of the glass/pipe in % */
    uint8_t  ir_ratio;           /* IR transmission ratio of light glass/pipe in % */
    uint16_t dc_offset;          /* DC offset */
    uint16_t als_thresh_hi;
    uint16_t als_thresh_lo;
    uint16_t reserved_1;
    uint16_t als_factor;         /* ALS calibration factor */
    uint32_t max44006_magic_num;    
    uint32_t reserved_2;   
    uint32_t als_gain;           /* Ambient gain AMBGA */
    uint32_t als_time;           /* Ambient integration time */

} max44006_dd_nv_param_s;

typedef struct
{
    uint32_t magic_num; //5000

    uint32_t r_low_thresh; //5001
    uint32_t r_high_thresh;  //5002
    uint32_t r_gain_trim;  //5003
    uint32_t r_trim_amb_value; //5004

    uint32_t g_low_thresh;  //5005
    uint32_t g_high_thresh; //5006
    uint32_t g_gain_trim; //5007
    uint32_t g_trim_amb_value; //5008
    
    uint32_t b_low_thresh; //5009
    uint32_t b_high_thresh; //5010
    uint32_t b_gain_trim; //5011
    uint32_t b_trim_amb_value;  //5012
    
    uint32_t c_low_thresh;  //5013
    uint32_t c_high_thresh; //5014
    uint32_t c_gain_trim; //5015
    uint32_t c_trim_amb_value; //5016
    
    uint32_t ir_low_thresh;  //5017
    uint32_t ir_high_thresh; //5018
    uint32_t ir_gain_trim;  //5019
    uint32_t ir_trim_amb_value; //5020

    /* NOTE: Register 02 values applied only in OEM Test, currently
     *       Code to apply it in regular mode is in #if 0 in sns_dd_als_max44006_init_normal() */
    uint32_t register_02;  //5021

    uint32_t	data_from_registry;  //5022
} max44006_dd_rgb_nv_param_s;

typedef struct
{
    sns_ddf_odr_t common_cur_odr;
    sns_ddf_odr_t max44006_als_odr;
    sns_ddf_odr_t max44006_rgb_odr;
    sns_ddf_odr_t max44006_ct_c_odr;
    sns_ddf_odr_t max44006_dummy_odr;
} max44006_dd_common_s;


typedef enum
{
    MAX44006_OEM_TEST_NONE,
    MAX44006_OEM_TEST_TRIM_GAIN_LOW_TEST, /*test if the trim gains can match */	
    MAX44006_OEM_TEST_TRIM_GAIN_HIGH_TEST, /*test if the trim gains can match */	
    MAX44006_OEM_TEST_TRIM_GAIN_CLEAR,
    MAX44006_OEM_TEST_TRIM_GAIN_RED,
    MAX44006_OEM_TEST_TRIM_GAIN_GREEN,
    MAX44006_OEM_TEST_TRIM_GAIN_BLUE,
    MAX44006_OEM_TEST_TRIM_GAIN_IR
} max44006_dd_oem_test_e;

typedef struct
{
    bool					running;
    max44006_dd_oem_test_e  curtest;
    sns_ddf_timer_s         test_timer;
    bool    test_timer_inited; /* one time flag to call timer init*/
    uint32_t    timer_val_microsec;
} max44006_dd_oem_test_s;

typedef enum
{
    MAX44006_OEM_TEST_FAIL, /*general failure*/
    MAX44006_OEM_TEST_I2C_FAIL, /*test failed due to i2c read or write failure*/
    MAX44006_OEM_TEST_THRESHOLD_MATCH_FAIL 	/*test failed to bring ambient value within threshold, */
                                            /* even after tweaking all the trim bits*/
} max44006_dd_oem_test_ret_e;

typedef enum
{
    MAX44006_AMB_DATA_NOT_CHECKED, 
    MAX44006_AMB_DATA_WITHIN_THRESHOLDS, 
    MAX44006_AMB_DATA_ABOVE_UPPER_THRESHOLD,
    MAX44006_AMB_DATA_BELOW_LOWER_THRESHOLD
} max44006_dd_amb_data_check_e;


/* data structure for the driver instance, allocated on init */
typedef struct
{
    sns_ddf_handle_t        port_handle;  /* I2C bus handle */
    bool 				               enabled_als;
    bool				                enabled_rgb;
    bool				                enabled_ct_c;
    sns_ddf_dev_list_t      next;
    max44006_sub_dev_s      sub_dev[MAX44006_DD_MAX_SENSOR_TYPES];
    max44006_dd_common_s    common;
    uint16_t			             hi_thresh;
    uint16_t			             lo_thresh;
    sns_ddf_odr_t           cur_odr;
    max44006_dd_nv_param_s  nv_param;
    uint16_t                nv_size;
    max44006_dd_rgb_nv_param_s  nv_rgb_param;
    uint16_t                nv_rgb_size;
    sns_ddf_status_e        reg_ret;
    sns_ddf_status_e        reg_rgb_ret;
    max44006_dd_oem_test_s  oem_test;
} sns_dd_als_rgb_state_t;

#endif
