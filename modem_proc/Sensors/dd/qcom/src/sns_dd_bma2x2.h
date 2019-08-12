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



#ifndef _SNSDACCELPRIV_H
#define _SNSDACCELPRIV_H

#ifndef BMA2X2_CONFIG_RUN_ON_OSSC
#define BMA2X2_CONFIG_RUN_ON_OSSC 0
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

#if BMA2X2_CONFIG_RUN_ON_OSSC || BMA2X2_CONFIG_ENABLE_COMPILE_CHECK

#define sns_accel_bma2x2_driver_fn_list sns_dd_vendor_if_1

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


//TODO: lucas, change the log level after the code works
#ifndef BMA2X2_CONFIG_ENABLE_UIMAGE
#define BMA2X2_CONFIG_ENABLE_UIMAGE             0
#endif

#ifndef BMA2X2_CONFIG_ENABLE_INT_MAP_2
#define BMA2X2_CONFIG_ENABLE_INT_MAP_2          0
#endif

#define BMA2X2_CONFIG_ENABLE_MD_LPM             1

#define BMA2X2_CONFIG_ENABLE_DRI_IMITATE_FF_WMI 1

#define BMA2X2_ENABLE_INT_DOUBLE_TAP

#define BMA2X2_CONFIG_SKIP_SAMPLE_CNT_ODRC      4

#define BMA2X2_CONFIG_TS_ITVL_EST_COEF          6
#define BMA2X2_CONFIG_TS_ERR_TOLERANCE_PERCENT  5
#define BMA2X2_CONFIG_TOLERANCE_DRYD_TIME_ACCURACY (0.2)

#if !BMA2X2_CONFIG_ENABLE_UIMAGE
#define sns_ddf_malloc_ex(ptr, size, handle)                            sns_ddf_malloc(ptr, size)
#define sns_ddf_memhandler_malloc_ex(mem_handler, size, handle)         sns_ddf_memhandler_malloc(mem_handler, size)
#define sns_ddf_mfree_ex(ptr, handle)                                   sns_ddf_mfree(ptr)
//if this is enabled, WML configurations happen only after sample_cnt_to_skip_odrc is zero
#define BMA2X2_CONFIG_ENABLE_DELAYED_WML_CFG    1
#endif


//TODO:FEATURES_CONFIG features config cluster should be done here

//#define BMA2X2_ENABLE_NVM_PARAM

#ifndef likely
#define likely(x) (x)
#endif

#ifndef BMA2X2_DD_CHECK_RETVAL
#define BMA2X2_DD_CHECK_RETVAL(val, val_success)\
    if (likely(val_success == val)) {       \
    } else {                                \
        val = val;                      \
        return val;                     \
    }
#endif

//resume this if you want to have debug messages in the log
#ifndef BMA2X2_DEBUG
#define BMA2X2_DEBUG 0
#endif

#define BMA2X2_CONFIG_DBG_LEVEL  DBG_MED_PRIO
//#define PRIMITIVE_DEBUG_MSG

#define DBG_MEDIUM_PRIO DBG_MED_PRIO

#if BMA2X2_DEBUG        //TODO
#ifndef PRIMITIVE_DEBUG_MSG


#if !BMA2X2_CONFIG_ENABLE_UIMAGE
#define BMA2X2_MSG_0(level,msg)             do { if (DBG_##level##_PRIO >= BMA2X2_CONFIG_DBG_LEVEL) MSG(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMA2X2-" msg);} while (0)
#define BMA2X2_MSG_1(level,msg,p1)          do { if (DBG_##level##_PRIO >= BMA2X2_CONFIG_DBG_LEVEL) MSG_1(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMA2X2-" msg,p1);} while (0)
#define BMA2X2_MSG_2(level,msg,p1,p2)       do { if (DBG_##level##_PRIO >= BMA2X2_CONFIG_DBG_LEVEL) MSG_2(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMA2X2-" msg,p1,p2);} while (0)
#define BMA2X2_MSG_3(level,msg,p1,p2,p3)    do { if (DBG_##level##_PRIO >= BMA2X2_CONFIG_DBG_LEVEL) MSG_3(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMA2X2-" msg,p1,p2,p3);} while (0)
#define BMA2X2_MSG_3_P(level,msg,p1,p2,p3)  do { if (DBG_##level##_PRIO >= BMA2X2_CONFIG_DBG_LEVEL) MSG_3(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMA2X2-" msg,p1,p2,p3);} while (0)
#define BMA2X2_MSG_3_F(level,msg,p1,p2,p3)  do { if (DBG_##level##_PRIO >= BMA2X2_CONFIG_DBG_LEVEL) MSG_3(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMA2X2-" msg,p1,p2,p3);} while (0)
#define BMA2X2_MSG_4(level,msg,p1,p2,p3,p4) do { if (DBG_##level##_PRIO >= BMA2X2_CONFIG_DBG_LEVEL) MSG_4(MSG_SSID_QDSP6,DBG_##level##_PRIO, "BMA2X2-" msg,p1,p2,p3,p4);} while (0)
#else   //BMA2X2_CONFIG_ENABLE_UIMAGE
#define BMA2X2_MSG_0(level,msg)             do { if (DBG_##level##_PRIO >= BMA2X2_CONFIG_DBG_LEVEL) UMSG(MSG_SSID_SNS,DBG_##level##_PRIO, "BMA2X2-" msg);} while (0)
#define BMA2X2_MSG_1(level,msg,p1)          do { if (DBG_##level##_PRIO >= BMA2X2_CONFIG_DBG_LEVEL) UMSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "BMA2X2-" msg,p1);} while (0)
#define BMA2X2_MSG_2(level,msg,p1,p2)       do { if (DBG_##level##_PRIO >= BMA2X2_CONFIG_DBG_LEVEL) UMSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "BMA2X2-" msg,p1,p2);} while (0)
#define BMA2X2_MSG_3(level,msg,p1,p2,p3)    do { if (DBG_##level##_PRIO >= BMA2X2_CONFIG_DBG_LEVEL) UMSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BMA2X2-" msg,p1,p2,p3);} while (0)
#define BMA2X2_MSG_3_P(level,msg,p1,p2,p3)  do { if (DBG_##level##_PRIO >= BMA2X2_CONFIG_DBG_LEVEL) UMSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BMA2X2-" msg,p1,p2,p3);} while (0)
#define BMA2X2_MSG_3_F(level,msg,p1,p2,p3)  do { if (DBG_##level##_PRIO >= BMA2X2_CONFIG_DBG_LEVEL) UMSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BMA2X2-" msg,p1,p2,p3);} while (0)
#define BMA2X2_MSG_4(level,msg,p1,p2,p3,p4) do { if (DBG_##level##_PRIO >= BMA2X2_CONFIG_DBG_LEVEL) UMSG_4(MSG_SSID_SNS,DBG_##level##_PRIO, "BMA2X2-" msg,p1,p2,p3,p4);} while (0)
#endif  //BMA2X2_CONFIG_ENABLE_UIMAGE



#else
#define BMA2X2_MSG_0(level,msg)
#define BMA2X2_MSG_1(level,msg,p1)
#define BMA2X2_MSG_2(level,msg,p1,p2)
#define BMA2X2_MSG_3(level,msg,p1,p2,p3)
#define BMA2X2_MSG_4(level,msg,p1,p2,p3)
//#define BMA2X2_MSG_3_P(level,msg,p1,p2,p3)
#define BMA2X2_MSG_3_P(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
#define BMA2X2_MSG_4(level,msg,p1,p2,p3,p4)
#define MED MEDIUM
#endif
#else
#define BMA2X2_MSG_0(level,msg)
#define BMA2X2_MSG_1(level,msg,p1)
#define BMA2X2_MSG_2(level,msg,p1,p2)
#define BMA2X2_MSG_3(level,msg,p1,p2,p3)
#define BMA2X2_MSG_3_P(level,msg,p1,p2,p3) //MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BMA2X2-" msg,p1,p2,p3)
#define BMA2X2_MSG_3_F(level,msg,p1,p2,p3)  //MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "BMA2X2-" msg,p1,p2,p3)
#define BMA2X2_MSG_4(level,msg,p1,p2,p3,p4)
#endif



#define BMA2X2_SSC_US_2_TICKS(us) sns_ddf_convert_usec_to_tick(us)



#define BMA2X2_MAX_FIFO_LEVEL 32
#define BMA2X2_MAX_FIFO_F_BYTES 6


#define BST_IGNORE_WARNING_UNUSED_VAR(v)        ((void)v)
#define BST_GET_VAL_BIT(val, bit) (((val)>>(bit)) & 0x01)
#define BST_GET_VAL_BITBLOCK(val, start, end) (((val)>>(start)) & ((1<<(end - start + 1))-1))

#define BST_SET_VAL_BIT(val, bit)      (val | (1 << bit))
#define BST_CLR_VAL_BIT(val, bit)      (val & (~(1 << bit)))

#define BST_SET_VAL_BITBLOCK(val, start, end, val_bb) \
    ( (val & \
       (~ (((1<<(end - start + 1))-1) << start) ) \
      )\
      |\
      ((val_bb &\
        ((1<<(end - start + 1))-1)) << start)\
    )

