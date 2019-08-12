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
  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
  ==============================================================================*/
/*==============================================================================

  S E N S O R S   A C C E L E R O M E T E R    D R I V E R

  DESCRIPTION

  Impelements the driver for the accelerometer driver

  EDIT HISTORY FOR FILE


  when         who      what
  ----------   ---     -----------------------------------------------------------
  07/09/2015   AH      Fix compiler warnings
  01/21/2015   BD      move read_accel_x read_accel_y read_accel_z from uImage to bigImage
  10/30/2014   sc      Fixed KW error in bma2x2_fifo_data_read_out_frames()
  09/11/2014   pn      Fixed KW error in sns_dd_acc_bma2x2_get_data()
  08/24/14     vy      Used new DDF apis to allocate/free mem supported in uImage
  07/27/2014   VY      Replaced "div of two constants" by another constant
  07/20/2014   VY      Disabled logging in uImage
  06/12/2014   VY      Refactored for uimage support


  ==============================================================================*/
/*============================================================================

  INCLUDE FILES

  ============================================================================*/
#include "sns_dd_bma2x2.h"

/*=======================================================================

  FUNCTION PROTOTYPES

  ========================================================================*/

extern sns_ddf_status_e sns_dd_acc_bma2x2_init(
        sns_ddf_handle_t *dd_handle_ptr,
        sns_ddf_handle_t         smgr_handle,
        sns_ddf_nv_params_s *nv_params,
        sns_ddf_device_access_s  device_info[],
        uint32_t                 num_devices,
        sns_ddf_memhandler_s *memhandler,
        sns_ddf_sensor_e *sensors[],
        uint32_t *num_sensors);

extern sns_ddf_status_e sns_dd_acc_bma2x2_set_attr(
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor_type,
        sns_ddf_attribute_e  attrib,
        void *value);

extern sns_ddf_status_e sns_dd_acc_bma2x2_get_attr(
        sns_ddf_handle_t       dd_handle,
        sns_ddf_sensor_e       sensor_type,
        sns_ddf_attribute_e    attrib,
        sns_ddf_memhandler_s *memhandler,
        void **value,
        uint32_t *num_elems);

extern sns_ddf_status_e sns_dd_acc_bma2x2_reset(
        sns_ddf_handle_t dd_handle);

extern sns_ddf_status_e sns_dd_acc_bma2x2_self_test(
        sns_ddf_handle_t dd_handle,
        sns_ddf_sensor_e sensor_type,
        sns_ddf_test_e test,
        uint32_t *err);

extern sns_ddf_status_e sns_dd_acc_bma2x2_probe(
        sns_ddf_device_access_s *device_info,
        sns_ddf_memhandler_s *memhandler,
        uint32_t *num_sensors,
        sns_ddf_sensor_e **sensors);

extern sns_ddf_status_e sns_dd_acc_bma2x2_enable_sched_data(
        sns_ddf_handle_t dd_handle,
        sns_ddf_sensor_e sensor_type,
        bool enable);

static sns_ddf_status_e sns_dd_acc_bma2x2_get_data(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s *memhandler,
        sns_ddf_sensor_data_s *data[]);

static void sns_dd_acc_bma2x2_handle_timer(
        sns_ddf_handle_t dd_handle,
        void *arg);

static void sns_dd_acc_bma2x2_interrupt_handler(
        sns_ddf_handle_t       dd_handle,
        uint32_t               gpio_num,
        sns_ddf_time_t         timestamp);

static sns_ddf_status_e sns_dd_acc_bma2x2_trigger_fifo_data(
        sns_ddf_handle_t    dd_handle,
        sns_ddf_sensor_e    sensor,
        uint16_t            num_samples,
        bool                trigger_now);


/*=======================================================================

  INTERNAL FUNCTION PROTOTYPES

  ========================================================================*/

/*! forward declarations of global/static functions */
static sns_ddf_status_e bma2x2_parse_fifo_frame(unsigned char * data,
        uint8_t sensor_type, struct bma2x2_data *acc);


/* !global variables */
sns_ddf_driver_if_s sns_accel_bma2x2_driver_fn_list =
{
    .init                 = &sns_dd_acc_bma2x2_init,
    .get_data             = &sns_dd_acc_bma2x2_get_data,
    .set_attrib           = &sns_dd_acc_bma2x2_set_attr,
    .get_attrib           = &sns_dd_acc_bma2x2_get_attr,
    .handle_timer         = &sns_dd_acc_bma2x2_handle_timer,
    .handle_irq           = &sns_dd_acc_bma2x2_interrupt_handler,
    .reset                = &sns_dd_acc_bma2x2_reset,
    .run_test             = &sns_dd_acc_bma2x2_self_test,
    .enable_sched_data    = &sns_dd_acc_bma2x2_enable_sched_data,
    .probe                = &sns_dd_acc_bma2x2_probe,
    .trigger_fifo_data    = &sns_dd_acc_bma2x2_trigger_fifo_data
};


const uint32_t BMA2X2_LSB_RANGE_LIST[4] =
{
    LSB_RANGE_14BITS,  /*! BMA280 */
    LSB_RANGE_12BITS,  /*! BMA255 */
    LSB_RANGE_10BITS,  /*! BMA250E */
    LSB_RANGE_8BITS    /*! BMA222E */
};

// BMA2x2_G_OVER_RANGE_LIST[i] = G/BMA2X2_LSB_RANGE_LIST[i]
// Created during init
float BMA2x2_G_OVER_RANGE_LIST[4];


/*! reported Temprature resolution is 1 C (Celsius) */
static const q16_t BMA2X2_TEMP_RESOLUTION =
FX_FLTTOFIX_Q16(BOSCH_ACCEL_SENSOR_TEMP_DEG_PER_LSB);


/*! function implementations */
sns_ddf_status_e bma2x2_smbus_read_byte_block(
        sns_ddf_handle_t port_handle,
        unsigned char reg_addr,
        unsigned char *data,
        unsigned char len)
{
    sns_ddf_status_e stat;
    uint8_t out;

    if ((stat = sns_ddf_read_port(port_handle,
                    reg_addr,
                    data,
                    len,
                    &out)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }
    return stat;
}

sns_ddf_status_e bma2x2_smbus_read_byte(
        sns_ddf_handle_t port_handle,
        unsigned char reg_addr,
        unsigned char *data)
{
    sns_ddf_status_e stat;
    uint8_t out;

    stat = sns_ddf_read_port(port_handle,
            reg_addr,
            data,
            1,
            &out);

    BMA2X2_MSG_3(MED, "sbus_read@0x%x = 0x%x stat: %d",
            reg_addr, data[0], stat);

    if (stat != SNS_DDF_SUCCESS) {
        return stat;
    }

    return stat;
}

sns_ddf_status_e bma2x2_smbus_write_byte(
        sns_ddf_handle_t port_handle,
        unsigned char reg_addr,
        unsigned char *data)
{
    sns_ddf_status_e stat;
    uint8_t out;

    stat = sns_ddf_write_port(port_handle,
            reg_addr,
            data,
            1,
            &out);

    BMA2X2_MSG_3(HIGH, "sbus_write@0x%x = 0x%x stat: %d",
            reg_addr, data[0], stat);

    return stat;
}


sns_ddf_status_e bma2x2_reset_interrupt(
        sns_ddf_handle_t port_handle,
        unsigned char reset_int)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;


    comres = bma2x2_smbus_read_byte(port_handle,
            BMA2X2_RESET_INT__REG, &data);
    data = BMA2X2_SET_BITSLICE(data, BMA2X2_RESET_INT, reset_int);
    comres = bma2x2_smbus_write_byte(port_handle,
            BMA2X2_RESET_INT__REG, &data);

    return comres;
}

sns_ddf_status_e bma2x2_read_accel_xyz(
        sns_ddf_handle_t port_handle,
        uint8_t sensor_type,
        struct bma2x2_data *acc)

{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data[6];

