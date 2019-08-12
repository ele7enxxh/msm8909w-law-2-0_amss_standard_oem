/*******************************************************************************
 * Copyright (c) 2014, Bosch Sensortec GmbH
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

/****************************************************************************
  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
 ****************************************************************************/


#include "sns_dd_bmi160.h"
#include "sns_dd_bmi160_priv.h"

sns_ddf_driver_if_s SNS_DD_IF_BMI160;

sns_ddf_handle_t bmi160_sns_port_handle_singleton = NULL;

const double BMI160_SCALE_FACTOR_ACC_PM2G = (4 * G) / 65536;
const double BMI160_SCALE_FACTOR_GYR_PM125DPS = (250 * PI) / 180 / 65536;
const double BMI160_SCALE_FACTOR_TEMP = 1.0 / 512;

const double BMI160_TCOC_COEFX_MS2 = G * BMI160_CONFIG_TCOC_ACC_COEFX * 1e-3;
const double BMI160_TCOC_COEFY_MS2 = G * BMI160_CONFIG_TCOC_ACC_COEFY * 1e-3;
const double BMI160_TCOC_COEFZ_MS2 = G * BMI160_CONFIG_TCOC_ACC_COEFZ * 1e-3;
/*! function implementations */


/*! make sure the return value from sns_ddf_read_port is pass through
 *  as other code will check against some return values
 */
int32_t bmi160_sbus_read_wrapper(
        void                    *plat_obj,
        uint8_t                 reg_addr,
        uint8_t                 *buf,
        uint32_t                len)
{
    sns_ddf_status_e        status;
    sns_dd_bmi160_state_t   *state = (sns_dd_bmi160_state_t *)plat_obj;
    uint32_t delay_us = 0;

    if (len < 1) {
        return SNS_DDF_EINVALID_PARAM;
    }

    //<wait according to spec>
    if (!state->sbus_in_normal_mode) {
        sns_ddf_time_t ts_curr = bmi160_get_sys_tick();

        if (ts_curr > state->ts_last_sbus_write) {
            delay_us = bmi160_convert_tick_2_us(ts_curr - state->ts_last_sbus_write);

            if (delay_us > BMI160_SPEC_IF_IDLE_TIME_SUSPEND_US) {
                delay_us = 0;
            } else {
                delay_us = BMI160_SPEC_IF_IDLE_TIME_SUSPEND_US - delay_us;
            }
        } else {
            delay_us = BMI160_SPEC_IF_IDLE_TIME_SUSPEND_US;
        }

        if (delay_us > 0) {
            sns_ddf_delay(delay_us);
        }
    }
    //</wait according to spec>

    uint8_t out_wrapper;

    status = sns_ddf_read_port(
            state->port_handle_real,
            reg_addr,
            buf,
            len,
            &out_wrapper);

    if ((1 == len) && !state->sbus_mon_single_byte_rw_off) {
        BMI160_MSG_4(HIGH, "<sbus_read> <@0x%02x : 0x%02x status: %d d: %d>",
                reg_addr, buf[0], status, delay_us);
    }

    return status;
}

/*! make sure the return value from sns_ddf_write_port is pass through
 *  as other code will check against some return values
 */
int32_t bmi160_sbus_write_wrapper(
        void                    *plat_obj,
        uint8_t                 reg_addr,
        uint8_t                 *buf,
        uint32_t                len)
{
    sns_ddf_status_e status;
    uint32_t delay_us = 0;

    sns_dd_bmi160_state_t   *state = (sns_dd_bmi160_state_t *)plat_obj;

    if (len < 1) {
        return SNS_DDF_EINVALID_PARAM;
    }

    //<wait according to spec>
    if (!state->sbus_in_normal_mode) {
        sns_ddf_time_t ts_curr = bmi160_get_sys_tick();

        if (ts_curr > state->ts_last_sbus_write) {
            delay_us = bmi160_convert_tick_2_us(ts_curr - state->ts_last_sbus_write);

            if (delay_us > BMI160_SPEC_IF_IDLE_TIME_SUSPEND_US) {
                delay_us = 0;
            } else {
                delay_us = BMI160_SPEC_IF_IDLE_TIME_SUSPEND_US - delay_us;
            }
        } else {
            delay_us = BMI160_SPEC_IF_IDLE_TIME_SUSPEND_US;
        }

        if (delay_us > 0) {
            sns_ddf_delay(delay_us);
        }
    }
    //</wait according to spec>

    uint8_t out;
    status = sns_ddf_write_port(state->port_handle_real,
            reg_addr,
            buf,
            len,
            &out);
    state->ts_last_sbus_write = bmi160_get_sys_tick();

    if (1 == len) {
        switch (buf[0]) {
            case BMI160_REGV_CMD_ACC_MODE_NORMAL:
            case BMI160_REGV_CMD_GYR_MODE_NORMAL:
            case BMI160_REGV_CMD_MAG_MODE_NORMAL:
                //note: this API is not suitable for frequent power mode transitions because the timer will be started too frequently

                status = sns_dd_bmi160_start_timer_sbus_susp(state);
                BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);
                break;
            case BMI160_REGV_CMD_SOFT_RESET:
                state->sbus_in_normal_mode = false;
                state->timer_pending_sbus_susp_mode = false;
                break;
            default:
                break;
        }
    }

    if ((1 == len) && !state->sbus_mon_single_byte_rw_off) {
        BMI160_MSG_4(HIGH, "<sbus_write> <@0x%02x : 0x%02x status: %d d: %d>",
                reg_addr, buf[0], status, delay_us);
    }

    return status;
}

//reviewed
void bmi160_parse_data_acc(
        uint8_t                 *buf,
        uint8_t                 len,
        struct bmi160_val_acc   *val)
{
    val->x = (buf[1] << 8) | (buf[0]);
    val->y = (buf[3] << 8) | (buf[2]);
    val->z = (buf[5] << 8) | (buf[4]);
}

//reviewed
static void bmi160_parse_data_ts(
        uint8_t                 *buf,
        uint8_t                 len,
        uint32_t                *ts_hw)
{
    *ts_hw = ((buf[2] << 16)
            | (buf[1] << 8)
            | (buf[0]));
}

//reviewed
static void bmi160_parse_data_gyr(
        uint8_t                 *buf,
        uint8_t                 len,
        struct bmi160_val_gyr   *val)
{
    val->x = (buf[1] << 8) | (buf[0]);
    val->y = (buf[3] << 8) | (buf[2]);
    val->z = (buf[5] << 8) | (buf[4]);
}

void bmi160_parse_reg_status(
        uint8_t                 regv_status,
        bmi160_status_reg_t     *stat_reg)
{
    stat_reg->bits.drdy_acc = BST_GET_VAL_BIT(regv_status, 7);
    stat_reg->bits.drdy_gyr = BST_GET_VAL_BIT(regv_status, 6);
    stat_reg->bits.drdy_mag = BST_GET_VAL_BIT(regv_status, 5);
}

#if BMI160_CONFIG_ENABLE_INT_DOUBLE_TAP
/*!
 *  @brief
 *  @return
 *  Available values
 *  0x00: none
 *  0x10: x,
 *  0x20: y,
 *  0x40: z
 */
//reviewed
int32_t bmi160_get_tap_first(uint8_t int_stat)
{
    int8_t first_x;
    int8_t first_y;
    int8_t first_z;

    first_x = BST_GET_VAL_BIT(int_stat, 4);
    first_y = BST_GET_VAL_BIT(int_stat, 5);
    first_z = BST_GET_VAL_BIT(int_stat, 6);

    //this code have preference as: z->y->x
    if (first_z) {
        return BMI160_INT_INFO_TAP_FIRST_Z;
    }

    if (first_y) {
        return BMI160_INT_INFO_TAP_FIRST_Y;
    }

    if (first_x) {
        return BMI160_INT_INFO_TAP_FIRST_X;
    }

    return BMI160_INT_INFO_TAP_FIRST_NONE;
}

#endif

//NU
sns_ddf_status_e bmi160_get_reg_ts_hw(
        sns_ddf_handle_t        port_handle,
        uint32_t                *ts_hw)
{
    sns_ddf_status_e status;
    uint8_t si_buf_rd[3];

    status = bmi160_sbus_read_wrapper(port_handle,
            BMI160_REGA_USR_SENSORTIME_0, si_buf_rd, 3);
    BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

    bmi160_parse_data_ts(si_buf_rd, 3, ts_hw);

    return SNS_DDF_SUCCESS;
}

//uted
uint32_t bmi160_get_ts_dev_update_msb(
        uint32_t start,
        uint32_t end,
        uint8_t  lsb_bits)
{
    uint32_t mask = ~((1 << lsb_bits) - 1);
    uint32_t ret;

    start &= mask;
    end &= mask;

    if (end >= start) {
        ret = (end - start) >> lsb_bits;
    } else {
        ret = (BMI160_TS_LSB_OVERFLOW_VAL - start + end) >> lsb_bits;
    }

    return ret;
}

//reviewed
sns_ddf_status_e bmi160_get_reg_hw_err_stat(
        sns_ddf_handle_t                port_handle,
        union bmi160_hw_err_stat        *hw_err_st)
{
    sns_ddf_status_e status;
    uint8_t si_buf_rd;

    status = bmi160_sbus_read_wrapper(
            port_handle,
            BMI160_REGA_USR_ERR_REG,
            &si_buf_rd,
            1);
    BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

    hw_err_st->bits.mag_drdy_err    = BST_GET_VAL_BITBLOCK(si_buf_rd, 7, 7);
    hw_err_st->bits.drop_cmd_err    = BST_GET_VAL_BITBLOCK(si_buf_rd, 6, 6);
    hw_err_st->bits.i2c_fail_err    = BST_GET_VAL_BITBLOCK(si_buf_rd, 5, 5);
    hw_err_st->bits.err_code        = BST_GET_VAL_BITBLOCK(si_buf_rd, 1, 4);
    hw_err_st->bits.fatal_err       = BST_GET_VAL_BITBLOCK(si_buf_rd, 0, 0);

    if (hw_err_st->regv) {
        BMI160_MSG_3(ERROR, "WARNING!!! hw_err_st: 0x%x %d %d",
                hw_err_st->regv,
                hw_err_st->bits.fatal_err,
                hw_err_st->bits.err_code);

        BMI160_MSG_3(ERROR, "WARNING!!! hw_err_st: 0x%x %d %d",
                hw_err_st->bits.mag_drdy_err,
                hw_err_st->bits.drop_cmd_err,
                hw_err_st->bits.i2c_fail_err);
    }

    return SNS_DDF_SUCCESS;
}

//uted
sns_ddf_status_e bmi160_get_reg_fifo_count(
        sns_ddf_handle_t                port_handle,
        uint16_t                        *fifo_cnt)
{
    sns_ddf_status_e status;
    uint8_t si_buf[2] = "";

    status = bmi160_sbus_read_wrapper(port_handle,
            BMI160_REGA_USR_FIFO_LENGTH_0,
            si_buf, 2);

    *fifo_cnt = ((si_buf[1] & 0x07) << 8) | si_buf[0];

    return status;
}

//reviewed
static inline sns_ddf_status_e bmi160_reset_int(void    *plat_obj)
{
    sns_ddf_status_e status;
    uint8_t si_buf_wr = BMI160_REGV_CMD_INT_RESET;

    status = bmi160_sbus_write_wrapper(plat_obj,
            BMI160_REGA_CMD_CMD, &si_buf_wr, 1);

    return status;
}

//reviewed
sns_ddf_status_e bmi160_read_fifo_bytes(
        void    *plat_obj,
        uint8_t *data,
        uint32_t len,
        uint32_t *out)
{
    sns_dd_bmi160_state_t   *state = (sns_dd_bmi160_state_t *)plat_obj;

#if BMI160_CONFIG_ENABLE_FIFO
    sns_ddf_status_e status;

    status = sns_ddf_read_port_u32_count(state->port_handle_real,
            BMI160_REGA_USR_FIFO_DATA,
            data,
            len,
            out);
    BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);
#endif

    return SNS_DDF_SUCCESS;
}


int32_t bmi160_read_reg_int_ctx(
        void                            *plat_obj,
        bmi160_int_en_flag_t            *int_en_flags,
        struct bmi160_reg_int_context   *ctx)
{
    int32_t err;
    sns_ddf_handle_t port_handle = (sns_ddf_handle_t)plat_obj;
    int8_t reg_addr_start = -1;
    int8_t reg_addr_end = -1;
    int8_t len = 0;
    uint8_t si_buf_rd[9] = "";

    sns_ddf_time_t t1;
    sns_ddf_time_t t2;

    ctx->avail.flags = 0;
    //always include ts
    reg_addr_start = BMI160_REGA_USR_SENSORTIME_0;
    ctx->avail.bits.ts = 1;

    ctx->avail.bits.status_0 = 1;
    ctx->avail.bits.status_1 = 1;
    if (!int_en_flags->bits.dt) {
        reg_addr_end = BMI160_REGA_USR_INT_STATUS_1;
    } else {
        reg_addr_end = BMI160_REGA_USR_INT_STATUS_2;
        ctx->avail.bits.status_2 = 1;
    }

    len = reg_addr_end - reg_addr_start + 1;
    if ((len > 0) && (len <= BST_ARRAY_SIZE(si_buf_rd))) {

        t1 = bmi160_get_sys_tick();
        err = bmi160_sbus_read_wrapper(port_handle,
                reg_addr_start, si_buf_rd, len);
        t2 = bmi160_get_sys_tick();

        ctx->ts_sys = t1 + (sns_dd_bmi160_get_time_elapse_sys(t1, t2) >> 1);

        BMI160_DD_CHECK_RETVAL(err, 0);

        bmi160_parse_data_ts(si_buf_rd, 3, &ctx->ts_hw);

        ctx->status    = si_buf_rd[3];
        ctx->int_status_0 = si_buf_rd[4];
        ctx->int_status_1 = si_buf_rd[5];
        ctx->int_status_2 = si_buf_rd[6];
    } else {
        return SNS_DDF_EINVALID_PARAM;
    }

    return 0;
}

inline void bmi160_parse_int_stat_flags(
        struct bmi160_reg_int_context   *ctx,
        bmi160_int_stat_flag_t          *int_stat)
{
    int_stat->bits.md       = BST_GET_VAL_BIT(ctx->int_status_0, 2);
    int_stat->bits.dt       = BST_GET_VAL_BIT(ctx->int_status_0, 4);

    int_stat->bits.drdy     = BST_GET_VAL_BIT(ctx->int_status_1, 4);
    int_stat->bits.ff_full  = BST_GET_VAL_BIT(ctx->int_status_1, 5);
    int_stat->bits.ff_wml   = BST_GET_VAL_BIT(ctx->int_status_1, 6);
}

//reviewing
static sns_ddf_status_e bmi160_read_data(
        sns_ddf_handle_t                        port_handle,
        struct bmi160_sensor_data_raw_buf       *data,
        union bmi160_sensor_data_req            *req)
{
    sns_ddf_status_e        status = SNS_DDF_SUCCESS;
    uint8_t                 len;
    uint8_t                 idx;
    uint8_t                 reg_addr_start;
    uint8_t                 reg_addr_end;
    uint8_t                 si_buf_rd[32] = "";

    //read data a, g, m firstly, then temperature
    reg_addr_start = 0;
    reg_addr_end = 0;

    if (req->bits.m) {
        reg_addr_start  = BMI160_REGA_USR_DATA_0;
        reg_addr_end    = BMI160_REGA_USR_DATA_7;
    }

    if (req->bits.g) {
        if (0 == reg_addr_start) {
            reg_addr_start  = BMI160_REGA_USR_DATA_8;
        }
        reg_addr_end    = BMI160_REGA_USR_DATA_13;
    }

    if (req->bits.a) {
        if (0 == reg_addr_start) {
            reg_addr_start  = BMI160_REGA_USR_DATA_14;
        }
        reg_addr_end    = BMI160_REGA_USR_DATA_19;
    }

    if (req->bits.ts) {
        if (0 == reg_addr_start) {
            reg_addr_start  = BMI160_REGA_USR_SENSORTIME_0;
        }
        reg_addr_end    = BMI160_REGA_USR_SENSORTIME_2;
    }

    //we want to do a burst read of data and time together
    //for accuracy and sync of time

    if ((reg_addr_start> 0) && (reg_addr_end >= reg_addr_start)) {
        len = reg_addr_end - reg_addr_start + 1;
    } else {
        len = 0;
    }

    if (len > 0) {
        status = bmi160_sbus_read_wrapper(port_handle,
                reg_addr_start,
                si_buf_rd,
                len);
        BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);
    }

    idx = 0;

    if (req->bits.m) {
#if BMI160_CONFIG_ENABLE_MAG_IF
        memscpy(data->buf_m, 8, si_buf_rd + idx, 8);
#endif
        idx += 8;
    }

    if (req->bits.g) {
        memscpy(data->buf_g, 6, si_buf_rd + idx, 6);
    }

    if ((idx > 0) || (req->bits.g)) {
        idx += BMI160_DATA_LEN_GYR;
    }

    if (req->bits.a) {
        memscpy(data->buf_a, 6, si_buf_rd + idx, 6);
    }

    if ((idx > 0) || (req->bits.a)) {
        idx += BMI160_DATA_LEN_ACC;
    }

    if (req->bits.ts) {
        memscpy(data->buf_ts, 3, si_buf_rd + idx, 3);
        data->avail_ts = true;
    }

    //read temperature seperately
    if (req->bits.t) {
        reg_addr_start  = BMI160_REGA_USR_TEMPERATURE_0;
        reg_addr_end    =  BMI160_REGA_USR_TEMPERATURE_1;
        len = reg_addr_end - reg_addr_start + 1;

        status = bmi160_sbus_read_wrapper(port_handle,
                reg_addr_start,
                si_buf_rd,
                len);
        BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

        memscpy(data->buf_temp, 2, si_buf_rd + idx, 2);
    }

    return status;
}

