/*******************************************************************************
* Copyright (c) 2013-2015, ALPS ELECTRIC CO., LTD.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of ALPS ELECTRIC CO., LTD. nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
* IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

/******************************************************************************
 *
 * REVISON HISTORY
 *
 * VERSION  DATE        WHO         DESCRIPTION
 * -------  ----------  --------    ------------------------------------------
 * 1.1      Feb/24/2015 ALPS        Change sensor type.
 *                                  Primarily is uv, secondarily is ambient.
 *                                  Delete "DDK" option.
 *                                  Add Dragonboard8074 debug option.
 *                                  Add calculation equation under solar light.
 * 1.0      10/30/2014  ALPS        Initial version.
 * -------  ----------  --------    ------------------------------------------
 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "fixed_point.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_signal.h"

#include "sns_dd_uva_amb_hsvddd002a.h"

#include "log_codes.h"
#ifdef _USE_DRAGONBOARD8074
#include "sns_log_types_public.h"
#include "sns_log_api_public.h"
#else /* _USE_DRAGONBOARD8074 */
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#endif /* _USE_DRAGONBOARD8074 */

/* Forward declaration */
#ifdef _USE_DRAGONBOARD8074
#define sns_dd_uva_amb_hsvddd_if sns_dd_vendor_if_1
#endif
sns_ddf_driver_if_s sns_dd_uva_amb_hsvddd_if;

typedef struct {
    uint32_t                uv1_bias;
    uint32_t                uv1_scale;
    uint32_t                uv2_bias;
    uint32_t                uv2_scale;
    uint32_t                uv_ds1;
    uint32_t                uv_ds2;
} alps_uv_amb_dd_nv_t;

typedef struct {
    sns_ddf_handle_t        smgr_handle;
    uint32_t                num_devices;
    sns_ddf_handle_t        i2c_handle;
    sns_ddf_powerstate_e    powerstate;
    sns_ddf_timer_s         timer_handle;
    bool                    is_timer_running;
    bool                    enable[HSVDDD_SNSNUM];
    sns_ddf_sensor_data_s   sensor_data[HSVDDD_SNSNUM];
    sns_ddf_sensor_sample_s sample_cache_uv[HSVDDD_UV_DATANUM];
    sns_ddf_sensor_sample_s sample_cache_amb[HSVDDD_AMB_DATANUM];
	alps_uv_amb_dd_nv_t     nv_source; 
} alps_uv_amb_dd_state_t;

#ifdef _USE_DRAGONBOARD8074
#define SNS_DDF_SENSOR_ULTRA_VIOLET SNS_DDF_SENSOR_OEM_SENSOR_01
#define SNS_DDF_SENSOR_AMBIENT SNS_DDF_SENSOR_OEM_SENSOR_02
#else
//#define SNS_DDF_SENSOR_ULTRA_VIOLET SNS_DDF_SENSOR_ULTRA_VIOLET
//#define SNS_DDF_SENSOR_AMBIENT SNS_DDF_SENSOR_AMBIENT
#endif
const static sns_ddf_sensor_e   alps_uv_amb_sensors[] =
{
    SNS_DDF_SENSOR_ULTRA_VIOLET,
#ifdef HSVDDD002A_SUPPORT_SECONDARY
    SNS_DDF_SENSOR_AMBIENT
#endif
};


/******************************************************************************
 *
 *    log functions
 *
 *****************************************************************************/
static void alps_dd_hsvddd_log(
    sns_ddf_sensor_e    sensor,
    uint32_t            data1,
    uint32_t            data2,
    uint32_t            data3,
    sns_ddf_time_t      sample_time)
{
    sns_err_code_e              err_code;
    sns_log_sensor_data_pkt_s*  log_struct_ptr;
    log_pkt_t                   log_pkt_type;

#ifdef _USE_DRAGONBOARD8074
        log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA_PUBLIC;
#else
        log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA;
#endif

    err_code = sns_logpkt_malloc(log_pkt_type,
        sizeof(sns_log_sensor_data_pkt_s),
        (void**)&log_struct_ptr);

    if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
    {
        log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
        log_struct_ptr->sensor_id = sensor;
        log_struct_ptr->vendor_id = SNS_DDF_VENDOR_ALPS;
        log_struct_ptr->timestamp = sample_time;
        if ((sensor == SNS_DDF_SENSOR_ULTRA_VIOLET) ||
            (sensor == SNS_DDF_SENSOR_AMBIENT))
        {
            log_struct_ptr->num_data_types = 3;
            log_struct_ptr->data[0] = data1;
            log_struct_ptr->data[1] = data2;
            log_struct_ptr->data[2] = data3;
        }
        else
        {
            HSVDDD002A_MSG_1(ERROR, "Unknown sensor=%d", sensor);
            return;
        }

        err_code = sns_logpkt_commit(log_pkt_type, log_struct_ptr);
    }
}


/******************************************************************************
 *
 *    driver functions
 *
 *****************************************************************************/
/*--------------------------------------------------------------------------
 * Sensor initialize
 *--------------------------------------------------------------------------*/