    switch (sensor_type) {
        case BMA255_TYPE:
            comres = bma2x2_smbus_read_byte_block(port_handle,
                    BMA2X2_ACC_X12_LSB__REG, data, 6);
            acc->x = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_X12_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_X_MSB)<<(BMA2X2_ACC_X12_LSB__LEN));
            acc->x = acc->x << (sizeof(short)*8-(BMA2X2_ACC_X12_LSB__LEN +
                        BMA2X2_ACC_X_MSB__LEN));
            acc->x = acc->x >> (sizeof(short)*8-(BMA2X2_ACC_X12_LSB__LEN +
                        BMA2X2_ACC_X_MSB__LEN));

            acc->y = BMA2X2_GET_BITSLICE(data[2], BMA2X2_ACC_Y12_LSB)|
                (BMA2X2_GET_BITSLICE(data[3],
                                     BMA2X2_ACC_Y_MSB)<<(BMA2X2_ACC_Y12_LSB__LEN
                                         ));
            acc->y = acc->y << (sizeof(short)*8-(BMA2X2_ACC_Y12_LSB__LEN +
                        BMA2X2_ACC_Y_MSB__LEN));
            acc->y = acc->y >> (sizeof(short)*8-(BMA2X2_ACC_Y12_LSB__LEN +
                        BMA2X2_ACC_Y_MSB__LEN));

            acc->z = BMA2X2_GET_BITSLICE(data[4], BMA2X2_ACC_Z12_LSB)|
                (BMA2X2_GET_BITSLICE(data[5],
                                     BMA2X2_ACC_Z_MSB)<<(BMA2X2_ACC_Z12_LSB__LEN));
            acc->z = acc->z << (sizeof(short)*8-(BMA2X2_ACC_Z12_LSB__LEN +
                        BMA2X2_ACC_Z_MSB__LEN));
            acc->z = acc->z >> (sizeof(short)*8-(BMA2X2_ACC_Z12_LSB__LEN +
                        BMA2X2_ACC_Z_MSB__LEN));
            break;
        case BMA250E_TYPE:
            comres = bma2x2_smbus_read_byte_block(port_handle,
                    BMA2X2_ACC_X10_LSB__REG, data, 6);
            acc->x = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_X10_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_X_MSB)<<(BMA2X2_ACC_X10_LSB__LEN));
            acc->x = acc->x << (sizeof(short)*8-(BMA2X2_ACC_X10_LSB__LEN +
                        BMA2X2_ACC_X_MSB__LEN));
            acc->x = acc->x >> (sizeof(short)*8-(BMA2X2_ACC_X10_LSB__LEN +
                        BMA2X2_ACC_X_MSB__LEN));

            acc->y = BMA2X2_GET_BITSLICE(data[2], BMA2X2_ACC_Y10_LSB)|
                (BMA2X2_GET_BITSLICE(data[3],
                                     BMA2X2_ACC_Y_MSB)<<(BMA2X2_ACC_Y10_LSB__LEN
                                         ));
            acc->y = acc->y << (sizeof(short)*8-(BMA2X2_ACC_Y10_LSB__LEN +
                        BMA2X2_ACC_Y_MSB__LEN));
            acc->y = acc->y >> (sizeof(short)*8-(BMA2X2_ACC_Y10_LSB__LEN +
                        BMA2X2_ACC_Y_MSB__LEN));

            acc->z = BMA2X2_GET_BITSLICE(data[4], BMA2X2_ACC_Z10_LSB)|
                (BMA2X2_GET_BITSLICE(data[5],
                                     BMA2X2_ACC_Z_MSB)<<(BMA2X2_ACC_Z10_LSB__LEN));
            acc->z = acc->z << (sizeof(short)*8-(BMA2X2_ACC_Z10_LSB__LEN +
                        BMA2X2_ACC_Z_MSB__LEN));
            acc->z = acc->z >> (sizeof(short)*8-(BMA2X2_ACC_Z10_LSB__LEN +
                        BMA2X2_ACC_Z_MSB__LEN));
            break;
        case BMA222E_TYPE:
            comres = bma2x2_smbus_read_byte_block(port_handle,
                    BMA2X2_ACC_X8_LSB__REG, data, 6);
            acc->x = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_X8_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_X_MSB)<<(BMA2X2_ACC_X8_LSB__LEN));
            acc->x = acc->x << (sizeof(short)*8-(BMA2X2_ACC_X8_LSB__LEN +
                        BMA2X2_ACC_X_MSB__LEN));
            acc->x = acc->x >> (sizeof(short)*8-(BMA2X2_ACC_X8_LSB__LEN +
                        BMA2X2_ACC_X_MSB__LEN));

            acc->y = BMA2X2_GET_BITSLICE(data[2], BMA2X2_ACC_Y8_LSB)|
                (BMA2X2_GET_BITSLICE(data[3],
                                     BMA2X2_ACC_Y_MSB)<<(BMA2X2_ACC_Y8_LSB__LEN
                                         ));
            acc->y = acc->y << (sizeof(short)*8-(BMA2X2_ACC_Y8_LSB__LEN +
                        BMA2X2_ACC_Y_MSB__LEN));
            acc->y = acc->y >> (sizeof(short)*8-(BMA2X2_ACC_Y8_LSB__LEN +
                        BMA2X2_ACC_Y_MSB__LEN));

            acc->z = BMA2X2_GET_BITSLICE(data[4], BMA2X2_ACC_Z8_LSB)|
                (BMA2X2_GET_BITSLICE(data[5],
                                     BMA2X2_ACC_Z_MSB)<<(BMA2X2_ACC_Z8_LSB__LEN));
            acc->z = acc->z << (sizeof(short)*8-(BMA2X2_ACC_Z8_LSB__LEN +
                        BMA2X2_ACC_Z_MSB__LEN));
            acc->z = acc->z >> (sizeof(short)*8-(BMA2X2_ACC_Z8_LSB__LEN +
                        BMA2X2_ACC_Z_MSB__LEN));
            break;
        case BMA280_TYPE:
            comres = bma2x2_smbus_read_byte_block(port_handle,
                    BMA2X2_ACC_X14_LSB__REG, data, 6);
            acc->x = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_X14_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_X_MSB)<<(BMA2X2_ACC_X14_LSB__LEN));
            acc->x = acc->x << (sizeof(short)*8-(BMA2X2_ACC_X14_LSB__LEN +
                        BMA2X2_ACC_X_MSB__LEN));
            acc->x = acc->x >> (sizeof(short)*8-(BMA2X2_ACC_X14_LSB__LEN +
                        BMA2X2_ACC_X_MSB__LEN));

            acc->y = BMA2X2_GET_BITSLICE(data[2], BMA2X2_ACC_Y14_LSB)|
                (BMA2X2_GET_BITSLICE(data[3],
                                     BMA2X2_ACC_Y_MSB)<<(BMA2X2_ACC_Y14_LSB__LEN
                                         ));
            acc->y = acc->y << (sizeof(short)*8-(BMA2X2_ACC_Y14_LSB__LEN +
                        BMA2X2_ACC_Y_MSB__LEN));
            acc->y = acc->y >> (sizeof(short)*8-(BMA2X2_ACC_Y14_LSB__LEN +
                        BMA2X2_ACC_Y_MSB__LEN));

            acc->z = BMA2X2_GET_BITSLICE(data[4], BMA2X2_ACC_Z14_LSB)|
                (BMA2X2_GET_BITSLICE(data[5],
                                     BMA2X2_ACC_Z_MSB)<<(BMA2X2_ACC_Z14_LSB__LEN));
            acc->z = acc->z << (sizeof(short)*8-(BMA2X2_ACC_Z14_LSB__LEN +
                        BMA2X2_ACC_Z_MSB__LEN));
            acc->z = acc->z >> (sizeof(short)*8-(BMA2X2_ACC_Z14_LSB__LEN +
                        BMA2X2_ACC_Z_MSB__LEN));
            break;
        default:
            break;
    }

    return comres;
}


sns_ddf_status_e bma2x2_read_temperature(
        sns_ddf_handle_t port_handle,
        signed char *temperature)
{
    unsigned char data;
    sns_ddf_status_e comres;

    comres = bma2x2_smbus_read_byte(port_handle, BMA2X2_TEMP_RD_REG, &data);
    *temperature = (signed char)data;

    return comres;
}


/*!
 *  @brief
 *  @return
 *  Available values
 *  0x00: none
 *  0x10: x,
 *  0x20: y,
 *  0x40: z
 */
int bma2x2_get_tap_first(
        uint8_t int_stat,
        sns_dd_acc_bma2x2_state_t *state)
{
    int first_x;
    int first_y;
    int first_z;

    if ((BMA280_TYPE != state->sub_model)
            || (BMA2X2_DEV_SEL_NUM_BMI058 != state->dev_select)) {
        first_x = BMA2X2_GET_BITSLICE(int_stat, BMA2X2_TAP_FIRST_X);
        first_y = BMA2X2_GET_BITSLICE(int_stat, BMA2X2_TAP_FIRST_Y);
    } else {
        first_y = BMA2X2_GET_BITSLICE(int_stat, BMA2X2_TAP_FIRST_X);
        first_x = BMA2X2_GET_BITSLICE(int_stat, BMA2X2_TAP_FIRST_Y);
    }


    first_z = BMA2X2_GET_BITSLICE(int_stat, BMA2X2_TAP_FIRST_Z);

    //this code have preference as: z->y->x
    if (first_z)
    {
        return BMA2X2_INT_INFO_TAP_FIRST_Z;
    }

    if (first_y)
    {
        return BMA2X2_INT_INFO_TAP_FIRST_Y;
    }

    if (first_x)
    {
        return BMA2X2_INT_INFO_TAP_FIRST_X;
    }

    return BMA2X2_INT_INFO_TAP_FIRST_NONE;
}


/*!
 *  @brief get direction of double tap
 *
 *  @detail
 *
 *  @return
 *  Available tap direction values
 *  SNS_DDF_TAP_DIR_NONE,
 *  SNS_DDF_TAP_DIR_POS_X,
 *  SNS_DDF_TAP_DIR_NEG_X,
 *  SNS_DDF_TAP_DIR_POS_Y,
 *  SNS_DDF_TAP_DIR_NEG_Y,
 *  SNS_DDF_TAP_DIR_POS_Z,
 *  SNS_DDF_TAP_DIR_NEG_Z,
 *
 */
static sns_ddf_tap_dir_e bma2x2_get_tap_dir(sns_dd_acc_bma2x2_state_t *state)
{
    sns_ddf_tap_dir_e ret;
    sns_ddf_status_e stat;
    unsigned char dir_first_tap;
    unsigned char sign;
    uint8_t int_stat = 0;


    stat = bma2x2_smbus_read_byte(state->port_handle,
            BMA2X2_STATUS_TAP_SLOPE_REG, &int_stat);
    if (stat != SNS_DDF_SUCCESS)
    {
        return SNS_DDF_TAP_DIR_NONE;
    }

    sign = BMA2X2_GET_BITSLICE(int_stat, BMA2X2_TAP_SIGN_S);

    dir_first_tap = bma2x2_get_tap_first(int_stat, state);

    switch (dir_first_tap)
    {
        case BMA2X2_INT_INFO_TAP_FIRST_X:
            if (sign)
                ret = SNS_DDF_TAP_DIR_NEG_X;
            else
                ret = SNS_DDF_TAP_DIR_POS_X;

            break;
        case BMA2X2_INT_INFO_TAP_FIRST_Y:
            if (sign)
                ret = SNS_DDF_TAP_DIR_NEG_Y;
            else
                ret = SNS_DDF_TAP_DIR_POS_Y;

            break;
        case BMA2X2_INT_INFO_TAP_FIRST_Z:
            if (sign)
                ret = SNS_DDF_TAP_DIR_NEG_Z;
            else
                ret = SNS_DDF_TAP_DIR_POS_Z;

            break;
        default:
            ret = SNS_DDF_TAP_DIR_NONE;
    }

    return ret;
}


sns_ddf_status_e bma2x2_get_fifo_framecount(
        sns_ddf_handle_t dd_handle,
        uint8_t *framecount,
        uint8_t *overrun)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(dd_handle,
            BMA2X2_FIFO_FRAME_COUNTER_S__REG, &data);

    //BMA2X2_MSG_3_P(MEDIUM, "bma2x2_get_fifo_framecount data = %d i = %d %d", data, 1, 5555584);
    *framecount = BMA2X2_GET_BITSLICE(data, BMA2X2_FIFO_FRAME_COUNTER_S);

    *overrun = BMA2X2_GET_BITSLICE(data, BMA2X2_FIFO_OVERRUN);

    return comres;
}


/*  data_sel
    0 ------ X,Y,Z
    1 ------ X
    2 ------ Y
    3 ------ Z

*/
sns_ddf_status_e bma2x2_get_fifo_data_sel(
        sns_ddf_handle_t dd_handle,
        unsigned char *data_sel)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;
    unsigned char data;

    comres = bma2x2_smbus_read_byte(dd_handle,
            BMA2X2_FIFO_DATA_SELECT__REG, &data);
    *data_sel = BMA2X2_GET_BITSLICE(data, BMA2X2_FIFO_DATA_SELECT);

    return comres;
}