#define BST_ARRARY_SIZE(array) (sizeof(array)/sizeof(array[0]))

struct bst_val_pair {
    uint32_t l;
    uint32_t r;
};

typedef struct __attribute__ ((packed))
{
    sns_ddf_axes_map_s    axes_map;
    uint8_t     Reserved[128 - sizeof(sns_ddf_axes_map_s)];    // Reserved Space
    uint32_t    magic_num;                // Magic Number to verify the start of bma2x2 tap settings
    uint8_t th;
    uint8_t shock;
    uint8_t quiet;
    uint8_t dur;

} sns_dd_bma2x2_nv_params_s;


typedef struct
{
    uint8_t th;
    uint8_t shock;
    uint8_t quiet;
    uint8_t dur;
}sns_dd_bma2x2_tap_settings_s;




struct bst_cust_param_bma {
    struct {
        uint8_t th;
        uint8_t shock;
        uint8_t quiet;
        uint8_t dur;
    } tap;
};

struct bst_cust_params {
    struct bst_cust_param_bma bma;
};

#define BST_NVM_MAGIC (0x5F425354)      //('_BST')

#define SNS_DDF_NVM_SIZE            256
#define SNS_DDF_NVM_DEV_START       64
#define SNS_DDF_NVM_DEV_START_BST   128
#define SNS_DDF_NVM_DEV_END         (240 - 1)
struct bst_sensor_nvm_params_entire {
    /*! axis remap: 3 bytes */
    struct {
        uint8_t orient_x;
        uint8_t orient_y;
        uint8_t orient_z;
    } axis_remap;

    /*! reserved aread 1: from offset 3 to 63 */
    uint8_t reserved1[SNS_DDF_NVM_DEV_START - 3];


    union {
        /*! device specific block from offset 64 to offset 640 */
        uint8_t opaque[SNS_DDF_NVM_DEV_END
            - SNS_DDF_NVM_DEV_START + 1];
        struct {
            /*! unknown area to BOSCH */
            uint8_t unknown1[SNS_DDF_NVM_DEV_START_BST
                - SNS_DDF_NVM_DEV_START];

            /*! from offset 128 to offset 240, it is BOSCH area */

            /*-------<BOSCH_AREA>-----*/
            /*! magic written as in the order of:
             * @128: 'T' @129: 'B' @130: 'S' @131: '_'
             */
            uint8_t magic[4];

            /*! param of all bst sensors */
            struct bst_cust_params bst_cust_all;
            /*------</BOSCH_AREA>-----*/
        } dev_param_blk;
    };

    uint8_t reserved2[SNS_DDF_NVM_SIZE - (SNS_DDF_NVM_DEV_END + 1)];
};


#define BMA2X2_ODR_NUM         8
#define BMA2X2_MAX_ODR         2000
#define BMA2X2_DBT_ODR         400




#define BMA2X2_INT_INFO_TAP_FIRST_NONE      0x00
#define BMA2X2_INT_INFO_TAP_FIRST_X         0x10
#define BMA2X2_INT_INFO_TAP_FIRST_Y         0x20
#define BMA2X2_INT_INFO_TAP_FIRST_Z         0x40

#define BMA2X2_RESET_INT__POS        7
#define BMA2X2_RESET_INT__LEN        1
#define BMA2X2_RESET_INT__MSK        0x80
#define BMA2X2_RESET_INT__REG        BMA2X2_INT_CTRL_REG


#define BMA2X2_TAP_FIRST_X__POS        4
#define BMA2X2_TAP_FIRST_X__LEN        1
#define BMA2X2_TAP_FIRST_X__MSK        0x10
#define BMA2X2_TAP_FIRST_X__REG        BMA2X2_STATUS_TAP_SLOPE_REG

#define BMA2X2_TAP_FIRST_Y__POS        5
#define BMA2X2_TAP_FIRST_Y__LEN        1
#define BMA2X2_TAP_FIRST_Y__MSK        0x20
#define BMA2X2_TAP_FIRST_Y__REG        BMA2X2_STATUS_TAP_SLOPE_REG

#define BMA2X2_TAP_FIRST_Z__POS        6
#define BMA2X2_TAP_FIRST_Z__LEN        1
#define BMA2X2_TAP_FIRST_Z__MSK        0x40
#define BMA2X2_TAP_FIRST_Z__REG        BMA2X2_STATUS_TAP_SLOPE_REG

#define BMA2X2_TAP_SIGN_S__POS         7
#define BMA2X2_TAP_SIGN_S__LEN         1
#define BMA2X2_TAP_SIGN_S__MSK         0x80
#define BMA2X2_TAP_SIGN_S__REG         BMA2X2_STATUS_TAP_SLOPE_REG

#define BMA2X2_FIFO_FRAME_COUNTER_S__POS             0
#define BMA2X2_FIFO_FRAME_COUNTER_S__LEN             7
#define BMA2X2_FIFO_FRAME_COUNTER_S__MSK             0x7F
#define BMA2X2_FIFO_FRAME_COUNTER_S__REG             BMA2X2_STATUS_FIFO_REG

#define BMA2X2_FIFO_OVERRUN__POS             7
#define BMA2X2_FIFO_OVERRUN__LEN             1
#define BMA2X2_FIFO_OVERRUN__MSK             0x80
#define BMA2X2_FIFO_OVERRUN__REG             BMA2X2_STATUS_FIFO_REG


#define BMA2X2_MAX_SUB_DEV      16
//TODO: extend here for more virtual sensors
//currently supported: accel, temp, double tap
#define BMA2X2_MAX_NUM_SENSORS  3


#define LOW_G_INTERRUPT             0x04
#define MOTION_DETECT_INTERRUPT     0x04
#define HIGH_G_INTERRUPT            0x02
#define DOUBLE_TAP_INTERRUPT        0x10
#define SINGLE_TAP_INTERRUPT        0x20
#define ORIENT_INTERRUPT            0x40
#define FLAT_INTERRUPT              0x80
#define SLOW_NO_MOTION_INTERRUPT    0x08

#define FIFO_FULL_INTERRUPT     0x20
#define FIFO_WM_INTERRUPT           0x40


#define BMA255_CHIP_ID 0XFA
#define BMA250E_CHIP_ID 0XF9
#define BMA222E_CHIP_ID 0XF8
#define BMA280_CHIP_ID 0XFB

#define BMA280_TYPE 0
#define BMA255_TYPE 1
#define BMA250E_TYPE 2
#define BMA222E_TYPE 3

#define BMA2X2_REGA_INT_STATUS_0                0x09
#define BMA2X2_REGA_INT_STATUS_1                0x0a

#define BMA2X2_REGA_FIFO_CONFIG_1               0x3e

#define BMA2X2_REGA_PMU_BW                      0x10
#define BMA2X2_REGA_PMU_LPW                     0x11

#define BMA2X2_REGA_INT_EN_0                    0x16
#define BMA2X2_REGA_INT_EN_1                    0x17

#define BMA2X2_REGA_INT_MAP_0                   0x19
#define BMA2X2_REGA_INT_MAP_1                   0x1a
#define BMA2X2_REGA_INT_MAP_2                   0x1b

#define BOSCH_ACCEL_BMA280_MSB_SHIFT_LEFT       6
#define BOSCH_ACCEL_BMA255_MSB_SHIFT_LEFT       4
#define BOSCH_ACCEL_BMA250E_MSB_SHIFT_LEFT      2
#define BOSCH_ACCEL_BMA222E_MSB_SHIFT_LEFT      0

#define BOSCH_ACCEL_BMA280_LSB_SHIFT_RIGHT       2
#define BOSCH_ACCEL_BMA255_LSB_SHIFT_RIGHT       4
#define BOSCH_ACCEL_BMA250E_LSB_SHIFT_RIGHT      6
#define BOSCH_ACCEL_BMA222E_LSB_SHIFT_RIGHT      8

#define LSB_RANGE_14BITS        16384
#define LSB_RANGE_12BITS        4096
#define LSB_RANGE_10BITS        1024
#define LSB_RANGE_8BITS         256

