/*  Date: 2015/05/20  17:00:00
 *  Revision: 2.3.17
 */







/*******************************************************************************
 * Copyright (c) 2013, Bosch Sensortec GmbH
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


/*===========================================================================

  S E N S O R S    G Y R O S C O P E    D R I V E R

  DESCRIPTION

  Defines the interface exposed by the qcom Gyroscope sensor driver

  EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



  when           who     what, where, why
  ----------     ---     ------------------------------------------------------
  03/07/12     Albert     create
  02/25/14       MW       Added debug macros
  08/24/2014     vy       Replaced MSG_x by UMSG_x
  09/24/2014     MW       Updated BMG160_CONFIG_SKIP_SAMPLE_CNT_ODRC

  ==========================================================================*/


#ifndef __BMG160_H__
#define __BMG160_H__

#ifndef BMG160_CONFIG_RUN_ON_OSSC
#define BMG160_CONFIG_RUN_ON_OSSC 0
#endif

#include "fixed_point.h"

#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_dd_inertial_test.h"
#include "sns_ddf_signal.h"

#if BMG160_CONFIG_RUN_ON_OSSC || BMG160_CONFIG_ENABLE_COMPILE_CHECK

//#define sns_bmg160_driver_fn_list sns_dd_vendor_if_2
//Include files required to support gyro log packet
#include "sns_log_types_public.h"
#include "sns_log_api_public.h"

#else

#include "sns_debug_str.h"
#include "sns_debug_api.h"

#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"

#endif

//#include <limits.h> // needed to test integer limits


typedef unsigned short  BMG160_U16;       // 16 bit achieved with short
typedef signed short    BMG160_S16;
typedef signed int      BMG160_S32;        // 32 bit achieved with int

typedef struct __attribute__ ((packed)) {
    sns_ddf_axes_map_s    axes_map;
    uint8_t     Reserved[128 - sizeof(sns_ddf_axes_map_s)];    // Reserved Space
    uint32_t    magic_num;                // Magic Number to verify the start of bma2x2 tap settings
} sns_dd_bmg160_nv_params_s;


#ifndef BMG160_CONFIG_ENABLE_VERBOSE_MESSAGE
#define BMG160_CONFIG_ENABLE_VERBOSE_MESSAGE    0
#endif

#ifndef BMG160_CONFIG_ENABLE_LOG_DATA_L1
#define BMG160_CONFIG_ENABLE_LOG_DATA_L1 1
#endif

#ifndef BMG160_CONFIG_ENABLE_LOG_DATA_L2
#define BMG160_CONFIG_ENABLE_LOG_DATA_L2 0
#endif

#ifndef BMG160_CONFIG_ENABLE_UIMAGE
#define BMG160_CONFIG_ENABLE_UIMAGE             0
#endif

#if !BMG160_CONFIG_ENABLE_UIMAGE
#define sns_ddf_malloc_ex(ptr, size, handle)                            sns_ddf_malloc(ptr, size)
#define sns_ddf_memhandler_malloc_ex(mem_handler, size, handle)         sns_ddf_memhandler_malloc(mem_handler, size)
#endif

enum DD_DATA_LOG_CONTEXT {
    DD_DATA_LOG_CONTEXT_POLL = 0,
    DD_DATA_LOG_CONTEXT_DRI,
    DD_DATA_LOG_CONTEXT_FIFO_WMI,
    DD_DATA_LOG_CONTEXT_FIFO_FLUSH,
    DD_DATA_LOG_CONTEXT_FIFO_WMI_IMITATED,
};

//resume this if you want to have debug messages in the log
//#define BMG160_DEBUG

#define BMG160_CONFIG_DBG_LEVEL DBG_MED_PRIO

//#define PRIMITIVE_DEBUG_MSG
#define BMG160_INT_EN_1_EXPECTED_VAL_INT1_PUSHPULL_ACTIVE_HIGH 0X0D
#define DBG_MEDIUM_PRIO DBG_MED_PRIO

#ifdef BMG160_DEBUG        //TODO

#ifndef PRIMITIVE_DEBUG_MSG

#if !BMG160_CONFIG_ENABLE_UIMAGE

#define BMG160_MSG_0(level,msg)                 do { if (DBG_##level##_PRIO >= BMG160_CONFIG_DBG_LEVEL) MSG(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMG160-" msg); } while (0)
#define BMG160_MSG_1(level,msg,p1)              do { if (DBG_##level##_PRIO >= BMG160_CONFIG_DBG_LEVEL) MSG_1(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMG160-" msg,p1); } while (0)
#define BMG160_MSG_2(level,msg,p1,p2)           do { if (DBG_##level##_PRIO >= BMG160_CONFIG_DBG_LEVEL) MSG_2(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMG160-" msg,p1,p2); } while (0)
#define BMG160_MSG_3(level,msg,p1,p2,p3)        do { if (DBG_##level##_PRIO >= BMG160_CONFIG_DBG_LEVEL) MSG_3(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMG160-" msg,p1,p2,p3); } while (0)
#define BMG160_MSG_3_P(level,msg,p1,p2,p3)      do { if (DBG_##level##_PRIO >= BMG160_CONFIG_DBG_LEVEL) MSG_3(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMG160-" msg,p1,p2,p3); } while (0)
#define BMG160_MSG_3_F(level,msg,p1,p2,p3)      do { if (DBG_##level##_PRIO >= BMG160_CONFIG_DBG_LEVEL) MSG_3(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMG160-" msg,p1,p2,p3); } while (0)
#define BMG160_MSG_4(level,msg,p1,p2,p3, p4)    do { if (DBG_##level##_PRIO >= BMG160_CONFIG_DBG_LEVEL) MSG_4(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMG160-" msg,p1,p2,p3,p4); } while (0)

#else   //BMG160_CONFIG_RUN_ON_OSSC

#define BMG160_MSG_0(level,msg)                 do { if (DBG_##level##_PRIO >= BMG160_CONFIG_DBG_LEVEL) UMSG(MSG_SSID_SNS,DBG_##level##_PRIO, "BMG160-" msg); } while (0)
#define BMG160_MSG_1(level,msg,p1)              do { if (DBG_##level##_PRIO >= BMG160_CONFIG_DBG_LEVEL) UMSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "BMG160-" msg,p1); } while (0)
#define BMG160_MSG_2(level,msg,p1,p2            do { if (DBG_##level##_PRIO >= BMG160_CONFIG_DBG_LEVEL) UMSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "BMG160-" msg,p1,p2); } while (0)
#define BMG160_MSG_3(level,msg,p1,p2,p3)        do { if (DBG_##level##_PRIO >= BMG160_CONFIG_DBG_LEVEL) UMSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BMG160-" msg,p1,p2,p3); } while (0)
#define BMG160_MSG_3_P(level,msg,p1,p2,p3)      do { if (DBG_##level##_PRIO >= BMG160_CONFIG_DBG_LEVEL) UMSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BMG160-" msg,p1,p2,p3); } while (0)
#define BMG160_MSG_3_F(level,msg,p1,p2,p3)      do { if (DBG_##level##_PRIO >= BMG160_CONFIG_DBG_LEVEL) UMSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BMG160-" msg,p1,p2,p3); } while (0)
#define BMG160_MSG_4(level,msg,p1,p2,p3, p4)    do { if (DBG_##level##_PRIO >= BMG160_CONFIG_DBG_LEVEL) UMSG_4(MSG_SSID_SNS,DBG_##level##_PRIO, "BMG160-" msg,p1,p2,p3,p4); } while (0)

#endif  //BMG160_CONFIG_RUN_ON_OSSC


#else   //PRIMITIVE_DEBUG_MSG

#define BMG160_MSG_0(level,msg)
#define BMG160_MSG_1(level,msg,p1)
#define BMG160_MSG_2(level,msg,p1,p2)
#define BMG160_MSG_3(level,msg,p1,p2,p3)
#define BMG160_MSG_3_P(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
#define BMG160_MSG_4(level,msg,p1,p2,p3, p4)
#define MED MEDIUM

#endif  //PRIMITIVE_DEBUG_MSG

#else   //BMG160_DEBUG

#define BMG160_MSG_0(level,msg)
#define BMG160_MSG_1(level,msg,p1)
#define BMG160_MSG_2(level,msg,p1,p2)
#define BMG160_MSG_3(level,msg,p1,p2,p3)
#define BMG160_MSG_3_P(level,msg,p1,p2,p3) //MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BMG160-" msg,p1,p2,p3)
#define BMG160_MSG_3_F(level,msg,p1,p2,p3)  //MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BMG160-" msg,p1,p2,p3)
#define BMG160_MSG_4(level,msg,p1,p2,p3, p4)

#endif  //BMG160_DEBUG


#define BMG160_MAX_FIFO_LEVEL 100
#define BMG160_MAX_FIFO_F_BYTES 6
#define FIFO_INTERRUPT		0x10

#define BMG160_SSC_US_2_TICKS(us) sns_ddf_convert_usec_to_tick(us)



#define SNS_DD_GYRO_MAX_RANGE                 5

#define BOSCH_GYRO_SENSOR_RANGE_2000          0
#define BOSCH_GYRO_SENSOR_RANGE_1000          1
#define BOSCH_GYRO_SENSOR_RANGE_500           2
#define BOSCH_GYRO_SENSOR_RANGE_250           3
#define BOSCH_GYRO_SENSOR_RANGE_125           4

#define BOSCH_GYRO_SENSOR_BW_523HZ            0
#define BOSCH_GYRO_SENSOR_BW_230HZ            1
#define BOSCH_GYRO_SENSOR_BW_116HZ            2
#define BOSCH_GYRO_SENSOR_BW_47HZ             3
#define BOSCH_GYRO_SENSOR_BW_23HZ             4
#define BOSCH_GYRO_SENSOR_BW_12HZ             5
#define BOSCH_GYRO_SENSOR_BW_64HZ             6
#define BOSCH_GYRO_SENSOR_BW_32HZ             7

#define BOSCH_GYRO_SENSOR_RANGE_2000_MAX      FX_FLTTOFIX_Q16(2000 * PI /180)
#define BOSCH_GYRO_SENSOR_RANGE_2000_MIN      FX_FLTTOFIX_Q16(-2000 * PI /180)
#define BOSCH_GYRO_SENSOR_RANGE_1000_MAX      FX_FLTTOFIX_Q16(1000 * PI /180)
#define BOSCH_GYRO_SENSOR_RANGE_1000_MIN      FX_FLTTOFIX_Q16(-1000 * PI /180)
#define BOSCH_GYRO_SENSOR_RANGE_500_MAX       FX_FLTTOFIX_Q16(500 * PI /180)
#define BOSCH_GYRO_SENSOR_RANGE_500_MIN       FX_FLTTOFIX_Q16(-500 * PI /180)
#define BOSCH_GYRO_SENSOR_RANGE_250_MAX       FX_FLTTOFIX_Q16(250 * PI /180)
#define BOSCH_GYRO_SENSOR_RANGE_250_MIN       FX_FLTTOFIX_Q16(-250 * PI /180)
#define BOSCH_GYRO_SENSOR_RANGE_125_MAX       FX_FLTTOFIX_Q16(125 * PI /180)
#define BOSCH_GYRO_SENSOR_RANGE_125_MIN       FX_FLTTOFIX_Q16(-125 * PI /180)



#define BOSCH_GYRO_SENSOR_BW_VALUE_523HZ      FX_FLTTOFIX_Q16(523)
#define BOSCH_GYRO_SENSOR_BW_VALUE_230HZ      FX_FLTTOFIX_Q16(230)
#define BOSCH_GYRO_SENSOR_BW_VALUE_116HZ      FX_FLTTOFIX_Q16(116)
#define BOSCH_GYRO_SENSOR_BW_VALUE_47HZ       FX_FLTTOFIX_Q16(47)
#define BOSCH_GYRO_SENSOR_BW_VALUE_23HZ       FX_FLTTOFIX_Q16(23)
#define BOSCH_GYRO_SENSOR_BW_VALUE_12HZ       FX_FLTTOFIX_Q16(12)
#define BOSCH_GYRO_SENSOR_BW_VALUE_64HZ       FX_FLTTOFIX_Q16(64)
#define BOSCH_GYRO_SENSOR_BW_VALUE_32HZ       FX_FLTTOFIX_Q16(32)



sns_ddf_status_e bmg160_sbus_read(
        sns_ddf_handle_t        port_handle,
        uint8_t                 rega,
        uint8_t                 *buf,
        uint8_t                 len,
        uint8_t                 *out);

sns_ddf_status_e bmg160_sbus_write(
        sns_ddf_handle_t        port_handle,
        uint8_t                 rega,
        uint8_t                 *buf,
        uint8_t                 len,
        uint8_t                 *out);


/**\brief defines the calling parameter types of the BMG160_WR_FUNCTION */
#define BMG160_BUS_WR_RETURN_TYPE char

/**\brief links the order of parameters defined in BMG160_BUS_WR_PARAM_TYPE to function calls used inside the API*/
#define BMG160_BUS_WR_PARAM_TYPES unsigned char, unsigned char, unsigned char *, unsigned char

/**\brief links the order of parameters defined in BMG160_BUS_WR_PARAM_TYPE to function calls used inside the API*/
#define BMG160_BUS_WR_PARAM_ORDER device_addr, register_addr, register_data, wr_len

/* never change this line */
#define BMG160_BUS_WRITE_FUNC(device_addr, register_addr, register_data, wr_len )\
    bus_write(device_addr, register_addr, register_data, wr_len )
/* EasyCASE ) */
/* EasyCASE ( 2086
   BMG160_BUS_READ_FUNC */
/**\brief defines the return parameter type of the BMG160_RD_FUNCTION
*/
#define BMG160_BUS_RD_RETURN_TYPE char
/**\brief defines the calling parameter types of the BMG160_RD_FUNCTION
*/
#define BMG160_BUS_RD_PARAM_TYPES unsigned char, unsigned char, unsigned char *, unsigned char
/**\brief links the order of parameters defined in BMG160_BUS_RD_PARAM_TYPE to function calls used inside the API
*/
#define BMG160_BUS_RD_PARAM_ORDER device_addr, register_addr, register_data
/* never change this line */
#define BMG160_BUS_READ_FUNC(device_addr, register_addr, register_data, rd_len )\
    bus_read(device_addr, register_addr, register_data, rd_len )
/* EasyCASE ) */
/* EasyCASE ( 3017
   BMG160_BURST_READ_FUNC */
/**\brief defines the return parameter type of the BMG160_RD_FUNCTION
*/
#define BMG160_BURST_RD_RETURN_TYPE char
/**\brief defines the calling parameter types of the BMG160_RD_FUNCTION
*/
#define BMG160_BURST_RD_PARAM_TYPES unsigned char, unsigned char, unsigned char *, unsigned char
/**\brief links the order of parameters defined in BMG160_BURST_RD_PARAM_TYPE to function calls used inside the API
*/
#define BMG160_BURST_RD_PARAM_ORDER device_addr, register_addr, register_data
/* never change this line */
#define BMG160_BURST_READ_FUNC(device_addr, register_addr, register_data, rd_len )\
    burst_read(device_addr, register_addr, register_data, rd_len )
/* EasyCASE ) */
/* EasyCASE ( 2087
   BMG160_DELAY */
/**\brief defines the return parameter type of the BMG160_DELAY_FUNCTION
*/
#define BMG160_DELAY_RETURN_TYPE void
/**\brief defines the calling parameter types of the BMG160_DELAY_FUNCTION
*/
#define BMG160_DELAY_PARAM_TYPES uint32_t
/* never change this line */
#define BMG160_DELAY_FUNC(delay_in_msec)\
    delay_func(delay_in_msec)
/* EasyCASE ) */
/* EasyCASE ( 2088
   REGISTER ADDRESS */
#define BMG160_RETURN_FUNCTION_TYPE        sns_ddf_status_e

#define BMG160_I2C_ADDR1                0x68
#define BMG160_I2C_ADDR                 BMG160_I2C_ADDR1
#define BMG160_I2C_ADDR2                0x69



/*Define of registers*/

/* Hard Wired */
#define BMG160_CHIP_ID                     0x00            /**<        Address of Chip ID Register               */
#define BMG160_REVISION_ID                 0x01            /**<        Address of Revision ID Register           */

/* Data Register */
#define BMG160_DATAX_LSB                   0x02            /**<        Address of X axis Rate LSB Register       */
#define BMG160_DATAX_MSB                   0x03            /**<        Address of X axis Rate MSB Register       */
#define BMG160_DATAY_LSB                   0x04            /**<        Address of Y axis Rate LSB Register       */
#define BMG160_DATAY_MSB                   0x05            /**<        Address of Y axis Rate MSB Register       */
#define BMG160_DATAZ_LSB                   0x06            /**<        Address of Z axis Rate LSB Register       */
#define BMG160_DATAZ_MSB                   0x07            /**<        Address of Z axis Rate MSB Register       */
#define BMG160_TEMP                        0x08            /**<        Address of Temperature Data LSB Register  */

/* Status Register */
#define BMG160_INT_STATUS0                 0x09            /**<        Address of Interrupt status Register 0    */
#define BMG160_INT_STATUS1                 0x0A            /**<        Address of Interrupt status Register 1    */
#define BMG160_INT_STATUS2                 0x0B            /**<        Address of Interrupt status Register 2    */
#define BMG160_INT_STATUS3                 0x0C            /**<        Address of Interrupt status Register 3    */
#define BMG160_FIFO_STATUS                 0x0E            /**<        Address of FIFO status Register           */

/* Control Register */
#define BMG160_DSP_RANGE_ADDR              0x0F            /**<        Address of DSP Range address Register     */
#define BMG160_DSP_BW_ADDR                 0x10            /**<        Address of DSP Bandwidth Register         */
#define BMG160_PMU_LPW0_ADDR               0x11            /**<        Address of PMU LPW0 Register              */
#define BMG160_MODE_CTRL_REG                       0x11                    /**<        Address of PMU LPW0 Register              */
#define BMG160_PMU_LPW1_ADDR               0x12            /**<        Address of PMU LPW1  Register             */
#define BMG160_RATED_HBW_ADDR              0x13            /**<        Address of DSP HBW Register               */
#define BMG160_BA_SOFTRESET_ADDR           0x14            /**<        Address of BA Softreset Register          */
#define BMG160_INT_ENABLE0                 0x15            /**<        Address of Interrupt Enable 0             */
#define BMG160_INT_ENABLE1                 0x16            /**<        Address of Interrupt Enable 1             */
#define BMG160_INT_MAP_0                   0x17            /**<        Address of Interrupt MAP 0                */
#define BMG160_INT_MAP_1                   0x18            /**<        Address of Interrupt MAP 1                */
#define BMG160_INT_MAP_2                   0x19            /**<        Address of Interrupt MAP 2                */
#define BMG160_INT_0_ADDR                  0x1A            /**<        Address of Interrupt 0 Address register   */
#define BMG160_INT_1_ADDR                  0x1B            /**<        Address of Interrupt 1 Address register   */
#define BMG160_INT_2_ADDR                  0x1C            /**<        Address of Interrupt 2 Address register   */
#define BMG160_INT_3_ADDR                  0x1D            /**<        Address of Interrupt 3 Address register   */
#define BMG160_INT_4_ADDR                  0x1E            /**<        Address of Interrupt 4 Address register   */
#define BMG160_INT_5_ADDR                  0x1F            /**<        Address of Interrupt 5 Address register   */
#define BMG160_INT_6_ADDR                  0x20            /**<        Address of Interrupt 6 Address register   */
#define BMG160_RST_LATCH_ADDR              0x21            /**<        Address of Reset Latch Register           */
#define BMG160_INT_7_ADDR                  0x22            /**<        Address of Interrupt 7 Address register   */
#define BMG160_INT_8_ADDR                  0x23            /**<        Address of Interrupt 8 Address register   */
#define BMG160_INT_9_ADDR                  0x24            /**<        Address of Interrupt 9 Address register   */
#define BMG160_INT_10_ADDR                 0x25            /**<        Address of Interrupt 10 Address register  */
#define BMG160_INT_11_ADDR                 0x26            /**<        Address of Interrupt 11 Address register  */
#define BMG160_INT_12_ADDR                 0x27            /**<        Address of Interrupt 12 Address register  */
#define BMG160_INT_13_ADDR                 0x28            /**<        Address of Interrupt 13 Address register  */
#define BMG160_INT_14_ADDR                 0x29            /**<        Address of Interrupt 14 Address register  */
#define BMG160_INT_15_ADDR                 0x2A            /**<        Address of Interrupt 15 Address register  */
#define BMG160_INT_16_ADDR                 0x2B            /**<        Address of Interrupt 16 Address register  */
#define BMG160_INT_17_ADDR                 0x2C            /**<        Address of Interrupt 17 Address register  */
#define BMG160_INT_19_ADDR                 0x2D            /**<        Address of Interrupt 19 Address register  */
#define BMG160_INT_20_ADDR                 0x2E            /**<        Address of Interrupt 20 Address register  */
#define BMG160_INT_21_ADDR                 0x2F            /**<        Address of Interrupt 21 Address register  */
#define BMG160_INT_22_ADDR                 0x30            /**<        Address of Interrupt 22 Address register  */
#define BMG160_INT_OFF0_ADDR               0x31            /**<        Address of Interrupt OFF0 register        */
#define BMG160_INT_OFF1_ADDR               0x32            /**<        Address of Interrupt OFF1 Register        */
#define BMG160_TRIM_NVM_CTRL_ADDR          0x33            /**<        Address of Trim NVM control register      */
#define BMG160_BA_SPI3_WDT_ADDR            0x34            /**<        Address of BA SPI3,WDT Register           */
#define BMG160_INL_OFFSET_MSB_ADDR         0x36            /**<        Address of INL Offset MSB Register        */

/* Trim Register */
#define BMG160_INL_OFFSET_X                0x37            /**<        Address of INL Offset X Register          */
#define BMG160_INL_OFFSET_Y                0x38            /**<        Address of INL Offset Y Register          */
#define BMG160_INL_OFFSET_Z                0x39            /**<        Address of INL Offset Z Register          */
#define BMG160_TRIM_GP0_ADDR               0x3A            /**<        Address of Trim GP0 Register              */
#define BMG160_TRIM_GP1_ADDR               0x3B            /**<        Address of Trim GP1 Register              */
#define BMG160_FPGA_REV_ADDR               0x3C            /**<        Address of FPGA Rev Register              */

/* Control Register */
#define BMG160_FIFO_CGF1_ADDR              0x3D            /**<        Address of FIFO CGF0 Register             */
#define BMG160_FIFO_CGF0_ADDR              0x3E            /**<        Address of FIFO CGF1 Register             */

/* Data Register */
#define BMG160_FIFO_DATA_ADDR              0x3F            /**<        Address of FIFO Data Register             */
/* EasyCASE ) */
/* EasyCASE ( 2089
   BIT WISE REGISTER */
/* EasyCASE ( 2090
   Data Registers */
/* Rate X LSB Regsiter */
#define BMG160_DATAX_LSB_VALUEX__POS        0                       /**< Last 6 bits of RateX LSB Registers */
#define BMG160_DATAX_LSB_VALUEX__LEN        8
#define BMG160_DATAX_LSB_VALUEX__MSK        0xFF
#define BMG160_DATAX_LSB_VALUEX__REG        BMG160_DATAX_LSB