sns_dd_acc_bma2x2_state_t *sns_dd_acc_bma2x2_get_dev(
        sns_ddf_handle_t dd_handle)
{
    bma2x2_sub_dev_t *sub_dev = (bma2x2_sub_dev_t *)dd_handle;
    return (sns_dd_acc_bma2x2_state_t *)sub_dev->private_data;
}


bma2x2_sub_dev_t *sns_dd_acc_bma2x2_get_subdev_from_type(
        sns_dd_acc_bma2x2_state_t *state,
        const sns_ddf_sensor_e       sensor_type)
{
    bma2x2_sub_dev_t *sub_dev = NULL;
    uint32_t i;
    uint32_t j;

    for (i = 0; i < state->sub_dev_cnt; i++)
    {
        sub_dev = state->sub_dev + i;
        for (j = 0; j < BMA2X2_MAX_NUM_SENSORS; j++)
        {
            if (sensor_type == sub_dev->sensors[j].sensor_type)
            {
                return sub_dev;
            }
        }
    }

    return NULL;
}

bma2x2_sub_dev_t *sns_dd_acc_bma2x2_get_subdev_cache_acc(
        sns_dd_acc_bma2x2_state_t *state)
{
    bma2x2_sub_dev_t *sub_dev;

    sub_dev = state->sub_dev_cache_acc;
    if (NULL != sub_dev) {
        return sub_dev;
    } else {
        sub_dev = sns_dd_acc_bma2x2_get_subdev_from_type(state, SNS_DDF_SENSOR_ACCEL);
    }

    return sub_dev;
}


/*===========================================================================

FUNCTION:   sns_dd_acc_bma2x2_handle_timer

===========================================================================*/
/*!
 * @brief
 *
 * @detail
 *
 *
 * @param[] :
 *
 */
/*=========================================================================*/
    static void sns_dd_acc_bma2x2_handle_timer(
            sns_ddf_handle_t dd_handle,
            void* arg)
{
    BMA2X2_MSG_1(HIGH, "<bma2x2_if_ timer> 0x%x", dd_handle);

    sns_dd_acc_bma2x2_state_t *state = sns_dd_acc_bma2x2_get_dev(dd_handle);

    state->bw_timer_pending = false;
}


/*!
 *  @brief Configure (enable/disable) the motion interrupt
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
sns_ddf_status_e sns_dd_acc_bma2x2_config_motion_int(
        sns_dd_acc_bma2x2_state_t  *state,
        bool                        enable)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    uint8_t bosch_write_buffer;
    uint8_t rega;
    uint8_t regv = 0x00;

    /*! Perform read, modify & write to set motion interrupt bit */
    if ((status = bma2x2_smbus_read_byte(state->port_handle,
                    BMA2X2_INT_ENABLE1_REG,
                    &bosch_write_buffer)) == SNS_DDF_SUCCESS)
    {
        if (enable)
        {
            /*! enable XYZ slope interrupt */
            bosch_write_buffer |= BMA2X2_ACCEL_ENABLE_SLOPE_XYZ_VALUE;

        }
        else
        {
            /*! disable XYZ slope interrupt */
            bosch_write_buffer &= ~(BMA2X2_ACCEL_ENABLE_SLOPE_XYZ_VALUE);

        }

        status = bma2x2_smbus_write_byte(state->port_handle,
                BMA2X2_INT_ENABLE1_REG,
                &bosch_write_buffer);
    }

    if (status == SNS_DDF_SUCCESS)
    {
        state->en_md = enable;
    }

#if BMA2X2_CONFIG_ENABLE_INT_MAP_2
    rega = BMA2X2_REGA_INT_MAP_2;
#else
    rega = BMA2X2_REGA_INT_MAP_0;
#endif
    status = bma2x2_smbus_read_byte(state->port_handle,
            rega, &regv);
    BMA2X2_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

    if (enable) {
        regv = BST_SET_VAL_BIT(regv, 2);
    } else {
        regv = BST_CLR_VAL_BIT(regv, 2);
    }

    status = bma2x2_smbus_write_byte(state->port_handle,
            rega, &regv);
    BMA2X2_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);


    return status;
}

/*!
 *  @brief Resets Bosch interrupt latched state
 *
 *  @detail
 *  programs the BOSCH_ACCEL_SENSOR_RESET_INT in
 *  BMA2X2_INT_CTRL_REG reg @param[in] state: Ptr to the
 *  driver structure
 *
 *   @return
 *    The error code definition within the DDF
 *    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 *
 */
static sns_ddf_status_e sns_dd_acc_bma2x2_reset_any_motion_int(
        sns_dd_acc_bma2x2_state_t  *state)
{
    uint8_t bosch_write_buffer, out;
    sns_ddf_status_e stat;

    if ((stat = sns_ddf_read_port(state->port_handle,
                    BMA2X2_INT_CTRL_REG,
                    &bosch_write_buffer,
                    1,
                    &out)) == SNS_DDF_SUCCESS)
    {
        bosch_write_buffer |= BOSCH_ACCEL_SENSOR_RESET_INT;

        if ((stat = bma2x2_smbus_write_byte(state->port_handle,
                        BMA2X2_INT_CTRL_REG,
                        &bosch_write_buffer)) != SNS_DDF_SUCCESS)
        {
            return stat;
        }
    }

    return stat;
}


/*!
 *  @brief Gets the acceleration data from the bosch accelerometer as well
 *  as the temperature
 *
 *  @detail
 *    read the accel data and temp data registers and scales the values
 *    and stores them in the driver state structure in Q16 format
 *
 *  @param[in] state: Ptr to the driver structure
 *
 *  @return
 *    The error code definition within the DDF
 *   SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 *
 */
static sns_ddf_status_e sns_dd_acc_bma2x2_get_data_all(
        sns_dd_acc_bma2x2_state_t *state,
        bool accel_data_needed,
        bool temp_data_needed)
{
    signed char temperature_lsb;
    sns_ddf_status_e stat;
    struct bma2x2_data acc;

#ifdef ADSP_STANDALONE
    // Dummy values for ADSP_STANDALONE
    sns_ddf_axes_map_s   local_axes_map;
    local_axes_map.indx_x = 1;
    local_axes_map.indx_y = 2;
    local_axes_map.indx_z = 3;
    local_axes_map.sign_x = 1;
    local_axes_map.sign_y = 1;
    local_axes_map.sign_z = 1;
#endif

    if (accel_data_needed)
    {
        stat = bma2x2_read_accel_xyz(state->port_handle, state->sub_model, &acc);
        if (stat != SNS_DDF_SUCCESS)
        {
            BMA2X2_MSG_1(ERROR, "BMA2X2 - read accel data error: %d", stat);
            return stat;
        }


        if ((BMA280_TYPE != state->sub_model)
                || (BMA2X2_DEV_SEL_NUM_BMI058 != state->dev_select)) {
            state->data_cache[SDD_ACC_X] =
                FX_FLTTOFIX_Q16((4<<state->range_idx) * BMA2x2_G_OVER_RANGE_LIST[state->sub_model] * acc.x);
            state->data_cache[SDD_ACC_Y] =
                FX_FLTTOFIX_Q16((4<<state->range_idx) * BMA2x2_G_OVER_RANGE_LIST[state->sub_model] * acc.y);
        } else {
            state->data_cache[SDD_ACC_X] =
                FX_FLTTOFIX_Q16((4<<state->range_idx) * BMA2x2_G_OVER_RANGE_LIST[state->sub_model] * acc.y);
            state->data_cache[SDD_ACC_Y] =
                FX_FLTTOFIX_Q16((4<<state->range_idx) * BMA2x2_G_OVER_RANGE_LIST[state->sub_model] * acc.x);
        }

        state->data_cache[SDD_ACC_Z] =
            FX_FLTTOFIX_Q16(acc.z * 2*2*(1<<state->range_idx)*BMA2x2_G_OVER_RANGE_LIST[state->sub_model]);

#ifdef ADSP_STANDALONE
        sns_ddf_map_axes(&local_axes_map, &state->data_cache[SDD_ACC_X]);
#else
        sns_ddf_map_axes(&state->axes_map, &state->data_cache[SDD_ACC_X]);
#endif
    }

    if (temp_data_needed)
    {
        stat = bma2x2_read_temperature(state->port_handle, &temperature_lsb);
        if (stat != SNS_DDF_SUCCESS)
        {
            BMA2X2_MSG_1(ERROR, "BMA2X2 - read temp data error: %d", stat);
            return stat;
        }
        state->data_cache[SDD_TEMP] = temperature_lsb + BOSCH_ACCEL_SENSOR_TEMP_OFFSET_DEG;
    }

    return SNS_DDF_SUCCESS;
}


/*===========================================================================
FUNCTION:   sns_dd_accel_log_fifo
===========================================================================*/
/*!
  @brief log accel sensor data in fifo

  @detail
  - Logs latest set of sensor data sampled from the sensor.

  @param[in] accel_data_ptr: ptr to the driver data
  */
/*=========================================================================*/
void sns_dd_accel_log_fifo(
        sns_ddf_sensor_data_s           *accel_data_ptr,
        enum DD_DATA_LOG_CONTEXT        context)
{
    BST_IGNORE_WARNING_UNUSED_VAR(context);

    BMA2X2_MSG_4(HIGH, "dl_meta<:0x%x,%d,%d,%d:>",
            ((accel_data_ptr->sensor << 24) | (context << 16) | accel_data_ptr->status << 8),
            accel_data_ptr->num_samples,
            accel_data_ptr->timestamp,
            accel_data_ptr->end_timestamp);

#if !BMA2X2_CONFIG_RUN_ON_OSSC

#if BMA2X2_CONFIG_FIFO_LOG
    sns_err_code_e err_code;
    sns_log_sensor_data_pkt_s* log_struct_ptr;
    uint16 idx = 0;
    log_pkt_t                  log_pkt_type;


#if BMA2X2_CONFIG_RUN_ON_OSSC || BMA2X2_CONFIG_ENABLE_COMPILE_CHECK
    log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA_PUBLIC;
#else
    log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA;
#endif