static sns_ddf_status_e alps_dd_hsvddd_init(
                alps_uv_amb_dd_state_t *state)
{
    uint8_t count, buf[1];
    sns_ddf_status_e status;

    /* CTRL2 setting */
    buf[0] = REG_CS2_UVADEN | REG_CS2_AMBDEN;
    status = sns_ddf_write_port(state->i2c_handle, REG_CS2, buf, 1, &count);
    if (status != SNS_DDF_SUCCESS)
    {
        HSVDDD002A_MSG_1(ERROR, LOG_TAG "sns_ddf_write_port error=%d", status);
    }

    return status;
}

/*--------------------------------------------------------------------------
 * Software reset
 *--------------------------------------------------------------------------*/
sns_ddf_status_e alps_dd_hsvddd_reset(
                alps_uv_amb_dd_state_t *state)
{
    uint8_t count, buf[1];
    sns_ddf_status_e status;

    /* CTRL3 Soft Reset Execute */
    buf[0] = (REG_CS3_SRST);
    status = sns_ddf_write_port(state->i2c_handle, REG_CS3, buf, 1, &count);
    if (status != SNS_DDF_SUCCESS)
    {
        HSVDDD002A_MSG_1(ERROR, LOG_TAG "sns_ddf_write_port error=%d", status);
        return status;
    }
    sns_ddf_delay(3600);

    /* Sensor initialize */
    status = alps_dd_hsvddd_init(state);
    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    state->powerstate = SNS_DDF_POWERSTATE_LOWPOWER;

    return status;
}
/*--------------------------------------------------------------------------
 * Check sensor ID
 *--------------------------------------------------------------------------*/
static sns_ddf_status_e alps_dd_hsvddd_check_id(
                sns_ddf_handle_t *port_handle, uint32_t *err)
{
    uint8_t count, buf[2];
    uint16_t minfo;
    sns_ddf_status_e status;

    if (err != NULL)
    {
         *err = 0;
    }

    status = sns_ddf_read_port(*port_handle, REG_MIF, buf, 2, &count);
    if (status != SNS_DDF_SUCCESS)
    {
        if (err != NULL)
        {
            *err = (uint32_t)(-1);
        }
        HSVDDD002A_MSG_1(ERROR, LOG_TAG "sns_ddf_read_port error=%d", status);
        return status;
    }
    minfo = (((uint16_t)buf[1])<<8) | ((uint16_t)buf[0]);
    if (minfo != REG_INF_TE)
    {
        if (err != NULL)
        {
             *err = minfo;
        }
        HSVDDD002A_MSG_2(ERROR, LOG_TAG "Sensor ID mismatch (0x%X != 0x%X)",
            minfo, REG_INF_TE);
        status = SNS_DDF_EFAIL;
    }

    return status;
}

/*--------------------------------------------------------------------------
 * Self test
 *--------------------------------------------------------------------------*/
sns_ddf_status_e alps_dd_hsvddd_self_test(
                sns_ddf_handle_t *port_handle, uint32_t *err)
{
    uint8_t count, i, buf[2], ctrl1[1];
    sns_ddf_status_e status = SNS_DDF_EFAIL;
    *err = STR_PASS;

    do {
        /* --------------- Initalize ----------------- */
        /* Control resister1 backup  */
        status = sns_ddf_read_port(*port_handle, REG_CS1, ctrl1, 1, &count);
        if (status != SNS_DDF_SUCCESS)
        {
            *err = STR_ERR_I2C;
            return status;
        }
        sns_ddf_delay(30);
        /* CTRL2 : Ensble UV-A/Ambient */
        buf[0] = REG_CS2_AMBDEN | REG_CS2_UVADEN;
        status = sns_ddf_write_port(*port_handle, REG_CS2, buf, 1, &count);
        if (status != SNS_DDF_SUCCESS)
        {
            *err = STR_ERR_I2C;
            return status;
        }
        sns_ddf_delay(30);
        /* Active mode (Force state)  */
        buf[0] = (REG_CS1_PC1 | REG_CS1_FS1);
        status = sns_ddf_write_port(*port_handle, REG_CS1, buf, 1, &count);
        if (status != SNS_DDF_SUCCESS)
        {
            *err = STR_ERR_I2C;
            break;
        }
        /* Dummy read Self test A register   */
        for (i = 0; i < 2; i++) {
            status = sns_ddf_read_port(*port_handle, REG_STA, buf, 2, &count);
            if (status != SNS_DDF_SUCCESS)
            {
                *err = STR_ERR_I2C;
                break;
            }
        }

        /* --------------- Self test A --------------- */
        /* Get inital Self test A register  */
        status = sns_ddf_read_port(*port_handle, REG_STA, buf, 1, &count);
        if (status != SNS_DDF_SUCCESS)
        {
            *err = STR_ERR_I2C;
            break;
        }
        if (buf[0] != STR_REG_DEF)
        {
            *err = STR_ERR_INIT | STR_ERR_STA;
            status = SNS_DDF_EFAIL;
            break;
        }
        sns_ddf_delay(30);
        /* Execute Self test A   */
        buf[0] = (REG_CS3_STCA);
        status = sns_ddf_write_port(*port_handle, REG_CS3, buf, 1, &count);
        if (status != SNS_DDF_SUCCESS)
        {
            *err = STR_ERR_I2C;
            break;
        }
        sns_ddf_delay(100);
        /* Get 1st Self test A register  */
        status = sns_ddf_read_port(*port_handle, REG_STA, buf, 1, &count);
        if (status != SNS_DDF_SUCCESS)
        {
            *err = STR_ERR_I2C;
            break;
        }
        if (buf[0] != STR_REG_PASS)
        {
            *err = STR_ERR_1ST | STR_ERR_STA;
            status = SNS_DDF_EFAIL;
            break;
        }
        sns_ddf_delay(100);
        /* Get 2nd Self test A register  */
        status = sns_ddf_read_port(*port_handle, REG_STA, buf, 1, &count);
        if (status != SNS_DDF_SUCCESS)
        {
            *err = STR_ERR_I2C;
            break;
        }
        if (buf[0] != STR_REG_DEF)
        {
            *err = STR_ERR_2ND | STR_ERR_STA;
            status = SNS_DDF_EFAIL;
            break;
        }

        /* --------------- Finalize -------------- */
        /* Control resister1 set  */
        status = sns_ddf_write_port(*port_handle, REG_CS1, ctrl1, 1, &count);
        if (status != SNS_DDF_SUCCESS)
        {
            *err = STR_ERR_I2C;
        }

    } while (0);

    /* --------------- Finalize(error) ----------- */
    if (status != SNS_DDF_SUCCESS)
    {
        sns_ddf_write_port(*port_handle, REG_CS1, ctrl1, 1, &count);
    }

    return status;
}