/* Rate Y LSB Regsiter */
#define BMG160_DATAY_LSB_VALUEY__POS        0                       /**<  Last 6 bits of RateY LSB Registers */
#define BMG160_DATAY_LSB_VALUEY__LEN        8
#define BMG160_DATAY_LSB_VALUEY__MSK        0xFF
#define BMG160_DATAY_LSB_VALUEY__REG        BMG160_DATAY_LSB

/* Rate Z LSB Regsiter */
#define BMG160_DATAZ_LSB_VALUEZ__POS        0                       /**< Last 6 bits of RateZ LSB Registers */
#define BMG160_DATAZ_LSB_VALUEZ__LEN        8
#define BMG160_DATAZ_LSB_VALUEZ__MSK        0xFF
#define BMG160_DATAZ_LSB_VALUEZ__REG        BMG160_DATAZ_LSB

/* EasyCASE ) */
/* EasyCASE ( 2091
   Status Registers */
/* EasyCASE ( 2092
   Interrupt Status0 */
/* Interrupt status 0 Regsiter */

#define BMG160_INT_STATUS0_ANY_INT__POS     2                       /**< 2th bit of Interrupt status 0 register */
#define BMG160_INT_STATUS0_ANY_INT__LEN     1
#define BMG160_INT_STATUS0_ANY_INT__MSK     0x04
#define BMG160_INT_STATUS0_ANY_INT__REG     BMG160_INT_STATUS0

#define BMG160_INT_STATUS0_HIGH_INT__POS    1                       /**< 1th bit of Interrupt status 0 register */
#define BMG160_INT_STATUS0_HIGH_INT__LEN    1
#define BMG160_INT_STATUS0_HIGH_INT__MSK    0x02
#define BMG160_INT_STATUS0_HIGH_INT__REG    BMG160_INT_STATUS0
/* EasyCASE ) */
/* EasyCASE ( 2093
   Interrupt Status1 */
/* Interrupt status 1 Regsiter */
#define BMG160_INT_STATUS1_DATA_INT__POS           7                       /**< 7th bit of Interrupt status 1 register */
#define BMG160_INT_STATUS1_DATA_INT__LEN           1
#define BMG160_INT_STATUS1_DATA_INT__MSK           0x80
#define BMG160_INT_STATUS1_DATA_INT__REG           BMG160_INT_STATUS1

#define BMG160_INT_STATUS1_AUTO_OFFSET_INT__POS    6                       /**< 6th bit of Interrupt status 1 register */
#define BMG160_INT_STATUS1_AUTO_OFFSET_INT__LEN    1
#define BMG160_INT_STATUS1_AUTO_OFFSET_INT__MSK    0x40
#define BMG160_INT_STATUS1_AUTO_OFFSET_INT__REG    BMG160_INT_STATUS1

#define BMG160_INT_STATUS1_FAST_OFFSET_INT__POS    5                       /**< 5th bit of Interrupt status 1 register */
#define BMG160_INT_STATUS1_FAST_OFFSET_INT__LEN    1
#define BMG160_INT_STATUS1_FAST_OFFSET_INT__MSK    0x20
#define BMG160_INT_STATUS1_FAST_OFFSET_INT__REG    BMG160_INT_STATUS1

#define BMG160_INT_STATUS1_FIFO_INT__POS           4                       /**< 4th bit of Interrupt status 1 register */
#define BMG160_INT_STATUS1_FIFO_INT__LEN           1
#define BMG160_INT_STATUS1_FIFO_INT__MSK           0x10
#define BMG160_INT_STATUS1_FIFO_INT__REG           BMG160_INT_STATUS1
/* EasyCASE ) */
/* EasyCASE ( 2094
   Interrupt Status2 */
/* Interrupt status 2 Regsiter */


#define BMG160_INT_STATUS2_ANY_SIGN_INT__POS     3                       /**< 3th bit of Interrupt status 2 register */
#define BMG160_INT_STATUS2_ANY_SIGN_INT__LEN     1
#define BMG160_INT_STATUS2_ANY_SIGN_INT__MSK     0x08
#define BMG160_INT_STATUS2_ANY_SIGN_INT__REG     BMG160_INT_STATUS2

#define BMG160_INT_STATUS2_ANY_FIRSTZ_INT__POS   2                       /**< 2th bit of Interrupt status 2 register */
#define BMG160_INT_STATUS2_ANY_FIRSTZ_INT__LEN   1
#define BMG160_INT_STATUS2_ANY_FIRSTZ_INT__MSK   0x04
#define BMG160_INT_STATUS2_ANY_FIRSTZ_INT__REG   BMG160_INT_STATUS2

#define BMG160_INT_STATUS2_ANY_FIRSTY_INT__POS   1                       /**< 1st bit of Interrupt status 2 register */
#define BMG160_INT_STATUS2_ANY_FIRSTY_INT__LEN   1
#define BMG160_INT_STATUS2_ANY_FIRSTY_INT__MSK   0x02
#define BMG160_INT_STATUS2_ANY_FIRSTY_INT__REG   BMG160_INT_STATUS2

#define BMG160_INT_STATUS2_ANY_FIRSTX_INT__POS   0                       /**< 0th bit of Interrupt status 2 register */
#define BMG160_INT_STATUS2_ANY_FIRSTX_INT__LEN   1
#define BMG160_INT_STATUS2_ANY_FIRSTX_INT__MSK   0x01
#define BMG160_INT_STATUS2_ANY_FIRSTX_INT__REG   BMG160_INT_STATUS2
/* EasyCASE ) */
/* EasyCASE ( 2095
   Interrupt Status3 */
/* Interrupt status 3 Regsiter */

#define BMG160_INT_STATUS3_HIGH_SIGN_INT__POS     3                       /**< 3th bit of Interrupt status 3 register */
#define BMG160_INT_STATUS3_HIGH_SIGN_INT__LEN     1
#define BMG160_INT_STATUS3_HIGH_SIGN_INT__MSK     0x08
#define BMG160_INT_STATUS3_HIGH_SIGN_INT__REG     BMG160_INT_STATUS3

#define BMG160_INT_STATUS3_HIGH_FIRSTZ_INT__POS   2                       /**< 2th bit of Interrupt status 3 register */
#define BMG160_INT_STATUS3_HIGH_FIRSTZ_INT__LEN   1
#define BMG160_INT_STATUS3_HIGH_FIRSTZ_INT__MSK   0x04
#define BMG160_INT_STATUS3_HIGH_FIRSTZ_INT__REG  BMG160_INT_STATUS3

#define BMG160_INT_STATUS3_HIGH_FIRSTY_INT__POS   1                       /**< 1st bit of Interrupt status 3 register */
#define BMG160_INT_STATUS3_HIGH_FIRSTY_INT__LEN   1
#define BMG160_INT_STATUS3_HIGH_FIRSTY_INT__MSK   0x02
#define BMG160_INT_STATUS3_HIGH_FIRSTY_INT__REG   BMG160_INT_STATUS3

#define BMG160_INT_STATUS3_HIGH_FIRSTX_INT__POS   0                       /**< 0th bit of Interrupt status 3 register */
#define BMG160_INT_STATUS3_HIGH_FIRSTX_INT__LEN   1
#define BMG160_INT_STATUS3_HIGH_FIRSTX_INT__MSK   0x01
#define BMG160_INT_STATUS3_HIGH_FIRSTX_INT__REG   BMG160_INT_STATUS3
/* EasyCASE ) */
/* EasyCASE ( 2547
   FIFO Status */
/* BMG160 FIFO Status Regsiter */
#define BMG160_FIFO_STATUS_OVERRUN__POS         7                       /**< 7th bit of FIFO status Regsiter */
#define BMG160_FIFO_STATUS_OVERRUN__LEN         1
#define BMG160_FIFO_STATUS_OVERRUN__MSK         0x80
#define BMG160_FIFO_STATUS_OVERRUN__REG         BMG160_FIFO_STATUS

#define BMG160_FIFO_STATUS_FRAME_COUNTER__POS   0                       /**< First 7 bits of FIFO status Regsiter */
#define BMG160_FIFO_STATUS_FRAME_COUNTER__LEN   7
#define BMG160_FIFO_STATUS_FRAME_COUNTER__MSK   0x7F
#define BMG160_FIFO_STATUS_FRAME_COUNTER__REG   BMG160_FIFO_STATUS
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2097
   Control Register */
/* EasyCASE ( 2098
   BMG160_DSP_RANGE_ADDR */
#define BMG160_DSP_RANGE_ADDR_RANGE__POS           0                       /**< First 3 bits of DSP range Registers */
#define BMG160_DSP_RANGE_ADDR_RANGE__LEN           3
#define BMG160_DSP_RANGE_ADDR_RANGE__MSK           0x07
#define BMG160_DSP_RANGE_ADDR_RANGE__REG           BMG160_DSP_RANGE_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2099
   BMG160_DSP_BW_ADDR */
#define BMG160_DSP_BW_ADDR_HIGH_RES__POS       7                       /**< Last bit of DSP Bandwidth Registers */
#define BMG160_DSP_BW_ADDR_HIGH_RES__LEN       1
#define BMG160_DSP_BW_ADDR_HIGH_RES__MSK       0x80
#define BMG160_DSP_BW_ADDR_HIGH_RES__REG       BMG160_DSP_BW_ADDR

#define BMG160_DSP_BW_ADDR_BW__POS             0                       /**< First 4 bits of DSP Bandwidth Registers */
#define BMG160_DSP_BW_ADDR_BW__LEN             3
#define BMG160_DSP_BW_ADDR_BW__MSK             0x07
#define BMG160_DSP_BW_ADDR_BW__REG             BMG160_DSP_BW_ADDR

#define BMG160_DSP_BW_ADDR_IMG_STB__POS             6                       /**< First 4 bits of DSP Bandwidth Registers */
#define BMG160_DSP_BW_ADDR_IMG_STB__LEN             1
#define BMG160_DSP_BW_ADDR_IMG_STB__MSK             0x40
#define BMG160_DSP_BW_ADDR_IMG_STB__REG             BMG160_DSP_BW_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2100
   BMG160_PMU_LPW0_ADDR */
#define BMG160_MODE_CTRL__POS             5
#define BMG160_MODE_CTRL__LEN             3
#define BMG160_MODE_CTRL__MSK             0xE0
#define BMG160_MODE_CTRL__REG             BMG160_MODE_CTRL_REG
/* EasyCASE ) */
/* EasyCASE ( 2551
   BMG160_PMU_LPW1_ADDR */
#define BMG160_PMU_LPW1_ADDR_FAST_POWERUP__POS         7                       /**< 7th bit of PMU LPW1 Register */
#define BMG160_PMU_LPW1_ADDR_FAST_POWERUP__LEN         1
#define BMG160_PMU_LPW1_ADDR_FAST_POWERUP__MSK         0x80
#define BMG160_PMU_LPW1_ADDR_FAST_POWERUP__REG         BMG160_PMU_LPW1_ADDR

#define BMG160_PMU_LPW1_ADDR_EXT_TRI_SEL__POS          4                       /**< 4th & 5th bit of PMU LPW1 Register */
#define BMG160_PMU_LPW1_ADDR_EXT_TRI_SEL__LEN          2
#define BMG160_PMU_LPW1_ADDR_EXT_TRI_SEL__MSK          0x30
#define BMG160_PMU_LPW1_ADDR_EXT_TRI_SEL__REG          BMG160_PMU_LPW1_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2101
   BMG160_DSP_HBW_ADDR */
#define BMG160_RATED_HBW_ADDR_DATA_HIGHBW__POS         7                       /**< 7th bit of DSP HBW Register */
#define BMG160_RATED_HBW_ADDR_DATA_HIGHBW__LEN         1
#define BMG160_RATED_HBW_ADDR_DATA_HIGHBW__MSK         0x80
#define BMG160_RATED_HBW_ADDR_DATA_HIGHBW__REG         BMG160_RATED_HBW_ADDR

#define BMG160_RATED_HBW_ADDR_SHADOW_DIS__POS          6                       /**< 6th bit of DSP HBW Register */
#define BMG160_RATED_HBW_ADDR_SHADOW_DIS__LEN          1
#define BMG160_RATED_HBW_ADDR_SHADOW_DIS__MSK          0x40
#define BMG160_RATED_HBW_ADDR_SHADOW_DIS__REG          BMG160_RATED_HBW_ADDR

#define BMG160_RATED_HBW_ADDR_FPGA_BYPASS_CG__POS      5                       /**< 5th bit of DSP HBW Register */
#define BMG160_RATED_HBW_ADDR_FPGA_BYPASS_CG__LEN      1
#define BMG160_RATED_HBW_ADDR_FPGA_BYPASS_CG__MSK      0x20
#define BMG160_RATED_HBW_ADDR_FPGA_BYPASS_CG__REG      BMG160_RATED_HBW_ADDR

#define BMG160_RATED_HBW_ADDR_FPGA_BYPASS_DSP__POS     4                       /**< 4th bit of DSP HBW Register */
#define BMG160_RATED_HBW_ADDR_FPGA_BYPASS_DSP__LEN     1
#define BMG160_RATED_HBW_ADDR_FPGA_BYPASS_DSP__MSK     0x10
#define BMG160_RATED_HBW_ADDR_FPGA_BYPASS_DSP__REG     BMG160_RATED_HBW_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2864
   BMG160_INT_ENABLE_0 */
#define BMG160_INT_ENABLE0_DATAEN__POS               7                       /**< 7th bit of Interrupt Enable 0 Registers */
#define BMG160_INT_ENABLE0_DATAEN__LEN               1
#define BMG160_INT_ENABLE0_DATAEN__MSK               0x80
#define BMG160_INT_ENABLE0_DATAEN__REG               BMG160_INT_ENABLE0

#define BMG160_INT_ENABLE0_FIFOEN__POS               6                       /**< 6th bit of Interrupt Enable 0 Registers */
#define BMG160_INT_ENABLE0_FIFOEN__LEN               1
#define BMG160_INT_ENABLE0_FIFOEN__MSK               0x40
#define BMG160_INT_ENABLE0_FIFOEN__REG               BMG160_INT_ENABLE0

#define BMG160_INT_ENABLE0_S_TAP_EN__POS             5                       /**< 5th bit of Interrupt Enable 0 Registers */
#define BMG160_INT_ENABLE0_S_TAP_EN__LEN             1
#define BMG160_INT_ENABLE0_S_TAP_EN__MSK             0x20
#define BMG160_INT_ENABLE0_S_TAP_EN__REG             BMG160_INT_ENABLE0

#define BMG160_INT_ENABLE0_D_TAP_EN__POS             4                       /**< 4th bit of Interrupt Enable 0 Registers */
#define BMG160_INT_ENABLE0_D_TAP_EN__LEN             1
#define BMG160_INT_ENABLE0_D_TAP_EN__MSK             0x10
#define BMG160_INT_ENABLE0_D_TAP_EN__REG             BMG160_INT_ENABLE0

#define BMG160_INT_ENABLE0_AUTO_OFFSETEN__POS        2                       /**< 2nd bit of Interrupt Enable 0 Registers */
#define BMG160_INT_ENABLE0_AUTO_OFFSETEN__LEN        1
#define BMG160_INT_ENABLE0_AUTO_OFFSETEN__MSK        0x04
#define BMG160_INT_ENABLE0_AUTO_OFFSETEN__REG        BMG160_INT_ENABLE0
/* EasyCASE ) */
/* EasyCASE ( 2557
   BMG160_INT_ENABLE_1 */
#define BMG160_INT_ENABLE1_IT2_OD__POS               3                       /**< 3rd bit of Interrupt Enable 1 Registers */
#define BMG160_INT_ENABLE1_IT2_OD__LEN               1
#define BMG160_INT_ENABLE1_IT2_OD__MSK               0x08
#define BMG160_INT_ENABLE1_IT2_OD__REG               BMG160_INT_ENABLE1

#define BMG160_INT_ENABLE1_IT2_LVL__POS              2                       /**< 2nd bit of Interrupt Enable 1 Registers */
#define BMG160_INT_ENABLE1_IT2_LVL__LEN              1
#define BMG160_INT_ENABLE1_IT2_LVL__MSK              0x04
#define BMG160_INT_ENABLE1_IT2_LVL__REG              BMG160_INT_ENABLE1

#define BMG160_INT_ENABLE1_IT1_OD__POS               1                       /**< 1st bit of Interrupt Enable 1 Registers */
#define BMG160_INT_ENABLE1_IT1_OD__LEN               1
#define BMG160_INT_ENABLE1_IT1_OD__MSK               0x02
#define BMG160_INT_ENABLE1_IT1_OD__REG               BMG160_INT_ENABLE1

#define BMG160_INT_ENABLE1_IT1_LVL__POS              0                       /**< 0th bit of Interrupt Enable 1 Registers */
#define BMG160_INT_ENABLE1_IT1_LVL__LEN              1
#define BMG160_INT_ENABLE1_IT1_LVL__MSK              0x01
#define BMG160_INT_ENABLE1_IT1_LVL__REG              BMG160_INT_ENABLE1
/* EasyCASE ) */
/* EasyCASE ( 2104
   BMG160_INT_MAP_0 */
#define BMG160_INT_MAP_0_INT1_TURN1__POS           7                       /**< 7th bit of Interrupt MAP 0 Registers */
#define BMG160_INT_MAP_0_INT1_TURN1__LEN           1
#define BMG160_INT_MAP_0_INT1_TURN1__MSK           0x80
#define BMG160_INT_MAP_0_INT1_TURN1__REG           BMG160_INT_MAP_0

#define BMG160_INT_MAP_0_INT1_TURN2__POS           6                       /**< 6th bit of Interrupt MAP 0 Registers */
#define BMG160_INT_MAP_0_INT1_TURN2__LEN           1
#define BMG160_INT_MAP_0_INT1_TURN2__MSK           0x40
#define BMG160_INT_MAP_0_INT1_TURN2__REG           BMG160_INT_MAP_0

#define BMG160_INT_MAP_0_INT1_S_TAP__POS           5                       /**< 5th bit of Interrupt MAP 0 Registers */
#define BMG160_INT_MAP_0_INT1_S_TAP__LEN           1
#define BMG160_INT_MAP_0_INT1_S_TAP__MSK           0x20
#define BMG160_INT_MAP_0_INT1_S_TAP__REG           BMG160_INT_MAP_0

#define BMG160_INT_MAP_0_INT1_D_TAP__POS           4                       /**< 4th bit of Interrupt MAP 0 Registers */
#define BMG160_INT_MAP_0_INT1_D_TAP__LEN           1
#define BMG160_INT_MAP_0_INT1_D_TAP__MSK           0x10
#define BMG160_INT_MAP_0_INT1_D_TAP__REG           BMG160_INT_MAP_0

#define BMG160_INT_MAP_0_INT1_HIGH__POS            3                       /**< 3rd bit of Interrupt MAP 0 Registers */
#define BMG160_INT_MAP_0_INT1_HIGH__LEN            1
#define BMG160_INT_MAP_0_INT1_HIGH__MSK            0x08
#define BMG160_INT_MAP_0_INT1_HIGH__REG            BMG160_INT_MAP_0

#define BMG160_INT_MAP_0_INT1_CONST__POS           2                       /**< 2nd bit of Interrupt MAP 0 Registers */
#define BMG160_INT_MAP_0_INT1_CONST__LEN           1
#define BMG160_INT_MAP_0_INT1_CONST__MSK           0x04
#define BMG160_INT_MAP_0_INT1_CONST__REG           BMG160_INT_MAP_0

#define BMG160_INT_MAP_0_INT1_ANY__POS             1                       /**< 1st bit of Interrupt MAP 0 Registers */
#define BMG160_INT_MAP_0_INT1_ANY__LEN             1
#define BMG160_INT_MAP_0_INT1_ANY__MSK             0x02
#define BMG160_INT_MAP_0_INT1_ANY__REG             BMG160_INT_MAP_0

#define BMG160_INT_MAP_0_INT1_SHAKE__POS           0                       /**< 0th bit of Interrupt MAP 0 Registers */
#define BMG160_INT_MAP_0_INT1_SHAKE__LEN           1
#define BMG160_INT_MAP_0_INT1_SHAKE__MSK           0x01
#define BMG160_INT_MAP_0_INT1_SHAKE__REG           BMG160_INT_MAP_0
/* EasyCASE ) */
/* EasyCASE ( 2105
   BMG160_INT_MAP_1 */
#define BMG160_MAP_1_INT2_DATA__POS                  7                       /**< 7th bit of MAP_1Registers */
#define BMG160_MAP_1_INT2_DATA__LEN                  1
#define BMG160_MAP_1_INT2_DATA__MSK                  0x80
#define BMG160_MAP_1_INT2_DATA__REG                  BMG160_INT_MAP_1

#define BMG160_MAP_1_INT2_FAST_OFFSET__POS           6                       /**< 6th bit of MAP_1Registers */
#define BMG160_MAP_1_INT2_FAST_OFFSET__LEN           1
#define BMG160_MAP_1_INT2_FAST_OFFSET__MSK           0x40
#define BMG160_MAP_1_INT2_FAST_OFFSET__REG           BMG160_INT_MAP_1

#define BMG160_MAP_1_INT2_FIFO__POS                  5                       /**< 5th bit of MAP_1Registers */
#define BMG160_MAP_1_INT2_FIFO__LEN                  1
#define BMG160_MAP_1_INT2_FIFO__MSK                  0x20
#define BMG160_MAP_1_INT2_FIFO__REG                  BMG160_INT_MAP_1

#define BMG160_MAP_1_INT2_AUTO_OFFSET__POS           4                       /**< 4th bit of MAP_1Registers */
#define BMG160_MAP_1_INT2_AUTO_OFFSET__LEN           1
#define BMG160_MAP_1_INT2_AUTO_OFFSET__MSK           0x10
#define BMG160_MAP_1_INT2_AUTO_OFFSET__REG           BMG160_INT_MAP_1

#define BMG160_MAP_1_INT1_AUTO_OFFSET__POS           3                       /**< 3rd bit of MAP_1Registers */
#define BMG160_MAP_1_INT1_AUTO_OFFSET__LEN           1
#define BMG160_MAP_1_INT1_AUTO_OFFSET__MSK           0x08
#define BMG160_MAP_1_INT1_AUTO_OFFSET__REG           BMG160_INT_MAP_1

#define BMG160_MAP_1_INT1_FIFO__POS                  2                       /**< 2nd bit of MAP_1Registers */
#define BMG160_MAP_1_INT1_FIFO__LEN                  1
#define BMG160_MAP_1_INT1_FIFO__MSK                  0x04
#define BMG160_MAP_1_INT1_FIFO__REG                  BMG160_INT_MAP_1

#define BMG160_MAP_1_INT1_FAST_OFFSET__POS           1                       /**< 1st bit of MAP_1Registers */
#define BMG160_MAP_1_INT1_FAST_OFFSET__LEN           1
#define BMG160_MAP_1_INT1_FAST_OFFSET__MSK           0x02
#define BMG160_MAP_1_INT1_FAST_OFFSET__REG           BMG160_INT_MAP_1

#define BMG160_MAP_1_INT1_DATA__POS                  0                       /**< 0th bit of MAP_1Registers */
#define BMG160_MAP_1_INT1_DATA__LEN                  1
#define BMG160_MAP_1_INT1_DATA__MSK                  0x01
#define BMG160_MAP_1_INT1_DATA__REG                  BMG160_INT_MAP_1
/* EasyCASE ) */
/* EasyCASE ( 2106
   BMG160_INT_MAP_2 */