    //Allocate log packet
    err_code = sns_logpkt_malloc(log_pkt_type,
            sizeof(sns_log_sensor_data_pkt_s) + (accel_data_ptr->num_samples -1)*sizeof(int32_t),
            (void**)&log_struct_ptr);
    if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
    {
        log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
        log_struct_ptr->sensor_id = SNS_DDF_SENSOR_ACCEL;
        log_struct_ptr->vendor_id = SNS_DDF_VENDOR_BOSCH;

        //Timestamp the log with sample time
        log_struct_ptr->timestamp = accel_data_ptr->timestamp;
        log_struct_ptr->end_timestamp = accel_data_ptr->end_timestamp;

        log_struct_ptr->num_data_types = SDD_ACC_NUM_AXIS;
        log_struct_ptr->num_samples = accel_data_ptr->num_samples / SDD_ACC_NUM_AXIS;

        //Log the sensor fifo data
        log_struct_ptr->data[0]  = accel_data_ptr->samples[0].sample;
        log_struct_ptr->data[1]  = accel_data_ptr->samples[1].sample;
        log_struct_ptr->data[2]  = accel_data_ptr->samples[2].sample;

        for(idx=0; idx<accel_data_ptr->num_samples; idx++)
        {
            log_struct_ptr->samples[idx]  = accel_data_ptr->samples[idx].sample;
        }

        //Commit log (also frees up the log packet memory)
        (void) sns_logpkt_commit(log_pkt_type,
                log_struct_ptr);
    }
#endif  //#if BMA2X2_CONFIG_FIFO_LOG

#endif
}

/*===========================================================================

FUNCTION:   sns_dd_accel_log_data

===========================================================================*/

/*!
 *  @brief log sensor data
 *
 *  @detail
 *   Logs latest set of sensor data sampled from the sensor
 *
 *  @param[in] state: ptr to the driver structure
 *  @param[in] sample_time: Time that the sensor was sampled
 *
 *
 *  @return
 *
 */
/*=========================================================================*/
#if !BMA2X2_CONFIG_RUN_ON_OSSC
static void sns_dd_accel_log_data(
        sns_dd_acc_bma2x2_state_t *state,
        sns_ddf_time_t sample_time)
{
    sns_err_code_e err_code;
    sns_log_sensor_data_pkt_s* log_struct_ptr;
    log_pkt_t                       log_pkt_type;

#if BMA2X2_CONFIG_RUN_ON_OSSC || BMA2X2_CONFIG_ENABLE_COMPILE_CHECK
    log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA_PUBLIC;
#else
    log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA;
#endif

    BMA2X2_MSG_0(HIGH, "BMA2x2 Log Data");

    //Allocate log packet
    err_code = sns_logpkt_malloc(log_pkt_type,
            sizeof(sns_log_sensor_data_pkt_s),
            (void**)&log_struct_ptr);

    if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
    {
        log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
        log_struct_ptr->sensor_id = SNS_DDF_SENSOR_ACCEL;
        log_struct_ptr->vendor_id = SNS_DDF_VENDOR_BOSCH;

        //Timestamp the log with sample time
        log_struct_ptr->timestamp = sample_time;

        //Log the sensor data
        log_struct_ptr->num_data_types = SDD_ACC_NUM_DATATYPES;
        log_struct_ptr->data[0]  = state->data_cache[SDD_ACC_X];
        log_struct_ptr->data[1] = state->data_cache[SDD_ACC_Y];
        log_struct_ptr->data[2]  = state->data_cache[SDD_ACC_Z];
        log_struct_ptr->data[3] = state->data_cache[SDD_TEMP];

        //Commit log (also frees up the log packet memory)
        err_code = sns_logpkt_commit(log_pkt_type,
                log_struct_ptr);
    }

    if (err_code != SNS_SUCCESS)
    {
        BMA2X2_MSG_1(ERROR, "BMA2x2 Log Data - logpkt_malloc failed with err: %d", err_code);
    }
}
#endif


/*!
 *  @brief Called by the SMGR to get data
 *
 *  @detail
 *  Requests a single sample of sensor data from each of the specified
 *    sensors. Data is returned immediately after being read from the
 *    sensor, in which case data[] is populated in the same order it was
 *    requested,
 *
 *  @param[in]  dd_handle    Handle to a driver instance.
 *  @param[in] sensors       List of sensors for which data isrequested.
 *  @param[in] num_sensors   Length of @a sensors.
 *  @param[in] memhandler    Memory handler used to dynamically allocate
 *                           output parameters, if applicable.
 *  @param[out] data         Sampled sensor data. The number of elements
 *                           must match @a num_sensors.
 *  @return
 *    The error code definition within the DDF
 *    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 *
 */