/*--------------------------------------------------------------------------
 * Enable/disable sensor
 *--------------------------------------------------------------------------*/
static sns_ddf_status_e alps_dd_hsvddd_set_mode(sns_ddf_handle_t dd_handle,
    sns_ddf_powerstate_e power_state)
{
    uint8_t count, buf[1];
    sns_ddf_status_e status;
    alps_uv_amb_dd_state_t *state = dd_handle;

    switch (power_state) {
    case SNS_DDF_POWERSTATE_ACTIVE:
        status = alps_dd_hsvddd_init(state);
        if (status != SNS_DDF_SUCCESS)
        {
            return status;
        }
        sns_ddf_delay(30);
        /* CTRL1 Active Mode: Force State: 13bit */
        buf[0] = (REG_CS1_PC1 | REG_CS1_RS2 | REG_CS1_RS1 | REG_CS1_FS1);
        status = sns_ddf_write_port(state->i2c_handle,
                REG_CS1, buf, 1, &count);
        if (status != SNS_DDF_SUCCESS)
        {
            HSVDDD002A_MSG_1(ERROR, LOG_TAG "sns_ddf_write_port error=%d",
                status);
            return status;
        }
        state->powerstate = SNS_DDF_POWERSTATE_ACTIVE;
        status = SNS_DDF_SUCCESS;
        break;

    case SNS_DDF_POWERSTATE_LOWPOWER:
        /* CTRL1 Stand-by Mode: Force State */
        buf[0] = (REG_CS1_RS2 | REG_CS1_RS1 | REG_CS1_FS1);
        status = sns_ddf_write_port(state->i2c_handle,
                REG_CS1, buf, 1, &count);
        if (status != SNS_DDF_SUCCESS)
        {
            HSVDDD002A_MSG_1(ERROR, LOG_TAG "sns_ddf_write_port error=%d",
                status);
            return status;
        }
        state->powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
        status = SNS_DDF_SUCCESS;
        break;

    default:
        status = SNS_DDF_EINVALID_PARAM;
    }
    return status;
}

/*--------------------------------------------------------------------------
 * get sensor data
 *--------------------------------------------------------------------------*/
static sns_ddf_status_e alps_dd_hsvddd_get_uv(sns_ddf_handle_t dd_handle)
{
    uint8_t count;
    uint8_t buf = REG_CS3_FORCE;

    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    alps_uv_amb_dd_state_t *state = dd_handle;

    HSVDDD002A_MSG_0(MED, LOG_TAG "alps_dd_hsvddd_get_uv - enter");

    /* Start 5 ms measurement timer */
    if (!state->is_timer_running)
    {

        status = sns_ddf_write_port(state->i2c_handle,
                                    REG_CS3, &buf, 1, &count);
        if (status != SNS_DDF_SUCCESS)
        {
            HSVDDD002A_MSG_1(ERROR, LOG_TAG "sns_ddf_write_port error=%d",
                status);
            return status;
        }

        status =
            sns_ddf_timer_start(state->timer_handle, HSVDDD_MEASURE_TIME_US);
        if (status != SNS_DDF_SUCCESS)
        {
            HSVDDD002A_MSG_1(ERROR, LOG_TAG "sns_ddf_timer_start error=%d",
                status);
            return status;
        }

        state->is_timer_running = true;
    }

    return status;
}


/*--------------------------------------------------------------------------
 * run test
 *--------------------------------------------------------------------------*/