#define BMA2X2_CHIP_ID_REG                      0x00
#define BMA2X2_VERSION_REG                      0x01
#define BMA2X2_X_AXIS_LSB_REG                   0x02
#define BMA2X2_X_AXIS_MSB_REG                   0x03
#define BMA2X2_Y_AXIS_LSB_REG                   0x04
#define BMA2X2_Y_AXIS_MSB_REG                   0x05
#define BMA2X2_Z_AXIS_LSB_REG                   0x06
#define BMA2X2_Z_AXIS_MSB_REG                   0x07
#define BMA2X2_TEMP_RD_REG                      0x08
#define BMA2X2_STATUS1_REG                      0x09
#define BMA2X2_STATUS2_REG                      0x0A
#define BMA2X2_STATUS_TAP_SLOPE_REG             0x0B
#define BMA2X2_STATUS_ORIENT_HIGH_REG           0x0C
#define BMA2X2_STATUS_FIFO_REG                  0x0E
#define BMA2X2_RANGE_SEL_REG                    0x0F
#define BMA2X2_BW_SEL_REG                       0x10
#define BMA2X2_MODE_CTRL_REG                    0x11
#define BMA2X2_LOW_NOISE_CTRL_REG               0x12
#define BMA2X2_DATA_CTRL_REG                    0x13
#define BMA2X2_RESET_REG                        0x14
#define BMA2X2_INT_ENABLE1_REG                  0x16
#define BMA2X2_INT_ENABLE2_REG                  0x17
#define BMA2X2_INT_SLO_NO_MOT_REG               0x18
#define BMA2X2_INT1_PAD_SEL_REG                 0x19
#define BMA2X2_INT_DATA_SEL_REG                 0x1A
#define BMA2X2_INT2_PAD_SEL_REG                 0x1B
#define BMA2X2_INT_SRC_REG                      0x1E
#define BMA2X2_INT_SET_REG                      0x20
#define BMA2X2_INT_CTRL_REG                     0x21
#define BMA2X2_LOW_DURN_REG                     0x22
#define BMA2X2_LOW_THRES_REG                    0x23
#define BMA2X2_LOW_HIGH_HYST_REG                0x24
#define BMA2X2_HIGH_DURN_REG                    0x25
#define BMA2X2_HIGH_THRES_REG                   0x26
#define BMA2X2_SLOPE_DURN_REG                   0x27
#define BMA2X2_SLOPE_THRES_REG                  0x28
#define BMA2X2_SLO_NO_MOT_THRES_REG             0x29
#define BMA2X2_TAP_PARAM_REG                    0x2A
#define BMA2X2_TAP_THRES_REG                    0x2B
#define BMA2X2_ORIENT_PARAM_REG                 0x2C
#define BMA2X2_THETA_BLOCK_REG                  0x2D
#define BMA2X2_THETA_FLAT_REG                   0x2E
#define BMA2X2_FLAT_HOLD_TIME_REG               0x2F
#define BMA2X2_FIFO_WML_TRIG                    0x30
#define BMA2X2_STATUS_LOW_POWER_REG             0x31
#define BMA2X2_SELF_TEST_REG                    0x32
#define BMA2X2_EEPROM_CTRL_REG                  0x33
#define BMA2X2_SERIAL_CTRL_REG                  0x34
#define BMA2X2_CTRL_UNLOCK_REG                  0x35
#define BMA2X2_OFFSET_CTRL_REG                  0x36
#define BMA2X2_OFFSET_PARAMS_REG                0x37
#define BMA2X2_OFFSET_FILT_X_REG                0x38
#define BMA2X2_OFFSET_FILT_Y_REG                0x39
#define BMA2X2_OFFSET_FILT_Z_REG                0x3A
#define BMA2X2_OFFSET_UNFILT_X_REG              0x3B
#define BMA2X2_OFFSET_UNFILT_Y_REG              0x3C
#define BMA2X2_OFFSET_UNFILT_Z_REG              0x3D
#define BMA2X2_FIFO_MODE_REG                    0x3E
#define BMA2X2_FIFO_DATA_OUTPUT_REG             0x3F



#define BMA2x2_SLO_NO_MOT_DUR__POS  2
#define BMA2x2_SLO_NO_MOT_DUR__LEN  6
#define BMA2x2_SLO_NO_MOT_DUR__MSK  0xFC
#define BMA2x2_SLO_NO_MOT_DUR__REG  BMA2X2_SLOPE_DURN_REG


#define BMA2X2_ACC_X14_LSB__POS           2
#define BMA2X2_ACC_X14_LSB__LEN           6
#define BMA2X2_ACC_X14_LSB__MSK           0xFC
#define BMA2X2_ACC_X14_LSB__REG           BMA2X2_X_AXIS_LSB_REG

#define BMA2X2_ACC_X12_LSB__POS           4
#define BMA2X2_ACC_X12_LSB__LEN           4
#define BMA2X2_ACC_X12_LSB__MSK           0xF0
#define BMA2X2_ACC_X12_LSB__REG           BMA2X2_X_AXIS_LSB_REG

#define BMA2X2_ACC_X10_LSB__POS           6
#define BMA2X2_ACC_X10_LSB__LEN           2
#define BMA2X2_ACC_X10_LSB__MSK           0xC0
#define BMA2X2_ACC_X10_LSB__REG           BMA2X2_X_AXIS_LSB_REG

#define BMA2X2_ACC_X8_LSB__POS           0
#define BMA2X2_ACC_X8_LSB__LEN           0
#define BMA2X2_ACC_X8_LSB__MSK           0x00
#define BMA2X2_ACC_X8_LSB__REG           BMA2X2_X_AXIS_LSB_REG


#define BMA2X2_ACC_Y14_LSB__POS           2
#define BMA2X2_ACC_Y14_LSB__LEN           6
#define BMA2X2_ACC_Y14_LSB__MSK           0xFC
#define BMA2X2_ACC_Y14_LSB__REG           BMA2X2_Y_AXIS_LSB_REG

#define BMA2X2_ACC_Y12_LSB__POS           4
#define BMA2X2_ACC_Y12_LSB__LEN           4
#define BMA2X2_ACC_Y12_LSB__MSK           0xF0
#define BMA2X2_ACC_Y12_LSB__REG           BMA2X2_Y_AXIS_LSB_REG

#define BMA2X2_ACC_Y10_LSB__POS           6
#define BMA2X2_ACC_Y10_LSB__LEN           2
#define BMA2X2_ACC_Y10_LSB__MSK           0xC0
#define BMA2X2_ACC_Y10_LSB__REG           BMA2X2_Y_AXIS_LSB_REG

#define BMA2X2_ACC_Y8_LSB__POS           0
#define BMA2X2_ACC_Y8_LSB__LEN           0
#define BMA2X2_ACC_Y8_LSB__MSK           0x00
#define BMA2X2_ACC_Y8_LSB__REG           BMA2X2_Y_AXIS_LSB_REG


#define BMA2X2_ACC_Z14_LSB__POS           2
#define BMA2X2_ACC_Z14_LSB__LEN           6
#define BMA2X2_ACC_Z14_LSB__MSK           0xFC
#define BMA2X2_ACC_Z14_LSB__REG           BMA2X2_Z_AXIS_LSB_REG

#define BMA2X2_ACC_Z12_LSB__POS           4
#define BMA2X2_ACC_Z12_LSB__LEN           4
#define BMA2X2_ACC_Z12_LSB__MSK           0xF0
#define BMA2X2_ACC_Z12_LSB__REG           BMA2X2_Z_AXIS_LSB_REG

#define BMA2X2_ACC_Z10_LSB__POS           6
#define BMA2X2_ACC_Z10_LSB__LEN           2
#define BMA2X2_ACC_Z10_LSB__MSK           0xC0
#define BMA2X2_ACC_Z10_LSB__REG           BMA2X2_Z_AXIS_LSB_REG

#define BMA2X2_ACC_Z8_LSB__POS           0
#define BMA2X2_ACC_Z8_LSB__LEN           0
#define BMA2X2_ACC_Z8_LSB__MSK           0x00
#define BMA2X2_ACC_Z8_LSB__REG           BMA2X2_Z_AXIS_LSB_REG

#define BMA2X2_ACC_X_LSB__POS           4
#define BMA2X2_ACC_X_LSB__LEN           4
#define BMA2X2_ACC_X_LSB__MSK           0xF0
#define BMA2X2_ACC_X_LSB__REG           BMA2X2_X_AXIS_LSB_REG

#define BMA2X2_ACC_X_MSB__POS           0
#define BMA2X2_ACC_X_MSB__LEN           8
#define BMA2X2_ACC_X_MSB__MSK           0xFF
#define BMA2X2_ACC_X_MSB__REG           BMA2X2_X_AXIS_MSB_REG

#define BMA2X2_ACC_Y_LSB__POS           4
#define BMA2X2_ACC_Y_LSB__LEN           4
#define BMA2X2_ACC_Y_LSB__MSK           0xF0
#define BMA2X2_ACC_Y_LSB__REG           BMA2X2_Y_AXIS_LSB_REG

#define BMA2X2_ACC_Y_MSB__POS           0
#define BMA2X2_ACC_Y_MSB__LEN           8
#define BMA2X2_ACC_Y_MSB__MSK           0xFF
#define BMA2X2_ACC_Y_MSB__REG           BMA2X2_Y_AXIS_MSB_REG

#define BMA2X2_ACC_Z_LSB__POS           4
#define BMA2X2_ACC_Z_LSB__LEN           4
#define BMA2X2_ACC_Z_LSB__MSK           0xF0
#define BMA2X2_ACC_Z_LSB__REG           BMA2X2_Z_AXIS_LSB_REG

#define BMA2X2_ACC_Z_MSB__POS           0
#define BMA2X2_ACC_Z_MSB__LEN           8
#define BMA2X2_ACC_Z_MSB__MSK           0xFF
#define BMA2X2_ACC_Z_MSB__REG           BMA2X2_Z_AXIS_MSB_REG