#if DEBUG
static void cd2__________________________________________________dummy()
{
}
#endif
sns_dd_bmi160_state_t* sns_dd_bmi160_get_dev(
        sns_ddf_handle_t dd_handle)
{
    bmi160_sub_dev_t *sub_dev = (bmi160_sub_dev_t *)dd_handle;
    return (sns_dd_bmi160_state_t *)sub_dev->private_data;
}

//reviewed
bmi160_sub_dev_t* sns_dd_bmi160_get_subdev_from_type(
        sns_dd_bmi160_state_t *state,
        const sns_ddf_sensor_e       sensor_type)
{
    bmi160_sub_dev_t *sub_dev = NULL;
    uint32_t i;
    uint32_t j;

    for (i = 0; i < state->sub_dev_cnt; i++) {
        sub_dev = state->sub_dev + i;
        for (j = 0; j < BMI160_MAX_SENSORS_NUM; j++) {
            if (sensor_type == sub_dev->sensors[j].sensor_type) {
                return sub_dev;
            }
        }
    }

    return NULL;
}

static inline bmi160_sub_dev_t* sns_dd_bmi160_get_subdev_cache_acc(
        sns_dd_bmi160_state_t *state)
{
    if (!state->sub_dev_cache_acc) {
        state->sub_dev_cache_acc =
            sns_dd_bmi160_get_subdev_from_type(state,
                    SNS_DDF_SENSOR_ACCEL);
    }

    return state->sub_dev_cache_acc;
}

static inline bmi160_sub_dev_t* sns_dd_bmi160_get_subdev_cache_gyr(
        sns_dd_bmi160_state_t *state)
{
    if (!state->sub_dev_cache_gyr) {
        state->sub_dev_cache_gyr =
            sns_dd_bmi160_get_subdev_from_type(state,
                    SNS_DDF_SENSOR_GYRO);
    }

    return state->sub_dev_cache_gyr;
}

#if BMI160_CONFIG_ENABLE_MAG_IF
static inline bmi160_sub_dev_t* sns_dd_bmi160_get_subdev_cache_mag(
        sns_dd_bmi160_state_t *state)
{
    if (!state->sub_dev_cache_mag) {
        state->sub_dev_cache_mag =
            sns_dd_bmi160_get_subdev_from_type(state,
                    SNS_DDF_SENSOR_MAG);
    }

    return state->sub_dev_cache_mag;
}
#endif

//reviewed
bmi160_hw_mod_t* sns_dd_bmi160_get_hw_mod(
        sns_dd_bmi160_state_t   *state,
        sns_ddf_sensor_e        sensor_type)
{
    uint32_t i;
    uint32_t j;
    struct bmi160_hw_mod *hw_mod;

    for (i = 0; i < BST_ARRAY_SIZE(state->hw_mods); i++) {
        hw_mod = state->hw_mods + i;
        for (j = 0; j < BST_ARRAY_SIZE(hw_mod->sensor_type_supported); j++) {
            if (hw_mod->sensor_type_supported[j] == sensor_type) {
                return hw_mod;
            }
        }
    }


    return NULL;
}

struct bmi160_hw_mod* sns_dd_bmi160_get_hw_mod_cache_acc(
        sns_dd_bmi160_state_t   *state)
{
    if (!state->hw_mod_cache_acc) {
        state->hw_mod_cache_acc =
            sns_dd_bmi160_get_hw_mod(state, SNS_DDF_SENSOR_ACCEL);
    }

    return state->hw_mod_cache_acc;
}

struct bmi160_hw_mod* sns_dd_bmi160_get_hw_mod_cache_gyr(
        sns_dd_bmi160_state_t   *state)
{
    if (!state->hw_mod_cache_gyr) {
        state->hw_mod_cache_gyr =
            sns_dd_bmi160_get_hw_mod(state, SNS_DDF_SENSOR_GYRO);
    }

    return state->hw_mod_cache_gyr;
}

#if BMI160_CONFIG_ENABLE_MAG_IF
struct bmi160_hw_mod* sns_dd_bmi160_get_hw_mod_cache_mag(
        sns_dd_bmi160_state_t   *state)
{
    if (!state->hw_mod_cache_mag) {
        state->hw_mod_cache_mag =
            sns_dd_bmi160_get_hw_mod(state, SNS_DDF_SENSOR_MAG);
    }

    return state->hw_mod_cache_mag;
}
#endif

//only use this API when you want to keep track of what last_cmd_regv is,
//especially for critical commands such as reset, power mode
sns_ddf_status_e sns_dd_bmi160_send_cmd(
        sns_dd_bmi160_state_t   *state,
        uint8_t                 cmd_regv)
{
    sns_ddf_status_e status;

    status = bmi160_sbus_write_wrapper(state->port_handle,
            BMI160_REGA_CMD_CMD,
            &cmd_regv, 1);

    state->last_cmd_regv = cmd_regv;
    BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

#if BMI160_CONFIG_ENABLE_DUMP_CORE_REG

    status = bmi160_get_reg_hw_err_stat(state->port_handle,
            &state->hw_err_st);
    BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

    if (state->hw_err_st.bits.drop_cmd_err) {
        //status = sns_dd_bmi160_send_cmd(state, state->last_cmd_regv);
    }
#endif

    return SNS_DDF_SUCCESS;
}

//reviewed
void sns_dd_bmi160_format_data_temp(
        sns_dd_bmi160_state_t   *state,
        uint8_t                 buf[],
        double                  *temp)
{
    int16_t temp_lsb;

    temp_lsb = (int16_t)buf[0] | (((int16_t)buf[1]) << 8);

    *temp = (temp_lsb * BMI160_SCALE_FACTOR_TEMP + 23);
}

//reviewed
static void sns_dd_bmi160_format_data_acc(
        sns_dd_bmi160_state_t   *state,
        uint8_t                 buf[6],
        sns_ddf_sensor_sample_s samples[3])
{
    struct bmi160_val_acc   val_lsb;
    q16_t                   val_si[3] = {0};


    bmi160_parse_data_acc(buf, 6, &val_lsb);

#if BMI160_CONFIG_ENABLE_ACC_TCOC
    val_si[0] = FX_FLTTOFIX_Q16(((1 << state->range_idx_acc) * val_lsb.x * BMI160_SCALE_FACTOR_ACC_PM2G)
            - BMI160_TCOC_COEFX_MS2 * (state->cache_temperature - 25));
    val_si[1] = FX_FLTTOFIX_Q16(((1 << state->range_idx_acc) * val_lsb.y * BMI160_SCALE_FACTOR_ACC_PM2G)
            - BMI160_TCOC_COEFY_MS2 * (state->cache_temperature - 25));
    val_si[2] = FX_FLTTOFIX_Q16(((1 << state->range_idx_acc) * val_lsb.z * BMI160_SCALE_FACTOR_ACC_PM2G)
            - BMI160_TCOC_COEFZ_MS2 * (state->cache_temperature - 25));
#else
    val_si[0] = FX_FLTTOFIX_Q16((1 << state->range_idx_acc) * val_lsb.x * BMI160_SCALE_FACTOR_ACC_PM2G);
    val_si[1] = FX_FLTTOFIX_Q16((1 << state->range_idx_acc) * val_lsb.y * BMI160_SCALE_FACTOR_ACC_PM2G);
    val_si[2] = FX_FLTTOFIX_Q16((1 << state->range_idx_acc) * val_lsb.z * BMI160_SCALE_FACTOR_ACC_PM2G);
#endif

    sns_ddf_map_axes(&state->axes_map_acc, val_si);

    samples[0].sample = val_si[0];
    samples[1].sample = val_si[1];
    samples[2].sample = val_si[2];

    //TODO: optimize
    samples[0].status = SNS_DDF_SUCCESS;
    samples[1].status = SNS_DDF_SUCCESS;
    samples[2].status = SNS_DDF_SUCCESS;
}

//reviewed
static void sns_dd_bmi160_format_data_gyr(
        sns_dd_bmi160_state_t   *state,
        uint8_t                 buf[6],
        sns_ddf_sensor_sample_s samples[3])
{
    struct bmi160_val_gyr   val_lsb;
    q16_t                   val_si[3] = {0};

    bmi160_parse_data_gyr(buf, 6, &val_lsb);

    val_si[0] = FX_FLTTOFIX_Q16((1 << state->range_idx_gyr) * val_lsb.x * BMI160_SCALE_FACTOR_GYR_PM125DPS);
    val_si[1] = FX_FLTTOFIX_Q16((1 << state->range_idx_gyr) * val_lsb.y * BMI160_SCALE_FACTOR_GYR_PM125DPS);
    val_si[2] = FX_FLTTOFIX_Q16((1 << state->range_idx_gyr) * val_lsb.z * BMI160_SCALE_FACTOR_GYR_PM125DPS);

    sns_ddf_map_axes(&state->axes_map_gyr, val_si);

    samples[0].sample = val_si[0];
    samples[1].sample = val_si[1];
    samples[2].sample = val_si[2];

    samples[0].status = SNS_DDF_SUCCESS;
    samples[1].status = SNS_DDF_SUCCESS;
    samples[2].status = SNS_DDF_SUCCESS;
}

#if BMI160_CONFIG_ENABLE_MAG_IF
static void sns_dd_bmi160_format_data_mag(
        sns_dd_bmi160_state_t   *state,
        uint8_t                 buf[8],
        sns_ddf_sensor_sample_s samples[3])
{
    q16_t                   val_si[3] = {0};
    int32_t                 val_ug[3] = {0};

    struct bmi160_slave *slave_mag;
    slave_mag = state->slaves[BMI160_SLAVE_SENSOR_TYPE_MAG];
    slave_mag->parse_data(slave_mag->private_data_slave,
            buf, 8, val_ug, 3);

    val_si[0] = FX_FLTTOFIX_Q16(val_ug[0] * 1e-6);
    val_si[1] = FX_FLTTOFIX_Q16(val_ug[1] * 1e-6);
    val_si[2] = FX_FLTTOFIX_Q16(val_ug[2] * 1e-6);

    sns_ddf_map_axes(&state->axes_map_mag, val_si);

    samples[0].sample = val_si[0];
    samples[1].sample = val_si[1];
    samples[2].sample = val_si[2];

    samples[0].status = SNS_DDF_SUCCESS;
    samples[1].status = SNS_DDF_SUCCESS;
    samples[2].status = SNS_DDF_SUCCESS;
}
#endif

//uted
    static inline
sns_ddf_time_t sns_dd_bmi160_get_ts_backward(
        sns_ddf_time_t now,
        sns_ddf_time_t elapse)
{
    if (now >= elapse) {
        return now - elapse;
    } else {
        return (sns_ddf_time_t)(-1) - (elapse - now) + 1;
    }
}

//uted
void sns_dd_bmi160_get_ts_cor(
        enum BMI160_ODR_IDX     odr_idx,
        uint32_t                ts_dev,
        sns_ddf_time_t          ts_sys,
        sns_ddf_time_t          *ts_sys_cor)
{
    uint8_t  lsb_bits = (BMI160_ODR_IDX_1600HZ - BMI160_ODR_IDX_NORMAL_START - odr_idx) + 4;
    uint32_t age_dev = (ts_dev & ((1 << lsb_bits) - 1));
    uint32_t age_sys = BMI160_SSC_US_2_TICKS((uint32_t)(age_dev * BMI160_TS_HW_RES_US_F));

    if (ts_sys >= age_sys) {
        *ts_sys_cor = ts_sys - age_sys;
    } else {
        *ts_sys_cor = ((sns_ddf_time_t)(-1)) - (age_sys - ts_sys) + 1;

        BMI160_MSG_3(ERROR, "WARNING!!! time rewind: <%u %u %u>",
                ts_dev, ts_sys, *ts_sys_cor);
    }
}

    static
void sns_dd_bmi160_adj_sample_sys_tick(
        sns_dd_bmi160_state_t   *state,
        sns_ddf_sensor_data_s   *data)
{
#if BMI160_CONFIG_ENABLE_UT_TS
    data->timestamp -= TIME_SYS_OFFSET;
    data->end_timestamp -= TIME_SYS_OFFSET;
#endif

#if BMI160_CONFIG_ENABLE_TS_ADJUST_FILTER
    if (SNS_DDF_SENSOR_ACCEL == data->sensor) {
        data->timestamp -= (sns_ddf_time_t)(state->ts_adjust_acc);
        data->end_timestamp -= (sns_ddf_time_t)(state->ts_adjust_acc);
    } else if (SNS_DDF_SENSOR_GYRO == data->sensor) {
        data->timestamp -= (sns_ddf_time_t)(state->ts_adjust_gyr);
        data->end_timestamp -= (sns_ddf_time_t)(state->ts_adjust_gyr);
    }
#endif
}

//uted
void sns_dd_bmi160_get_couple_ts_host_and_dev(sns_dd_bmi160_state_t *state)
{
    sns_ddf_time_t ts;
    sns_ddf_time_t te;
    uint8_t buf[3] = "";

    ts = bmi160_get_sys_tick();
    bmi160_sbus_read_wrapper(state->port_handle,
            BMI160_REGA_USR_SENSORTIME_0,
            buf, 3);
    te = bmi160_get_sys_tick();


    state->ts_sync_sys_and_dev.ts_sys = ts + (sns_dd_bmi160_get_time_elapse_sys(ts, te) >> 1);

    bmi160_parse_data_ts(buf, 3, &state->ts_sync_sys_and_dev.ts_dev);
}

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
//reviewing
#if BMI160_FOR_8X26_BUILD
/*
   this version breaks 14c1 packets into sub packets if they exceed the
   size limitation imposed by.SNS_DEBUG_MAX_LOG_SIZE_V01
   */
