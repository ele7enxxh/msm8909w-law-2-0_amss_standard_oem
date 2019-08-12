/*  Date: 2015/06/15 23:12:00
 *  Revision: 2.7.9
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


/*==============================================================================

  S E N S O R S   A C C E L E R O M E T E R    D R I V E R

  DESCRIPTION

  Impelements the driver for the accelerometer driver

  EDIT HISTORY FOR FILE


  version when         who      what
  ----------   ---     -----------------------------------------------------------
  08/07/12     Albert  basic values, bandwidth, g range, power mode, motion detect interrupt
  08/22/12     Albert  self test
  03/06/13     Albert  temperature
  04/09/13     Albert  DRI support
  06/20/13     Albert  add interrupt for low G , high G , no motion, single tap, double tap, orientation, flat.
  07/13/13     Albert  modify to combo sensor ddf driver with double tap sensor.
  07/12/13     lk      first working version of hcombo dev driver with double-tap, v2.0-pre for baseline of code clean up
  07/25/13     Albert  design ODR and power management
  07/30/13     Albert  Remap from NVM, update reset function with ODR, update self test
  08/06/13     Albert  FIFO support
  09/18/12     AG      Lower interrupt threshold
  09/18/13     AG      Fix axes conversion for 8x26 MTP temporarily until nv_params is used
  09/02/13     AG/SD   Add 14C1 logs
  09/18/13     VH      Interchange X & Y axis values in sns_dd_accel_log_data
  09/18/13     PS      Fix OEM selftest
  09/18/13     an      Probe function Update.
  09/18/13     tc      Adjusted the delay after bandwidth selection to use the Update Times specified in the BMA222E datasheet
  09/18/13     hw      Read only requested data on bma2x2
  09/18/13     hw      Fix the Klocwork error
  10/07/13     sd      corrected data axis inconsistency between polling and fifo mode
  10/16/13     sd      Fixed the Klocwork error
  10/16/13     sd      Fixed the Klocwork error
  12/11/13     albert  add wa for pm
  11/04/13     albert  add support for bmi058
  11/04/13     albert  fix OEM test notify state issue
  11/15/13     tc      Completed support for temperature sensor
  02/25/14     MW      Updated version from Bosch with fix in self test and fix for Klocwork report
  03/17/14     lk      code merge of changes from qcm and bst,
  03/17/14     lk      dynamically register/deregister signal handler, update the version information
  03/18/14     albert  interval review by bst after code merge
  03/19/14     lk      fix an CR reported about resolution
  03/24/14     lk      use dev_select to handle chips variations
  04/04/l4     lk      heavy testing performed
  04/11/14     lk      removed an uncessary delay
  04/11/14     lk      add automatic detection of BMI058
  04/17/14     lk      set range to +/-4G at reset
  05/01/14     lk      add SNS_DDF_TEST_SELF handling
  05/30/14     MW      Updated DT and accel max_rate to 200Hz
  06/04/14     lk      add fail safe for FIFO f_count read out
  06/04/14     lk      reset(): set mode to suspend mode
  07/06/14     vy      refactored to support uImage
  07/27/14     vy      Replaced "div of two constants" by another constants
  08/24/14     vy      Used new DDF apis to allocate/free mem supported in uImage
  08/21/lk     lk      2.5.7: add support for ossc
  08/21/lk     lk      2.6.0: use LPM for motion detection
  08/21/lk     lk      2.6.1: use bma2x2_smbus_write_byte() wrapper instead of sns_ddf_write_port()
  08/27/lk     lk      2.6.2: fix error in register address for sns_dd_acc_bma2x2_config_any_motion_lpm1()
  08/27/lk     lk      2.6.3: remove get_out_of_mdlpm()
  08/29/lk     lk      2.6.4: code clean up for MOTION_DETECT in LPM1
  08/29/lk     lk      2.6.5: fortify interrupt handling when using EDGE_TRIGGERING
  09/02/14     lk      2.6.6: sns_ddf_signal_register() before map/enable INT1 in enable_sched_data()
  09/09/14     sd      2.6.7: reduced MD threshold in low power mode
  09/17/14     vy      2.6.8: Updated memhandler api in probe fn
  09/18/14     MW      2.6.9: increased MD dur and reduced sleep duration
  09/24/14     MW      2.6.10: remove redundant MD config
  10/09/14     lk      2.6.11: Update motion config to address gentle pick up
  10/15/14     lk      2.6.12: mark sample as invalid when needed
  10/15/14     lk      2.6.13: fix the sequence with signal_deregister
  10/16/14     lk      2.6.14: flush data before ODR changes
  10/26/14     lk      2.6.15: updated number of invalid samples after changing ODR
  10/26/14     lk      2.6.16: log fine tuning
  11/15/14     lk      2.6.17: changes bw setting function so it's easier for FIFO ts handling
  11/15/14     lk      2.6.18: timestamp fixes and improvements in update_sample_ts()
  11/17/14     lk      2.6.19: fix for the get_attr(ATTR_BIAS)
  11/26/14     lk      2.6.20: corner case coverage for potential timestamp issue in trigger_fifo_data()
  12/01/14     lk      2.6.21: fix compile warnings and errors on customer platform
  12/11/14     lk      2.6.22: fix timing drift when wml == 1
  12/16/14     sd      2.6.23  added timestamp check for all scenarios
  01/21/15     BD      move read_accel_x read_accel_y read_accel_z from uImage to bigImage
  02/02/15     sd      2.6.24  frame count recheck
  02/08/15     lk      2.6.25  add FIFO WMI imitation by DRI
  02/09/15     lk      2.7     add FIFO WMI imitation by DRI
  02/09/15     lk      2.7.1   changed the log format for easier debugging
  02/14/15     lk      2.7.2   added anti_false_irq logic
  02/14/15     lk      2.7.3   improvement of jitter between batches
  02/24/15     lk      2.7.4   reset FIFO when FIFO overflows in trigger_fifo_data()
  02/26/15     lk      2.7.5   fix some errors found in code review
  04/14/15     lk      2.7.6   add CHECK of the f_count in fifo_flush() to avoid overflow
  06/03/15     lk      2.7.8   delay cfg of WML
  06/15/15     lk      2.7.9   add BMA2X2_CONFIG_ENABLE_INT_MAP_2
==============================================================================*/

/*============================================================================

                      INCLUDE FILES

============================================================================*/
#include "sns_dd_bma2x2.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define BMA2X2_SNS_DDF_ATTRIB_VERSION   2070902

/*=======================================================================

                                  INTERNAL FUNCTION PROTOTYPES

========================================================================*/

/*! forward declarations of global/static variables */
extern sns_ddf_driver_if_s sns_accel_bma2x2_driver_fn_list;


/*
static bma2x2_sensor_t *sns_dd_acc_bma2x2_get_sensor_from_type(
bma2x2_sub_dev_t                *sub_dev,
const sns_ddf_sensor_e          sensor_type);
*/

static sns_ddf_status_e sns_dd_acc_bma2x2_config_motion_detect_thresh(
    sns_dd_acc_bma2x2_state_t  *state);

#if !BMA2X2_CONFIG_ENABLE_MD_LPM
static
sns_ddf_status_e sns_dd_acc_bma2x2_config_motion_detect_duration(
    sns_dd_acc_bma2x2_state_t  *state);
#endif

static sns_ddf_status_e bma2x2_set_fifo_mode(sns_ddf_handle_t dd_handle, unsigned char
        fifo_mode);



                /* !global variables */
static const uint32_t BMA2X2_ACC_RANGE_LIST[4] =
{
    BOSCH_ACCEL_SENSOR_RANGE_2G,
    BOSCH_ACCEL_SENSOR_RANGE_4G,
    BOSCH_ACCEL_SENSOR_RANGE_8G,
    BOSCH_ACCEL_SENSOR_RANGE_16G
};


static const uint32_t  sns_dd_accel_bw_update_time_us[8] =
{
    64000, // BMA2X2_REGV_BW_7_81HZ
    32000, // BMA2X2_REGV_BW_15_63HZ
    16000, // BMA2X2_REGV_BW_31_25HZ
    8000,  // BMA2X2_REGV_BW_62_50HZ
    4000,  // BMA2X2_REGV_BW_125HZ
    2000,  // BMA2X2_REGV_BW_250HZ
    1000,  // BMA2X2_REGV_BW_500HZ
    500    // BMA2X2_REGV_BW_1000HZ
};

static const uint32_t BMA2X2_ACC_FREQ_LIST[8] =
{
    BOSCH_ACCEL_SENSOR_BW_VALUE_7_81Hz,
    BOSCH_ACCEL_SENSOR_BW_VALUE_15_63Hz,
    BOSCH_ACCEL_SENSOR_BW_VALUE_31_25Hz,
    BOSCH_ACCEL_SENSOR_BW_VALUE_62_50Hz,
    BOSCH_ACCEL_SENSOR_BW_VALUE_125Hz,
    BOSCH_ACCEL_SENSOR_BW_VALUE_250Hz,
    BOSCH_ACCEL_SENSOR_BW_VALUE_500Hz,
    BOSCH_ACCEL_SENSOR_BW_VALUE_1000Hz
};

extern const uint32_t BMA2X2_LSB_RANGE_LIST[4];

extern float BMA2x2_G_OVER_RANGE_LIST[4];


static struct bst_val_pair BMA2X2_MAP_BW_ODR[] =
{
    {
        .l = 16,
        .r = BMA2X2_REGV_BW_7_81HZ
    },

    {

        .l = 32,
        .r = BMA2X2_REGV_BW_15_63HZ
    },

    {
        .l = 63,
        .r = BMA2X2_REGV_BW_31_25HZ
    },

    {
        .l = 125,
        .r = BMA2X2_REGV_BW_62_50HZ
    },

    {
        .l = 250,
        .r = BMA2X2_REGV_BW_125HZ
    },

    {
        .l = 500,
        .r = BMA2X2_REGV_BW_250HZ
    },

    {
        .l = 1000,
        .r = BMA2X2_REGV_BW_500HZ
    },

    {
        .l = 2000,
        .r = BMA2X2_REGV_BW_1000HZ
    },
};


struct bms_accel_any_motion_setting {
    //power in unit of uA
    uint8_t         pwr;
    //sensitivity to user motion
    uint8_t         sensitivity;

    uint16_t        odr;

    //regv
    uint16_t        bw;


    //regv
    uint8_t         sleep_dur;

    //regv
    uint8_t         dur;
    //regv
    uint8_t         threshold;
};


const struct bms_accel_any_motion_setting BMA2X2_ANY_MOTION_PRESETS_LP1 [] = {
    {
        .pwr            = 5,
        .sensitivity    = 2,

        .odr            = 2000,
        .bw             = 0x0f,
        //50ms
        .sleep_dur      = 0x0c,
        .dur            = 0,
        .threshold      = 16,
    },

    {
        .pwr            = 9,
        .sensitivity    = 3,

        .odr            = 2000,
        .bw             = 0x0f,
        //25ms
        .sleep_dur      = 0x0b,
        .dur            = 0,
        .threshold      = 16,
    },

    {
        // power measured on QC platform was close to 57uA
        .pwr            = 57,
        .sensitivity    = 3,

        .odr            = 32,
        // BW = 16Hz
        .bw             = 0x09,
        // sleep duration in LP1 mode = 0ms
        .sleep_dur      = 0x00,
        // use at least 1 sample to determine MD
        .dur            = 0,
        // threshold for 4G range = (4 >> 1)*7.81mg = 15.62mg
        .threshold      = 2,
    },

};


static sns_acc_dd_bma2x2_head_s bma2x2_sns_dd_list_head = {
    NULL,
    {"BOSCH BMA2X2 Driver", BMA2X2_SNS_DDF_ATTRIB_VERSION}
};

/*! function implementations */

static sns_ddf_status_e bma2x2_set_powerstate(
        sns_dd_acc_bma2x2_state_t *state,
        int powerstate)
{
    sns_ddf_status_e status;
    uint8_t byte_reg;
    uint8_t out;
    uint8_t fifo_cfg = 0x0c;


    if ((status = sns_ddf_read_port(state->port_handle,
                    BMA2X2_MODE_CTRL_REG,
                    &byte_reg,
                    1,
                    &out)) != SNS_DDF_SUCCESS)
    {
        return status;
    }

    if (SNS_DDF_POWERSTATE_LOWPOWER == powerstate) {
        if ((status = sns_ddf_read_port(state->port_handle,
                        BMA2X2_FIFO_MODE_REG,
                        &fifo_cfg,
                        1,
                        &out)) != SNS_DDF_SUCCESS) {
            return status;
        }
    }

    switch (powerstate)
    {
        case SNS_DDF_POWERSTATE_ACTIVE:
            byte_reg = BST_CLR_VAL_BIT(byte_reg, 7);
            byte_reg = BST_CLR_VAL_BIT(byte_reg, 6);
            byte_reg = BST_CLR_VAL_BIT(byte_reg, 5);
            break;
        case SNS_DDF_POWERSTATE_LOWPOWER:
            byte_reg = BST_SET_VAL_BIT(byte_reg, 7);
            byte_reg = BST_CLR_VAL_BIT(byte_reg, 6);
            byte_reg = BST_CLR_VAL_BIT(byte_reg, 5);

            byte_reg = BST_SET_VAL_BITBLOCK(byte_reg, 1, 4, 0);
            break;
        default:
            return SNS_DDF_EINVALID_PARAM;
    }

    /*! Set opmode */
    if ((status = bma2x2_smbus_write_byte(state->port_handle,
                    BMA2X2_MODE_CTRL_REG,
                    &byte_reg)) != SNS_DDF_SUCCESS)
    {
        return status;
    }

    sns_ddf_delay(500);

    if (SNS_DDF_POWERSTATE_LOWPOWER == powerstate) {
        if ((status = bma2x2_smbus_write_byte(state->port_handle,
                        BMA2X2_FIFO_MODE__REG,
                        &fifo_cfg)) != SNS_DDF_SUCCESS) {
            return status;
        }
    }

    return status;
}


static sns_ddf_status_e bma2x2_enable_int(
        sns_ddf_handle_t port_handle,
        unsigned char int_type,
        unsigned char enable)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data1, data2;

    if ((11 < int_type) && (int_type < 16)) {
        switch (int_type) {
            case 12:
                /* slow/no motion X Interrupt  */
                comres = bma2x2_smbus_read_byte(port_handle,
                        BMA2X2_INT_SLO_NO_MOT_EN_X_INT__REG, &data1);
                data1 = BMA2X2_SET_BITSLICE(data1,
                        BMA2X2_INT_SLO_NO_MOT_EN_X_INT, enable);
                comres = bma2x2_smbus_write_byte(port_handle,
                        BMA2X2_INT_SLO_NO_MOT_EN_X_INT__REG, &data1);
                break;
            case 13:
                /* slow/no motion Y Interrupt  */
                comres = bma2x2_smbus_read_byte(port_handle,
                        BMA2X2_INT_SLO_NO_MOT_EN_Y_INT__REG, &data1);
                data1 = BMA2X2_SET_BITSLICE(data1,
                        BMA2X2_INT_SLO_NO_MOT_EN_Y_INT, enable);
                comres = bma2x2_smbus_write_byte(port_handle,
                        BMA2X2_INT_SLO_NO_MOT_EN_Y_INT__REG, &data1);
                break;
            case 14:
                /* slow/no motion Z Interrupt  */
                comres = bma2x2_smbus_read_byte(port_handle,
                        BMA2X2_INT_SLO_NO_MOT_EN_Z_INT__REG, &data1);
                data1 = BMA2X2_SET_BITSLICE(data1,
                        BMA2X2_INT_SLO_NO_MOT_EN_Z_INT, enable);
                comres = bma2x2_smbus_write_byte(port_handle,
                        BMA2X2_INT_SLO_NO_MOT_EN_Z_INT__REG, &data1);
                break;
            case 15:
                /* slow / no motion Interrupt select */
                comres = bma2x2_smbus_read_byte(port_handle,
                        BMA2X2_INT_SLO_NO_MOT_EN_SEL_INT__REG, &data1);
                data1 = BMA2X2_SET_BITSLICE(data1,
                        BMA2X2_INT_SLO_NO_MOT_EN_SEL_INT, enable);
                comres = bma2x2_smbus_write_byte(port_handle,
                        BMA2X2_INT_SLO_NO_MOT_EN_SEL_INT__REG, &data1);
        }

        return comres;
    }

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_INT_ENABLE1_REG, &data1);
    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_INT_ENABLE2_REG, &data2);

    enable = enable & 1;
    switch (int_type) {
        case 0:
            /* Low G Interrupt  */
            data2 = BMA2X2_SET_BITSLICE(data2, BMA2X2_EN_LOWG_INT, enable);
            break;
        case 1:
            /* High G X Interrupt */

            data2 = BMA2X2_SET_BITSLICE(data2, BMA2X2_EN_HIGHG_X_INT,
                    enable);
            break;
        case 2:
            /* High G Y Interrupt */

            data2 = BMA2X2_SET_BITSLICE(data2, BMA2X2_EN_HIGHG_Y_INT,
                    enable);
            break;
        case 3:
            /* High G Z Interrupt */

            data2 = BMA2X2_SET_BITSLICE(data2, BMA2X2_EN_HIGHG_Z_INT,
                    enable);
            break;
        case 4:
            /* New Data Interrupt  */

            data2 = BMA2X2_SET_BITSLICE(data2, BMA2X2_EN_NEW_DATA_INT,
                    enable);
            break;
        case 5:
            /* Slope X Interrupt */

            data1 = BMA2X2_SET_BITSLICE(data1, BMA2X2_EN_SLOPE_X_INT,
                    enable);
            break;
        case 6:
            /* Slope Y Interrupt */

            data1 = BMA2X2_SET_BITSLICE(data1, BMA2X2_EN_SLOPE_Y_INT,
                    enable);
            break;
        case 7:
            /* Slope Z Interrupt */

            data1 = BMA2X2_SET_BITSLICE(data1, BMA2X2_EN_SLOPE_Z_INT,
                    enable);
            break;
        case 8:
            /* Single Tap Interrupt */

            data1 = BMA2X2_SET_BITSLICE(data1, BMA2X2_EN_SINGLE_TAP_INT,
                    enable);
            break;
        case 9:
            /* Double Tap Interrupt */

            data1 = BMA2X2_SET_BITSLICE(data1, BMA2X2_EN_DOUBLE_TAP_INT,
                    enable);
            break;
        case 10:
            /* Orient Interrupt  */

            data1 = BMA2X2_SET_BITSLICE(data1, BMA2X2_EN_ORIENT_INT, enable);
            break;
        case 11:
            /* Flat Interrupt */

            data1 = BMA2X2_SET_BITSLICE(data1, BMA2X2_EN_FLAT_INT, enable);
            break;

        case 16:
            /* Ffull Interrupt */

            data2 = BMA2X2_SET_BITSLICE(data2, BMA2X2_INT_FFULL_EN_INT, enable);
            break;

        case 17:
            /* Fwm Interrupt */

            data2 = BMA2X2_SET_BITSLICE(data2, BMA2X2_INT_FWM_EN_INT, enable);
            break;


        default:
            break;
    }
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_INT_ENABLE1_REG,
            &data1);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_INT_ENABLE2_REG,
            &data2);

    return comres;
}