#define BMA2X2_RANGE_SEL__POS             0
#define BMA2X2_RANGE_SEL__LEN             4
#define BMA2X2_RANGE_SEL__MSK             0x0F
#define BMA2X2_RANGE_SEL__REG             BMA2X2_RANGE_SEL_REG

#define BMA2X2_EN_SELF_TEST__POS                0
#define BMA2X2_EN_SELF_TEST__LEN                2
#define BMA2X2_EN_SELF_TEST__MSK                0x03
#define BMA2X2_EN_SELF_TEST__REG                BMA2X2_SELF_TEST_REG


#define BMA2X2_NEG_SELF_TEST__POS               2
#define BMA2X2_NEG_SELF_TEST__LEN               1
#define BMA2X2_NEG_SELF_TEST__MSK               0x04
#define BMA2X2_NEG_SELF_TEST__REG               BMA2X2_SELF_TEST_REG

#define BMA2X2_SELF_TEST_AMP__POS               4
#define BMA2X2_SELF_TEST_AMP__LEN               1
#define BMA2X2_SELF_TEST_AMP__MSK               0x10
#define BMA2X2_SELF_TEST_AMP__REG               BMA2X2_SELF_TEST_REG


#define BMA2X2_EN_LOW_POWER__POS          6
#define BMA2X2_EN_LOW_POWER__LEN          1
#define BMA2X2_EN_LOW_POWER__MSK          0x40
#define BMA2X2_EN_LOW_POWER__REG          BMA2X2_MODE_CTRL_REG

#define BMA2X2_EN_SUSPEND__POS            7
#define BMA2X2_EN_SUSPEND__LEN            1
#define BMA2X2_EN_SUSPEND__MSK            0x80
#define BMA2X2_EN_SUSPEND__REG            BMA2X2_MODE_CTRL_REG

#define BMA2X2_SLEEP_TIMER__POS          5
#define BMA2X2_SLEEP_TIMER__LEN          1
#define BMA2X2_SLEEP_TIMER__MSK          0x20
#define BMA2X2_SLEEP_TIMER__REG          BMA2X2_LOW_NOISE_CTRL_REG

#define BMA2X2_LOW_POWER_MODE__POS          6
#define BMA2X2_LOW_POWER_MODE__LEN          1
#define BMA2X2_LOW_POWER_MODE__MSK          0x40
#define BMA2X2_LOW_POWER_MODE__REG          BMA2X2_LOW_NOISE_CTRL_REG

#define BMA2X2_EN_LOW_NOISE__POS          7
#define BMA2X2_EN_LOW_NOISE__LEN          1
#define BMA2X2_EN_LOW_NOISE__MSK          0x80
#define BMA2X2_EN_LOW_NOISE__REG          BMA2X2_LOW_NOISE_CTRL_REG

#define BMA2X2_DIS_SHADOW_PROC__POS       6
#define BMA2X2_DIS_SHADOW_PROC__LEN       1
#define BMA2X2_DIS_SHADOW_PROC__MSK       0x40
#define BMA2X2_DIS_SHADOW_PROC__REG       BMA2X2_DATA_CTRL_REG

#define BMA2X2_EN_DATA_HIGH_BW__POS         7
#define BMA2X2_EN_DATA_HIGH_BW__LEN         1
#define BMA2X2_EN_DATA_HIGH_BW__MSK         0x80
#define BMA2X2_EN_DATA_HIGH_BW__REG         BMA2X2_DATA_CTRL_REG

#define BMA2X2_EN_SOFT_RESET__POS         0
#define BMA2X2_EN_SOFT_RESET__LEN         8
#define BMA2X2_EN_SOFT_RESET__MSK         0xFF
#define BMA2X2_EN_SOFT_RESET__REG         BMA2X2_RESET_REG

#define BMA2X2_EN_SOFT_RESET_VALUE        0xB6

#define BMA2X2_EN_SLOPE_X_INT__POS         0
#define BMA2X2_EN_SLOPE_X_INT__LEN         1
#define BMA2X2_EN_SLOPE_X_INT__MSK         0x01
#define BMA2X2_EN_SLOPE_X_INT__REG         BMA2X2_INT_ENABLE1_REG

#define BMA2X2_EN_SLOPE_Y_INT__POS         1
#define BMA2X2_EN_SLOPE_Y_INT__LEN         1
#define BMA2X2_EN_SLOPE_Y_INT__MSK         0x02
#define BMA2X2_EN_SLOPE_Y_INT__REG         BMA2X2_INT_ENABLE1_REG

#define BMA2X2_EN_SLOPE_Z_INT__POS         2
#define BMA2X2_EN_SLOPE_Z_INT__LEN         1
#define BMA2X2_EN_SLOPE_Z_INT__MSK         0x04
#define BMA2X2_EN_SLOPE_Z_INT__REG         BMA2X2_INT_ENABLE1_REG

#define BMA2X2_EN_DOUBLE_TAP_INT__POS      4
#define BMA2X2_EN_DOUBLE_TAP_INT__LEN      1
#define BMA2X2_EN_DOUBLE_TAP_INT__MSK      0x10
#define BMA2X2_EN_DOUBLE_TAP_INT__REG      BMA2X2_INT_ENABLE1_REG

#define BMA2X2_EN_SINGLE_TAP_INT__POS      5
#define BMA2X2_EN_SINGLE_TAP_INT__LEN      1
#define BMA2X2_EN_SINGLE_TAP_INT__MSK      0x20
#define BMA2X2_EN_SINGLE_TAP_INT__REG      BMA2X2_INT_ENABLE1_REG

#define BMA2X2_EN_ORIENT_INT__POS          6
#define BMA2X2_EN_ORIENT_INT__LEN          1
#define BMA2X2_EN_ORIENT_INT__MSK          0x40
#define BMA2X2_EN_ORIENT_INT__REG          BMA2X2_INT_ENABLE1_REG

#define BMA2X2_EN_FLAT_INT__POS            7
#define BMA2X2_EN_FLAT_INT__LEN            1
#define BMA2X2_EN_FLAT_INT__MSK            0x80
#define BMA2X2_EN_FLAT_INT__REG            BMA2X2_INT_ENABLE1_REG

#define BMA2X2_EN_HIGHG_X_INT__POS         0
#define BMA2X2_EN_HIGHG_X_INT__LEN         1
#define BMA2X2_EN_HIGHG_X_INT__MSK         0x01
#define BMA2X2_EN_HIGHG_X_INT__REG         BMA2X2_INT_ENABLE2_REG

#define BMA2X2_EN_HIGHG_Y_INT__POS         1
#define BMA2X2_EN_HIGHG_Y_INT__LEN         1
#define BMA2X2_EN_HIGHG_Y_INT__MSK         0x02
#define BMA2X2_EN_HIGHG_Y_INT__REG         BMA2X2_INT_ENABLE2_REG

#define BMA2X2_EN_HIGHG_Z_INT__POS         2
#define BMA2X2_EN_HIGHG_Z_INT__LEN         1
#define BMA2X2_EN_HIGHG_Z_INT__MSK         0x04
#define BMA2X2_EN_HIGHG_Z_INT__REG         BMA2X2_INT_ENABLE2_REG

#define BMA2X2_EN_LOWG_INT__POS            3
#define BMA2X2_EN_LOWG_INT__LEN            1
#define BMA2X2_EN_LOWG_INT__MSK            0x08
#define BMA2X2_EN_LOWG_INT__REG            BMA2X2_INT_ENABLE2_REG

#define BMA2X2_EN_NEW_DATA_INT__POS        4
#define BMA2X2_EN_NEW_DATA_INT__LEN        1
#define BMA2X2_EN_NEW_DATA_INT__MSK        0x10
#define BMA2X2_EN_NEW_DATA_INT__REG        BMA2X2_INT_ENABLE2_REG

#define BMA2X2_INT_FFULL_EN_INT__POS        5
#define BMA2X2_INT_FFULL_EN_INT__LEN        1
#define BMA2X2_INT_FFULL_EN_INT__MSK        0x20
#define BMA2X2_INT_FFULL_EN_INT__REG        BMA2X2_INT_ENABLE2_REG

#define BMA2X2_INT_FWM_EN_INT__POS        6
#define BMA2X2_INT_FWM_EN_INT__LEN        1
#define BMA2X2_INT_FWM_EN_INT__MSK        0x40
#define BMA2X2_INT_FWM_EN_INT__REG        BMA2X2_INT_ENABLE2_REG

#define BMA2X2_INT_SLO_NO_MOT_EN_X_INT__POS        0
#define BMA2X2_INT_SLO_NO_MOT_EN_X_INT__LEN        1
#define BMA2X2_INT_SLO_NO_MOT_EN_X_INT__MSK        0x01
#define BMA2X2_INT_SLO_NO_MOT_EN_X_INT__REG        BMA2X2_INT_SLO_NO_MOT_REG