void sns_dd_bmi160_log_data_be(
        const sns_ddf_sensor_data_s     *data_collection,
        uint8_t                         num_data_types_per_sample)
{
#if BMI160_CONFIG_ENABLE_LOG_DATA_PKT && !BMI160_CONFIG_RUN_UT
    sns_err_code_e                  err_code;
    sns_log_sensor_data_pkt_s       *log_pkt;
    uint16_t                        total_samples;
    uint16_t                        samples_logged_so_far;
    uint16_t                        remaining_samples;
    uint16_t                        max_samples_per_pkt;
    uint16_t                        samples_this_pkt;
    uint16_t                        sample_set_count;
    uint16_t                        i;
    uint16_t                        sample_ndx;
    log_pkt_t                       log_pkt_type;
    uint64_t                        adjusted_timestamp;
    uint64_t                        single_sample_set_time;


    if (!num_data_types_per_sample) {
        return;
    }

    if (data_collection->num_samples / num_data_types_per_sample < 1) {
        // don't have a complete sample set
        return;
    }

#if BMI160_CONFIG_RUN_ON_OSSC || BMI160_CONFIG_ENABLE_COMPILE_CHECK
    log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA_PUBLIC;
#else
    log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA;
#endif

    /* choose a maximum samples per packet that maximizes the number of samples
       without exceeding the size limit of the log packet and not breaking up
       sample sets.*/
    max_samples_per_pkt = ((SNS_DEBUG_MAX_LOG_SIZE_V01-(1+sizeof(sns_log_sensor_data_pkt_s)/sizeof(uint32_t)))/
            num_data_types_per_sample ) *
        num_data_types_per_sample;
    /* a "sample_set" is a collection of num_data_types_per_sample that all have
       the same timestamp */
    sample_set_count = (data_collection->num_samples / num_data_types_per_sample);

    /* since the log might be broken into multiple packets, will need
       to have adjusted start and end timestamps for each packet.
       The difference between end_timestamp and timestamp is the time
       required for sample_set_count-1 sample sets.
       */
    single_sample_set_time = (sample_set_count)== 1 ? 0 :
        (data_collection->end_timestamp - data_collection->timestamp ) / (sample_set_count-1);

    samples_logged_so_far = 0;
    remaining_samples = data_collection->num_samples;
    adjusted_timestamp = data_collection->timestamp;
    do
    {
        samples_this_pkt = remaining_samples < max_samples_per_pkt ? remaining_samples : max_samples_per_pkt;
        remaining_samples -= samples_this_pkt;
        /* since num_samples will be populated, we will always place samples in samples[] as well as data[] */
        err_code = sns_logpkt_malloc(log_pkt_type,
                sizeof(sns_log_sensor_data_pkt_s)
                + (samples_this_pkt - 1)* sizeof(log_pkt->samples[0]),
                (void**)&log_pkt);

        if ((SNS_SUCCESS == err_code) && log_pkt) {
            log_pkt->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
            log_pkt->sensor_id = data_collection->sensor;
            log_pkt->vendor_id = SNS_DDF_VENDOR_BOSCH;

            //Timestamp the log with sample time
            log_pkt->timestamp = adjusted_timestamp;

            //Log the sensor data
            log_pkt->num_data_types = num_data_types_per_sample;

            log_pkt->num_samples = samples_this_pkt / num_data_types_per_sample;

            // mark this as a multi-packet group as necessary. This could get overwritten if num_data_types_per_sample = SNS_LOG_MAX_DATA_TYPES_PER_SENSOR
            log_pkt->data[SNS_LOG_MAX_DATA_TYPES_PER_SENSOR-1] =
                (data_collection->num_samples == samples_this_pkt) ? 0  // not a multipacket group
                : remaining_samples > 0 ?  0xF0E1c0001       // multi-packet group, but this is not the last packet
                :  0xF0E1c0009;                             // last packet in multipacket group.

            // populate data[] with the 1st sample set
            for (i = 0; i < num_data_types_per_sample; i++) {
                if (i < SNS_LOG_MAX_DATA_TYPES_PER_SENSOR) {
                    log_pkt->data[i] = data_collection->samples[i+samples_logged_so_far].sample;
                }
            }

            // populate samples[] with ALL the sample sets.
            for (i = 0; i < samples_this_pkt; i++,samples_logged_so_far++) {
                log_pkt->samples[i] = data_collection->samples[samples_logged_so_far].sample;
            }


            /* recalculate adjusted time stamp rounding error */
            adjusted_timestamp = data_collection->timestamp + (samples_logged_so_far / num_data_types_per_sample) * single_sample_set_time;
            /* end timestamp is either the overall end time stamp (last packet) or the time stamp of the start of the next
               packet minus a single sample time. */
            log_pkt->end_timestamp = (remaining_samples==0)? data_collection->end_timestamp : adjusted_timestamp - single_sample_set_time;

            //commit log (also frees up the log packet memory)
            err_code = sns_logpkt_commit(log_pkt_type, log_pkt);

            if (SNS_SUCCESS != err_code) {
                DDF_MSG_1(ERROR, "[bmi160_log_data_pkt] - commit error: %d", err_code);
                break;
            }
        } else {
            if (SNS_SUCCESS != err_code) {
                DDF_MSG_1(ERROR, "[bmi160_log_data_pkt] - init error: %d", err_code);
            }
            break;
        }
    } while (remaining_samples && (SNS_SUCCESS == err_code));
#endif
}
#else
void sns_dd_bmi160_log_data_be(
        const sns_ddf_sensor_data_s     *data_collection,
        uint8_t                         num_data_types_per_sample)
{

#if BMI160_CONFIG_ENABLE_LOG_DATA_PKT && !BMI160_CONFIG_RUN_UT
    sns_err_code_e                  err_code;
    sns_log_sensor_data_pkt_s       *log_pkt;
    uint16_t                        num_samples_to_allocate;
    uint16_t                        i;
    log_pkt_t                       log_pkt_type;


    if (!num_data_types_per_sample) {
        return;
    }

    if (data_collection->num_samples / num_data_types_per_sample < 1) {
        return;
    }

    if (data_collection->num_samples / num_data_types_per_sample > 1) {
        num_samples_to_allocate = data_collection->num_samples - 1;
    } else {
        num_samples_to_allocate = 0;
    }

#if BMI160_CONFIG_RUN_ON_OSSC || BMI160_CONFIG_ENABLE_COMPILE_CHECK
    log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA_PUBLIC;
#else
    log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA;
#endif

    //Allocate log packet
    err_code = sns_logpkt_malloc(log_pkt_type,
            sizeof(sns_log_sensor_data_pkt_s)
            + num_samples_to_allocate * sizeof(log_pkt->samples[0]),
            (void**)&log_pkt);

    if ((SNS_SUCCESS == err_code) && log_pkt) {
        log_pkt->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
        log_pkt->sensor_id = data_collection->sensor;
        log_pkt->vendor_id = SNS_DDF_VENDOR_BOSCH;

        //Timestamp the log with sample time
        log_pkt->timestamp = data_collection->timestamp;
        log_pkt->end_timestamp = data_collection->end_timestamp;

        //Log the sensor data
        log_pkt->num_data_types = num_data_types_per_sample;
        //CHECK???
        log_pkt->num_samples = data_collection->num_samples / num_data_types_per_sample;

        for (i = 0; i < num_data_types_per_sample; i++) {
            if (i < SNS_LOG_MAX_DATA_TYPES_PER_SENSOR) {
                log_pkt->data[i] = data_collection->samples[i].sample;
            }
        }

        if (num_samples_to_allocate > 0) {
            for (i = 0; i < data_collection->num_samples; i++) {
                log_pkt->samples[i] = data_collection->samples[i].sample;
            }
        }

        //commit log (also frees up the log packet memory)
        err_code = sns_logpkt_commit(log_pkt_type, log_pkt);

        if (SNS_SUCCESS != err_code) {
            DDF_MSG_1(ERROR, "[bmi160_log_data_pkt] - commit error: %d", err_code);
        }
    } else {
        if (SNS_SUCCESS != err_code) {
            DDF_MSG_1(ERROR, "[bmi160_log_data_pkt] - init error: %d", err_code);
        }
    }
#endif
}
#endif

uint32_t g_ff_acc_cnt;
void sns_dd_bmi160_log_data(
        sns_dd_bmi160_state_t           *state,
        const sns_ddf_sensor_data_s     *data_collection,
        uint8_t                         num_data_types_per_sample,
        enum DD_DATA_RPT_CONTEXT        context,
        sns_ddf_time_t                  ts_curr)
{
    if (0 == num_data_types_per_sample) {
        return;
    }

    sns_dd_bmi160_log_data_be(data_collection, num_data_types_per_sample);

    BST_IGNORE_WARNING_UNUSED_VAR(context);


#if BMI160_CONFIG_ENABLE_LOG_DATA_L0
    sns_ddf_time_t end_timestamp;
    //note: if you have problem with using BMI160_MSG_5(), please only remove ts_curr from the log
    //e.g: BMI160_MSG_4(HIGH, "dl_meta<:0x%x,%d,%u,%u:>",
    //this is important for the tool to do analysis
    end_timestamp = data_collection->end_timestamp;

    BMI160_MSG_5(HIGH, "dl_meta<:0x%x,%d,%u,%u,%u:>",
            ((data_collection->sensor << 24) | (context << 16) | data_collection->status << 8),
            data_collection->num_samples,
            data_collection->timestamp,
            data_collection->end_timestamp,
            ts_curr);

    sns_ddf_time_t gap;
    uint32_t fc = 0;

#if BMI160_CONFIG_ENABLE_TS_ADJUST_FILTER
    if (SNS_DDF_SENSOR_ACCEL == data_collection->sensor) {
        end_timestamp += state->ts_adjust_acc;
    } else if (SNS_DDF_SENSOR_GYRO == data_collection->sensor) {
        end_timestamp += state->ts_adjust_gyr;
    }
#endif

    if (end_timestamp <= ts_curr) {
        sns_ddf_time_t gap_max;

        if (context >= DD_DATA_RPT_CONTEXT_FIFO_WMI) {
            fc =
                state->ff_cache_samples_a.fc +
                state->ff_cache_samples_g.fc;
#if BMI160_CONFIG_ENABLE_MAG_IF
            fc += state->ff_cache_samples_m.fc;
#endif
        } else {
            fc = (state->odr_acc_req > 0) + (state->odr_gyr_req > 0) + (state->odr_mag_req > 0);
        }

        gap = ts_curr - end_timestamp;
        //the gap is a result of delay due to time consumed by processing the data in the driver, and log could also result in some delay
        //the numbers 6 and 97 is statistical result,
        //for example on 8974, the delay typically is 700 for 100 frames
        //when you see the following WARNING in the log, it does not necessarily mean there is a problem,
        //we are being paranoid here to get the attention for any potential errors
        gap_max = ((fc * 6) + (97 + 66));
        if (gap > gap_max) {
            BMI160_MSG_3(HIGH, "NOTICE!!! gap too big<%u %u %u>",
                    end_timestamp, ts_curr, gap);
        }
    } else {
        gap = end_timestamp - ts_curr;
        BMI160_MSG_3(HIGH, "WARNING!!! end_timestamp exceeds ts_curr <%u %u %u>",
                end_timestamp, ts_curr, gap);
    }
#endif

#if BMI160_CONFIG_ENABLE_LOG_DATA_L1
    uint32_t i;
    sns_ddf_sensor_sample_s  *samples;
    BST_IGNORE_WARNING_UNUSED_VAR(samples);

    if (3 == num_data_types_per_sample) {
        for (i = 0; i < data_collection->num_samples / 3; i++) {
            samples = data_collection->samples + i * 3;
            BMI160_MSG_4(HIGH, "dl_data<:0x%x, %d,%d,%d:>",
                    ((data_collection->sensor << 24) | (context << 16)) | (i & 0xffff),
                    samples[0].sample,
                    samples[1].sample,
                    samples[2].sample);


#if 0
            if (BST_ABS(samples[2].sample) > FX_FLTTOFIX_Q16(G * 0.5)) {
                BMI160_MSG_4(ERROR,
                        "[data_log_acc] WARNING!!! cnt: %d @<%d %d %d> ",
                        g_ff_acc_cnt,
                        samples[0].sample,
                        samples[1].sample,
                        samples[2].sample);

                BMI160_MSG_2(ERROR, "ctx: %d i: %d", context, i);
            }

            g_ff_acc_cnt++;
#endif

        }

    } else if (1 == num_data_types_per_sample) {
        for (i = 0; i < data_collection->num_samples; i++) {
            samples = data_collection->samples + i;
            BMI160_MSG_3(HIGH, "dl_data<:0x%xd, %d, %d:>",
                    ((data_collection->sensor << 24) | (context << 16)) | (i & 0xffff),
                    samples[0].sample,
                    0);
        }
    }
#endif
}

#if BMI160_CONFIG_ENABLE_FIFO
//reviewing
void sns_dd_bmi160_fifo_fill_cache_acc(
        sns_dd_bmi160_state_t   *state,
        uint8_t                 *ff_buf)
{
    sns_ddf_sensor_sample_s  *samples;

    if (3 * state->ff_cache_samples_a.fc
            >= BST_ARRAY_SIZE(state->ff_cache_samples_a.samples)) {
        BMI160_MSG_1(ERROR,
                "[fifo_fill_cache] WARNING!!! the buffer for acc overruns: %d",
                state->ff_cache_samples_a.fc);
        return;
    }

    samples = state->ff_cache_samples_a.samples
        + 3 * state->ff_cache_samples_a.fc;


    sns_dd_bmi160_format_data_acc(state, ff_buf, samples);

#if BMI160_CONFIG_ENABLE_LOG_DATA_L3
    uint8_t i;

    BST_IGNORE_WARNING_UNUSED_VAR(i);
    for (i = 0; i < 6; i++) {
        BMI160_MSG_1(LOW, "[fill_cache_acc] 0x%02x", ff_buf[i]);
    }

    BMI160_MSG_4(MED, "fill_cache_acc <@idx: %d %d %d %d>",
            state->ff_cache_samples_a.fc,
            samples[0].sample,
            samples[1].sample,
            samples[2].sample);
#endif

    state->ff_cache_samples_a.fc += 1;
}

//reviewing
void sns_dd_bmi160_fifo_fill_cache_gyr(
        sns_dd_bmi160_state_t   *state,
        uint8_t                 *ff_buf)
{
    sns_ddf_sensor_sample_s  *samples;


    if (3 * state->ff_cache_samples_g.fc
            >= BST_ARRAY_SIZE(state->ff_cache_samples_g.samples)) {
        BMI160_MSG_1(ERROR,
                "[fifo_fill_cache] WARNING!!! the buffer for gyr overruns: %d",
                state->ff_cache_samples_g.fc);
        return;
    }

    samples = state->ff_cache_samples_g.samples
        + 3 * state->ff_cache_samples_g.fc;


    sns_dd_bmi160_format_data_gyr(state, ff_buf, samples);
#if BMI160_CONFIG_ENABLE_LOG_DATA_L3
    uint8_t i;

    BST_IGNORE_WARNING_UNUSED_VAR(i);

    for (i = 0; i < 6; i++) {
        BMI160_MSG_1(LOW, "[fill_cache_gyr] 0x%02x", ff_buf[i]);
    }

    BMI160_MSG_4(MED, "fill_cache_gyr <@idx: %d %d %d %d>",
            state->ff_cache_samples_g.fc,
            samples[0].sample,
            samples[1].sample,
            samples[2].sample);
#endif

    state->ff_cache_samples_g.fc += 1;
}

//reviewing
void sns_dd_bmi160_fifo_fill_cache_mag(
        sns_dd_bmi160_state_t   *state,
        uint8_t                 *ff_buf)
{
#if BMI160_CONFIG_ENABLE_MAG_IF
    sns_ddf_sensor_sample_s  *samples;


    if (state->ff_cache_samples_m.fc
            >= BST_ARRAY_SIZE(state->ff_cache_samples_m.samples)) {
        BMI160_MSG_1(ERROR,
                "[fifo_fill_cache] WARNING!!! the buffer for mag overruns: %d",
                state->ff_cache_samples_g.fc);
        return;
    }

    samples = state->ff_cache_samples_m.samples
        + 3 * state->ff_cache_samples_m.fc;

    state->ff_cache_samples_m.fc += 1;

    sns_dd_bmi160_format_data_mag(state, ff_buf, samples);
#endif
}

//reviewing
static void sns_dd_bmi160_fifo_update_sync_info(
        sns_dd_bmi160_state_t           *state,
        bmi160_fifo_parse_result_t      *ff_parse_rslt)
{
    if (ff_parse_rslt->bits.ff_avail_data_agm) {
        state->ff_sample_sync_info.fc_master += 1;
    }

    if (ff_parse_rslt->bits.ff_avail_data_agm & B0_SET) {
        state->ff_sample_sync_info.last_sync_marker_a.fc = state->ff_cache_samples_a.fc;
        state->ff_sample_sync_info.last_sync_marker_a.fc_master = state->ff_sample_sync_info.fc_master;
    }

    if (ff_parse_rslt->bits.ff_avail_data_agm & B1_SET) {
        state->ff_sample_sync_info.last_sync_marker_g.fc = state->ff_cache_samples_g.fc;
        state->ff_sample_sync_info.last_sync_marker_g.fc_master = state->ff_sample_sync_info.fc_master;
    }

#if BMI160_CONFIG_ENABLE_MAG_IF
    if (ff_parse_rslt->bits.ff_avail_data_agm & B2_SET) {
        state->ff_sample_sync_info.last_sync_marker_m.fc = state->ff_cache_samples_m.fc;
        state->ff_sample_sync_info.last_sync_marker_m.fc_master = state->ff_sample_sync_info.fc_master;
    }
#endif
}