static sns_ddf_status_e bma2x2_enable_fifo_wm_int(
        sns_ddf_handle_t    port_handle,
        int                 en)
{
    sns_ddf_status_e    status = SNS_DDF_SUCCESS;
    uint8_t             regv;

    regv = 0;
    status = bma2x2_smbus_read_byte(port_handle,
            BMA2X2_REGA_INT_EN_1, &regv);
    BMA2X2_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

    if (en) {
        regv = BST_SET_VAL_BIT(regv, 6);
    } else {
        regv = BST_CLR_VAL_BIT(regv, 6);
    }
    status = bma2x2_smbus_write_byte(port_handle,
            BMA2X2_REGA_INT_EN_1, &regv);
    BMA2X2_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

    regv = 0;
    status = bma2x2_smbus_read_byte(port_handle,
            BMA2X2_REGA_INT_MAP_1, &regv);
    BMA2X2_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

#if BMA2X2_CONFIG_ENABLE_INT_MAP_2
    if (en) {
        regv = BST_SET_VAL_BITBLOCK(regv, 5, 6, 3);
    } else {
        regv = BST_SET_VAL_BITBLOCK(regv, 5, 6, 0);
    }
#else
    if (en) {
        regv = BST_SET_VAL_BITBLOCK(regv, 1, 2, 3);
    } else {
        regv = BST_SET_VAL_BITBLOCK(regv, 1, 2, 0);
    }
#endif

    status = bma2x2_smbus_write_byte(port_handle,
            BMA2X2_REGA_INT_MAP_1, &regv);
    BMA2X2_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

    return status;
}


/*!
 *  @brief Configure (enable/disable) the data ready interrupt
 *
 *  @detail
 *  @param[in] state: Ptr to the driver structure
 *  @param[in] enable: If true enable the interrupt if false disable the
 *  interrupt
 *
 *  @return
 *   The error code definition within the DDF
 *   SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 *
 */
static sns_ddf_status_e bma2x2_enable_data_ready_int(
        sns_ddf_handle_t    port_handle,
        bool                enable)
{
    sns_ddf_status_e    status = SNS_DDF_SUCCESS;
    uint8_t             regv;

    regv = 0;
    status = bma2x2_smbus_read_byte(port_handle,
            BMA2X2_REGA_INT_EN_1, &regv);
    BMA2X2_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

    if (enable) {
        regv = BST_SET_VAL_BIT(regv, 4);
    } else {
        regv = BST_CLR_VAL_BIT(regv, 4);
    }
    status = bma2x2_smbus_write_byte(port_handle,
            BMA2X2_REGA_INT_EN_1, &regv);
    BMA2X2_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);


    regv = 0;
    status = bma2x2_smbus_read_byte(port_handle,
            BMA2X2_REGA_INT_MAP_1, &regv);
    BMA2X2_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

#if BMA2X2_CONFIG_ENABLE_INT_MAP_2
    if (enable) {
        regv = BST_SET_VAL_BIT(regv, 7);
    } else {
        regv = BST_CLR_VAL_BIT(regv, 7);
    }
#else
    if (enable) {
        regv = BST_SET_VAL_BIT(regv, 0);
    } else {
        regv = BST_CLR_VAL_BIT(regv, 0);
    }
#endif
    status = bma2x2_smbus_write_byte(port_handle,
            BMA2X2_REGA_INT_MAP_1, &regv);
    BMA2X2_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

    return status;
}


static sns_ddf_status_e sns_dd_acc_bma2x2_signal_register(
        sns_ddf_handle_t dd_handle,
        bool signal_registered)
{
    sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;
    uint32_t irq_trigger_type = BMA2X2_IRQ_TRIGGER_TYPE_DEFAULT;
    sns_dd_acc_bma2x2_state_t *state = sns_dd_acc_bma2x2_get_dev(dd_handle);
    bool dri_imitate_ff_wmi = false;

    BST_IGNORE_WARNING_UNUSED_VAR(dri_imitate_ff_wmi);

#if BMA2X2_CONFIG_ENABLE_DRI_IMITATE_FF_WMI
    if (1 == state->f_wml) {
        dri_imitate_ff_wmi = true;
    }
#endif

#if BMA2X2_CONFIG_WMI_TRIGGER_LEVEL
    if (state->en_fifo_int && !dri_imitate_ff_wmi) {
        irq_trigger_type = SNS_DDF_SIGNAL_IRQ_HIGH;
    }

    if ((signal_registered)
            && (!state->en_fifo_int_orig)
            && (state->en_fifo_int)
            && (BMA2X2_IRQ_TRIGGER_TYPE_DEFAULT != SNS_DDF_SIGNAL_IRQ_HIGH)) {
        /* need to change trigger type */
        ret_val = sns_ddf_signal_deregister(state->gpio_num);
        signal_registered = false;
        state->en_fifo_int_orig = state->en_fifo_int;
    }
#endif
    if (!signal_registered) {
        if ((ret_val = sns_ddf_signal_register(
                        state->gpio_num,
                        dd_handle,
                        &sns_accel_bma2x2_driver_fn_list,
                        irq_trigger_type)) != SNS_DDF_SUCCESS) {
            return ret_val;
        }

        BMA2X2_MSG_1(HIGH, "signal_registered: %d", irq_trigger_type);
    }

    return ret_val;
}

static sns_ddf_status_e sns_dd_acc_bma2x2_signal_deregister(
        sns_ddf_handle_t dd_handle,
        bool signal_registered)
{
    sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;
    uint32_t signal_needed = 0;
    sns_dd_acc_bma2x2_state_t *state = sns_dd_acc_bma2x2_get_dev(dd_handle);

    signal_needed = ((state->en_md) || (state->en_db_tap_int)
            || (state->en_dri) || (state->en_fifo_int));

    if (signal_registered && (!signal_needed)) {
        ret_val = sns_ddf_signal_deregister(state->gpio_num);
        BMA2X2_MSG_1(MEDIUM, "sns_dd_acc_bma2x2_signal_deregister  %d ",
                state->gpio_num);
    }

    return ret_val;
}


sns_ddf_status_e bma2x2_set_int_mode(
        sns_ddf_handle_t port_handle,
        unsigned char Mode)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle,
            BMA2X2_INT_MODE_SEL__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_INT_MODE_SEL, Mode);
    comres = bma2x2_smbus_write_byte(port_handle,
            BMA2X2_INT_MODE_SEL__REG, &data);

    return comres;
}



sns_ddf_status_e bma2x2_set_slope_duration(
        sns_ddf_handle_t port_handle,
        unsigned char duration)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle,
            BMA2X2_SLOPE_DUR__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_SLOPE_DUR, duration);
    comres = bma2x2_smbus_write_byte(port_handle,
            BMA2X2_SLOPE_DUR__REG, &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_slope_no_mot_duration(
        sns_ddf_handle_t port_handle,
        unsigned char duration)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;


    comres = bma2x2_smbus_read_byte(port_handle,
            BMA2x2_SLO_NO_MOT_DUR__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2x2_SLO_NO_MOT_DUR, duration);
    comres = bma2x2_smbus_write_byte(port_handle,
            BMA2x2_SLO_NO_MOT_DUR__REG, &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_slope_threshold(
        sns_ddf_handle_t port_handle,
        unsigned char threshold)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    data = threshold;
    comres = bma2x2_smbus_write_byte(port_handle,
            BMA2X2_SLOPE_THRES__REG, &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_slope_no_mot_threshold(
        sns_ddf_handle_t port_handle,
        unsigned char threshold)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    data = threshold;
    comres = bma2x2_smbus_write_byte(port_handle,
            BMA2X2_SLO_NO_MOT_THRES_REG, &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_low_g_duration(
        sns_ddf_handle_t port_handle,
        unsigned char duration)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_LOWG_DUR__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_LOWG_DUR, duration);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_LOWG_DUR__REG, &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_low_g_threshold(
        sns_ddf_handle_t port_handle,
        unsigned char threshold)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_LOWG_THRES__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_LOWG_THRES, threshold);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_LOWG_THRES__REG, &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_high_g_duration(
        sns_ddf_handle_t port_handle,
        unsigned char duration)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_HIGHG_DUR__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_HIGHG_DUR, duration);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_HIGHG_DUR__REG, &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_high_g_threshold(
        sns_ddf_handle_t port_handle,
        unsigned char threshold)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_HIGHG_THRES__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_HIGHG_THRES, threshold);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_HIGHG_THRES__REG,
            &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_tap_duration(
        sns_ddf_handle_t port_handle,
        unsigned char duration)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_TAP_DUR__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_TAP_DUR, duration);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_TAP_DUR__REG, &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_tap_shock(
        sns_ddf_handle_t port_handle,
        unsigned char setval)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_TAP_SHOCK_DURN__REG,
            &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_TAP_SHOCK_DURN, setval);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_TAP_SHOCK_DURN__REG,
            &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_tap_quiet(
        sns_ddf_handle_t port_handle,
        unsigned char duration)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_TAP_QUIET_DURN__REG,
            &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_TAP_QUIET_DURN, duration);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_TAP_QUIET_DURN__REG,
            &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_tap_threshold(
        sns_ddf_handle_t port_handle,
        unsigned char threshold)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_TAP_THRES__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_TAP_THRES, threshold);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_TAP_THRES__REG, &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_tap_samp(
        sns_ddf_handle_t port_handle,
        unsigned char samp)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_TAP_SAMPLES__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_TAP_SAMPLES, samp);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_TAP_SAMPLES__REG,
            &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_orient_mode(
        sns_ddf_handle_t port_handle,
        unsigned char mode)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_ORIENT_MODE__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_ORIENT_MODE, mode);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_ORIENT_MODE__REG,
            &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_orient_blocking(
        sns_ddf_handle_t port_handle,
        unsigned char samp)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_ORIENT_BLOCK__REG,
            &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_ORIENT_BLOCK, samp);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_ORIENT_BLOCK__REG,
            &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_orient_hyst(
        sns_ddf_handle_t port_handle,
        unsigned char orienthyst)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_ORIENT_HYST__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_ORIENT_HYST, orienthyst);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_ORIENT_HYST__REG,
            &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_theta_blocking(
        sns_ddf_handle_t port_handle,
        unsigned char thetablk)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_THETA_BLOCK__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_THETA_BLOCK, thetablk);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_THETA_BLOCK__REG,
            &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_theta_flat(
        sns_ddf_handle_t port_handle,
        unsigned char thetaflat)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_THETA_FLAT__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_THETA_FLAT, thetaflat);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_THETA_FLAT__REG, &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_flat_hold_time(
        sns_ddf_handle_t port_handle,
        unsigned char holdtime)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_FLAT_HOLD_TIME__REG,
            &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_FLAT_HOLD_TIME, holdtime);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_FLAT_HOLD_TIME__REG,
            &data);

    return comres;
}



#define BMA2X2_FIFO_MODE_BYPASS 0
#define BMA2X2_FIFO_MODE_STREAM 2
static sns_ddf_status_e bma2x2_config_fifo_mode(
        sns_ddf_handle_t port_handle,
        uint32_t wml)
{
    sns_ddf_status_e status;
    uint8_t fifo_mode;

    if (wml > 0) {
        fifo_mode = BMA2X2_FIFO_MODE_STREAM;
    } else {
        fifo_mode = BMA2X2_FIFO_MODE_BYPASS;
    }
    status = bma2x2_set_fifo_mode(port_handle, fifo_mode);


    return status;
}

static sns_ddf_status_e bma2x2_enable_doubletap_int(
        sns_ddf_handle_t port_handle,
        int en)
{
    sns_ddf_status_e status;

    uint8_t regv = 0x00;
    uint8_t rega;

#if BMA2X2_CONFIG_ENABLE_INT_MAP_2
    rega = BMA2X2_REGA_INT_MAP_2;
#else
    rega = BMA2X2_REGA_INT_MAP_0;
#endif
    status = bma2x2_smbus_read_byte(port_handle,
            rega, &regv);
    BMA2X2_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

    if (en) {
        regv = BST_SET_VAL_BIT(regv, 4);
    } else {
        regv = BST_CLR_VAL_BIT(regv, 4);
    }

    status = bma2x2_smbus_write_byte(port_handle,
            rega, &regv);
    BMA2X2_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);



    BMA2X2_MSG_3_P(HIGH, "%d %d %d",
            55555600, en, status);

    status = bma2x2_enable_int(port_handle, 9, !!en);

    BMA2X2_MSG_3_P(HIGH, "%d %d %d",
            55555601, en, status);

    return status;
}



sns_ddf_status_e bma2x2_set_range(
        sns_ddf_handle_t port_handle,
        unsigned char Range)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data1;


    if (Range < 4) {
        comres = bma2x2_smbus_read_byte(port_handle,
                BMA2X2_RANGE_SEL_REG, &data1);
        switch (Range) {
            case 0:
                data1  = BMA2X2_SET_BITSLICE(data1,
                        BMA2X2_RANGE_SEL, 3);
                break;
            case 1:
                data1  = BMA2X2_SET_BITSLICE(data1,
                        BMA2X2_RANGE_SEL, 5);
                break;
            case 2:
                data1  = BMA2X2_SET_BITSLICE(data1,
                        BMA2X2_RANGE_SEL, 8);
                break;
            case 3:
                data1  = BMA2X2_SET_BITSLICE(data1,
                        BMA2X2_RANGE_SEL, 12);
                break;
            default:
                break;
        }
        comres += bma2x2_smbus_write_byte(port_handle,
                BMA2X2_RANGE_SEL_REG, &data1);
    } else{
        comres = SNS_DDF_EINVALID_PARAM;
    }


    return comres;
}

sns_ddf_status_e bma2x2_set_selftest_st(
        sns_ddf_handle_t port_handle,
        unsigned char selftest)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_EN_SELF_TEST__REG,
            &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_EN_SELF_TEST, selftest);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_EN_SELF_TEST__REG,
            &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_selftest_stn(
        sns_ddf_handle_t port_handle,
        unsigned char stn)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_NEG_SELF_TEST__REG,
            &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_NEG_SELF_TEST, stn);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_NEG_SELF_TEST__REG,
            &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_selftest_amp(
        sns_ddf_handle_t port_handle,
        unsigned char amp)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_SELF_TEST_AMP__REG,
            &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_SELF_TEST_AMP, amp);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_SELF_TEST_AMP__REG,
            &data);

    return comres;
}



int bma2x2_get_tap_sign(uint8_t int_stat)
{
    int ret;

    ret = BMA2X2_GET_BITSLICE(int_stat, BMA2X2_TAP_SIGN_S);

    return ret;
}


sns_ddf_status_e bma2x2_set_int2_fwm(
        sns_ddf_handle_t port_handle,
        unsigned char int2_fwm)
{
    unsigned char data;
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;

    comres = bma2x2_smbus_read_byte(port_handle,
            BMA2X2_EN_INT2_PAD_FWM__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_EN_INT2_PAD_FWM, int2_fwm);
    comres = bma2x2_smbus_write_byte(port_handle,
            BMA2X2_EN_INT2_PAD_FWM__REG, &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_int1_ffull(
        sns_ddf_handle_t port_handle,
        unsigned char int1_ffull)
{
    unsigned char data;
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;

    comres = bma2x2_smbus_read_byte(port_handle,
            BMA2X2_EN_INT1_PAD_FFULL__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_EN_INT1_PAD_FFULL, int1_ffull);
    comres = bma2x2_smbus_write_byte(port_handle,
            BMA2X2_EN_INT1_PAD_FFULL__REG, &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_int2_ffull(
        sns_ddf_handle_t port_handle,
        unsigned char int2_ffull)
{
    unsigned char data;
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;

    comres = bma2x2_smbus_read_byte(port_handle,
            BMA2X2_EN_INT2_PAD_FFULL__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_EN_INT2_PAD_FFULL, int2_ffull);
    comres = bma2x2_smbus_write_byte(port_handle,
            BMA2X2_EN_INT2_PAD_FFULL__REG, &data);

    return comres;
}

sns_ddf_status_e bma2x2_set_fifo_wml_trig(
        sns_ddf_handle_t port_handle,
        unsigned char fifo_wml_trig)
{
    unsigned char data;
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_FIFO_WML_TRIG_RETAIN__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_FIFO_WML_TRIG_RETAIN, fifo_wml_trig);
    comres = bma2x2_smbus_write_byte(port_handle, BMA2X2_FIFO_WML_TRIG_RETAIN__REG, &data);

    return comres;
}

sns_ddf_status_e bma2x2_get_fifo_wml_trig(
        sns_ddf_handle_t port_handle,
        unsigned char *fifo_wml_trig)
{
    unsigned char data;
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;

    comres = bma2x2_smbus_read_byte(port_handle,
            BMA2X2_FIFO_WML_TRIG_RETAIN__REG, &data);
    *fifo_wml_trig = BMA2X2_GET_BITSLICE(data, BMA2X2_FIFO_WML_TRIG_RETAIN);

    return comres;
}

/*  fifo_mode
    0 ------ Bypass
    1 ------ FIFO
    2 ------ Stream
    */

sns_ddf_status_e bma2x2_get_fifo_mode(
        sns_ddf_handle_t dd_handle,
        unsigned char *fifo_mode)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(dd_handle, BMA2X2_FIFO_MODE__REG, &data);
    *fifo_mode = BMA2X2_GET_BITSLICE(data, BMA2X2_FIFO_MODE);

    return comres;
}

/*  fifo_mode
    0 ------ Bypass
    1 ------ FIFO
    2 ------ Stream
    */

static sns_ddf_status_e bma2x2_set_fifo_mode(
        sns_ddf_handle_t dd_handle,
        unsigned char fifo_mode)
{
    unsigned char data;
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;

    if (fifo_mode < 4) {
        comres = bma2x2_smbus_read_byte(dd_handle, BMA2X2_FIFO_MODE__REG,
                &data);
        data = BMA2X2_SET_BITSLICE(data, BMA2X2_FIFO_MODE, fifo_mode);
        data |= (0x03 << 2); /* WA */
        comres = bma2x2_smbus_write_byte(dd_handle, BMA2X2_FIFO_MODE__REG,
                &data);
    } else {
        comres = SNS_DDF_EINVALID_PARAM ;
    }

    return comres;
}

/* fifo_trig

   0 ------ IGNORE FIFO
   1 ------ ENTER FIFO MODE

*/
sns_ddf_status_e bma2x2_get_fifo_trig(
        sns_ddf_handle_t dd_handle,
        unsigned char *fifo_trig)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(dd_handle,
            BMA2X2_FIFO_TRIGGER_ACTION__REG, &data);
    *fifo_trig = BMA2X2_GET_BITSLICE(data, BMA2X2_FIFO_TRIGGER_ACTION);

    return comres;
}

/* fifo_trig

   0 ------ IGNORE FIFO
   1 ------ ENTER FIFO MODE

*/

sns_ddf_status_e bma2x2_set_fifo_trig(
        sns_ddf_handle_t dd_handle,
        unsigned char fifo_trig)
{
    unsigned char data;
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;

    if (fifo_trig < 4) {
        comres = bma2x2_smbus_read_byte(dd_handle,
                BMA2X2_FIFO_TRIGGER_ACTION__REG, &data);
        data = BMA2X2_SET_BITSLICE(data, BMA2X2_FIFO_TRIGGER_ACTION,
                fifo_trig);
        comres = bma2x2_smbus_write_byte(dd_handle,
                BMA2X2_FIFO_TRIGGER_ACTION__REG, &data);
    } else {
        comres = SNS_DDF_EINVALID_PARAM ;
    }

    return comres;
}

/* trig_src
   0 ------ INT1
   1 ------ INT2

*/
sns_ddf_status_e bma2x2_get_fifo_trig_src(
        sns_ddf_handle_t dd_handle,
        unsigned char *trig_src)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(dd_handle,
            BMA2X2_FIFO_TRIGGER_SOURCE__REG, &data);
    *trig_src = BMA2X2_GET_BITSLICE(data, BMA2X2_FIFO_TRIGGER_SOURCE);

    return comres;
}

