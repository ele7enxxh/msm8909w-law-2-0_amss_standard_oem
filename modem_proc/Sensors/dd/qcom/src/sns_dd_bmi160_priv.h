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

#ifndef __SNS_DD_BMI160_PRIV_H
#define __SNS_DD_BMI160_PRIV_H

#include "sns_dd_bmi160.h"

#if BMI160_CONFIG_RUN_ON_OSSC
#define SNS_DD_IF_BMI160 sns_dd_vendor_if_1
#else
#define SNS_DD_IF_BMI160 sns_dd_fn_list_bmi160

#endif


extern
void bmi160_mdelay(uint32_t ms);

extern
int32_t bmi160_sbus_read_wrapper(
        void                    *plat_obj,
        uint8_t                 reg_addr,
        uint8_t                 *buf,
        uint32_t                len);

extern
int32_t bmi160_sbus_write_wrapper(
        void                    *plat_obj,
        uint8_t                 reg_addr,
        uint8_t                 *buf,
        uint32_t                len);

extern
void sns_dd_bmi160_get_ts_cor(
        enum BMI160_ODR_IDX     odr_idx,
        uint32_t                ts_dev,
        sns_ddf_time_t          ts_sys,
        sns_ddf_time_t          *ts_sys_cor);

extern
sns_ddf_status_e bmi160_get_reg_hw_err_stat(
        sns_ddf_handle_t                port_handle,
        union bmi160_hw_err_stat        *hw_err_st);

extern
void bmi160_parse_reg_status(
        uint8_t                 regv_status,
        bmi160_status_reg_t     *stat_reg);

extern
sns_ddf_status_e bmi160_set_reg_int_map(
        sns_ddf_handle_t                port_handle,
        enum BMI160_INT_MAP_REG_NUM     map_num,
        uint8_t                         bit,
        bool                            enable);

extern
void bmi160_parse_data_acc(
        uint8_t                 *buf,
        uint8_t                 len,
        struct bmi160_val_acc   *val);

extern
void sns_dd_bmi160_format_data_temp(
        sns_dd_bmi160_state_t   *state,
        uint8_t                 buf[],
        double                  *temp);

extern
sns_dd_bmi160_state_t* sns_dd_bmi160_get_dev(
        sns_ddf_handle_t dd_handle);

extern
bmi160_sub_dev_t* sns_dd_bmi160_get_subdev_from_type(
        sns_dd_bmi160_state_t *state,
        const sns_ddf_sensor_e       sensor_type);

extern
bmi160_hw_mod_t* sns_dd_bmi160_get_hw_mod(
        sns_dd_bmi160_state_t   *state,
        sns_ddf_sensor_e        sensor_type);

extern
sns_ddf_status_e sns_dd_bmi160_signal_register(
        sns_dd_bmi160_state_t   *state);

extern
sns_ddf_status_e sns_dd_bmi160_signal_deregister(
        sns_dd_bmi160_state_t *state);

extern
sns_ddf_status_e sns_dd_bmi160_send_cmd(
        sns_dd_bmi160_state_t   *state,
        uint8_t                 cmd_regv);


extern
void sns_dd_bmi160_log_data_be(
        const sns_ddf_sensor_data_s     *data_collection,
        uint8_t                         num_data_types_per_sample);

extern
sns_ddf_status_e sns_dd_bmi160_fifo_flush(
        sns_dd_bmi160_state_t           *state,
        uint16_t                        num_samples,
        uint32_t                        caller);

extern
void sns_dd_bmi160_get_couple_ts_host_and_dev(sns_dd_bmi160_state_t *state);


extern
sns_ddf_status_e sns_dd_bmi160_start_timer_sbus_susp(
        sns_dd_bmi160_state_t *state);
extern
int32_t sns_dd_bmi160_handle_timer_sbus_susp(void *private_data);


extern
struct bmi160_hw_mod* sns_dd_bmi160_get_hw_mod_cache_acc(
        sns_dd_bmi160_state_t   *state);

extern
struct bmi160_hw_mod* sns_dd_bmi160_get_hw_mod_cache_gyr(
        sns_dd_bmi160_state_t   *state);

extern
struct bmi160_hw_mod* sns_dd_bmi160_get_hw_mod_cache_mag(
        sns_dd_bmi160_state_t   *state);

//reviewed
    static inline
bmi160_sub_dev_t* sns_dd_bmi160_get_subdev(
        sns_ddf_handle_t dd_handle)
{
    bmi160_sub_dev_t *sub_dev = (bmi160_sub_dev_t *)dd_handle;
    return sub_dev;
}

//uted
    static inline
uint32_t bmi160_get_ts_dev_msb(
        enum BMI160_ODR_IDX     odr_idx,
        uint32_t                ts_dev)
{
    uint8_t  lsb_bits = (BMI160_ODR_IDX_1600HZ - odr_idx) + 4;

    return (ts_dev & ~((1 << lsb_bits) - 1));
}