static sns_ddf_status_e sns_dd_bmi160_fifo_parse_frames(
        sns_dd_bmi160_state_t           *state,
        uint8_t                         *ff_buf,
        uint32_t                        ff_buf_len,
        uint32_t                        *ff_proc_len_left,
        bmi160_fifo_parse_result_t      *ff_parse_rslt,
        uint32_t                        *ts_dev_ff)
{
    uint16_t                err_code = 0;

    uint8_t                 frame_header;
    uint8_t                 fh_mode;
    uint8_t                 fh_param;

    uint8_t                 frames_skipped;
    uint8_t                 frame_input_cfg;
    uint8_t                 frame_length;

    uint32_t                ff_buf_idx = 0;

#if BMI160_CONFIG_ENABLE_LOG_DATA_L3
    uint16_t i;

    for (i = 0; i < ff_buf_len; i++) {
        BMI160_MSG_2(LOW, "[parse_fifob] @%03d: 0x%02x",
                i, ff_buf[i]);

        if (0x80 == ff_buf[i]) {
            i++;
            BMI160_MSG_2(LOW, "[parse_fifob] %d bytes omitted next byte: 0x%02x",
                    ff_buf_len - i, ff_buf[i]);
            break;
        }
    }
#endif

    err_code = 0;
    ff_buf_idx = 0;

    while ((ff_buf_idx < ff_buf_len) &&  (!err_code)) {
        frame_header = ff_buf[ff_buf_idx];
        ff_buf_idx += 1;
        if (BMI160_FF_FH_EMPTY == frame_header) {
            // fifo empty
            BMI160_MSG_1(MED, "[parse_fifo]@%d frame_end",
                    ff_buf_idx - 1);
            ff_parse_rslt->bits.ff_avail_end_frame = 1;
            break;
        }

        fh_mode         = BMI160_FF_FH_MODE(frame_header);
        fh_param        = BMI160_FF_FH_PARAM(frame_header);

#if BMI160_CONFIG_ENABLE_LOG_DATA_L3
        BMI160_MSG_3(MED, "[parse_fifo] frame_header = 0x%02x fh_mode: 0x%02x fh_param: 0x%02x",
                frame_header, fh_mode, fh_param);
#endif

        if (BMI160_FF_FH_MODE_REGULAR == fh_mode) {
            frame_length = 0;
            frame_length += ((fh_param & B2_SET) ? BMI160_FF_DATA_LEN_MAG : 0);
            frame_length += ((fh_param & B1_SET) ? BMI160_FF_DATA_LEN_GYR : 0);
            frame_length += ((fh_param & B0_SET) ? BMI160_FF_DATA_LEN_ACC : 0);

            if (ff_buf_idx + frame_length > ff_buf_len) {
                // buf is over
                BMI160_MSG_3(MED,
                        "[parse_fifo] NOTICE!!! partial read of frame, %d bytes yet to be read, total: %d fh: 0x%x",
                        ff_buf_idx + frame_length - ff_buf_len,
                        ff_buf_len,
                        frame_header);

                err_code = (BMI160_FF_FRAME_ERR_PARTIAL_READ << 8) | frame_header;
                break;
            }

            if (fh_param & B2_SET) {
                // mag data
                sns_dd_bmi160_fifo_fill_cache_mag(state, ff_buf + ff_buf_idx);
#if BMI160_CONFIG_ENABLE_MAG_IF
                if (state->ff_cache_samples_m.fc >= BMI160_FF_CACHE_SIZE_ACC) {
                    ff_parse_rslt->bits.ff_cache_full_agm |= B2_SET;
                }
#endif

                ff_buf_idx += BMI160_FF_DATA_LEN_MAG;
                ff_parse_rslt->bits.ff_avail_data_agm |= B2_SET;
            }

            if (fh_param & B1_SET) {
                // gyr data
                sns_dd_bmi160_fifo_fill_cache_gyr(state, ff_buf + ff_buf_idx);
                if (state->ff_cache_samples_g.fc >= BMI160_FF_CACHE_SIZE_ACC) {
                    ff_parse_rslt->bits.ff_cache_full_agm |= B1_SET;
                }

                ff_buf_idx += BMI160_FF_DATA_LEN_GYR;
                ff_parse_rslt->bits.ff_avail_data_agm |= B1_SET;
            }

            if (fh_param & B0_SET) {
                // acc data
                sns_dd_bmi160_fifo_fill_cache_acc(state, ff_buf + ff_buf_idx);
                if (state->ff_cache_samples_a.fc >= BMI160_FF_CACHE_SIZE_ACC) {
                    ff_parse_rslt->bits.ff_cache_full_agm |= B0_SET;
                }

                ff_buf_idx += BMI160_FF_DATA_LEN_ACC;
                ff_parse_rslt->bits.ff_avail_data_agm |= B0_SET;
            }

            sns_dd_bmi160_fifo_update_sync_info(state, ff_parse_rslt);
            ff_parse_rslt->bits.ff_avail_data_agm = 0;

            if (unlikely(ff_parse_rslt->bits.ff_cache_full_agm)) {
                *ff_proc_len_left = ff_buf_len - ff_buf_idx;
                goto exit_parse_ff_frames;
            }

        } else if (BMI160_FF_FH_MODE_CTRL == fh_mode) {
            switch (fh_param) {
                case 0x00:
                    if (ff_buf_idx + 1 > ff_buf_len) {
                        err_code = (BMI160_FF_FRAME_ERR_PARTIAL_READ << 8)
                            | frame_header;
                        break;
                    }
                    //fifo overflowed
                    ff_parse_rslt->bits.ff_overflow = true;

                    frames_skipped = ff_buf[ff_buf_idx];
                    ff_buf_idx += 1;
                    BMI160_MSG_2(ERROR, "[parse_fifo]@%d frame_ctrl WARNING!!! frames_skipped = 0x%02x some samples are missing due to delay of FIFO read",
                            ff_buf_idx - 2,
                            frames_skipped);

                    BST_IGNORE_WARNING_UNUSED_VAR(frames_skipped);
                    break;
                case 0x01:
                    ff_parse_rslt->bits.ff_avail_ts_header = 1;

                    if (ff_buf_idx + 3 > ff_buf_len) {
                        BMI160_MSG_2(HIGH, "[parse_fifo]@%d frame_ctrl sensortime NOTICE!!! partial read, %d more bytes needed",
                                ff_buf_idx - 1, ff_buf_len + 3 - ff_buf_idx);

                        err_code = (BMI160_FF_FRAME_ERR_PARTIAL_READ << 8)
                            | frame_header;
                        break;
                    }

                    bmi160_parse_data_ts(ff_buf + ff_buf_idx, 3,
                            ts_dev_ff);

                    BMI160_MSG_3(MED,
                            "[parse_fifo]@%d frame_ctrl sensortime: 0x%x %u",
                            ff_buf_idx - 1,
                            *ts_dev_ff,
                            state->ff_sample_sync_info.fc_master);

                    ff_buf_idx += 3;
                    ff_parse_rslt->bits.ff_avail_ts = 1;
                    break;
                case 0x02:
                    // Fifo_Input_Config Frame
                    if (ff_buf_idx + 1 > ff_buf_len) {
                        err_code = (BMI160_FF_FRAME_ERR_PARTIAL_READ << 8)
                            | frame_header;
                        break;
                    }

                    frame_input_cfg = ff_buf[ff_buf_idx];
                    ff_buf_idx += 1;
                    BMI160_MSG_2(ERROR, "[parse_fifo]@%d frame_ctrl Fifo_Input_Config = 0x%02x",
                            ff_buf_idx - 2,
                            frame_input_cfg);
                    BST_IGNORE_WARNING_UNUSED_VAR(frame_input_cfg);
                    break;
                default:
                    BMI160_MSG_2(ERROR,
                            "[parse_fifo]%@d frame_unknown WARNING!!! frame format unknown frame_header: 0x%02x",
                            ff_buf_idx,
                            frame_header);
                    err_code = (BMI160_FF_FRAME_ERR_UNKNOWN << 8)
                        | frame_header;
                    break;
            }
        } else {
            BMI160_MSG_2(ERROR, "[parse_fifo]@%d frame_unknown WARNING!!! unknown fifo frame_header: 0x%02x",
                    ff_buf_idx - 1,
                    frame_header);
            err_code = (BMI160_FF_FRAME_ERR_UNKNOWN << 8)
                | frame_header;
            break;
        }
    }
    //end of while

    *ff_proc_len_left = 0;
exit_parse_ff_frames:
    if (err_code &&
            (((err_code >> 8) & 0xff) != BMI160_FF_FRAME_ERR_PARTIAL_READ)) {
        BMI160_MSG_3(ERROR, "[parse_fifo] WARNING!!! error seen during parsing: <err: 0x%04x %d %d>",
                err_code, ff_buf_idx, ff_buf_len);
    }

    return SNS_DDF_SUCCESS;
}

static void sns_dd_bmi160_fifo_populate_sample_ts(
        sns_dd_bmi160_state_t   *state,
        sns_ddf_sensor_e        sensor_type,
        uint16_t                fc)
{
    bmi160_fifo_sample_sync_info_t          *ff_sample_sync_info;
    sns_ddf_sensor_data_s                   *ff_cache;
    struct bmi160_fifo_sync_marker          *sync_marker;
    uint32_t                                fc_master_1st_frame;
    uint32_t                                fc_master_last_frame;
    uint32_t                                fc_curr = 0;
    bmi160_hw_mod_t                         *hw_mod;
    bmi160_hw_mod_t                         *hw_mod_ff_master;
    uint8_t                                 avail_last_ts;
    sns_ddf_time_t                          end_timestamp;
    sns_ddf_time_t                          ts_start_est_this = 0;
    sns_ddf_time_t                          ts_end_est_this = 0;
    int8_t                                  odr_times;
    sns_ddf_time_t                          ts_curr;
    sns_ddf_time_t                          *ts_last_sample = NULL;


    BST_IGNORE_WARNING_UNUSED_VAR(ts_curr);
    BST_IGNORE_WARNING_UNUSED_VAR(fc_curr);

    ts_curr = bmi160_get_sys_tick();

    ff_sample_sync_info = &state->ff_sample_sync_info;

    switch (sensor_type) {
        case SNS_DDF_SENSOR_ACCEL:
            ff_cache = &state->ff_cache_a;
            sync_marker = &ff_sample_sync_info->last_sync_marker_a;
            hw_mod = sns_dd_bmi160_get_hw_mod_cache_acc(state);
            ts_last_sample = &state->ts_last_sample_a;
            fc_curr = state->ff_cache_samples_a.fc;
            break;
        case SNS_DDF_SENSOR_GYRO:
            ff_cache = &state->ff_cache_g;
            sync_marker = &ff_sample_sync_info->last_sync_marker_g;
            hw_mod = sns_dd_bmi160_get_hw_mod_cache_gyr(state);
            ts_last_sample = &state->ts_last_sample_g;
            fc_curr = state->ff_cache_samples_g.fc;
            break;
#if BMI160_CONFIG_ENABLE_MAG_IF
        case SNS_DDF_SENSOR_MAG:
            ff_cache = &state->ff_cache_m;
            sync_marker = &ff_sample_sync_info->last_sync_marker_m;
            hw_mod = sns_dd_bmi160_get_hw_mod_cache_mag(state);
            ts_last_sample = &state->ts_last_sample_m;
            fc_curr = state->ff_cache_samples_m.fc;
            break;
#endif
        default:
            return;
    }

    hw_mod_ff_master = state->hw_mod_ff_master;

    if ((NULL == hw_mod) || (NULL == hw_mod_ff_master)) {
        BMI160_MSG_2(ERROR, "[populate_sample_ts] WARNING!!! NULL pointer <%p %p>",
                hw_mod, hw_mod_ff_master);
        return;
    }

    if (hw_mod_ff_master->odr_map_idx >= hw_mod->odr_map_idx) {
        odr_times = 1 << (hw_mod_ff_master->odr_map_idx - hw_mod->odr_map_idx);
    } else {
        BMI160_MSG_4(ERROR, "[populate_sample_ts] WARNING!!! odd situation: <%d,%d|%d,%d> this could cause wrong timestamps",
                hw_mod_ff_master->hw_mod_type,
                hw_mod_ff_master->odr_map_idx,
                hw_mod->hw_mod_type,
                hw_mod->odr_map_idx);

        odr_times = 1;
    }

    if (sync_marker->fc_master <= ff_sample_sync_info->fc_master) {
        //fc_master_1st_frame and fc_master_last_frame must be unsigned
        fc_master_1st_frame = sync_marker->fc_master -
            (sync_marker->fc - 1) * odr_times;
        fc_master_last_frame = sync_marker->fc_master +
            ((int32_t)fc - (int32_t)sync_marker->fc) * odr_times;
    } else {
        fc_master_1st_frame  = ff_sample_sync_info->fc_master;
        fc_master_last_frame = ff_sample_sync_info->fc_master;

        BMI160_MSG_4(ERROR,
                "[populate_sample_ts] WARNING!!! odd state detected, could cause wrong timestamp <fc: %d marker: %d,%d fc_master: %d>",
                fc,
                sync_marker->fc,
                sync_marker->fc_master,
                ff_sample_sync_info->fc_master);
    }

    ts_start_est_this = ff_sample_sync_info->ts_sys_last_sync_ref
        + ((int32_t)fc_master_1st_frame - (int32_t)ff_sample_sync_info->fc_master_last_sync_ref)
        * ff_sample_sync_info->ts_itvl_master_sample_est;
    avail_last_ts = ff_sample_sync_info->avail_last_ts;
    //CHECK: overflow
    if (BST_GET_VAL_BIT(avail_last_ts, hw_mod->hw_mod_type)) {
        ff_cache->timestamp = *ts_last_sample +
            odr_times * ff_sample_sync_info->ts_itvl_master_sample_est;
    } else {
        ff_sample_sync_info->avail_last_ts =
            BST_SET_VAL_BIT(avail_last_ts, hw_mod->hw_mod_type);


        ff_cache->timestamp = ts_start_est_this;

        if (0 == ff_cache->timestamp) {
            ff_cache->timestamp = 1;
            //this is to make sure the following if (ff_cache->timestamp <= *ts_last_sample) clause fail when *ts_last_sample is set to 0
        }


        if (ff_cache->timestamp <= *ts_last_sample) {
            //ASSUMPTION: sns_dd_bmi160_reset_subdev() will be called properly
            //in this case ts_last_sample_[agm] will be set to zero
            if (!sns_dd_bmi160_time_rewinded(*ts_last_sample, ff_cache->timestamp)) {
                BMI160_MSG_3(ERROR, "[populate_sample_ts] WARNING!!! time offset l.e >= t.s <%u %u %u>",
                        *ts_last_sample,
                        ff_cache->timestamp,
                        BMI160_SSC_US_2_TICKS(39));

                ff_cache->timestamp = *ts_last_sample + BMI160_SSC_US_2_TICKS(39);
            } else {
                BMI160_MSG_3(ERROR, "[populate_sample_ts] NOTICE!!! time rewind <ts_last: %u ts: %u ts_curr: %u>",
                        *ts_last_sample,
                        ff_cache->timestamp,
                        ts_curr);
            }
        }
    }

    //CHECK: overflow
    ts_end_est_this = ff_sample_sync_info->ts_sys_last_sync_ref
        + ((int32_t)fc_master_last_frame - (int32_t)ff_sample_sync_info->fc_master_last_sync_ref)
        * ff_sample_sync_info->ts_itvl_master_sample_est;

    end_timestamp = ts_end_est_this;
    if (sns_dd_bmi160_time_net(ff_cache->timestamp, end_timestamp)) {

        end_timestamp = ff_cache->timestamp +
            ((int32_t)fc_master_last_frame - (int32_t)fc_master_1st_frame) *
            ff_sample_sync_info->ts_itvl_master_sample_est;

        if (sns_dd_bmi160_time_net(end_timestamp, ts_curr)) {
            BMI160_MSG_3(ERROR, "[populate_sample_ts] WARNING!!! timing est error, possible error of last_ts<%u %u %u>",
                    *ts_last_sample, ff_cache->timestamp, ts_curr);
            ff_cache->timestamp = ts_start_est_this;
            ff_cache->end_timestamp = ts_end_est_this;
        }
        if (fc > 1) {
            BMI160_MSG_4(ERROR, "[populate_sample_ts] WARNING!!! time offset t.s > t.e <l.e:%u t.s:%u t.e:%u cor:%u>",
                    *ts_last_sample,
                    ff_cache->timestamp,
                    ts_end_est_this,
                    end_timestamp);
        }
    }

    if (fc > 1) {
        ff_cache->end_timestamp = end_timestamp;
    } else {
        //fc == 1

        ff_cache->end_timestamp = bmi160_get_time_sys_mid(ff_cache->timestamp,
                end_timestamp);
        ff_cache->timestamp = ff_cache->end_timestamp;
    }

    if (sns_dd_bmi160_time_net(ff_cache->end_timestamp, ts_curr)) {
        BMI160_MSG_2(HIGH, "WARNING!!! t.e corrected: %u to %u",
                ff_cache->end_timestamp, ts_end_est_this);
        ff_cache->end_timestamp = ts_end_est_this;
        if (1 == fc) {
            ff_cache->timestamp = ff_cache->end_timestamp;
        } else {
            if (!BST_GET_VAL_BIT(avail_last_ts, hw_mod->hw_mod_type)) {
                ff_cache->timestamp = ff_cache->end_timestamp -
                    (((int32_t)fc_master_last_frame - (int32_t)fc_master_1st_frame) *
                    ff_sample_sync_info->ts_itvl_master_sample_est);
            }
        }
    }

    *ts_last_sample = ff_cache->end_timestamp;

#if BMI160_CONFIG_ENABLE_LOG_DATA_L2
    BST_IGNORE_WARNING_UNUSED_VAR(fc_curr);

    BMI160_MSG_4(MED, "[populate_sample_ts] <sensor_info: 0x%x fc: %d marker (fc: %d fc_m: %d)>",
            (sensor_type << 8) | (BST_GET_VAL_BIT(avail_last_ts, hw_mod->hw_mod_type)),
            fc_curr,
            sync_marker->fc,
            sync_marker->fc_master);

    BMI160_MSG_4(MED, "[populate_sample_ts] <master_sync_ref (fc: %d ts_sys: %u) avail_master_ref: %d est_itvl: %u>",
            ff_sample_sync_info->fc_master_last_sync_ref,
            ff_sample_sync_info->ts_sys_last_sync_ref,
            ff_sample_sync_info->avail_master_ref,
            ff_sample_sync_info->ts_itvl_master_sample_est);
#endif
}

static sns_ddf_status_e sns_dd_bmi160_fifo_post_process(
        sns_dd_bmi160_state_t           *state,
        uint8_t                         in_irq,
        uint8_t                         trigger_now)
{
    uint16_t        idx;
    int32_t         size;
    uint16_t        frame_cnt_acc;
    uint16_t        frame_cnt_gyr;

    uint16_t        frame_cnt_acc_used;
    uint16_t        frame_cnt_gyr_used;
    sns_ddf_sensor_sample_s           *buf;

#if BMI160_CONFIG_ENABLE_MAG_IF
    uint16_t        frame_cnt_mag = 0;
    uint16_t        frame_cnt_mag_used;
    frame_cnt_mag = state->ff_cache_samples_m.fc;
#endif

    frame_cnt_acc = state->ff_cache_samples_a.fc;
    frame_cnt_gyr = state->ff_cache_samples_g.fc;

    BST_IGNORE_WARNING_UNUSED_VAR(buf);
    BST_IGNORE_WARNING_UNUSED_VAR(idx);
    BST_IGNORE_WARNING_UNUSED_VAR(size);

    if (in_irq || !trigger_now) {
        if (frame_cnt_acc >= state->ff_wml_req_acc) {
            if (state->ff_wml_req_acc > 0) {
                //see process_samples_ts()
#if BMI160_CONFIG_ENABLE_CACHE_EXTRA_FRAMES
                frame_cnt_acc_used = frame_cnt_acc / state->ff_wml_req_acc * state->ff_wml_req_acc;

                buf = state->ff_cache_samples_a.samples;
                idx = frame_cnt_acc_used * 3;
                size = ((int)frame_cnt_acc - (int)frame_cnt_acc_used)
                    * 3 * sizeof(sns_ddf_sensor_sample_s);

                if (size > 0) {
                    memscpy(buf, size, buf + idx, size);
                }
#else
                frame_cnt_acc_used = frame_cnt_acc;
#endif

                state->ff_cache_samples_a.fc -= frame_cnt_acc_used;

                state->ff_sample_sync_info.last_sync_marker_a.fc = state->ff_cache_samples_a.fc;
            }
        }

        if (frame_cnt_gyr >= state->ff_wml_req_gyr) {
            if (state->ff_wml_req_gyr > 0) {
#if BMI160_CONFIG_ENABLE_CACHE_EXTRA_FRAMES
                frame_cnt_gyr_used = frame_cnt_gyr / state->ff_wml_req_gyr * state->ff_wml_req_gyr;

                buf = state->ff_cache_samples_g.samples;
                idx = frame_cnt_gyr_used * 3;
                size = ((int)frame_cnt_gyr - (int)frame_cnt_gyr_used)
                    * 3 * sizeof(sns_ddf_sensor_sample_s);
                if (size > 0) {
                    memscpy(buf, size, buf + idx, size);
                }
#else
                frame_cnt_gyr_used = frame_cnt_gyr;
#endif

                state->ff_cache_samples_g.fc -= frame_cnt_gyr_used;

                state->ff_sample_sync_info.last_sync_marker_g.fc = state->ff_cache_samples_g.fc;
            }
        }
#if BMI160_CONFIG_ENABLE_MAG_IF
        if (frame_cnt_mag >= state->ff_wml_req_mag) {
#if BMI160_CONFIG_ENABLE_CACHE_EXTRA_FRAMES
            frame_cnt_mag_used = frame_cnt_mag / state->ff_wml_req_mag * state->ff_wml_req_mag;

            buf = state->ff_cache_samples_m.samples;
            idx = frame_cnt_mag_used * 3;
            size = ((int)frame_cnt_mag - (int)frame_cnt_mag_used)
                * 3 * sizeof(sns_ddf_sensor_sample_s);
            if (size > 0) {
                memscpy(buf, size, buf + idx, size);
            }
#else
            frame_cnt_mag_used = frame_cnt_mag;
#endif

            state->ff_cache_samples_m.fc -= frame_cnt_mag_used;
            state->ff_sample_sync_info.last_sync_marker_m.fc = state->ff_cache_samples_m.fc;
        }
#endif
    } else {
        state->ff_cache_samples_a.fc = 0;
        state->ff_cache_samples_g.fc = 0;
#if BMI160_CONFIG_ENABLE_MAG_IF
        state->ff_cache_samples_m.fc = 0;
#endif
    }

    return SNS_DDF_SUCCESS;
}