/* trig_src
   0 ------ INT1
   1 ------ INT2

*/

sns_ddf_status_e bma2x2_set_fifo_trig_src(
        sns_ddf_handle_t dd_handle,
        unsigned char trig_src)
{
    unsigned char data;
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;

    if (trig_src < 4) {
        comres = bma2x2_smbus_read_byte(dd_handle,
                BMA2X2_FIFO_TRIGGER_SOURCE__REG, &data);
        data = BMA2X2_SET_BITSLICE(data, BMA2X2_FIFO_TRIGGER_SOURCE,
                trig_src);
        comres = bma2x2_smbus_write_byte(dd_handle,
                BMA2X2_FIFO_TRIGGER_SOURCE__REG, &data);
    } else {
        comres = SNS_DDF_EINVALID_PARAM ;
    }

    return comres;
}



/*  data_sel
    0 ------ X,Y,Z
    1 ------ X
    2 ------ Y
    3 ------ Z

*/

sns_ddf_status_e bma2x2_set_fifo_data_sel(
        sns_ddf_handle_t dd_handle,
        unsigned char data_sel)
{
    unsigned char data;
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;

    if (data_sel < 4) {
        comres = bma2x2_smbus_read_byte(dd_handle,
                BMA2X2_FIFO_DATA_SELECT__REG,
                &data);
        data = BMA2X2_SET_BITSLICE(data, BMA2X2_FIFO_DATA_SELECT,
                data_sel);
        comres = bma2x2_smbus_write_byte(dd_handle,
                BMA2X2_FIFO_DATA_SELECT__REG,
                &data);
    } else {
        comres = SNS_DDF_EINVALID_PARAM ;
    }

    return comres;
}

sns_ddf_status_e bma2x2_get_fifo_data_out_reg(
        sns_ddf_handle_t dd_handle,
        unsigned char *out_reg)
{
    unsigned char data;
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;

    comres = bma2x2_smbus_read_byte(dd_handle,
            BMA2X2_FIFO_DATA_OUTPUT_REG, &data);
    *out_reg = data;

    return comres;
}


static sns_ddf_status_e bma2x2_get_dev_model_info(
        sns_dd_acc_bma2x2_state_t *state)
{
    sns_ddf_status_e stat;
    uint8_t chip_id;
    uint8_t reg_val;
    uint8_t out;
    uint8_t bit0;
    uint8_t bit1;
    uint8_t bit2;


    if ((stat = sns_ddf_read_port(state->port_handle,
                    BMA2X2_CHIP_ID_REG,
                    &chip_id,
                    1,
                    &out)) != SNS_DDF_SUCCESS) {
        return stat;
    }


    switch (chip_id) {
        case BMA255_CHIP_ID:
            state->sub_model = BMA255_TYPE;
            break;
        case BMA250E_CHIP_ID:
            state->sub_model = BMA250E_TYPE;
            break;
        case BMA222E_CHIP_ID:
            state->sub_model = BMA222E_TYPE;
            break;
        case BMA280_CHIP_ID:
            state->sub_model = BMA280_TYPE;
            break;
        default:
            state->sub_model =  0xFF;
    }


    /* check the part info further */
    reg_val = 0xAA;
    if ((stat = bma2x2_smbus_write_byte(state->port_handle,
                    0x35,
                    &reg_val)) != SNS_DDF_SUCCESS) {
        return stat;
    }
    sns_ddf_delay(2000);
    reg_val = 0xAA;
    if ((stat = bma2x2_smbus_write_byte(state->port_handle,
                    0x35,
                    &reg_val)) != SNS_DDF_SUCCESS) {
        return stat;
    }
    sns_ddf_delay(2000);

    if ((stat = sns_ddf_read_port(state->port_handle,
                    0x07,
                    &reg_val,
                    1,
                    &out)) != SNS_DDF_SUCCESS) {
        return stat;
    }
    bit0 = reg_val & (1<<5);

    if ((stat = sns_ddf_read_port(state->port_handle,
                    0x08,
                    &reg_val,
                    1,
                    &out)) != SNS_DDF_SUCCESS) {
        return stat;
    }
    bit1 = reg_val & (1<<5);

    if ((stat = sns_ddf_read_port(state->port_handle,
                    0x09,
                    &reg_val,
                    1,
                    &out)) != SNS_DDF_SUCCESS) {
        return stat;
    }
    bit2 = reg_val & (1<<5);

    reg_val = 0x0A;
    if ((stat = bma2x2_smbus_write_byte(state->port_handle,
                    0x35,
                    &reg_val)) != SNS_DDF_SUCCESS) {
        return stat;
    }
    sns_ddf_delay(2000);

    if ((!bit0) && (bit1) && (!bit2)) {
        state->dev_select = BMA2X2_DEV_SEL_NUM_BMI058;
    }
    /* end of check */

    return stat;
}

static bma2x2_sub_dev_t * sns_dd_acc_bma2x2_get_subdev(
        sns_ddf_handle_t dd_handle)
{
    bma2x2_sub_dev_t *sub_dev = (bma2x2_sub_dev_t *)dd_handle;
    return sub_dev;
}



/*
   static bma2x2_sensor_t *sns_dd_acc_bma2x2_get_sensor_from_type(
   bma2x2_sub_dev_t *sub_dev,
   const sns_ddf_sensor_e       sensor_type)
   {
   uint32_t i;

   for (i = 0; i < BMA2X2_MAX_NUM_SENSORS; i++)
   {
   if (sensor_type == sub_dev->sensors[i].sensor_type)
   {
   return sub_dev->sensors + i;
   }
   }

   return NULL;
   }
   */


uint32_t sns_dd_acc_bma2x2_fill_sub_dev_slot(
        sns_dd_acc_bma2x2_state_t    *state,
        bma2x2_sub_dev_t*            sub_dev_slot,
        sns_ddf_handle_t             smgr_handle,
        uint32_t                     gpio,
        uint32_t                     num_sensors,
        sns_ddf_sensor_e             sensors[],
        sns_ddf_sensor_e*            sensors_init[])
{
    uint32_t num_sensors_filled = 0;
    uint32_t i;

    if ((NULL == state) || (NULL == sub_dev_slot) || (NULL == sensors_init))
        return 0;

    if (num_sensors > BMA2X2_MAX_NUM_SENSORS)
    {
        BMA2X2_MSG_3(ERROR, "too many sensors for the subdev slot 0x%x num_sensors %d max_num %d",
                sub_dev_slot, num_sensors, BMA2X2_MAX_NUM_SENSORS);
        return 0;
    }

    for (i = 0; i < num_sensors; i++)
    {
        if ((sensors[i] == SNS_DDF_SENSOR_ACCEL)
                || (sensors[i] == SNS_DDF_SENSOR_TEMP)
                || (sensors[i] == SNS_DDF_SENSOR_DOUBLETAP))
        {
            sub_dev_slot->sensors[num_sensors_filled++].sensor_type
                = sensors[i];
            (*sensors_init)[i] = sensors[i];
        }

    }

    sub_dev_slot->smgr_handle   = smgr_handle;
    sub_dev_slot->num_sensors   = num_sensors_filled;
    sub_dev_slot->private_data  = state;

    return sub_dev_slot->num_sensors;
}



/*!
 * @brief Sets the BOSCH accelerometer to the passed in range
 *
 * @detail
 *   The range information is taken from the driver structure which is
 *   set to a default value as part of the reset function
 *   before calling this one.
 *
 *
 *  @param[in] state: Ptr to the driver structure
 *
 *  @return
 *    The error code definition within the DDF
 *    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 */