sns_ddf_status_e alps_dd_hsvddd_run_test(sns_ddf_handle_t dd_handle,
    sns_ddf_test_e test, uint32_t *err)
{
    sns_ddf_status_e status = SNS_DDF_EFAIL;
    alps_uv_amb_dd_state_t *state = dd_handle;

    *err = 0;
    status = SNS_DDF_EFAIL;

    switch (test) {
    case SNS_DDF_TEST_SELF_HW:         /**< Hardware self test. */
        status = alps_dd_hsvddd_self_test(&state->i2c_handle, err);
        break;
    case SNS_DDF_TEST_SELF:
    case SNS_DDF_TEST_CONNECTIVITY:  /**< Basic connectivity test. */
        status = alps_dd_hsvddd_check_id(&state->i2c_handle, err);
        break;
    default:
        status = SNS_DDF_EINVALID_TEST;
        break;
    }

    return status;
}


/******************************************************************************
 *
 *    DDF functions
 *
 *****************************************************************************/
/*--------------------------------------------------------------------------
 * get sensor data
 *--------------------------------------------------------------------------*/
sns_ddf_status_e alps_uva_amb_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s *memhandler,
    sns_ddf_sensor_data_s **data)
{
    uint8_t i;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    alps_uv_amb_dd_state_t *state = dd_handle;

    HSVDDD002A_MSG_1(MED, LOG_TAG "alps_uva_amb_get_data, num_sensors=%d",
        num_sensors);

    /* check work memory */
    if (state == NULL)
    {
        return SNS_DDF_ENOMEM;
    }

    /* check powerstate */
    if (state->powerstate != SNS_DDF_POWERSTATE_ACTIVE)
    {
        HSVDDD002A_MSG_0(ERROR, LOG_TAG "In low power");
        return SNS_DDF_EFAIL;
    }

    for (i = 0; i < num_sensors; i++) {
        switch (sensors[i]) {
        case SNS_DDF_SENSOR_ULTRA_VIOLET:
            HSVDDD002A_MSG_0(MED, "SNS_DDF_SENSOR_UV");
            state->enable[HSVDDD_UVA] = true;
            break;

        case SNS_DDF_SENSOR_AMBIENT:
            HSVDDD002A_MSG_0(MED, "SNS_DDF_SENSOR_AMBIENT");
            state->enable[HSVDDD_AMB] = true;
            break;

        default:
            HSVDDD002A_MSG_1(MED, "Unknown sensor=%d", sensors[i]);
            return SNS_DDF_EFAIL;
        }
    }

    if (state->enable[HSVDDD_UVA] || state->enable[HSVDDD_AMB])
    {
        /* Request data */
        status = alps_dd_hsvddd_get_uv(state);
        if (status != SNS_DDF_SUCCESS)
        {
            HSVDDD002A_MSG_1(ERROR, LOG_TAG "alps_dd_hsvddd_get_uv error=%d",
                status);
            return status;
        }
    }

    return SNS_DDF_PENDING;
}

/*--------------------------------------------------------------------------
 * Set attributes
 *--------------------------------------------------------------------------*/
sns_ddf_status_e alps_uva_amb_set_attrib(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void *value)
{
    alps_uv_amb_dd_state_t *state = dd_handle;

    HSVDDD002A_MSG_3(MED,
        LOG_TAG "alps_uva_amb_set_attrib, sensor=%d, attrib=%d, value=%d",
        sensor, attrib, *(uint8_t *)value);

    /* check work memory */
    if (state == NULL)
    {
        return SNS_DDF_ENOMEM;
    }

    switch (attrib) {
    case SNS_DDF_ATTRIB_POWER_STATE:
        {
            sns_ddf_powerstate_e *power_state = value;
            return alps_dd_hsvddd_set_mode(dd_handle, *power_state);
        }
    default:
        return SNS_DDF_EINVALID_ATTR;
    }
}

/*--------------------------------------------------------------------------
 * Get attributes
 *--------------------------------------------------------------------------*/