/*!description
 *
 */
static void sns_dd_bmi160_fifo_estimate_itvl_master(
        sns_dd_bmi160_state_t   *state,
        bmi160_time_info_t      *time_info)
{
    sns_ddf_time_t                  ts_itvl_master_sample_est = 0;
    sns_ddf_time_t                  ts_cor;
    uint32_t                        ts_dev_update_msb;

    struct ts_pair_sys_and_dev ts_sync_sys_and_dev_last = state->ts_sync_sys_and_dev;
    uint8_t lsb_bits = (BMI160_ODR_IDX_1600HZ - BMI160_ODR_IDX_NORMAL_START - state->hw_mod_ff_master->odr_map_idx) + 4;

    sns_dd_bmi160_get_couple_ts_host_and_dev(state);


#if BMI160_CONFIG_ENABLE_LOG_DATA_L2
    BMI160_MSG_4(MED, "[estimate_itvl_master] <s1: %u d1: 0x%x s2: %u d2: 0x%x>",
            ts_sync_sys_and_dev_last.ts_sys,
            ts_sync_sys_and_dev_last.ts_dev,
            state->ts_sync_sys_and_dev.ts_sys,
            state->ts_sync_sys_and_dev.ts_dev);
#endif

    if (state->ff_sample_sync_info.ready_for_itvl_est) {
        ts_itvl_master_sample_est = sns_dd_bmi160_get_time_elapse_sys(
                ts_sync_sys_and_dev_last.ts_sys,
                state->ts_sync_sys_and_dev.ts_sys) * (1 << lsb_bits) * 1.0 /
            bmi160_get_time_elapse_dev_lsb(ts_sync_sys_and_dev_last.ts_dev,
                    state->ts_sync_sys_and_dev.ts_dev);

        if (BST_TIME_ELAPSE_NEAR_ITVL(ts_itvl_master_sample_est,
                    state->ff_sample_sync_info.ts_itvl_master_sample_exp)) {

            if (state->ff_sample_sync_info.avail_1st_itvl_est) {
                state->ff_sample_sync_info.ts_itvl_master_sample_est =
                    (state->ff_sample_sync_info.ts_itvl_master_sample_est * BMI160_CONFIG_TS_ITVL_EST_COEF
                     + ts_itvl_master_sample_est * (10 - BMI160_CONFIG_TS_ITVL_EST_COEF)) / 10;
            } else {
                state->ff_sample_sync_info.avail_1st_itvl_est = 1;
                state->ff_sample_sync_info.ts_itvl_master_sample_est =
                    ts_itvl_master_sample_est;
            }
        }
    } else {
        state->ff_sample_sync_info.ready_for_itvl_est = 1;
    }

    sns_dd_bmi160_get_ts_cor(
            state->hw_mod_ff_master->odr_map_idx,
            state->ts_sync_sys_and_dev.ts_dev,
            state->ts_sync_sys_and_dev.ts_sys,
            &ts_cor);

    if (time_info->avail_ts_dev_ff) {
        state->ff_sample_sync_info.avail_master_ref = 1;

        ts_dev_update_msb = bmi160_get_ts_dev_update_msb(
                time_info->ts_dev_ff,
                state->ts_sync_sys_and_dev.ts_dev,
                lsb_bits);

        if (ts_dev_update_msb > 0) {
            ts_cor = sns_dd_bmi160_get_ts_backward(
                    ts_cor,
                    ts_dev_update_msb *
                    state->ff_sample_sync_info.ts_itvl_master_sample_est);
        }

        state->ff_sample_sync_info.ts_sys_last_sync_ref         = ts_cor;
        state->ff_sample_sync_info.fc_master_last_sync_ref      = state->ff_sample_sync_info.fc_master;
    } else {
        ts_dev_update_msb = bmi160_get_ts_dev_update_msb(
                time_info->ts_dev_reg,
                state->ts_sync_sys_and_dev.ts_dev,
                lsb_bits);

        if (0 == ts_dev_update_msb) {
            state->ff_sample_sync_info.avail_master_ref = 1;
            BMI160_MSG_0(HIGH, "!!! good luck: get time_ref even when no sensortime is get");

            state->ff_sample_sync_info.ts_sys_last_sync_ref         = ts_cor;
            state->ff_sample_sync_info.fc_master_last_sync_ref      = state->ff_sample_sync_info.fc_master;
        }
    }


    BMI160_MSG_4(MED, "[estimate_itvl_master] <lsb_bits: %d msb: %d est_itvl: %d sys_ref: %u>",
            lsb_bits, ts_dev_update_msb, ts_itvl_master_sample_est, ts_cor);
}

static sns_ddf_status_e sns_dd_bmi160_fifo_process_samples_ts(
        sns_dd_bmi160_state_t           *state,
        uint8_t                         in_irq,
        uint8_t                         trigger_now,
        bmi160_fifo_status_t            *ff_status)
{
    //TODO: when fifo overflowed, we shall not continue?
    uint16_t        frame_cnt_acc;
    uint16_t        frame_cnt_gyr;


    frame_cnt_acc = state->ff_cache_samples_a.fc;
    frame_cnt_gyr = state->ff_cache_samples_g.fc;

#if BMI160_CONFIG_ENABLE_MAG_IF
    uint16_t        frame_cnt_mag = 0;
    frame_cnt_mag = state->ff_cache_samples_m.fc;

#endif

    if ((in_irq) || (!trigger_now)) {
        if (frame_cnt_acc >= state->ff_wml_req_acc) {
            state->ff_frames_underrun_acc = 0;

            if (state->ff_wml_req_acc > 0) {
                //the number manipulation must be corelated to the code in post_process()
#if BMI160_CONFIG_ENABLE_CACHE_EXTRA_FRAMES
                frame_cnt_acc = frame_cnt_acc / state->ff_wml_req_acc * state->ff_wml_req_acc;
#endif
                state->ff_cache_a.num_samples = frame_cnt_acc * 3;
                ff_status->bits.ff_rdy_acc = true;
            } else {
                if (frame_cnt_acc > 0) {
                    BMI160_MSG_1(HIGH,
                            "NOTICE!!! get %d acc frames when WML is 0", frame_cnt_acc);
                    state->ff_cache_samples_a.fc = 0;
                }
            }
        } else {
            if (state->ff_wml_req_acc > BMI160_CONFIG_FF_UNDERRUN_CNT) {
                if ((state->ff_wml_req_acc - frame_cnt_acc) <= BMI160_CONFIG_FF_UNDERRUN_CNT) {
                    state->ff_frames_underrun_acc = state->ff_wml_req_acc - frame_cnt_acc;
                    BMI160_MSG_1(HIGH, "acc ff_ur:%d", state->ff_frames_underrun_acc);
                }
            }
        }

        if (frame_cnt_gyr >= state->ff_wml_req_gyr) {
            state->ff_frames_underrun_gyr = 0;
            if (state->ff_wml_req_gyr > 0) {
#if BMI160_CONFIG_ENABLE_CACHE_EXTRA_FRAMES
                frame_cnt_gyr = frame_cnt_gyr / state->ff_wml_req_gyr * state->ff_wml_req_gyr;
#endif
                state->ff_cache_g.num_samples = frame_cnt_gyr * 3;
                ff_status->bits.ff_rdy_gyr = true;
            } else {
                if (frame_cnt_gyr > 0) {
                    BMI160_MSG_1(HIGH,
                            "NOTICE!!! get %d gyr frames when WML is 0", frame_cnt_gyr);
                    state->ff_cache_samples_g.fc = 0;
                }
            }
        } else {
            if (state->ff_wml_req_gyr > BMI160_CONFIG_FF_UNDERRUN_CNT) {
                if ((state->ff_wml_req_gyr - frame_cnt_gyr) <= BMI160_CONFIG_FF_UNDERRUN_CNT) {
                    state->ff_frames_underrun_gyr = state->ff_wml_req_gyr - frame_cnt_gyr;
                    BMI160_MSG_1(HIGH, "gyr ff_ur:%d", state->ff_frames_underrun_gyr);
                }
            }
        }

#if BMI160_CONFIG_ENABLE_MAG_IF
        if (frame_cnt_mag >= state->ff_wml_req_mag) {
            state->ff_frames_underrun_mag = 0;
            if(state->ff_wml_req_mag > 0) {
#if BMI160_CONFIG_ENABLE_CACHE_EXTRA_FRAMES
                frame_cnt_mag = frame_cnt_mag / state->ff_wml_req_mag * state->ff_wml_req_mag;
#endif
                state->ff_cache_m.num_samples = frame_cnt_mag * 3;
                ff_status->bits.ff_rdy_mag = true;
            } else {
                if (frame_cnt_mag > 0) {
                    BMI160_MSG_1(HIGH,
                            "NOTICE!!! get %d mag frames when WML is 0", frame_cnt_mag);
                    state->ff_cache_samples_m.fc = 0;
                }
            }
        } else {
            if (state->ff_wml_req_mag > BMI160_CONFIG_FF_UNDERRUN_CNT) {
                if ((state->ff_wml_req_mag - frame_cnt_mag) <= BMI160_CONFIG_FF_UNDERRUN_CNT) {
                    state->ff_frames_underrun_mag = state->ff_wml_req_mag - frame_cnt_mag;
                    BMI160_MSG_1(HIGH, "mag ff_ur:%d", state->ff_frames_underrun_mag);
                }
            }
        }

#endif
    } else {
        //called from trigger_fifo_data()
        if (frame_cnt_acc > 0) {
            state->ff_cache_a.num_samples = frame_cnt_acc * 3;

            if (state->ff_wml_req_acc > 0) {
                ff_status->bits.ff_rdy_acc = true;
            } else {
                ff_status->bits.ff_rdy_acc = false;
                BMI160_MSG_1(HIGH, "NOTICE!!! %d acc frames when WML is 0", frame_cnt_acc);
            }
        }

        if (frame_cnt_gyr > 0) {
            state->ff_cache_g.num_samples = frame_cnt_gyr * 3;
            if (state->ff_wml_req_gyr > 0) {
                ff_status->bits.ff_rdy_gyr = true;
            } else {
                ff_status->bits.ff_rdy_gyr = false;
                BMI160_MSG_1(HIGH, "NOTICE!!! %d gyr frames when WML is 0", frame_cnt_gyr);
            }
        }

#if BMI160_CONFIG_ENABLE_MAG_IF
        if (frame_cnt_mag > 0) {
            state->ff_cache_m.num_samples = frame_cnt_mag * 3;
            if (state->ff_wml_req_mag > 0) {
                ff_status->bits.ff_rdy_mag = true;
            } else {
                ff_status->bits.ff_rdy_mag = false;
                BMI160_MSG_1(HIGH, "NOTICE!!! %d mag frames when WML is 0", frame_cnt_mag);
            }
        }
#endif
        state->ff_frames_underrun_acc = 0;
        state->ff_frames_underrun_gyr = 0;
        state->ff_frames_underrun_mag = 0;
    }

    if (ff_status->bits.ff_rdy_acc) {
        sns_dd_bmi160_fifo_populate_sample_ts(state, SNS_DDF_SENSOR_ACCEL, frame_cnt_acc);
    }

    if (ff_status->bits.ff_rdy_gyr) {
        sns_dd_bmi160_fifo_populate_sample_ts(state, SNS_DDF_SENSOR_GYRO, frame_cnt_gyr);
    }

#if BMI160_CONFIG_ENABLE_MAG_IF
    if (ff_status->bits.ff_rdy_mag) {
        sns_dd_bmi160_fifo_populate_sample_ts(state, SNS_DDF_SENSOR_MAG, frame_cnt_mag);
    }
#endif

    return SNS_DDF_SUCCESS;
}

static void sns_dd_bmi160_fifo_purge_cache(
        struct sns_dd_bmi160_ff_sample_cache    *ff_cache_samples)
{
    uint16_t        idx;
    int32_t         size;
    sns_ddf_sensor_sample_s         *buf;

    uint16_t        frame_cnt;

    frame_cnt = ff_cache_samples->fc;

    if (frame_cnt >= BMI160_FF_CACHE_SIZE_ACC) {
        buf = ff_cache_samples->samples;

        idx = BMI160_FF_CACHE_SIZE_PURGE * 3;
        size = ((int)frame_cnt - (int)BMI160_FF_CACHE_SIZE_PURGE)
            * 3 * sizeof(sns_ddf_sensor_sample_s);

        if (size > 0) {
            memscpy(buf, size, buf + idx, size);
        }

        ff_cache_samples->fc -= BMI160_FF_CACHE_SIZE_PURGE;

        BMI160_MSG_0(HIGH, "cache purged");
    }
}

static sns_ddf_status_e sns_dd_bmi160_fifo_process_n_report_buffer_samples(
        sns_dd_bmi160_state_t           *state,
        dd_fifo_read_ctx_t              *ctx)
{
    sns_ddf_status_e        status;

    bmi160_fifo_status_t    ff_status;
    sns_ddf_time_t          ts_data_report;
    bool in_irq = ctx->in_irq;

    bmi160_sub_dev_t *sub_dev_acc = sns_dd_bmi160_get_subdev_cache_acc(state);
    bmi160_sub_dev_t *sub_dev_gyr = sns_dd_bmi160_get_subdev_cache_gyr(state);
#if BMI160_CONFIG_ENABLE_MAG_IF
    bmi160_sub_dev_t *sub_dev_mag = sns_dd_bmi160_get_subdev_cache_mag(state);
#endif

    ff_status.stat = 0;
    status = sns_dd_bmi160_fifo_process_samples_ts(state,
            in_irq,
            ctx->trigger_now,
            &ff_status);
    BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

    ts_data_report = bmi160_get_sys_tick();

    if (ff_status.bits.ff_rdy_acc && (NULL != sub_dev_acc)) {
        if (in_irq) {
            status = sns_ddf_smgr_notify_event(sub_dev_acc->smgr_handle,
                    SNS_DDF_SENSOR_ACCEL,
                    SNS_DDF_EVENT_FIFO_WM_INT);
        }

        sns_dd_bmi160_adj_sample_sys_tick(state, &state->ff_cache_a);

        status = sns_ddf_smgr_notify_data(sub_dev_acc->smgr_handle,
                &state->ff_cache_a, 1);

        sns_dd_bmi160_log_data(state, &state->ff_cache_a, 3,
                ctx->data_rpt_ctx, ts_data_report);
    }

    if (ff_status.bits.ff_rdy_gyr && (NULL != sub_dev_gyr)) {
        if (in_irq) {
            status = sns_ddf_smgr_notify_event(sub_dev_gyr->smgr_handle,
                    SNS_DDF_SENSOR_GYRO,
                    SNS_DDF_EVENT_FIFO_WM_INT);
        }

        sns_dd_bmi160_adj_sample_sys_tick(state, &state->ff_cache_g);
        status = sns_ddf_smgr_notify_data(sub_dev_gyr->smgr_handle,
                &state->ff_cache_g, 1);

        sns_dd_bmi160_log_data(state, &state->ff_cache_g, 3,
                ctx->data_rpt_ctx, ts_data_report);
    }

#if BMI160_CONFIG_ENABLE_MAG_IF
    if (ff_status.bits.ff_rdy_mag && (NULL != sub_dev_mag)) {
        if (in_irq) {
            status = sns_ddf_smgr_notify_event(sub_dev_mag->smgr_handle,
                    SNS_DDF_SENSOR_MAG,
                    SNS_DDF_EVENT_FIFO_WM_INT);
        }

        sns_dd_bmi160_adj_sample_sys_tick(state, &state->ff_cache_m);
        status = sns_ddf_smgr_notify_data(sub_dev_mag->smgr_handle,
                &state->ff_cache_m, 1);

        sns_dd_bmi160_log_data(state, &state->ff_cache_m, 3,
                ctx->data_rpt_ctx, ts_data_report);
    }
    //sns_dd_bmi160_log_mag_data(state,
    //state->ff_cache_g.samples[0].timestamp);
#endif
    sns_dd_bmi160_fifo_post_process(state, in_irq, ctx->trigger_now);

    return SNS_DDF_SUCCESS;
}