#define BMG160_INT_MAP_2_INT2_TURN1__POS           7                       /**< 7th bit of Interrupt Map 2 Registers */
#define BMG160_INT_MAP_2_INT2_TURN1__LEN           1
#define BMG160_INT_MAP_2_INT2_TURN1__MSK           0x80
#define BMG160_INT_MAP_2_INT2_TURN1__REG           BMG160_INT_MAP_2

#define BMG160_INT_MAP_2_INT2_TURN2__POS           6                       /**< 6th bit of Interrupt Map 2 Registers */
#define BMG160_INT_MAP_2_INT2_TURN2__LEN           1
#define BMG160_INT_MAP_2_INT2_TURN2__MSK           0x40
#define BMG160_INT_MAP_2_INT2_TURN2__REG           BMG160_INT_MAP_2

#define BMG160_INT_MAP_2_INT2_S_TAP__POS           5                       /**< 5th bit of Interrupt Map 2 Registers */
#define BMG160_INT_MAP_2_INT2_S_TAP__LEN           1
#define BMG160_INT_MAP_2_INT2_S_TAP__MSK           0x20
#define BMG160_INT_MAP_2_INT2_S_TAP__REG           BMG160_INT_MAP_2

#define BMG160_INT_MAP_2_INT2_D_TAP__POS           4                       /**< 4th bit of Interrupt Map 2 Registers */
#define BMG160_INT_MAP_2_INT2_D_TAP__LEN           1
#define BMG160_INT_MAP_2_INT2_D_TAP__MSK           0x10
#define BMG160_INT_MAP_2_INT2_D_TAP__REG           BMG160_INT_MAP_2

#define BMG160_INT_MAP_2_INT2_HIGH__POS            3                       /**< 3rd bit of Interrupt Map 2 Registers */
#define BMG160_INT_MAP_2_INT2_HIGH__LEN            1
#define BMG160_INT_MAP_2_INT2_HIGH__MSK            0x08
#define BMG160_INT_MAP_2_INT2_HIGH__REG            BMG160_INT_MAP_2

#define BMG160_INT_MAP_2_INT2_CONST__POS           2                       /**< 2nd bit of Interrupt Map 2 Registers */
#define BMG160_INT_MAP_2_INT2_CONST__LEN           1
#define BMG160_INT_MAP_2_INT2_CONST__MSK           0x04
#define BMG160_INT_MAP_2_INT2_CONST__REG           BMG160_INT_MAP_2

#define BMG160_INT_MAP_2_INT2_ANY__POS             1                       /**< 1st bit of Interrupt Map 2 Registers */
#define BMG160_INT_MAP_2_INT2_ANY__LEN             1
#define BMG160_INT_MAP_2_INT2_ANY__MSK             0x02
#define BMG160_INT_MAP_2_INT2_ANY__REG             BMG160_INT_MAP_2

#define BMG160_INT_MAP_2_INT2_SHAKE__POS           0                       /**< 0th bit of Interrupt Map 2 Registers */
#define BMG160_INT_MAP_2_INT2_SHAKE__LEN           1
#define BMG160_INT_MAP_2_INT2_SHAKE__MSK           0x01
#define BMG160_INT_MAP_2_INT2_SHAKE__REG           BMG160_INT_MAP_2
/* EasyCASE ) */
/* EasyCASE ( 2107
   BMG160_INT_0_ADDR */
#define BMG160_INT_0_ADDR_TURN1_UNFITDATA__POS            7                       /**< 7th bit of Interrupt 0 Registers */
#define BMG160_INT_0_ADDR_TURN1_UNFITDATA__LEN            1
#define BMG160_INT_0_ADDR_TURN1_UNFITDATA__MSK            0x80
#define BMG160_INT_0_ADDR_TURN1_UNFITDATA__REG            BMG160_INT_0_ADDR

#define BMG160_INT_0_ADDR_TURN2_UNFITDATA__POS            6                       /**< 6th bit of Interrupt 0 Registers */
#define BMG160_INT_0_ADDR_TURN2_UNFITDATA__LEN            1
#define BMG160_INT_0_ADDR_TURN2_UNFITDATA__MSK            0x40
#define BMG160_INT_0_ADDR_TURN2_UNFITDATA__REG            BMG160_INT_0_ADDR

#define BMG160_INT_0_ADDR_SLOW_OFFSET_UNFIT__POS          5                       /**< 5th bit of Interrupt 0 Registers */
#define BMG160_INT_0_ADDR_SLOW_OFFSET_UNFIT__LEN          1
#define BMG160_INT_0_ADDR_SLOW_OFFSET_UNFIT__MSK          0x20
#define BMG160_INT_0_ADDR_SLOW_OFFSET_UNFIT__REG          BMG160_INT_0_ADDR

#define BMG160_INT_0_ADDR_TAP_UNFIT_DATA__POS             4                       /**< 4th bit of Interrupt 0 Registers */
#define BMG160_INT_0_ADDR_TAP_UNFIT_DATA__LEN             1
#define BMG160_INT_0_ADDR_TAP_UNFIT_DATA__MSK             0x10
#define BMG160_INT_0_ADDR_TAP_UNFIT_DATA__REG             BMG160_INT_0_ADDR

#define BMG160_INT_0_ADDR_HIGH_UNFIT_DATA__POS            3                       /**< 3rd bit of Interrupt 0 Registers */
#define BMG160_INT_0_ADDR_HIGH_UNFIT_DATA__LEN            1
#define BMG160_INT_0_ADDR_HIGH_UNFIT_DATA__MSK            0x08
#define BMG160_INT_0_ADDR_HIGH_UNFIT_DATA__REG            BMG160_INT_0_ADDR

#define BMG160_INT_0_ADDR_CONST_UNFIT_DATA__POS           2                       /**< 2nd bit of Interrupt 0 Registers */
#define BMG160_INT_0_ADDR_CONST_UNFIT_DATA__LEN           1
#define BMG160_INT_0_ADDR_CONST_UNFIT_DATA__MSK           0x04
#define BMG160_INT_0_ADDR_CONST_UNFIT_DATA__REG           BMG160_INT_0_ADDR

#define BMG160_INT_0_ADDR_ANY_UNFIT_DATA__POS             1                       /**< 1st bit of Interrupt 0 Registers */
#define BMG160_INT_0_ADDR_ANY_UNFIT_DATA__LEN             1
#define BMG160_INT_0_ADDR_ANY_UNFIT_DATA__MSK             0x02
#define BMG160_INT_0_ADDR_ANY_UNFIT_DATA__REG             BMG160_INT_0_ADDR

#define BMG160_INT_0_ADDR_SHAKE_UNFIT_DATA__POS           0                       /**< 0th bit of Interrupt 0 Registers */
#define BMG160_INT_0_ADDR_SHAKE_UNFIT_DATA__LEN           1
#define BMG160_INT_0_ADDR_SHAKE_UNFIT_DATA__MSK           0x01
#define BMG160_INT_0_ADDR_SHAKE_UNFIT_DATA__REG           BMG160_INT_0_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2108
   BMG160_INT_1_ADDR */
#define BMG160_INT_1_ADDR_FAST_OFFSET_UNFIT__POS            7                       /**< 7th bit of INT_1  Registers */
#define BMG160_INT_1_ADDR_FAST_OFFSET_UNFIT__LEN            1
#define BMG160_INT_1_ADDR_FAST_OFFSET_UNFIT__MSK            0x80
#define BMG160_INT_1_ADDR_FAST_OFFSET_UNFIT__REG            BMG160_INT_1_ADDR

#define BMG160_INT_1_ADDR_ANY_TH__POS                       0                       /**< First 7 bits of INT_1  Registers */
#define BMG160_INT_1_ADDR_ANY_TH__LEN                       7
#define BMG160_INT_1_ADDR_ANY_TH__MSK                       0x7F
#define BMG160_INT_1_ADDR_ANY_TH__REG                       BMG160_INT_1_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2109
   BMG160_INT_2_ADDR */
#define BMG160_INT_2_ADDR_AWAKE_DUR__POS          6                       /**< Last 2 bits of INT 2Registers */
#define BMG160_INT_2_ADDR_AWAKE_DUR__LEN          2
#define BMG160_INT_2_ADDR_AWAKE_DUR__MSK          0xC0
#define BMG160_INT_2_ADDR_AWAKE_DUR__REG          BMG160_INT_2_ADDR

#define BMG160_INT_2_ADDR_ANY_DURSAMPLE__POS      4                       /**< 4th & 5th bit of INT 2Registers */
#define BMG160_INT_2_ADDR_ANY_DURSAMPLE__LEN      2
#define BMG160_INT_2_ADDR_ANY_DURSAMPLE__MSK      0x30
#define BMG160_INT_2_ADDR_ANY_DURSAMPLE__REG      BMG160_INT_2_ADDR

#define BMG160_INT_2_ADDR_ANY_EN_Z__POS           2                       /**< 2nd bit of INT 2Registers */
#define BMG160_INT_2_ADDR_ANY_EN_Z__LEN           1
#define BMG160_INT_2_ADDR_ANY_EN_Z__MSK           0x04
#define BMG160_INT_2_ADDR_ANY_EN_Z__REG           BMG160_INT_2_ADDR

#define BMG160_INT_2_ADDR_ANY_EN_Y__POS           1                       /**< 1st bit of INT 2Registers */
#define BMG160_INT_2_ADDR_ANY_EN_Y__LEN           1
#define BMG160_INT_2_ADDR_ANY_EN_Y__MSK           0x02
#define BMG160_INT_2_ADDR_ANY_EN_Y__REG           BMG160_INT_2_ADDR

#define BMG160_INT_2_ADDR_ANY_EN_X__POS           0                       /**< 0th bit of INT 2Registers */
#define BMG160_INT_2_ADDR_ANY_EN_X__LEN           1
#define BMG160_INT_2_ADDR_ANY_EN_X__MSK           0x01
#define BMG160_INT_2_ADDR_ANY_EN_X__REG           BMG160_INT_2_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2110
   BMG160_INT_3_ADDR */
#define BMG160_INT_3_ADDR_TURN1_QUIT_TH__POS      6                       /**< Last 2 bits of INT 3 Registers */
#define BMG160_INT_3_ADDR_TURN1_QUIT_TH__LEN      2
#define BMG160_INT_3_ADDR_TURN1_QUIT_TH__MSK      0xC0
#define BMG160_INT_3_ADDR_TURN1_QUIT_TH__REG      BMG160_INT_3_ADDR

#define BMG160_INT_3_ADDR_TURN1_TH__POS           3                       /**< 3 bits of INT 3 Registers */
#define BMG160_INT_3_ADDR_TURN1_TH__LEN           3
#define BMG160_INT_3_ADDR_TURN1_TH__MSK           0x38
#define BMG160_INT_3_ADDR_TURN1_TH__REG           BMG160_INT_3_ADDR

#define BMG160_INT_3_ADDR_TURN1_ANGLE__POS        0                       /**< First 3 bits of INT 3 Registers */
#define BMG160_INT_3_ADDR_TURN1_ANGLE__LEN        3
#define BMG160_INT_3_ADDR_TURN1_ANGLE__MSK        0x07
#define BMG160_INT_3_ADDR_TURN1_ANGLE__REG        BMG160_INT_3_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2111
   BMG160_INT_4_ADDR */
#define BMG160_INT_4_FIFO_WM_EN__POS           7                       /**< Last bit of INT 4 Registers */
#define BMG160_INT_4_FIFO_WM_EN__LEN           1
#define BMG160_INT_4_FIFO_WM_EN__MSK           0x80
#define BMG160_INT_4_FIFO_WM_EN__REG           BMG160_INT_4_ADDR

#define BMG160_INT_4_TURN1_ANGLE_RANGE__POS    3                       /**< 3 bits of INT 4 Registers */
#define BMG160_INT_4_TURN1_ANGLE_RANGE__LEN    3
#define BMG160_INT_4_TURN1_ANGLE_RANGE__MSK    0x38
#define BMG160_INT_4_TURN1_ANGLE_RANGE__REG    BMG160_INT_4_ADDR

#define BMG160_INT_4_TURN1_EN_Z__POS           2                       /**< 2nd bit of INT 4 Registers */
#define BMG160_INT_4_TURN1_EN_Z__LEN           1
#define BMG160_INT_4_TURN1_EN_Z__MSK           0x04
#define BMG160_INT_4_TURN1_EN_Z__REG           BMG160_INT_4_ADDR

#define BMG160_INT_4_TURN1_EN_Y__POS           1                       /**< 1st bit of INT 4 Registers */
#define BMG160_INT_4_TURN1_EN_Y__LEN           1
#define BMG160_INT_4_TURN1_EN_Y__MSK           0x02
#define BMG160_INT_4_TURN1_EN_Y__REG           BMG160_INT_4_ADDR

#define BMG160_INT_4_TURN1_EN_X__POS           0                       /**< 0th bit of INT 4 Registers */
#define BMG160_INT_4_TURN1_EN_X__LEN           1
#define BMG160_INT_4_TURN1_EN_X__MSK           0x01
#define BMG160_INT_4_TURN1_EN_X__REG           BMG160_INT_4_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2112
   BMG160_INT_5_ADDR */
#define BMG160_INT_5_ADDR_TURN2_TH__POS                  5                       /**< Last 3 bits of INT 5 Registers */
#define BMG160_INT_5_ADDR_TURN2_TH__LEN                  3
#define BMG160_INT_5_ADDR_TURN2_TH__MSK                  0xE0
#define BMG160_INT_5_ADDR_TURN2_TH__REG                  BMG160_INT_5_ADDR

#define BMG160_INT_5_ADDR_TURN1_QUIT_TIME__POS           0                       /**< First 5 bits of INT 5 Registers */
#define BMG160_INT_5_ADDR_TURN1_QUIT_TIME__LEN           5
#define BMG160_INT_5_ADDR_TURN1_QUIT_TIME__MSK           0x1F
#define BMG160_INT_5_ADDR_TURN1_QUIT_TIME__REG           BMG160_INT_5_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2113
   BMG160_INT_6_ADDR */
#define BMG160_INT_6_ADDR_TURN2_ANGLE__POS          3                       /**< 3 bits of INT 6 Registers */
#define BMG160_INT_6_ADDR_TURN2_ANGLE__LEN          3
#define BMG160_INT_6_ADDR_TURN2_ANGLE__MSK          0x38
#define BMG160_INT_6_ADDR_TURN2_ANGLE__REG          BMG160_INT_6_ADDR

#define BMG160_INT_6_ADDR_TURN2_EN_Z__POS           2                       /**< 2nd bit of INT 6 Registers */
#define BMG160_INT_6_ADDR_TURN2_EN_Z__LEN           1
#define BMG160_INT_6_ADDR_TURN2_EN_Z__MSK           0x04
#define BMG160_INT_6_ADDR_TURN2_EN_Z__REG           BMG160_INT_6_ADDR

#define BMG160_INT_6_ADDR_TURN2_EN_Y__POS           1                       /**< 1st bit of INT 6 Registers */
#define BMG160_INT_6_ADDR_TURN2_EN_Y__LEN           1
#define BMG160_INT_6_ADDR_TURN2_EN_Y__MSK           0x02
#define BMG160_INT_6_ADDR_TURN2_EN_Y__REG           BMG160_INT_6_ADDR

#define BMG160_INT_6_ADDR_TURN2_EN_X__POS           0                       /**< 0th bit of INT 6 Registers */
#define BMG160_INT_6_ADDR_TURN2_EN_X__LEN           1
#define BMG160_INT_6_ADDR_TURN2_EN_X__MSK           0x01
#define BMG160_INT_6_ADDR_TURN2_EN_X__REG           BMG160_INT_6_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2114
   BMG160_RST_LATCH_ADDR */
#define BMG160_RST_LATCH_ADDR_RESET_INT__POS           7                       /**< Last bit of Reset Latch Registers */
#define BMG160_RST_LATCH_ADDR_RESET_INT__LEN           1
#define BMG160_RST_LATCH_ADDR_RESET_INT__MSK           0x80
#define BMG160_RST_LATCH_ADDR_RESET_INT__REG           BMG160_RST_LATCH_ADDR

#define BMG160_RST_LATCH_ADDR_OFFSET_RESET__POS        6                       /**< 6th bit of Reset Latch Registers */
#define BMG160_RST_LATCH_ADDR_OFFSET_RESET__LEN        1
#define BMG160_RST_LATCH_ADDR_OFFSET_RESET__MSK        0x40
#define BMG160_RST_LATCH_ADDR_OFFSET_RESET__REG        BMG160_RST_LATCH_ADDR

#define BMG160_RST_LATCH_ADDR_LATCH_STATUS__POS        4                       /**< 4th bit of Reset Latch Registers */
#define BMG160_RST_LATCH_ADDR_LATCH_STATUS__LEN        1
#define BMG160_RST_LATCH_ADDR_LATCH_STATUS__MSK        0x10
#define BMG160_RST_LATCH_ADDR_LATCH_STATUS__REG        BMG160_RST_LATCH_ADDR

#define BMG160_RST_LATCH_ADDR_LATCH_INT__POS           0                       /**< First 4 bits of Reset Latch Registers */
#define BMG160_RST_LATCH_ADDR_LATCH_INT__LEN           4
#define BMG160_RST_LATCH_ADDR_LATCH_INT__MSK           0x0F
#define BMG160_RST_LATCH_ADDR_LATCH_INT__REG           BMG160_RST_LATCH_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2115
   BMG160_INT_7_ADDR */
#define BMG160_INT_7_ADDR_HIGH_HY_X__POS        6                       /**< Last bit of Int 7 Registers */
#define BMG160_INT_7_ADDR_HIGH_HY_X__LEN        2
#define BMG160_INT_7_ADDR_HIGH_HY_X__MSK        0xC0
#define BMG160_INT_7_ADDR_HIGH_HY_X__REG        BMG160_INT_7_ADDR

#define BMG160_INT_7_ADDR_HIGH_TH_X__POS        1                       /**< 6 bits of Int 7 Registers */
#define BMG160_INT_7_ADDR_HIGH_TH_X__LEN        5
#define BMG160_INT_7_ADDR_HIGH_TH_X__MSK        0x3E
#define BMG160_INT_7_ADDR_HIGH_TH_X__REG        BMG160_INT_7_ADDR

#define BMG160_INT_7_ADDR_HIGH_EN_X__POS        0                      /**< 0th bit of Int 7 Registers */
#define BMG160_INT_7_ADDR_HIGH_EN_X__LEN        1
#define BMG160_INT_7_ADDR_HIGH_EN_X__MSK        0x01
#define BMG160_INT_7_ADDR_HIGH_EN_X__REG        BMG160_INT_7_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2116
   BMG160_INT_9_ADDR */
#define BMG160_INT_9_ADDR_HIGH_HY_Y__POS        6                       /**< Last bit of Int 9 Registers */
#define BMG160_INT_9_ADDR_HIGH_HY_Y__LEN        2
#define BMG160_INT_9_ADDR_HIGH_HY_Y__MSK        0xC0
#define BMG160_INT_9_ADDR_HIGH_HY_Y__REG        BMG160_INT_9_ADDR

#define BMG160_INT_9_ADDR_HIGH_TH_Y__POS        1                       /**< 6 bits of Int 9 Registers */
#define BMG160_INT_9_ADDR_HIGH_TH_Y__LEN        5
#define BMG160_INT_9_ADDR_HIGH_TH_Y__MSK        0x3E
#define BMG160_INT_9_ADDR_HIGH_TH_Y__REG        BMG160_INT_9_ADDR

#define BMG160_INT_9_ADDR_HIGH_EN_Y__POS        0                      /**< 0th bit of Int 9 Registers */
#define BMG160_INT_9_ADDR_HIGH_EN_Y__LEN        1
#define BMG160_INT_9_ADDR_HIGH_EN_Y__MSK        0x01
#define BMG160_INT_9_ADDR_HIGH_EN_Y__REG        BMG160_INT_9_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2117
   BMG160_INT_11_ADDR */
#define BMG160_INT_11_ADDR_HIGH_HY_Z__POS        6                       /**< Last bit of Int 11 Registers */
#define BMG160_INT_11_ADDR_HIGH_HY_Z__LEN        2
#define BMG160_INT_11_ADDR_HIGH_HY_Z__MSK        0xC0
#define BMG160_INT_11_ADDR_HIGH_HY_Z__REG        BMG160_INT_11_ADDR

#define BMG160_INT_11_ADDR_HIGH_TH_Z__POS        1                       /**< 6 bits of Int 11 Registers */
#define BMG160_INT_11_ADDR_HIGH_TH_Z__LEN        5
#define BMG160_INT_11_ADDR_HIGH_TH_Z__MSK        0x3E
#define BMG160_INT_11_ADDR_HIGH_TH_Z__REG        BMG160_INT_11_ADDR

#define BMG160_INT_11_ADDR_HIGH_EN_Z__POS        0                      /**< 0th bit of Int 11 Registers */
#define BMG160_INT_11_ADDR_HIGH_EN_Z__LEN        1
#define BMG160_INT_11_ADDR_HIGH_EN_Z__MSK        0x01
#define BMG160_INT_11_ADDR_HIGH_EN_Z__REG        BMG160_INT_11_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2118
   BMG160_INT_13_ADDR */
#define BMG160_INT_13_ADDR_CONST_TH__POS        2                      /**< Last6 bit of Int 12 Registers */
#define BMG160_INT_13_ADDR_CONST_TH__LEN        6
#define BMG160_INT_13_ADDR_CONST_TH__MSK        0xFC
#define BMG160_INT_13_ADDR_CONST_TH__REG        BMG160_INT_13_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2119
   BMG160_INT_15_ADDR */
#define BMG160_INT_15_ADDR_SHAKE_QUIT__POS        6                       /**< Last 2 bits of Int 15 Registers */
#define BMG160_INT_15_ADDR_SHAKE_QUIT__LEN        2
#define BMG160_INT_15_ADDR_SHAKE_QUIT__MSK        0xC0
#define BMG160_INT_15_ADDR_SHAKE_QUIT__REG        BMG160_INT_15_ADDR

#define BMG160_INT_15_ADDR_CONST_RANGE__POS       3                       /**< 3 bits of Int 15 Registers */
#define BMG160_INT_15_ADDR_CONST_RANGE__LEN       3
#define BMG160_INT_15_ADDR_CONST_RANGE__MSK       0x38
#define BMG160_INT_15_ADDR_CONST_RANGE__REG       BMG160_INT_15_ADDR

#define BMG160_INT_15_ADDR_CONST_EN_Z__POS        2                       /**< 2nd bit of Int 15 Registers */
#define BMG160_INT_15_ADDR_CONST_EN_Z__LEN        1
#define BMG160_INT_15_ADDR_CONST_EN_Z__MSK        0x04
#define BMG160_INT_15_ADDR_CONST_EN_Z__REG        BMG160_INT_15_ADDR

#define BMG160_INT_15_ADDR_CONST_EN_Y__POS        1                       /**< 1st bit of Int 15 Registers */
#define BMG160_INT_15_ADDR_CONST_EN_Y__LEN        1
#define BMG160_INT_15_ADDR_CONST_EN_Y__MSK        0x02
#define BMG160_INT_15_ADDR_CONST_EN_Y__REG        BMG160_INT_15_ADDR