sns_ddf_status_e alps_uva_amb_get_attrib(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s *memhandler,
    void **value,
    uint32_t *num_elems)
{
    alps_uv_amb_dd_state_t *state = dd_handle;

    HSVDDD002A_MSG_2(MED, LOG_TAG "alps_uva_amb_get_attrib, sensor=%d, attrib=%d",
        sensor, attrib);

    /* check work memory */
    if (state == NULL)
    {
        return SNS_DDF_ENOMEM;
    }

    switch (attrib) {
    case SNS_DDF_ATTRIB_POWER_STATE:
        {
            sns_ddf_powerstate_e *power = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_powerstate_e));
            if (power == NULL)
            {
                return SNS_DDF_ENOMEM;
            }
            *value = (void *)&(state->powerstate);
            *num_elems = 1;
            break;
        }
    case SNS_DDF_ATTRIB_POWER_INFO:
        {
            sns_ddf_power_info_s *power = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_power_info_s));
            if (power == NULL)
            {
                return SNS_DDF_ENOMEM;
            }
            power->active_current   = HSVDDD_SENSOR_POWER_ACTIVE;
            power->lowpower_current = HSVDDD_SENSOR_LOWPOWER;
            *value     = power;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }
    case SNS_DDF_ATTRIB_RANGE:
        {
            switch (sensor) {
            case SNS_DDF_SENSOR_ULTRA_VIOLET:
                {
                    sns_ddf_range_s *ranges;
                    ranges = sns_ddf_memhandler_malloc(
                        memhandler,
                        sizeof(sns_ddf_range_s));
                    if (ranges == NULL)
                    {
                        return SNS_DDF_ENOMEM;
                    }
                    ranges[0].min = HSVDDD_UVA_SENSOR_RANGE_MIN;
                    ranges[0].max = HSVDDD_UVA_SENSOR_RANGE_MAX;
                    *value     = ranges;
                    *num_elems = 1;
                    return SNS_DDF_SUCCESS;
                }
            case SNS_DDF_SENSOR_AMBIENT:
                {
                    sns_ddf_range_s *ranges;
                    ranges = sns_ddf_memhandler_malloc(
                        memhandler,
                        sizeof(sns_ddf_range_s));
                    if (ranges == NULL)
                    {
                        return SNS_DDF_ENOMEM;
                    }
                    ranges[0].min = HSVDDD_AMB_SENSOR_RANGE_MIN;
                    ranges[0].max = HSVDDD_AMB_SENSOR_RANGE_MAX;
                    *value     = ranges;
                    *num_elems = 1;
                    return SNS_DDF_SUCCESS;
                }
            default:
                return SNS_DDF_EINVALID_ATTR;
            }
        }
    case SNS_DDF_ATTRIB_RESOLUTION:
        {
            switch (sensor) {
            case SNS_DDF_SENSOR_ULTRA_VIOLET:
                {
                    sns_ddf_resolution_t *res;
                    res = sns_ddf_memhandler_malloc(
                        memhandler,
                        sizeof(sns_ddf_resolution_t));
                    if (res == NULL)
                    {
                        return SNS_DDF_ENOMEM;
                    }
                    res[0] = FX_FLTTOFIX_Q16(HSVDDD_UVA_SENSOR_RESOLUTION);
                    *value     = res;
                    *num_elems = 1;
                    return SNS_DDF_SUCCESS;
                }
            case SNS_DDF_SENSOR_AMBIENT:
                {
                    sns_ddf_resolution_t *res;
                    res = sns_ddf_memhandler_malloc(
                        memhandler,
                        sizeof(sns_ddf_resolution_t));
                    if (res == NULL)
                    {
                        return SNS_DDF_ENOMEM;
                    }
                    res[0] = FX_FLTTOFIX_Q16(HSVDDD_AMB_SENSOR_RESOLUTION);
                    *value     = res;
                    *num_elems = 1;
                    return SNS_DDF_SUCCESS;
                }
            default:
                return SNS_DDF_EINVALID_ATTR;
            }
        }
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            switch (sensor) {
            case SNS_DDF_SENSOR_ULTRA_VIOLET:
            case SNS_DDF_SENSOR_AMBIENT:
                {
                    sns_ddf_resolution_adc_s *res_adc;
                    res_adc = sns_ddf_memhandler_malloc(memhandler,
                        sizeof(sns_ddf_resolution_adc_s));
                    if (res_adc == NULL)
                    {
                        return SNS_DDF_ENOMEM;
                    }
                    res_adc[0].bit_len = HSVDDD_SENSOR_BIT_LEN;
                    res_adc[0].max_freq = HSVDDD_SENSOR_MAX_FREQ;
                    *value     = res_adc;
                    *num_elems = 1;
                    return SNS_DDF_SUCCESS;
                }
            default:
                return SNS_DDF_EINVALID_ATTR;
            }
        }
    case SNS_DDF_ATTRIB_DELAYS:
        {
            sns_ddf_delays_s *delays = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_delays_s));
            if (delays == NULL)
            {
                return SNS_DDF_ENOMEM;
            }
            delays->time_to_active = HSVDDD_SENSOR_DELAY_ACTIVE;
            delays->time_to_data   = HSVDDD_SENSOR_DELAY_DATA;
            *value     = delays;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

    case SNS_DDF_ATTRIB_DRIVER_INFO:
        {
            sns_ddf_driver_info_s *info = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_driver_info_s));
            if (info == NULL)
            {
                return SNS_DDF_ENOMEM;
            }
            info->name    = HSVDDD_SENSOR_DRIVER_NAME;
            info->version = HSVDDD_SENSOR_DRIVER_VER;
            *value     = info;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

    case SNS_DDF_ATTRIB_DEVICE_INFO:
        {
            sns_ddf_device_info_s *info = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_device_info_s));
            if (info == NULL)
            {
                return SNS_DDF_ENOMEM;
            }
            if (sensor == SNS_DDF_SENSOR_ULTRA_VIOLET)
            {
                info->name = HSVDDD_SENSOR_DEVICE_UVA_NAME;
            }
            else if (sensor == SNS_DDF_SENSOR_AMBIENT)
            {
                info->name = HSVDDD_SENSOR_DEVICE_AMB_NAME;
            }
            else
            {
                return SNS_DDF_EINVALID_PARAM;
            }
            info->vendor  = HSVDDD_SENSOR_DEVICE_VENDOR;
            info->model   = HSVDDD_SENSOR_DEVICE_MODEL;
            info->version = HSVDDD_SENSOR_DEVICE_VER;
            *value     = info;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }
    default:
        return SNS_DDF_EINVALID_ATTR;
    }
    return SNS_DDF_SUCCESS;
}