#define BMA2X2_INT_SLO_NO_MOT_EN_Y_INT__POS        1
#define BMA2X2_INT_SLO_NO_MOT_EN_Y_INT__LEN        1
#define BMA2X2_INT_SLO_NO_MOT_EN_Y_INT__MSK        0x02
#define BMA2X2_INT_SLO_NO_MOT_EN_Y_INT__REG        BMA2X2_INT_SLO_NO_MOT_REG

#define BMA2X2_INT_SLO_NO_MOT_EN_Z_INT__POS        2
#define BMA2X2_INT_SLO_NO_MOT_EN_Z_INT__LEN        1
#define BMA2X2_INT_SLO_NO_MOT_EN_Z_INT__MSK        0x04
#define BMA2X2_INT_SLO_NO_MOT_EN_Z_INT__REG        BMA2X2_INT_SLO_NO_MOT_REG

#define BMA2X2_INT_SLO_NO_MOT_EN_SEL_INT__POS        3
#define BMA2X2_INT_SLO_NO_MOT_EN_SEL_INT__LEN        1
#define BMA2X2_INT_SLO_NO_MOT_EN_SEL_INT__MSK        0x08
#define BMA2X2_INT_SLO_NO_MOT_EN_SEL_INT__REG        BMA2X2_INT_SLO_NO_MOT_REG

#define BMA2X2_EN_INT1_PAD_LOWG__POS        0
#define BMA2X2_EN_INT1_PAD_LOWG__LEN        1
#define BMA2X2_EN_INT1_PAD_LOWG__MSK        0x01
#define BMA2X2_EN_INT1_PAD_LOWG__REG        BMA2X2_INT1_PAD_SEL_REG

#define BMA2X2_EN_INT1_PAD_HIGHG__POS       1
#define BMA2X2_EN_INT1_PAD_HIGHG__LEN       1
#define BMA2X2_EN_INT1_PAD_HIGHG__MSK       0x02
#define BMA2X2_EN_INT1_PAD_HIGHG__REG       BMA2X2_INT1_PAD_SEL_REG

#define BMA2X2_EN_INT1_PAD_SLOPE__POS       2
#define BMA2X2_EN_INT1_PAD_SLOPE__LEN       1
#define BMA2X2_EN_INT1_PAD_SLOPE__MSK       0x04
#define BMA2X2_EN_INT1_PAD_SLOPE__REG       BMA2X2_INT1_PAD_SEL_REG

#define BMA2X2_EN_INT1_PAD_SLO_NO_MOT__POS        3
#define BMA2X2_EN_INT1_PAD_SLO_NO_MOT__LEN        1
#define BMA2X2_EN_INT1_PAD_SLO_NO_MOT__MSK        0x08
#define BMA2X2_EN_INT1_PAD_SLO_NO_MOT__REG        BMA2X2_INT1_PAD_SEL_REG

#define BMA2X2_EN_INT1_PAD_DB_TAP__POS      4
#define BMA2X2_EN_INT1_PAD_DB_TAP__LEN      1
#define BMA2X2_EN_INT1_PAD_DB_TAP__MSK      0x10
#define BMA2X2_EN_INT1_PAD_DB_TAP__REG      BMA2X2_INT1_PAD_SEL_REG

#define BMA2X2_EN_INT1_PAD_SNG_TAP__POS     5
#define BMA2X2_EN_INT1_PAD_SNG_TAP__LEN     1
#define BMA2X2_EN_INT1_PAD_SNG_TAP__MSK     0x20
#define BMA2X2_EN_INT1_PAD_SNG_TAP__REG     BMA2X2_INT1_PAD_SEL_REG

#define BMA2X2_EN_INT1_PAD_ORIENT__POS      6
#define BMA2X2_EN_INT1_PAD_ORIENT__LEN      1
#define BMA2X2_EN_INT1_PAD_ORIENT__MSK      0x40
#define BMA2X2_EN_INT1_PAD_ORIENT__REG      BMA2X2_INT1_PAD_SEL_REG

#define BMA2X2_EN_INT1_PAD_FLAT__POS        7
#define BMA2X2_EN_INT1_PAD_FLAT__LEN        1
#define BMA2X2_EN_INT1_PAD_FLAT__MSK        0x80
#define BMA2X2_EN_INT1_PAD_FLAT__REG        BMA2X2_INT1_PAD_SEL_REG

#define BMA2X2_INT_MODE_SEL__POS                0
#define BMA2X2_INT_MODE_SEL__LEN                4
#define BMA2X2_INT_MODE_SEL__MSK                0x0F
#define BMA2X2_INT_MODE_SEL__REG                BMA2X2_INT_CTRL_REG


#define BMA2X2_LOWG_DUR__POS                    0
#define BMA2X2_LOWG_DUR__LEN                    8
#define BMA2X2_LOWG_DUR__MSK                    0xFF
#define BMA2X2_LOWG_DUR__REG                    BMA2X2_LOW_DURN_REG

#define BMA2X2_LOWG_THRES__POS                  0
#define BMA2X2_LOWG_THRES__LEN                  8
#define BMA2X2_LOWG_THRES__MSK                  0xFF
#define BMA2X2_LOWG_THRES__REG                  BMA2X2_LOW_THRES_REG

#define BMA2X2_LOWG_HYST__POS                   0
#define BMA2X2_LOWG_HYST__LEN                   2
#define BMA2X2_LOWG_HYST__MSK                   0x03
#define BMA2X2_LOWG_HYST__REG                   BMA2X2_LOW_HIGH_HYST_REG

#define BMA2X2_LOWG_INT_MODE__POS               2
#define BMA2X2_LOWG_INT_MODE__LEN               1
#define BMA2X2_LOWG_INT_MODE__MSK               0x04
#define BMA2X2_LOWG_INT_MODE__REG               BMA2X2_LOW_HIGH_HYST_REG

#define BMA2X2_HIGHG_DUR__POS                    0
#define BMA2X2_HIGHG_DUR__LEN                    8
#define BMA2X2_HIGHG_DUR__MSK                    0xFF
#define BMA2X2_HIGHG_DUR__REG                    BMA2X2_HIGH_DURN_REG

#define BMA2X2_HIGHG_THRES__POS                  0
#define BMA2X2_HIGHG_THRES__LEN                  8
#define BMA2X2_HIGHG_THRES__MSK                  0xFF
#define BMA2X2_HIGHG_THRES__REG                  BMA2X2_HIGH_THRES_REG

#define BMA2X2_HIGHG_HYST__POS                  6
#define BMA2X2_HIGHG_HYST__LEN                  2
#define BMA2X2_HIGHG_HYST__MSK                  0xC0
#define BMA2X2_HIGHG_HYST__REG                  BMA2X2_LOW_HIGH_HYST_REG

#define BMA2X2_SLOPE_DUR__POS                    0
#define BMA2X2_SLOPE_DUR__LEN                    2
#define BMA2X2_SLOPE_DUR__MSK                    0x03
#define BMA2X2_SLOPE_DUR__REG                    BMA2X2_SLOPE_DURN_REG

#define BMA2X2_SLO_NO_MOT_DUR__POS                    2
#define BMA2X2_SLO_NO_MOT_DUR__LEN                    6
#define BMA2X2_SLO_NO_MOT_DUR__MSK                    0xFC
#define BMA2X2_SLO_NO_MOT_DUR__REG                    BMA2X2_SLOPE_DURN_REG

#define BMA2X2_SLOPE_THRES__POS                  0
#define BMA2X2_SLOPE_THRES__LEN                  8
#define BMA2X2_SLOPE_THRES__MSK                  0xFF
#define BMA2X2_SLOPE_THRES__REG                  BMA2X2_SLOPE_THRES_REG

#define BMA2X2_SLO_NO_MOT_THRES__POS                  0
#define BMA2X2_SLO_NO_MOT_THRES__LEN                  8
#define BMA2X2_SLO_NO_MOT_THRES__MSK                  0xFF
#define BMA2X2_SLO_NO_MOT_THRES__REG           BMA2X2_SLO_NO_MOT_THRES_REG

#define BMA2X2_TAP_DUR__POS                    0
#define BMA2X2_TAP_DUR__LEN                    3
#define BMA2X2_TAP_DUR__MSK                    0x07
#define BMA2X2_TAP_DUR__REG                    BMA2X2_TAP_PARAM_REG

#define BMA2X2_TAP_SHOCK_DURN__POS             6
#define BMA2X2_TAP_SHOCK_DURN__LEN             1
#define BMA2X2_TAP_SHOCK_DURN__MSK             0x40
#define BMA2X2_TAP_SHOCK_DURN__REG             BMA2X2_TAP_PARAM_REG

#define BMA2X2_ADV_TAP_INT__POS                5
#define BMA2X2_ADV_TAP_INT__LEN                1
#define BMA2X2_ADV_TAP_INT__MSK                0x20
#define BMA2X2_ADV_TAP_INT__REG                BMA2X2_TAP_PARAM_REG

#define BMA2X2_TAP_QUIET_DURN__POS             7
#define BMA2X2_TAP_QUIET_DURN__LEN             1
#define BMA2X2_TAP_QUIET_DURN__MSK             0x80
#define BMA2X2_TAP_QUIET_DURN__REG             BMA2X2_TAP_PARAM_REG

