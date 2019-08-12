/*	Date: 2015/11/10 14:00:00
 *	Revision: 1.4.2
 */






/*******************************************************************************
* Copyright (c) 2013, <Bosch Sensortec GmbH>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of Bosch Sensortec GmbH nor the
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

/*-----------------------------------------------------------------------------
 * Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
  -----------------------------------------------------------------------------*/

/*==============================================================================

  S E N S O R S   P R E S S U R E	D R I V E R

DESCRIPTION

 Impelements the driver for the BMP280 driver

==============================================================================*/


#ifndef __BMP280_H__
#define __BMP280_H__

#define BMP_NAME "bmp280"


#ifndef BMP280_CONFIG_RUN_ON_OSSC
#define BMP280_CONFIG_RUN_ON_OSSC 	0
#endif


#ifndef BMP280_CONFIG_ENABLE_20BIT
#define BMP280_CONFIG_ENABLE_20BIT 	1
#endif

#ifndef BMP280_CONFIG_ENABLE_UIMAGE
#define BMP280_CONFIG_ENABLE_UIMAGE             1
#endif

#if BMP280_CONFIG_RUN_ON_OSSC
#include "fixed_point.h"

#include "qurt_elite_diag.h"
#include "sns_ddf_signal.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "stdbool.h"
#include <string.h>
#include "sns_log_api_public.h"
#include "sns_log_types_public.h"
//#include "sns_dd_inertial_test.h"
#else
#include "fixed_point.h"

#include "sns_memmgr.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
//#include "sns_dd_inertial_test.h"
#include "sns_ddf_signal.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#endif


#if !BMP280_CONFIG_ENABLE_UIMAGE
#define sns_ddf_malloc_ex(ptr, size, handle)                            sns_ddf_malloc(ptr, size)
#define sns_ddf_memhandler_malloc_ex(mem_handler, size, handle)         sns_ddf_memhandler_malloc(mem_handler, size)
#endif

/*!
 * @brief  Reported Temprature resolution is 0.01°C (Celsius).
 */
 extern const q16_t bmp280_temperature_resolution;

/*!
 * @brief Reported Pressure resolution is 1.0 Pa (Pascal) = 0.01 hPa.
 * Expected range 80K to 120K Pascals.
 */
 extern const q16_t bmp280_pressure_resolution;

#define DD_ATTR_LOCAL static


//#define BMP280_DEBUG
//#define PRIMITIVE_DEBUG_MSG

#define DBG_MEDIUM_PRIO DBG_MED_PRIO

#ifdef BMP280_DEBUG
#ifndef PRIMITIVE_DEBUG_MSG
#if !BMP280_CONFIG_ENABLE_UIMAGE
#define BMP280_MSG_0(level,msg)          MSG(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMP280 - " msg)
#define BMP280_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMP280 - " msg,p1)
#define BMP280_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMP280 - " msg,p1,p2)
#define BMP280_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMP280 - " msg,p1,p2,p3)
#define BMP280_MSG_3_P(level,msg,p1,p2,p3) MSG_3(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMP280 - " msg,p1,p2,p3)
#define BMP280_MSG_3_F(level,msg,p1,p2,p3)  MSG_3(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMP280 - " msg,p1,p2,p3)
#define BMP280_MSG_4(level,msg,p1,p2,p3,p4)  MSG_4(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMP280 - " msg,p1,p2,p3,p4)
#else   //BMP280_CONFIG_ENABLE_UIMAGE
#define BMP280_MSG_0(level,msg)          UMSG(MSG_SSID_SNS,DBG_##level##_PRIO, "BMP280 - " msg)
#define BMP280_MSG_1(level,msg,p1)       UMSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "BMP280 - " msg,p1)
#define BMP280_MSG_2(level,msg,p1,p2)    UMSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "BMP280 - " msg,p1,p2)
#define BMP280_MSG_3(level,msg,p1,p2,p3) UMSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BMP280 - " msg,p1,p2,p3)
#define BMP280_MSG_3_P(level,msg,p1,p2,p3) UMSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BMP280 - " msg,p1,p2,p3)
#define BMP280_MSG_3_F(level,msg,p1,p2,p3)  UMSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BMP280 - " msg,p1,p2,p3)
#define BMP280_MSG_4(level,msg,p1,p2,p3,p4)  UMSG_4(MSG_SSID_SNS,DBG_##level##_PRIO, "BMP280 - " msg,p1,p2,p3,p4)
#endif  //BMP280_CONFIG_ENABLE_UIMAGE
#else
#define BMP280_MSG_0(level,msg)
#define BMP280_MSG_1(level,msg,p1)
#define BMP280_MSG_2(level,msg,p1,p2)
#define BMP280_MSG_3(level,msg,p1,p2,p3)
#define BMP280_MSG_4(level,msg,p1,p2,p3,p4)
//#define BMP280_MSG_3_P(level,msg,p1,p2,p3)
#define BMP280_MSG_3_P(level,msg,p1,p2,p3) //SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)