/*--------------------------------------------------------------------------
 * Timer function
 *--------------------------------------------------------------------------*/
void alps_uva_amb_handle_timer(
    sns_ddf_handle_t dd_handle,
    void *arg)
{
    alps_uv_amb_dd_state_t *state = (alps_uv_amb_dd_state_t *)arg;
    uint8_t count, buf[HSVDDD_SENSOR_READ_DATA_NUM];
    sns_ddf_status_e status;
    int16_t uvA_lsb, amb_lsb;
    sns_ddf_time_t timestamp = sns_ddf_get_timestamp();
    uint8_t num_sensor_data = 0;
    double amb_lx;

    HSVDDD002A_MSG_0(MED, LOG_TAG "alps_uva_amb_handle_timer");

    /* check work memory */
    if (state == NULL)
    {
        return;
    }

    status = sns_ddf_read_port(state->i2c_handle,
                REG_UVA_AMB, buf, HSVDDD_SENSOR_READ_DATA_NUM, &count);

    state->is_timer_running = false;

    if (status != SNS_DDF_SUCCESS)
    {
        HSVDDD002A_MSG_1(ERROR, LOG_TAG "sns_ddf_read_port error=%d", status);
        return;
    }

    /* convert uv-A */
    memset(state->sample_cache_uv, 0, sizeof(state->sample_cache_uv));
    uvA_lsb = (int16_t)(((uint16_t)buf[5])<<8) | ((uint16_t)buf[4]);

    if (uvA_lsb >= (int16_t)0x0FFF)
    {
        state->sample_cache_uv[HSVDDD_UVA_Q16].status = SNS_DDF_EDATA_BOUND;
    }
    else if (uvA_lsb < 0)
    {
        uvA_lsb = 0;
    }
    
    uvA_lsb = (uvA_lsb * state->nv_source.uv1_scale / 100) - state->nv_source.uv1_bias;

    state->sample_cache_uv[HSVDDD_UVA_Q16].sample =
            FX_FLTTOFIX_Q16((double)uvA_lsb *
                                HSVDDD_UVA_SENSOR_RESOLUTION);

    /* convert Ambient */
    memset(state->sample_cache_amb, 0, sizeof(state->sample_cache_amb));
    amb_lsb = (((uint16_t)buf[1])<<8) | ((uint16_t)buf[0]);

    if (amb_lsb >= (uint16_t)0x0FFF)
    {
        state->sample_cache_amb[HSVDDD_AMB_LX_Q16].status = SNS_DDF_EDATA_BOUND;
        state->sample_cache_amb[HSVDDD_AMB_RAW].status = SNS_DDF_EDATA_BOUND;
        state->sample_cache_amb[HSVDDD_AMB_LX_UI32].status = SNS_DDF_EDATA_BOUND;
    }
    else if (amb_lsb < 0)
    {
        amb_lsb = 0;
    }
    amb_lx = (double)amb_lsb * HSVDDD_AMB_SENSOR_RESOLUTION;
    state->sample_cache_amb[HSVDDD_AMB_LX_Q16].sample =
        FX_FLTTOFIX_Q16(amb_lx);
    if (state->sample_cache_amb[HSVDDD_AMB_LX_Q16].sample == FX_MAX_Q16)
    {
        state->sample_cache_amb[HSVDDD_AMB_LX_Q16].status = SNS_DDF_EDATA_BOUND;
    }
    state->sample_cache_amb[HSVDDD_AMB_RAW].sample = (uint32_t)amb_lsb;
    state->sample_cache_amb[HSVDDD_AMB_LX_UI32].sample = (uint32_t)amb_lx;

    HSVDDD002A_MSG_2(MED,
        LOG_TAG "uvA_lsb, 0x%04X, amb_lsb, 0x%04X",
        uvA_lsb, amb_lsb);

    /* update uv-a data to upper layer */
    if (state->enable[HSVDDD_UVA])
    {
        state->sensor_data[num_sensor_data].sensor
                            = SNS_DDF_SENSOR_ULTRA_VIOLET;
        state->sensor_data[num_sensor_data].samples
                            = state->sample_cache_uv;
        state->sensor_data[num_sensor_data].status =
                    state->sample_cache_uv[HSVDDD_UVA_Q16].status;
        state->sensor_data[num_sensor_data].timestamp = timestamp;
        state->sensor_data[num_sensor_data].num_samples = 1;

        num_sensor_data++;
        state->enable[HSVDDD_UVA] = false;

        alps_dd_hsvddd_log(SNS_DDF_SENSOR_ULTRA_VIOLET,
            (uint32_t)(state->sample_cache_uv[HSVDDD_UVA_Q16].sample),
            0,
            0,
            timestamp);
    }
    /* update ambient data to upper layer */
    if (state->enable[HSVDDD_AMB])
    {
        state->sensor_data[num_sensor_data].sensor = SNS_DDF_SENSOR_AMBIENT;
        state->sensor_data[num_sensor_data].samples
                            = state->sample_cache_amb;
        state->sensor_data[num_sensor_data].status =
                    state->sample_cache_amb[HSVDDD_AMB_RAW].status;
        state->sensor_data[num_sensor_data].timestamp = timestamp;
        state->sensor_data[num_sensor_data].num_samples = 3;

        num_sensor_data++;
        state->enable[HSVDDD_AMB] = false;

        alps_dd_hsvddd_log(SNS_DDF_SENSOR_AMBIENT,
            (uint32_t)(state->sample_cache_amb[HSVDDD_AMB_LX_UI32].sample),
            (uint32_t)(state->sample_cache_amb[HSVDDD_AMB_LX_Q16].sample),
            (uint32_t)(state->sample_cache_amb[HSVDDD_AMB_RAW].sample),
            timestamp);
    }

    if (num_sensor_data != 0)
    {
        sns_ddf_smgr_notify_data(state->smgr_handle,
            state->sensor_data, num_sensor_data);
    }
}