//uted
    static inline
uint32_t bmi160_get_time_elapse_dev_lsb(uint32_t before, uint32_t after)
{
    if (after >= before) {
        return (after - before);
    } else {
        return (BMI160_TS_LSB_OVERFLOW_VAL - before + after);
    }
}

//uted
    static inline
uint32_t bmi160_get_time_elapse_dev_us(uint32_t before, uint32_t after)
{
    if (after > before) {
        return BMI160_TS_HW_RES_US * (after - before);
    } else {
        return BMI160_TS_HW_RES_US * (BMI160_TS_LSB_OVERFLOW_VAL - before + after);
    }
}



#define TS_MASK_LOW32  ((uint64_t)(((uint64_t)1 << 32) - 1))
#define BMI160_MSG_64BIT_NUM(n) BMI160_MSG_2(HIGH, "64b <%u %u>", n & TS_MASK_LOW32, ((uint64_t)n) >> 32)

//uted
/*! @description: time has just rewinded between a and b
 *  @assumption: there is at most one time roll over between a and b
 */
    static inline
bool sns_dd_bmi160_time_rewinded(
        sns_ddf_time_t before,
        sns_ddf_time_t after)
{
    bool rewinded;

    if (sizeof(sns_ddf_time_t) >= sizeof(uint64_t)) {
        return false;
    }

    uint8_t pos_msb = (sizeof(sns_ddf_time_t) * 8) - 2;

    //printf("pos_msb: %d 1bits of %x \n:%x", pos_msb, before, before  & (1 << pos_msb));
    rewinded = ((before & (3 << pos_msb)) == (3 << pos_msb))
        && !(after & (3 << pos_msb));

    return rewinded;
}

/*! @description: not eariler than
*/
//uted
    static inline
bool sns_dd_bmi160_time_net(
        sns_ddf_time_t a,
        sns_ddf_time_t b)
{
    if (sizeof(sns_ddf_time_t) >= sizeof(uint64_t)) {
        return a >= b;
    }

    if (a < b) {
        return sns_dd_bmi160_time_rewinded(b, a);
    } else {
        return !sns_dd_bmi160_time_rewinded(a, b);
    }
}

//uted
    static inline
sns_ddf_time_t sns_dd_bmi160_get_time_elapse_sys(
        sns_ddf_time_t start,
        sns_ddf_time_t end)
{
    sns_ddf_time_t elapse;

    if (end >= start) {
        elapse = end - start;
    } else {
        elapse = (sns_ddf_time_t)(-1) - start + end + 1;
    }

    return elapse;
}

//uted
    static inline
sns_ddf_time_t bmi160_get_time_sys_mid(
        sns_ddf_time_t a,
        sns_ddf_time_t b)
{
    sns_ddf_time_t m;

    sns_ddf_time_t tmp;

    if (sns_dd_bmi160_time_net(b, a)) {
        //printf("[mid]a >= b %x %x\n", a, b);
        tmp = (sns_dd_bmi160_get_time_elapse_sys(a, b) >> 1);
        m = a + tmp;
    } else {
        //printf("[mid]a < b %x %x\n", a, b);
        tmp = (sns_dd_bmi160_get_time_elapse_sys(b, a) >> 1);
        m = b + tmp;
    }

    return m;
}

#define TIME_SYS_HALF_OF    ((sns_ddf_time_t)(1<<31))
#define TIME_SYS_FULL_OF    ((sns_ddf_time_t)(-1))
#define TIME_SYS_3MIN       (32768 * 180)   //9830400

#define BMI160_CONFIG_ENABLE_UT_TS 0

#if BMI160_CONFIG_ENABLE_UT_TS
#define TIME_SYS_OFFSET   (TIME_SYS_HALF_OF - TIME_SYS_3MIN)
//#define TIME_SYS_OFFSET   (TIME_SYS_FULL_OF - TIME_SYS_3MIN)
#else
#define TIME_SYS_OFFSET     0
#endif

    static inline
void bmi160_cor_isr_time(sns_ddf_time_t *ts_irq)
{
#if BMI160_CONFIG_ENABLE_UT_TS
    *ts_irq += TIME_SYS_OFFSET;
#endif
}

    static inline
sns_ddf_time_t bmi160_get_sys_tick(void)
{
#if BMI160_CONFIG_ENABLE_UT_TS
    return sns_ddf_get_timestamp() + TIME_SYS_OFFSET;
#else
    return sns_ddf_get_timestamp();
#endif
}

static inline
uint32_t bmi160_convert_tick_2_us(sns_ddf_time_t ticks)
{
    uint32_t us;

    us = (uint32_t)(ticks * 1.0 / BMI160_SSC_US_2_TICKS(1000000) * 1000000);

    return us;
}

#endif