#define MED MEDIUM
#endif
#else
#define BMP280_MSG_0(level,msg)
#define BMP280_MSG_1(level,msg,p1)
#define BMP280_MSG_2(level,msg,p1,p2)
#define BMP280_MSG_3(level,msg,p1,p2,p3)
#define BMP280_MSG_4(level,msg,p1,p2,p3,p4)
#define BMP280_MSG_3_P(level,msg,p1,p2,p3) //MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BMP280 - " msg,p1,p2,p3)
#define BMP280_MSG_3_F(level,msg,p1,p2,p3)  //MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BMP280 - " msg,p1,p2,p3)
#endif

#define BST_ARRAY_SIZE(array)   (sizeof(array)/sizeof(array[0]))

struct bst_odr_ts_map {
    uint8_t odr;
    int32_t interval;
};

struct bst_val_pair {
    uint32_t l;
    uint32_t r;
};


#define BMP280_CONFIG_LOWEST_ODR_20BIT            1
#define BMP280_CONFIG_FASTEST_ODR_20BIT          26
#define BMP280_CONFIG_LOWEST_ODR_18BIT            1
#define BMP280_CONFIG_FASTEST_ODR_18BIT          83



//#define NULL ((void *)0)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))


/*! define sensor chip id [BMP280 = 0x56],[... = ...] */
#define BMP_SENSOR_CHIP_ID	    0x56
#define BMP_SENSOR_CHIP_ID_2	0x57
#define BMP_SENSOR_CHIP_ID_3	0x58


/*! define sensor i2c address */
#define BMP_I2C_ADDRESS		BMP280_I2C_ADDRESS
/*! define minimum pressure value by input event */
#define ABS_MIN_PRESSURE	30000
/*! define maximum pressure value by input event */
#define ABS_MAX_PRESSURE	110000
/*! define default delay time used by input event [unit:ms] */
#define BMP_DELAY_DEFAULT	200
/*! define maximum temperature oversampling */
#define BMP_OVERSAMPLING_T_MAX	BMP_VAL_NAME(OVERSAMPLING_16X)
/*! define maximum pressure oversampling */
#define BMP_OVERSAMPLING_P_MAX	BMP_VAL_NAME(OVERSAMPLING_16X)
/*! define defalut filter coefficient */
#define BMP_FILTER_DEFAULT	BMP_VAL_NAME(FILTERCOEFF_8)
/*! define maximum filter coefficient */
#define BMP_FILTER_MAX		BMP_VAL_NAME(FILTERCOEFF_16)
/*! define default work mode */
#define BMP_WORKMODE_DEFAULT		BMP_VAL_NAME(STANDARDRESOLUTION_MODE)
/*! define default standby duration [unit:ms] */
#define BMP_STANDBYDUR_DEFAULT	1
#define BMP_STANDBYDUR_20BIT_10HZ	63
#define BMP_STANDBYDUR_20BIT_6HZ	125



#define BMP280_U16_t uint16_t
#define BMP280_S16_t int16_t
#define BMP280_U32_t uint32_t
#define BMP280_S32_t int32_t
#define BMP280_S64_t int64_t
#define BMP280_64BITSUPPORT_PRESENT


/* If the user wants to support floating point calculations, please set \
    the following #define. If floating point calculation is not wanted \
    or allowed (e.g. in Linux kernel), please do not set the define. */
#define BMP280_ENABLE_FLOAT
/* If the user wants to support 64 bit integer calculation (needed for \
    optimal pressure accuracy) please set the following #define. If \
    int64 calculation is not wanted (e.g. because it would include \
    large libraries), please do not set the define. */