static sns_ddf_status_e sns_dd_acc_bma2x2_get_data(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s*   memhandler,
        sns_ddf_sensor_data_s*  data[])
{
    uint8_t i;
    sns_ddf_status_e stat;
    sns_ddf_sensor_data_s *data_ptr;
    sns_dd_acc_bma2x2_state_t *state = sns_dd_acc_bma2x2_get_dev(dd_handle);

    bma2x2_sub_dev_t *sub_dev = sns_dd_acc_bma2x2_get_subdev_from_type(state,
            SNS_DDF_SENSOR_ACCEL);

    bool accel_data_needed = false;
    bool temp_data_needed = false;

#if BMA2X2_CONFIG_RUN_ON_OSSC
    /* avoid compilation warning*/
    (void)sub_dev;
#endif

    BMA2X2_MSG_3_P(LOW, "%d 0x%x %d",
            55555510, dd_handle, num_sensors);

    BMA2X2_MSG_3_P(LOW, "%d %d %d",
            55555511, sensors[0], sensors[1]);

    /* Sanity check*/
    if (sub_dev == NULL)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    for (i = 0; i < num_sensors; i++)
    {
        switch (sensors[i])
        {
            case SNS_DDF_SENSOR_ACCEL:
                accel_data_needed = true;
                break;
            case SNS_DDF_SENSOR_TEMP:
                temp_data_needed = true;
                break;
            case SNS_DDF_SENSOR_DOUBLETAP:
                break;
            default:
                return SNS_DDF_EINVALID_PARAM;
        }
    }

    if ((stat = sns_dd_acc_bma2x2_get_data_all(
                    state, accel_data_needed, temp_data_needed)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }
    if ((data_ptr = sns_ddf_memhandler_malloc_ex(memhandler,
                    (num_sensors)
                    *(sizeof(sns_ddf_sensor_data_s)), sub_dev->smgr_handle))
            == NULL)
    {
        return SNS_DDF_ENOMEM;
    }

    *data = data_ptr;

    for (i = 0; i < num_sensors; i++)
    {
        data_ptr[i].sensor = sensors[i];
        data_ptr[i].status = SNS_DDF_SUCCESS;
        data_ptr[i].timestamp = sns_ddf_get_timestamp();

        if (sensors[i] == SNS_DDF_SENSOR_ACCEL)
        {
            if ((data_ptr[i].samples =
                        sns_ddf_memhandler_malloc_ex(memhandler,
                            SDD_ACC_NUM_AXIS
                            * sizeof(sns_ddf_sensor_sample_s),sub_dev->smgr_handle))
                    == NULL)
            {
                return SNS_DDF_ENOMEM;
            }

            data_ptr[i].samples[0].sample  = state->data_cache[SDD_ACC_X]+state->bias[0];
            data_ptr[i].samples[1].sample  = state->data_cache[SDD_ACC_Y]+state->bias[1];
            data_ptr[i].samples[2].sample  = state->data_cache[SDD_ACC_Z]+state->bias[2];   //TODO

            // If the timer is still pending, invalidate the data
            if (state->bw_timer_pending)
            {
                BMA2X2_MSG_1(HIGH, "BMA2X2 - Invalidating sample at time: %u", data_ptr[i].timestamp);
                // Note: the status values for each samples[i] is UNUSED
                data_ptr[i].samples[0].status = SNS_DDF_EINVALID_DATA;
                data_ptr[i].samples[1].status = SNS_DDF_EINVALID_DATA;
                data_ptr[i].samples[2].status = SNS_DDF_EINVALID_DATA;
                data_ptr[i].status = SNS_DDF_EINVALID_DATA;
            }
            else
            {
                data_ptr[i].samples[0].status = SNS_DDF_SUCCESS;
                data_ptr[i].samples[1].status = SNS_DDF_SUCCESS;
                data_ptr[i].samples[2].status = SNS_DDF_SUCCESS;
                data_ptr[i].status = SNS_DDF_SUCCESS;
            }
            data_ptr[i].num_samples = SDD_ACC_NUM_AXIS;
        }
        else if (sensors[i] == SNS_DDF_SENSOR_TEMP)
        {
            if ((data_ptr[i].samples = sns_ddf_memhandler_malloc_ex(memhandler,
                            sizeof(sns_ddf_sensor_sample_s),sub_dev->smgr_handle))
                    == NULL)
            {
                return SNS_DDF_ENOMEM;
            }

            data_ptr[i].samples[0].sample =
                state->data_cache[SDD_TEMP]
                * BMA2X2_TEMP_RESOLUTION;
            data_ptr[i].samples[0].status = SNS_DDF_SUCCESS;

            data_ptr[i].num_samples = 1;
        }
        else if (sensors[i] == SNS_DDF_SENSOR_DOUBLETAP)
        {
            if ((data_ptr[i].samples = sns_ddf_memhandler_malloc_ex(memhandler,
                            sizeof(sns_ddf_sensor_sample_s),sub_dev->smgr_handle))
                    == NULL)
            {
                return SNS_DDF_ENOMEM;
            }

            data_ptr[i].samples[0].sample =
                state->data_cache[SDD_DOUBLE_TAP];
            data_ptr[i].samples[0].status = SNS_DDF_SUCCESS;

            data_ptr[i].num_samples = 1;
        }
    }

#if !BMA2X2_CONFIG_RUN_ON_OSSC
    sns_dd_accel_log_data(state, data_ptr[0].timestamp);
#endif

    return SNS_DDF_SUCCESS;
}

#if BMA2X2_CONFIG_ENABLE_DELAYED_WML_CFG
static
sns_ddf_status_e sns_dd_acc_bma2x2_clr_pending_wml_req(sns_dd_acc_bma2x2_state_t *state)
{
    sns_ddf_status_e status;
    bma2x2_sub_dev_t *sub_dev = NULL;
    uint8_t attr_fifo;


    sub_dev = sns_dd_acc_bma2x2_get_subdev_cache_acc(state);
    if (!sub_dev)
    {
        return SNS_DDF_EINVALID_DATA;
    }
    status = sns_dd_acc_bma2x2_enable_sched_data(sub_dev,
            SNS_DDF_SENSOR_ACCEL, 0);

    state->f_wml_cfg_pending = 0;

    attr_fifo = state->f_wml_pending;
    status = sns_dd_acc_bma2x2_set_attr(sub_dev,
            SNS_DDF_SENSOR_ACCEL, SNS_DDF_ATTRIB_FIFO, &attr_fifo);

    status = sns_dd_acc_bma2x2_enable_sched_data(sub_dev,
            SNS_DDF_SENSOR_ACCEL, 1);

    state->ts_irq_last_fwml = state->ts_irq;
    state->ts_1st_sample_est = state->ts_irq + state->ts_itvl_ideal;
    BMA2X2_MSG_1(HIGH, "clr_pending_wml: %u", state->ts_irq);

    return status;
}
#endif

/*!
 *  @brief BMA2x2 interrupt handler
 *
 *  @detail
 *
 *  @return
 *    The error code definition within the DDF
 *    SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS
 *
 */
static void sns_dd_acc_bma2x2_interrupt_handler(
        sns_ddf_handle_t       dd_handle,
        uint32_t               gpio_num,
        sns_ddf_time_t         timestamp)
{
    sns_ddf_status_e status = 0;
    uint8_t int_status_0 = 0;
    uint8_t int_status_1 = 0;
    uint8_t f_count = 0;
    uint8_t f_overrun = 0;
    uint8_t in_irq = 0;
    uint8_t out = 0;

    uint8_t si_buf_wr;
    bool    wmi_missing_possible = false;

    sns_dd_acc_bma2x2_state_t *state = NULL;
    bma2x2_sub_dev_t *sub_dev = NULL;

    bool false_irq = false;
    int32_t lapse;

    BMA2X2_MSG_1(HIGH, "<bma2x2_iff_isr>ts=%u",
            timestamp);

    state = sns_dd_acc_bma2x2_get_dev(dd_handle);

    if (NULL == state) {
        return;
    }

    BMA2X2_MSG_1(MED, "[bma2x2_if_isr] en_int: 0x%x",
            (state->en_db_tap_int << 24) | (state->en_md << 16) |
            (state->en_fifo_int << 8) | (state->en_dri));

    state->ts_irq = timestamp;

    //we want to  check FIFO first for the sake for lower latency
    if ( (state->en_fifo_int)
            && (state->f_wml > 0) ) {
        f_count = 0;
        f_overrun = 0;
        sub_dev = sns_dd_acc_bma2x2_get_subdev_cache_acc(state);

        if (NULL== sub_dev) {
            BMA2X2_MSG_2(ERROR,
                    "[bma2x2_if_isr] sub_dev is NULL 0x%x timestamp = %d",
                    dd_handle, timestamp);
            return;
        }

        status = bma2x2_smbus_read_byte(
                state->port_handle, BMA2X2_REGA_INT_STATUS_1,
                &int_status_1);

        if ((status = bma2x2_get_fifo_framecount(state->port_handle,
                        &f_count, &f_overrun))
                != SNS_DDF_SUCCESS)
            return;

        if (f_overrun) {
            if (!state->f_overrun_detected_in_irq) {
                state->f_overrun_detected_in_irq = 1;

                status = sns_ddf_smgr_notify_event(sub_dev->smgr_handle,
                        SNS_DDF_SENSOR_ACCEL,
                        SNS_DDF_EVENT_FIFO_OVERFLOW);
            }
        }

        if (f_count < state->f_wml) {
            if (BST_GET_VAL_BIT(int_status_1, 6)) {
                BMA2X2_MSG_2(HIGH,
                        "framecount cor from: %d to %d",
                        f_count,
                        state->f_wml);

                f_count = state->f_wml;
            }

            if ((state->f_wml - f_count) <= 2) {
                BMA2X2_MSG_3_F(ERROR, "[bma2x2_if_isr] wmi_missing_possible 1 "
                        "timestamp: %d wml: %d f_count: %d",
                        timestamp, state->f_wml, f_count);
                wmi_missing_possible = true;

                si_buf_wr = 0x00;
                bma2x2_smbus_write_byte(state->port_handle, BMA2X2_INT_DATA_SEL_REG, &si_buf_wr);
                sns_ddf_delay(BMA2X2_CONFIG_INT_DET_RISING_EDGE_THRES_US);
            }
        }

        if (f_count >= state->f_wml) {
            status = sns_ddf_smgr_notify_event(sub_dev->smgr_handle,
                    SNS_DDF_SENSOR_ACCEL,
                    SNS_DDF_EVENT_FIFO_WM_INT);
        }

        in_irq = 1;
        while (f_count >= state->f_wml) {
            //f_count = f_count / state->f_wml * state->f_wml;
            if (f_count < 2 * state->f_wml) {
                if (((2 * state->f_wml) - f_count) <= 1) {
                    BMA2X2_MSG_3_F(ERROR, "[bma2x2_if_isr] wmi_missing_possible 2"
                            "timestamp: %d wml: %d f_count: %d",
                            timestamp, state->f_wml, f_count);
                    wmi_missing_possible = true;
                }

                f_count = state->f_wml;
            } else {
                wmi_missing_possible = true;
            }

            if (wmi_missing_possible) {
                si_buf_wr = 0;
                sns_ddf_write_port(state->port_handle, BMA2X2_INT_DATA_SEL_REG, &si_buf_wr, 1, &out);
            }

            if (f_count >= BMA2X2_MAX_FIFO_LEVEL) {
                BMA2X2_MSG_3_F(ERROR, "[bma2x2_if_isr] overflow condition"
                        "55555548 timestamp: %d wml: %d f_count: %d",
                        timestamp, state->f_wml, f_count);

                f_count = BMA2X2_MAX_FIFO_LEVEL - 1;
            }
            bma2x2_fifo_data_read_out_frames(state, f_count, f_overrun, in_irq);
            in_irq = 0;

            if (state->f_frames_cache.num_samples > 0) {
                status = sns_ddf_smgr_notify_data(sub_dev->smgr_handle,
                        &state->f_frames_cache, 1);
                sns_dd_accel_log_fifo(&state->f_frames_cache, DD_DATA_LOG_CONTEXT_FIFO_WMI);
            }

            f_count = 0;
            f_overrun = 0;
            int_status_1 = 0;
            status = bma2x2_smbus_read_byte(
                    state->port_handle, BMA2X2_REGA_INT_STATUS_1,
                    &int_status_1);
            if (SNS_DDF_SUCCESS != status) {
                return;
            }

            if (BST_GET_VAL_BIT(int_status_1, 6)) {
                status = bma2x2_get_fifo_framecount(state->port_handle,
                        &f_count,
                        &f_overrun);

                if (SNS_DDF_SUCCESS != status) {
                    return;
                }

                BMA2X2_MSG_2(HIGH,
                        "[get_fifo_fc] <%d %d>",
                        f_count, f_overrun);

                if (f_count < state->f_wml) {
                    f_count = state->f_wml;
                }
            }
        }
    }

    if (state->en_dri) {
        false_irq = false;
        lapse = (int32_t)timestamp - (int32_t)state->ts_irq_last_drdy;

        if (lapse < state->ts_itvl_ideal) {
			if ((state->ts_itvl_ideal - lapse) > state->itvl_drdy_tolerance) {
				false_irq = true;
                BMA2X2_MSG_3(MED, "[bma2x2_if_isr] anti_false_irq false_irq detected <%d %d %d>",
                        lapse, state->ts_itvl_ideal - lapse, state->itvl_drdy_tolerance);
			}
        }
    }

    if (state->en_dri && !false_irq) {
        state->ts_irq_last_drdy = timestamp;

        status = sns_dd_acc_bma2x2_get_data_all(
                state, true, false);
        if (SNS_DDF_SUCCESS != status) {
            return;
        }

        sub_dev = sns_dd_acc_bma2x2_get_subdev_cache_acc(state);
        if (NULL != sub_dev) {
            sns_ddf_sensor_data_s *sensor_data = &state->sensor_data;
            bool data_valid = true;
            q16_t *samples;

            if (state->sample_cnt_to_skip_odrc > 0) {
                data_valid = false;
                state->sample_cnt_to_skip_odrc--;

#if BMA2X2_CONFIG_ENABLE_DELAYED_WML_CFG
                if (0 == state->sample_cnt_to_skip_odrc) {
                    if (state->f_wml_cfg_pending) {
                        sns_dd_acc_bma2x2_clr_pending_wml_req(state);
                    }
                }
#endif
            }

            samples = state->data_cache + SDD_ACC_X;

            sensor_data->status = SNS_DDF_SUCCESS;
            sensor_data->sensor = SNS_DDF_SENSOR_ACCEL;
            sensor_data->num_samples = SDD_ACC_NUM_AXIS;
            sensor_data->timestamp = state->ts_irq;
            sensor_data->end_timestamp = state->ts_irq;

            state->avail_accurate_ts_last_sample = 1;
            state->ts_last_sample = sensor_data->end_timestamp;

            sensor_data->samples[0].sample = samples[0];
            sensor_data->samples[0].status = data_valid ? SNS_DDF_SUCCESS : SNS_DDF_EINVALID_DATA;
            sensor_data->samples[1].sample = samples[1];
            sensor_data->samples[1].status = data_valid ? SNS_DDF_SUCCESS : SNS_DDF_EINVALID_DATA;
            sensor_data->samples[2].sample = samples[2];
            sensor_data->samples[2].status = data_valid ? SNS_DDF_SUCCESS : SNS_DDF_EINVALID_DATA;

            sensor_data->status = data_valid ? SNS_DDF_SUCCESS : SNS_DDF_EINVALID_DATA;

            status = sns_ddf_smgr_notify_data(sub_dev->smgr_handle,
                    sensor_data, 1);

            if (1 == state->f_wml) {
                bma2x2_smbus_write_byte(state->port_handle, BMA2X2_REGA_FIFO_CONFIG_1, &state->regv_fifo_config_1);
            }

            sns_dd_accel_log_fifo(sensor_data,
                    (0 < state->f_wml) ? DD_DATA_LOG_CONTEXT_FIFO_WMI_IMITATED : DD_DATA_LOG_CONTEXT_DRI);
        }
    }

    if (state->en_db_tap_int || state->en_md) {
        //int status 2 is not read, because it is used for FIFO only
        //watermark level needs to be read out anyway, so it makes reading status2 unnecessary
        status = bma2x2_smbus_read_byte(
                state->port_handle, BMA2X2_REGA_INT_STATUS_0,
                &int_status_0);

        if (status != SNS_DDF_SUCCESS) {
            return;
        }
    }

    if (state->en_db_tap_int)
    {
        if (int_status_0 & DOUBLE_TAP_INTERRUPT)
        {
            state->data_cache[SDD_DOUBLE_TAP] =
                bma2x2_get_tap_dir(state);

            status = bma2x2_reset_interrupt(state->port_handle,1);
            if (status != SNS_DDF_SUCCESS)
            {
                BMA2X2_MSG_3_P(ERROR, "%d 0x%x 0x%x",
                        55555545, state, 12345678);
                return;
            }
            state->data_status[SDD_DOUBLE_TAP] = SNS_DDF_SUCCESS;
            /*! Notify the SMGR of a doubletap event*/
            state->sensor_data.status = SNS_DDF_SUCCESS;
            state->sensor_data.sensor = SNS_DDF_SENSOR_DOUBLETAP;
            state->sensor_data.num_samples = 1;
            state->sensor_data.timestamp = timestamp;
            state->sensor_data.samples[0].sample = state->data_cache[SDD_DOUBLE_TAP];
            state->sensor_data.samples[0].status = SNS_DDF_SUCCESS;
            state->sensor_data.samples[1].sample = 0;
            state->sensor_data.samples[1].status = SNS_DDF_SUCCESS;
            state->sensor_data.samples[2].sample = 0;
            state->sensor_data.samples[2].status = SNS_DDF_SUCCESS;

            sub_dev = sns_dd_acc_bma2x2_get_subdev_from_type(state,
                    SNS_DDF_SENSOR_DOUBLETAP);

            BMA2X2_MSG_3_P(ERROR, "%d %d 0x%x",
                    55555542, state->data_cache[SDD_DOUBLE_TAP], sub_dev);

            if ((sub_dev) && (sub_dev->smgr_handle))
            {
                status = sns_ddf_smgr_notify_data(sub_dev->smgr_handle,
                        &state->sensor_data, 1);

                //  return;
            }

        }
    }

    if (state->en_md)
    {
        if (int_status_0 & MOTION_DETECT_INTERRUPT)
        {
            BMA2X2_MSG_3_F(HIGH, "MD interrupt %d 0x%x timestamp = %d",
                    55555546, dd_handle, timestamp);
            /*! Reset the interrupt flag */
            sns_dd_acc_bma2x2_reset_any_motion_int(state);

            /*! disable int */
            sns_dd_acc_bma2x2_config_motion_int(state, 0);

#if 0
            status = sns_ddf_signal_deregister(gpio_num);
            if (status != SNS_DDF_SUCCESS)
                return;
#endif
            sub_dev = sns_dd_acc_bma2x2_get_subdev_from_type(state,
                    SNS_DDF_SENSOR_ACCEL);
            /*! Notify manager of MOTION DETECT event */
            if (sub_dev)
            {
                BMA2X2_MSG_3_F(HIGH, "MD interrupt notify event %d 0x%x timestamp = %d",
                        55555546, dd_handle, timestamp);
                sns_ddf_smgr_notify_event(sub_dev->smgr_handle,
                        SNS_DDF_SENSOR_ACCEL,
                        SNS_DDF_EVENT_MOTION);
            }
            return;
        }
    }

    if (wmi_missing_possible) {
        sns_ddf_write_port(state->port_handle, BMA2X2_REGA_INT_MAP_1, &state->regv_int_map_1, 1, &out);
    }

    //TODO: do not need to malloc every time, just use int_status_0 which is allocated in init()
    //to report samples for self-sched

#if 0
    status = sns_ddf_malloc((void **) &report, sizeof(sns_ddf_sensor_data_s));

    if (status)
    {
        goto report_err;
    }

    status = sns_ddf_malloc((void **) &samples,
            3 * sizeof(sns_ddf_sensor_sample_s));

    if (status)
    {
        goto report_err;
    }

    if ((status = sns_dd_acc_bma2x2_get_data_all(state))
            != SNS_DDF_SUCCESS)
    {
        return;
    }

    report->timestamp   = timestamp;
    report->samples    = samples;
    report->status      = SNS_DDF_SUCCESS;
    report->sensor = SNS_DDF_SENSOR_ACCEL;

    samples[0].sample  = state->data_cache[SDD_ACC_Y]+state->bias[0];
    samples[1].sample  = state->data_cache[SDD_ACC_X]+state->bias[1];
    samples[2].sample  = -state->data_cache[SDD_ACC_Z]+state->bias[2];


    samples[0].status = SNS_DDF_SUCCESS;
    samples[1].status = SNS_DDF_SUCCESS;
    samples[2].status = SNS_DDF_SUCCESS;
    report->num_samples = SDD_ACC_NUM_AXIS;

report_err:
    report->status = status;


    if (sub_dev->smgr_handle!= NULL)
    {
        sns_ddf_smgr_notify_data(sub_dev->smgr_handle, report, 1);
    }

    if (report->samples != NULL)
    {
        sns_ddf_mfree(report->samples);
    }

    if (report != NULL)
    {
        sns_ddf_mfree(report);
    }
#endif

    return;
}


static sns_ddf_status_e bma2x2_parse_fifo_frame(
        unsigned char * data,
        uint8_t sensor_type,
        struct bma2x2_data *acc)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;

    switch (sensor_type) {
        case BMA255_TYPE:

            acc->x = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_X12_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_X_MSB)<<(BMA2X2_ACC_X12_LSB__LEN));
            acc->x = acc->x << (sizeof(short)*8-(BMA2X2_ACC_X12_LSB__LEN +
                        BMA2X2_ACC_X_MSB__LEN));
            acc->x = acc->x >> (sizeof(short)*8-(BMA2X2_ACC_X12_LSB__LEN +
                        BMA2X2_ACC_X_MSB__LEN));

            acc->y = BMA2X2_GET_BITSLICE(data[2], BMA2X2_ACC_Y12_LSB)|
                (BMA2X2_GET_BITSLICE(data[3],
                                     BMA2X2_ACC_Y_MSB)<<(BMA2X2_ACC_Y12_LSB__LEN
                                         ));
            acc->y = acc->y << (sizeof(short)*8-(BMA2X2_ACC_Y12_LSB__LEN +
                        BMA2X2_ACC_Y_MSB__LEN));
            acc->y = acc->y >> (sizeof(short)*8-(BMA2X2_ACC_Y12_LSB__LEN +
                        BMA2X2_ACC_Y_MSB__LEN));

            acc->z = BMA2X2_GET_BITSLICE(data[4], BMA2X2_ACC_Z12_LSB)|
                (BMA2X2_GET_BITSLICE(data[5],
                                     BMA2X2_ACC_Z_MSB)<<(BMA2X2_ACC_Z12_LSB__LEN));
            acc->z = acc->z << (sizeof(short)*8-(BMA2X2_ACC_Z12_LSB__LEN +
                        BMA2X2_ACC_Z_MSB__LEN));
            acc->z = acc->z >> (sizeof(short)*8-(BMA2X2_ACC_Z12_LSB__LEN +
                        BMA2X2_ACC_Z_MSB__LEN));
            break;
        case BMA250E_TYPE:

            acc->x = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_X10_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_X_MSB)<<(BMA2X2_ACC_X10_LSB__LEN));
            acc->x = acc->x << (sizeof(short)*8-(BMA2X2_ACC_X10_LSB__LEN +
                        BMA2X2_ACC_X_MSB__LEN));
            acc->x = acc->x >> (sizeof(short)*8-(BMA2X2_ACC_X10_LSB__LEN +
                        BMA2X2_ACC_X_MSB__LEN));

            acc->y = BMA2X2_GET_BITSLICE(data[2], BMA2X2_ACC_Y10_LSB)|
                (BMA2X2_GET_BITSLICE(data[3],
                                     BMA2X2_ACC_Y_MSB)<<(BMA2X2_ACC_Y10_LSB__LEN
                                         ));
            acc->y = acc->y << (sizeof(short)*8-(BMA2X2_ACC_Y10_LSB__LEN +
                        BMA2X2_ACC_Y_MSB__LEN));
            acc->y = acc->y >> (sizeof(short)*8-(BMA2X2_ACC_Y10_LSB__LEN +
                        BMA2X2_ACC_Y_MSB__LEN));

            acc->z = BMA2X2_GET_BITSLICE(data[4], BMA2X2_ACC_Z10_LSB)|
                (BMA2X2_GET_BITSLICE(data[5],
                                     BMA2X2_ACC_Z_MSB)<<(BMA2X2_ACC_Z10_LSB__LEN));
            acc->z = acc->z << (sizeof(short)*8-(BMA2X2_ACC_Z10_LSB__LEN +
                        BMA2X2_ACC_Z_MSB__LEN));
            acc->z = acc->z >> (sizeof(short)*8-(BMA2X2_ACC_Z10_LSB__LEN +
                        BMA2X2_ACC_Z_MSB__LEN));
            break;
        case BMA222E_TYPE:

            acc->x = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_X8_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_X_MSB)<<(BMA2X2_ACC_X8_LSB__LEN));
            acc->x = acc->x << (sizeof(short)*8-(BMA2X2_ACC_X8_LSB__LEN +
                        BMA2X2_ACC_X_MSB__LEN));
            acc->x = acc->x >> (sizeof(short)*8-(BMA2X2_ACC_X8_LSB__LEN +
                        BMA2X2_ACC_X_MSB__LEN));

            acc->y = BMA2X2_GET_BITSLICE(data[2], BMA2X2_ACC_Y8_LSB)|
                (BMA2X2_GET_BITSLICE(data[3],
                                     BMA2X2_ACC_Y_MSB)<<(BMA2X2_ACC_Y8_LSB__LEN
                                         ));
            acc->y = acc->y << (sizeof(short)*8-(BMA2X2_ACC_Y8_LSB__LEN +
                        BMA2X2_ACC_Y_MSB__LEN));
            acc->y = acc->y >> (sizeof(short)*8-(BMA2X2_ACC_Y8_LSB__LEN +
                        BMA2X2_ACC_Y_MSB__LEN));

            acc->z = BMA2X2_GET_BITSLICE(data[4], BMA2X2_ACC_Z8_LSB)|
                (BMA2X2_GET_BITSLICE(data[5],
                                     BMA2X2_ACC_Z_MSB)<<(BMA2X2_ACC_Z8_LSB__LEN));
            acc->z = acc->z << (sizeof(short)*8-(BMA2X2_ACC_Z8_LSB__LEN +
                        BMA2X2_ACC_Z_MSB__LEN));
            acc->z = acc->z >> (sizeof(short)*8-(BMA2X2_ACC_Z8_LSB__LEN +
                        BMA2X2_ACC_Z_MSB__LEN));
            break;
        case BMA280_TYPE:

            acc->x = BMA2X2_GET_BITSLICE(data[0], BMA2X2_ACC_X14_LSB)|
                (BMA2X2_GET_BITSLICE(data[1],
                                     BMA2X2_ACC_X_MSB)<<(BMA2X2_ACC_X14_LSB__LEN));
            acc->x = acc->x << (sizeof(short)*8-(BMA2X2_ACC_X14_LSB__LEN +
                        BMA2X2_ACC_X_MSB__LEN));
            acc->x = acc->x >> (sizeof(short)*8-(BMA2X2_ACC_X14_LSB__LEN +
                        BMA2X2_ACC_X_MSB__LEN));

            acc->y = BMA2X2_GET_BITSLICE(data[2], BMA2X2_ACC_Y14_LSB)|
                (BMA2X2_GET_BITSLICE(data[3],
                                     BMA2X2_ACC_Y_MSB)<<(BMA2X2_ACC_Y14_LSB__LEN
                                         ));
            acc->y = acc->y << (sizeof(short)*8-(BMA2X2_ACC_Y14_LSB__LEN +
                        BMA2X2_ACC_Y_MSB__LEN));
            acc->y = acc->y >> (sizeof(short)*8-(BMA2X2_ACC_Y14_LSB__LEN +
                        BMA2X2_ACC_Y_MSB__LEN));

            acc->z = BMA2X2_GET_BITSLICE(data[4], BMA2X2_ACC_Z14_LSB)|
                (BMA2X2_GET_BITSLICE(data[5],
                                     BMA2X2_ACC_Z_MSB)<<(BMA2X2_ACC_Z14_LSB__LEN));
            acc->z = acc->z << (sizeof(short)*8-(BMA2X2_ACC_Z14_LSB__LEN +
                        BMA2X2_ACC_Z_MSB__LEN));
            acc->z = acc->z >> (sizeof(short)*8-(BMA2X2_ACC_Z14_LSB__LEN +
                        BMA2X2_ACC_Z_MSB__LEN));
            break;
        default:
            break;
    }

    return comres;
}