#define BMG160_INT_15_ADDR_CONST_EN_X__POS        0                      /**< 0th bit of Int 15 Registers */
#define BMG160_INT_15_ADDR_CONST_EN_X__LEN        1
#define BMG160_INT_15_ADDR_CONST_EN_X__MSK        0x01
#define BMG160_INT_15_ADDR_CONST_EN_X__REG        BMG160_INT_15_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2120
   BMG160_INT_16_ADDR */
#define BMG160_INT_16_ADDR_SHAKE_DUR__POS       5                       /**< Last 3 bits of Int 16 Registers */
#define BMG160_INT_16_ADDR_SHAKE_DUR__LEN       3
#define BMG160_INT_16_ADDR_SHAKE_DUR__MSK       0xE0
#define BMG160_INT_16_ADDR_SHAKE_DUR__REG       BMG160_INT_16_ADDR

#define BMG160_INT_16_ADDR_SHAKE_TH__POS        0                       /**< First 5 bits of Int 16 Registers */
#define BMG160_INT_16_ADDR_SHAKE_TH__LEN        5
#define BMG160_INT_16_ADDR_SHAKE_TH__MSK        0x1F
#define BMG160_INT_16_ADDR_SHAKE_TH__REG        BMG160_INT_16_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2121
   BMG160_INT_17_ADDR */
#define BMG160_INT_17_ADDR_SHAKE_BELOW_TH__POS       5                       /**< Last 3 bits of Int 17 Registers */
#define BMG160_INT_17_ADDR_SHAKE_BELOW_TH__LEN       3
#define BMG160_INT_17_ADDR_SHAKE_BELOW_TH__MSK       0xE0
#define BMG160_INT_17_ADDR_SHAKE_BELOW_TH__REG       BMG160_INT_17_ADDR

#define BMG160_INT_17_ADDR_SHAKE_TH2__POS            0                       /**< First 5 bits of Int 17 Registers */
#define BMG160_INT_17_ADDR_SHAKE_TH2__LEN            5
#define BMG160_INT_17_ADDR_SHAKE_TH2__MSK            0x1F
#define BMG160_INT_17_ADDR_SHAKE_TH2__REG            BMG160_INT_17_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2563
   BMG160_INT_19_ADDR */
#define BMG160_INT_19_ADDR_SHAKE_SHOCK_TIME__POS  3                       /**< 2  bits of INT 19 Registers */
#define BMG160_INT_19_ADDR_SHAKE_SHOCK_TIME__LEN  4
#define BMG160_INT_19_ADDR_SHAKE_SHOCK_TIME__MSK  0x78
#define BMG160_INT_19_ADDR_SHAKE_SHOCK_TIME__REG  BMG160_INT_19_ADDR

#define BMG160_INT_19_ADDR_SHAKE_EN_Z__POS        2                       /**< 2nd bit of INT 19 Registers */
#define BMG160_INT_19_ADDR_SHAKE_EN_Z__LEN        1
#define BMG160_INT_19_ADDR_SHAKE_EN_Z__MSK        0x04
#define BMG160_INT_19_ADDR_SHAKE_EN_Z__REG        BMG160_INT_19_ADDR

#define BMG160_INT_19_ADDR_SHAKE_EN_Y__POS        1                       /**< 1st bit of INT 19 Registers */
#define BMG160_INT_19_ADDR_SHAKE_EN_Y__LEN        1
#define BMG160_INT_19_ADDR_SHAKE_EN_Y__MSK        0x02
#define BMG160_INT_19_ADDR_SHAKE_EN_Y__REG        BMG160_INT_19_ADDR

#define BMG160_INT_19_ADDR_SHAKE_EN_X__POS        0                      /**< 0th bit of INT 19 Registers */
#define BMG160_INT_19_ADDR_SHAKE_EN_X__LEN        1
#define BMG160_INT_19_ADDR_SHAKE_EN_X__MSK        0x01
#define BMG160_INT_19_ADDR_SHAKE_EN_X__REG        BMG160_INT_19_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2559
   BMG160_INT_20_ADDR */
#define BMG160_INT_20_ADDR_TAP_DUR__POS       6                       /**< Last 2 bits of Int 20 Registers */
#define BMG160_INT_20_ADDR_TAP_DUR__LEN       2
#define BMG160_INT_20_ADDR_TAP_DUR__MSK       0xC0
#define BMG160_INT_20_ADDR_TAP_DUR__REG       BMG160_INT_20_ADDR

#define BMG160_INT_20_ADDR_TAP_TH__POS        0                       /**< First 6 bits of Int 20 Registers */
#define BMG160_INT_20_ADDR_TAP_TH__LEN        6
#define BMG160_INT_20_ADDR_TAP_TH__MSK        0x3F
#define BMG160_INT_20_ADDR_TAP_TH__REG        BMG160_INT_20_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2560
   BMG160_INT_21_ADDR */
#define BMG160_INT_21_ADDR_TAP_SHOCK_IGNORE__POS       6                       /**< Last 2 bits of Int 21 Registers */
#define BMG160_INT_21_ADDR_TAP_SHOCK_IGNORE__LEN       2
#define BMG160_INT_21_ADDR_TAP_SHOCK_IGNORE__MSK       0xC0
#define BMG160_INT_21_ADDR_TAP_SHOCK_IGNORE__REG       BMG160_INT_21_ADDR

#define BMG160_INT_21_ADDR_TAP_QUIT__POS               0                       /**< First 6 bits of Int 21 Registers */
#define BMG160_INT_21_ADDR_TAP_QUIT__LEN               6
#define BMG160_INT_21_ADDR_TAP_QUIT__MSK               0x3F
#define BMG160_INT_21_ADDR_TAP_QUIT__REG               BMG160_INT_21_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2561
   BMG160_INT_22_ADDR */
#define BMG160_INT_22_ADDR_EN_HIGH_AXIS__POS    7                       /**< Last bit of Int 22 Registers */
#define BMG160_INT_22_ADDR_EN_HIGH_AXIS__LEN    1
#define BMG160_INT_22_ADDR_EN_HIGH_AXIS__MSK    0x80
#define BMG160_INT_22_ADDR_EN_HIGH_AXIS__REG    BMG160_INT_22_ADDR

#define BMG160_INT_22_ADDR_HIGH_TIME__POS       6                       /**< 6 bit of Int 22 Registers */
#define BMG160_INT_22_ADDR_HIGH_TIME__LEN       1
#define BMG160_INT_22_ADDR_HIGH_TIME__MSK       0x40
#define BMG160_INT_22_ADDR_HIGH_TIME__REG       BMG160_INT_22_ADDR

#define BMG160_INT_22_ADDR_TAP_SHOCK__POS       3                       /**< 3 bits of Int 22 Registers */
#define BMG160_INT_22_ADDR_TAP_SHOCK__LEN       3
#define BMG160_INT_22_ADDR_TAP_SHOCK__MSK       0x38
#define BMG160_INT_22_ADDR_TAP_SHOCK__REG       BMG160_INT_22_ADDR

#define BMG160_INT_22_ADDR_TAP_EN_Z__POS        2                       /**< 2nd bit of Int 22 Registers */
#define BMG160_INT_22_ADDR_TAP_EN_Z__LEN        1
#define BMG160_INT_22_ADDR_TAP_EN_Z__MSK        0x04
#define BMG160_INT_22_ADDR_TAP_EN_Z__REG        BMG160_INT_22_ADDR

#define BMG160_INT_22_ADDR_TAP_EN_Y__POS        1                       /**< 1st bit of Int 22 Registers */
#define BMG160_INT_22_ADDR_TAP_EN_Y__LEN        1
#define BMG160_INT_22_ADDR_TAP_EN_Y__MSK        0x02
#define BMG160_INT_22_ADDR_TAP_EN_Y__REG        BMG160_INT_22_ADDR

#define BMG160_INT_22_ADDR_TAP_EN_X__POS        0                      /**< 0th bit of Int 22 Registers */
#define BMG160_INT_22_ADDR_TAP_EN_X__LEN        1
#define BMG160_INT_22_ADDR_TAP_EN_X__MSK        0x01
#define BMG160_INT_22_ADDR_TAP_EN_X__REG        BMG160_INT_22_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2126
   BMG160_INT_OFF0_ADDR */
#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_TH__POS          6                       /**< Last 3 bits of INT OFF0 Registers */
#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_TH__LEN          2
#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_TH__MSK          0xC0
#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_TH__REG          BMG160_INT_OFF0_ADDR

#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_DUR__POS         3                       /**< 2  bits of INT OFF0 Registers */
#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_DUR__LEN         3
#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_DUR__MSK         0x38
#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_DUR__REG         BMG160_INT_OFF0_ADDR

#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_EN_Z__POS        2                       /**< 2nd bit of INT OFF0 Registers */
#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_EN_Z__LEN        1
#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_EN_Z__MSK        0x04
#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_EN_Z__REG        BMG160_INT_OFF0_ADDR

#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_EN_Y__POS        1                       /**< 1st bit of INT OFF0 Registers */
#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_EN_Y__LEN        1
#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_EN_Y__MSK        0x02
#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_EN_Y__REG        BMG160_INT_OFF0_ADDR

#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_EN_X__POS        0                       /**< 0th bit of INT OFF0 Registers */
#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_EN_X__LEN        1
#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_EN_X__MSK        0x01
#define BMG160_INT_OFF0_ADDR_SLOW_OFFSET_EN_X__REG        BMG160_INT_OFF0_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2127
   BMG160_INT_OFF1_ADDR */
#define BMG160_INT_OFF1_ADDR_AUTO_OFFSET_WL__POS        6                       /**< Last 2 bits of INT OFF1 Registers */
#define BMG160_INT_OFF1_ADDR_AUTO_OFFSET_WL__LEN        2
#define BMG160_INT_OFF1_ADDR_AUTO_OFFSET_WL__MSK        0xC0
#define BMG160_INT_OFF1_ADDR_AUTO_OFFSET_WL__REG        BMG160_INT_OFF1_ADDR

#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_WL__POS        4                       /**< 2  bits of INT OFF1 Registers */
#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_WL__LEN        2
#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_WL__MSK        0x30
#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_WL__REG        BMG160_INT_OFF1_ADDR

#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_EN__POS        3                       /**< 3nd bit of INT OFF1 Registers */
#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_EN__LEN        1
#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_EN__MSK        0x08
#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_EN__REG        BMG160_INT_OFF1_ADDR

#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_EN_Z__POS      2                       /**< 2nd bit of INT OFF1 Registers */
#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_EN_Z__LEN      1
#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_EN_Z__MSK      0x04
#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_EN_Z__REG      BMG160_INT_OFF1_ADDR

#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_EN_Y__POS      1                       /**< 1st bit of INT OFF1 Registers */
#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_EN_Y__LEN      1
#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_EN_Y__MSK      0x02
#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_EN_Y__REG      BMG160_INT_OFF1_ADDR

#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_EN_X__POS      0                      /**< 0th bit of INT OFF1 Registers */
#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_EN_X__LEN      1
#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_EN_X__MSK      0x01
#define BMG160_INT_OFF1_ADDR_FAST_OFFSET_EN_X__REG      BMG160_INT_OFF1_ADDR

#define BMG160_FAST_OFFSET_EN_XYZ__POS      0                      /**< 0 to 2 bits of INT OFF1 Registers */
#define BMG160_FAST_OFFSET_EN_XYZ__LEN      3
#define BMG160_FAST_OFFSET_EN_XYZ__MSK      0x07
#define BMG160_FAST_OFFSET_EN_XYZ__REG      BMG160_INT_OFF1_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2128
   BMG160_TRIM_NVM_CTRL_ADDR */
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_REMAIN__POS        4                       /**< Last 4 bits of Trim NVM control Registers */
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_REMAIN__LEN        4
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_REMAIN__MSK        0xF0
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_REMAIN__REG        BMG160_TRIM_NVM_CTRL_ADDR

#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_LOAD__POS          3                       /**< 3rd bit of Trim NVM control Registers */
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_LOAD__LEN          1
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_LOAD__MSK          0x08
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_LOAD__REG          BMG160_TRIM_NVM_CTRL_ADDR

#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_RDY__POS           2                       /**< 2nd bit of Trim NVM control Registers */
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_RDY__LEN           1
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_RDY__MSK           0x04
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_RDY__REG           BMG160_TRIM_NVM_CTRL_ADDR

#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_PROG_TRIG__POS     1                       /**< 1st bit of Trim NVM control Registers */
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_PROG_TRIG__LEN     1
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_PROG_TRIG__MSK     0x02
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_PROG_TRIG__REG     BMG160_TRIM_NVM_CTRL_ADDR

#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_PROG_MODE__POS     0                       /**< 0th bit of Trim NVM control Registers */
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_PROG_MODE__LEN     1
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_PROG_MODE__MSK     0x01
#define BMG160_TRIM_NVM_CTRL_ADDR_NVM_PROG_MODE__REG     BMG160_TRIM_NVM_CTRL_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2129
   BMG160_BA_SPI3_WDT_ADDR */
#define BMG160_BA_SPI3_WDT_ADDR_BURST_SAME_EN__POS      3                   /**< 3rd bit of SPI3 WDT Registers */
#define BMG160_BA_SPI3_WDT_ADDR_BURST_SAME_EN__LEN      1
#define BMG160_BA_SPI3_WDT_ADDR_BURST_SAME_EN__MSK      0x08
#define BMG160_BA_SPI3_WDT_ADDR_BURST_SAME_EN__REG      BMG160_BA_SPI3_WDT_ADDR

#define BMG160_BA_SPI3_WDT_ADDR_I2C_WDT_EN__POS      2                       /**< 2nd bit of SPI3 WDT Registers */
#define BMG160_BA_SPI3_WDT_ADDR_I2C_WDT_EN__LEN      1
#define BMG160_BA_SPI3_WDT_ADDR_I2C_WDT_EN__MSK      0x04
#define BMG160_BA_SPI3_WDT_ADDR_I2C_WDT_EN__REG      BMG160_BA_SPI3_WDT_ADDR

#define BMG160_BA_SPI3_WDT_ADDR_I2C_WDT_SEL__POS     1                       /**< 1st bit of SPI3 WDT Registers */
#define BMG160_BA_SPI3_WDT_ADDR_I2C_WDT_SEL__LEN     1
#define BMG160_BA_SPI3_WDT_ADDR_I2C_WDT_SEL__MSK     0x02
#define BMG160_BA_SPI3_WDT_ADDR_I2C_WDT_SEL__REG     BMG160_BA_SPI3_WDT_ADDR

#define BMG160_BA_SPI3_WDT_ADDR_SPI3__POS            0                       /**< 0th bit of SPI3 WDT Registers */
#define BMG160_BA_SPI3_WDT_ADDR_SPI3__LEN            1
#define BMG160_BA_SPI3_WDT_ADDR_SPI3__MSK            0x01
#define BMG160_BA_SPI3_WDT_ADDR_SPI3__REG            BMG160_BA_SPI3_WDT_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2567
   BMG160_FIFO_CGF1_ADDR */
#define BMG160_FIFO_CGF1_ADDR_TAG__POS     7                       /**< 7th bit of FIFO CGF1 Registers */
#define BMG160_FIFO_CGF1_ADDR_TAG__LEN     1
#define BMG160_FIFO_CGF1_ADDR_TAG__MSK     0x80
#define BMG160_FIFO_CGF1_ADDR_TAG__REG     BMG160_FIFO_CGF1_ADDR

#define BMG160_FIFO_CGF1_ADDR_WML__POS     0                       /**< First 7 bits of FIFO CGF1 Registers */
#define BMG160_FIFO_CGF1_ADDR_WML__LEN     7
#define BMG160_FIFO_CGF1_ADDR_WML__MSK     0x7F
#define BMG160_FIFO_CGF1_ADDR_WML__REG     BMG160_FIFO_CGF1_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2568
   BMG160_FIFO_CGF0_ADDR */
#define BMG160_FIFO_CGF0_ADDR_MODE__POS         6                       /**< Last 2 bits of FIFO CGF0 Addr Registers */
#define BMG160_FIFO_CGF0_ADDR_MODE__LEN         2
#define BMG160_FIFO_CGF0_ADDR_MODE__MSK         0xC0
#define BMG160_FIFO_CGF0_ADDR_MODE__REG         BMG160_FIFO_CGF0_ADDR

#define BMG160_FIFO_CGF0_ADDR_TRIG_ACT__POS     4                       /**< 2 bits of FIFO CGF0 Addr WDT Registers */
#define BMG160_FIFO_CGF0_ADDR_TRIG_ACT__LEN     2
#define BMG160_FIFO_CGF0_ADDR_TRIG_ACT__MSK     0x30
#define BMG160_FIFO_CGF0_ADDR_TRIG_ACT__REG     BMG160_FIFO_CGF0_ADDR

#define BMG160_FIFO_CGF0_ADDR_TRIG_SRC__POS     2                       /**< 2 bits of FIFO CGF0 Addr Registers */
#define BMG160_FIFO_CGF0_ADDR_TRIG_SRC__LEN     2
#define BMG160_FIFO_CGF0_ADDR_TRIG_SRC__MSK     0x0C
#define BMG160_FIFO_CGF0_ADDR_TRIG_SRC__REG     BMG160_FIFO_CGF0_ADDR

#define BMG160_FIFO_CGF0_ADDR_DATA_SEL__POS     0                       /**< First 2 bits of FIFO CGF0 Addr Registers */
#define BMG160_FIFO_CGF0_ADDR_DATA_SEL__LEN     2
#define BMG160_FIFO_CGF0_ADDR_DATA_SEL__MSK     0x03
#define BMG160_FIFO_CGF0_ADDR_DATA_SEL__REG     BMG160_FIFO_CGF0_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2131
   BMG160_TRIM0 */
#define BMG160_TRIM0_MARGIN_TRIM__POS      6                       /**< Last 2 bits of Trim 0 Registers */
#define BMG160_TRIM0_MARGIN_TRIM__LEN      2
#define BMG160_TRIM0_MARGIN_TRIM__MSK      0xC0
#define BMG160_TRIM0_MARGIN_TRIM__REG      BMG160_TRIM0

#define BMG160_TRIM0_NVM_ERROR_FLAG__POS   2                       /**< 2nd bit of Trim 0 Registers */
#define BMG160_TRIM0_NVM_ERROR_FLAG__LEN   1
#define BMG160_TRIM0_NVM_ERROR_FLAG__MSK   0x04
#define BMG160_TRIM0_NVM_ERROR_FLAG__REG   BMG160_TRIM0

#define BMG160_TRIM0_NVM_RDY_FLAG__POS     1                       /**< 1st bit of Trim 0 Registers */
#define BMG160_TRIM0_NVM_RDY_FLAG__LEN     1
#define BMG160_TRIM0_NVM_RDY_FLAG__MSK     0x02
#define BMG160_TRIM0_NVM_RDY_FLAG__REG     BMG160_TRIM0

#define BMG160_TRIM0_NVM_ERASE__POS        0                       /**< 0th bit of Trim 0 Registers */
#define BMG160_TRIM0_NVM_ERASE__LEN        1
#define BMG160_TRIM0_NVM_ERASE__MSK        0x01
#define BMG160_TRIM0_NVM_ERASE__REG        BMG160_TRIM0
/* EasyCASE ) */
/* EasyCASE ( 2132
   BMG160_TMC_EXT_MODE_HI */
#define BMG160_TMC_EXT_MODE_HI_MODE__POS        7                       /**< 7th bit of TMC Ext Mode HI Registers */
#define BMG160_TMC_EXT_MODE_HI_MODE__LEN        1
#define BMG160_TMC_EXT_MODE_HI_MODE__MSK        0x80
#define BMG160_TMC_EXT_MODE_HI_MODE__REG        BMG160_TMC_EXT_MODE_HI

#define BMG160_TMC_EXT_MODE_HI_PAGE__POS        6                       /**< 6th bit of TMC Ext Mode HI Registers */
#define BMG160_TMC_EXT_MODE_HI_PAGE__LEN        1
#define BMG160_TMC_EXT_MODE_HI_PAGE__MSK        0x40
#define BMG160_TMC_EXT_MODE_HI_PAGE__REG        BMG160_TMC_EXT_MODE_HI

#define BMG160_TMC_EXT_MODE_HI_MAP2UPPER__POS   5                       /**< 5th bit of TMC Ext Mode HI Registers */
#define BMG160_TMC_EXT_MODE_HI_MAP2UPPER__LEN   1
#define BMG160_TMC_EXT_MODE_HI_MAP2UPPER__MSK   0x20
#define BMG160_TMC_EXT_MODE_HI_MAP2UPPER__REG   BMG160_TMC_EXT_MODE_HI

#define BMG160_TMC_EXT_MODE_HI_MAGIC_NO__POS    0                       /**< First 5 bits of TMC Ext Mode HI Registers */
#define BMG160_TMC_EXT_MODE_HI_MAGIC_NO__LEN    5
#define BMG160_TMC_EXT_MODE_HI_MAGIC_NO__MSK    0x1F
#define BMG160_TMC_EXT_MODE_HI_MAGIC_NO__REG    BMG160_TMC_EXT_MODE_HI
/* EasyCASE ) */
/* EasyCASE ( 2133
   BMG160_TRIM1 */
#define BMG160_TRIM1_DSP_RAM_WAIT__POS       5                       /**< 5th bit of Trim 1 Registers */
#define BMG160_TRIM1_DSP_RAM_WAIT__LEN       1
#define BMG160_TRIM1_DSP_RAM_WAIT__MSK       0x20
#define BMG160_TRIM1_DSP_RAM_WAIT__REG       BMG160_TRIM1

#define BMG160_TRIM1_NVM_START_ADDR__POS     0                       /**< First 5 bits of Trim 1 Registers */
#define BMG160_TRIM1_NVM_START_ADDR__LEN     5
#define BMG160_TRIM1_NVM_START_ADDR__MSK     0x1F
#define BMG160_TRIM1_NVM_START_ADDR__REG     BMG160_TRIM1
/* EasyCASE ) */
/* EasyCASE ( 2134
   BMG160_TRIM2 */
#define BMG160_TRIM2_NVM_STOP_ADDR__POS    0                       /**< Last 5 bits of Trim 2 Registers */
#define BMG160_TRIM2_NVM_STOP_ADDR__LEN    5
#define BMG160_TRIM2_NVM_STOP_ADDR__MSK    0xF8
#define BMG160_TRIM2_NVM_STOP_ADDR__REG    BMG160_TRIM2

#define BMG160_TRIM2_MAP_GP2NVM__POS       2                       /**< 2nd bit of Trim 2 Registers */
#define BMG160_TRIM2_MAP_GP2NVM__LEN       1
#define BMG160_TRIM2_MAP_GP2NVM__MSK       0x04
#define BMG160_TRIM2_MAP_GP2NVM__REG       BMG160_TRIM2

#define BMG160_TRIM2_IP_ALWAYS_EN__POS     1                       /**< 1st 5 bit of Trim 2 Registers */
#define BMG160_TRIM2_IP_ALWAYS_EN__LEN     1
#define BMG160_TRIM2_IP_ALWAYS_EN__MSK     0x02
#define BMG160_TRIM2_IP_ALWAYS_EN__REG     BMG160_TRIM2

#define BMG160_TRIM2_NVM_RETRY__POS        0                       /**< First bit of Trim 2 Registers */
#define BMG160_TRIM2_NVM_RETRY__LEN        1
#define BMG160_TRIM2_NVM_RETRY__MSK        0x01
#define BMG160_TRIM2_NVM_RETRY__REG        BMG160_TRIM2
/* EasyCASE ) */
/* EasyCASE ( 2572
   BMG160_TMC_CONTROL */