#define BMP280_ENABLE_INT64

/** defines the return parameter type of the BMP280_WR_FUNCTION */
#define BMP280_BUS_WR_RETURN_TYPE signed char

/**\brief links the order of parameters defined in
BMP280_BUS_WR_PARAM_TYPE to function calls used inside the API*/
#define BMP280_BUS_WR_PARAM_TYPES unsigned char, unsigned char,\
    unsigned char *, unsigned char

/**\brief links the order of parameters defined in
BMP280_BUS_WR_PARAM_TYPE to function calls used inside the API*/
#define BMP280_BUS_WR_PARAM_ORDER(device_addr, register_addr,\
    register_data, wr_len)

/* never change this line */
#define BMP280_BUS_WRITE_FUNC(device_addr, register_addr,\
register_data, wr_len) bus_write(device_addr, register_addr,\
    register_data, wr_len)

/**\brief defines the return parameter type of the BMP280_RD_FUNCTION
*/
#define BMP280_BUS_RD_RETURN_TYPE signed char

/**\brief defines the calling parameter types of the BMP280_RD_FUNCTION
*/
#define BMP280_BUS_RD_PARAM_TYPES unsigned char, unsigned char,\
    unsigned char *, unsigned char

/**\brief links the order of parameters defined in \
BMP280_BUS_RD_PARAM_TYPE to function calls used inside the API
*/
#define BMP280_BUS_RD_PARAM_ORDER device_addr, register_addr,\
    register_data

/* never change this line */
#define BMP280_BUS_READ_FUNC(device_addr, register_addr,\
    register_data, rd_len)bus_read(device_addr, register_addr,\
    register_data, rd_len)

/**\brief defines the return parameter type of the BMP280_DELAY_FUNCTION
*/
#define BMP280_DELAY_RETURN_TYPE void

/**\brief defines the calling parameter types of the BMP280_DELAY_FUNCTION
*/
#define BMP280_DELAY_PARAM_TYPES BMP280_U16_t

/* never change this line */
#define BMP280_DELAY_FUNC(delay_in_msec)\
        delay_func(delay_in_msec)