static void sns_dd_bmi160_fifo_cor_ff_count(
        sns_dd_bmi160_state_t           *state,
        uint32_t                        *ff_count)
{
    bmi160_hw_mod_t *hw_mod_ff_master;
    uint32_t        time_sbus_xfer_est;
    uint32_t        aug = 0;
    uint32_t        tmp;

    hw_mod_ff_master = state->hw_mod_ff_master;
    if ((NULL == hw_mod_ff_master) || (0 == hw_mod_ff_master->odr_curr)) {
        BMI160_MSG_1(ERROR, "WARNING!!! odd state: <%p>", hw_mod_ff_master);
        return;
    }

    if (SNS_DDF_BUS_I2C == state->bus_type) {
        time_sbus_xfer_est = 25 * (*ff_count + 3);
    } else {
        time_sbus_xfer_est = (((*ff_count) << 3) + 2) * 1.0 / state->bus_clk_rate;
    }

    tmp = time_sbus_xfer_est * hw_mod_ff_master->odr_curr;
    aug = (tmp / BMS_SCALE_S2US);
    if ((tmp - aug * BMS_SCALE_S2US) > (BMS_SCALE_S2US >> 1)) {
        aug++;
    }

    if (aug > 0) {
        aug = aug * (1 +
                ((state->ff_wml_req_acc ? BMI160_FF_DATA_LEN_ACC : 0) +
                 (state->ff_wml_req_gyr ? BMI160_FF_DATA_LEN_GYR : 0) +
                 (state->ff_wml_req_mag ? BMI160_FF_DATA_LEN_MAG : 0)));

        BMI160_MSG_3(MED, "[cor_ff_count] odr: %d ff_count: %d aug: %d",
                hw_mod_ff_master->odr_curr, *ff_count, aug);
    }

    *ff_count += aug;
}

static sns_ddf_status_e sns_dd_bmi160_fifo_read_out_frames(
        sns_dd_bmi160_state_t           *state,
        uint16_t                        ff_count,
        dd_fifo_read_ctx_t              *ctx)
{
#if BMI160_CONFIG_ENABLE_FIFO
    sns_ddf_status_e        status;

    uint8_t                 *ff_buf = state->ff_cache_raw;
    uint8_t                 *ff_buf_part;

    uint32_t                ff_bytes_to_read = 0;
    uint32_t                ff_bytes_read = 0;
    uint32_t                ff_buf_proc_left = 0;

    uint32_t                fc_master_old = 0;
    int16_t                 i;

    bmi160_fifo_parse_result_t ff_parse_rslt = {.rslt = 0};
    bmi160_time_info_t      time_info = {0};
    uint32_t                ts_dev_ff = 0;
    bool                    read_boost = false;

    ff_bytes_to_read = ff_count;
    //this is to make sure we get the timestamp
    if (DD_DATA_RPT_CONTEXT_FIFO_FC_COMPENSATION != ctx->data_rpt_ctx) {
        if (!state->ff_sample_sync_info.avail_1st_itvl_est) {
            read_boost = true;
        } else {
            if (BST_TIME_SERIES_DIST(
                        state->ff_sample_sync_info.fc_master_last_sync_ref,
                        state->ff_sample_sync_info.fc_master)
                    >= BMI160_CONFIG_FIFO_READ_BOOT_WDT) {
                read_boost = true;
            }
        }

        if (read_boost) {
            ff_bytes_to_read += 4 * (
                    state->ff_wml_req_acc ? (BMI160_FF_DATA_LEN_ACC + 1) : 0 +
                    state->ff_wml_req_gyr ? (BMI160_FF_DATA_LEN_GYR + 1) : 0 +
                    state->ff_wml_req_mag ? (BMI160_FF_DATA_LEN_MAG + 1) : 0);

            BMI160_MSG_0(MED, "boost read size");
        }

        sns_dd_bmi160_fifo_cor_ff_count(state, &ff_bytes_to_read);
    }

    if (ctx->in_irq) {
        time_info.ts_dev_reg = state->reg_int_ctx.ts_hw;
    }

    i = 0;
    do {
        if (DD_DATA_RPT_CONTEXT_FIFO_FC_COMPENSATION != ctx->data_rpt_ctx) {
            ff_bytes_to_read += BMI160_FF_FRAME_LEN_TS + 4;
        }

        if (ff_bytes_to_read > BMI160_SSC_I2C_RD_LEN_MAX) {
            ff_bytes_to_read = BMI160_SSC_I2C_RD_LEN_MAX;
        }

        status = bmi160_read_fifo_bytes(state->port_handle,
                ff_buf, ff_bytes_to_read, &ff_bytes_read);

        BMI160_MSG_3(MED, "[read_out_fifo] ff_count: %d ff_bytes_to_read: %d @round: %d",
                ff_count, ff_bytes_read, i);

        BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

        ff_buf_proc_left = ff_bytes_read;
        ff_buf_part = ff_buf;
        while (ff_buf_proc_left > 0) {
            fc_master_old = state->ff_sample_sync_info.fc_master;

            ff_parse_rslt.rslt = 0;
            status = sns_dd_bmi160_fifo_parse_frames(state,
                    ff_buf_part,
                    ff_buf_proc_left,
                    &ff_buf_proc_left,
                    &ff_parse_rslt,
                    &ts_dev_ff);
            BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

            if (fc_master_old == state->ff_sample_sync_info.fc_master) {
                if (ctx->trigger_now) {
                    //we might already have some samples for some of the sensors
                    status = sns_dd_bmi160_fifo_process_n_report_buffer_samples(
                            state,
                            ctx);
                }

                if (!(ff_parse_rslt.bits.ff_cache_full_agm)) {
                    BMI160_MSG_0(MED, "no new frames found, get out");
                    break;
                }
            }

            if (ff_parse_rslt.bits.ff_avail_ts) {
                time_info.ts_dev_ff = ts_dev_ff;
                time_info.avail_ts_dev_ff = true;
            }


            if (ctx->in_irq || ff_parse_rslt.bits.ff_avail_ts) {
                if (ctx->in_irq || ctx->trigger_now) {
                    sns_dd_bmi160_fifo_estimate_itvl_master(
                            state,
                            &time_info);
                }
            }

            if (state->ff_sample_sync_info.avail_master_ref) {
                status = sns_dd_bmi160_fifo_process_n_report_buffer_samples(
                        state,
                        ctx);
            } else {
                if (!ff_parse_rslt.bits.ff_overflow) {
                } else {
                    BMI160_MSG_0(ERROR,
                            "WARNING!!! overflow detected");
                }

                if (ff_parse_rslt.bits.ff_cache_full_agm) {
                    BMI160_MSG_1(ERROR,
                            "WARNING!!! fifo cache full: 0x%x",
                            ff_parse_rslt.bits.ff_cache_full_agm);

                    sns_dd_bmi160_fifo_purge_cache(&state->ff_cache_samples_a);

                    sns_dd_bmi160_fifo_purge_cache(&state->ff_cache_samples_g);
#if BMI160_CONFIG_ENABLE_MAG_IF
                    sns_dd_bmi160_fifo_purge_cache(&state->ff_cache_samples_m);
#endif
                }
            }

            if (ff_buf_proc_left) {
                BMI160_MSG_1(MED, "%d bytes left for proc", ff_buf_proc_left);
                ff_buf_part = ff_buf + (ff_bytes_read - ff_buf_proc_left);
            }
        } //while (ff_buf_proc_left > 0)

        if ((ff_parse_rslt.bits.ff_avail_ts_header) || (ff_parse_rslt.bits.ff_avail_end_frame)) {
            break;
        }

        if (DD_DATA_RPT_CONTEXT_FIFO_FC_COMPENSATION == ctx->data_rpt_ctx) {
            break;
        }

        if (!ff_parse_rslt.bits.ff_avail_ts) {
            if (ff_parse_rslt.bits.ff_avail_end_frame) {
                BMI160_MSG_0(MED,
                        "[read_out_fifo] NOTICE!!! get EMPTY but not sensortime");
                break;
            }

            if (i > 2) {
                BMI160_MSG_0(ERROR, "[read_out_fifo] NOTICE!!! did not get sensortime from fifo, give up");
                break;
            }

            i++;
            BMI160_MSG_1(MED,
                    "[read_out_fifo] NOTICE!!! did not get sensortime from fifo, retry @round: %d",
                    i);

            if (!ff_parse_rslt.bits.ff_overflow) {
                ff_bytes_to_read = (BMI160_FF_DATA_LEN_MAG + BMI160_FF_DATA_LEN_GYR + BMI160_FF_DATA_LEN_ACC + 3) * i;
            } else {
                //assuming that I2C speed is at least 400KHZ
                ff_bytes_to_read = (BMI160_FF_DATA_LEN_ACC + 1) * 14;
            }

            continue;
        }
    } while (1);
#endif
    return SNS_DDF_SUCCESS;
}
#endif


sns_ddf_status_e sns_dd_bmi160_fifo_flush(
        sns_dd_bmi160_state_t           *state,
        uint16_t                        num_samples,
        uint32_t                        caller)
{
    sns_ddf_status_e status =  SNS_DDF_SUCCESS;
#if BMI160_CONFIG_ENABLE_FIFO
    uint16_t ff_count = 0;
    dd_fifo_read_ctx_t ctx;

    ctx.data_rpt_ctx = DD_DATA_RPT_CONTEXT_FIFO_FLUSH;
    ctx.in_irq = false;
    ctx.trigger_now = true;

    if (0 == num_samples) {
        if (!BMI160_CHIP_REV_IS_A2(state->chip_rev)) {
            status = bmi160_get_reg_fifo_count(state->port_handle, &ff_count);
            BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);
        } else {
            ff_count = state->ff_suggested_bytes2read;
        }
    } else {
        ff_count = num_samples;
    }

    BMI160_MSG_3(HIGH,
            "[bmi160_fifo_flush] num_samples: %d caller: %d ff_count: %d",
            num_samples, caller, ff_count);

    if (0 == ff_count) {
        ff_count = 5;
        //this may cause interpacket delays due to time spent draining an already empty FIFO, thus
        //lowering down the ff_count to 5
        //ff_count = fifo.state->ff_suggested_bytes2read;
    }

    status = sns_dd_bmi160_fifo_read_out_frames(state, ff_count, &ctx);
    BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);
    //sns_dd_bmi160_send_cmd(state, BMI160_REGV_CMD_FIFO_FLUSH);
#endif
    return status;
}