#define BMG160_TMC_CONTROL_EXT_CLK__POS             7                       /**< 7th bit of TMC Control Register */
#define BMG160_TMC_CONTROL_EXT_CLK__LEN             1
#define BMG160_TMC_CONTROL_EXT_CLK__MSK             0x80
#define BMG160_TMC_CONTROL_EXT_CLK__REG             BMG160_TMC_CONTROL

#define BMG160_TMC_CONTROL_FORCE_MODE__POS          5                       /**< 2 bits of TMC Control Register */
#define BMG160_TMC_CONTROL_FORCE_MODE__LEN          2
#define BMG160_TMC_CONTROL_FORCE_MODE__MSK          0x60
#define BMG160_TMC_CONTROL_FORCE_MODE__REG          BMG160_TMC_CONTROL

#define BMG160_TMC_CONTROL_TEST_MODE_EN__POS        4                       /**< 4th bit of TMC Control Register */
#define BMG160_TMC_CONTROL_TEST_MODE_EN__LEN        1
#define BMG160_TMC_CONTROL_TEST_MODE_EN__MSK        0x10
#define BMG160_TMC_CONTROL_TEST_MODE_EN__REG        BMG160_TMC_CONTROL

#define BMG160_TMC_CONTROL_TEST_MODE_REG__POS       3                       /**< 3rd bit of TMC Control Register */
#define BMG160_TMC_CONTROL_TEST_MODE_REG__LEN       1
#define BMG160_TMC_CONTROL_TEST_MODE_REG__MSK       0x08
#define BMG160_TMC_CONTROL_TEST_MODE_REG__REG       BMG160_TMC_CONTROL

#define BMG160_TMC_CONTROL_SLOW_OSC_EN__POS         2                       /**< 2nd bit of TMC Control Register */
#define BMG160_TMC_CONTROL_SLOW_OSC_EN__LEN         1
#define BMG160_TMC_CONTROL_SLOW_OSC_EN__MSK         0x04
#define BMG160_TMC_CONTROL_SLOW_OSC_EN__REG         BMG160_TMC_CONTROL
/* EasyCASE ) */
/* EasyCASE ( 2135
   BMG160_SCAN_EN */
#define BMG160_SCAN_EN__POS        7                       /**< 7th bit of Scan Mode Enable Register */
#define BMG160_SCAN_EN__LEN        1
#define BMG160_SCAN_EN__MSK        0x80
#define BMG160_SCAN_EN__REG        BMG160_SCAN_EN
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2136
   Trim Register */
/* EasyCASE ( 2137
   BMG160_INL_OFFSET_MSB_ADDR */
#define BMG160_INL_OFFSET_MSB_ADDR_OFFSET_X__POS       6                       /**< Last 2 bits of INL Offset MSB Registers */
#define BMG160_INL_OFFSET_MSB_ADDR_OFFSET_X__LEN       2
#define BMG160_INL_OFFSET_MSB_ADDR_OFFSET_X__MSK       0xC0
#define BMG160_INL_OFFSET_MSB_ADDR_OFFSET_X__REG       BMG160_INL_OFFSET_MSB_ADDR

#define BMG160_INL_OFFSET_MSB_ADDR_OFFSET_Y__POS       3                       /**< 3 bits of INL Offset MSB Registers */
#define BMG160_INL_OFFSET_MSB_ADDR_OFFSET_Y__LEN       3
#define BMG160_INL_OFFSET_MSB_ADDR_OFFSET_Y__MSK       0x38
#define BMG160_INL_OFFSET_MSB_ADDR_OFFSET_Y__REG       BMG160_INL_OFFSET_MSB_ADDR

#define BMG160_INL_OFFSET_MSB_ADDR_OFFSET_Z__POS       0                       /**< First 3 bits of INL Offset MSB Registers */
#define BMG160_INL_OFFSET_MSB_ADDR_OFFSET_Z__LEN       3
#define BMG160_INL_OFFSET_MSB_ADDR_OFFSET_Z__MSK       0x07
#define BMG160_INL_OFFSET_MSB_ADDR_OFFSET_Z__REG       BMG160_INL_OFFSET_MSB_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2576
   BMG160_TRIM_GP0 */
#define BMG160_TRIM_GP0_ADDR_GP0__POS            4                       /**< 4 bits of Trim GP0 Registers */
#define BMG160_TRIM_GP0_ADDR_GP0__LEN            4
#define BMG160_TRIM_GP0_ADDR_GP0__MSK            0xF0
#define BMG160_TRIM_GP0_ADDR_GP0__REG            BMG160_TRIM_GP0_ADDR

#define BMG160_TRIM_GP0_ADDR_OFFSET_X__POS       2                       /**< 2 bits of Trim GP0 Registers */
#define BMG160_TRIM_GP0_ADDR_OFFSET_X__LEN       2
#define BMG160_TRIM_GP0_ADDR_OFFSET_X__MSK       0x0C
#define BMG160_TRIM_GP0_ADDR_OFFSET_X__REG       BMG160_TRIM_GP0_ADDR

#define BMG160_TRIM_GP0_ADDR_OFFSET_Y__POS       1                       /**< 1st bit of Trim GP0 Registers */
#define BMG160_TRIM_GP0_ADDR_OFFSET_Y__LEN       1
#define BMG160_TRIM_GP0_ADDR_OFFSET_Y__MSK       0x02
#define BMG160_TRIM_GP0_ADDR_OFFSET_Y__REG       BMG160_TRIM_GP0_ADDR

#define BMG160_TRIM_GP0_ADDR_OFFSET_Z__POS       0                       /**< First bit of Trim GP0 Registers */
#define BMG160_TRIM_GP0_ADDR_OFFSET_Z__LEN       1
#define BMG160_TRIM_GP0_ADDR_OFFSET_Z__MSK       0x01
#define BMG160_TRIM_GP0_ADDR_OFFSET_Z__REG       BMG160_TRIM_GP0_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2140
   TRIM_IB_ADDR */
#define BMG160_TRIM_IB_IBIAS_DRIVE__POS   6                       /**< Last 2 bits of Trim IB Registers */
#define BMG160_TRIM_IB_IBIAS_DRIVE__LEN   2
#define BMG160_TRIM_IB_IBIAS_DRIVE__MSK   0xC0
#define BMG160_TRIM_IB_IBIAS_DRIVE__REG   BMG160_TRIM_IB

#define BMG160_TRIM_IB_IBIAS_RATE__POS    4                       /**< 2 bits of Trim IB Registers */
#define BMG160_TRIM_IB_IBIAS_RATE__LEN    2
#define BMG160_TRIM_IB_IBIAS_RATE__MSK    0x30
#define BMG160_TRIM_IB_IBIAS_RATE__REG    BMG160_TRIM_IB

#define BMG160_TRIM_IB_AD_TRIM_IB__POS    0                       /**< First 4 bits of Trim IB Registers */
#define BMG160_TRIM_IB_AD_TRIM_IB__LEN    4
#define BMG160_TRIM_IB_AD_TRIM_IB__MSK    0x0F
#define BMG160_TRIM_IB_AD_TRIM_IB__REG    BMG160_TRIM_IB
/* EasyCASE ) */
/* EasyCASE ( 2142
   TRIM_OSC_ADDR */
#define BMG160_TRIM_OSC_BAA_MODE__POS           7                       /**< Last bit of Trim Osc Registers */
#define BMG160_TRIM_OSC_BAA_MODE__LEN           1
#define BMG160_TRIM_OSC_BAA_MODE__MSK           0x80
#define BMG160_TRIM_OSC_BAA_MODE__REG           BMG160_TRIM_OSC

#define BMG160_TRIM_OSC_BMA_MODE__POS           6                       /**< 6th bit of Trim Osc Registers */
#define BMG160_TRIM_OSC_BMA_MODE__LEN           1
#define BMG160_TRIM_OSC_BMA_MODE__MSK           0x40
#define BMG160_TRIM_OSC_BMA_MODE__REG           BMG160_TRIM_OSC

#define BMG160_TRIM_OSC_DRIVE_STRENTH__POS      4                       /**< 2 bits of Trim Osc Registers */
#define BMG160_TRIM_OSC_DRIVE_STRENTH__LEN      2
#define BMG160_TRIM_OSC_DRIVE_STRENTH__MSK      0x30
#define BMG160_TRIM_OSC_DRIVE_STRENTH__REG      BMG160_TRIM_OSC

#define BMG160_TRIM_OSC_SLOW_OSC_CAL__POS       1                       /**< 2 bits of Trim Osc Registers */
#define BMG160_TRIM_OSC_SLOW_OSC_CAL__LEN       3
#define BMG160_TRIM_OSC_SLOW_OSC_CAL__MSK       0x0E
#define BMG160_TRIM_OSC_SLOW_OSC_CAL__REG       BMG160_TRIM_OSC
/* EasyCASE ) */
/* EasyCASE ( 2143
   TRIM_I2C_ADDR */
#define BMG160_TRIM_I2C_I2C_ADDR_LSBL__POS    4                       /**< 3 bits of Trim I2C Registers */
#define BMG160_TRIM_I2C_I2C_ADDR_LSBL__LEN    4
#define BMG160_TRIM_I2C_I2C_ADDR_LSBL__MSK    0xF0
#define BMG160_TRIM_I2C_I2C_ADDR_LSBL__REG    BMG160_TRIM_I2C

#define BMG160_TRIM_I2C_I2C_DEL_BYPASS__POS   3                       /**< 3rd bit of Trim I2C Registers */
#define BMG160_TRIM_I2C_I2C_DEL_BYPASS__LEN   1
#define BMG160_TRIM_I2C_I2C_DEL_BYPASS__MSK   0x08
#define BMG160_TRIM_I2C_I2C_DEL_BYPASS__REG   BMG160_TRIM_I2C

#define BMG160_TRIM_I2C_I2C_DELAY_CAL__POS    0                       /**< First 3 bits of Trim I2C Registers */
#define BMG160_TRIM_I2C_I2C_DELAY_CAL__LEN    3
#define BMG160_TRIM_I2C_I2C_DELAY_CAL__MSK    0x07
#define BMG160_TRIM_I2C_I2C_DELAY_CAL__REG    BMG160_TRIM_I2C
/* EasyCASE ) */
/* EasyCASE ( 2144
   BMG160_TRIM_K_DCO */
#define BMG160_TRIM_K_DCO_DCO__POS       0                       /**< First 2 bits of Trim K DCO Registers */
#define BMG160_TRIM_K_DCO_DCO__LEN       2
#define BMG160_TRIM_K_DCO_DCO__MSK       0x03
#define BMG160_TRIM_K_DCO_DCO__REG       BMG160_TRIM_K_DCO
/* EasyCASE ) */
/* EasyCASE ( 2145
   RATE_DEC_OFFESET_ADDR */
#define BMG160_RATE_DEC_OFFESET_DEC_OFFSET__POS       0                       /**< First 6 bits of Rate Dec Offset Registers */
#define BMG160_RATE_DEC_OFFESET_DEC_OFFSET__LEN       6
#define BMG160_RATE_DEC_OFFESET_DEC_OFFSET__MSK       0x3F
#define BMG160_RATE_DEC_OFFESET_DEC_OFFSET__REG       BMG160_RATE_DEC_OFFESET
/* EasyCASE ) */
/* EasyCASE ( 2146
   RATE_X_COARSE_OFF_ADDR_H */
#define BMG160_RATE_X_COARSE_OFF_H_MSB__POS       0                       /**< First 6 bits of Rate X offset address high Registers */
#define BMG160_RATE_X_COARSE_OFF_H_MSB__LEN       6
#define BMG160_RATE_X_COARSE_OFF_H_MSB__MSK       0x3F
#define BMG160_RATE_X_COARSE_OFF_H_MSB__REG       BMG160_RATE_X_COARSE_OFF_ADDR_H
/* EasyCASE ) */
/* EasyCASE ( 2147
   TCO_TRIM_X_ADDR */
#define BMG160_TCO_TRIM_X_DATA__POS         3                       /**< Last 5 bits of TCO Trim X Registers */
#define BMG160_TCO_TRIM_X_DATA__LEN         5
#define BMG160_TCO_TRIM_X_DATA__MSK         0xF8
#define BMG160_TCO_TRIM_X_DATA__REG         BMG160_TCO_TRIM_X

#define BMG160_TCO_TRIM_X_FINE_GAIN__POS    0                       /**< First 3 bits of TCO Trim X Registers */
#define BMG160_TCO_TRIM_X_FINE_GAIN__LEN    3
#define BMG160_TCO_TRIM_X_FINE_GAIN__MSK    0x07
#define BMG160_TCO_TRIM_X_FINE_GAIN__REG    BMG160_TCO_TRIM_X
/* EasyCASE ) */
/* EasyCASE ( 2148
   HVDAC_OFFSET_X_ADDR */
#define BMG160_HVDAC_OFFSET_X_INVERTX__POS        7                       /**< Last bit of HVDAC Offset X Registers */
#define BMG160_HVDAC_OFFSET_X_INVERTX__LEN        1
#define BMG160_HVDAC_OFFSET_X_INVERTX__MSK        0x80
#define BMG160_HVDAC_OFFSET_X_INVERTX__REG        BMG160_HVDAC_OFFSET_X

#define BMG160_HVDAC_OFFSET_X_EN_QUAD_REG__POS    6                       /**< 6th bit of HVDAC Offset X Registers */
#define BMG160_HVDAC_OFFSET_X_EN_QUAD_REG__LEN    1
#define BMG160_HVDAC_OFFSET_X_EN_QUAD_REG__MSK    0x40
#define BMG160_HVDAC_OFFSET_X_EN_QUAD_REG__REG    BMG160_HVDAC_OFFSET_X

#define BMG160_HVDAC_OFFSET_X_HVDAC_OFFSET__POS   0                       /**< First 6 bits of HVDAC Offset X Registers */
#define BMG160_HVDAC_OFFSET_X_HVDAC_OFFSET__LEN   6
#define BMG160_HVDAC_OFFSET_X_HVDAC_OFFSET__MSK   0x3F
#define BMG160_HVDAC_OFFSET_X_HVDAC_OFFSET__REG   BMG160_HVDAC_OFFSET_X
/* EasyCASE ) */
/* EasyCASE ( 2149
   TCS_TRIM_X_ADDR */
#define BMG160_TCS_TRIM_X_QUADI_GAIN__POS    5                       /**< Last 3 bits of TCS Trim X Registers */
#define BMG160_TCS_TRIM_X_QUADI_GAIN__LEN    3
#define BMG160_TCS_TRIM_X_QUADI_GAIN__MSK    0xE0
#define BMG160_TCS_TRIM_X_QUADI_GAIN__REG    BMG160_TCS_TRIM_X

#define BMG160_TCS_TRIM_X_TCS_TRIM__POS      0                       /**< First 5 bits of TCS Trim X Registers */
#define BMG160_TCS_TRIM_X_TCS_TRIM__LEN      5
#define BMG160_TCS_TRIM_X_TCS_TRIM__MSK      0x1F
#define BMG160_TCS_TRIM_X_TCS_TRIM__REG      BMG160_TCS_TRIM_X
/* EasyCASE ) */
/* EasyCASE ( 2150
   FREQ_TUNE_X_MSB_ADDR */
#define BMG160_FREQ_TUNE_X_MSB_FREQ_TUNE__POS    0                       /**< First 6 bits of Freq Tune X MSB Registers */
#define BMG160_FREQ_TUNE_X_MSB_FREQ_TUNE__LEN    6
#define BMG160_FREQ_TUNE_X_MSB_FREQ_TUNE__MSK    0x3F
#define BMG160_FREQ_TUNE_X_MSB_FREQ_TUNE__REG    BMG160_FREQ_TUNE_X_MSB
/* EasyCASE ) */
/* EasyCASE ( 2151
   QUAD_FORCE_X_MSB_ADDR */
#define BMG160_QUAD_FORCE_X_MSB_QUAD_FORCE__POS    0                       /**< First 6 bits of Quad Forcee X MSB Registers */
#define BMG160_QUAD_FORCE_X_MSB_QUAD_FORCE__LEN    6
#define BMG160_QUAD_FORCE_X_MSB_QUAD_FORCE__MSK    0x3F
#define BMG160_QUAD_FORCE_X_MSB_QUAD_FORCE__REG    BMG160_QUAD_FORCE_X_MSB
/* EasyCASE ) */
/* EasyCASE ( 2152
   RATE_Y_COARSE_OFF_ADDR_H */
#define BMG160_RATE_Y_COARSE_OFF_H_MSB__POS       0                       /**< First 6 bits of Rate Y offset address high Registers */
#define BMG160_RATE_Y_COARSE_OFF_H_MSB__LEN       6
#define BMG160_RATE_Y_COARSE_OFF_H_MSB__MSK       0x3F
#define BMG160_RATE_Y_COARSE_OFF_H_MSB__REG       BMG160_RATE_Y_COARSE_OFF_ADDR_H
/* EasyCASE ) */
/* EasyCASE ( 2153
   TCO_TRIM_Y_ADDR */
#define BMG160_TCO_TRIM_Y_DATA__POS         3                       /**< Last 5 bits of TCO Trim Y Registers */
#define BMG160_TCO_TRIM_Y_DATA__LEN         5
#define BMG160_TCO_TRIM_Y_DATA__MSK         0xF8
#define BMG160_TCO_TRIM_Y_DATA__REG         BMG160_TCO_TRIM_Y

#define BMG160_TCO_TRIM_Y_FINE_GAIN__POS    0                       /**< First 3 bits of TCO Trim Y Registers */
#define BMG160_TCO_TRIM_Y_FINE_GAIN__LEN    3
#define BMG160_TCO_TRIM_Y_FINE_GAIN__MSK    0x07
#define BMG160_TCO_TRIM_Y_FINE_GAIN__REG    BMG160_TCO_TRIM_Y
/* EasyCASE ) */
/* EasyCASE ( 2154
   HVDAC_OFFSET_Y_ADDR */
#define BMG160_HVDAC_OFFSET_Y_INVERTY__POS        7                       /**< Last bit of HVDAC Offset Y Registers */
#define BMG160_HVDAC_OFFSET_Y_INVERTY__LEN        1
#define BMG160_HVDAC_OFFSET_Y_INVERTY__MSK        0x80
#define BMG160_HVDAC_OFFSET_Y_INVERTY__REG        BMG160_HVDAC_OFFSET_Y

#define BMG160_HVDAC_OFFSET_Y_EN_QUAD_REG__POS    6                       /**< 6th bit of HVDAC Offset Y Registers */
#define BMG160_HVDAC_OFFSET_Y_EN_QUAD_REG__LEN    1
#define BMG160_HVDAC_OFFSET_Y_EN_QUAD_REG__MSK    0x40
#define BMG160_HVDAC_OFFSET_Y_EN_QUAD_REG__REG    BMG160_HVDAC_OFFSET_Y

#define BMG160_HVDAC_OFFSET_Y_HVDAC_OFFSET__POS   0                       /**< First 6 bits of HVDAC Offset Y Registers */
#define BMG160_HVDAC_OFFSET_Y_HVDAC_OFFSET__LEN   6
#define BMG160_HVDAC_OFFSET_Y_HVDAC_OFFSET__MSK   0x3F
#define BMG160_HVDAC_OFFSET_Y_HVDAC_OFFSET__REG   BMG160_HVDAC_OFFSET_Y
/* EasyCASE ) */
/* EasyCASE ( 2155
   TCS_TRIM_Y_ADDR */
#define BMG160_TCS_TRIM_Y_QUADI_GAIN__POS    5                       /**< Last 3 bits of TCS Trim Y Registers */
#define BMG160_TCS_TRIM_Y_QUADI_GAIN__LEN    3
#define BMG160_TCS_TRIM_Y_QUADI_GAIN__MSK    0xE0
#define BMG160_TCS_TRIM_Y_QUADI_GAIN__REG    BMG160_TCS_TRIM_Y

#define BMG160_TCS_TRIM_Y_TCS_TRIM__POS      0                       /**< First 5 bits of TCS Trim Y Registers */
#define BMG160_TCS_TRIM_Y_TCS_TRIM__LEN      5
#define BMG160_TCS_TRIM_Y_TCS_TRIM__MSK      0x1F
#define BMG160_TCS_TRIM_Y_TCS_TRIM__REG      BMG160_TCS_TRIM_Y
/* EasyCASE ) */
/* EasyCASE ( 2156
   FREQ_TUNE_Y_MSB_ADDR */
#define BMG160_FREQ_TUNE_Y_MSB_FREQ_TUNE__POS    0                       /**< First 6 bits of Freq Tune Y MSB Registers */
#define BMG160_FREQ_TUNE_Y_MSB_FREQ_TUNE__LEN    6
#define BMG160_FREQ_TUNE_Y_MSB_FREQ_TUNE__MSK    0x3F
#define BMG160_FREQ_TUNE_Y_MSB_FREQ_TUNE__REG    BMG160_FREQ_TUNE_Y_MSB
/* EasyCASE ) */
/* EasyCASE ( 2157
   QUAD_FORCE_Y_MSB_ADDR */
#define BMG160_QUAD_FORCE_Y_MSB_QUAD_FORCE__POS    0                       /**< First 6 bits of Quad Forcee Y MSB Registers */
#define BMG160_QUAD_FORCE_Y_MSB_QUAD_FORCE__LEN    6
#define BMG160_QUAD_FORCE_Y_MSB_QUAD_FORCE__MSK    0x3F
#define BMG160_QUAD_FORCE_Y_MSB_QUAD_FORCE__REG    BMG160_QUAD_FORCE_Y_MSB
/* EasyCASE ) */
/* EasyCASE ( 2158
   RATE_Z_COARSE_OFF_ADDR_H */
#define BMG160_RATE_Z_COARSE_OFF_H_MSB__POS       0                       /**< First 6 bits of Rate Z offset address high Registers */
#define BMG160_RATE_Z_COARSE_OFF_H_MSB__LEN       6
#define BMG160_RATE_Z_COARSE_OFF_H_MSB__MSK       0x3F
#define BMG160_RATE_Z_COARSE_OFF_H_MSB__REG       BMG160_RATE_Z_COARSE_OFF_ADDR_H
/* EasyCASE ) */
/* EasyCASE ( 2159
   TCO_TRIM_Z_ADDR */
#define BMG160_TCO_TRIM_Z_DATA__POS         3                       /**< Last 5 bits of TCO Trim Z Registers */
#define BMG160_TCO_TRIM_Z_DATA__LEN         5
#define BMG160_TCO_TRIM_Z_DATA__MSK         0xF8
#define BMG160_TCO_TRIM_Z_DATA__REG         BMG160_TCO_TRIM_Z

#define BMG160_TCO_TRIM_Z_FINE_GAIN__POS    0                       /**< First 3 bits of TCO Trim Z Registers */
#define BMG160_TCO_TRIM_Z_FINE_GAIN__LEN    3
#define BMG160_TCO_TRIM_Z_FINE_GAIN__MSK    0x07
#define BMG160_TCO_TRIM_Z_FINE_GAIN__REG    BMG160_TCO_TRIM_Z
/* EasyCASE ) */
/* EasyCASE ( 2160
   HVDAC_OFFSET_Z_ADDR */