#define BMA2X2_TAP_THRES__POS                  0
#define BMA2X2_TAP_THRES__LEN                  5
#define BMA2X2_TAP_THRES__MSK                  0x1F
#define BMA2X2_TAP_THRES__REG                  BMA2X2_TAP_THRES_REG

#define BMA2X2_TAP_SAMPLES__POS                6
#define BMA2X2_TAP_SAMPLES__LEN                2
#define BMA2X2_TAP_SAMPLES__MSK                0xC0
#define BMA2X2_TAP_SAMPLES__REG                BMA2X2_TAP_THRES_REG

#define BMA2X2_ORIENT_MODE__POS                  0
#define BMA2X2_ORIENT_MODE__LEN                  2
#define BMA2X2_ORIENT_MODE__MSK                  0x03
#define BMA2X2_ORIENT_MODE__REG                  BMA2X2_ORIENT_PARAM_REG

#define BMA2X2_ORIENT_BLOCK__POS                 2
#define BMA2X2_ORIENT_BLOCK__LEN                 2
#define BMA2X2_ORIENT_BLOCK__MSK                 0x0C
#define BMA2X2_ORIENT_BLOCK__REG                 BMA2X2_ORIENT_PARAM_REG

#define BMA2X2_ORIENT_HYST__POS                  4
#define BMA2X2_ORIENT_HYST__LEN                  3
#define BMA2X2_ORIENT_HYST__MSK                  0x70
#define BMA2X2_ORIENT_HYST__REG                  BMA2X2_ORIENT_PARAM_REG

#define BMA2X2_ORIENT_AXIS__POS                  7
#define BMA2X2_ORIENT_AXIS__LEN                  1
#define BMA2X2_ORIENT_AXIS__MSK                  0x80
#define BMA2X2_ORIENT_AXIS__REG                  BMA2X2_THETA_BLOCK_REG

#define BMA2X2_ORIENT_UD_EN__POS                  6
#define BMA2X2_ORIENT_UD_EN__LEN                  1
#define BMA2X2_ORIENT_UD_EN__MSK                  0x40
#define BMA2X2_ORIENT_UD_EN__REG                  BMA2X2_THETA_BLOCK_REG

#define BMA2X2_THETA_BLOCK__POS                  0
#define BMA2X2_THETA_BLOCK__LEN                  6
#define BMA2X2_THETA_BLOCK__MSK                  0x3F
#define BMA2X2_THETA_BLOCK__REG                  BMA2X2_THETA_BLOCK_REG

#define BMA2X2_THETA_FLAT__POS                  0
#define BMA2X2_THETA_FLAT__LEN                  6
#define BMA2X2_THETA_FLAT__MSK                  0x3F
#define BMA2X2_THETA_FLAT__REG                  BMA2X2_THETA_FLAT_REG

#define BMA2X2_FLAT_HOLD_TIME__POS              4
#define BMA2X2_FLAT_HOLD_TIME__LEN              2
#define BMA2X2_FLAT_HOLD_TIME__MSK              0x30
#define BMA2X2_FLAT_HOLD_TIME__REG              BMA2X2_FLAT_HOLD_TIME_REG

#define BMA2X2_FLAT_HYS__POS                   0
#define BMA2X2_FLAT_HYS__LEN                   3
#define BMA2X2_FLAT_HYS__MSK                   0x07
#define BMA2X2_FLAT_HYS__REG                   BMA2X2_FLAT_HOLD_TIME_REG

#define BMA2X2_FIFO_WML_TRIG_RETAIN__POS                   0
#define BMA2X2_FIFO_WML_TRIG_RETAIN__LEN                   6
#define BMA2X2_FIFO_WML_TRIG_RETAIN__MSK                   0x3F
#define BMA2X2_FIFO_WML_TRIG_RETAIN__REG                   BMA2X2_FIFO_WML_TRIG

#define BMA2X2_EN_INT2_PAD_FFULL__POS     5
#define BMA2X2_EN_INT2_PAD_FFULL__LEN     1
#define BMA2X2_EN_INT2_PAD_FFULL__MSK     0x20
#define BMA2X2_EN_INT2_PAD_FFULL__REG     BMA2X2_INT_DATA_SEL_REG

#define BMA2X2_EN_INT2_PAD_FWM__POS     6
#define BMA2X2_EN_INT2_PAD_FWM__LEN     1
#define BMA2X2_EN_INT2_PAD_FWM__MSK     0x40
#define BMA2X2_EN_INT2_PAD_FWM__REG     BMA2X2_INT_DATA_SEL_REG

#define BMA2X2_EN_INT1_PAD_FWM__POS     1
#define BMA2X2_EN_INT1_PAD_FWM__LEN     1
#define BMA2X2_EN_INT1_PAD_FWM__MSK     0x02
#define BMA2X2_EN_INT1_PAD_FWM__REG     BMA2X2_INT_DATA_SEL_REG

#define BMA2X2_EN_INT1_PAD_FFULL__POS     2
#define BMA2X2_EN_INT1_PAD_FFULL__LEN     1
#define BMA2X2_EN_INT1_PAD_FFULL__MSK     0x04
#define BMA2X2_EN_INT1_PAD_FFULL__REG     BMA2X2_INT_DATA_SEL_REG



#define BMA2X2_FIFO_DATA_SELECT__POS                 0
#define BMA2X2_FIFO_DATA_SELECT__LEN                 2
#define BMA2X2_FIFO_DATA_SELECT__MSK                 0x03
#define BMA2X2_FIFO_DATA_SELECT__REG                 BMA2X2_FIFO_MODE_REG

#define BMA2X2_FIFO_TRIGGER_SOURCE__POS                 2
#define BMA2X2_FIFO_TRIGGER_SOURCE__LEN                 2
#define BMA2X2_FIFO_TRIGGER_SOURCE__MSK                 0x0C
#define BMA2X2_FIFO_TRIGGER_SOURCE__REG                 BMA2X2_FIFO_MODE_REG

#define BMA2X2_FIFO_TRIGGER_ACTION__POS                 4
#define BMA2X2_FIFO_TRIGGER_ACTION__LEN                 2
#define BMA2X2_FIFO_TRIGGER_ACTION__MSK                 0x30
#define BMA2X2_FIFO_TRIGGER_ACTION__REG                 BMA2X2_FIFO_MODE_REG

#define BMA2X2_FIFO_MODE__POS                 6
#define BMA2X2_FIFO_MODE__LEN                 2
#define BMA2X2_FIFO_MODE__MSK                 0xC0
#define BMA2X2_FIFO_MODE__REG                 BMA2X2_FIFO_MODE_REG


#define BMA2X2_REGV_BW_BASE          0x08
#define BMA2X2_REGV_BW_7_81HZ        0x08
#define BMA2X2_REGV_BW_15_63HZ       0x09
#define BMA2X2_REGV_BW_31_25HZ       0x0A
#define BMA2X2_REGV_BW_62_50HZ       0x0B
#define BMA2X2_REGV_BW_125HZ         0x0C
#define BMA2X2_REGV_BW_250HZ         0x0D
#define BMA2X2_REGV_BW_500HZ         0x0E
#define BMA2X2_REGV_BW_1000HZ        0x0F

#define BMA2X2_BW_FILTER_SIZE   4

#define BMA2X2_EN_SOFT_RESET_VALUE        0xB6
#define BMA2X2_ACCEL_ENABLE_SLOPE_XYZ_VALUE        7
#define BMA2X2_ACCEL_ENABLE_DATA_READY_VALUE        0x10

#define BMA2X2_MAP_SLOPE_INT_TO_INT1         4
#define BMA2X2_MAP_DR_INT_TO_INT1         1

#define PAD_LOWG                    0
#define PAD_HIGHG                   1
#define PAD_SLOP                    2
#define PAD_DOUBLE_TAP                  3
#define PAD_SINGLE_TAP                  4
#define PAD_ORIENT                  5
#define PAD_FLAT                    6
#define PAD_SLOW_NO_MOTION              7



#define BOSCH_ACCEL_SENSOR_TAP_DUR  7
#define BOSCH_ACCEL_SENSOR_TAP_SHOCK  0
#define BOSCH_ACCEL_SENSOR_TAP_QUIET  0
#define BOSCH_ACCEL_SENSOR_TAP_TH  0x0A





#define BOSCH_ACCEL_SENSOR_RESET_INT      0x80

#define BOSCH_ACCEL_SENSOR_SLEEP_MODE           0x80
#define BOSCH_ACCEL_SENSOR_NORMAL_MODE          0

#define SNS_DD_ACCEL_MAX_RANGE                  4
#define SNS_DD_ACCEL_MAX_BW                  8

#define BOSCH_ACCEL_SENSOR_RANGE_2G             3
#define BOSCH_ACCEL_SENSOR_RANGE_4G             5
#define BOSCH_ACCEL_SENSOR_RANGE_8G             8
#define BOSCH_ACCEL_SENSOR_RANGE_16G            12