static sns_ddf_status_e sns_dd_acc_bma2x2_config_range(
        sns_dd_acc_bma2x2_state_t *state)
{
    uint8_t bosch_write_buffer;
    sns_ddf_status_e stat;

    /*! set range and bandwidth */
    bosch_write_buffer = state->range;
    if ((stat = bma2x2_smbus_write_byte(state->port_handle,
                    BMA2X2_RANGE_SEL_REG,
                    &bosch_write_buffer)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    if (state->en_md)
    {
        return sns_dd_acc_bma2x2_config_motion_detect_thresh(state);
    }

    return SNS_DDF_SUCCESS;
}

    static
sns_ddf_status_e sns_dd_acc_bma2x2_flush_fifo_data(
        sns_dd_acc_bma2x2_state_t *state)
{

    uint8_t                 f_count = 0;
    uint8_t                 f_overrun = 0;
    sns_ddf_status_e        status;
    bma2x2_sub_dev_t        *sub_dev;

    status = bma2x2_get_fifo_framecount(state->port_handle,
            &f_count, &f_overrun);

    if (SNS_DDF_SUCCESS != status) {
        return status;
    }

    if (f_count > 0) {
        if (f_count >= BMA2X2_MAX_FIFO_LEVEL) {
            BMA2X2_MSG_2(ERROR, "[bma2x2_fifo_flush] overflow condition"
                    "wml: %d f_count: %d",
                    state->f_wml, f_count);

            f_count = BMA2X2_MAX_FIFO_LEVEL - 1;
        }

        bma2x2_fifo_data_read_out_frames(state, f_count, f_overrun, 0);
        sub_dev = sns_dd_acc_bma2x2_get_subdev_from_type(state,
                SNS_DDF_SENSOR_ACCEL);

        if ((NULL != sub_dev)
                && (state->f_frames_cache.num_samples > 0)) {
            /*! notice : the number data len */
            status = sns_ddf_smgr_notify_data(sub_dev->smgr_handle,
                    &state->f_frames_cache, 1);
            sns_dd_accel_log_fifo(&state->f_frames_cache, DD_DATA_LOG_CONTEXT_FIFO_FLUSH);
        }

    }

    return SNS_DDF_SUCCESS;
}

/*!
 *  @brief Sets the BOSCH accelerometer to the passed in bandwidth
 *
 *
 *  @detail
 *
 *    The bandwidth information is taken from the driver structure which is
 *    set to BMA2X2_REGV_BW_31_25HZ as part of the reset function
 *
 *    before calling this one.
 *
 *
 *  @param[in] state: Ptr to the driver structure
 *  @param[in] delay: Update Time required for the output to be valid (in ns.)
 *
 *  @return
 *    The error code definition within the DDF
 *    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 *
 */
static sns_ddf_status_e sns_dd_acc_bma2x2_config_bw(
        sns_dd_acc_bma2x2_state_t   *state,
        uint8_t                     bw_idx)
{
    sns_ddf_status_e stat;
    uint32_t i;

    uint8_t regv_bw;
    uint8_t regv_bw_orig;

    if (state->sub_model == BMA280_TYPE) {
        if ((BMA2X2_REGV_BW_1000HZ - BMA2X2_REGV_BW_BASE) == bw_idx) {
            bw_idx--;
        }
    }

    regv_bw = BMA2X2_MAP_BW_ODR[bw_idx].r;
    regv_bw_orig = 0;
    stat = bma2x2_smbus_read_byte(state->port_handle, BMA2X2_REGA_PMU_BW, &regv_bw_orig);
    BMA2X2_DD_CHECK_RETVAL(stat, SNS_DDF_SUCCESS);

    if (regv_bw != regv_bw_orig) {
        if (state->f_wml > 0) {
            sns_dd_acc_bma2x2_flush_fifo_data(state);
        }

        /*! set range and bandwidth */
        stat = bma2x2_smbus_write_byte(state->port_handle,
                        BMA2X2_REGA_PMU_BW,
                        &regv_bw);
        BMA2X2_DD_CHECK_RETVAL(stat, SNS_DDF_SUCCESS);
    } else {
            BMA2X2_MSG_0(MEDIUM, "no need to change BW");
    }

    state->bw_idx = bw_idx;
    state->ts_itvl_ideal = BMA2X2_SSC_US_2_TICKS(1e6 / BMA2X2_MAP_BW_ODR[bw_idx].l);
    state->ts_itvl_est = state->ts_itvl_ideal;

    state->itvl_drdy_tolerance = BMA2X2_SSC_US_2_TICKS(1e6 / BMA2X2_MAP_BW_ODR[bw_idx].l) * BMA2X2_CONFIG_TOLERANCE_DRYD_TIME_ACCURACY;

    state->avail_accurate_ts_last_sample = 0;

    if (BMA2X2_REGV_BW_7_81HZ == regv_bw) {
        /* from experiment, use one fewer invalid sample after ODR is changed to 16 to reduce MD
           to accel stream latency */
        if (BMA2X2_CONFIG_SKIP_SAMPLE_CNT_ODRC > 1) {
            state->sample_cnt_to_skip_odrc = BMA2X2_CONFIG_SKIP_SAMPLE_CNT_ODRC - 1;
        } else {
            state->sample_cnt_to_skip_odrc = 0;
        }
    } else {
        state->sample_cnt_to_skip_odrc = BMA2X2_CONFIG_SKIP_SAMPLE_CNT_ODRC;
    }

    BMA2X2_MSG_3_P(MED, "%d %d %d",
            55555620, regv_bw, stat);

    for (i = 0; i < BST_ARRARY_SIZE(BMA2X2_MAP_BW_ODR); i++) {
        if (BMA2X2_MAP_BW_ODR[i].r == regv_bw) {
            /* Set the is_data_valid flag to false so the driver will know
             * to flag all output as invalid" until the timer expires */

            // If there is currently a bw-timer set, cancel it before starting a new one
            if (state->bw_timer_pending)
            {
                BMA2X2_MSG_0(HIGH, "BMA2X2 - BW timer already running, cancelling it...");
                if ((stat = sns_ddf_timer_cancel(state->bw_timer)) != SNS_DDF_SUCCESS)
                {
                    BMA2X2_MSG_1(ERROR, "BMA2X2 - BW timer cancel FAILED. Error %d", stat);
                    return stat;
                }
            }

            /* Set a timer here for sns_dd_accel_bw_update_time_us[BW_index] * (BMA2X2_BW_FILTER_SIZE - 1)
             * This will invalidate the next (BMA2X2_BW_FILTER_SIZE - 1) potentially unstable samples from
             * this sensor. The next (BMA2X2_BW_FILTER_SIZE-th) sample should be stable since the internal
             * filter on the BMA2X2 sensor is guaranteed to use valid values, at that point.
             */
            if ((stat = sns_ddf_timer_start(state->bw_timer,
                            sns_dd_accel_bw_update_time_us[i]
                            * (BMA2X2_BW_FILTER_SIZE - 1)))
                    == SNS_DDF_SUCCESS)
            {
                state->bw_timer_pending = true;
                BMA2X2_MSG_1(MEDIUM, "BMA2X2 - Started BW timer for %u us",
                        sns_dd_accel_bw_update_time_us[i]
                        * (BMA2X2_BW_FILTER_SIZE - 1));
            }
            else
            {
                BMA2X2_MSG_1(ERROR, "BMA2X2 - BW timer failed to start. Error: %d", stat);
                return stat;
            }

            break;
        }
    }

    return SNS_DDF_SUCCESS;
}

/*!
 *  @brief Sets the BOSCH acclerometer to its default operational state
 *
 *  @detail
 *  Programs the register BOSCH_ACCEL_SENSOR_CTRL5_REG_ADDR for the MD
 *  threshold
 *
 *  @param[in] state: Ptr to the driver structure
 *
 *  @return
 *    The error code definition within the DDF
 *    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 *
 */

static sns_ddf_status_e sns_dd_acc_bma2x2_config_default(
        sns_dd_acc_bma2x2_state_t *state)
{
    uint8_t  bosch_write_buffer;

    sns_ddf_status_e stat;


    /*! soft reset */
    bosch_write_buffer = BMA2X2_EN_SOFT_RESET_VALUE;
    if ((stat = bma2x2_smbus_write_byte(state->port_handle,
                    BMA2X2_RESET_REG,
                    &bosch_write_buffer)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    sns_ddf_delay(BMA2X2_SOFTRESET2READY_DELAY);

    //TODO: according to QCM, reset() will be called very often when needed,
    //and thus the delay should be kept and removed to the minimum possible

    /*! set the range */
    if ((stat = sns_dd_acc_bma2x2_config_range(state)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    /*! set the bandwidth */
    stat = sns_dd_acc_bma2x2_config_bw(state, BMA2X2_REGV_BW_125HZ - BMA2X2_REGV_BW_BASE);
    if (SNS_DDF_SUCCESS != stat)
    {
        return stat;
    }

    /*!set interrupt latch time,  if the interrupt signal can not be catched easily,
      then set this mode, notice: not set at DRI mode */
    //bma2x2_set_int_mode(state->port_handle, 1); // latched 250 ms

    if ((state->acc_cur_rate == 0)
            && (state->dbt_cur_rate == 0)
            && (state->tmp_cur_rate == 0)
            && (!state->test_init))
    {
        bma2x2_set_powerstate(state, SNS_DDF_POWERSTATE_LOWPOWER);
    }


#ifdef BMA2X2_ENABLE_INT_DOUBLE_TAP
    bma2x2_set_tap_duration(state->port_handle,
            state->tap_settings.dur);
    bma2x2_set_tap_shock(state->port_handle,
            state->tap_settings.shock);
    bma2x2_set_tap_quiet(state->port_handle,
            state->tap_settings.quiet);
    bma2x2_set_tap_threshold(state->port_handle,
            state->tap_settings.th);

#endif

    return stat;
}

/*!
 *  @brief Resets the driver and device so they return to the state they were
 *         in after init() was called.
 *
 *  @detail
 *   resets the bias to zero & range to BOSCH_ACCEL_SENSOR_RANGE_2G which
 *   is stored in the driver state structure
 *   Calls the sns_dd_acc_bma2x2_config_default routine
 *
 *  @param[in] state: Ptr to the driver structure
 *
 *  @return
 *    The error code definition within the DDF
 *    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 *
 */
sns_ddf_status_e sns_dd_acc_bma2x2_reset(sns_ddf_handle_t dd_handle)
{
    uint8_t i;
    sns_dd_acc_bma2x2_state_t *state = sns_dd_acc_bma2x2_get_dev(dd_handle);
    bma2x2_sub_dev_t *sub_dev;
    bma2x2_sub_dev_t *sub_dev_acc;
    bma2x2_sub_dev_t *sub_dev_dbt;

    sub_dev = sns_dd_acc_bma2x2_get_subdev(dd_handle);

    if (NULL == sub_dev) {
        return SNS_DDF_EINVALID_PARAM;
    }

    if ((NULL == state) || (state->sub_dev_cnt >= BMA2X2_MAX_SUB_DEV)) {
        return SNS_DDF_EINVALID_PARAM;
    }

    BMA2X2_MSG_2(HIGH, "<bma2x2_if_ reset> 0x%x sensor: %d",
            dd_handle,
            sub_dev->sensors[0].sensor_type);

    if (SNS_DDF_SENSOR_ACCEL == sub_dev->sensors[0].sensor_type) {
        state->ts_last_sample = 0;
    }

    sub_dev_acc = sns_dd_acc_bma2x2_get_subdev_from_type(state, SNS_DDF_SENSOR_ACCEL);
    sub_dev_dbt = sns_dd_acc_bma2x2_get_subdev_from_type(state, SNS_DDF_SENSOR_DOUBLETAP);
    if (sub_dev == sub_dev_acc) {
        state->acc_cur_rate = 0;
        state->tmp_cur_rate = 0;

        state->en_md = false;
        state->en_dri = false;
        state->en_fifo_int = false;

        state->bw_timer_pending = false;
    } else if(sub_dev == sub_dev_dbt) {
        state->dbt_cur_rate = 0;

        state->en_db_tap_int = false;
    } else {
        return SNS_DDF_EINVALID_PARAM;
    }

#if BMA2X2_DEBUG
    sns_ddf_axes_map_s            *axes_map = &state->axes_map;
#endif

    BMA2X2_MSG_3_F(HIGH, "sns_dd_acc_bma2x2_reset %d 0x%x %d",
            55555560, dd_handle, state->dev_select);
    BMA2X2_MSG_3_P(HIGH, "%d 0x%x %d",
            55555561, state->acc_cur_rate, state->dbt_cur_rate);
    BMA2X2_MSG_3_P(HIGH, "%d 0x%x %d",
            55555562, state->acc_desired_rate, state->tmp_cur_rate);


    BMA2X2_MSG_3_P(HIGH,"55555565 %d %d %d", axes_map->indx_x, axes_map->indx_y, axes_map->indx_z);
    BMA2X2_MSG_3_P(HIGH,"55555565 %d %d %d", axes_map->sign_x, axes_map->sign_y, axes_map->sign_z);

    state->range_idx = 1;   /* 1 for +/-4G */
    state->range = BMA2X2_ACC_RANGE_LIST[state->range_idx];

    if (!state->soft_reset) {
        if ((0 == state->acc_cur_rate)
                && (0 == state->dbt_cur_rate)
                && (0 == state->tmp_cur_rate)) {
            bma2x2_set_powerstate(state, SNS_DDF_POWERSTATE_LOWPOWER);
        }
        return SNS_DDF_SUCCESS;
    }

    for (i = 0; i < SDD_ACC_NUM_AXIS; i++) {
        state->bias[i] = 0;
    }


    BMA2X2_MSG_3_P(HIGH, "%d 0x%x %d",
            55555563, state->acc_cur_rate, state->dbt_cur_rate);
    BMA2X2_MSG_3_P(HIGH, "%d 0x%x %d",
            55555564, state->acc_desired_rate, state->tmp_cur_rate);

    return sns_dd_acc_bma2x2_config_default(state);
}


#if 0
void sns_dd_acc_bma2x2_dmp_fifo_reg(sns_dd_acc_bma2x2_state_t *state)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    uint8_t buffer, out;

    return;
    if ((status = sns_ddf_read_port(state->port_handle,
                    BMA2X2_FIFO_MODE_REG,
                    &buffer,
                    1,
                    &out)) == SNS_DDF_SUCCESS)
    {
        BMA2X2_MSG_3_F(HIGH, "%d 0x%x BMA2X2_FIFO_MODE_REG %d",
                55555583, BMA2X2_FIFO_MODE_REG, buffer);
    }

    if ((status = sns_ddf_read_port(state->port_handle,
                    BMA2X2_INT_ENABLE2_REG,
                    &buffer,
                    1,
                    &out)) == SNS_DDF_SUCCESS)
    {
        BMA2X2_MSG_3_F(HIGH, "%d 0x%x BMA2X2_INT_ENABLE2_REG %d",
                55555583, BMA2X2_INT_ENABLE2_REG, buffer);
    }

    if ((status = sns_ddf_read_port(state->port_handle,
                    BMA2X2_INT_DATA_SEL_REG,
                    &buffer,
                    1,
                    &out)) == SNS_DDF_SUCCESS)
    {
        BMA2X2_MSG_3_F(HIGH, "%d 0x%x BMA2X2_INT_DATA_SEL_REG %d",
                55555583, BMA2X2_INT_DATA_SEL_REG, buffer);
    }

    if ((status = sns_ddf_read_port(state->port_handle,
                    BMA2X2_BW_SEL_REG,
                    &buffer,
                    1,
                    &out)) == SNS_DDF_SUCCESS)
    {
        BMA2X2_MSG_3_F(HIGH, "%d 0x%x BMA2X2_BW_SEL_REG %d",
                55555583, BMA2X2_BW_SEL_REG, buffer);
    }

    if ((status = sns_ddf_read_port(state->port_handle,
                    BMA2X2_MODE_CTRL_REG,
                    &buffer,
                    1,
                    &out)) == SNS_DDF_SUCCESS)
    {
        BMA2X2_MSG_3_F(HIGH, "%d 0x%x BMA2X2_MODE_CTRL_REG %d",
                55555583, BMA2X2_MODE_CTRL_REG, buffer);
    }

    if ((status = sns_ddf_read_port(state->port_handle,
                    BMA2X2_STATUS1_REG,
                    &buffer,
                    1,
                    &out)) == SNS_DDF_SUCCESS)
    {
        BMA2X2_MSG_3_F(HIGH, "%d 0x%x BMA2X2_STATUS1_REG %d",
                55555583, BMA2X2_STATUS1_REG, buffer);
    }

    if ((status = sns_ddf_read_port(state->port_handle,
                    BMA2X2_STATUS2_REG,
                    &buffer,
                    1,
                    &out)) == SNS_DDF_SUCCESS)
    {
        BMA2X2_MSG_3_F(HIGH, "%d 0x%x BMA2X2_STATUS2_REG %d",
                55555583, BMA2X2_STATUS2_REG, buffer);
    }

    if ((status = sns_ddf_read_port(state->port_handle,
                    BMA2X2_FIFO_WML_TRIG,
                    &buffer,
                    1,
                    &out)) == SNS_DDF_SUCCESS)
    {
        BMA2X2_MSG_3_F(HIGH, " %d 0x%x BMA2X2_FIFO_WML_TRIG %d",
                55555583, BMA2X2_FIFO_WML_TRIG, buffer);
    }

    if ((status = sns_ddf_read_port(state->port_handle,
                    BMA2X2_STATUS_FIFO_REG,
                    &buffer,
                    1,
                    &out)) == SNS_DDF_SUCCESS)
    {
        BMA2X2_MSG_3_F(HIGH, " %d 0x%x BMA2X2_STATUS_FIFO_REG %d",
                55555583, BMA2X2_STATUS_FIFO_REG, buffer);
    }
}
#endif



static sns_dd_acc_bma2x2_state_t* sns_dd_acc_bma2x2_find_same_port_dev(
        sns_ddf_port_config_s *port_config_ptr)
{
    sns_dd_acc_bma2x2_state_t* cur_dd_dev_ptr =
        (sns_dd_acc_bma2x2_state_t*)bma2x2_sns_dd_list_head.next;

    while (cur_dd_dev_ptr)
    {
        if (true == sns_ddf_is_same_port_id(port_config_ptr, &cur_dd_dev_ptr->port_id))
        {
            return cur_dd_dev_ptr;
        }
        else
        {
            cur_dd_dev_ptr = cur_dd_dev_ptr->next;
        }
    }
    return NULL;
}

/**
 * @brief Validate and initialize the tap settings
 */
void sns_dd_bma2x2_tap_settings_init(
        sns_dd_bma2x2_tap_settings_s*    tap_settings,
        const sns_dd_bma2x2_nv_params_s* nv_params)
{
    if (tap_settings == NULL)
        return;

    tap_settings->th = BOSCH_ACCEL_SENSOR_TAP_TH;
    tap_settings->shock = BOSCH_ACCEL_SENSOR_TAP_SHOCK;
    tap_settings->quiet = BOSCH_ACCEL_SENSOR_TAP_QUIET;
    tap_settings->dur = BOSCH_ACCEL_SENSOR_TAP_DUR;
    BMA2X2_MSG_3_P(HIGH, "%d %d %d",
            55555702, tap_settings, nv_params);


    // If no nv_params are specified, then keep the default
    if (nv_params == NULL)
    {
        BMA2X2_MSG_3_P(HIGH, "%d %d %d",
                55555703, 55555703, 55555703);
        return;
    }

    // Check to see if the nv_params contains tap-settings by validating the magic number
    if (BST_NVM_MAGIC == nv_params->magic_num)
    {
        /*!The registry magic number is correct. Loading NV params...*/
        BMA2X2_MSG_3_P(HIGH, "%d %d %d",
                55555704, nv_params->th, nv_params->shock);
        BMA2X2_MSG_3_P(HIGH, "%d %d %d",
                55555705, nv_params->quiet, nv_params->dur);

        tap_settings->th = nv_params->th;
        tap_settings->shock = nv_params->shock;
        tap_settings->quiet = nv_params->quiet;
        tap_settings->dur = nv_params->dur;

    }
    else
    {
        /*!Incorrect registry magic number specified, using defaults...*/
        BMA2X2_MSG_3_P(HIGH, "%d %d %d",
                55555706, 55555706, 55555706);
        return;
    }
}

/**
 * @brief Parses the nv_params and fills in appropriate values for the axes_map
 *        and the tap_settings (if applicable)
 */
sns_ddf_status_e sns_dd_bma2x2_parse_nv_params(
        sns_ddf_nv_params_s             *entire_nv_params,
        sns_dd_acc_bma2x2_state_t       *state)
{
    sns_ddf_axes_map_s            *axes_map = &state->axes_map;
    sns_dd_bma2x2_tap_settings_s  *tap_settings = &state->tap_settings;

    // If there are no nv_params handed in, then set all to default values
    if (NULL == entire_nv_params)
    {
        BMA2X2_MSG_3_P(HIGH, "%d %d %d",
                55555701, entire_nv_params, axes_map);
        sns_ddf_axes_map_init(axes_map, NULL);
        sns_dd_bma2x2_tap_settings_init(tap_settings, NULL);
    }
    // Else, parse the nv_params and validate each entry
    else
    {
        const sns_dd_bma2x2_nv_params_s* nv_params;
        if (NULL == entire_nv_params->data)
        {
            BMA2X2_MSG_3_F(ERROR,"sns_dd_bma2x2_parse_nv_params no nv ram data %d %d %d", 0,1,1);
            return SNS_DDF_EFAIL;
        }
        nv_params = (sns_dd_bma2x2_nv_params_s*) entire_nv_params->data;
        // Initialize the axes-map
        sns_ddf_axes_map_init(axes_map, entire_nv_params->data);
        sns_dd_bma2x2_tap_settings_init(tap_settings, nv_params);
        BMA2X2_MSG_3_P(HIGH,"%d %d %d",axes_map->indx_x, axes_map->indx_y, axes_map->indx_z);
        BMA2X2_MSG_3_P(HIGH,"%d %d %d",axes_map->sign_x, axes_map->sign_y, axes_map->sign_z);
    }

    return SNS_DDF_SUCCESS;
}

/*!
 *  @brief Initializes the 3 axis Bosch accelerometer BMA2X2
 *              determines the device to use
 *
 *  @detail
 *  - Allocates memory for driver state structure.
 *  Opens the device port by calling sns_ddf_open_port routine
 *  Calls sns_dd_acc_bma2x2_reset routine
 *
 *
 * @param[out] dd_handle_ptr  Pointer that this function must malloc and
 *                             populate. This is a handle to the driver
 *                             instance that will be passed in to all
 *                             other functions.
 *  @param[in]  smgr_handle    Handle used to identify this driver when it
 *                             calls into Sensors Manager functions.
 *  @param[in]  nv_params      NV parameters retrieved for the driver.
 *  @param[in]  device_info    Information describing each of the physical
 *                             devices controlled by this driver. This is
 *                             used to configure the bus and talk to the
 *                             device.
 *  @param[in]  memhandler     Memory handler used to dynamically allocate
 *                             output parameters, if applicable. NB: Do
 *                             not use memhandler to allocate memory for
 *                             @a dd_handle_ptr.
 * @param[in]  num_devices    Length of @a device_info.
 * @param[out] sensors        List of supported sensors, populated and
 returned by this function.
 *  @param[out] num_sensors    Length of @a sensors.
 *
 *
 *  @return
 *    The error code definition within the DDF
 *    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 *
 */
sns_ddf_status_e sns_dd_acc_bma2x2_init(
        sns_ddf_handle_t*        dd_handle_ptr,
        sns_ddf_handle_t         smgr_handle,
        sns_ddf_nv_params_s*     nv_params,
        sns_ddf_device_access_s  device_info[],
        uint32_t                 num_devices,
        sns_ddf_memhandler_s*    memhandler,
        sns_ddf_sensor_e*        sensors[],
        uint32_t*                num_sensors)
{
    sns_ddf_status_e stat;
    sns_dd_acc_bma2x2_state_t *state;
    sns_ddf_sensor_e *sensors_init;
    uint8_t i;

    /*=============================================================================
      UIMAGE REFACTORING SUPPORT IMPORTANT NOTES:
      1. The fuction "sns_ddf_smgr_set_uimg_refac" should be the very first thing
      to be done in the init function.
      2. The dd_handle_ptr should be updated immediatly after it is available
      before using it for anything, otherwise, refactoring might not be effective.
      3. The relevant scons file should be updated to include refactored files.
      =============================================================================*/

    /* Update smgr sensor data for the driver to indicate uImage support  */
#if BMA2X2_CONFIG_ENABLE_UIMAGE
    sns_ddf_smgr_set_uimg_refac(smgr_handle);
#endif

    BMA2X2_MSG_3_P(HIGH, "%d 0x%x %d",
            55555500, smgr_handle, device_info->first_gpio);
    BMA2X2_MSG_3_P(HIGH, "%d %d %d",
            55555500, nv_params->nvitem_grp, nv_params->nvitem_id);

    if (dd_handle_ptr == NULL || num_sensors == NULL || sensors == NULL || memhandler == NULL)
    {
        BMA2X2_MSG_0(ERROR, "BMA2X2_init: Null Pointer argument found.");
        return SNS_DDF_EINVALID_PARAM;
    }

    BMA2X2_MSG_2(HIGH, "bma2x2_init: number of sensors: %u first sensor: %u",
            *num_sensors, (*sensors)[0]);


    state = sns_dd_acc_bma2x2_find_same_port_dev(&device_info->port_config);

    BMA2X2_MSG_3_P(HIGH, "%d 0x%x %d",
            55555501, state, 12345678);


    BMA2X2_MSG_3_P(HIGH, "%d %d %d",
            55555501, *num_sensors, 12345678);

    BMA2X2_MSG_3_P(HIGH, "%d %d %d",
            55555501, (*sensors)[0], 12345678);
    BMA2X2_MSG_3(HIGH, "%d %d %d",
            55555501, (*sensors)[1], 12345678);
    BMA2X2_MSG_3(HIGH, "%d %d %d",
            55555501, (*sensors)[2], 12345678);


    for (i=0; i<ARRAY_SIZE(BMA2x2_G_OVER_RANGE_LIST); i++)
    {
        BMA2x2_G_OVER_RANGE_LIST[i] = G/BMA2X2_LSB_RANGE_LIST[i];
    }

    if (NULL == state)
    {

        BMA2X2_MSG_2(HIGH,
                "bma2x2_init new dev I2C addr: 0x%X bus instance: 0x%X",
                device_info->port_config.bus_config.i2c->slave_addr,
                device_info->port_config.bus_instance);

        stat = sns_ddf_malloc_ex((void**)&state,
                sizeof(sns_dd_acc_bma2x2_state_t),smgr_handle);
        if (stat != SNS_DDF_SUCCESS)
        {
            return stat;
        }
        memset(state, 0, sizeof(*state));

        BMA2X2_MSG_3_P(HIGH, "%d 0x%x, %d",
                55555502, state, 12345678);

        memset(state, 0, sizeof(sns_dd_acc_bma2x2_state_t));


        state->next = bma2x2_sns_dd_list_head.next;
        bma2x2_sns_dd_list_head.next = state;

        sns_ddf_get_port_id(&device_info->port_config, &state->port_id);

        //CHECK: the malloc size
        stat = sns_ddf_malloc_ex((void**)&(state->sensor_data.samples),
                sizeof(sns_ddf_sensor_sample_s)
                * SDD_ACC_NUM_AXIS, smgr_handle);
        if (stat != SNS_DDF_SUCCESS)
        {
            sns_ddf_mfree_ex(state,smgr_handle);
            return stat;
        }

        stat = sns_ddf_open_port(&state->port_handle,
                &device_info->port_config);
        if (stat != SNS_DDF_SUCCESS)
        {
            sns_ddf_mfree_ex(state->sensor_data.samples,smgr_handle);
            sns_ddf_mfree_ex(state,smgr_handle);
            return stat;
        }


        stat = sns_ddf_malloc_ex((void**)&(state->f_frames_cache.samples),
                sizeof(sns_ddf_sensor_sample_s)
                * SDD_ACC_NUM_AXIS* BMA2X2_MAX_FIFO_LEVEL, smgr_handle);
        if (stat != SNS_DDF_SUCCESS)
        {
            sns_ddf_mfree_ex(state->sensor_data.samples,smgr_handle);
            sns_ddf_mfree_ex(state,smgr_handle);
            return stat;
        }


        sensors_init = sns_ddf_memhandler_malloc_ex(memhandler,
                sizeof(sns_ddf_sensor_e)
                * BMA2X2_MAX_NUM_SENSORS, smgr_handle);


        // Check to see if valid input is given for num_sensors and sensors.
        // If not, then it is using the older version of SMGR (without using probe & SSI)
        //CHECK
        if (0 == *num_sensors && SNS_DDF_SENSOR__NONE == *sensors[0])
        {
            *num_sensors = 2;
            (*sensors)[0] = (sns_ddf_sensor_e)SNS_DDF_SENSOR_ACCEL;
            (*sensors)[1] = (sns_ddf_sensor_e)SNS_DDF_SENSOR_TEMP;
        }


        // Fill out the first sub-device slot
        uint32_t num_sensors_init = sns_dd_acc_bma2x2_fill_sub_dev_slot(
                state,
                &state->sub_dev[0],
                smgr_handle,
                device_info->first_gpio,
                *num_sensors,
                *sensors,
                &sensors_init);
        if (num_sensors_init == 0)
        {
            BMA2X2_MSG_0(ERROR, "bma2x2_init - unable to initialize any sensors");
            sns_ddf_close_port(state->port_handle);
            sns_ddf_mfree_ex(state->f_frames_cache.samples,smgr_handle);
            sns_ddf_mfree_ex(state->sensor_data.samples,smgr_handle);
            sns_ddf_mfree_ex(state,smgr_handle);
            //this error is due to unrecoginized sensor types in the sensors param
            return SNS_DDF_EINVALID_PARAM;
        }

        BMA2X2_MSG_3_P(HIGH, "%d %d 0x%x",
                55555501, num_sensors_init, state->dev_select);
        // Assign the dd_handle_ptr
        //CHECK:
        //(*dd_handle_ptr) = (sns_ddf_handle_t) ((uint32_t) state | state->sub_dev_cnt);
        (*dd_handle_ptr) = (sns_ddf_handle_t) (&state->sub_dev[0]);
        BMA2X2_MSG_3_P(HIGH, "%d 0x%x %d",
                55555503, (*dd_handle_ptr), 12345678);
        state->sub_dev_cnt = 0;
        state->sub_dev_cnt++;


        sns_dd_bma2x2_parse_nv_params(nv_params, state);


        BMA2X2_MSG_3_P(HIGH,"%d %d %d",55555501,device_info->first_gpio, 12345678);
        state->gpio_num = device_info->first_gpio;

        if ((stat = sns_ddf_timer_init(&state->bw_timer,
                        *dd_handle_ptr,
                        &sns_accel_bma2x2_driver_fn_list,
                        state,
                        0)) != SNS_DDF_SUCCESS)
        {
            BMA2X2_MSG_0(ERROR, "bma2x2_init - unable to initialize timer");
            sns_ddf_close_port(state->port_handle);
            sns_ddf_mfree_ex(state->f_frames_cache.samples,smgr_handle);
            sns_ddf_mfree_ex(state->sensor_data.samples,smgr_handle);
            sns_ddf_mfree_ex(state,smgr_handle);
            return stat;
        }

        bma2x2_get_dev_model_info(state);
        state->soft_reset = 1;
        stat = sns_dd_acc_bma2x2_reset(*dd_handle_ptr);
        state->soft_reset = 0;

        if (stat != SNS_DDF_SUCCESS)
        {
            sns_ddf_close_port(state->port_handle);
            sns_ddf_mfree_ex(state->f_frames_cache.samples,smgr_handle);
            sns_ddf_mfree_ex(state->sensor_data.samples,smgr_handle);
            sns_ddf_mfree_ex(state,smgr_handle);
            return stat;
        }


        (*num_sensors) = num_sensors_init;
        (*sensors) = sensors_init;
        return SNS_DDF_SUCCESS;
    }
    else
    {
        BMA2X2_MSG_3_P(HIGH, "%d %d %d",
                55555501, state->sub_dev_cnt, 12345678);
        // If there is an empty slot
        if (state->sub_dev_cnt < BMA2X2_MAX_SUB_DEV)
        {
            sns_ddf_sensor_e* sensors_init =
                sns_ddf_memhandler_malloc_ex(memhandler,
                        sizeof(sns_ddf_sensor_e)
                        * BMA2X2_MAX_NUM_SENSORS, smgr_handle);


            // Fill out the next sub-device slot
            uint32_t num_sensors_init = sns_dd_acc_bma2x2_fill_sub_dev_slot(
                    state,
                    &state->sub_dev[state->sub_dev_cnt],
                    smgr_handle,
                    device_info->first_gpio,
                    *num_sensors,
                    *sensors,
                    &sensors_init);
            if (num_sensors_init == 0)
            {
                return SNS_DDF_EINVALID_PARAM;
            }

            BMA2X2_MSG_3_P(HIGH, "%d %d %d",
                    55555501, num_sensors_init, 12345678);
            // Assign the dd_handle_ptr
            //(*dd_handle_ptr) = (sns_ddf_handle_t) ((uint32_t) state | state->sub_dev_cnt);
            //CHECK:
            (*dd_handle_ptr) = (sns_ddf_handle_t) (&state->
                    sub_dev[state->sub_dev_cnt]);

            BMA2X2_MSG_3_P(HIGH, "%d 0x%x %d",
                    55555503, (*dd_handle_ptr), 12345678);



            (*num_sensors) = num_sensors_init;
            (*sensors) = sensors_init;
            BMA2X2_MSG_3(HIGH, "%d Finished initializing %u sensor(s). First sensor %u",
                    55555501, *num_sensors, (*sensors)[0]);
            BMA2X2_MSG_3(HIGH, "%d %d %d",
                    55555501, *num_sensors, 12345678);
            BMA2X2_MSG_3(HIGH, "%d %d %d",
                    55555501, (*sensors)[0], 12345678);
            state->sub_dev_cnt++;
            BMA2X2_MSG_3_P(HIGH, "%d %d %d",
                    55555504, state->sub_dev_cnt, 12345678);
            return SNS_DDF_SUCCESS;
        }
        // Else
        else
        {

            return SNS_DDF_ENOMEM;
        }
    }
}

/*!
 *  @brief Sets the BOSCH acclerometer to the passed in operating mode
 *
 *  @detail
 *  Set the operational mode (sleep or NORMAL_MODE) by programming
 *  BMA2X2_MODE_CTRL_REG register
 *
 *  @param[in] state: Ptr to the driver structure
 *  @param[in] mode: Ptr to the variable of sns_ddf_powerstate_e type
 *
 *  @return
 *    The error code definition within the DDF
 *    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 *
 */
static sns_ddf_status_e sns_dd_acc_bma2x2_config_mode(
        sns_ddf_handle_t          dd_handle,
        sns_ddf_sensor_e          sensor_type,
        sns_ddf_powerstate_e      power_state)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    sns_dd_acc_bma2x2_state_t *state = sns_dd_acc_bma2x2_get_dev(dd_handle);

    BMA2X2_MSG_3_P(LOW, "55555617 %x %d %d",
            dd_handle,
            power_state,
            sensor_type);


    if (SNS_DDF_POWERSTATE_LOWPOWER == power_state) {
        if (sensor_type == SNS_DDF_SENSOR_ACCEL) {
            state->acc_desired_rate = 0;
        } else if (sensor_type == SNS_DDF_SENSOR_TEMP) {
            state->tmp_cur_rate = 0;
        } else if (sensor_type == SNS_DDF_SENSOR_DOUBLETAP) {
            state->dbt_cur_rate = 0;
            state->dbt_desired_rate = 0;
        }

        if ((state->acc_desired_rate == 0)
                && (state->dbt_desired_rate == 0)
                && (state->tmp_cur_rate == 0)) {
            status = bma2x2_set_powerstate(state,
                    SNS_DDF_POWERSTATE_LOWPOWER);

            if (status != SNS_DDF_SUCCESS)
                return status;
        }
    } else if (SNS_DDF_POWERSTATE_ACTIVE == power_state) {
        status = bma2x2_set_powerstate(state,
                SNS_DDF_POWERSTATE_ACTIVE);
    }

    BMA2X2_MSG_3_P(LOW, "55555618 %d %d %d",
            state->acc_desired_rate,
            state->dbt_desired_rate,
            state->tmp_cur_rate);

    return status;
}

/*!
 * @brief Find the matched internal ODR for desired ODR.

 *
 * @param[in]  desired_odr New desired ODR.
 * @param[out] new_rate          Matched internal ODR.
 * @param[out] new_index         The index of matched internal ODR in lis3dsh_odr.
 *
 * @return SNS_DDF_SUCCESS if this operation was done successfully. Otherwise
 *         SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to
 *         indicate and error has occurred.
 */
sns_ddf_status_e  sns_dd_acc_bma2x2_match_odr(
        sns_ddf_odr_t desired_odr ,
        sns_ddf_odr_t *new_rate,
        uint32_t *new_index)
{
    uint8_t idx;

    for (idx = 0; idx < BMA2X2_ODR_NUM; idx++) {
        if (desired_odr <= BMA2X2_MAP_BW_ODR[idx].l)
            break;
    }

    if (idx < BMA2X2_ODR_NUM) {
        *new_rate = BMA2X2_MAP_BW_ODR[idx].l;
        *new_index = idx;
        return SNS_DDF_SUCCESS;
    } else {
        return SNS_DDF_EINVALID_PARAM;
    }
}


sns_ddf_status_e sns_dd_acc_bma2x2_config_any_motion_lpm1(
        sns_dd_acc_bma2x2_state_t  *state)
{
    sns_ddf_status_e status;
    uint8_t         regv;
    uint16_t        odr;

    int8_t          idx_score_highest = -1;
    uint8_t         score_highest;
    uint8_t         score_curr;
    uint8_t         i;

    const struct bms_accel_any_motion_setting *any_motion_setting;

    if (BMA280_TYPE == state->sub_model) {
        odr = 32;
    } else {
        odr = 2000;
    }

    score_highest = 0;
    for (i = 0; i < BST_ARRARY_SIZE(BMA2X2_ANY_MOTION_PRESETS_LP1); i++) {
        any_motion_setting = BMA2X2_ANY_MOTION_PRESETS_LP1 + i;
        if (any_motion_setting->odr == odr) {
            score_curr = 1000 * any_motion_setting->sensitivity / any_motion_setting->pwr;

            if (score_curr > score_highest) {
                score_highest = score_curr;
                idx_score_highest = i;
            }
        }
    }

    if (idx_score_highest < 0) {
        return SNS_DDF_EINVALID_DATA;
    }

    any_motion_setting = BMA2X2_ANY_MOTION_PRESETS_LP1 + idx_score_highest;

    //register settings
    regv = any_motion_setting->threshold >> state->range_idx;
    if (0 == regv) {
        regv = 1;
    }

    status = bma2x2_smbus_write_byte(state->port_handle, BMA2X2_SLOPE_THRES_REG, &regv);
    if (SNS_DDF_SUCCESS != status) {
        return status;
    }

    regv = 0;
    regv = BST_SET_VAL_BITBLOCK(regv, 0, 1, any_motion_setting->dur);
    status = bma2x2_smbus_write_byte(state->port_handle, BMA2X2_SLOPE_DURN_REG, &regv);
    if (SNS_DDF_SUCCESS != status) {
        return status;
    }

    regv = 0x00;
    regv = BST_SET_VAL_BITBLOCK(regv, 0, 4, any_motion_setting->bw);
    status = bma2x2_smbus_write_byte(state->port_handle, BMA2X2_BW_SEL_REG, &regv);
    if (SNS_DDF_SUCCESS != status) {
        return status;
    }

    regv = 0x00;
    regv = BST_SET_VAL_BIT(regv, 6);
    regv = BST_SET_VAL_BITBLOCK(regv, 1, 4, any_motion_setting->sleep_dur);
    status = bma2x2_smbus_write_byte(state->port_handle, BMA2X2_MODE_CTRL_REG, &regv);
    if (SNS_DDF_SUCCESS != status) {
        return status;
    }

    state->md_lpm = true;
    BMA2X2_MSG_0(HIGH, "BMA2X2 is now in md_lpm due to MD request");

    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_acc_bma2x2_get_out_of_mdlpm(
        sns_dd_acc_bma2x2_state_t *state)
{
    uint8_t                 regv;
    uint8_t                 regv_f_cfg_1;
    sns_ddf_status_e        status;

    status = bma2x2_smbus_read_byte(state->port_handle, BMA2X2_FIFO_MODE_REG, &regv_f_cfg_1);
    regv = 0;
    status |= bma2x2_smbus_write_byte(state->port_handle, BMA2X2_MODE_CTRL_REG, &regv);
    sns_ddf_delay(500);
    status |= bma2x2_smbus_write_byte(state->port_handle, BMA2X2_FIFO_MODE_REG, &regv_f_cfg_1);

    if (SNS_DDF_SUCCESS != status) {
        return status;
    }

    state->md_lpm = false;
    BMA2X2_MSG_0(HIGH, "BMA2x2 gets out of md_lpm due to motion event");

    return SNS_DDF_SUCCESS;
}

/*!
 *  @brief Sets an attribute of the bosch accelerometer
 *
 * @detail
 *  Called by SMGR to set certain device attributes that are
 *  programmable. Curently its the power mode and range.
 *
 *  @param[in] dd_handle   Handle to a driver instance.
 *  @param[in] sensor Sensor for which this attribute is to be set.
 *  @param[in] attrib      Attribute to be set.
 *  @param[in] value      Value to set this attribute.
 *
 *  @return
 *    The error code definition within the DDF
 *    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
 *    SNS_DDF_EINVALID_PARAM
 *
 */
sns_ddf_status_e sns_dd_acc_bma2x2_set_attr(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensor_type,
        sns_ddf_attribute_e     attrib,
        void                    *value)
{
    sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;
    sns_dd_acc_bma2x2_state_t *state;
    bma2x2_sub_dev_t *sub_dev;
    //  bma2x2_sensor_t *sensor;
    bma2x2_sub_dev_t* accel_sub_dev = NULL;
    uint32_t signal_registered;

    uint8_t n;

    BMA2X2_MSG_4(HIGH, "<bma2x2_if_ set_attrib> 0x%x sensor: %d attr: %d val: %d",
            dd_handle, sensor_type, attrib, *(uint8_t *)value);

    if (dd_handle == NULL || value == NULL)
    {
        BMA2X2_MSG_3_P(ERROR, "%d 0x%x %d",
                55555521, 55555521, 55555521);
        return SNS_DDF_EINVALID_PARAM;
    }

    // If the requested sensor type is NONE, then do nothing
    if (sensor_type == SNS_DDF_SENSOR__NONE)
    {
        BMA2X2_MSG_3_P(ERROR, "%d 0x%x %d",
                55555520, 55555520, 55555520);
        return SNS_DDF_SUCCESS;
    }

    sub_dev = sns_dd_acc_bma2x2_get_subdev(dd_handle);

    if (NULL == sub_dev)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    BMA2X2_MSG_3_P(MEDIUM, "%d %d %d",
            55555619, sub_dev, dd_handle);

    state = sns_dd_acc_bma2x2_get_dev(dd_handle);
    if (NULL == state)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    /* If the requested sensor type is ALL, then iterate through all of the
     * sub-device's sensors and call this function with a specific sensor type */
    if (sensor_type == SNS_DDF_SENSOR__ALL)
    {
        for (n = 0; n < sub_dev->num_sensors; n++)
        {
            ret_val = sns_dd_acc_bma2x2_set_attr(dd_handle, sub_dev->sensors[n].sensor_type, attrib, value);
            if (ret_val != SNS_DDF_SUCCESS)
                return ret_val;
        }
        return SNS_DDF_SUCCESS;
    }


    BMA2X2_MSG_3_P(MEDIUM, "%d 0x%x 0x%x",
            55555522, state, sub_dev);

    BMA2X2_MSG_3_P(MEDIUM, "%d %d %d",
            55555523,
            (int)sub_dev - (int)state,
            sizeof(*sub_dev));

    switch (attrib)
    {
        case SNS_DDF_ATTRIB_POWER_STATE:
            /*! set power mode */
            {
                sns_ddf_powerstate_e power_state = *(sns_ddf_powerstate_e *)value;
                BMA2X2_MSG_2(MEDIUM,
                        "bma2x2 set power state for sensor: %d with value: %d",
                        sensor_type,
                        power_state);

                ret_val = sns_dd_acc_bma2x2_config_mode(dd_handle,
                        sensor_type,
                        power_state);
            }
            break;

        case SNS_DDF_ATTRIB_RANGE:
            /*! change the range for accel */
            if (sensor_type == SNS_DDF_SENSOR_ACCEL)
            {
                if (*((uint8_t *)value) >= SNS_DD_ACCEL_MAX_RANGE)
                {
                    return SNS_DDF_EINVALID_ATTR;
                }
                else
                {
                    state->range_idx = *((uint8_t *)value);
                    state->range = BMA2X2_ACC_RANGE_LIST[state->range_idx];
                }

                ret_val = sns_dd_acc_bma2x2_config_range(state);
            }
            else
            {
                return SNS_DDF_EINVALID_PARAM;
            }
            break;

        case SNS_DDF_ATTRIB_LOWPASS:
            if (sensor_type == SNS_DDF_SENSOR_ACCEL)
            {
                if (*((uint8_t *)value) >= SNS_DD_ACCEL_MAX_BW)
                {
                    return SNS_DDF_EINVALID_ATTR;
                }
                else
                {
                    ret_val = sns_dd_acc_bma2x2_config_bw(state,
                            *((uint8_t *)value));
                }
            }
            else
            {
                return SNS_DDF_EINVALID_PARAM;
            }

            break;

        case SNS_DDF_ATTRIB_RESOLUTION_ADC:
            break;

        case SNS_DDF_ATTRIB_ODR:
            {
                sns_ddf_odr_t desired_odr = *((sns_ddf_odr_t *)value);
                sns_ddf_odr_t new_rate = 0;
                uint32_t new_rate_index = 0;

                BMA2X2_MSG_3_P(MEDIUM, "%d %d %d",
                        55555526, desired_odr, state->acc_cur_rate);


                if (SNS_DDF_SENSOR_ACCEL == sensor_type)
                {
                    if (0 == desired_odr) {
                        state->acc_cur_rate = 0;
                        state->acc_desired_rate = 0;
                        return SNS_DDF_SUCCESS;
                    }

                    /*!find the matched internal ODR for desired ODR */
                    ret_val = sns_dd_acc_bma2x2_match_odr(desired_odr,
                            &new_rate,
                            &new_rate_index);

                    BMA2X2_MSG_3_P(MEDIUM, "55555526 %d %d %d",
                            new_rate, state->acc_cur_rate, state->dbt_desired_rate);

                    if (SNS_DDF_SUCCESS != ret_val)
                        return SNS_DDF_EINVALID_PARAM;

                    state->acc_desired_rate = desired_odr;
                    //TODO: choose MAX ODR among accel (could be > 200HZ) /dbt(200HZ)/temp(1HZ)
                    if ((new_rate != state->acc_cur_rate) && (0 == state->dbt_desired_rate))
                    {

                        ret_val = sns_dd_acc_bma2x2_config_bw(state, new_rate_index);

                        if (SNS_DDF_SUCCESS == ret_val)
                        {
                            state->acc_cur_rate = new_rate;
                            return ret_val;
                        }
                        else
                        {
                            return ret_val;
                        }
                    }
                    else
                    {
                        if (state->dbt_desired_rate > 0)
                            //DBT is in use
                        {
                            if (new_rate > state->dbt_cur_rate) {
                                BMA2X2_MSG_3_P(MEDIUM, "%d %d %d",
                                        55555528, desired_odr, new_rate);

                                state->acc_cur_rate = new_rate;
                                state->dbt_cur_rate = new_rate;
                                ret_val = sns_dd_acc_bma2x2_config_bw(state, new_rate_index);
                            } else {
                                state->acc_cur_rate = state->dbt_cur_rate;
                            }
                        }

                        return SNS_DDF_SUCCESS;
                    }
                }
                else if (SNS_DDF_SENSOR_TEMP == sensor_type)
                {
                    //TODO: check impact on ODR of other sensors
                    state->tmp_cur_rate = desired_odr;
                    return SNS_DDF_SUCCESS;
                }
                else if (SNS_DDF_SENSOR_DOUBLETAP == sensor_type)
                {
                    new_rate = desired_odr;

                    signal_registered = ((state->en_md) || (state->en_db_tap_int)
                            || (state->en_dri) || (state->en_fifo_int));

                    BMA2X2_MSG_3_P(MEDIUM, "55555529 %d %d %d",
                            desired_odr, signal_registered, state->acc_cur_rate);
                    /*!enable double tap */
                    if (new_rate > 0)
                    {
                        state->en_db_tap_int = true;

                        state->dbt_desired_rate = new_rate;
                        state->dbt_cur_rate = new_rate;

                        if (state->dbt_desired_rate != state->acc_cur_rate)
                        {
                            ret_val =  sns_dd_acc_bma2x2_match_odr(
                                    max(BMA2X2_DBT_MIN_ODR, state->acc_desired_rate),
                                    &new_rate,
                                    &new_rate_index);

                            if (SNS_DDF_SUCCESS != ret_val)
                                return SNS_DDF_EINVALID_PARAM;

                            state->dbt_cur_rate = new_rate;
                            BMA2X2_MSG_3_P(MEDIUM, "55555529 %d %d %d",
                                    desired_odr, new_rate ,new_rate_index);

                            ret_val = sns_dd_acc_bma2x2_config_bw(state, new_rate_index);

                            if (state->acc_desired_rate > 0) {
                                if (state->acc_cur_rate != new_rate) {
                                    state->acc_cur_rate = new_rate;

                                    accel_sub_dev = sns_dd_acc_bma2x2_get_subdev_from_type(state,
                                            SNS_DDF_SENSOR_ACCEL);

                                    /*! Notify the SMGR of an ODR Changed Event */
                                    if (accel_sub_dev != NULL)
                                        sns_ddf_smgr_notify_event(accel_sub_dev->smgr_handle,
                                                SNS_DDF_SENSOR_ACCEL,
                                                SNS_DDF_EVENT_ODR_CHANGED);

                                    BMA2X2_MSG_3_P(HIGH, "%d %d %d",
                                            55555529, 77777777, 77777777);
                                }
                            }
                        }

                        ret_val = bma2x2_enable_doubletap_int(state->port_handle, 1);
                        if (SNS_DDF_SUCCESS != ret_val) {
                            return ret_val;
                        }

                        ret_val = sns_dd_acc_bma2x2_signal_register(dd_handle, signal_registered);
                        BMA2X2_MSG_3_P(HIGH, "%d %d %d",
                                ret_val, desired_odr, new_rate);
                    }
                    else if (0 == new_rate)
                    {
                        state->en_db_tap_int = FALSE;
                        ret_val = sns_dd_acc_bma2x2_signal_deregister(dd_handle, signal_registered);
                        ret_val = bma2x2_enable_doubletap_int(state->port_handle, 0);

                        BMA2X2_MSG_3_P(MEDIUM, "%d %d %d",
                                55555527, ret_val, 1234567);

                        if (SNS_DDF_SUCCESS != ret_val)
                            return ret_val;

                        state->dbt_desired_rate = 0;
                        state->dbt_cur_rate = 0;

                        if (state->acc_desired_rate > 0) {
                            ret_val = sns_dd_acc_bma2x2_match_odr(state->acc_desired_rate,
                                    &new_rate,
                                    &new_rate_index);

                            if (SNS_DDF_SUCCESS != ret_val)
                                return SNS_DDF_EINVALID_PARAM;
                        }

                        if (new_rate != state->acc_cur_rate)
                        {
                            BMA2X2_MSG_3_P(MEDIUM, "%d %d %d",
                                    55555527, state->acc_cur_rate, new_rate);

                            ret_val = sns_dd_acc_bma2x2_config_bw(state, new_rate_index);

                            if (SNS_DDF_SUCCESS == ret_val)
                            {
                                state->acc_cur_rate = new_rate;
                            }
                            else
                            {
                                return ret_val;
                            }
                            if (state->acc_desired_rate > 0)
                            {
                                accel_sub_dev = sns_dd_acc_bma2x2_get_subdev_from_type(state,
                                        SNS_DDF_SENSOR_ACCEL);

                                /*! Notify the SMGR of an ODR Changed Event*/
                                if (accel_sub_dev != NULL)
                                    sns_ddf_smgr_notify_event(accel_sub_dev->smgr_handle,
                                            SNS_DDF_SENSOR_ACCEL,
                                            SNS_DDF_EVENT_ODR_CHANGED);

                                BMA2X2_MSG_3_P(HIGH, "%d %d %d",
                                        55555527, 3333333, 3333333);

                            }
                        }

                    }

                }
                else
                {
                    return SNS_DDF_EINVALID_PARAM;
                }

                break;
            }

        case SNS_DDF_ATTRIB_MOTION_DETECT:
            {
                bool enable = *((bool *)value);

                if (!sns_ddf_signal_irq_enabled())
                {
                    return SNS_DDF_EINVALID_PARAM;
                }

                signal_registered = ((state->en_md) || (state->en_db_tap_int)
                        || (state->en_dri) || (state->en_fifo_int));
                if (enable)
                {
                    if ((ret_val = sns_dd_acc_bma2x2_config_motion_int(
                                    state, 1)) != SNS_DDF_SUCCESS)
                    {
                        return ret_val;
                    }


                    if ((0 == state->acc_desired_rate)
                            && (0 == state->dbt_desired_rate)
                            && (0 == state->tmp_cur_rate)) {
#if BMA2X2_CONFIG_ENABLE_MD_LPM
                        sns_dd_acc_bma2x2_config_any_motion_lpm1(state);
#else
                        if ((ret_val = sns_dd_acc_bma2x2_config_motion_detect_thresh(
                                        state)) != SNS_DDF_SUCCESS)
                        {
                            return ret_val;
                        }
                        if ((ret_val = sns_dd_acc_bma2x2_config_motion_detect_duration(
                                        state)) != SNS_DDF_SUCCESS)
                        {
                            return ret_val;
                        }

                        ret_val = sns_dd_acc_bma2x2_config_bw(state,
                                BMA2X2_REGV_BW_31_25HZ - BMA2X2_REGV_BW_BASE);
#endif
                    }

                    ret_val = sns_dd_acc_bma2x2_signal_register(dd_handle, signal_registered);

                    return ret_val;
                }
                else
                {

#if BMA2X2_CONFIG_ENABLE_MD_LPM
                    sns_dd_acc_bma2x2_get_out_of_mdlpm(state);
#endif
                    if (state->en_md)
                    {

                        if ((ret_val = sns_dd_acc_bma2x2_config_motion_int(
                                        state, 0)) != SNS_DDF_SUCCESS)
                        {
                            return ret_val;
                        }

                        ret_val = sns_dd_acc_bma2x2_signal_deregister(dd_handle, signal_registered);
                        return ret_val;
                    }
                    else
                    {
                        return SNS_DDF_SUCCESS;
                    }
                }
            }

        case SNS_DDF_ATTRIB_IO_REGISTER:
            break;

        case SNS_DDF_ATTRIB_FIFO:
            if (sensor_type == SNS_DDF_SENSOR_ACCEL)
            {
                /* we are using STREAM mode */

                if (*((uint8_t *)value) >= BMA2X2_MAX_FIFO_LEVEL)
                {
                    return SNS_DDF_EINVALID_PARAM;
                }
                else
                {

#if BMA2X2_CONFIG_ENABLE_DELAYED_WML_CFG
                    if ((*(uint8_t *)value) > 1) {
                        if (state->sample_cnt_to_skip_odrc > 0) {
                            state->f_wml = 0;
                            state->f_wml_pending = *((uint8_t *)value);
                            state->f_wml_cfg_pending = 1;

                            BMA2X2_MSG_1(HIGH, "wml req pending odrc: %d",
                                    state->sample_cnt_to_skip_odrc);

                            return SNS_DDF_SUCCESS;
                        }
                    }
#endif

                    state->f_wml = *((uint8_t *)value);

                    if (state->f_wml > 0) {
                        ret_val = bma2x2_set_fifo_wml_trig(state->port_handle,
                                state->f_wml);

                        if (SNS_DDF_SUCCESS != ret_val)
                            return ret_val;
                    }

                    /* this will purge the FIFO */
                    ret_val = bma2x2_config_fifo_mode(state->port_handle,
                            state->f_wml);

                    if (SNS_DDF_SUCCESS != ret_val)
                        return ret_val;

                    if (state->f_wml > 0) {
                        sns_ddf_time_t ts_curr;

                        ts_curr = sns_ddf_get_timestamp();
                        state->ts_last_sample = ts_curr;
                        state->ts_1st_sample_est = ts_curr + state->ts_itvl_est;

                        state->f_acnt_frame_cnt_irq = 0;
                        state->f_acnt_frame_cnt = 0;
                        BMA2X2_MSG_2(HIGH, "ts_1st_sample_est: <%d %d>",
                                ts_curr, state->ts_1st_sample_est);

                        //we read this value out as a cache so that in the future we just need to write it back when needed
                        //the intention is to reduce I2C traffic and increase response
                        state->regv_fifo_config_1 = 0x0c;
                        ret_val = bma2x2_smbus_read_byte(state->port_handle,
                                BMA2X2_REGA_FIFO_CONFIG_1, &state->regv_fifo_config_1);

                        if (SNS_DDF_SUCCESS != ret_val)
                            return ret_val;
                    }
                }
            }
            else
            {
                return SNS_DDF_EINVALID_PARAM;
            }

            break;

        default:
            return SNS_DDF_EINVALID_PARAM;
    }

    return (ret_val);
}



/*!
 *  @brief Configure the motion interrupt threshold
 *
 *  @detail
 *  Programs the register BMA2X2_SLOPE_THRES_REG for the MD
 *  threshold
 *
 *  @param[in] state: Ptr to the driver structure
 *
 *  @return
 *  The error code definition within the DDF
 *  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 *
 */
static sns_ddf_status_e sns_dd_acc_bma2x2_config_motion_detect_thresh(
        sns_dd_acc_bma2x2_state_t *state)
{
    uint8_t bosch_write_buffer;

    bosch_write_buffer = BMA2X2_MD_THRESHOLD >> state->range_idx;
    if (0 == bosch_write_buffer) {
        bosch_write_buffer = 1;
    }

    return bma2x2_smbus_write_byte(state->port_handle,
            BMA2X2_SLOPE_THRES_REG,
            &bosch_write_buffer);
}


#if !BMA2X2_CONFIG_ENABLE_MD_LPM
static sns_ddf_status_e sns_dd_acc_bma2x2_config_motion_detect_duration(
        sns_dd_acc_bma2x2_state_t *state)
{
    uint8_t bosch_write_buffer;

    bosch_write_buffer = BMA2X2_MD_DUR;

    return bma2x2_smbus_write_byte(state->port_handle,
            BMA2X2_SLOPE_DURN_REG,
            &bosch_write_buffer);
}

#endif
/*!
 *  @brief Called by the SMGR to retrieves the value of an attribute of
 *  the sensor.
 *
 *  @detail
 *  - range and resolution info is from the device data sheet.
 *
 *  @param[in]  dd_handle   Handle to a driver instance.
 *  @param[in] sensor       Sensor whose attribute is to be retrieved.
 *  @param[in]  attrib      Attribute to be retrieved.
 *  @param[in] memhandler  Memory handler used to dynamically allocate
 *                         output parameters, if applicable.
 *  @param[out] value      Pointer that this function will allocate or set
 *                         to the attribute's value.
 *  @param[out] num_elems  Number of elements in @a value.
 *
 *  @return
 *    The error code definition within the DDF
 *    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 *
 */
sns_ddf_status_e sns_dd_acc_bma2x2_get_attr(
        sns_ddf_handle_t       dd_handle,
        sns_ddf_sensor_e       sensor_type,
        sns_ddf_attribute_e    attrib,
        sns_ddf_memhandler_s*  memhandler,
        void**                 value,
        uint32_t*              num_elems)
{
    sns_dd_acc_bma2x2_state_t *state = sns_dd_acc_bma2x2_get_dev(dd_handle);;
    bma2x2_sub_dev_t *sub_dev;
    //bma2x2_sensor_t *sensor = NULL;

    BMA2X2_MSG_3(HIGH, "<bma2x2_if_ get_attrib> 0x%x sensor: %d attr: %d",
            dd_handle, sensor_type, attrib);

    BMA2X2_MSG_3_P(LOW, "%d %d %d", 55555531, sensor_type, attrib);

    //state = sns_dd_acc_bma2x2_get_dev(dd_handle);
    if (NULL == state)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    //sub_dev = sns_dd_acc_bma2x2_get_subdev(dd_handle);
    sub_dev = sns_dd_acc_bma2x2_get_subdev_from_type(state,
            sensor_type);
    if (NULL == sub_dev)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    /*
       if (SNS_DDF_SENSOR__ALL != sensor_type)
       {
       sensor = sns_dd_acc_bma2x2_get_sensor_from_type(sub_dev,
       sensor_type);

       BMA2X2_MSG_3_P(ERROR, "%d %d 0x%x",
       55555524, sensor_type, sensor);

       if (NULL == sensor)
       {
       return SNS_DDF_EINVALID_PARAM;
       }
       }
       */

    switch (attrib)
    {
        case SNS_DDF_ATTRIB_POWER_INFO:
            {
                sns_ddf_power_info_s* power_attrib;
                if ((*value = sns_ddf_memhandler_malloc_ex(memhandler,
                                sizeof(sns_ddf_power_info_s),sub_dev->smgr_handle)) == NULL)
                {
                    return SNS_DDF_ENOMEM;
                }
                power_attrib = *value;
                power_attrib->active_current = 130;
                power_attrib->lowpower_current = 2;
                *num_elems = 1;
            }
            break;

        case SNS_DDF_ATTRIB_RANGE:
            {
                sns_ddf_range_s *device_ranges;

                if (sensor_type == SNS_DDF_SENSOR_ACCEL)
                {
                    if ((*value = sns_ddf_memhandler_malloc_ex(memhandler,
                                    SNS_DD_ACCEL_MAX_RANGE
                                    * sizeof(sns_ddf_range_s),sub_dev->smgr_handle))
                            == NULL)
                    {
                        return SNS_DDF_ENOMEM;
                    }
                    device_ranges = *value;
                    *num_elems = 4;
                    device_ranges[0].min = BOSCH_ACCEL_SENSOR_RANGE_2G_MIN;
                    device_ranges[0].max = BOSCH_ACCEL_SENSOR_RANGE_2G_MAX;
                    device_ranges[1].min = BOSCH_ACCEL_SENSOR_RANGE_4G_MIN;
                    device_ranges[1].max = BOSCH_ACCEL_SENSOR_RANGE_4G_MAX;
                    device_ranges[2].min = BOSCH_ACCEL_SENSOR_RANGE_8G_MIN;
                    device_ranges[2].max = BOSCH_ACCEL_SENSOR_RANGE_8G_MAX;
                    device_ranges[3].min = BOSCH_ACCEL_SENSOR_RANGE_16G_MIN;
                    device_ranges[3].max = BOSCH_ACCEL_SENSOR_RANGE_16G_MAX;
                }
                else if (sensor_type == SNS_DDF_SENSOR_TEMP)
                {
                    if ((*value = sns_ddf_memhandler_malloc_ex(memhandler,
                                    sizeof(sns_ddf_range_s),sub_dev->smgr_handle)) == NULL)
                    {
                        return SNS_DDF_ENOMEM;
                    }
                    device_ranges = *value;
                    *num_elems = 1;
                    device_ranges->min = BOSCH_ACCEL_SENSOR_TEMPER_MIN;
                    device_ranges->max = BOSCH_ACCEL_SENSOR_TEMPER_MAX;
                } else {
                    //CHECK: following_ref_code_now
                    return SNS_DDF_EINVALID_PARAM;
                }
            }
            break;

        case SNS_DDF_ATTRIB_RESOLUTION_ADC:
            {
                sns_ddf_resolution_adc_s *device_res;
                if ((*value = sns_ddf_memhandler_malloc_ex(memhandler,
                                sizeof(sns_ddf_resolution_adc_s),sub_dev->smgr_handle))
                        == NULL)
                {
                    return SNS_DDF_ENOMEM;
                }

                device_res = *value;

                *num_elems = 1;
                if (sensor_type == SNS_DDF_SENSOR_ACCEL)
                {
                    device_res->bit_len = BOSCH_ACCEL_SENSOR_ACC_BIT_LEN;
                    device_res->max_freq = BMA2X2_ACC_MAX_ODR;
                }
                else if (sensor_type == SNS_DDF_SENSOR_TEMP)
                {
                    device_res->bit_len = BOSCH_ACCEL_SENSOR_TEMPER_BIT_LEN;
                    device_res->max_freq = BMA2X2_ACC_MAX_ODR;
                }
                //NOTE:CRITICAL: if the max_freq is not filled properly
                //the SMGR does not interact with this driver later any more
                //CHECK: following_ref_code_now
                else if (sensor_type == SNS_DDF_SENSOR_DOUBLETAP)
                {
                    device_res->bit_len = BOSCH_ACCEL_SENSOR_TEMPER_BIT_LEN;
                    device_res->max_freq = BMA2X2_DBT_MIN_ODR;
                }
            }
            break;

        case SNS_DDF_ATTRIB_RESOLUTION:
            {
                sns_ddf_resolution_t *device_res;
                uint32_t lsb_per_g;

                if ((*value = sns_ddf_memhandler_malloc_ex(memhandler,
                                sizeof(sns_ddf_resolution_t),sub_dev->smgr_handle))
                        == NULL)
                {
                    return SNS_DDF_ENOMEM;
                }

                device_res = *value;

                *num_elems = 1;
                if (sensor_type == SNS_DDF_SENSOR_ACCEL)
                {
                    lsb_per_g = BMA2X2_LSB_RANGE_LIST[state->sub_model]
                        / (4 << state->range_idx);
                    *device_res = FX_FLTTOFIX_Q16(G / lsb_per_g);
                }
                else if (sensor_type == SNS_DDF_SENSOR_TEMP)
                {
                    *device_res = FX_FLTTOFIX_Q16(1.0); /*! Degrees C */
                }
                else if (sensor_type == SNS_DDF_SENSOR_DOUBLETAP)
                {
                    //CHECK: following_ref_code_now
                    *device_res = 1;
                }
            }
            break;

        case SNS_DDF_ATTRIB_LOWPASS:
            {
                uint8_t i;
                sns_ddf_lowpass_freq_t *freq_set;
                *num_elems = 8;
                if ((*value = sns_ddf_memhandler_malloc_ex(memhandler,
                                (*num_elems)*sizeof(sns_ddf_lowpass_freq_t),sub_dev->smgr_handle))
                        == NULL)
                {
                    return SNS_DDF_ENOMEM;
                }
                freq_set = *value;

                *num_elems = 8;
                for (i = 0; i<*num_elems; i++)
                {
                    *(freq_set+i) = BMA2X2_ACC_FREQ_LIST[i];
                }
            }
            break;

        case SNS_DDF_ATTRIB_DELAYS:
            {
                sns_ddf_delays_s *device_delay;
                if ((*value = sns_ddf_memhandler_malloc_ex(memhandler,
                                sizeof(sns_ddf_delays_s),sub_dev->smgr_handle)) == NULL)
                {
                    return SNS_DDF_ENOMEM;
                }
                device_delay = *value;
                *num_elems = 1;
                device_delay->time_to_active = 3000; /*! 3ms is the startup time*/
                device_delay->time_to_data = 330;
            }
            break;

        case SNS_DDF_ATTRIB_DRIVER_INFO:
            {
                BMA2X2_MSG_0(LOW, "Getting attribute DRIVER INFO");
                *value = &bma2x2_sns_dd_list_head.dd_info;
                *num_elems = 1;
                return SNS_DDF_SUCCESS;
            }

        case SNS_DDF_ATTRIB_DEVICE_INFO:
            {
                sns_ddf_device_info_s *device_info;
                if ((*value = sns_ddf_memhandler_malloc_ex(memhandler,
                                sizeof(sns_ddf_device_info_s),sub_dev->smgr_handle))
                        == NULL)
                {
                    return SNS_DDF_ENOMEM;
                }
                device_info = *value;
                *num_elems = 1;
                device_info->vendor = "BOSCH";
                if (BMA2X2_DEV_SEL_NUM_BMI058 == state->dev_select) {
                    device_info->model = "BMI058";
                } else if (BMA2X2_DEV_SEL_NUM_BMI055 == state->dev_select) {
                    device_info->model = "BMI055";
                } else {
                    device_info->model = "BMA2X2";
                }
                device_info->name = "Accelerometer/Temperature/Double-tap";
                device_info->version = BMA2X2_SNS_DDF_ATTRIB_VERSION;
            }
            break;

        case SNS_DDF_ATTRIB_IO_REGISTER:
            {
                sns_ddf_io_register_s* io_reg_in = (sns_ddf_io_register_s *) (*value);
                uint8_t bytes_read;
                sns_ddf_status_e status;

                if ((uint32_t)(io_reg_in->address_ptr) > 0xFFFF)
                {
                    return SNS_DDF_EINVALID_PARAM;
                }
                if (io_reg_in->length > 0xFF)
                {
                    return SNS_DDF_EINVALID_PARAM;
                }

                // Allocate memory for the output data structure and for the data to be read from the register(s)
                sns_ddf_io_register_s* io_reg_out = sns_ddf_memhandler_malloc_ex(
                        memhandler, sizeof(sns_ddf_io_register_s) + (sizeof(uint8_t) * (io_reg_in->length - 1)),sub_dev->smgr_handle);
                if (io_reg_out == NULL)
                    return SNS_DDF_ENOMEM;

                status = sns_ddf_read_port(
                        state->port_handle,
                        (uint16_t) ((uint32_t)io_reg_in->address_ptr),
                        &(io_reg_out->data[0]),
                        (uint8_t) io_reg_in->length,
                        &bytes_read);
                if (status != SNS_DDF_SUCCESS)
                    return status;

                // Fill out the information in the output io_register data structure
                io_reg_out->address_ptr = io_reg_in->address_ptr;
                io_reg_out->addr_fixed = io_reg_in->addr_fixed;
                io_reg_out->data_width = io_reg_in->data_width;
                io_reg_out->length = (uint32_t) bytes_read;
                // Return the io_register data structure by reference
                *value = io_reg_out;
                *num_elems = 1;

            }
            break;
        case SNS_DDF_ATTRIB_BIAS:
            {
                q16_t *bias = NULL;

                bias = sns_ddf_memhandler_malloc_ex(memhandler,
                                sizeof(q16_t) * 3,
                                sub_dev->smgr_handle);
                if (NULL == bias) {
                    return SNS_DDF_ENOMEM;
                }

                if (SNS_DDF_SENSOR_ACCEL == sensor_type) {
                    uint32_t i;
                    for (i = 0; i < 3; i++) {
                        bias[i] = state->bias[i];
                    }

                    *value = bias;
                    *num_elems = 3;
                    return SNS_DDF_SUCCESS;
                } else {
                    //CHECK: following_ref_code_now
                    return SNS_DDF_EINVALID_PARAM;
                }
            }
        case SNS_DDF_ATTRIB_ODR:
            {
                // for both
                sns_ddf_odr_t *res = sns_ddf_memhandler_malloc_ex(
                        memhandler ,sizeof(sns_ddf_odr_t),sub_dev->smgr_handle);
                if (NULL == res)
                    return SNS_DDF_ENOMEM;

                if (SNS_DDF_SENSOR_ACCEL == sensor_type)
                    *res = state->acc_cur_rate;
                else if (SNS_DDF_SENSOR_TEMP == sensor_type)
                    *res = state->tmp_cur_rate;
                else if (SNS_DDF_SENSOR_DOUBLETAP == sensor_type)
                    *res = state->dbt_cur_rate;
                else
                    return SNS_DDF_EINVALID_PARAM;

                *value = res;
                *num_elems = 1;

                BMA2X2_MSG_3_P(LOW, "%d %d %d", 55555534, SNS_DDF_ATTRIB_ODR, *res);
                BMA2X2_MSG_3_P(LOW, "%d %d %d", 55555535, sensor_type, *res);

                return SNS_DDF_SUCCESS;
            }

        case SNS_DDF_ATTRIB_FIFO:
            {
                if (sensor_type == SNS_DDF_SENSOR_ACCEL)
                {
                    // for ACCEL
                    sns_ddf_fifo_attrib_get_s *fifo_attrib_info = sns_ddf_memhandler_malloc_ex(
                            memhandler ,sizeof(sns_ddf_fifo_attrib_get_s), sub_dev->smgr_handle);
                    if (NULL == fifo_attrib_info)
                        return SNS_DDF_ENOMEM;

                    fifo_attrib_info->is_supported = true;
                    fifo_attrib_info->is_sw_watermark = false;
                    fifo_attrib_info->max_watermark = 32;
                    fifo_attrib_info->share_sensor_cnt = 0;
                    fifo_attrib_info->share_sensors[0] =  NULL;
                    *value = fifo_attrib_info;
                    *num_elems = 1;

                    return SNS_DDF_SUCCESS;
                }
                else
                {
                    return SNS_DDF_EINVALID_ATTR;
                }

                break;
            }


        default:
            return SNS_DDF_EINVALID_PARAM;
    }

    return SNS_DDF_SUCCESS;
}



static void sns_dd_acc_bma2x2_test_notify(
        sns_ddf_handle_t handle,
        sns_ddf_sensor_e sensor_type,
        sns_ddf_status_e status,
        uint32_t err,
        q16_t* biases)
{
    static const uint8_t num_axis = 3;
    sns_dd_acc_bma2x2_state_t *state = sns_dd_acc_bma2x2_get_dev(handle);
    bma2x2_sub_dev_t *sub_dev;
    q16_t* biases_ptr;
    uint8_t i;

    if (state == NULL)
    {
        BMA2X2_MSG_0(ERROR, "sns_dd_acc_bma2x2_test_notify - NULL state");
        return;
    }
    sub_dev = sns_dd_acc_bma2x2_get_subdev_from_type(state, sensor_type);
    if (sub_dev == NULL)
    {
        BMA2X2_MSG_0(ERROR, "sns_dd_acc_bma2x2_test_notify - NULL sub_dev");
        return;
    }
    BMA2X2_MSG_1(HIGH, "sns_dd_acc_bma2x2_test_notify - sensor=%d", sensor_type);
    //CHECK: this is different with before
    if (err != SNS_DD_ITEST_ZERO_VARIANCE)
    {
        BMA2X2_MSG_1(HIGH, "err != SNS_DD_ITEST_ZERO_VARIANCE err=%d", err);
        sns_ddf_smgr_notify_test_complete(sub_dev->smgr_handle,
                sensor_type,
                status, err);
    }
    else
    {
        /* Overwrite err with SNS_DD_ITEST_SUCCESS and
         * status with SNS_DDF_SUCCESS in case of zero variance */
        BMA2X2_MSG_0(HIGH, "err == SNS_DD_ITEST_ZERO_VARIANCE");
        sns_ddf_smgr_notify_test_complete(sub_dev->smgr_handle,
                sensor_type,
                SNS_DDF_SUCCESS, SNS_DD_ITEST_SUCCESS);

    }

    if ((status == SNS_DDF_SUCCESS) || (err == SNS_DD_ITEST_ZERO_VARIANCE))
    {
        biases_ptr = state->bias;

        for (i = 0; i < num_axis; i++)
            biases_ptr[i] = biases[i];

        sns_ddf_smgr_notify_event(sub_dev->smgr_handle,
                sensor_type,
                SNS_DDF_EVENT_BIAS_READY);

    }
}

static sns_ddf_status_e sns_dd_acc_bma2x2_run_test_self(
        sns_dd_acc_bma2x2_state_t *state,
        sns_ddf_sensor_e sensor_type,
        uint32_t* err)
{
    sns_ddf_status_e        status = SNS_DDF_SUCCESS;
    uint8_t                 si_buf = 0;
    uint8_t                 bytes_read;

    if ((status = sns_ddf_read_port(state->port_handle,
                    BMA2X2_CHIP_ID_REG,
                    &si_buf,
                    1,
                    &bytes_read)) != SNS_DDF_SUCCESS) {
        return status;
    }

    if (!((BMA222E_CHIP_ID == si_buf)
                ||(BMA250E_CHIP_ID == si_buf)
                || (BMA255_CHIP_ID == si_buf)
                ||(BMA280_CHIP_ID == si_buf))) {
        *err = si_buf;
        return SNS_DDF_EDEVICE;
    }

    *err = 0;
    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_acc_bma2x2_self_test(
        sns_ddf_handle_t dd_handle,
        sns_ddf_sensor_e sensor_type,
        sns_ddf_test_e test,
        uint32_t* err)
{
    sns_dd_acc_bma2x2_state_t *state = sns_dd_acc_bma2x2_get_dev(dd_handle);
    sns_ddf_status_e stat;

    unsigned char clear_value = 0;
    short value1 = 0;
    short value2 = 0;
    short diff = 0;
    unsigned long result = 0;
    unsigned short test_result_branch = 0;
    uint8_t bosch_write_buffer = 0;
    sns_ddf_status_e status;
    sns_dd_inertial_test_config_s test_config;

    *err = 0;

    BMA2X2_MSG_3(HIGH, "<bma2x2_if_ self_test> 0x%x sensor: %d test: %d",
            dd_handle, sensor_type, test);

    if (test == SNS_DDF_TEST_OEM)
    {
        BMA2X2_MSG_3_P(HIGH, "SNS_DDF_TEST_OEM %d 0x%x timestamp = %d",
                55555568, dd_handle, test);

        BMA2X2_MSG_0(HIGH, "SNS_DDF_TEST_OEM");

        test_config.sample_rate =  100;
        test_config.num_samples =  64;
        test_config.max_variance = (int64_t)(FX_CONV((FX_FLTTOFIX_Q16(0.3)),16,32));
        test_config.bias_thresholds[0] = FX_FLTTOFIX_Q16(0.2*G);
        test_config.bias_thresholds[1] = FX_FLTTOFIX_Q16(0.2*G);
        test_config.bias_thresholds[2] = FX_FLTTOFIX_Q16(0.26*G);

        /*! soft reset */
        state->soft_reset = 1;
        state->test_init = 1;
        sns_dd_acc_bma2x2_reset(dd_handle);
        state->soft_reset = 0;
        state->test_init = 0;

        //This delay is needed because it takes
        //some time for the new BW setting to take affect
        //TODO: now 10ms is enough because the BW is set to 250HZ in reset()
        sns_ddf_delay(10000);
        status = sns_dd_inertial_test_run(
                sensor_type,
                dd_handle,
                &sns_accel_bma2x2_driver_fn_list,
                &test_config,
                &sns_dd_acc_bma2x2_test_notify);

        if (status != SNS_DDF_SUCCESS)
            return status;

        BMA2X2_MSG_0(HIGH, "return pending");
        return SNS_DDF_PENDING;
    }
    else if ((test == SNS_DDF_TEST_SELF_SW)
            ||(test == SNS_DDF_TEST_SELF_HW))
    {
        /*! soft reset */
        bosch_write_buffer = BMA2X2_EN_SOFT_RESET_VALUE;
        if ((stat = bma2x2_smbus_write_byte(state->port_handle,
                        BMA2X2_RESET_REG,
                        &bosch_write_buffer)) != SNS_DDF_SUCCESS)
        {
            *err = 1; return stat;
        }

        sns_ddf_delay(BMA2X2_SOFTRESET2READY_DELAY);

        /*! clear register 0x32 */
        if ((stat = bma2x2_smbus_write_byte(state->port_handle,
                        0x32,
                        &clear_value)) != SNS_DDF_SUCCESS)
        {
            *err =2;  return stat;
        }

        if ((state->sub_model == BMA222E_TYPE)
                || (state->sub_model == BMA250E_TYPE)
                || (state->sub_model == BMA255_TYPE)
                || (state->sub_model == BMA280_TYPE)) {
            /*! set to 8 G range */
            if ((stat = bma2x2_set_range(state->port_handle, 2)) != SNS_DDF_SUCCESS)
            {*err = 19;  return stat;}

            if ((stat = bma2x2_set_selftest_amp(state->port_handle, 1)) != SNS_DDF_SUCCESS)
            {*err = 20;  return stat;}
        }


        if ((stat = bma2x2_set_selftest_st(state->port_handle, 1))!= SNS_DDF_SUCCESS) /*! 1 for x-axis*/
        {*err = 3;  return stat;}

        if ((stat = bma2x2_set_selftest_stn(state->port_handle, 0))!= SNS_DDF_SUCCESS) /*! positive
                                                                                         direction*/
        {*err = 4;  return stat;}

        sns_ddf_delay(10000);
        if ((stat = bma2x2_read_accel_x(state->port_handle, state->sub_model, &value1))!= SNS_DDF_SUCCESS)
        {*err = 5;  return stat;}
        if ((stat = bma2x2_set_selftest_stn(state->port_handle, 1))!= SNS_DDF_SUCCESS) /*! negative
                                                                                         direction*/
        {*err = 6;  return stat;}
        sns_ddf_delay(10000);

        if ((stat = bma2x2_read_accel_x(state->port_handle, state->sub_model,&value2))!= SNS_DDF_SUCCESS)
        {*err = 7;  return stat;}
        diff = value1-value2;

        if ((BMA280_TYPE != state->sub_model)
                || (BMA2X2_DEV_SEL_NUM_BMI058 != state->dev_select)) {
            test_result_branch = 1;
        } else {
            test_result_branch = 2;
        }

        if (state->sub_model == BMA280_TYPE) {
            if (abs(diff) < 819)
            {result |= test_result_branch; *err |= 1;}
        }
        if (state->sub_model == BMA255_TYPE) {
            if (abs(diff) < 204)
            {result |= test_result_branch; *err |= 1;}
        }
        if (state->sub_model == BMA250E_TYPE) {
            if (abs(diff) < 51)
            {result |= test_result_branch; *err |= 1;}
        }
        if (state->sub_model == BMA222E_TYPE) {
            if (abs(diff) < 12)
            {result |= test_result_branch; *err |= 1;}
        }



        if ((stat = bma2x2_set_selftest_st(state->port_handle, 2))!= SNS_DDF_SUCCESS) /*! 2 for y-axis*/
        {*err = 8;  return stat;}
        if ((stat = bma2x2_set_selftest_stn(state->port_handle, 0))!= SNS_DDF_SUCCESS) /*! positive
                                                                                         direction*/
        {*err = 9;  return stat;}
        sns_ddf_delay(10000);

        if ((stat = bma2x2_read_accel_y(state->port_handle,state->sub_model, &value1))!= SNS_DDF_SUCCESS)
        {*err = 10;  return stat;}
        if ((stat = bma2x2_set_selftest_stn(state->port_handle, 1))!= SNS_DDF_SUCCESS) /*! negative
                                                                                         direction*/
        {*err = 11;  return stat;}

        sns_ddf_delay(10000);
        if ((stat = bma2x2_read_accel_y(state->port_handle, state->sub_model, &value2))!= SNS_DDF_SUCCESS)
        {*err = 12;  return stat;}
        diff = value1-value2;


        if (state->sub_model == BMA280_TYPE) {
            if (abs(diff) < 819)
            {result |= test_result_branch; *err |= 2;}
        }
        if (state->sub_model == BMA255_TYPE) {
            if (abs(diff) < 204)
            {result |= test_result_branch; *err |= 2;}
        }
        if (state->sub_model == BMA250E_TYPE) {
            if (abs(diff) < 51)
            {result |= test_result_branch; *err |= 2;}
        }
        if (state->sub_model == BMA222E_TYPE) {
            if (abs(diff) < 12)
            {result |= test_result_branch; *err |= 2;}
        }


        if ((stat = bma2x2_set_selftest_st(state->port_handle, 3))!= SNS_DDF_SUCCESS) /*! 3 for z-axis*/
        {*err = 13;  return stat;}
        if ((stat = bma2x2_set_selftest_stn(state->port_handle, 0))!= SNS_DDF_SUCCESS) /*! positive
                                                                                         direction*/
        {*err = 14;  return stat;}

        sns_ddf_delay(10000);
        if ((stat = bma2x2_read_accel_z(state->port_handle,state->sub_model, &value1))!= SNS_DDF_SUCCESS)
        {*err = 15;  return stat;}
        if ((stat = bma2x2_set_selftest_stn(state->port_handle, 1))!= SNS_DDF_SUCCESS) /*! negative
                                                                                         direction*/
        {*err = 16;  return stat;}

        sns_ddf_delay(10000);
        if ((stat = bma2x2_read_accel_z(state->port_handle,state->sub_model, &value2))!= SNS_DDF_SUCCESS)
        {*err = 17;  return stat;}

        diff = value1-value2;

        if (state->sub_model == BMA280_TYPE) {
            if (abs(diff) < 409)
            {result |= 4; *err |= 4;}
        }
        if (state->sub_model == BMA255_TYPE) {
            if (abs(diff) < 102)
            {result |= 4; *err |= 4;}
        }
        if (state->sub_model == BMA250E_TYPE) {
            if (abs(diff) < 25)
            {result |= 4; *err |= 4;}
        }
        if (state->sub_model == BMA222E_TYPE) {
            if (abs(diff) < 6)
            {result |= 4; *err |= 4;}
        }


        if (result == 0) stat = SNS_DDF_SUCCESS;
        else stat = SNS_DDF_EFAIL;

        state->soft_reset = 1;
        state->test_init = 1;
        sns_dd_acc_bma2x2_reset(dd_handle);
        state->test_init = 0;
        state->soft_reset = 0;
        return stat;
    }
    else if (test == SNS_DDF_TEST_SELF) {
        stat = sns_dd_acc_bma2x2_run_test_self(state, sensor_type, err);
        return stat;
    }
    else {
        return SNS_DDF_EINVALID_TEST;
    }
}


sns_ddf_status_e sns_dd_acc_bma2x2_probe(
        sns_ddf_device_access_s*  device_info,
        sns_ddf_memhandler_s*     memhandler,
        uint32_t*                 num_sensors,
        sns_ddf_sensor_e**        sensors)
{
    sns_ddf_status_e status;
    sns_ddf_handle_t port_handle;

    uint8_t si_buf;
    uint8_t bytes_read;
    *num_sensors = 0;
    *sensors = NULL;

    BMA2X2_MSG_3_P(LOW, "%d %d %c",
            55555590, 12345678, 'p');

    status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    /*  BMA2X2_MSG_2(ERROR,
        "bma2x2: Probing I2C addr: 0x%X  bus instance: 0x%X",
        device_info->port_config.bus_config.i2c->slave_addr,
        device_info->port_config.bus_instance);
        */

    if ((status = sns_ddf_read_port(port_handle,
                    BMA2X2_CHIP_ID_REG,
                    &si_buf,
                    1,
                    &bytes_read)) != SNS_DDF_SUCCESS)
    {
        sns_ddf_close_port(port_handle);
        return status;
    }
    sns_ddf_close_port(port_handle);

    BMA2X2_MSG_3_P(LOW, "%d %d %d",
            55555591, __LINE__, *num_sensors);

    if ((si_buf == BMA255_CHIP_ID)
            ||(si_buf == BMA250E_CHIP_ID)
            ||(si_buf == BMA222E_CHIP_ID)
            ||(si_buf == BMA280_CHIP_ID))
    {
        *num_sensors = 3;
        *sensors = sns_ddf_memhandler_malloc(memhandler,
                sizeof(sns_ddf_sensor_e) * (*num_sensors));
        if (*sensors != NULL)
        {
            (*sensors)[0] = SNS_DDF_SENSOR_ACCEL;
            (*sensors)[1] = SNS_DDF_SENSOR_TEMP;
            (*sensors)[2] = SNS_DDF_SENSOR_DOUBLETAP;
            status = SNS_DDF_SUCCESS;
        }
        else
        {
            status = SNS_DDF_ENOMEM;
        }
        return status;
    }
    else
    {
        /* Unknown device ID. Return now with nothing detected */
        return SNS_DDF_SUCCESS;
    }
}


sns_ddf_status_e sns_dd_acc_bma2x2_enable_sched_data(
        sns_ddf_handle_t    dd_handle,
        sns_ddf_sensor_e    sensor_type,
        bool                enable)
{
    sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;
    sns_dd_acc_bma2x2_state_t *state = sns_dd_acc_bma2x2_get_dev(dd_handle);
    uint32_t signal_registered;
    bool dri_imitate_ff_wmi = false;
    uint8_t regv;

    BMA2X2_MSG_3(HIGH, "<bma2x2_if_ enable_sched_data> <sensor: %d f_wml:%d enable:%d>",
            sensor_type, state->f_wml, enable);

    BMA2X2_MSG_4(MEDIUM, "[enable_sched_data] %d %d %d 0x%x",
            state->acc_cur_rate, state->dbt_cur_rate, state->tmp_cur_rate,
            (state->en_db_tap_int << 24) | (state->en_md << 8) |
            (state->en_fifo_int << 8) | (state->en_dri));


    if (!sns_ddf_signal_irq_enabled()) {
        return SNS_DDF_EINVALID_PARAM;
    }

    //the code that follows this part applies to accel only
    if (SNS_DDF_SENSOR_ACCEL != sensor_type) {
        return SNS_DDF_SUCCESS;
    }

    signal_registered = ((state->en_md) || (state->en_db_tap_int)
            || (state->en_dri) || (state->en_fifo_int));
    state->en_fifo_int_orig = state->en_fifo_int;


    if (enable) {
        ret_val = sns_dd_acc_bma2x2_signal_register(dd_handle, signal_registered);
    }

#if BMA2X2_CONFIG_ENABLE_DRI_IMITATE_FF_WMI
    if (1 == state->f_wml) {
        dri_imitate_ff_wmi = true;
    }
#endif

    if (enable) {
        if (state->f_wml > 0) {
            if (!dri_imitate_ff_wmi) {
                ret_val = bma2x2_enable_fifo_wm_int(state->port_handle, 1);

                if (SNS_DDF_SUCCESS != ret_val) {
                    return ret_val;
                }

                state->en_fifo_int = true;
            } else {
                ret_val = bma2x2_enable_data_ready_int(state->port_handle, 1);
                if (SNS_DDF_SUCCESS != ret_val) {
                    return ret_val;
                }

                state->en_dri = true;
            }
        } else {
            ret_val = bma2x2_enable_data_ready_int(state->port_handle, 1);
            if (SNS_DDF_SUCCESS != ret_val) {
                return ret_val;
            }

            state->en_dri = true;
        }
    } else {
        if (state->en_fifo_int) {
            ret_val = bma2x2_enable_fifo_wm_int(state->port_handle, 0);
            if (SNS_DDF_SUCCESS != ret_val) {
                return ret_val;
            }
        }

        if (state->en_dri) {
            ret_val = bma2x2_enable_data_ready_int(state->port_handle, 0);
            if (SNS_DDF_SUCCESS != ret_val) {
                return ret_val;
            }
        }

        state->en_fifo_int = false;
        state->en_dri = false;
    }

    if (enable) {
        ret_val = bma2x2_smbus_read_byte(state->port_handle, BMA2X2_INT_DATA_SEL_REG, &state->regv_int_map_1);
        BMA2X2_DD_CHECK_RETVAL(ret_val, SNS_DDF_SUCCESS);
    } else {
        ret_val = sns_dd_acc_bma2x2_signal_deregister(dd_handle, signal_registered);
    }

    state->ts_irq_last_drdy = 0;

    BST_IGNORE_WARNING_UNUSED_VAR(regv);
    //ret_val |= bma2x2_smbus_read_byte(state->port_handle, BMA2X2_REGA_INT_EN_1, &regv);

    return ret_val;
}

sns_ddf_status_e bma2x2_read_accel_x(
        sns_ddf_handle_t port_handle,
        uint8_t sensor_type,
        short *a_x)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data[2];

    switch (sensor_type) {
        case 0:
            comres = bma2x2_smbus_read_byte_block(port_handle,
                    BMA2X2_ACC_X14_LSB__REG, data, 2);
            *a_x = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_X14_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_X_MSB)<<(BMA2X2_ACC_X14_LSB__LEN));
            *a_x = *a_x << (sizeof(short)*8-(BMA2X2_ACC_X14_LSB__LEN
                        + BMA2X2_ACC_X_MSB__LEN));
            *a_x = *a_x >> (sizeof(short)*8-(BMA2X2_ACC_X14_LSB__LEN
                        + BMA2X2_ACC_X_MSB__LEN));
            break;
        case 1:
            comres = bma2x2_smbus_read_byte_block(port_handle,
                    BMA2X2_ACC_X12_LSB__REG, data, 2);
            *a_x = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_X12_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_X_MSB)<<(BMA2X2_ACC_X12_LSB__LEN));
            *a_x = *a_x << (sizeof(short)*8-(BMA2X2_ACC_X12_LSB__LEN
                        + BMA2X2_ACC_X_MSB__LEN));
            *a_x = *a_x >> (sizeof(short)*8-(BMA2X2_ACC_X12_LSB__LEN
                        + BMA2X2_ACC_X_MSB__LEN));
            break;
        case 2:
            comres = bma2x2_smbus_read_byte_block(port_handle,
                    BMA2X2_ACC_X10_LSB__REG, data, 2);
            *a_x = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_X10_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_X_MSB)<<(BMA2X2_ACC_X10_LSB__LEN));
            *a_x = *a_x << (sizeof(short)*8-(BMA2X2_ACC_X10_LSB__LEN
                        + BMA2X2_ACC_X_MSB__LEN));
            *a_x = *a_x >> (sizeof(short)*8-(BMA2X2_ACC_X10_LSB__LEN
                        + BMA2X2_ACC_X_MSB__LEN));
            break;
        case 3:
            comres = bma2x2_smbus_read_byte_block(port_handle,
                    BMA2X2_ACC_X8_LSB__REG, data, 2);
            *a_x = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_X8_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_X_MSB)<<(BMA2X2_ACC_X8_LSB__LEN));
            *a_x = *a_x << (sizeof(short)*8-(BMA2X2_ACC_X8_LSB__LEN
                        + BMA2X2_ACC_X_MSB__LEN));
            *a_x = *a_x >> (sizeof(short)*8-(BMA2X2_ACC_X8_LSB__LEN
                        + BMA2X2_ACC_X_MSB__LEN));
            break;

        default:
            break;
    }

    return comres;
}