/*--------------------------------------------------------------------------
 * Reset function
 *--------------------------------------------------------------------------*/
sns_ddf_status_e alps_uva_amb_reset(sns_ddf_handle_t dd_handle)
{
    alps_uv_amb_dd_state_t *state = dd_handle;

    HSVDDD002A_MSG_0(MED, LOG_TAG "alps_uva_amb_reset");

    /* check work memory */
    if (state == NULL)
    {
        return SNS_DDF_ENOMEM;
    }

    state->enable[HSVDDD_UVA] = false;
    state->enable[HSVDDD_AMB] = false;
    state->is_timer_running = false;
    state->powerstate = SNS_DDF_POWERSTATE_LOWPOWER;

    return alps_dd_hsvddd_reset(state);
}

/*--------------------------------------------------------------------------
 * Run test function
 *--------------------------------------------------------------------------*/
sns_ddf_status_e alps_uva_amb_run_test(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_test_e test,
    uint32_t *err)
{
    sns_ddf_status_e status;
    alps_uv_amb_dd_state_t *state = dd_handle;

    HSVDDD002A_MSG_2(MED, LOG_TAG "alps_uva_amb_run_test, sensor=%d, test=%d",
        sensor, test);

    /* check work memory */
    if (state == NULL)
    {
        return SNS_DDF_ENOMEM;
    }

    if (state->powerstate == SNS_DDF_POWERSTATE_ACTIVE)
    {
        return SNS_DDF_EDEVICE_BUSY;
    }

    status = alps_dd_hsvddd_run_test(state, test, err);
    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    return SNS_DDF_SUCCESS;
}

/*--------------------------------------------------------------------------
 * Initializes function
 *--------------------------------------------------------------------------*/