#define BMP280_GET_BITSLICE(regvar, bitname)\
    ((regvar & bitname##__MSK) >> bitname##__POS)

#define BMP280_SET_BITSLICE(regvar, bitname, val)\
    ((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))


/* Constants */
#define BMP280_NULL                          0
#define BMP280_RETURN_FUNCTION_TYPE          sns_ddf_status_e

#define SHIFT_RIGHT_4_POSITION				 4
#define SHIFT_LEFT_2_POSITION                2
#define SHIFT_LEFT_4_POSITION                4
#define SHIFT_LEFT_5_POSITION                5
#define SHIFT_LEFT_8_POSITION                8
#define SHIFT_LEFT_12_POSITION               12
#define SHIFT_LEFT_16_POSITION               16
#define BMA280_Four_U8X                      4
#define BMA280_Eight_U8X                     8

#define E_BMP280_NULL_PTR                    ((signed char)-127)
#define E_BMP280_COMM_RES                    ((signed char)-1)
#define E_BMP280_OUT_OF_RANGE                ((signed char)-2)

#define BMP280_I2C_ADDRESS1                  0x76
#define BMP280_I2C_ADDRESS2                  0x77
#define BMP280_I2C_ADDRESS                   BMP280_I2C_ADDRESS2

/* Sensor Specific constants */
#define BMP280_SLEEP_MODE                    0x00
#define BMP280_FORCED_MODE                   0x01
#define BMP280_NORMAL_MODE                   0x03
#define BMP280_SOFT_RESET                    0xB6

#define BMP280_DELAYTIME_MS_NONE             0
#define BMP280_DELAYTIME_MS_5                5
#define BMP280_DELAYTIME_MS_6                6
#define BMP280_DELAYTIME_MS_8                8
#define BMP280_DELAYTIME_MS_12               12
#define BMP280_DELAYTIME_MS_22               22
#define BMP280_DELAYTIME_MS_38               38

#define BMP280_STANDBYTIME_1_MS              1
#define BMP280_STANDBYTIME_63_MS             63
#define BMP280_STANDBYTIME_125_MS            125
#define BMP280_STANDBYTIME_250_MS            250
#define BMP280_STANDBYTIME_500_MS            500
#define BMP280_STANDBYTIME_1000_MS           1000
#define BMP280_STANDBYTIME_2000_MS           2000
#define BMP280_STANDBYTIME_4000_MS           4000

#define BMP280_OVERSAMPLING_SKIPPED          0x00
#define BMP280_OVERSAMPLING_1X               0x01
#define BMP280_OVERSAMPLING_2X               0x02
#define BMP280_OVERSAMPLING_4X               0x03
#define BMP280_OVERSAMPLING_8X               0x04
#define BMP280_OVERSAMPLING_16X              0x05

#define BMP280_ULTRALOWPOWER_MODE            0x00
#define BMP280_LOWPOWER_MODE	             0x01
#define BMP280_STANDARDRESOLUTION_MODE       0x02
#define BMP280_HIGHRESOLUTION_MODE           0x03
#define BMP280_ULTRAHIGHRESOLUTION_MODE      0x04

#define BMP280_ULTRALOWPOWER_OSRS_P          BMP280_OVERSAMPLING_1X
#define BMP280_ULTRALOWPOWER_OSRS_T          BMP280_OVERSAMPLING_1X

#define BMP280_LOWPOWER_OSRS_P	             BMP280_OVERSAMPLING_2X
#define BMP280_LOWPOWER_OSRS_T	             BMP280_OVERSAMPLING_1X

#define BMP280_STANDARDRESOLUTION_OSRS_P     BMP280_OVERSAMPLING_4X
#define BMP280_STANDARDRESOLUTION_OSRS_T     BMP280_OVERSAMPLING_1X

#define BMP280_HIGHRESOLUTION_OSRS_P         BMP280_OVERSAMPLING_8X
#define BMP280_HIGHRESOLUTION_OSRS_T         BMP280_OVERSAMPLING_1X

#define BMP280_ULTRAHIGHRESOLUTION_OSRS_P    BMP280_OVERSAMPLING_16X
#define BMP280_ULTRAHIGHRESOLUTION_OSRS_T    BMP280_OVERSAMPLING_2X

#define BMP280_FILTERCOEFF_OFF               0x00
#define BMP280_FILTERCOEFF_2                 0x01
#define BMP280_FILTERCOEFF_4                 0x02
#define BMP280_FILTERCOEFF_8                 0x03
#define BMP280_FILTERCOEFF_16                0x04

#define T_INIT_MAX							20
                /* 20/16 = 1.25 ms */
#define T_MEASURE_PER_OSRS_MAX				37
                /* 37/16 = 2.3125 ms*/
#define T_SETUP_PRESSURE_MAX				10
                /* 10/16 = 0.625 ms */

/*calibration parameters */
#define BMP280_DIG_T1_LSB_REG                0x88
#define BMP280_DIG_T1_MSB_REG                0x89
#define BMP280_DIG_T2_LSB_REG                0x8A
#define BMP280_DIG_T2_MSB_REG                0x8B
#define BMP280_DIG_T3_LSB_REG                0x8C
#define BMP280_DIG_T3_MSB_REG                0x8D
#define BMP280_DIG_P1_LSB_REG                0x8E
#define BMP280_DIG_P1_MSB_REG                0x8F
#define BMP280_DIG_P2_LSB_REG                0x90
#define BMP280_DIG_P2_MSB_REG                0x91
#define BMP280_DIG_P3_LSB_REG                0x92
#define BMP280_DIG_P3_MSB_REG                0x93
#define BMP280_DIG_P4_LSB_REG                0x94
#define BMP280_DIG_P4_MSB_REG                0x95
#define BMP280_DIG_P5_LSB_REG                0x96
#define BMP280_DIG_P5_MSB_REG                0x97
#define BMP280_DIG_P6_LSB_REG                0x98
#define BMP280_DIG_P6_MSB_REG                0x99
#define BMP280_DIG_P7_LSB_REG                0x9A
#define BMP280_DIG_P7_MSB_REG                0x9B
#define BMP280_DIG_P8_LSB_REG                0x9C
#define BMP280_DIG_P8_MSB_REG                0x9D
#define BMP280_DIG_P9_LSB_REG                0x9E
#define BMP280_DIG_P9_MSB_REG                0x9F

#define BMP280_CHIPID_REG                    0xD0  /*Chip ID Register */
#define BMP280_RESET_REG                     0xE0  /*Softreset Register */
#define BMP280_STATUS_REG                    0xF3  /*Status Register */
#define BMP280_CTRLMEAS_REG                  0xF4  /*Ctrl Measure Register */
#define BMP280_CONFIG_REG                    0xF5  /*Configuration Register */
#define BMP280_PRESSURE_MSB_REG              0xF7  /*Pressure MSB Register */
#define BMP280_PRESSURE_LSB_REG              0xF8  /*Pressure LSB Register */
#define BMP280_PRESSURE_XLSB_REG             0xF9  /*Pressure XLSB Register */
#define BMP280_TEMPERATURE_MSB_REG           0xFA  /*Temperature MSB Reg */
#define BMP280_TEMPERATURE_LSB_REG           0xFB  /*Temperature LSB Reg */
#define BMP280_TEMPERATURE_XLSB_REG          0xFC  /*Temperature XLSB Reg */

/* Status Register */
#define BMP280_STATUS_REG_MEASURING__POS           3
#define BMP280_STATUS_REG_MEASURING__MSK           0x08
#define BMP280_STATUS_REG_MEASURING__LEN           1
#define BMP280_STATUS_REG_MEASURING__REG           BMP280_STATUS_REG

#define BMP280_STATUS_REG_IMUPDATE__POS            0
#define BMP280_STATUS_REG_IMUPDATE__MSK            0x01
#define BMP280_STATUS_REG_IMUPDATE__LEN            1
#define BMP280_STATUS_REG_IMUPDATE__REG            BMP280_STATUS_REG

/* Control Measurement Register */
#define BMP280_CTRLMEAS_REG_OSRST__POS             5
#define BMP280_CTRLMEAS_REG_OSRST__MSK             0xE0
#define BMP280_CTRLMEAS_REG_OSRST__LEN             3
#define BMP280_CTRLMEAS_REG_OSRST__REG             BMP280_CTRLMEAS_REG

#define BMP280_CTRLMEAS_REG_OSRSP__POS             2
#define BMP280_CTRLMEAS_REG_OSRSP__MSK             0x1C
#define BMP280_CTRLMEAS_REG_OSRSP__LEN             3
#define BMP280_CTRLMEAS_REG_OSRSP__REG             BMP280_CTRLMEAS_REG

#define BMP280_CTRLMEAS_REG_MODE__POS              0
#define BMP280_CTRLMEAS_REG_MODE__MSK              0x03
#define BMP280_CTRLMEAS_REG_MODE__LEN              2
#define BMP280_CTRLMEAS_REG_MODE__REG              BMP280_CTRLMEAS_REG

/* Configuation Register */
#define BMP280_CONFIG_REG_TSB__POS                 5
#define BMP280_CONFIG_REG_TSB__MSK                 0xE0
#define BMP280_CONFIG_REG_TSB__LEN                 3
#define BMP280_CONFIG_REG_TSB__REG                 BMP280_CONFIG_REG

#define BMP280_CONFIG_REG_FILTER__POS              2
#define BMP280_CONFIG_REG_FILTER__MSK              0x1C
#define BMP280_CONFIG_REG_FILTER__LEN              3
#define BMP280_CONFIG_REG_FILTER__REG              BMP280_CONFIG_REG

#define BMP280_CONFIG_REG_SPI3WEN__POS             0
#define BMP280_CONFIG_REG_SPI3WEN__MSK             0x01
#define BMP280_CONFIG_REG_SPI3WEN__LEN             1
#define BMP280_CONFIG_REG_SPI3WEN__REG             BMP280_CONFIG_REG

/* Data Register */
#define BMP280_PRESSURE_XLSB_REG_DATA__POS         4
#define BMP280_PRESSURE_XLSB_REG_DATA__MSK         0xF0
#define BMP280_PRESSURE_XLSB_REG_DATA__LEN         4
#define BMP280_PRESSURE_XLSB_REG_DATA__REG         BMP280_PRESSURE_XLSB_REG

#define BMP280_TEMPERATURE_XLSB_REG_DATA__POS      4
#define BMP280_TEMPERATURE_XLSB_REG_DATA__MSK      0xF0
#define BMP280_TEMPERATURE_XLSB_REG_DATA__LEN      4
#define BMP280_TEMPERATURE_XLSB_REG_DATA__REG      BMP280_TEMPERATURE_XLSB_REG

#define BMP280_WR_FUNC_PTR\
    BMP280_RETURN_FUNCTION_TYPE (*bus_write)(unsigned char, unsigned char,\
            unsigned char *, unsigned char)

#define BMP280_RD_FUNC_PTR\
    BMP280_RETURN_FUNCTION_TYPE (*bus_read)(unsigned char, unsigned char,\
            unsigned char *, unsigned char)

#define BMP280_MDELAY_DATA_TYPE uint32_t

/** this structure holds all device specific calibration parameters */
struct bmp280_calibration_param_t {
    BMP280_U16_t dig_T1;
    BMP280_S16_t dig_T2;
    BMP280_S16_t dig_T3;
    BMP280_U16_t dig_P1;
    BMP280_S16_t dig_P2;
    BMP280_S16_t dig_P3;
    BMP280_S16_t dig_P4;
    BMP280_S16_t dig_P5;
    BMP280_S16_t dig_P6;
    BMP280_S16_t dig_P7;
    BMP280_S16_t dig_P8;
    BMP280_S16_t dig_P9;
	signed char  dig_P10;

    BMP280_S32_t t_fine;
};
/** BMP280 image registers data structure */
struct bmp280_t {
    struct bmp280_calibration_param_t cal_param;

    unsigned char chip_id;
    unsigned char dev_addr;

    unsigned char osrs_t;
    unsigned char osrs_p;

    BMP280_WR_FUNC_PTR;
    BMP280_RD_FUNC_PTR;
    void(*delay_msec)(BMP280_MDELAY_DATA_TYPE);
};

/* Function Declarations */
BMP280_RETURN_FUNCTION_TYPE bmp280_init(struct bmp280_t *bmp280);
BMP280_RETURN_FUNCTION_TYPE bmp280_read_ut(BMP280_S32_t *utemperature);
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T);
BMP280_RETURN_FUNCTION_TYPE bmp280_read_up(BMP280_S32_t *upressure);
BMP280_U32_t bmp280_compensate_P_int32(BMP280_S32_t adc_P);
BMP280_RETURN_FUNCTION_TYPE bmp280_read_uput(BMP280_S32_t *upressure,\
        BMP280_S32_t *utemperature);
BMP280_RETURN_FUNCTION_TYPE bmp280_read_pt(BMP280_U32_t *pressure,\
        BMP280_S32_t *temperature);
BMP280_RETURN_FUNCTION_TYPE bmp280_get_calib_param(void);
BMP280_RETURN_FUNCTION_TYPE bmp280_verify_calib_param(void);
BMP280_RETURN_FUNCTION_TYPE bmp280_verify_calib_range(void);
BMP280_RETURN_FUNCTION_TYPE bmp280_get_osrs_t(unsigned char *value);
BMP280_RETURN_FUNCTION_TYPE bmp280_set_osrs_t(unsigned char value);
BMP280_RETURN_FUNCTION_TYPE bmp280_get_osrs_p(unsigned char *value);
BMP280_RETURN_FUNCTION_TYPE bmp280_set_osrs_p(unsigned char value);
BMP280_RETURN_FUNCTION_TYPE bmp280_get_mode(unsigned char *mode);
BMP280_RETURN_FUNCTION_TYPE bmp280_set_mode(unsigned char mode);
BMP280_RETURN_FUNCTION_TYPE bmp280_set_softreset(void);
BMP280_RETURN_FUNCTION_TYPE bmp280_get_spi3(unsigned char *enable_disable);
BMP280_RETURN_FUNCTION_TYPE bmp280_set_spi3(unsigned char enable_disable);
BMP280_RETURN_FUNCTION_TYPE bmp280_get_filter(unsigned char *value);
BMP280_RETURN_FUNCTION_TYPE bmp280_set_filter(unsigned char value);
BMP280_RETURN_FUNCTION_TYPE bmp280_get_standbydur(unsigned char *time);
BMP280_RETURN_FUNCTION_TYPE bmp280_set_standbydur(unsigned char time);
BMP280_RETURN_FUNCTION_TYPE bmp280_set_workmode(unsigned char mode);
BMP280_RETURN_FUNCTION_TYPE bmp280_get_forced_uput(BMP280_S32_t *upressure,\
        BMP280_S32_t *utemperature);
BMP280_RETURN_FUNCTION_TYPE bmp280_write_register(unsigned char addr, \
    unsigned char *data, unsigned char len);
BMP280_RETURN_FUNCTION_TYPE bmp280_read_register(unsigned char addr, \
    unsigned char *data, unsigned char len);
#ifdef BMP280_ENABLE_FLOAT
double bmp280_compensate_T_double(BMP280_S32_t adc_T);
double bmp280_compensate_P_double(BMP280_S32_t adc_P);
#endif
#if defined(BMP280_ENABLE_INT64) && defined(BMP280_64BITSUPPORT_PRESENT)
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P);
#endif
BMP280_RETURN_FUNCTION_TYPE bmp280_compute_wait_time(unsigned char \
    *v_delaytime_u8r);