#define BMG160_HVDAC_OFFSET_Z_INVERTZ__POS        7                       /**< Last bit of HVDAC Offset Z Registers */
#define BMG160_HVDAC_OFFSET_Z_INVERTZ__LEN        1
#define BMG160_HVDAC_OFFSET_Z_INVERTZ__MSK        0x80
#define BMG160_HVDAC_OFFSET_Z_INVERTZ__REG        BMG160_HVDAC_OFFSET_Z

#define BMG160_HVDAC_OFFSET_Z_EN_QUAD_REG__POS    6                       /**< 6th bit of HVDAC Offset Z Registers */
#define BMG160_HVDAC_OFFSET_Z_EN_QUAD_REG__LEN    1
#define BMG160_HVDAC_OFFSET_Z_EN_QUAD_REG__MSK    0x40
#define BMG160_HVDAC_OFFSET_Z_EN_QUAD_REG__REG    BMG160_HVDAC_OFFSET_Z

#define BMG160_HVDAC_OFFSET_Z_HVDAC_OFFSET__POS   0                       /**< First 6 bits of HVDAC Offset Z Registers */
#define BMG160_HVDAC_OFFSET_Z_HVDAC_OFFSET__LEN   6
#define BMG160_HVDAC_OFFSET_Z_HVDAC_OFFSET__MSK   0x3F
#define BMG160_HVDAC_OFFSET_Z_HVDAC_OFFSET__REG   BMG160_HVDAC_OFFSET_Z
/* EasyCASE ) */
/* EasyCASE ( 2161
   TCS_TRIM_Z_ADDR */
#define BMG160_TCS_TRIM_Z_QUADI_GAIN__POS    5                       /**< Last 3 bits of TCS Trim Z Registers */
#define BMG160_TCS_TRIM_Z_QUADI_GAIN__LEN    3
#define BMG160_TCS_TRIM_Z_QUADI_GAIN__MSK    0xE0
#define BMG160_TCS_TRIM_Z_QUADI_GAIN__REG    BMG160_TCS_TRIM_Z

#define BMG160_TCS_TRIM_Z_TCS_TRIM__POS      0                       /**< First 5 bits of TCS Trim Z Registers */
#define BMG160_TCS_TRIM_Z_TCS_TRIM__LEN      5
#define BMG160_TCS_TRIM_Z_TCS_TRIM__MSK      0x1F
#define BMG160_TCS_TRIM_Z_TCS_TRIM__REG      BMG160_TCS_TRIM_Z
/* EasyCASE ) */
/* EasyCASE ( 2162
   FREQ_TUNE_Z_MSB_ADDR */
#define BMG160_FREQ_TUNE_Z_MSB_FREQ_TUNE__POS    0                       /**< First 6 bits of Freq Tune Z MSB Registers */
#define BMG160_FREQ_TUNE_Z_MSB_FREQ_TUNE__LEN    6
#define BMG160_FREQ_TUNE_Z_MSB_FREQ_TUNE__MSK    0x3F
#define BMG160_FREQ_TUNE_Z_MSB_FREQ_TUNE__REG    BMG160_FREQ_TUNE_Z_MSB
/* EasyCASE ) */
/* EasyCASE ( 2537
   QUAD_FORCE_Z_MSB_ADDR */
#define BMG160_QUAD_FORCE_Z_MSB_QUAD_FORCE__POS    0                       /**< First 6 bits of Quad Forcee Z MSB Registers */
#define BMG160_QUAD_FORCE_Z_MSB_QUAD_FORCE__LEN    6
#define BMG160_QUAD_FORCE_Z_MSB_QUAD_FORCE__MSK    0x3F
#define BMG160_QUAD_FORCE_Z_MSB_QUAD_FORCE__REG    BMG160_QUAD_FORCE_Z_MSB
/* EasyCASE ) */
/* EasyCASE ( 2970
   EN_QUAD_STOP_ADDR */
#define BMG160_EN_QUAD_STOP_ADDR_QUAD_STOP__POS    7                       /**< Last bit of En Quad Stop Registers */
#define BMG160_EN_QUAD_STOP_ADDR_QUAD_STOP__LEN    1
#define BMG160_EN_QUAD_STOP_ADDR_QUAD_STOP__MSK    0x80
#define BMG160_EN_QUAD_STOP_ADDR_QUAD_STOP__REG    BMG160_EN_QUAD_STOP_ADDR
/* EasyCASE ) */
/* EasyCASE ( 2164
   TEMP_OFFSET_EN_CORR */
#define BMG160_TEMP_OFFSET_EN_CORR_EN_FULL_HV_RG__POS   7                       /**< Last bit of Temp Offset Corr Registers */
#define BMG160_TEMP_OFFSET_EN_CORR_EN_FULL_HV_RG__LEN   1
#define BMG160_TEMP_OFFSET_EN_CORR_EN_FULL_HV_RG__MSK   0x80
#define BMG160_TEMP_OFFSET_EN_CORR_EN_FULL_HV_RG__REG   BMG160_TEMP_OFFSET_EN_CORR

#define BMG160_TEMP_OFFSET_EN_CORR_EN_CORR__POS         6                       /**< 6th bit of Temp Offset Corr Registers */
#define BMG160_TEMP_OFFSET_EN_CORR_EN_CORR__LEN         1
#define BMG160_TEMP_OFFSET_EN_CORR_EN_CORR__MSK         0x40
#define BMG160_TEMP_OFFSET_EN_CORR_EN_CORR__REG         BMG160_TEMP_OFFSET_EN_CORR

#define BMG160_TEMP_OFFSET_EN_CORR_TEMP_OFFSET__POS     0                       /**< First 6 bits of Temp Offset Corr Registers */
#define BMG160_TEMP_OFFSET_EN_CORR_TEMP_OFFSET__LEN     6
#define BMG160_TEMP_OFFSET_EN_CORR_TEMP_OFFSET__MSK     0x3F
#define BMG160_TEMP_OFFSET_EN_CORR_TEMP_OFFSET__REG     BMG160_TEMP_OFFSET_EN_CORR
/* EasyCASE ) */
/* EasyCASE ( 2165
   RATE_COARSE_SHIFT */
#define BMG160_RATE_COARSE_SHIFT_FFB_X__POS     6                       /**< Last 2 bits of Rate Coarse shift Registers */
#define BMG160_RATE_COARSE_SHIFT_FFB_X__LEN     2
#define BMG160_RATE_COARSE_SHIFT_FFB_X__MSK     0xC0
#define BMG160_RATE_COARSE_SHIFT_FFB_X__REG     BMG160_RATE_COARSE_SHIFT

#define BMG160_RATE_COARSE_SHIFT_RCS_Z__POS     4                       /**< 2 bits of Rate Coarse shift Registers */
#define BMG160_RATE_COARSE_SHIFT_RCS_Z__LEN     2
#define BMG160_RATE_COARSE_SHIFT_RCS_Z__MSK     0x30
#define BMG160_RATE_COARSE_SHIFT_RCS_Z__REG     BMG160_RATE_COARSE_SHIFT

#define BMG160_RATE_COARSE_SHIFT_RCS_Y__POS     2                       /**< 2 bits of Rate Coarse shift Registers */
#define BMG160_RATE_COARSE_SHIFT_RCS_Y__LEN     2
#define BMG160_RATE_COARSE_SHIFT_RCS_Y__MSK     0x0C
#define BMG160_RATE_COARSE_SHIFT_RCS_Y__REG     BMG160_RATE_COARSE_SHIFT

#define BMG160_RATE_COARSE_SHIFT_RCS_X__POS     0                       /**< First 2 bits of Rate Coarse shift Registers */
#define BMG160_RATE_COARSE_SHIFT_RCS_X__LEN     2
#define BMG160_RATE_COARSE_SHIFT_RCS_X__MSK     0x03
#define BMG160_RATE_COARSE_SHIFT_RCS_X__REG     BMG160_RATE_COARSE_SHIFT
/* EasyCASE ) */
/* EasyCASE ( 2166
   TUNE_GAIN_TRIM_XY */
#define BMG160_TUNE_GAIN_TRIM_XY_FFB_Y__POS           6                       /**< Last 2 bits of Tune Gain XY Registers */
#define BMG160_TUNE_GAIN_TRIM_XY_FFB_Y__LEN           2
#define BMG160_TUNE_GAIN_TRIM_XY_FFB_Y__MSK           0xC0
#define BMG160_TUNE_GAIN_TRIM_XY_FFB_Y__REG           BMG160_TUNE_GAIN_TRIM_XY

#define BMG160_TUNE_GAIN_TRIM_XY_TUNE_GAIN_Y__POS     3                       /**< 3 bits of Tune Gain XY Registers */
#define BMG160_TUNE_GAIN_TRIM_XY_TUNE_GAIN_Y__LEN     3
#define BMG160_TUNE_GAIN_TRIM_XY_TUNE_GAIN_Y__MSK     0x38
#define BMG160_TUNE_GAIN_TRIM_XY_TUNE_GAIN_Y__REG     BMG160_TUNE_GAIN_TRIM_XY

#define BMG160_TUNE_GAIN_TRIM_XY_TUNE_GAIN_X__POS     0                       /**< First 3 bits of Tune Gain XY Registers */
#define BMG160_TUNE_GAIN_TRIM_XY_TUNE_GAIN_X__LEN     3
#define BMG160_TUNE_GAIN_TRIM_XY_TUNE_GAIN_X__MSK     0x07
#define BMG160_TUNE_GAIN_TRIM_XY_TUNE_GAIN_X__REG     BMG160_TUNE_GAIN_TRIM_XY
/* EasyCASE ) */
/* EasyCASE ( 2167
   TUNE_GAIN_TRIM_Z */
#define BMG160_TUNE_GAIN_TRIM_Z_FFB_Z__POS           6                       /**< Last 2 bits of Tune gain trim Registers */
#define BMG160_TUNE_GAIN_TRIM_Z_FFB_Z__LEN           2
#define BMG160_TUNE_GAIN_TRIM_Z_FFB_Z__MSK           0xC0
#define BMG160_TUNE_GAIN_TRIM_Z_FFB_Z__REG           BMG160_TUNE_GAIN_TRIM_Z

#define BMG160_TUNE_GAIN_TRIM_Z_FREQ_CTRL_ON__POS    3                       /**< 3 bits of Tune gain trim Registers */
#define BMG160_TUNE_GAIN_TRIM_Z_FREQ_CTRL_ON__LEN    3
#define BMG160_TUNE_GAIN_TRIM_Z_FREQ_CTRL_ON__MSK    0x38
#define BMG160_TUNE_GAIN_TRIM_Z_FREQ_CTRL_ON__REG    BMG160_TUNE_GAIN_TRIM_Z

#define BMG160_TUNE_GAIN_TRIM_Z_TUNE_GAIN_Z__POS     0                       /**< First 3 bits of Tune gain trim Registers */
#define BMG160_TUNE_GAIN_TRIM_Z_TUNE_GAIN_Z__LEN     3
#define BMG160_TUNE_GAIN_TRIM_Z_TUNE_GAIN_Z__MSK     0x07
#define BMG160_TUNE_GAIN_TRIM_Z_TUNE_GAIN_Z__REG     BMG160_TUNE_GAIN_TRIM_Z
/* EasyCASE ) */
/* EasyCASE ( 2168
   START_DITHER_XY */
#define BMG160_START_DITHER_XY_START_Y__POS     3                       /**< 3 bits of Start Dither XY Registers */
#define BMG160_START_DITHER_XY_START_Y__LEN     3
#define BMG160_START_DITHER_XY_START_Y__MSK     0x38
#define BMG160_START_DITHER_XY_START_Y__REG     BMG160_START_DITHER_XY

#define BMG160_START_DITHER_XY_START_X__POS     0                       /**< First 3 bits of Start Dither XY Registers */
#define BMG160_START_DITHER_XY_START_X__LEN     3
#define BMG160_START_DITHER_XY_START_X__MSK     0x07
#define BMG160_START_DITHER_XY_START_X__REG     BMG160_START_DITHER_XY
/* EasyCASE ) */
/* EasyCASE ( 2169
   START_DITHER_SIGN */
#define BMG160_START_DITHER_SIGN_OFFSET_Z__POS     5                       /**< 5 bit of Start Dither Sign Registers */
#define BMG160_START_DITHER_SIGN_OFFSET_Z__LEN     1
#define BMG160_START_DITHER_SIGN_OFFSET_Z__MSK     0x20
#define BMG160_START_DITHER_SIGN_OFFSET_Z__REG     BMG160_START_DITHER_SIGN

#define BMG160_START_DITHER_SIGN_OFFSET_Y__POS     4                       /**< 4th bit of Start Dither Sign Registers */
#define BMG160_START_DITHER_SIGN_OFFSET_Y__LEN     1
#define BMG160_START_DITHER_SIGN_OFFSET_Y__MSK     0x10
#define BMG160_START_DITHER_SIGN_OFFSET_Y__REG     BMG160_START_DITHER_SIGN

#define BMG160_START_DITHER_SIGN_OFFSET_X__POS     3                       /**< 3rd bit of Start Dither Sign Registers */
#define BMG160_START_DITHER_SIGN_OFFSET_X__LEN     1
#define BMG160_START_DITHER_SIGN_OFFSET_X__MSK     0x08
#define BMG160_START_DITHER_SIGN_OFFSET_X__REG     BMG160_START_DITHER_SIGN

#define BMG160_START_DITHER_SIGN_START_Z__POS      0                       /**< First 3 bits of Start Dither Sign Registers */
#define BMG160_START_DITHER_SIGN_START_Z__LEN      3
#define BMG160_START_DITHER_SIGN_START_Z__MSK      0x07
#define BMG160_START_DITHER_SIGN_START_Z__REG      BMG160_START_DITHER_SIGN
/* EasyCASE ) */
/* EasyCASE ( 2170
   DITHER_GAIN_XY */
#define BMG160_DITHER_GAIN_XY_GAIN_Y__POS     4                       /**< 4 bits of Dither Gain XY Registers */
#define BMG160_DITHER_GAIN_XY_GAIN_Y__LEN     4
#define BMG160_DITHER_GAIN_XY_GAIN_Y__MSK     0xF0
#define BMG160_DITHER_GAIN_XY_GAIN_Y__REG     BMG160_DITHER_GAIN_XY

#define BMG160_DITHER_GAIN_XY_GAIN_X__POS     0                       /**< First 4 bits of Dither Gain XY Registers */
#define BMG160_DITHER_GAIN_XY_GAIN_X__LEN     4
#define BMG160_DITHER_GAIN_XY_GAIN_X__MSK     0x0F
#define BMG160_DITHER_GAIN_XY_GAIN_X__REG     BMG160_DITHER_GAIN_XY
/* EasyCASE ) */
/* EasyCASE ( 2171
   DITHER_GAIN_Z_FFBSI */
#define BMG160_DITHER_GAIN_Z_PHI_RST__POS        7                       /**< 7th bit of Dither Gain Z Registers */
#define BMG160_DITHER_GAIN_Z_PHI_RST__LEN        1
#define BMG160_DITHER_GAIN_Z_PHI_RST__MSK        0x80
#define BMG160_DITHER_GAIN_Z_PHI_RST__REG        BMG160_DITHER_GAIN_Z

#define BMG160_DITHER_GAIN_Z_FFB_SIGN_Z__POS     6                       /**< 6th bit of Dither Gain Z Registers */
#define BMG160_DITHER_GAIN_Z_FFB_SIGN_Z__LEN     1
#define BMG160_DITHER_GAIN_Z_FFB_SIGN_Z__MSK     0x40
#define BMG160_DITHER_GAIN_Z_FFB_SIGN_Z__REG     BMG160_DITHER_GAIN_Z

#define BMG160_DITHER_GAIN_Z_FFB_SIGN_Y__POS     5                       /**< 5th bit of Dither Gain Z Registers */
#define BMG160_DITHER_GAIN_Z_FFB_SIGN_Y__LEN     1
#define BMG160_DITHER_GAIN_Z_FFB_SIGN_Y__MSK     0x20
#define BMG160_DITHER_GAIN_Z_FFB_SIGN_Y__REG     BMG160_DITHER_GAIN_Z

#define BMG160_DITHER_GAIN_Z_FFB_SIGN_X__POS     4                       /**< 4th bit of Dither Gain Z Registers */
#define BMG160_DITHER_GAIN_Z_FFB_SIGN_X__LEN     1
#define BMG160_DITHER_GAIN_Z_FFB_SIGN_X__MSK     0x10
#define BMG160_DITHER_GAIN_Z_FFB_SIGN_X__REG     BMG160_DITHER_GAIN_Z

#define BMG160_DITHER_GAIN_Z_GAIN_Z__POS         0                       /**< First 4 bits of Dither Gain Z Registers */
#define BMG160_DITHER_GAIN_Z_GAIN_Z__LEN         4
#define BMG160_DITHER_GAIN_Z_GAIN_Z__MSK         0x0F
#define BMG160_DITHER_GAIN_Z_GAIN_Z__REG         BMG160_DITHER_GAIN_Z
/* EasyCASE ) */
/* EasyCASE ( 2172
   BMG160_TRIM_DRV_OFF0 */
#define BMG160_TRIM_DRV_OFF0_DRIVE_OFFSET_P__POS     4                       /**< last 4 bits of Trim DRV OFF 0 Registers */
#define BMG160_TRIM_DRV_OFF0_DRIVE_OFFSET_P__LEN     4
#define BMG160_TRIM_DRV_OFF0_DRIVE_OFFSET_P__MSK     0xF0
#define BMG160_TRIM_DRV_OFF0_DRIVE_OFFSET_P__REG     BMG160_TRIM_DRV_OFF0

#define BMG160_TRIM_DRV_OFF0_DRIVE_OFFSET_N__POS     0                       /**< First bit of Trim DRV OFF 0 Registers */
#define BMG160_TRIM_DRV_OFF0_DRIVE_OFFSET_N__LEN     4
#define BMG160_TRIM_DRV_OFF0_DRIVE_OFFSET_N__MSK     0x0F
#define BMG160_TRIM_DRV_OFF0_DRIVE_OFFSET_N__REG     BMG160_TRIM_DRV_OFF0
/* EasyCASE ) */
/* EasyCASE ( 2173
   BMG160_DRV_GAIN */
#define BMG160_DRV_GAIN_DRIVE_GAIN_TRIM__POS    5                       /**< Last 3 bits of DRV Gain Registers */
#define BMG160_DRV_GAIN_DRIVE_GAIN_TRIM__LEN    3
#define BMG160_DRV_GAIN_DRIVE_GAIN_TRIM__MSK    0xE0
#define BMG160_DRV_GAIN_DRIVE_GAIN_TRIM__REG    BMG160_DRV_GAIN

#define BMG160_DRV_GAIN_DRIVE_AMP__POS          2                       /**< 2 bits of DRV Gain Registers */
#define BMG160_DRV_GAIN_DRIVE_AMP__LEN          2
#define BMG160_DRV_GAIN_DRIVE_AMP__MSK          0x0C
#define BMG160_DRV_GAIN_DRIVE_AMP__REG          BMG160_DRV_GAIN

#define BMG160_DRV_GAIN_DRIVE_OFFSET_P__POS     1                       /**< 1st bits of DRV Gain Registers */
#define BMG160_DRV_GAIN_DRIVE_OFFSET_P__LEN     1
#define BMG160_DRV_GAIN_DRIVE_OFFSET_P__MSK     0x02
#define BMG160_DRV_GAIN_DRIVE_OFFSET_P__REG     BMG160_DRV_GAIN

#define BMG160_DRV_GAIN_DRIVE_OFFSET_N__POS     0                       /**< First bit of DRV Gain Registers */
#define BMG160_DRV_GAIN_DRIVE_OFFSET_N__LEN     1
#define BMG160_DRV_GAIN_DRIVE_OFFSET_N__MSK     0x01
#define BMG160_DRV_GAIN_DRIVE_OFFSET_N__REG     BMG160_DRV_GAIN
/* EasyCASE ) */
/* EasyCASE ( 2174
   BMG160_DRV_ADC */
#define BMG160_DRV_ADC_ADC_DITHER__POS        4                       /**< Last 4 bits of DRV ADC Registers */
#define BMG160_DRV_ADC_ADC_DITHER__LEN        4
#define BMG160_DRV_ADC_ADC_DITHER__MSK        0xF0
#define BMG160_DRV_ADC_ADC_DITHER__REG        BMG160_DRV_ADC

#define BMG160_DRV_ADC_ADC_REFERENCE__POS     0                       /**< First 4 bits of DRV ADC Registers */
#define BMG160_DRV_ADC_ADC_REFERENCE__LEN     4
#define BMG160_DRV_ADC_ADC_REFERENCE__MSK     0x0F
#define BMG160_DRV_ADC_ADC_REFERENCE__REG     BMG160_DRV_ADC
/* EasyCASE ) */
/* EasyCASE ( 2580
   BMG160_DEMOD_LUT_SEL */
#define BMG160_DEMOD_LUT_SELECT__POS     0                       /**< First 4 bits of DEMOD LUT Sel Registers */
#define BMG160_DEMOD_LUT_SELECT__LEN     4
#define BMG160_DEMOD_LUT_SELECT__MSK     0x0F
#define BMG160_DEMOD_LUT_SELECT__REG     BMG160_DEMOD_LUT_SEL
/* EasyCASE ) */
/* EasyCASE ( 2582
   BMG160_DRIVE_ADC_OFFSET */
#define BMG160_DRIVE_ADC_OFFSET_ADJ__POS     0                       /**< First 5 bits of DRV ADC Registers */
#define BMG160_DRIVE_ADC_OFFSET_ADJ__LEN     5
#define BMG160_DRIVE_ADC_OFFSET_ADJ__MSK     0x1F
#define BMG160_DRIVE_ADC_OFFSET_ADJ__REG     BMG160_DRIVE_ADC_OFFSET
/* EasyCASE ) */
/* EasyCASE ( 2584
   BMG160_TRIM_DRIVE_AMP_PI */
#define BMG160_TRIM_DRIVE_AMP_PI_KP__POS        6                       /**< Last 4 bits of Trim Drive Amp PI Registers */
#define BMG160_TRIM_DRIVE_AMP_PI_KP__LEN        2
#define BMG160_TRIM_DRIVE_AMP_PI_KP__MSK        0xC0
#define BMG160_TRIM_DRIVE_AMP_PI_KP__REG        BMG160_TRIM_DRIVE_AMP_PI

#define BMG160_TRIM_DRIVE_AMP_PI_KI__POS        4                       /**< Last 4 bits of Trim Drive Amp PI Registers */
#define BMG160_TRIM_DRIVE_AMP_PI_KI__LEN        2
#define BMG160_TRIM_DRIVE_AMP_PI_KI__MSK        0x30
#define BMG160_TRIM_DRIVE_AMP_PI_KI__REG        BMG160_TRIM_DRIVE_AMP_PI

#define BMG160_TRIM_DRIVE_AMP_REF__POS          0                       /**< First 4 bits of Trim Drive Amp PI Registers */
#define BMG160_TRIM_DRIVE_AMP_REF__LEN          4
#define BMG160_TRIM_DRIVE_AMP_REF__MSK          0x0F
#define BMG160_TRIM_DRIVE_AMP_REF__REG          BMG160_TRIM_DRIVE_AMP_PI
/* EasyCASE ) */
/* EasyCASE ( 2586
   BMG160_NVM_REMAIN_CNT */