/*! BOSCH Accelerometer Sensor defines */
#define BOSCH_ACCEL_SENSOR_I2C_ADDR             0x70
#define BOSCH_ACCEL_SENSOR_ID_REG_ADDR          0x00




#define BOSCH_ACCEL_SENSOR_TEMP_REG_ADDR        0x08
#define BOSCH_ACCEL_SENSOR_TEMP_DEG_PER_LSB     1.0        /*! 1.0 LSB */
#define BOSCH_ACCEL_SENSOR_TEMP_OFFSET_DEG      24        /*! 0 =24C */
#define BOSCH_ACCEL_SENSOR_Z_MSB_REG_ADDR       0x07
#define BOSCH_ACCEL_SENSOR_Z_LSB_REG_ADDR       0x06
#define BOSCH_ACCEL_SENSOR_Z_STATUS_REG_ADDR    0x06
#define BOSCH_ACCEL_SENSOR_Y_MSB_REG_ADDR       0x05
#define BOSCH_ACCEL_SENSOR_Y_LSB_REG_ADDR       0x04
#define BOSCH_ACCEL_SENSOR_Y_STATUS_REG_ADDR    0x04
#define BOSCH_ACCEL_SENSOR_X_MSB_REG_ADDR       0x03
#define BOSCH_ACCEL_SENSOR_X_LSB_REG_ADDR       0x02
#define BOSCH_ACCEL_SENSOR_LSB_MASK             (0x80 | 0x40)  /*! (B7|B6) */
#define BOSCH_ACCEL_SENSOR_X_STATUS_REG_ADDR    0x02
#define BOSCH_ACCEL_SENSOR_NEW_DATA_MASK        0x01     /*! B0 */
#define BOSCH_ACCEL_SENSOR_GAIN_T_REG_ADDR      0x19
#define BOSCH_ACCEL_SENSOR_GAIN_T_MASK          0x3F    /*! bit 5:0 */
#define BOSCH_ACCEL_SENSOR_LSB_T_OFFSET_MASK    0x03    /*! bits 1:0 */
#define BOSCH_ACCEL_SENSOR_LSB_T_OFFSET_L_SHIFT 6        /*! in bits 7:6 */
#define BOSCH_ACCEL_SENSOR_OFFSET_T_REG_ADDR    0x1d
#define BOSCH_ACCEL_SENSOR_MSB_T_OFFSET_MASK    0x3fc    /*! bit 9:2 */
#define BOSCH_ACCEL_SENSOR_MSB_T_OFFSET_R_SHIFT 2        /*! in bits 7:0 */

/*! Min/Max accel in milli-g for different range */
#define BOSCH_ACCEL_SENSOR_RANGE_2G_MAX         FX_FLTTOFIX_Q16(2*G)
#define BOSCH_ACCEL_SENSOR_RANGE_2G_MIN         FX_FLTTOFIX_Q16(-2*G)
#define BOSCH_ACCEL_SENSOR_RANGE_4G_MAX         FX_FLTTOFIX_Q16(4*G)
#define BOSCH_ACCEL_SENSOR_RANGE_4G_MIN         FX_FLTTOFIX_Q16(-4*G)
#define BOSCH_ACCEL_SENSOR_RANGE_8G_MAX         FX_FLTTOFIX_Q16(8*G)
#define BOSCH_ACCEL_SENSOR_RANGE_8G_MIN         FX_FLTTOFIX_Q16(-8*G)
#define BOSCH_ACCEL_SENSOR_RANGE_16G_MAX         FX_FLTTOFIX_Q16(16*G)
#define BOSCH_ACCEL_SENSOR_RANGE_16G_MIN         FX_FLTTOFIX_Q16(-16*G)
#define BOSCH_ACCEL_SENSOR_TEMPER_MIN           FX_FLTTOFIX_Q16(-40)
#define BOSCH_ACCEL_SENSOR_TEMPER_MAX           FX_FLTTOFIX_Q16(97.5)




#define BOSCH_ACCEL_SENSOR_BW_VALUE_7_81Hz      FX_FLTTOFIX_Q16(7.81)
#define BOSCH_ACCEL_SENSOR_BW_VALUE_15_63Hz     FX_FLTTOFIX_Q16(15.63)
#define BOSCH_ACCEL_SENSOR_BW_VALUE_31_25Hz     FX_FLTTOFIX_Q16(31.25)
#define BOSCH_ACCEL_SENSOR_BW_VALUE_62_50Hz     FX_FLTTOFIX_Q16(62.5)
#define BOSCH_ACCEL_SENSOR_BW_VALUE_125Hz       FX_FLTTOFIX_Q16(125)
#define BOSCH_ACCEL_SENSOR_BW_VALUE_250Hz       FX_FLTTOFIX_Q16(250)
#define BOSCH_ACCEL_SENSOR_BW_VALUE_500Hz       FX_FLTTOFIX_Q16(500)
#define BOSCH_ACCEL_SENSOR_BW_VALUE_1000Hz      FX_FLTTOFIX_Q16(1000)




/*! Resolution for different range in milli-g for accel or 1 deg C for temp */
#define BOSCH_ACCEL_SENSOR_RANGE_2G_RES         4
#define BOSCH_ACCEL_SENSOR_RANGE_4G_RES         8
#define BOSCH_ACCEL_SENSOR_RANGE_8G_RES         16
#define BOSCH_ACCEL_SENSOR_TEMPER_RES           5

#define BOSCH_ACCEL_SENSOR_ACC_BIT_LEN          10
#define BOSCH_ACCEL_SENSOR_ACC_CONV_TIME        333
#define BOSCH_ACCEL_SENSOR_TEMPER_BIT_LEN       8
#define BOSCH_ACCEL_SENSOR_TEMPER_CONV_TIME     333

#define BOSCH_ACCEL_MIN_RAW_DATA_VALUE 0xFFFFFE00
#define BOSCH_ACCEL_MAX_RAW_DATA_VALUE 0x1FF
#define BOSCH_TEMP_MAX_RAW_DATA_VALUE 0xFF
#define BOSCH_TEMP_MIN_RAW_DATA_VALUE 0x0


/*=======================================================================

  Driver specific configurations

  ========================================================================*/

#define BMA2X2_DEV_SEL_NUM_BMI058 0x58
#define BMA2X2_DEV_SEL_NUM_BMI055 0x55


#ifndef BMA2X2_CONFIG_WMI_TRIGGER_LEVEL
#define BMA2X2_CONFIG_WMI_TRIGGER_LEVEL  0
#endif

#define BMA2X2_CONFIG_FIFO_LOG           1


#define BMA2X2_CONFIG_INT_DET_RISING_EDGE_THRES_US      50

#define BMA2X2_IRQ_TRIGGER_TYPE_DEFAULT SNS_DDF_SIGNAL_IRQ_RISING

#define BMA2X2_DBT_MIN_ODR               200
#define BMA2X2_ACC_MAX_ODR               200

#define BMA2X2_MD_DUR                    0
#define BMA2X2_MD_THRESHOLD              16

#define BMA2X2_SOFTRESET2READY_DELAY     3000 /* us */

/*=======================================================================

  Preprocessor Definitions and Constants

  ========================================================================*/