#if BMI160_CONFIG_ENABLE_INT_DOUBLE_TAP
//reviewed
static sns_ddf_tap_dir_e sns_dd_bmi160_get_tap_dir(uint8_t int_stat)
{
    sns_ddf_tap_dir_e ret;
    uint8_t dir_first_tap;
    uint8_t sign;
    sign = BST_GET_VAL_BITBLOCK(int_stat, 7, 7);
    dir_first_tap = bmi160_get_tap_first(int_stat);
    switch (dir_first_tap) {
        case BMI160_INT_INFO_TAP_FIRST_X:
            if (sign)
                ret = SNS_DDF_TAP_DIR_NEG_X;
            else
                ret = SNS_DDF_TAP_DIR_POS_X;
            break;
        case BMI160_INT_INFO_TAP_FIRST_Y:
            if (sign)
                ret = SNS_DDF_TAP_DIR_NEG_Y;
            else
                ret = SNS_DDF_TAP_DIR_POS_Y;
            break;
        case BMI160_INT_INFO_TAP_FIRST_Z:
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

static sns_ddf_status_e sns_dd_bmi160_handle_irq_dt(
        sns_dd_bmi160_state_t           *state,
        sns_ddf_time_t                  ts_irq)
{
    sns_ddf_status_e        status;
    bmi160_sub_dev_t        *sub_dev;
    sns_ddf_time_t          timestamp = ts_irq;
    q16_t                   sample_dt;
    sns_ddf_sensor_data_s   *data_dt;
    struct bmi160_reg_int_context *ctx = &state->reg_int_ctx;

    if (ctx->avail.bits.status_0 && (ctx->int_status_0 & BMI160_INT_STATUS_0_D_TAP_INT)) {

        sample_dt =
            sns_dd_bmi160_get_tap_dir(ctx->int_status_2);
        BMI160_MSG_2(HIGH,
                "[handle_irq_dt] DT interrupt detected @timestamp = %u int_status_2: 0x%x",
                ts_irq, ctx->int_status_2);

        bmi160_reset_int(state->port_handle);

        /*! Notify the SMGR of a doubletap event*/
        data_dt = &state->sensor_data_cache_dt;
        data_dt->timestamp = timestamp;
        data_dt->samples[0].sample = sample_dt;

        sub_dev = sns_dd_bmi160_get_subdev_from_type(state,
                SNS_DDF_SENSOR_DOUBLETAP);

        if ((sub_dev) && sub_dev->smgr_handle) {
            status = sns_ddf_smgr_notify_data(sub_dev->smgr_handle,
                    data_dt, 1);
            BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);
        }
    }

    return SNS_DDF_SUCCESS;
}
#endif

static sns_ddf_status_e sns_dd_bmi160_handle_irq_md(
        sns_dd_bmi160_state_t           *state,
        sns_ddf_time_t                  ts_irq)
{
    bmi160_sub_dev_t *sub_dev;
    struct bmi160_reg_int_context *ctx = &state->reg_int_ctx;

    if (ctx->avail.bits.status_0 && (ctx->int_status_0 & BMI160_INT_STATUS_0_ANYM_INT)) {
        BMI160_MSG_1(HIGH,
                "[handle_irq_md] MD interrupt detected @timestamp = %u",
                ts_irq);

        sub_dev = sns_dd_bmi160_get_subdev_from_type(state,
                SNS_DDF_SENSOR_ACCEL);
        /*! Notify manager of MOTION DETECT event */
        if (sub_dev) {
            sns_ddf_smgr_notify_event(sub_dev->smgr_handle,
                    SNS_DDF_SENSOR_ACCEL,
                    SNS_DDF_EVENT_MOTION);
            BMI160_MSG_0(HIGH, "event: SNS_DDF_EVENT_MOTION");
        }

        bmi160_reset_int(state->port_handle);
    }

    return SNS_DDF_SUCCESS;
}

static sns_ddf_status_e sns_dd_bmi160_handle_irq_drdy(
        sns_dd_bmi160_state_t           *state,
        sns_ddf_time_t                  ts_irq)
{
    sns_ddf_status_e status;
    sns_ddf_sensor_sample_s *samples;

    struct bmi160_sensor_data_raw_buf       sensor_data_buf = {{0}};
    union bmi160_sensor_data_req            sensor_data_req = {.req = 0};
    union bmi160_status_reg                 stat_reg = {.reg = 0};
    bmi160_sub_dev_t                        *sub_dev;
    uint32_t                                ts_dev_data;
    sns_ddf_time_t                          ts_cor;
    sns_ddf_time_t                          ts_data_report;

    uint8_t                                 ff_avail_last_ts;

    bmi160_parse_reg_status(state->reg_int_ctx.status, &stat_reg);

    BMI160_MSG_4(MED, "[handle_irq_drdy] <0x%02x %d %d %d>",
            stat_reg.reg,
            stat_reg.bits.drdy_acc,
            stat_reg.bits.drdy_gyr,
            stat_reg.bits.drdy_mag);


    ff_avail_last_ts = state->ff_sample_sync_info.avail_last_ts;
    if (state->int_en_flags.bits.drdy.bits.acc && stat_reg.bits.drdy_acc) {
#if 0
        sns_ddf_time_t elapse;
        sns_ddf_time_t itvl;
        bmi160_hw_mod_t *hw_mod;

        hw_mod = sns_dd_bmi160_get_hw_mod_cache_acc(state);
        elapse = (int32_t)ts_irq - (int32_t)state->sensor_data_cache_a.timestamp;
        if (hw_mod) {
            itvl = BMI160_SSC_US_2_TICKS(1e6 / state->odr_acc_req);

            BMI160_MSG_3(MED, "[handle_irq_drdy] <%u %u %d>",
                    (int32_t)state->sensor_data_cache_a.timestamp,
                    ts_irq, state->odr_acc_req);
            BMI160_MSG_3(MED, "[handle_irq_drdy] <elapse: %u itvl: %u (%u)>",
                    elapse, itvl,
                    BST_TIME_ELAPSE_NEAR_ITVL(elapse, itvl));
            BMI160_MSG_2(MED, "[handle_irq_drdy] <%u %u>)",
                    BST_ABS(elapse - itvl), (itvl >> 5));


            if ((elapse > itvl) || BST_TIME_ELAPSE_NEAR_ITVL(elapse, itvl) || (elapse < 0)) {
                sensor_data_req.bits.a = true;
            }
        }
#else
        sensor_data_req.bits.a = true;
#endif
    }

    if (state->int_en_flags.bits.drdy.bits.gyr && stat_reg.bits.drdy_gyr) {
        sensor_data_req.bits.g = true;
    }

#if BMI160_CONFIG_ENABLE_MAG_IF
    if (state->int_en_flags.bits.drdy.bits.mag && stat_reg.bits.drdy_mag) {
        sensor_data_req.bits.m = true;
    }
#endif

    if (sensor_data_req.req) {
        status = bmi160_read_data(state->port_handle, &sensor_data_buf, &sensor_data_req);
    }

    bmi160_parse_data_ts(sensor_data_buf.buf_ts, 3, &ts_dev_data);

    if (state->hw_mod_odr_max) {
        sns_dd_bmi160_get_ts_cor(
                state->hw_mod_odr_max->odr_map_idx,
                state->reg_int_ctx.ts_hw,
                state->reg_int_ctx.ts_sys,
                &ts_cor);
        //LKTD: to read out ts together with data to check for stale data
    } else {
        BMI160_MSG_0(ERROR,
                "[handle_irq_drdy] WARNING! hw_mod not updated properly");
        ts_cor = ts_irq;
    }


    BMI160_MSG_3(MED,
            "[handle_irq_drdy] <ts_ctx_sys: %u  ts_ctx_dev: 0x%x ts_cor: %u>",
            state->reg_int_ctx.ts_sys, state->reg_int_ctx.ts_hw, ts_cor);

    ts_data_report = bmi160_get_sys_tick();

    if (sensor_data_req.bits.a) {
        sub_dev = sns_dd_bmi160_get_subdev_cache_acc(state);

        if (NULL != sub_dev) {
            samples = state->sensor_data_cache_a.samples;

            sns_dd_bmi160_format_data_acc(state, sensor_data_buf.buf_a, samples);

            if (state->avail_last_ts_dri_acc ||
                    BST_GET_VAL_BIT(ff_avail_last_ts, BMI160_HW_MOD_TYPE_ACC)) {
                if (sns_dd_bmi160_time_net(state->ts_last_sample_a, ts_cor)) {
                    BMI160_MSG_2(ERROR, "WARNING!!! l.e >= t.s <%u %u>",
                            state->ts_last_sample_a, ts_cor);
                    ts_cor = state->ts_last_sample_a + BMI160_SSC_US_2_TICKS(39);
                }
            } else {
                state->avail_last_ts_dri_acc = 1;
            }

            state->sensor_data_cache_a.timestamp = ts_cor;
            state->sensor_data_cache_a.end_timestamp = ts_cor;

            state->ts_last_sample_a = ts_cor;

            sns_dd_bmi160_adj_sample_sys_tick(state, &state->sensor_data_cache_a);
            sns_ddf_smgr_notify_data(sub_dev->smgr_handle, &state->sensor_data_cache_a, 1);

            sns_dd_bmi160_log_data(state, &state->sensor_data_cache_a, 3, DD_DATA_RPT_CONTEXT_DRI, ts_data_report);
        }
    }

    if (sensor_data_req.bits.g) {
        sub_dev = sns_dd_bmi160_get_subdev_cache_gyr(state);
        if (NULL != sub_dev) {
            samples = state->sensor_data_cache_g.samples;

            sns_dd_bmi160_format_data_gyr(state, sensor_data_buf.buf_g, samples);

            if (state->avail_last_ts_dri_gyr ||
                    BST_GET_VAL_BIT(ff_avail_last_ts, BMI160_HW_MOD_TYPE_GYR)) {
                if (sns_dd_bmi160_time_net(state->ts_last_sample_g, ts_cor)) {
                    BMI160_MSG_2(ERROR, "WARNING!!! l.e >= t.s <%u %u>",
                            state->ts_last_sample_g, ts_cor);
                    ts_cor = state->ts_last_sample_g + BMI160_SSC_US_2_TICKS(39);
                }
            } else {
                state->avail_last_ts_dri_gyr = 1;
            }
            state->sensor_data_cache_g.timestamp = ts_cor;
            state->sensor_data_cache_g.end_timestamp = ts_cor;

            state->ts_last_sample_g = ts_cor;

            sns_dd_bmi160_adj_sample_sys_tick(state, &state->sensor_data_cache_g);
            sns_ddf_smgr_notify_data(sub_dev->smgr_handle, &state->sensor_data_cache_g, 1);

            sns_dd_bmi160_log_data(state, &state->sensor_data_cache_g, 3, DD_DATA_RPT_CONTEXT_DRI, ts_data_report);
        }
    }

#if BMI160_CONFIG_ENABLE_MAG_IF
    if (sensor_data_req.bits.m) {
        sub_dev = sns_dd_bmi160_get_subdev_cache_mag(state);
        if (NULL != sub_dev) {
            samples = state->sensor_data_cache_m.samples;

            sns_dd_bmi160_format_data_mag(state, sensor_data_buf.buf_m, samples);

            if (state->avail_last_ts_dri_mag ||
                    BST_GET_VAL_BIT(ff_avail_last_ts, BMI160_HW_MOD_TYPE_MAG)) {
                if (sns_dd_bmi160_time_net(state->ts_last_sample_m, ts_cor)) {
                    BMI160_MSG_2(ERROR, "WARNING!!! l.e >= t.s <%u %u>",
                            state->ts_last_sample_m, ts_cor);
                    ts_cor = state->ts_last_sample_m + BMI160_SSC_US_2_TICKS(39);
                }
            } else {
                state->avail_last_ts_dri_mag = 1;
            }

            state->sensor_data_cache_m.timestamp = ts_cor;
            state->sensor_data_cache_m.end_timestamp = ts_cor;

            state->ts_last_sample_m = ts_cor;

            sns_dd_bmi160_adj_sample_sys_tick(state, &state->sensor_data_cache_m);
            sns_ddf_smgr_notify_data(sub_dev->smgr_handle, &state->sensor_data_cache_m, 1);

            sns_dd_bmi160_log_data(state, &state->sensor_data_cache_m, 3, DD_DATA_RPT_CONTEXT_DRI, ts_data_report);
        }
    }
#endif
    BST_IGNORE_WARNING_UNUSED_VAR(status);

    return SNS_DDF_SUCCESS;
}

static sns_ddf_status_e sns_dd_bmi160_handle_irq_ff_full(
        sns_dd_bmi160_state_t           *state,
        sns_ddf_time_t                  ts_irq)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    dd_fifo_read_ctx_t ctx;

    bmi160_sub_dev_t *sub_dev_acc = sns_dd_bmi160_get_subdev_cache_acc(state);
    bmi160_sub_dev_t *sub_dev_gyr = sns_dd_bmi160_get_subdev_cache_gyr(state);
#if BMI160_CONFIG_ENABLE_MAG_IF
    bmi160_sub_dev_t *sub_dev_mag = sns_dd_bmi160_get_subdev_cache_mag(state);
#endif

    BMI160_MSG_3(HIGH, "[handle_irq_ff_full] fc_curr <%d %d %d>",
            state->ff_cache_samples_a.fc,
            state->ff_cache_samples_g.fc,
#if BMI160_CONFIG_ENABLE_MAG_IF
            state->ff_cache_samples_m.fc
#else
            0
#endif
            );
    ctx.data_rpt_ctx = DD_DATA_RPT_CONTEXT_FIFO_FFI;
    ctx.in_irq = false;
    ctx.trigger_now = true;
    status = sns_dd_bmi160_fifo_process_n_report_buffer_samples(state,
            &ctx);
    if (status != SNS_DDF_SUCCESS)
    {
        // Do nothing. This is to eliminate conpiler warning.
    }

    //<reset_some_fifo_internal_states>
    state->ff_sample_sync_info.avail_last_ts               = 0;
    state->ff_sample_sync_info.avail_master_ref             = 0;
    state->ff_sample_sync_info.fc_master                    = 0;
    state->ff_sample_sync_info.fc_master_last_sync_ref      = 0;
    state->ff_sample_sync_info.ts_sys_last_sync_ref         = 0;
    //</reset_some_fifo_internal_states>

    //SMGR will call trigger_fifo_data() for each SNS_DDF_EVENT_FIFO_OVERFLOW
    if ((state->ff_wml_req_acc > 0) && (NULL != sub_dev_acc)) {
        BMI160_MSG_0(ERROR, "WARNING!!! event: SNS_DDF_EVENT_FIFO_OVERFLOW acc");
        status = sns_ddf_smgr_notify_event(sub_dev_acc->smgr_handle,
                SNS_DDF_SENSOR_ACCEL,
                SNS_DDF_EVENT_FIFO_OVERFLOW);
    }

    if ((state->ff_wml_req_gyr > 0) && (NULL != sub_dev_gyr)) {
        BMI160_MSG_0(ERROR, "WARNING!!! event: SNS_DDF_EVENT_FIFO_OVERFLOW gyr");
        status = sns_ddf_smgr_notify_event(sub_dev_gyr->smgr_handle,
                SNS_DDF_SENSOR_GYRO,
                SNS_DDF_EVENT_FIFO_OVERFLOW);
    }

#if BMI160_CONFIG_ENABLE_MAG_IF
    if ((state->ff_wml_req_mag > 0) && (NULL != sub_dev_mag)) {
        BMI160_MSG_0(ERROR, "WARNING!!! event: SNS_DDF_EVENT_FIFO_OVERFLOW mag");
        status = sns_ddf_smgr_notify_event(sub_dev_mag->smgr_handle,
                SNS_DDF_SENSOR_MAG,
                SNS_DDF_EVENT_FIFO_OVERFLOW);
    }
#endif

    return SNS_DDF_SUCCESS;
}

static sns_ddf_status_e sns_dd_bmi160_handle_irq_ff_wml(
        sns_dd_bmi160_state_t           *state,
        sns_ddf_time_t                  ts_irq)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    uint8_t         regv;
    uint16_t        ff_count = 0;

    bool            enable_dri = false;
    dd_fifo_read_ctx_t ctx;

    if (!BMI160_CHIP_REV_IS_A2(state->chip_rev)) {
        status = bmi160_get_reg_fifo_count(state->port_handle, &ff_count);
        BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);
    } else {
        ff_count = state->ff_suggested_bytes2read;
    }

    state->ts_ff_irq = ts_irq;

    if (state->ff_wmi_missing_possible) {
        regv = state->regvc_int_map_1;
        regv = BST_CLR_VAL_BIT(regv, 6);

        state->sbus_mon_single_byte_rw_off = 1;
        status = bmi160_sbus_write_wrapper(state->port_handle,
                BMI160_REGA_USR_INT_MAP_1,
                &regv, 1);
        state->sbus_mon_single_byte_rw_off = 0;
    }

    if (0 == ff_count) {
        if (state->ff_suggested_bytes2read > 0) {
            ff_count = state->ff_suggested_bytes2read;
        } else {
            ff_count = 1 + BMI160_FF_DATA_LEN_ACC + BMI160_FF_DATA_LEN_GYR + BMI160_FF_DATA_LEN_MAG + BMI160_FF_FRAME_LEN_TS + 1;
        }

        BMI160_MSG_2(ERROR, "WARNING!!! [handle_irq_ff_wml] ff_count corrected from 0 to : %d suggested: %d",
                ff_count, state->ff_suggested_bytes2read);
    }

    if (ff_count > 0) {
        ctx.data_rpt_ctx = DD_DATA_RPT_CONTEXT_FIFO_WMI;
        ctx.in_irq = true;
        ctx.trigger_now = false;

        status = sns_dd_bmi160_fifo_read_out_frames(state,
                ff_count, &ctx);

#if BMI160_CONFIG_ENABLE_WA_FF_FRAMES_UNDERRUN
        uint16_t ff_frames_underrun;

        ff_frames_underrun =
            state->ff_frames_underrun_acc
            | state->ff_frames_underrun_gyr
            | state->ff_frames_underrun_mag;

        if (ff_frames_underrun) {
            ff_count = 1;
            ff_count += state->ff_wml_req_acc ? 6 : 0;
            ff_count += state->ff_wml_req_gyr ? 6 : 0;
            ff_count += state->ff_wml_req_mag ? 8 : 0;

            ctx.data_rpt_ctx = DD_DATA_RPT_CONTEXT_FIFO_FC_COMPENSATION;
            ctx.in_irq = false;
            ctx.trigger_now = false;
            status = sns_dd_bmi160_fifo_read_out_frames(state,
                    ff_count, &ctx);

            ff_frames_underrun =
                state->ff_frames_underrun_acc
                | state->ff_frames_underrun_gyr
                | state->ff_frames_underrun_mag;
            if (ff_frames_underrun) {
                uint8_t dummy[2];

                BMI160_MSG_0(HIGH, "ff_ur after reread");

                //hybrid INT
                regv = state->regvc_int_map_1;
                regv = BST_SET_VAL_BIT(regv, 7);
                bmi160_sbus_write_wrapper(state->port_handle, BMI160_REGA_USR_INT_MAP_1, &regv, 1);

                //this is to clear DRDY status in 0x1b
                if (state->ff_frames_underrun_acc | state->ff_frames_underrun_gyr) {
                    bmi160_sbus_read_wrapper(state->port_handle, BMI160_REGA_USR_DATA_13, dummy, 2);
                }

                if (state->ff_frames_underrun_mag) {
                    bmi160_sbus_read_wrapper(state->port_handle, BMI160_REGA_USR_DATA_7, dummy, 2);
                }

                enable_dri = true;
            }
        }
#endif

    }


    if (state->ff_wmi_missing_possible) {
        regv = state->regvc_int_map_1;

        if (enable_dri) {
            regv = BST_SET_VAL_BIT(regv, 7);
        }

        state->sbus_mon_single_byte_rw_off = 1;
        status = bmi160_sbus_write_wrapper(state->port_handle,
                BMI160_REGA_USR_INT_MAP_1,
                &regv, 1);
        state->sbus_mon_single_byte_rw_off = 0;
    }

    return status;
}

#if BMI160_CONFIG_ENABLE_WA_FF_FRAMES_UNDERRUN
    static
sns_ddf_status_e sns_dd_bmi160_handle_irq_ff_wml_partial(
        sns_dd_bmi160_state_t           *state,
        sns_ddf_time_t                  ts_irq)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    uint16_t        ff_count = 0;
    uint16_t        ff_frames_underrun;


    dd_fifo_read_ctx_t ctx;
    ctx.data_rpt_ctx = DD_DATA_RPT_CONTEXT_FIFO_FC_COMPENSATION;
    ctx.in_irq = false;
    ctx.trigger_now = false;

    ff_frames_underrun =
        state->ff_frames_underrun_acc
        | state->ff_frames_underrun_gyr
        | state->ff_frames_underrun_mag;

    if (!ff_frames_underrun) {
        return SNS_DDF_SUCCESS;
    }

    ff_count = 1;

    if (BST_GET_VAL_BIT(state->reg_int_ctx.status, 7) && state->ff_wml_req_acc) {
        ff_count += 6;
    }

    if (BST_GET_VAL_BIT(state->reg_int_ctx.status, 6) && state->ff_wml_req_gyr) {
        ff_count += 6;
    }

    if (BST_GET_VAL_BIT(state->reg_int_ctx.status, 5) && state->ff_wml_req_mag) {
        ff_count += 8;
    }

    status = sns_dd_bmi160_fifo_read_out_frames(state,
            ff_count, &ctx);

    BMI160_MSG_2(HIGH, "read_ff_part: %u status: 0x%x",
            ff_count, state->reg_int_ctx.status);

    ff_frames_underrun =
        state->ff_frames_underrun_acc
        | state->ff_frames_underrun_gyr
        | state->ff_frames_underrun_mag;

    if (!ff_frames_underrun) {
        uint8_t regv;

        regv = state->regvc_int_map_1;

        if (!BST_GET_VAL_BIT(regv, 7)) {
            BMI160_MSG_0(HIGH, "restore int_map_1");
            status = bmi160_sbus_write_wrapper(state->port_handle, BMI160_REGA_USR_INT_MAP_1, &regv, 1);
        }
    }

    return status;
}
#endif

static sns_ddf_status_e sns_dd_bmi160_handle_irq_attach_temp(
        sns_dd_bmi160_state_t           *state,
        sns_ddf_time_t                  ts_irq)
{
    sns_ddf_status_e status;
    bmi160_sub_dev_t *sub_dev       = NULL;

    sns_ddf_time_t elapse;
    sns_ddf_time_t itvl_1hz         = BMI160_SSC_US_2_TICKS(1000000);
    sns_ddf_time_t ts_data_report;
    uint8_t        si_buf[2]        = "";
    bool           time_due;

    if ((0 == state->odr_tem_req_acc)
            && (0 == state->odr_tem_req_gyr)
            && (0 == state->odr_tem_req_mag)) {

#if BMI160_CONFIG_ENABLE_ACC_TCOC
        if (0 == state->odr_acc_req) {
            return SNS_DDF_SUCCESS;
        }
#else
        return SNS_DDF_SUCCESS;
#endif
    }

    elapse = sns_dd_bmi160_get_time_elapse_sys(
            state->ts_last_sample_t, ts_irq);

    time_due = false;
    if (elapse >= itvl_1hz) {
        time_due = true;
    } else {
        if (((itvl_1hz - elapse) * 200) <= itvl_1hz) {
            time_due = true;
        }
    }

    if (!time_due) {
        return SNS_DDF_SUCCESS;
    }

    status = bmi160_sbus_read_wrapper(state->port_handle,
            BMI160_REGA_USR_TEMPERATURE_0,
            si_buf, 2);
    BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

    sns_dd_bmi160_format_data_temp(state, si_buf,
            &state->cache_temperature);
    state->sensor_data_cache_t.samples[0].sample = FX_FLTTOFIX_Q16(state->cache_temperature);

    state->sensor_data_cache_t.timestamp = ts_irq;
    state->sensor_data_cache_t.end_timestamp = ts_irq;

    ts_data_report = bmi160_get_sys_tick();

    if (0 != state->odr_tem_req_acc) {
        sub_dev = sns_dd_bmi160_get_subdev_cache_acc(state);
        if (NULL != sub_dev) {
            BMI160_MSG_2(HIGH, "attach temp data: <%d %d>",
                    state->sensor_data_cache_t.status,
                    state->sensor_data_cache_t.sensor);
            sns_ddf_smgr_notify_data(sub_dev->smgr_handle, &state->sensor_data_cache_t, 1);
        }
    }

    if (0 != state->odr_tem_req_gyr) {
        sub_dev = sns_dd_bmi160_get_subdev_cache_gyr(state);
        if (NULL != sub_dev) {
            sns_ddf_smgr_notify_data(sub_dev->smgr_handle, &state->sensor_data_cache_t, 1);
        }
    }

#if BMI160_CONFIG_ENABLE_MAG_IF
    if (0 != state->odr_tem_req_mag) {
        sub_dev = sns_dd_bmi160_get_subdev_cache_mag(state);
        if (NULL != sub_dev) {
            sns_dd_bmi160_adj_sample_sys_tick(state, &state->sensor_data_cache_t);
            sns_ddf_smgr_notify_data(sub_dev->smgr_handle, &state->sensor_data_cache_t, 1);
        }
    }
#endif

    state->ts_last_sample_t = ts_irq;

    sns_dd_bmi160_log_data(state, &state->sensor_data_cache_t, 1, DD_DATA_RPT_CONTEXT_FIFO_WMI_IMITATED, ts_data_report);

    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_bmi160_start_timer_sbus_susp(
        sns_dd_bmi160_state_t *state)
{
    sns_ddf_status_e status;

    BMI160_MSG_2(MED, "timer sbus: %d %d",
            state->sbus_in_normal_mode,
            state->timer_pending_sbus_susp_mode);

    if (!state->sbus_in_normal_mode) {
        if (!state->timer_pending_sbus_susp_mode) {
            status = sns_ddf_timer_start(state->timer_sbus_susp_mode.timer_obj,
                    BMI160_SPEC_STARTUP_TIME_SBUS_MS * 1000);
            BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);

            state->timer_pending_sbus_susp_mode = 1;

            BMI160_MSG_1(MED, "timer sbus started at: %u",
                    bmi160_get_sys_tick());
        }
    }

    return SNS_DDF_SUCCESS;
}