static void sns_dd_acc_bma2x2_update_sample_ts(
        sns_dd_acc_bma2x2_state_t *state,
        bool            in_irq,
        uint8_t         f_count,
        bool            f_overrun,
        sns_ddf_time_t  ts_curr)
{
    sns_ddf_time_t est_start;
    sns_ddf_time_t est_end;

    sns_ddf_time_t est_itvl = 0;

    sns_ddf_time_t devi;
    sns_ddf_time_t itvl_irq;

    if (f_overrun) {
        BMA2X2_MSG_0(ERROR, "NOTICE!!! OVERFLOW");
    }

    if (in_irq && !f_overrun) {
        if (state->avail_accurate_ts_last_sample) {
            itvl_irq = sns_dd_bms_get_time_elapse_sys(state->ts_irq_last_fwml, state->ts_irq);
            itvl_irq /= (((int32_t)state->f_acnt_frame_cnt - (int32_t)state->f_acnt_frame_cnt_irq) + state->f_wml);

            devi = sns_dd_bms_get_time_deviation(itvl_irq, state->ts_itvl_est);

            if (devi < (state->ts_itvl_est >> 1)) {
                est_end = state->ts_irq + (f_count - state->f_wml) * state->ts_itvl_est;
                //est_start = est_end - (f_count - 1) * state->ts_itvl_est;
                est_start = state->ts_last_sample + state->ts_itvl_est;
            } else {
                //est_end = state->ts_last_sample + f_count * state->ts_itvl_est;
                //est_start = state->ts_last_sample + state->ts_itvl_est;

                est_end = state->ts_irq + (f_count - state->f_wml) * state->ts_itvl_est;
                //est_start = est_end - (f_count - 1) * state->ts_itvl_est;
                est_start = state->ts_last_sample + state->ts_itvl_est;

                BMA2X2_MSG_4(ERROR, "NOTICE!!! ts_irq not regular <%u %u %d %d>",
                        state->ts_irq_last_fwml, state->ts_irq,
                        state->f_acnt_frame_cnt_irq,
                        (state->f_acnt_frame_cnt + state->f_wml - state->f_acnt_frame_cnt_irq));
            }

            est_itvl = sns_dd_bms_get_time_elapse_sys(state->ts_irq_last_fwml, state->ts_irq) / state->f_wml;

            BMA2X2_MSG_3(LOW, "est_itvl <%d %d %d>",
                    est_itvl, state->ts_itvl_ideal, state->ts_itvl_est);

            if (sns_dd_bma2x2_itvl_within_accuracy(est_itvl, state->ts_itvl_ideal)) {
                state->ts_itvl_est = ((state->ts_itvl_est * BMA2X2_CONFIG_TS_ITVL_EST_COEF)
                        + est_itvl * (10 - BMA2X2_CONFIG_TS_ITVL_EST_COEF)) / 10;
            }
        } else {
            state->avail_accurate_ts_last_sample = 1;

            est_end = state->ts_irq + (f_count - state->f_wml) * state->ts_itvl_est;
            est_start = est_end - (f_count - 1) * state->ts_itvl_est;

            devi = abs((int32_t)est_start - (int32_t)state->ts_1st_sample_est);
            if (devi >= state->ts_itvl_est) {
                BMA2X2_MSG_2(ERROR, "NOTICE!!! 1st sample est error too big: <%d %d>",
                        est_start,
                        state->ts_1st_sample_est);
            }

            BMA2X2_MSG_3(HIGH, "[update_sample_ts] <%u %u %u>",
                    state->ts_1st_sample_est,
                    est_start,
                    est_end);
        }

        state->ts_irq_last_fwml = state->ts_irq;
        state->f_acnt_frame_cnt_irq = state->f_acnt_frame_cnt + state->f_wml;
    } else {
        if (state->avail_accurate_ts_last_sample && (!f_overrun)) {
            est_end = state->ts_last_sample + f_count * state->ts_itvl_est;
            est_start = state->ts_last_sample + state->ts_itvl_est;
        } else {
            BMA2X2_MSG_2(ERROR, "NOTICE!!! timestamp is likely not accurate in this case: %d ts_curr: %d",
                    state->ts_last_sample, ts_curr);

            //CAN BE rounded
            est_end = ts_curr;
            est_start = est_end - (f_count - 1) * state->ts_itvl_est;
        }
    }

    if (sns_dd_bms_time_not_before(state->ts_last_sample, est_start)) {
        BMA2X2_MSG_2(ERROR, "[update_sample_ts] NOTICE!!! l.e >= t.s<%u %u>",
                        state->ts_last_sample,
                        est_start);

        est_start = state->ts_last_sample + state->ts_itvl_est;

        if (1 == f_count) {
           est_end = est_start;
        }
    }

    if (sns_dd_bms_time_not_before(est_end, ts_curr)) {
        BMA2X2_MSG_3(ERROR, "[update_sample_ts] NOTICE!!! end >= ts_curr<%u %u %u>",
                        est_start,
                        est_end,
                        ts_curr);

        est_end = ts_curr;

        if (1 == f_count) {
           est_start = est_end;
        }
    }

    if (sns_dd_bms_time_not_before(est_start, est_end)) {
        if ( f_count>1 ) {
            est_start= state->ts_last_sample+(est_end-state->ts_last_sample)/f_count;
        }
        else {
            est_start = est_end;
        }
    }
    state->f_frames_cache.timestamp = est_start;
    state->f_frames_cache.end_timestamp = est_end;

    state->f_acnt_frame_cnt += f_count;

    BMA2X2_MSG_4(MED, "[update_sample_ts] in_irq:%d start=%u end=%u acnt_frm_cnt=%u",
            in_irq,
            est_start,
            est_end,
            state->f_acnt_frame_cnt);

    BMA2X2_MSG_3(MED, "[update_sample_ts] ts_curr: %d diff: %d f_count: %d",
            ts_curr, abs((int32_t)ts_curr - (int32_t)est_end), f_count);

    if (abs((int32_t)ts_curr - (int32_t)est_end) >= (state->ts_itvl_ideal * 2)) {
        BMA2X2_MSG_0(MED, "[update_sample_ts] NOTICE!!! timestamp out out range");
    }

    state->ts_last_sample = state->f_frames_cache.end_timestamp;
}