sns_ddf_status_e alps_uva_amb_init(
    sns_ddf_handle_t *dd_handle_ptr,
    sns_ddf_handle_t smgr_handle,
    sns_ddf_nv_params_s *nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t num_devices,
    sns_ddf_memhandler_s *memhandler,
    sns_ddf_sensor_e **sensors,
    uint32_t *num_sensors)
{
    sns_ddf_status_e status;
    alps_uv_amb_dd_state_t *alps_state_ptr;

    *dd_handle_ptr = NULL;
    *sensors       = NULL;

    HSVDDD002A_MSG_0(HIGH, "alps_uva_amb_init");

    status = sns_ddf_malloc((void **)&alps_state_ptr,
        sizeof(alps_uv_amb_dd_state_t));

    if (status != SNS_DDF_SUCCESS)
    {
        return SNS_DDF_ENOMEM;
    }

    memset(alps_state_ptr, 0, sizeof(alps_uv_amb_dd_state_t));

    status = sns_ddf_open_port(&(alps_state_ptr->i2c_handle),
        &(device_info->port_config));

    if (status != SNS_DDF_SUCCESS)
    {
        goto out_init_malloc;
    }

    /* parameter backup */
    alps_state_ptr->smgr_handle = smgr_handle;
    alps_state_ptr->num_devices = num_devices;

    /* init timer */
    status = sns_ddf_timer_init(&alps_state_ptr->timer_handle, 
                                alps_state_ptr->smgr_handle, 
                                &sns_dd_uva_amb_hsvddd_if, 
                                alps_state_ptr,
                                false);

    if (status != SNS_DDF_SUCCESS)
    {
        goto out_init_open_port;
    }

    /* Initialize Registry Items */
    if (!nv_params || (nv_params->data_len != sizeof(alps_uv_amb_dd_nv_t)))
    {
        /* Setting default values */
        alps_state_ptr->nv_source.uv1_bias  = 0;
        alps_state_ptr->nv_source.uv1_scale = 100;
        alps_state_ptr->nv_source.uv2_bias  = 0;
        alps_state_ptr->nv_source.uv2_scale = 100;
        alps_state_ptr->nv_source.uv_ds1    = 0;
        alps_state_ptr->nv_source.uv_ds2    = 0;
    }
    else
    {
        alps_uv_amb_dd_nv_t      *nv_source = (alps_uv_amb_dd_nv_t *)nv_params->data;
        alps_state_ptr->nv_source.uv1_bias  = nv_source->uv1_bias;
        alps_state_ptr->nv_source.uv1_scale = nv_source->uv1_scale;
        alps_state_ptr->nv_source.uv2_bias  = nv_source->uv2_bias;
        alps_state_ptr->nv_source.uv2_scale = nv_source->uv2_scale;
        alps_state_ptr->nv_source.uv_ds1    = nv_source->uv_ds1;
        alps_state_ptr->nv_source.uv_ds2    = nv_source->uv_ds2;
    }

    alps_dd_hsvddd_reset(alps_state_ptr);
    if (status != SNS_DDF_SUCCESS)
    {
        goto out_init_open_port;
    }

    /* return sensors parameter */
#ifdef HSVDDD002A_SUPPORT_SECONDARY
    *num_sensors   = HSVDDD_SNSNUM;
#else
    *num_sensors   = 1;
#endif

    *sensors  = (sns_ddf_sensor_e *)alps_uv_amb_sensors;
    *dd_handle_ptr = alps_state_ptr;

#if 1
/**
 * Factory tests.
 */
    {
        uint32_t err;
        sns_ddf_status_e test_rc;
        test_rc = alps_uva_amb_run_test((sns_ddf_handle_t)alps_state_ptr,
                SNS_DDF_SENSOR_ULTRA_VIOLET, SNS_DDF_TEST_SELF, &err);

        if (test_rc != SNS_DDF_SUCCESS)
        {
           // Do nothing. This is to eliminate compiler warning.
        }
        HSVDDD002A_MSG_2(MED, LOG_TAG "SNS_DDF_TEST_SELF, rc=%d, err=%d",
                test_rc, err);

        status = alps_uva_amb_run_test((sns_ddf_handle_t)alps_state_ptr,
                SNS_DDF_SENSOR_ULTRA_VIOLET, SNS_DDF_TEST_CONNECTIVITY, &err);
        HSVDDD002A_MSG_2(MED,
                LOG_TAG "SNS_DDF_TEST_CONNECTIVITY, rc=%d, err=%d",
                test_rc, err);
    }
#endif

    return SNS_DDF_SUCCESS;

out_init_open_port:
    sns_ddf_close_port(&(alps_state_ptr->i2c_handle));

out_init_malloc:
    sns_ddf_mfree((void **)&alps_state_ptr);
    alps_state_ptr = NULL;

    return status;

}

/*--------------------------------------------------------------------------
 * Probes function
 *--------------------------------------------------------------------------*/
sns_ddf_status_e alps_uva_amb_run_probe
    (
    sns_ddf_device_access_s* device_info,
    sns_ddf_memhandler_s*    memhandler,
    uint32_t*                num_sensors,
    sns_ddf_sensor_e**       sensors
    )
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    sns_ddf_handle_t port_handle;

    HSVDDD002A_MSG_0(MED, LOG_TAG "alps_uva_amb_run_probe");

    *num_sensors = 0;
    *sensors = NULL;

    status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
    if (status != SNS_DDF_SUCCESS) 
    {
        HSVDDD002A_MSG_1(ERROR, LOG_TAG "sns_ddf_open_port error=%d", status);
        return status;
    }

    status = alps_dd_hsvddd_check_id(&port_handle, NULL);
    if (status != SNS_DDF_SUCCESS)
    {
        HSVDDD002A_MSG_1(ERROR, LOG_TAG "Sensor ID check error=%d", status);
        sns_ddf_close_port(port_handle);
        return status;
    }

#ifdef HSVDDD002A_SUPPORT_SECONDARY
    *num_sensors = HSVDDD_SNSNUM;
#else
    *num_sensors = 1;
#endif
    *sensors = sns_ddf_memhandler_malloc( memhandler,
        sizeof(sns_ddf_sensor_e) * *num_sensors );

    if (*sensors != NULL)
    {
        (*sensors)[0] = SNS_DDF_SENSOR_ULTRA_VIOLET;
#ifdef HSVDDD002A_SUPPORT_SECONDARY
        (*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
#endif
        status = SNS_DDF_SUCCESS;
    }
    else
    {
        HSVDDD002A_MSG_0(ERROR, LOG_TAG "sns_ddf_memhandler_malloc error");
        status = SNS_DDF_ENOMEM;
    }

    sns_ddf_close_port( port_handle );
    return status;
}

sns_ddf_driver_if_s sns_dd_uva_amb_hsvddd_if = {
    .init = &alps_uva_amb_init,
    .get_data = &alps_uva_amb_get_data,
    .set_attrib = &alps_uva_amb_set_attrib,
    .get_attrib = &alps_uva_amb_get_attrib,
    .handle_timer = &alps_uva_amb_handle_timer,
    .handle_irq = NULL,
    .reset = &alps_uva_amb_reset,
    .run_test = &alps_uva_amb_run_test,
    .enable_sched_data = NULL,
    .probe = &alps_uva_amb_run_probe,
    .trigger_fifo_data = NULL
};