sns_ddf_status_e bma2x2_read_accel_y(
        sns_ddf_handle_t port_handle,
        uint8_t sensor_type,
        short *a_y)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data[2];

    switch (sensor_type) {

        case 0:
            comres = bma2x2_smbus_read_byte_block(port_handle,
                    BMA2X2_ACC_Y14_LSB__REG, data, 2);
            *a_y = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_Y14_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_Y_MSB)<<(BMA2X2_ACC_Y14_LSB__LEN));
            *a_y = *a_y << (sizeof(short)*8-(BMA2X2_ACC_Y14_LSB__LEN
                        + BMA2X2_ACC_Y_MSB__LEN));
            *a_y = *a_y >> (sizeof(short)*8-(BMA2X2_ACC_Y14_LSB__LEN
                        + BMA2X2_ACC_Y_MSB__LEN));
            break;
        case 1:
            comres = bma2x2_smbus_read_byte_block(port_handle,
                    BMA2X2_ACC_Y12_LSB__REG, data, 2);
            *a_y = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_Y12_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_Y_MSB)<<(BMA2X2_ACC_Y12_LSB__LEN));
            *a_y = *a_y << (sizeof(short)*8-(BMA2X2_ACC_Y12_LSB__LEN
                        + BMA2X2_ACC_Y_MSB__LEN));
            *a_y = *a_y >> (sizeof(short)*8-(BMA2X2_ACC_Y12_LSB__LEN
                        + BMA2X2_ACC_Y_MSB__LEN));
            break;
        case 2:
            comres = bma2x2_smbus_read_byte_block(port_handle,
                    BMA2X2_ACC_Y10_LSB__REG, data, 2);
            *a_y = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_Y10_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_Y_MSB)<<(BMA2X2_ACC_Y10_LSB__LEN));
            *a_y = *a_y << (sizeof(short)*8-(BMA2X2_ACC_Y10_LSB__LEN
                        + BMA2X2_ACC_Y_MSB__LEN));
            *a_y = *a_y >> (sizeof(short)*8-(BMA2X2_ACC_Y10_LSB__LEN
                        + BMA2X2_ACC_Y_MSB__LEN));
            break;
        case 3:
            comres = bma2x2_smbus_read_byte_block(port_handle,
                    BMA2X2_ACC_Y8_LSB__REG, data, 2);
            *a_y = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_Y8_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_Y_MSB)<<(BMA2X2_ACC_Y8_LSB__LEN));
            *a_y = *a_y << (sizeof(short)*8-(BMA2X2_ACC_Y8_LSB__LEN
                        + BMA2X2_ACC_Y_MSB__LEN));
            *a_y = *a_y >> (sizeof(short)*8-(BMA2X2_ACC_Y8_LSB__LEN
                        + BMA2X2_ACC_Y_MSB__LEN));
            break;

        default:
            break;
    }

    return comres;
}