sns_ddf_status_e bma2x2_fifo_data_read_out_frames(
        sns_dd_acc_bma2x2_state_t * state,
        uint8_t f_count,
        bool    f_overrun,
        bool    in_irq)
{
    int i;
    unsigned char fifo_data_out[BMA2X2_MAX_FIFO_LEVEL * BMA2X2_MAX_FIFO_F_BYTES] = {0};
    unsigned char *frame_pointer;
    unsigned char f_len = 6;
    //unsigned char fifo_datasel = 0;
    struct bma2x2_data acc = {0};
    sns_ddf_status_e stat;
    bool frames_has_invalid_data = false;
    int32_t num_frames_invalid_data = 0;
    sns_ddf_time_t ts_curr;

    ts_curr = sns_ddf_get_timestamp();

    BMA2X2_MSG_3(HIGH,
            "[fifo_data_read_out_frames] f_count=%d in_irq: %d ts_curr: %d",
            f_count, in_irq, ts_curr);

#if 0
    if ((stat = bma2x2_get_fifo_data_sel(state->port_handle, &fifo_datasel))!= SNS_DDF_SUCCESS)
        return stat;

    BMA2X2_MSG_3_F(LOW, "[fifo_data_read_out_frames] 2 =%d i = %d %d", 0, 1, 5555584);
    if (fifo_datasel)
        f_len = 2;
    else
        f_len = 6;
#endif


    if ((stat = bma2x2_smbus_read_byte_block(state->port_handle,
                    BMA2X2_FIFO_DATA_OUTPUT_REG,
                    fifo_data_out, f_count * f_len))
            != SNS_DDF_SUCCESS) {
        return stat;
    }

    sns_dd_acc_bma2x2_update_sample_ts(state, in_irq, f_count, f_overrun, ts_curr);

    //parse the frames
    frame_pointer = fifo_data_out;

    for (i = 0; i < f_count; i++) {
        //BMA2X2_MSG_3_P(LOW, "FIFO sample each fifo read f_count = %d i = %d %d", f_count, i, 5555584);

        bma2x2_parse_fifo_frame(frame_pointer, state->sub_model, &acc);
        frame_pointer += f_len;

        if ((BMA280_TYPE != state->sub_model)
                || (BMA2X2_DEV_SEL_NUM_BMI058 != state->dev_select)) {
            state->data_cache_fifo[SDD_ACC_X] =
                FX_FLTTOFIX_Q16((4<<state->range_idx) * BMA2x2_G_OVER_RANGE_LIST[state->sub_model] * acc.x);
            state->data_cache_fifo[SDD_ACC_Y] =
                FX_FLTTOFIX_Q16((4<<state->range_idx) * BMA2x2_G_OVER_RANGE_LIST[state->sub_model] * acc.y);
        } else {
            state->data_cache_fifo[SDD_ACC_X] =
                FX_FLTTOFIX_Q16((4<<state->range_idx) * BMA2x2_G_OVER_RANGE_LIST[state->sub_model] * acc.y);
            state->data_cache_fifo[SDD_ACC_Y] =
                FX_FLTTOFIX_Q16((4<<state->range_idx) * BMA2x2_G_OVER_RANGE_LIST[state->sub_model] * acc.x);
        }

        state->data_cache_fifo[SDD_ACC_Z] =
            FX_FLTTOFIX_Q16((4<<state->range_idx) * BMA2x2_G_OVER_RANGE_LIST[state->sub_model] * acc.z);

        sns_ddf_map_axes(&state->axes_map, &state->data_cache_fifo[SDD_ACC_X]);

        state->f_frames_cache.samples[SDD_ACC_NUM_AXIS*i+SDD_ACC_X].sample = state->data_cache_fifo[SDD_ACC_X];
        state->f_frames_cache.samples[SDD_ACC_NUM_AXIS*i+SDD_ACC_Y].sample = state->data_cache_fifo[SDD_ACC_Y];
        state->f_frames_cache.samples[SDD_ACC_NUM_AXIS*i+SDD_ACC_Z].sample = state->data_cache_fifo[SDD_ACC_Z];

        state->f_frames_cache.samples[SDD_ACC_NUM_AXIS*i+SDD_ACC_X].status = SNS_DDF_SUCCESS;
        state->f_frames_cache.samples[SDD_ACC_NUM_AXIS*i+SDD_ACC_Y].status = SNS_DDF_SUCCESS;
        state->f_frames_cache.samples[SDD_ACC_NUM_AXIS*i+SDD_ACC_Z].status = SNS_DDF_SUCCESS;


#if BMA2X2_CONFIG_ENABLE_DATA_LOG
        BMA2X2_MSG_3_P(HIGH, "FIFO sample <%d %d %d>",
                state->f_frames_cache.samples[SDD_ACC_NUM_AXIS*i+SDD_ACC_X].sample,
                state->f_frames_cache.samples[SDD_ACC_NUM_AXIS*i+SDD_ACC_Y].sample,
                state->f_frames_cache.samples[SDD_ACC_NUM_AXIS*i+SDD_ACC_Z].sample);
#endif

        if (state->sample_cnt_to_skip_odrc > 0) {
            frames_has_invalid_data = true;

            state->f_frames_cache.samples[SDD_ACC_NUM_AXIS*i+SDD_ACC_X].status = SNS_DDF_EINVALID_DATA;
            state->f_frames_cache.samples[SDD_ACC_NUM_AXIS*i+SDD_ACC_Y].status = SNS_DDF_EINVALID_DATA;
            state->f_frames_cache.samples[SDD_ACC_NUM_AXIS*i+SDD_ACC_Z].status = SNS_DDF_EINVALID_DATA;

            num_frames_invalid_data++;

            state->sample_cnt_to_skip_odrc--;
        }
    }

    if (!frames_has_invalid_data) {
    } else {
        uint32_t itvl = 0;
        bma2x2_sub_dev_t *sub_dev = sns_dd_acc_bma2x2_get_subdev_from_type(state,
                SNS_DDF_SENSOR_ACCEL);

        if (NULL == sub_dev)
        {
            BMA2X2_MSG_2(ERROR, "[fifo_data_read_out_frames] sub_device is NULL! f_count = %d in_irq = %d",
                    f_count, in_irq);
            return SNS_DDF_EFAIL;
        }

        if (f_count > 1) {
            itvl = (state->f_frames_cache.end_timestamp - state->f_frames_cache.timestamp) / (f_count - 1);
        }

        state->f_frames_cache.status            = SNS_DDF_EINVALID_DATA;
        state->f_frames_cache.num_samples       = num_frames_invalid_data * SDD_ACC_NUM_AXIS;

        if (f_count> 1) {
            state->f_frames_cache.end_timestamp     = state->f_frames_cache.timestamp +
                (num_frames_invalid_data - 1) * itvl;
        }

        if (state->f_frames_cache.num_samples > 0) {
            stat = sns_ddf_smgr_notify_data(sub_dev->smgr_handle,
                    &state->f_frames_cache, 1);
        }


        //for the good part
        f_count -= num_frames_invalid_data;
        state->f_frames_cache.status            = SNS_DDF_SUCCESS;
        state->f_frames_cache.timestamp         = state->f_frames_cache.end_timestamp + itvl;
        state->f_frames_cache.end_timestamp     = state->f_frames_cache.timestamp + itvl * (f_count - 1);

        if (f_count > 0) {
            memcpy(state->f_frames_cache.samples,
                    state->f_frames_cache.samples + num_frames_invalid_data * SDD_ACC_NUM_AXIS,
                    sizeof(state->f_frames_cache.samples[0]) * f_count * SDD_ACC_NUM_AXIS);
        }

        BMA2X2_MSG_3(HIGH, "num_frames_invalid_data: %d timestamp: %d end_timestamp: %d",
                num_frames_invalid_data,
                state->f_frames_cache.timestamp,
                state->f_frames_cache.end_timestamp);

        /*if ( state->f_frames_cache.timestamp > state->f_frames_cache.end_timestamp ) {
          BMA2X2_MSG_0(HIGH, "BMA2x2 timestamp rollover");
          }*/
    }

    state->f_frames_cache.status = SNS_DDF_SUCCESS;
    state->f_frames_cache.sensor = SNS_DDF_SENSOR_ACCEL;

    /*! notice : the number samples value */
    state->f_frames_cache.num_samples = f_count * SDD_ACC_NUM_AXIS;


    return stat;
}