/*! define BMP register name according to API */
#define BMP_REG_NAME(name) BMP280_##name
/*! define BMP value name according to API */
#define BMP_VAL_NAME(name) BMP280_##name
/*! define BMP hardware-related function according to API */
#define BMP_CALL_API(name) bmp280_##name
/*! only for debug */
/*#define DEBUG_BMP280*/

/*!
 * @brief bus communication operation
*/


struct bmp_bus_ops {

    BMP280_WR_FUNC_PTR;

    BMP280_RD_FUNC_PTR;
};

/*!
 * @brief bus data client
*/
struct bmp_data_bus {
    /*!bus communication operation */
    const struct bmp_bus_ops	*bops;
    /*!bmp client */
    void	*client;
};




#define ONE_BYTE 1
#define ONE_WORD 2


#define BMP280_ERR_MSG(msg)

/*!
 * @brief Swap the MS byte and the LS byte in a 16 bit word.
 */
#define SWAP16(val) ((val << 8) | (val >> 8))

/*!
 * @brief Oversampling and Resolution Mode
 */
typedef enum
{
    BMP280_MODE_ULTRA_LOW_POWER = 0,
    BMP280_MODE_LOW_POWER = 1,
    BMP280_MODE_STANDARD = 2,
    BMP280_MODE_HIGH_RESOLUTION = 3,
    BMP280_MODE_ULTRA_HIGH_RESOLUTION = 4,
    BMP280_NUM_MODES
} bmp280_mode_e ;