sns_ddf_status_e bma2x2_read_accel_z(
        sns_ddf_handle_t port_handle,
        uint8_t sensor_type,
        short *a_z)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data[2];

    switch (sensor_type) {
        case 0:
            comres = bma2x2_smbus_read_byte_block(port_handle,
                    BMA2X2_ACC_Z14_LSB__REG, data, 2);
            *a_z = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_Z14_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_Z_MSB)<<(BMA2X2_ACC_Z14_LSB__LEN));
            *a_z = *a_z << (sizeof(short)*8-(BMA2X2_ACC_Z14_LSB__LEN
                        + BMA2X2_ACC_Z_MSB__LEN));
            *a_z = *a_z >> (sizeof(short)*8-(BMA2X2_ACC_Z14_LSB__LEN
                        + BMA2X2_ACC_Z_MSB__LEN));
            break;
        case 1:
            comres = bma2x2_smbus_read_byte_block(port_handle,
                    BMA2X2_ACC_Z12_LSB__REG, data, 2);
            *a_z = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_Z12_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_Z_MSB)<<(BMA2X2_ACC_Z12_LSB__LEN));
            *a_z = *a_z << (sizeof(short)*8-(BMA2X2_ACC_Z12_LSB__LEN
                        + BMA2X2_ACC_Z_MSB__LEN));
            *a_z = *a_z >> (sizeof(short)*8-(BMA2X2_ACC_Z12_LSB__LEN
                        + BMA2X2_ACC_Z_MSB__LEN));
            break;
        case 2:
            comres = bma2x2_smbus_read_byte_block(port_handle,
                    BMA2X2_ACC_Z10_LSB__REG, data, 2);
            *a_z = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_Z10_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_Z_MSB)<<(BMA2X2_ACC_Z10_LSB__LEN));
            *a_z = *a_z << (sizeof(short)*8-(BMA2X2_ACC_Z10_LSB__LEN
                        + BMA2X2_ACC_Z_MSB__LEN));
            *a_z = *a_z >> (sizeof(short)*8-(BMA2X2_ACC_Z10_LSB__LEN
                        + BMA2X2_ACC_Z_MSB__LEN));
            break;
        case 3:
            comres = bma2x2_smbus_read_byte_block(port_handle,
                    BMA2X2_ACC_Z8_LSB__REG, data, 2);
            *a_z = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_Z8_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_Z_MSB)<<(BMA2X2_ACC_Z8_LSB__LEN));
            *a_z = *a_z << (sizeof(short)*8-(BMA2X2_ACC_Z8_LSB__LEN
                        + BMA2X2_ACC_Z_MSB__LEN));
            *a_z = *a_z >> (sizeof(short)*8-(BMA2X2_ACC_Z8_LSB__LEN
                        + BMA2X2_ACC_Z_MSB__LEN));
            break;

        default:
            break;
    }

    return comres;
}