/**
 * @brief Retrieves a set of sensor data. Asynchronous API
 *
 * Requests sample of sensor data from the specified sensor.
 *
 * @note If a sensor has failed or
 *       isn't available, @a sns_ddf_sensor_data_s.status must be used to
 *       reflect this status.
 *
 * @param[in]  dd_handle       Handle to a driver instance.
 * @param[in]  sensor          sensor for which data is requested.
 *
 * @param[in]  num_samples     number of samples to retrieve as available. Drain the FIFO if value is set to Zero.
 * @param[in]  trigger now     trigger nofity fifo data now or
 *       later when trigger_now is set to true.
 *
 *
 * @return SNS_DDF_SUCCESS     if data was populated successfully.
 *         via sns_ddf_smgr_data_notify() or if trigger_now is
 *         set to false; Otherwise a specific error code is
 *         returned.
 *
 * @see sns_ddf_data_notify_data() as this will be used to report the data.
 */

static sns_ddf_status_e sns_dd_acc_bma2x2_trigger_fifo_data(
        sns_ddf_handle_t    dd_handle,
        sns_ddf_sensor_e    sensor,
        uint16_t            num_samples,
        bool                trigger_now)
{
    sns_dd_acc_bma2x2_state_t *state = sns_dd_acc_bma2x2_get_dev(dd_handle);
    bma2x2_sub_dev_t *sub_dev;
    sns_ddf_status_e stat;
    uint8_t f_count = 0;
    uint8_t f_overrun = 0;

    BMA2X2_MSG_4(HIGH, "<bma2x2_if_ trigger_fifo_data> 0x%x sensor: %d num_samples=%d trigger_now=%d",
            dd_handle, sensor, num_samples, trigger_now);

    if (0 == trigger_now) {
        return SNS_DDF_SUCCESS;
    }

    if (0 == num_samples) {
        //TODO: notify FIFO_OVERFLOW event???
        if ((stat = bma2x2_get_fifo_framecount(state->port_handle,
                        &f_count, &f_overrun))
                != SNS_DDF_SUCCESS)
            return stat;
    } else {
        f_count = num_samples;
    }

    if (f_count > 0) {
        if (f_count >= BMA2X2_MAX_FIFO_LEVEL) {
            BMA2X2_MSG_3_F(ERROR, "sns_dd_acc_bma2x2_trigger_fifo_data overflow condition"
                    "55555548 timestamp: %d wml: %d f_count: %d",
                    sns_ddf_get_timestamp(), state->f_wml, f_count);

            f_count = BMA2X2_MAX_FIFO_LEVEL - 1;
        }

        bma2x2_fifo_data_read_out_frames(state, f_count, f_overrun, 0);

        sub_dev = sns_dd_acc_bma2x2_get_subdev_from_type(state,
                SNS_DDF_SENSOR_ACCEL);

        if (NULL != sub_dev ) {
            /*! notice : the number data len */
            if (state->f_frames_cache.num_samples > 0) {
                stat = sns_ddf_smgr_notify_data(sub_dev->smgr_handle,
                        &state->f_frames_cache, 1);
                sns_dd_accel_log_fifo(&state->f_frames_cache, DD_DATA_LOG_CONTEXT_FIFO_FLUSH);
            }

            if (f_overrun) {
                uint8_t regv;

                regv = state->regv_fifo_config_1;
                //this will reset the FIFO including the overrun flag
                stat |= bma2x2_smbus_write_byte(state->port_handle, BMA2X2_REGA_FIFO_CONFIG_1, &regv);

                if (!state->f_overrun_detected_in_irq) {
                    //we still want to notify SMGR that OVERFLOW happened, thus it knows some samples might have been lost
                    stat |= sns_ddf_smgr_notify_event(sub_dev->smgr_handle,
                            SNS_DDF_SENSOR_ACCEL,
                            SNS_DDF_EVENT_FIFO_OVERFLOW);
                }

                state->f_overrun_detected_in_irq = 0;
            }
        }
        else
        {
            BMA2X2_MSG_3_F(ERROR, "sns_dd_acc_bma2x2_trigger_fifo_data sub_device is NULL %d num_samples = %d trigger_now = %d",
                    55555521, num_samples, trigger_now);
            return SNS_DDF_EFAIL;
        }
    }


    return stat;
}