struct bmp_client_data {

    /*!data bus for hardware communication */
    struct bmp_data_bus data_bus;
    /*!device information used by sensor API */
    struct bmp280_t device;
    /*!device register to kernel device model */
    struct device *dev;
    /*!temperature oversampling variable */
    uint8_t oversampling_t;
    /*!pressure oversampling variable */
    uint8_t oversampling_p;
    /*!indicate operation mode */
    uint8_t op_mode;
    /*!indicate filter coefficient */
    uint8_t filter;
    /*!indicate standby duration */
    uint32_t standbydur;
    /*!indicate work mode */
    uint8_t workmode;
    /*!delay time used by input event */
    uint32_t delay;
    /*!enable/disable sensor output */
    uint32_t enable;
};


/*!
 * @brief  Driver context.
 *
 *  @i2c_handle - i2c handle used to access the I2C bus.
 *  @smgr_handle - sensor manager handle used to notify_data.
 *  @timer - timer object used to delay until conversion completed.
 *  @mode - power and oversampling mode.
 *  @state - idle or measurement in progress.
 *  @chip_id - reported from the device.
 *  @version - reported from the device.
 *  @param_b5 - calculated parameter depends on temperature.
 *  @ut - uncompansated temperature.
 *  @up - uncompansated pressure.
 *  @temperature - in 0.1°C.
 *  @pressure - in Pascal.
 *  @calibration - calibration parameters.
 */
typedef struct  {
    sns_ddf_handle_t    i2c_handle;
    sns_ddf_handle_t    smgr_handle;
    bmp280_mode_e mode;
    uint8_t chip_id;
    struct bmp_client_data data;
    /*! data cache */
      q16_t                   data_cache[2];
	/* odr_reported reported to SMGR */
	uint32_t odr_reported;



} sns_dd_bmp280_state_t;


#endif