int32_t sns_dd_bmi160_handle_timer_sbus_susp(void *private_data)
{
    sns_dd_bmi160_state_t *state = (sns_dd_bmi160_state_t *)private_data;

    state->sbus_in_normal_mode = 1;
    state->timer_pending_sbus_susp_mode = 0;

    BMI160_MSG_1(HIGH, "timer sbus expired at: %u", bmi160_get_sys_tick());

    return 0;
}

#if DEBUG
static void cd1__________________________________________________dummy()
{
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
//reviewed
static sns_ddf_status_e sns_dd_bmi160_get_data(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s    *memhandler,
        sns_ddf_sensor_data_s   *data[])
{
    uint8_t i;
    sns_ddf_status_e        status;
    sns_ddf_sensor_data_s   *data_ptr;
    sns_dd_bmi160_state_t   *state = sns_dd_bmi160_get_dev(dd_handle);
    bmi160_sub_dev_t        *sub_dev;
    sns_ddf_time_t          ts;

    struct bmi160_sensor_data_raw_buf       sensor_data_buf = {{0}};
    union bmi160_sensor_data_req            sensor_data_req = {.req = 0};

    BST_IGNORE_WARNING_UNUSED_VAR(sub_dev);

    /* Sanity check*/
    for (i = 0; i < num_sensors; i++) {
        switch (sensors[i]) {
            case SNS_DDF_SENSOR_ACCEL:
                sensor_data_req.bits.a = true;
#if BMI160_CONFIG_ENABLE_ACC_TCOC
                sensor_data_req.bits.t = true;
#endif
                break;
            case SNS_DDF_SENSOR_GYRO:
                sensor_data_req.bits.g = true;
                break;
            case SNS_DDF_SENSOR_MAG:
                sensor_data_req.bits.m = true;
                break;
            case SNS_DDF_SENSOR_TEMP:
                sensor_data_req.bits.t = true;
                break;
            default:
                return SNS_DDF_EINVALID_PARAM;
        }
    }


    status = bmi160_read_data(state->port_handle, &sensor_data_buf, &sensor_data_req);
    BMI160_DD_CHECK_RETVAL(status, SNS_DDF_SUCCESS);
    ts = bmi160_get_sys_tick();

    sub_dev = sns_dd_bmi160_get_subdev(dd_handle);

    if ((data_ptr = sns_ddf_memhandler_malloc_ex(memhandler,
                    num_sensors * sizeof(sns_ddf_sensor_data_s),
                    sub_dev->smgr_handle))
            == NULL) {
        return SNS_DDF_ENOMEM;
    }

    *data = data_ptr;

    for (i = 0; i < num_sensors; i++) {
        data_ptr[i].sensor = sensors[i];
        data_ptr[i].status = SNS_DDF_SUCCESS;
        data_ptr[i].timestamp = ts;
        data_ptr[i].end_timestamp = ts;

        sns_dd_bmi160_adj_sample_sys_tick(state, &data_ptr[i]);

        if (SNS_DDF_SENSOR_ACCEL == sensors[i]) {
            if ((data_ptr[i].samples =
                        sns_ddf_memhandler_malloc_ex(memhandler,
                            SDD_ACC_NUM_AXIS * sizeof(sns_ddf_sensor_sample_s),
                            sub_dev->smgr_handle))
                    == NULL) {
                return SNS_DDF_ENOMEM;
            }

#if BMI160_CONFIG_ENABLE_ACC_TCOC
            sns_dd_bmi160_format_data_temp(state, sensor_data_buf.buf_temp,
                    &state->cache_temperature);
#endif

            sns_dd_bmi160_format_data_acc(state, sensor_data_buf.buf_a, data_ptr[i].samples);

            state->test_inertial_in_progress = 1;
            if (state->test_inertial_in_progress) {
                BMI160_MSG_3(MED, "[get_data_for_inertial_test] acc <%d %d %d>",
                        data_ptr[i].samples[0].sample,
                        data_ptr[i].samples[1].sample,
                        data_ptr[i].samples[2].sample);
            }

            //TODO: resume after knowledge of the delay required for BW settling
#if 0
            // If the timer is still pending, invalidate the data
            if (state->bw_timer_pending) {
                DDF_MSG_1(HIGH, "BMI160 - Invalidating sample at time: %u", ts);
                // Note: the status values for each samples[i] is UNUSED
                data_ptr[i].samples[0].status = SNS_DDF_EINVALID_DATA;
                data_ptr[i].samples[1].status = SNS_DDF_EINVALID_DATA;
                data_ptr[i].samples[2].status = SNS_DDF_EINVALID_DATA;
                data_ptr[i].status = SNS_DDF_EINVALID_DATA;
            } else {
                data_ptr[i].samples[0].status = SNS_DDF_SUCCESS;
                data_ptr[i].samples[1].status = SNS_DDF_SUCCESS;
                data_ptr[i].samples[2].status = SNS_DDF_SUCCESS;
            }
#endif
            data_ptr[i].num_samples = 3;
            sns_dd_bmi160_log_data(state, &data_ptr[i], 3, DD_DATA_RPT_CONTEXT_POLL, ts);
        } else if (SNS_DDF_SENSOR_GYRO == sensors[i]) {
            if ((data_ptr[i].samples = sns_ddf_memhandler_malloc_ex(memhandler,
                            SDD_GYR_NUM_AXIS * sizeof(sns_ddf_sensor_sample_s),
                            sub_dev->smgr_handle))
                    == NULL) {
                return SNS_DDF_ENOMEM;
            }

            sns_dd_bmi160_format_data_gyr(state, sensor_data_buf.buf_g, data_ptr[i].samples);
            if (state->test_inertial_in_progress) {
                BMI160_MSG_3(MED, "[get_data_for_inertial_test] gyr <%d %d %d>",
                        data_ptr[i].samples[0].sample,
                        data_ptr[i].samples[1].sample,
                        data_ptr[i].samples[2].sample);
            }

            data_ptr[i].num_samples = 3;

            sns_dd_bmi160_log_data(state, &data_ptr[i], 3, DD_DATA_RPT_CONTEXT_POLL, ts);
#if BMI160_CONFIG_ENABLE_MAG_IF
        } else if (SNS_DDF_SENSOR_MAG == sensors[i]) {
            if ((data_ptr[i].samples = sns_ddf_memhandler_malloc_ex(memhandler,
                            SDD_MAG_NUM_AXIS * sizeof(sns_ddf_sensor_sample_s),
                            sub_dev->smgr_handle))
                    == NULL) {
                return SNS_DDF_ENOMEM;
            }

            sns_dd_bmi160_format_data_mag(state, sensor_data_buf.buf_m, data_ptr[i].samples);
            if (state->test_inertial_in_progress) {
                BMI160_MSG_3(MED, "[get_data_for_inertial_test] mag <%d %d %d>",
                        data_ptr[i].samples[0].sample,
                        data_ptr[i].samples[1].sample,
                        data_ptr[i].samples[2].sample);
            }

            data_ptr[i].num_samples = 3;
            sns_dd_bmi160_log_data(state, &data_ptr[i], 3, DD_DATA_RPT_CONTEXT_POLL, ts);
#endif
        } else if (SNS_DDF_SENSOR_TEMP == sensors[i]) {
            if ((data_ptr[i].samples = sns_ddf_memhandler_malloc_ex(memhandler,
                            1 * sizeof(sns_ddf_sensor_sample_s),
                            sub_dev->smgr_handle))
                    == NULL) {
                return SNS_DDF_ENOMEM;
            }

            sns_dd_bmi160_format_data_temp(state, sensor_data_buf.buf_temp,
                    &state->cache_temperature);
            data_ptr[i].samples[0].sample = FX_FLTTOFIX_Q16(state->cache_temperature);
            data_ptr[i].samples[0].status = SNS_DDF_SUCCESS;

            data_ptr[i].num_samples = 1;

            sns_dd_bmi160_log_data(state, &data_ptr[i], 1, DD_DATA_RPT_CONTEXT_POLL, ts);
        }
    }

    return SNS_DDF_SUCCESS;
}


/*===========================================================================

FUNCTION:   sns_dd_bmi160_handle_timer

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
//reviewed
static void sns_dd_bmi160_handle_timer(
        sns_ddf_handle_t    dd_handle,
        void                *arg)
{
    DDF_MSG_0(MEDIUM, "55555610 - <bmi160_if_ handle_timer> - Handling BW timer...");
    sns_dd_bmi160_state_t *state = sns_dd_bmi160_get_dev(dd_handle);

    struct bmi160_timer_data *td = (struct bmi160_timer_data *)arg;

    if (NULL != td) {
        td->cb(state);
    }
}


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
static void sns_dd_bmi160_interrupt_handler(
        sns_ddf_handle_t        dd_handle,
        uint32_t                gpio1,
        sns_ddf_time_t          timestamp)
{
    sns_ddf_status_e                status = 0;
    bmi160_int_stat_flag_t          int_stat;

    sns_dd_bmi160_state_t           *state = NULL;


    //BMI160_MSG_1(ERROR, "<bmi160_iff_isr>ts:%u", timestamp);

    bmi160_cor_isr_time(&timestamp);
    BMI160_MSG_1(HIGH, "<bmi160_iff_isr>ts:%u", timestamp);

    state = sns_dd_bmi160_get_dev(dd_handle);

    if (unlikely((NULL == state)
                || (BMI160_DEV_INSTANCE_MAGIC != state->magic)
                || (gpio1 != state->gpio1))) {
        BMI160_MSG_0(ERROR, "[bmi160_iff_isr] WARNING!!! false interrupt ignored");
        return;
    }


    if (!state->int_en_flags.bits.md) {
    } else {
        if (state->acc_in_lp) {
            status = sns_dd_bmi160_send_cmd(state,
                    BMI160_REGV_CMD_ACC_MODE_NORMAL);

            if (SNS_DDF_SUCCESS != status) {
                return;
            }

            state->acc_in_lp = 0;
        }
    }

    status = bmi160_read_reg_int_ctx(state->port_handle,
            &state->int_en_flags,
            &state->reg_int_ctx);

    BMI160_MSG_4(LOW,
            "[bmi160_if_isr] en_int<fifo:%d md:%d drdy:%d dt:%d>",
            state->int_en_flags.bits.fifo.flag,
            state->int_en_flags.bits.md,
            state->int_en_flags.bits.drdy.flag,
            state->int_en_flags.bits.dt);

    bmi160_parse_int_stat_flags(&state->reg_int_ctx, &int_stat);


    BMI160_MSG_4(MED,
            "[bmi160_if_isr] istat <fifo:%d md:%d drdy:%d dt:%d>",
            int_stat.bits.ff_wml | (int_stat.bits.ff_full << 1),
            int_stat.bits.md,
            int_stat.bits.drdy,
            int_stat.bits.dt);
    if (unlikely(status)) {
        BMI160_MSG_1(ERROR, "ERROR: %d", status);
        return;
    }

#if BMI160_CONFIG_ENABLE_WA_FF_FRAMES_UNDERRUN
    if (state->int_en_flags.bits.fifo.flag
            && !(int_stat.bits.ff_wml || int_stat.bits.ff_full)) {
        if (BST_GET_VAL_BITBLOCK(state->reg_int_ctx.status, 5, 7)) {
            status = sns_dd_bmi160_handle_irq_ff_wml_partial(state, timestamp);
        }
    }
#endif

    if (state->int_en_flags.bits.fifo.flag
            && (int_stat.bits.ff_wml || int_stat.bits.ff_full)
       ) {
        if (!int_stat.bits.ff_full) {
            status = sns_dd_bmi160_handle_irq_ff_wml(state, timestamp);
        } else {
            status = sns_dd_bmi160_handle_irq_ff_full(state, timestamp);
        }
    }


    if (state->int_en_flags.bits.drdy.flag) {
        //int_stat.bits.drdy is zero, and should not be used
        status = sns_dd_bmi160_handle_irq_drdy(state, timestamp);
    }

    if (state->int_en_flags.bits.md
            && int_stat.bits.md
       ) {
        status = sns_dd_bmi160_handle_irq_md(state, timestamp);
    }

#if BMI160_CONFIG_ENABLE_INT_DOUBLE_TAP
    if (state->int_en_flags.bits.dt
            && int_stat.bits.dt
       ) {
        status = sns_dd_bmi160_handle_irq_dt(state, timestamp);
    }
#endif

    sns_dd_bmi160_handle_irq_attach_temp(state, timestamp);
}

#if BMI160_CONFIG_ENABLE_FIFO
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

sns_ddf_status_e sns_dd_bmi160_trigger_fifo_data(
        sns_ddf_handle_t    dd_handle,
        sns_ddf_sensor_e    sensor,
        uint16_t            num_samples,
        bool                trigger_now)
{
    sns_ddf_status_e status =  SNS_DDF_SUCCESS;
    sns_dd_bmi160_state_t *state = sns_dd_bmi160_get_dev(dd_handle);


    BMI160_MSG_3_F(HIGH,
            "55555600 - <bmi160_if_ trigger_fifo_data> sensor: %d num_samples: %d trigger_now: %d",
            sensor, num_samples, trigger_now);

    if (!trigger_now) {
        return SNS_DDF_SUCCESS;
    }

    status = sns_dd_bmi160_fifo_flush(state, num_samples, 0);

    return status;
}
#endif

static sns_ddf_status_e sns_dd_bmi160_get_data(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s    *memhandler,
        sns_ddf_sensor_data_s   *data[]);

static void sns_dd_bmi160_handle_timer(
        sns_ddf_handle_t dd_handle,
        void* arg);

static void sns_dd_bmi160_interrupt_handler(
        sns_ddf_handle_t       dd_handle,
        uint32_t               gpio1,
        sns_ddf_time_t         timestamp);

extern
sns_ddf_status_e sns_dd_bmi160_init(
        sns_ddf_handle_t        *dd_handle_ptr,
        sns_ddf_handle_t        smgr_handle,
        sns_ddf_nv_params_s     *nv_params,
        sns_ddf_device_access_s device_info[],
        uint32_t                num_devices,
        sns_ddf_memhandler_s    *memhandler,
        sns_ddf_sensor_e        *sensors[],
        uint32_t                *num_sensors);

extern
sns_ddf_status_e sns_dd_bmi160_set_attr(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensor_type,
        sns_ddf_attribute_e     attrib,
        void                    *value);

extern
sns_ddf_status_e sns_dd_bmi160_get_attr(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensor_type,
        sns_ddf_attribute_e     attrib,
        sns_ddf_memhandler_s    *memhandler,
        void                    **value,
        uint32_t                *num_elems);

extern
sns_ddf_status_e sns_dd_bmi160_reset(sns_ddf_handle_t dd_handle);

extern sns_ddf_status_e sns_dd_bmi160_self_test(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensor_type,
        sns_ddf_test_e          test_type,
        uint32_t                *err);

extern
sns_ddf_status_e sns_dd_bmi160_probe(
        sns_ddf_device_access_s *device_info,
        sns_ddf_memhandler_s    *memhandler,
        uint32_t                *num_sensors,
        sns_ddf_sensor_e        **sensors);


extern
sns_ddf_status_e sns_dd_bmi160_enable_sched_data(
        sns_ddf_handle_t dd_handle,
        sns_ddf_sensor_e sensor_type,
        bool enable);

extern
sns_ddf_status_e sns_dd_bmi160_trigger_fifo_data(
        sns_ddf_handle_t    dd_handle,
        sns_ddf_sensor_e    sensor,
        uint16_t            num_samples,
        bool                trigger_now);


sns_ddf_driver_if_s SNS_DD_IF_BMI160 =
{
    /* 5555550X */
    .init                 = &sns_dd_bmi160_init,
    /* 5555551X */
    .get_data             = &sns_dd_bmi160_get_data,
    /* 5555552X */
    .set_attrib           = &sns_dd_bmi160_set_attr,
    /* 5555553X */
    .get_attrib           = &sns_dd_bmi160_get_attr,
    /* 5555554X */
    .handle_timer         = &sns_dd_bmi160_handle_timer,
    /* 5555555X */
    .handle_irq           = &sns_dd_bmi160_interrupt_handler,
    /* 5555556X */
    .reset                = &sns_dd_bmi160_reset,
    /* 5555557X */
    .run_test             = &sns_dd_bmi160_self_test,
    /* 5555558X */
    .enable_sched_data    = &sns_dd_bmi160_enable_sched_data,
    /* 5555559X */
    .probe                = &sns_dd_bmi160_probe,
    /* 5555560X */
#if BMI160_CONFIG_ENABLE_FIFO
    .trigger_fifo_data    = &sns_dd_bmi160_trigger_fifo_data
#endif
};