#define BMG160_NVM_REMAIN_COUNT__POS        4                       /**< Last 4 bits of NVM Remaining Count Registers */
#define BMG160_NVM_REMAIN_COUNT__LEN        4
#define BMG160_NVM_REMAIN_COUNT__MSK        0xF0
#define BMG160_NVM_REMAIN_COUNT__REG        BMG160_NVM_REMAIN_CNT
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2543
   Common */
/* For Axis Selection   */
#define BMG160_X_AXIS           0                            /**< It refers BMG160 X-axis */
#define BMG160_Y_AXIS           1                            /**< It refers BMG160 Y-axis */
#define BMG160_Z_AXIS           2                            /**< It refers BMG160 Z-axis */

/* For Mode Settings    */
#define BMG160_MODE_NORMAL             0
#define BMG160_MODE_LOWPOWERNORMAL     1
#define BMG160_MODE_SUSPENDNORMAL      2
#define BMG160_MODE_DEEPSUSPEND        3



/* get bit slice  */
#define BMG160_GET_BITSLICE(regvar, bitname)\
    (regvar & bitname##__MSK) >> bitname##__POS

/* Set bit slice */
#define BMG160_SET_BITSLICE(regvar, bitname, val)\
    (regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK)
/* Constants */

#define BMG160_NULL                             0               /**< constant declaration of NULL */
#define BMG160_DISABLE                          0               /**< It refers BMG160 disable */
#define BMG160_ENABLE                           1               /**< It refers BMG160 enable */
#define BMG160_OFF                              0               /**< It refers BMG160 OFF state */
#define BMG160_ON                               1               /**< It refers BMG160 ON state  */


#define BMG160_TURN1                            0               /**< It refers BMG160 TURN1 */
#define BMG160_TURN2                            1               /**< It refers BMG160 TURN2 */

#define BMG160_INT1                             0               /**< It refers BMG160 INT1 */
#define BMG160_INT2                             1               /**< It refers BMG160 INT2 */

#define INT_PIN_PUSH_PULL     0
#define INT_PIN_OPEN_DRAIN    1
#define INT_DISABLE           0
#define INT_ENABLE            1




#define BMG160_SLOW_OFFSET                      0               /**< It refers BMG160 Slow Offset */
#define BMG160_AUTO_OFFSET                      1               /**< It refers BMG160 Auto Offset */
#define BMG160_FAST_OFFSET                      2               /**< It refers BMG160 Fast Offset */
#define BMG160_S_TAP                            0               /**< It refers BMG160 Single Tap */
#define BMG160_D_TAP                            1               /**< It refers BMG160 Double Tap */
#define BMG160_INT1_DATA                        0               /**< It refers BMG160 Int1 Data */
#define BMG160_INT2_DATA                        1               /**< It refers BMG160 Int2 Data */
#define BMG160_TAP_UNFIT_DATA                   0               /**< It refers BMG160 Tap Unfit data */
#define BMG160_HIGH_UNFIT_DATA                  1               /**< It refers BMG160 High Unfit data */
#define BMG160_CONST_UNFIT_DATA                 2               /**< It refers BMG160 Const Unfit data */
#define BMG160_ANY_UNFIT_DATA                   3               /**< It refers BMG160 Any Unfit data */
#define BMG160_SHAKE_UNFIT_DATA                 4               /**< It refers BMG160 Shake Unfit data */
#define BMG160_SHAKE_TH                         0               /**< It refers BMG160 Shake Threshold */
#define BMG160_SHAKE_TH2                        1               /**< It refers BMG160 Shake Threshold2 */
#define BMG160_AUTO_OFFSET_WL                   0               /**< It refers BMG160 Auto Offset word length */
#define BMG160_FAST_OFFSET_WL                   1               /**< It refers BMG160 Fast Offset word length */
#define BMG160_I2C_WDT_EN                       0               /**< It refers BMG160 I2C WDT En */
#define BMG160_I2C_WDT_SEL                      1               /**< It refers BMG160 I2C WDT Sel */
#define BMG160_EXT_MODE                         0               /**< It refers BMG160 Ext Mode */
#define BMG160_EXT_PAGE                         1               /**< It refers BMG160 Ext page */
#define BMG160_START_ADDR                       0               /**< It refers BMG160 Start Address */
#define BMG160_STOP_ADDR                        1               /**< It refers BMG160 Stop Address */
#define BMG160_SLOW_CMD                         0               /**< It refers BMG160 Slow Command */
#define BMG160_FAST_CMD                         1               /**< It refers BMG160 Fast Command */
#define BMG160_TRIM_VRA                         0               /**< It refers BMG160 Trim VRA */
#define BMG160_TRIM_VRD                         1               /**< It refers BMG160 Trim VRD */
#define BMG160_LOGBIT_EM                        0               /**< It refers BMG160 LogBit Em */
#define BMG160_LOGBIT_VM                        1               /**< It refers BMG160 LogBit VM */
#define BMG160_GP0                              0               /**< It refers BMG160 GP0 */
#define BMG160_GP1                              1               /**< It refers BMG160 GP1*/
#define BMG160_LOW_SPEED                        0               /**< It refers BMG160 Low Speed Oscilator */
#define BMG160_HIGH_SPEED                       1               /**< It refers BMG160 High Speed Oscilator */
#define BMG160_DRIVE_OFFSET_P                   0               /**< It refers BMG160 Drive Offset P */
#define BMG160_DRIVE_OFFSET_N                   1               /**< It refers BMG160 Drive Offset N */
#define BMG160_TEST_MODE_EN                     0               /**< It refers BMG160 Test Mode Enable */
#define BMG160_TEST_MODE_REG                    1               /**< It refers BMG160 Test Mode reg */
#define BMG160_IBIAS_DRIVE_TRIM                 0               /**< It refers BMG160 IBIAS Drive Trim */
#define BMG160_IBIAS_RATE_TRIM                  1               /**< It refers BMG160 IBIAS Rate Trim */
#define BMG160_BAA_MODE                         0               /**< It refers BMG160 BAA Mode Trim */
#define BMG160_BMA_MODE                         1               /**< It refers BMG160 BMA Mode Trim */
#define BMG160_PI_KP                            0               /**< It refers BMG160 PI KP */
#define BMG160_PI_KI                            1               /**< It refers BMG160 PI KI */

#define BMG160_SPI_RD_MASK                      0x80            /**< Read mask **/
#define BMG160_READ_SET                         0x01            /**< Setting for rading data **/

#define BMG160_SHIFT_1_POSITION                 1               /**< Shift bit by 1 Position **/
#define BMG160_SHIFT_2_POSITION                 2               /**< Shift bit by 2 Position **/
#define BMG160_SHIFT_3_POSITION                 3               /**< Shift bit by 3 Position **/
#define BMG160_SHIFT_4_POSITION                 4               /**< Shift bit by 4 Position **/
#define BMG160_SHIFT_5_POSITION                 5               /**< Shift bit by 5 Position **/
#define BMG160_SHIFT_6_POSITION                 6               /**< Shift bit by 6 Position **/
#define BMG160_SHIFT_7_POSITION                 7               /**< Shift bit by 7 Position **/
#define BMG160_SHIFT_8_POSITION                 8               /**< Shift bit by 8 Position **/
#define BMG160_SHIFT_12_POSITION                12              /**< Shift bit by 12 Position **/

#define         C_BMG160_Null_U8X                              0
#define         C_BMG160_Zero_U8X                              0
#define         C_BMG160_One_U8X                               1
#define         C_BMG160_Two_U8X                               2
#define         C_BMG160_Three_U8X                             3
#define         C_BMG160_Four_U8X                              4
#define         C_BMG160_Five_U8X                              5
#define         C_BMG160_Six_U8X                               6
#define         C_BMG160_Seven_U8X                             7
#define         C_BMG160_Eight_U8X                             8
#define         C_BMG160_Nine_U8X                              9
#define         C_BMG160_Ten_U8X                               10
#define         C_BMG160_Eleven_U8X                            11
#define         C_BMG160_Twelve_U8X                            12
#define         C_BMG160_Thirteen_U8X                          13
#define         C_BMG160_Fifteen_U8X                           15
#define         C_BMG160_Sixteen_U8X                           16
#define         C_BMG160_TwentyTwo_U8X                         22
#define         C_BMG160_TwentyThree_U8X                       23
#define         C_BMG160_TwentyFour_U8X                        24
#define         C_BMG160_TwentyFive_U8X                        25
#define         C_BMG160_ThirtyTwo_U8X                         32
#define         C_BMG160_Hundred_U8X                           100
#define         C_BMG160_OneTwentySeven_U8X                    127
#define         C_BMG160_OneTwentyEight_U8X                    128
#define         C_BMG160_TwoFiftyFive_U8X                      255
#define         C_BMG160_TwoFiftySix_U16X                      256

#define E_BMG160_NULL_PTR               (char)-127
#define E_BMG160_COMM_RES               (char)-1
#define E_BMG160_OUT_OF_RANGE           (char)-2

#define BMG160_WR_FUNC_PTR sns_ddf_status_e (* bus_write)(unsigned char, unsigned char, unsigned char *, unsigned char)
#define BMG160_RD_FUNC_PTR sns_ddf_status_e (* bus_read)(unsigned char, unsigned char, unsigned char *, unsigned char)
#define BMG160_BRD_FUNC_PTR sns_ddf_status_e (* burst_read)(unsigned char, unsigned char, unsigned char *, BMG160_S32)
#define BMG160_MDELAY_DATA_TYPE uint32_t

#define BMG160_RESOLUTION                        1 /**< To set the Resolution of BMG160 as 16 bit(1) **/
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2176
   ENUM and struct Definitions */
/*user defined Structures*/
/* EasyCASE - */
/* EasyCASE < */


#define SENSOR_NAME "bmg160"

#define DD_ATTR_LOCAL static


#define BMG160_DEV_SEL_NUM_BMI055 0x55
#define BMG160_DEV_SEL_NUM_BMI058 0x58

#define BMG160_CONFIG_SKIP_SAMPLE_CNT_ODRC      17
#define BMG160_CONFIG_FASTEST_ODR 250
#define BMG160_CONFIG_MAX_FREQ    BMG160_CONFIG_FASTEST_ODR

#define BMG160_CONFIG_TOLERANCE_DRYD_TIME_ACCURACY (0.2)


#ifndef BMG160_CONFIG_ENABLE_TS_ADJUST_FILTER
#define BMG160_CONFIG_ENABLE_TS_ADJUST_FILTER          0
#endif

#ifndef BMG160_CONFIG_ENABLE_WA_EINVALID_DATA
#define BMG160_CONFIG_ENABLE_WA_EINVALID_DATA          1
#endif


#ifndef BMG160_CONFIG_WMI_TRIGGER_LEVEL
#define BMG160_CONFIG_WMI_TRIGGER_LEVEL  0
#endif

#define BMG160_CONFIG_DRI_MOCKUP_WMI     1

// Enable BMG160_CONFIG_BW_SEL_OLD_WAY to enable
// down-sampling in the driver
#ifndef BMG160_CONFIG_BW_SEL_OLD_WAY
#define BMG160_CONFIG_BW_SEL_OLD_WAY     0
#endif

#define BMG160_CONFIG_FIFO_LOG           1

#define BMG160_FIFO_HEAD_ROOM            10

#define BMG160_IRQ_TRIGGER_TYPE_DEFAULT  SNS_DDF_SIGNAL_IRQ_RISING


#define SENSOR_CHIP_ID_BMG (0x0f)

#define BMG_REG_NAME(name) BMG160_##name
#define BMG_VAL_NAME(name) BMG160_##name
#define BMG_CALL_API(name) bmg160_##name

#define BMG160_ABS(x) (((x) >= 0) ? (x) : (-x))

#define BST_ARRARY_SIZE(array) (sizeof(array)/sizeof(array[0]))

typedef enum {
    SDD_GYRO_X = 0,
    SDD_GYRO_Y,
    SDD_GYRO_Z,
    SDD_GYRO_NUM_AXES,
    SDD_GYRO_NUM_DATATYPES
} sdd_gyro_data_type_e;

struct bmg160_data
{
    BMG160_S16 datax;
    BMG160_S16 datay;
    BMG160_S16 dataz;
    char intstatus[5];
};

struct bmg160_offset
{
    BMG160_U16 datax;
    BMG160_U16 datay;
    BMG160_U16 dataz;
};

struct bmg160
{
    unsigned char chip_id;
    unsigned char revision_id;
    unsigned char dev_addr;
    BMG160_BRD_FUNC_PTR;
    BMG160_WR_FUNC_PTR;
    BMG160_RD_FUNC_PTR;
    void(*delay_msec)( BMG160_MDELAY_DATA_TYPE );
};


/* State struct for BMG16- driver */
typedef struct {
    sns_ddf_handle_t smgr_handle;    /* SDDI handle used to notify_data */
    q16_t            bias[SDD_GYRO_NUM_AXES];    /* Accelerometer bias */
    q16_t            data_cache[SDD_GYRO_NUM_DATATYPES]; /* data cache */
    sns_ddf_handle_t port_handle; /* handle used to access the I2C bus */
    struct bmg160 device;
    uint8_t          range;
    uint16_t         lowpass_bw;
    /*! GPIO number used for interrupts */
    uint32_t gpio_num;

    uint32_t dev_select;

    uint32_t ticks_per_ms;
    /* odr_reported reported to SMGR */
    uint32_t odr_reported;
    /* odr value actually used by HW */

    int8_t                  sample_cnt_to_skip_odrc;
    sns_ddf_time_t          itvl_drdy;
    sns_ddf_time_t          itvl_drdy_tolerance;

    uint32_t odr_hw;
    bool  dri_enabled;
    bool                    bw_timer_pending;
    sns_ddf_timer_s         bw_timer;

    uint8_t                 bw_decided;
    /* the ODR used for the whole device */
    uint32_t                odr_shared;

    /*! time of last update */
    sns_ddf_time_t          data_cache_time;
    sns_ddf_status_e        data_status[SDD_GYRO_NUM_DATATYPES];

    /*! used when notifying the data to SMGR */
    sns_ddf_sensor_data_s   sensor_data;

    /*! FIFO */
    /*! cache used when notifying the data to SMGR */
    sns_ddf_sensor_data_s   f_frames_cache;

    uint8_t                 regv_int_map_1;

#if BMG160_CONFIG_ENABLE_TS_ADJUST_FILTER
        //use uint16_t to save a little bit of memory
    uint16_t				ts_adjust_gyr;
#endif



    /* water mark level */
    uint8_t                 f_wml_req;
    uint8_t                 f_wml;
    sns_ddf_time_t          f_avg_sample_interval;
    sns_ddf_time_t          ts_start_first_sample;
    sns_ddf_time_t          ts_irq;
    uint32_t                en_dri          :1;
    uint32_t                en_fifo_int     :1;
    uint32_t                en_md           :1;
    uint32_t                en_db_tap_int   :1;
    uint32_t                test_init:1;

    uint32_t                num_irq_drdy;
    sns_ddf_time_t          ts_last_drdy;

    sns_ddf_axes_map_s      axes_map;
    q16_t                   data_cache_fifo[SDD_GYRO_NUM_DATATYPES];
} sns_dd_bmg160_state_t;


extern sns_ddf_status_e sns_dd_bmg160_init(
        sns_ddf_handle_t*        dd_handle_ptr,
        sns_ddf_handle_t         smgr_handle,
        sns_ddf_nv_params_s*     nv_params,
        sns_ddf_device_access_s  device_info[],
        uint32_t                 num_devices,
        sns_ddf_memhandler_s*    memhandler,
        sns_ddf_sensor_e*        sensors[],
        uint32_t*                num_sensors);

extern sns_ddf_status_e sns_dd_bmg160_get_data(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s*   memhandler,
        sns_ddf_sensor_data_s*  data[]);

extern sns_ddf_status_e sns_dd_bmg160_set_attr(
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        sns_ddf_attribute_e  attrib,
        void*                value);

extern sns_ddf_status_e sns_dd_bmg160_get_attr(
        sns_ddf_handle_t       dd_handle,
        sns_ddf_sensor_e       sensor,
        sns_ddf_attribute_e    attrib,
        sns_ddf_memhandler_s*  memhandler,
        void**                 value,
        uint32_t*              num_elems);

extern sns_ddf_status_e sns_dd_bmg160_reset(sns_ddf_handle_t dd_handle);

extern sns_ddf_status_e sns_dd_bmg160_enable_sched_data(
        sns_ddf_handle_t dd_handle,
        sns_ddf_sensor_e sensor,
        bool enable);

extern sns_ddf_status_e sns_dd_gyro_bmg160_self_test(
        sns_ddf_handle_t dd_handle,
        sns_ddf_sensor_e sensor,
        sns_ddf_test_e test,
        uint32_t* err);

extern void sns_dd_bmg160_interrupt_handler(
        sns_ddf_handle_t        dd_handle,
        uint32_t                gpio_num,
        sns_ddf_time_t          timestamp);

extern sns_ddf_status_e sns_dd_bmg160_triger_fifo_data(
        sns_ddf_handle_t    dd_handle,
        sns_ddf_sensor_e    sensor,
        uint16_t            num_samples,
        bool                trigger_now);

extern sns_ddf_status_e sns_dd_bmg160_probe(
        sns_ddf_device_access_s*  device_info,
        sns_ddf_memhandler_s*     memhandler,
        uint32_t*                 num_sensors,
        sns_ddf_sensor_e**        sensors);


extern
void sns_dd_gyro_log_fifo(
        sns_ddf_sensor_data_s *data_collection,
        uint8_t context);

extern
sns_ddf_status_e bmg160_fifo_data_read_out_frames(
        sns_dd_bmg160_state_t *state,
        uint8_t f_count,
        int in_irq);


/* EasyCASE > */
/* EasyCASE ) */
/* EasyCASE ( 2177
   Public API Declarations */
/* EasyCASE ( 2178
   Data Register */
/* EasyCASE ( 2179
   bmg160_init */


/*******************************************************************************
 * Description: *//**\brief API Initialization routine
 *
 *
 *
 *
 *  \param *p_bmg160 pointer to bmg160_t structured type
 *
 *
 *
 *
 *
 *  \return result of communication routines
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_init(struct bmg160 *p_bmg160);
/* EasyCASE ) */
/* EasyCASE ( 2180
   bmg160_get_dataX */
/*******************************************************************************
 * Description: *//**\brief Reads sensor Data X from 02h and 03h
 *
 *
 *
 *
 *  \param
 *       short  *data_x : Address of data_x
 *
 *
 *
 *
 *  \return Result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_dataX(BMG160_S16 *data_x);
/* EasyCASE ) */
/* EasyCASE ( 2181
   bmg160_get_datay */
/*******************************************************************************
 * Description: *//**\brief Reads sensor Data Y from location 04h and 05h
 *
 *
 *
 *
 *  \param
 *             short  *data_y : Address of data_y
 *
 *
 *  \return Result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_dataY(BMG160_S16 *data_y);
/* EasyCASE ) */
/* EasyCASE ( 2182
   bmg160_get_dataZ */
/*******************************************************************************
 * Description: *//**\brief Reads sensor Data Z from location 06h and 07h
 *
 *
 *
 *
 *  \param
 *              short  *data_z : Address of data_z
 *
 *
 *  \return Result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_dataZ(BMG160_S16 *data_z);
/* EasyCASE ) */
/* EasyCASE ( 2183
   bmg160_get_dataXYZ */
/*******************************************************************************
 * Description: *//**\brief
 *  1.  Reads sensor Data X
 *  2.  Reads sensor Data Y
 *  3.  Reads sensor Data Z
 *
 *  \param      bmg160_data_t *data  :  data pointer to bmg160_data_t
 *
 *
 *
 *  \return result of communication routines
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_dataXYZ(struct bmg160_data *data);
/* EasyCASE ) */
/* EasyCASE ( 3062
   bmg160_get_dataXYZI */
/*******************************************************************************
 * Description: *//**\brief
 *  1.  Reads sensor Data X
 *  2.  Reads sensor Data Y
 *  3.  Reads sensor Data Z
 *
 *  \param      bmg160_data_t *data  :  data pointer to bmg160_data_t
 *
 *
 *
 *  \return result of communication routines
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_dataXYZI(struct bmg160_data *data);
/* EasyCASE ) */
/* EasyCASE ( 2184
   bmg160_get_Temperature */
/*******************************************************************************
 * Description: *//**\brief Reads sensor Temperature data from location 08h
 *
 *
 *
 *
 *  \param
 *        unsigned char *temperature : Address of temperature
 *
 *
 *  \return Result of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_Temperature(unsigned char *temperature);
/* EasyCASE ) */
/* EasyCASE ( 2185
   New Data X */
/*******************************************************************************
 * Description: *//**\brief Reads new data X bit from location 02h
 *
 *
 *
 *
 *  \param
 *      unsigned char *new_datax : Address of new_datax
 *
 *
 *
 *
 *  \return result of communication routines
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_new_data_X (unsigned char *new_datax);
/* EasyCASE ) */
/* EasyCASE ( 2186
   New Data Y */
/*******************************************************************************
 * Description: *//**\brief Reads new data Y bit from location 04h
 *
 *
 *
 *
 *  \param
 *      unsigned char *new_datay : Address of new_datay
 *
 *
 *
 *
 *  \return result of communication routines
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_new_data_Y (unsigned char *new_datay);
/* EasyCASE ) */
/* EasyCASE ( 2187
   New Data Z */
/*******************************************************************************
 * Description: *//**\brief Reads new data Z bit from location 06h
 *
 *
 *
 *
 *  \param
 *      unsigned char *new_dataz : Address of new_dataz
 *
 *
 *
 *
 *  \return result of communication routines
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_new_data_Z (unsigned char *new_dataz);
/* EasyCASE ) */
/* EasyCASE ( 2188
   New Data XYZ */
/*******************************************************************************
 * Description: *//**\brief Reads New data X,Y and Z bits from location 02h, 04h and 06h
 *
 *
 *
 *
 *  \param
 *      unsigned char *new_dataxyz : Address of self test bits
 *
 *
 *
 *
 *  \return result of communication routines
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_new_data_XYZ (unsigned char *new_dataxyz);
/* EasyCASE ) */
/* EasyCASE ( 2770
   FIFO Data */
/*******************************************************************************
 * Description: *//**\brief Reads FIFI data from location 3Fh
 *
 *
 *
 *
 *  \param
 *      unsigned char *FIFO_data : Address of FIFO data bits
 *
 *
 *
 *
 *  \return result of communication routines
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_FIFO_data_reg (unsigned char *FIFO_data);
/* EasyCASE ) */
/* EasyCASE ( 2190
   bmg160_read_register */
/*******************************************************************************
 * Description: *//**\brief This API reads the data from the given register
 *
 *
 *
 *
 *  \param unsigned char addr, unsigned char *data
 *                       addr -> Address of the register
 *                       data -> address of the variable, read value will be kept
 *  \return  results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_read_register(unsigned char addr, unsigned char *data, unsigned char len);
/* EasyCASE ) */
/* EasyCASE ( 3021
   bmg160_burst_read */
/*******************************************************************************
 * Description: *//**\brief This API reads the data from the given register
 *
 *
 *
 *
 *  \param unsigned char addr, unsigned char *data
 *                       addr -> Address of the register
 *                       data -> address of the variable, read value will be kept
 *  \return  results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_burst_read(unsigned char addr, unsigned char *data, BMG160_S32 len);
/* EasyCASE ) */
/* EasyCASE ( 2191
   bmg160_write_register */
/*******************************************************************************
 * Description: *//**\brief This API given data to the given register
 *
 *
 *
 *
 *  \param unsigned char addr, unsigned char data
 *                   addr -> Address of the register
 *                   data -> Data to be written to the register
 *
 *  \return Results of bus communication function
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_write_register(unsigned char addr, unsigned char *data, unsigned char len);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2192
   Status Register */
/* EasyCASE ( 2193
   bmg160_get_interrupt_status_reg_0 */
/*******************************************************************************
 * Description: *//**\brief Reads interrupt status 0 register byte from 09h
 *
 *
 *
 *
 *  \param
 *      unsigned char *status0_data : Address of status 0 register
 *
 *
 *  \return
 *      Result of bus communication function
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/

BMG160_RETURN_FUNCTION_TYPE bmg160_get_interrupt_status_reg_0( unsigned char *status0_data);
/* EasyCASE ) */
/* EasyCASE ( 2194
   bmg160_get_interrupt_status_reg_1 */
/*******************************************************************************
 * Description: *//**\brief Reads interrupt status 1 register byte from 0Ah
 *
 *
 *
 *
 *  \param
 *      unsigned char *status1_data : Address of status register
 *
 *
 *  \return
 *      Result of bus communication function
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/

BMG160_RETURN_FUNCTION_TYPE bmg160_get_interrupt_status_reg_1( unsigned char *status1_data);
/* EasyCASE ) */
/* EasyCASE ( 2195
   bmg160_get_interrupt_status_reg_2 */
/*******************************************************************************
 * Description: *//**\brief Reads interrupt status register byte from 0Bh
 *
 *
 *
 *
 *  \param
 *      unsigned char *status2_data : Address of status 2 register
 *
 *
 *  \return
 *      Result of bus communication function
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/

BMG160_RETURN_FUNCTION_TYPE bmg160_get_interrupt_status_reg_2( unsigned char *status2_data);
/* EasyCASE ) */
/* EasyCASE ( 2196
   bmg160_get_interrupt_status_reg_3 */
/*******************************************************************************
 * Description: *//**\brief Reads interrupt status 3 register byte from 0Ch
 *
 *
 *
 *
 *  \param
 *      unsigned char *status3_data : Address of status 3 register
 *
 *
 *  \return
 *      Result of bus communication function
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/

BMG160_RETURN_FUNCTION_TYPE bmg160_get_interrupt_status_reg_3( unsigned char *status3_data);
/* EasyCASE ) */
/* EasyCASE ( 2608
   bmg160_get_fifostatus_reg */
/*******************************************************************************
 * Description: *//**\brief Reads interrupt fifo status register byte from 0Eh
 *
 *
 *
 *
 *  \param
 *      unsigned char *fifo_status : Address of Fifo status register
 *
 *
 *  \return
 *      Result of bus communication function
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/

BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifostatus_reg( uint8_t *fifo_status);
/* EasyCASE ) */
/* EasyCASE ( 3037
   bmg160_get_interrupt_status_reg */
/*******************************************************************************
 * Description: *//**\brief Reads interrupt all 5 status register byte from 09h
 *
 *
 *
 *
 *  \param
 *      unsigned char *status_data : Address of status register
 *
 *
 *  \return
 *      Result of bus communication function
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/

BMG160_RETURN_FUNCTION_TYPE bmg160_get_interrupt_status_reg( unsigned char *status_data);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2198
   Control Register */
/* EasyCASE ( 2199
   DSP_RANGE_ADDR */
/* EasyCASE ( 2859
   bmg160_get_dsp_range_reg */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_range_reg (unsigned char *range);
/* EasyCASE ) */
/* EasyCASE ( 2860
   bmg160_set_dsp_range_reg */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_range_reg (unsigned char range);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2202
   DSP_BW_ADDR */
/* EasyCASE ( 2203
   bmg160_get_high_res */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_high_res (unsigned char *high_res);
/* EasyCASE ) */
/* EasyCASE ( 2204
   bmg160_set_high_res */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_high_res (unsigned char high_res);
/* EasyCASE ) */
/* EasyCASE ( 3008
   bmg160_get_bw */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_bw (unsigned char *bandwidth);
/* EasyCASE ) */
/* EasyCASE ( 3009
   bmg160_set_bw */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_bw (unsigned char bandwidth);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2616
   PMU_LPW1_ADDR */
/* EasyCASE ( 2617
   bmg160_get_PMU_fast_powerup */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_pmu_fast_powerup (unsigned char *pwu_fast_powerup);
/* EasyCASE ) */
/* EasyCASE ( 2618
   bmg160_set_PMU_fast_powerup */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_pmu_fast_powerup (unsigned char pwu_fast_powerup);
/* EasyCASE ) */
/* EasyCASE ( 2621
   bmg160_get_PMU_ext_tri_sel */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_pmu_ext_tri_sel (unsigned char *pwu_ext_tri_sel);
/* EasyCASE ) */
/* EasyCASE ( 2622
   bmg160_set_PMU_ext_tri_sel */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_pmu_ext_tri_sel (unsigned char pwu_ext_tri_sel);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2214
   DSP_HBW_ADDR */
/* EasyCASE ( 2215
   bmg160_get_data_high_bw */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_high_bw (unsigned char *high_bw);
/* EasyCASE ) */
/* EasyCASE ( 2216
   bmg160_set_data_high_bw */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_high_bw (unsigned char high_bw);
/* EasyCASE ) */
/* EasyCASE ( 2217
   bmg160_get_shadow_dis */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_shadow_dis (unsigned char *shadow_dis);
/* EasyCASE ) */
/* EasyCASE ( 2218
   bmg160_set_shadow_dis */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_shadow_dis (unsigned char shadow_dis);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2219
   Soft_Reset */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_soft_reset (void);
/* EasyCASE ) */
/* EasyCASE ( 2220
   INT_ENABLE0 */
/* EasyCASE ( 2994
   bmg160_get_data_enable */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_data_enable (unsigned char *data_en);
/* EasyCASE ) */
/* EasyCASE ( 2995
   bmg160_set_data_enable */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_data_enable (unsigned char data_en);
/* EasyCASE ) */
/* EasyCASE ( 2996
   bmg160_get_fifo_enable */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_enable (unsigned char *fifo_en);
/* EasyCASE ) */
/* EasyCASE ( 2997
   bmg160_set_fifo_enable */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_fifo_enable (unsigned char fifo_en);
/* EasyCASE ) */
/* EasyCASE ( 2998
   bmg160_get_offset_enable */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_offset_enable (unsigned char mode,unsigned char *offset_en);
/* EasyCASE ) */
/* EasyCASE ( 2999
   bmg160_set_offset_enable */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_offset_enable (unsigned char mode,unsigned char offset_en);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2233
   INT_ENABLE1 */
/* EasyCASE ( 2656
   bmg160_get_int_od */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_int_od (unsigned char param,unsigned char *int_od);
/* EasyCASE ) */
/* EasyCASE ( 2657
   bmg160_set_int_od */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_int_od (unsigned char param,unsigned char int_od);
/* EasyCASE ) */
/* EasyCASE ( 2651
   bmg160_get_int_lvl */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_int_lvl (unsigned char param,unsigned char *int_lvl);
/* EasyCASE ) */
/* EasyCASE ( 2652
   bmg160_set_int_lvl */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_int_lvl (unsigned char param,unsigned char int_lvl);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2238
   INT_MAP_0 */
/* EasyCASE ( 2243
   bmg160_get_int1_high */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_int1_high (unsigned char *int1_high);
/* EasyCASE ) */
/* EasyCASE ( 2244
   bmg160_set_int1_high */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_int1_high (unsigned char int1_high);
/* EasyCASE ) */
/* EasyCASE ( 2247
   bmg160_get_int1_any */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_int1_any (unsigned char *int1_any);
/* EasyCASE ) */
/* EasyCASE ( 2248
   bmg160_set_int1_any */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_int1_any (unsigned char int1_any);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2251
   INT_MAP_1 */
/* EasyCASE ( 2252
   bmg160_get_int_data */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_int_data (unsigned char axis,unsigned char *int_data);
/* EasyCASE ) */
/* EasyCASE ( 2253
   bmg160_set_int_data */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_int_data (unsigned char axis,unsigned char int_data);
/* EasyCASE ) */
/* EasyCASE ( 2254
   bmg160_get_int2_offset */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_int2_offset (unsigned char axis,unsigned char *int2_offset);
/* EasyCASE ) */
/* EasyCASE ( 2255
   bmg160_set_int2_offset */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_int2_offset (unsigned char axis,unsigned char int2_offset);
/* EasyCASE ) */
/* EasyCASE ( 2256
   bmg160_get_int1_offset */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_int1_offset (unsigned char axis,unsigned char *int1_offset);
/* EasyCASE ) */
/* EasyCASE ( 2257
   bmg160_set_int1_offset */
/*******************************************************************************
 * Description: *//**\brief
 *

                   *
                   *
                   *
                   *  \param
                   *
                   *
                   *
                   *  \return
                   *
                   *
                   ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_int1_offset (unsigned char axis,unsigned char int1_offset);
/* EasyCASE ) */
/* EasyCASE ( 2667
   bmg160_get_int_fifo */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_int_fifo (unsigned char *int_fifo);
/* EasyCASE ) */
/* EasyCASE ( 2668
   bmg160_set_int_fifo */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_int_fifo (unsigned char axis,unsigned char int_fifo);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2258
   INT_MAP_2 */
/* EasyCASE ( 2263
   bmg160_get_int2_high */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_int2_high (unsigned char *int2_high);
/* EasyCASE ) */
/* EasyCASE ( 2264
   bmg160_set_int2_high */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_int2_high (unsigned char int2_high);
/* EasyCASE ) */
/* EasyCASE ( 2267
   bmg160_get_int2_any */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_int2_any (unsigned char *int2_any);
/* EasyCASE ) */
/* EasyCASE ( 2268
   bmg160_set_int2_any */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_int2_any (unsigned char int2_any);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2271
   INT_0_ADDR */
/* EasyCASE ( 2274
   bmg160_get_offset_unfit */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_offset_unfit (unsigned char param,unsigned char *offset_unfit);
/* EasyCASE ) */
/* EasyCASE ( 2275
   bmg160_set_offset_unfit */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_offset_unfit (unsigned char param,unsigned char offset_unfit);
/* EasyCASE ) */
/* EasyCASE ( 2276
   bmg160_get_unfit_data */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_unfit_data (unsigned char param,unsigned char *unfit_data);
/* EasyCASE ) */
/* EasyCASE ( 2277
   bmg160_set_unfit_data */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_unfit_data (unsigned char param,unsigned char unfit_data);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2278
   INT_1_ADDR */
/* EasyCASE ( 2279
   bmg160_get_any_th */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_any_th (unsigned char *any_th);
/* EasyCASE ) */
/* EasyCASE ( 2280
   bmg160_set_any_th */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_any_th (unsigned char any_th);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2281
   INT_2_ADDR */
/* EasyCASE ( 2282
   bmg160_get_awake_dur */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_awake_dur (unsigned char *awake_dur);
/* EasyCASE ) */
/* EasyCASE ( 2283
   bmg160_set_awake_dur */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_awake_dur (unsigned char awake_dur);
/* EasyCASE ) */
/* EasyCASE ( 2284
   bmg160_get_any_dursample */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_any_dursample (unsigned char *dursample);
/* EasyCASE ) */
/* EasyCASE ( 2285
   bmg160_set_any_dursample */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_any_dursample (unsigned char dursample);
/* EasyCASE ) */
/* EasyCASE ( 2286
   bmg160_get_any_en_ch */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_any_en_ch (unsigned char channel,unsigned char *data);
/* EasyCASE ) */
/* EasyCASE ( 2287
   bmg160_set_any_en_ch */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_any_en_ch (unsigned char channel,unsigned char data);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2295
   INT_4_ADDR */
/* EasyCASE ( 2296
   bmg160_get_fifo_watermark_enable */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_watermark_enable (unsigned char *fifo_wn_en);
#if 0
/* EasyCASE ) */
/* EasyCASE ( 2297
   bmg160_set_fifo_watermark_enable */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_fifo_watermark_enable (unsigned char fifo_wn_en);
#endif
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2306
   BMG160_RST_LATCH_ADDR */
/* EasyCASE ( 2307
   bmg160_set_reset_int */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_reset_int (unsigned char reset_int);
/* EasyCASE ) */
/* EasyCASE ( 2308
   bmg160_set_offset_reset */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_offset_reset (unsigned char offset_reset);
/* EasyCASE ) */
/* EasyCASE ( 2677
   bmg160_get_latch_status */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_latch_status (unsigned char *latch_status);
/* EasyCASE ) */
/* EasyCASE ( 2678
   bmg160_set_latch_status */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_latch_status (unsigned char latch_status);
/* EasyCASE ) */
/* EasyCASE ( 2309
   bmg160_get_latch_int */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_latch_int (unsigned char *latch_int);
/* EasyCASE ) */
/* EasyCASE ( 2310
   bmg160_set_latch_int */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_latch_int (unsigned char latch_int);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2311
   INT_7_ADDR */
/* EasyCASE ( 2312
   bmg160_get_high_hy */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_high_hy (unsigned char channel,unsigned char *high_hy);
/* EasyCASE ) */
/* EasyCASE ( 2313
   bmg160_set_high_hy */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_high_hy (unsigned char channel,unsigned char high_hy);
/* EasyCASE ) */
/* EasyCASE ( 2314
   bmg160_get_high_th */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_high_th (unsigned char channel,unsigned char *high_th);
/* EasyCASE ) */
/* EasyCASE ( 2315
   bmg160_set_high_th */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_high_th (unsigned char channel,unsigned char high_th);
/* EasyCASE ) */
/* EasyCASE ( 2316
   bmg160_get_high_en_ch */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_high_en_ch (unsigned char channel,unsigned char *high_en);
/* EasyCASE ) */
/* EasyCASE ( 2317
   bmg160_set_high_en_ch */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_high_en_ch (unsigned char channel,unsigned char high_en);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2318
   INT_8_ADDR */
/* EasyCASE ( 2319
   bmg160_get_high_dur */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_high_dur_ch(unsigned char channel,unsigned char *high_dur);
/* EasyCASE ) */
/* EasyCASE ( 2320
   bmg160_set_high_dur */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_high_dur_ch (unsigned char channel,unsigned char high_dur);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2366
   BMG160_INT_OFF0_ADDR */
/* EasyCASE ( 2367
   bmg160_get_slow_offset_th */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_slow_offset_th (unsigned char *offset_th);
/* EasyCASE ) */
/* EasyCASE ( 2368
   bmg160_set_slow_offset_th */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_slow_offset_th (unsigned char offset_th);
/* EasyCASE ) */
/* EasyCASE ( 2369
   bmg160_get_slow_offset_dur */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_slow_offset_dur (unsigned char *offset_dur);
/* EasyCASE ) */
/* EasyCASE ( 2370
   bmg160_set_slow_offset_dur */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_slow_offset_dur (unsigned char offset_dur);
/* EasyCASE ) */
/* EasyCASE ( 2371
   bmg160_get_slow_offset_en */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_slow_offset_en_ch (unsigned char channel,unsigned char *slow_offset);
/* EasyCASE ) */
/* EasyCASE ( 2372
   bmg160_set_slow_offset_en */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_slow_offset_en_ch (unsigned char channel,unsigned char slow_offset);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2373
   BMG160_INT_OFF1_ADDR */
/* EasyCASE ( 2374
   bmg160_get_offset_wl */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_offset_wl (unsigned char channel,unsigned char *offset_wl);
/* EasyCASE ) */
/* EasyCASE ( 2375
   bmg160_set_offset_wl */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_offset_wl (unsigned char channel,unsigned char offset_wl);
/* EasyCASE ) */
/* EasyCASE ( 2376
   bmg160_set_fast_offset_en */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_fast_offset_en (unsigned char fast_offset);
/* EasyCASE ) */
/* EasyCASE ( 2377
   bmg160_get_fast_offset_en_ch */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_fast_offset_en_ch (unsigned char *fast_offset);
/* EasyCASE ) */
/* EasyCASE ( 2378
   bmg160_set_fast_offset_en_ch */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_fast_offset_en_ch (unsigned char channel,unsigned char fast_offset);
/* EasyCASE ) */
/* EasyCASE ( 3013
   bmg160_enable_fast_offset */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_enable_fast_offset (void);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2379
   BMG160_TRIM_NVM_CTRL_ADDR */
/* EasyCASE ( 2380
   bmg160_get_nvm_remain */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_nvm_remain (unsigned char *nvm_remain);
/* EasyCASE ) */
/* EasyCASE ( 2381
   bmg160_set_nvm_load */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_nvm_load (unsigned char nvm_load);
/* EasyCASE ) */
/* EasyCASE ( 2382
   bmg160_get_nvm_rdy */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_nvm_rdy (unsigned char *nvm_rdy);
/* EasyCASE ) */
/* EasyCASE ( 2383
   bmg160_set_nvm_prog_trig */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_nvm_prog_trig (unsigned char prog_trig);
/* EasyCASE ) */
/* EasyCASE ( 2384
   bmg160_get_nvm_prog_mode */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_nvm_prog_mode (unsigned char *prog_mode);
/* EasyCASE ) */
/* EasyCASE ( 2385
   bmg160_set_nvm_prog_mode */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_nvm_prog_mode (unsigned char prog_mode);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2386
   BMG160_BA_SPI3_WDT_ADDR */
/* EasyCASE ( 2387
   bmg160_get_i2c_wdt */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_i2c_wdt (unsigned char i2c_wdt,unsigned char *prog_mode);
/* EasyCASE ) */
/* EasyCASE ( 2388
   bmg160_set_i2c_wdt */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_i2c_wdt (unsigned char i2c_wdt,unsigned char prog_mode);
/* EasyCASE ) */
/* EasyCASE ( 2389
   bmg160_get_spi3 */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_spi3 (unsigned char *spi3);
/* EasyCASE ) */
/* EasyCASE ( 2390
   bmg160_set_spi3 */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_spi3 (unsigned char spi3);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2720
   BMG160_FIF0_CGF1_ADDR */
/* EasyCASE ( 2721
   bmg160_get_fifo_tag */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_tag (unsigned char *tag);
#if 0
/* EasyCASE ) */
/* EasyCASE ( 2722
   bmg160_set_fifo_tag */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_fifo_tag (unsigned char tag);
#endif
/* EasyCASE ) */
/* EasyCASE ( 2723
   bmg160_get_fifo_watermarklevel */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_watermarklevel (unsigned char *water_mark_level);
/* EasyCASE ) */
/* EasyCASE ( 2724
   bmg160_set_fifo_watermarklevel */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_fifo_watermarklevel (unsigned char water_mark_level);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2727
   BMG160_FIFO_CGF0_ADDR */
/* EasyCASE ( 2728
   bmg160_get_fifo_mode */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_mode (unsigned char *mode);
/* EasyCASE ) */
/* EasyCASE ( 2729
   bmg160_set_fifo_mode */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_fifo_mode (unsigned char mode);
/* EasyCASE ) */
/* EasyCASE ( 2738
   bmg160_get_fifo_data_sel */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_data_sel (unsigned char *data_sel);
/* EasyCASE ) */
/* EasyCASE ( 2739
   bmg160_set_fifo_data_sel */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_fifo_data_sel (unsigned char data_sel);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 2418
   Trim Register */
/* EasyCASE ( 2419
   bmg160_get_offset */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_offset(unsigned char axis,BMG160_S16 *offset);
/* EasyCASE ) */
/* EasyCASE ( 2420
   bmg160_set_offset */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_offset (unsigned char axis,BMG160_S16 offset);
/* EasyCASE ) */
/* EasyCASE ( 2421
   TRIM_GP0_ADDR */
/* EasyCASE ( 2422
   bmg160_get_gp */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_gp(unsigned char param,unsigned char *value);
/* EasyCASE ) */
/* EasyCASE ( 2423
   bmg160_set_gp */
/*******************************************************************************
 * Description: *//**\brief
 *
 *
 *
 *
 *  \param
 *
 *
 *
 *  \return
 *
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_gp(unsigned char param,unsigned char value);
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ) */
/* EasyCASE ( 3029
   FIFO Frame count */
/*******************************************************************************
 * Description: *//**\brief Reads interrupt all 5 status register byte from 09h
 *
 *
 *
 *
 *  \param
 *      unsigned char *fifo_framecount : Address of status register
 *
 *
 *  \return
 *      Result of bus communication function
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_framecount( unsigned char *fifo_framecount);
/* EasyCASE ) */
/* EasyCASE ( 3033
   FIFO Overrun */
/*******************************************************************************
 * Description: *//**\brief Reads interrupt all 5 status register byte from 09h
 *
 *
 *
 *
 *  \param
 *      unsigned char *fifo_overrun : Address of status register
 *
 *
 *  \return
 *      Result of bus communication function
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_overrun( unsigned char *fifo_overrun);
/* EasyCASE ) */
/* EasyCASE ( 3049
   bmg160_get_int2_fifo */
/*******************************************************************************
 * Description: *//**\brief Reads interrupt all 5 status register byte from 09h
 *
 *
 *
 *
 *  \param
 *      unsigned char *int_fifo : Address of status register
 *
 *
 *  \return
 *      Result of bus communication function
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_int2_fifo (unsigned char *int_fifo);
/* EasyCASE ) */
/* EasyCASE ( 3050
   bmg160_get_int1_fifo */
/*******************************************************************************
 * Description: *//**\brief Reads interrupt all 5 status register byte from 09h
 *
 *
 *
 *
 *  \param
 *      unsigned char *int_fifo : Address of status register
 *
 *
 *  \return
 *      Result of bus communication function
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_int1_fifo (unsigned char *int_fifo);
/* EasyCASE ) */
/* EasyCASE ( 3054
   bmg160_set_int2_fifo */
/*******************************************************************************
 * Description: *//**\brief Reads interrupt all 5 status register byte from 09h
 *
 *
 *
 *
 *  \param
 *
 *
 *  \return
 *      Result of bus communication function
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_int2_fifo (unsigned char fifo_int2);
/* EasyCASE ) */
/* EasyCASE ( 3058
   bmg160_set_int1_fifo */
/*******************************************************************************
 * Description: *//**\brief Reads interrupt all 5 status register byte from 09h
 *
 *
 *
 *
 *  \param
 *
 *
 *  \return
 *      Result of bus communication function
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_int1_fifo (unsigned char fifo_int1);
/* EasyCASE ) */
/* EasyCASE ( 3066
   bmg160_get_mode */
/*******************************************************************************
 * Description: *//**\brief Reads interrupt all 5 status register byte from 09h
 *
 *
 *
 *
 *  \param
 *
 *
 *  \return
 *      Result of bus communication function
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_get_mode(unsigned char *Mode );
/* EasyCASE ) */
/* EasyCASE ( 3068
   bmg160_set_mode */
/*******************************************************************************
 * Description: *//**\brief Reads interrupt all 5 status register byte from 09h
 *
 *
 *
 *
 *  \param
 *
 *
 *  \return
 *      Result of bus communication function
 *
 ******************************************************************************/
/* Scheduling:
 *
 *
 *
 * Usage guide:
 *
 *
 * Remarks:
 *
 ******************************************************************************/
BMG160_RETURN_FUNCTION_TYPE bmg160_set_mode(unsigned char Mode );
/* EasyCASE ) */
/* EasyCASE ) */
#endif
/* EasyCASE ) */