#define BMA2X2_GET_BITSLICE(regvar, bitname)\
    ((regvar & bitname##__MSK) >> bitname##__POS)


#define BMA2X2_SET_BITSLICE(regvar, bitname, val)\
    ((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))


#define abs(x) ((x) < 0 ? -(x):(x))
#define max(a, b) ((a)>(b) ? (a) : (b))



/*=======================================================================

  STRUCTURE AND TYPEDEFS

  ========================================================================*/
struct bma2x2_data
{
    int16_t x;
    int16_t y;
    int16_t z;
};

/* Supported datatypes */

typedef enum
{
    SDD_ACC_X = 0,
    SDD_ACC_Y,
    SDD_ACC_Z,
    SDD_ACC_NUM_AXIS,
    SDD_TEMP = SDD_ACC_NUM_AXIS,
    SDD_DOUBLE_TAP,
    SDD_ACC_NUM_DATATYPES
} sdd_acc_data_type_e;


enum DD_DATA_LOG_CONTEXT {
    DD_DATA_LOG_CONTEXT_POLL = 0,
    DD_DATA_LOG_CONTEXT_DRI,
    DD_DATA_LOG_CONTEXT_FIFO_WMI,
    DD_DATA_LOG_CONTEXT_FIFO_FLUSH,
    DD_DATA_LOG_CONTEXT_FIFO_WMI_IMITATED,
};

/**
 * This struct is used to represent the head of the singly-linked list that
 * contains all device instances related to this device driver.
 */
typedef struct
{
    sns_ddf_dev_list_t     next;              // The next device instance
    sns_ddf_driver_info_s  dd_info;           // Device driver info
} sns_acc_dd_bma2x2_head_s;


/**
 * This struct represents a single sensor type that is contained within a combo
 * device. (e.g. ACCEL, TEMP, DOUBLETAP, etc.)
 */
typedef struct
{
    sns_ddf_sensor_e  sensor_type;  // Sensor ID based off of the sns_ddf_sensor_e enum


#if 0
    uint32_t          odr_req;      // The requested ODR (0, if there is no output requested for this sensor type)
    uint32_t          odr_curr;     // The current ODR for this sensor

    uint32_t          odr_min;     // The current ODR for this sensor
    uint32_t          odr_max;     // The current ODR for this sensor

    /*! if this sensor is enabled or not */
    uint8_t           en            :1;

    uint8_t           range;        // The full-scale range index. (only supported for gyro and accel)
    uint8_t           num_axis;     // Number of axes for this device. Also the size of @biases.

    q16_t             biases[SDD_ACC_NUM_AXIS];
#endif
} bma2x2_sensor_t;

/**
 * This struct represents a single sub-device that is contained within a combo
 * device. (e.g. ACCEL/TEMP,  DOUBLETAP, etc.)
 */
/* import the type */
typedef struct {
    /*! backward reference of parent device instance(i.e.: state) */
    void                            *private_data;
    /*! SMGR handle used to notify async events from this sub-device */
    sns_ddf_handle_t                smgr_handle;
    uint16_t                        num_sensors;

    /* for sensors within the same sub_dev */
    bma2x2_sensor_t                 sensors[BMA2X2_MAX_NUM_SENSORS];
} bma2x2_sub_dev_t;


/*!  state struct for BMA2X2 driver */
typedef struct {
    /*! pointer to next device instance in the device chain */
    sns_ddf_dev_list_t      next;
    sns_ddf_port_id_s       port_id;

    /*! handle used to access the (I2C) bus */
    sns_ddf_handle_t        port_handle;
    /*! GPIO number used for interrupts */
    uint32_t                gpio_num;
    uint32_t                dev_select;
    /*! customization and fine-tuning parameters*/
    struct {
        struct bst_cust_param_bma param;
        uint8_t         nvm_param_avail :1;
    } cust;

    /* BMA280, BMA255, BMA250E, BMA222E */
    uint8_t                 sub_model;
    /*! current range selection idx: starting from 0 */
    uint8_t                 range_idx;
    /*! current range selection: register setting */
    uint8_t                 range;
    /*! current lowpass filter BW */
    uint16_t     lowpass_bw;
    uint16_t     bw_idx;

    /*! True if the bw_timer is still pending, false otherwise
     *    If true, data is invalid (caused by BW changes)
     *    If false data is valid */
    bool                            bw_timer_pending;
    sns_ddf_timer_s                 bw_timer;


    /*! data cache */
    q16_t                   data_cache[SDD_ACC_NUM_DATATYPES];
    q16_t                   data_cache_fifo[SDD_ACC_NUM_DATATYPES];
    /*! time of last update */
    sns_ddf_time_t          data_cache_time;
    sns_ddf_status_e        data_status[SDD_ACC_NUM_DATATYPES];

    /*! used when notifying the data to SMGR */
    sns_ddf_sensor_data_s   sensor_data;

    uint8_t                 regv_int_map_1;
    uint8_t                 regv_fifo_config_1;

    /*! FIFO */
    /*! cache used when notifying the data to SMGR */
    sns_ddf_sensor_data_s   f_frames_cache;

    /* water mark level */
    uint8_t                 f_wml;

    uint8_t                 f_wml_pending;

    uint32_t                f_acnt_frame_cnt;
    uint32_t                f_acnt_frame_cnt_irq;

    sns_ddf_time_t          ts_irq;
    sns_ddf_time_t          ts_irq_last_fwml;

    sns_ddf_time_t          ts_irq_last_drdy;

    sns_ddf_time_t          ts_itvl_ideal;
    sns_ddf_time_t          ts_itvl_est;

    sns_ddf_time_t          ts_last_sample;

    sns_ddf_time_t          ts_1st_sample_est;


    uint32_t                avail_accurate_ts_last_sample :1;


    uint32_t                md_lpm          :1;
    uint32_t                en_md           :1;
    uint32_t                en_db_tap_int   :1;
    uint32_t                en_dri          :1;
    uint32_t                en_fifo_int     :1;
    uint32_t                en_fifo_int_orig            :1;

    uint32_t                soft_reset:     1;
    uint32_t                test_init:      1;

    uint32_t                f_overrun_detected_in_irq   :1;

    uint32_t                f_wml_cfg_pending :1;


    q16_t                   bias[SDD_ACC_NUM_AXIS];

    sns_ddf_axes_map_s      axes_map;
    sns_dd_bma2x2_tap_settings_s tap_settings;

    int8_t                  sample_cnt_to_skip_odrc;

    bma2x2_sub_dev_t        *sub_dev_cache_acc;

    sns_ddf_time_t          itvl_drdy_tolerance;

    sns_ddf_odr_t odr_hw;
    /*! Current accel sampling frequency. */
    sns_ddf_odr_t acc_cur_rate;

    /*! desired accel sensor sampling frequency. */
    sns_ddf_odr_t acc_desired_rate;

    /*! Current double tap sensor sampling frequency. */
    sns_ddf_odr_t dbt_cur_rate;

    /*! desired double tap sensor sampling frequency. */
    sns_ddf_odr_t dbt_desired_rate;

    sns_ddf_odr_t tmp_cur_rate;
    /*! for the combo device driver */
    uint8_t                 sub_dev_cnt;
    bma2x2_sub_dev_t        sub_dev[BMA2X2_MAX_SUB_DEV];
} sns_dd_acc_bma2x2_state_t;


extern sns_ddf_status_e bma2x2_smbus_read_byte(
        sns_ddf_handle_t port_handle,
        unsigned char reg_addr,
        unsigned char *data);

extern sns_ddf_status_e bma2x2_smbus_read_byte_block(
        sns_ddf_handle_t port_handle,
        unsigned char reg_addr,
        unsigned char *data,
        unsigned char len);

extern sns_ddf_status_e bma2x2_smbus_write_byte(
        sns_ddf_handle_t port_handle,
        unsigned char reg_addr,
        unsigned char *data);

extern sns_ddf_status_e bma2x2_set_int1_pad_sel(
        sns_ddf_handle_t port_handle,
        unsigned char int1sel, int enable);

extern sns_dd_acc_bma2x2_state_t* sns_dd_acc_bma2x2_get_dev(
        sns_ddf_handle_t dd_handle);

extern bma2x2_sub_dev_t* sns_dd_acc_bma2x2_get_subdev_from_type(
        sns_dd_acc_bma2x2_state_t *state,
        const sns_ddf_sensor_e       sensor_type);

extern sns_ddf_status_e sns_dd_acc_bma2x2_config_motion_int(
        sns_dd_acc_bma2x2_state_t  *state,
        bool                       enable);

extern sns_ddf_status_e bma2x2_read_accel_x(
        sns_ddf_handle_t port_handle,
        uint8_t sensor_type,
        short *a_x);

extern sns_ddf_status_e bma2x2_read_accel_y(
        sns_ddf_handle_t port_handle,
        uint8_t sensor_type,
        short *a_y);

extern sns_ddf_status_e bma2x2_read_accel_z(
        sns_ddf_handle_t port_handle,
        uint8_t sensor_type,
        short *a_z);
extern
sns_ddf_status_e bma2x2_fifo_data_read_out_frames(
        sns_dd_acc_bma2x2_state_t * state,
        uint8_t f_count,
        bool    f_overrun,
        bool    in_irq);

extern
sns_ddf_status_e bma2x2_get_fifo_framecount(
        sns_ddf_handle_t dd_handle,
        uint8_t *framecount,
        uint8_t *overrun);
extern
void sns_dd_accel_log_fifo(
        sns_ddf_sensor_data_s           *accel_data_ptr,
        enum DD_DATA_LOG_CONTEXT        context);

    static inline
bool sns_dd_bma2x2_itvl_within_accuracy(
        sns_ddf_time_t itvl_expected,
        sns_ddf_time_t itvl_est)
{
    sns_ddf_time_t err;

    if (itvl_expected > itvl_est) {
        err = itvl_expected - itvl_est;
    } else {
        err = itvl_est - itvl_expected;
    }

    return (100 * err) <= itvl_expected * BMA2X2_CONFIG_TS_ERR_TOLERANCE_PERCENT;
}

    static inline
sns_ddf_time_t sns_dd_bms_get_time_elapse_sys(
        sns_ddf_time_t start,
        sns_ddf_time_t end)
{
    sns_ddf_time_t elapse;

    if (end >= start) {
        elapse = end - start;
    } else {
        elapse = (sns_ddf_time_t)(-1) - end + start + 1;
    }

    return elapse;
}

static inline
sns_ddf_time_t sns_dd_bms_get_time_deviation(
        sns_ddf_time_t a,
        sns_ddf_time_t b)
{
    if (a > b) {
        return a - b;
    } else {
        return b - a;
    }
}

    static inline
bool sns_dd_bms_time_not_before(
        sns_ddf_time_t a,
        sns_ddf_time_t b)
{
    sns_ddf_time_t h;
    h = (((sns_ddf_time_t)(-2)) >> 1) + 1;

    return ((a + h) >= (b + h));
}

#endif /*! End include guard  SNSDACCELPRIV_H */

