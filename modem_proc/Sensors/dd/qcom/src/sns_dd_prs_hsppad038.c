/*******************************************************************************
* Copyright (c) 2013, ALPS ELECTRIC CO., LTD.
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

/*-----------------------------------------------------------------------------
* Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------------*/

/*==============================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     -----------------------------------------------------------
2013-02-04   ALPS    Changed the processing timing of timer running flag.
2013-02-03   MW      Added timer for asynch data notification
2014-01-29   ALPS    And Added temperature sensor.
2014-01-27   MW      Fix build errors, aded simple I2C APIs
2013-12-26   ALPS    Initial version
==============================================================================*/

#include <stdio.h>
#include <string.h>

#include "fixed_point.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_signal.h"

#include "sns_dd_prs_hsppad038.h"

#ifndef DDK
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#endif

// Forward declaration
sns_ddf_driver_if_s sns_dd_prs_hsppad_if;

typedef struct {
    sns_ddf_handle_t        alps_prs_smgr_handle;
    uint32_t                alps_prs_num_devices;
    sns_ddf_handle_t	    alps_prs_i2c_handle;
    uint8_t                 resolution_adc;
    uint8_t                 out_of_bound_flag[HSPPAD_NUM_DATATYPES];
    sns_ddf_powerstate_e    alps_prs_powerstate;
    sns_ddf_timer_s         alps_prs_timer;
    bool                    alps_prs_timer_is_running;
    bool                    pressure_enable;
    bool                    temp_enable;
    sns_ddf_sensor_data_s   sensor_data[HSPPAD_NUM_DATATYPES];
    sns_ddf_sensor_sample_s sample_cache[HSPPAD_NUM_DATATYPES];
} alps_prs_dd_state_t;

const static sns_ddf_sensor_e   alps_prs_sensors[] = {
    SNS_DDF_SENSOR_PRESSURE,
    SNS_DDF_SENSOR_TEMP
};

void alps_dd_hsppad_log(
    alps_prs_dd_state_t*   dd_ptr,
    sns_ddf_sensor_e       sensor,
    uint32_t               data1)
{
#ifndef DDK  
    sns_err_code_e             status;
    sns_log_sensor_data_pkt_s* log;

    status = sns_logpkt_malloc(
        SNS_LOG_CONVERTED_SENSOR_DATA,
        sizeof(sns_log_sensor_data_pkt_s),
        (void**)&log);

    if ( (status == SNS_SUCCESS) && (log != NULL) )
    {
        log->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
        log->sensor_id = sensor;
        log->vendor_id = SNS_DDF_VENDOR_ALPS;
        log->timestamp = sns_ddf_get_timestamp();
        log->num_data_types = 1;
        log->data[0] = data1;
        sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, log);
    }
#endif
}

sns_ddf_status_e alps_dd_hsppad_set_mode(sns_ddf_handle_t dd_handle,
    sns_ddf_powerstate_e power_state)
{
    uint8_t count, buf[1];
    sns_ddf_status_e status;
    alps_prs_dd_state_t *state = dd_handle;

    switch (power_state) {
    case SNS_DDF_POWERSTATE_ACTIVE:
        buf[0] = CMD_ACT;

#ifdef DDK
            status = sns_ddf_write_port(state->alps_prs_i2c_handle,
                CMD_ACT, buf, 1, &count);
#else
        status = sns_ddf_simple_write_port(state->alps_prs_i2c_handle,
            (uint8_t*)&buf[0],
            1,
            &count);
#endif
            if(status != SNS_DDF_SUCCESS)
            {
            HSPPAD038A_MSG_1(ERROR, "sns_ddf_simple_write_port error=%d", status);
                return status;
            }

            state->alps_prs_powerstate = SNS_DDF_POWERSTATE_ACTIVE;
            status = SNS_DDF_SUCCESS;
        break;

    case SNS_DDF_POWERSTATE_LOWPOWER:
        state->alps_prs_powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
        status = SNS_DDF_SUCCESS;
        break;

    default:
        status = SNS_DDF_EINVALID_PARAM;
    }
    return status;
}

static sns_ddf_status_e alps_dd_hsppad_get_prs_tmp(sns_ddf_handle_t dd_handle)
{
    uint8_t count;
#ifdef DDK 
    uint8_t buf[ALPS_PRS_SENSOR_READ_DATA_NUM];
#endif

#ifndef DDK
    uint8_t write_buf[1];
#endif
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    alps_prs_dd_state_t *state = dd_handle;

    HSPPAD038A_MSG_0(HIGH, "alps_dd_hsppad_get_prs_tmp - enter");

    /* Start 10 ms measurement timer */
    if(!state->alps_prs_timer_is_running)
    {

#ifdef DDK
#warning "Caution: Abnormal operation for hsppad sensor."
    status = sns_ddf_read_port(state->alps_prs_i2c_handle,
        CMD_ACT, buf,
        ALPS_PRS_SENSOR_READ_DATA_NUM,
        &count);
#else
    write_buf[0] = CMD_ACT;
    status = sns_ddf_simple_write_port(state->alps_prs_i2c_handle,
        (uint8_t*)&write_buf[0],
        1,
        &count);
#endif

    if(status != SNS_DDF_SUCCESS)
    {
        HSPPAD038A_MSG_1(ERROR, "sns_ddf_simple_write_port error=%d", status);
        return status;
    }

        status = sns_ddf_timer_start(state->alps_prs_timer, TIMER_10_MS);
    if(status != SNS_DDF_SUCCESS)
    {
            HSPPAD038A_MSG_1(ERROR,"sns_ddf_timer_start error=%d", status);
        return status;
    }

        state->alps_prs_timer_is_running = true;
    }

    return status;
}


static sns_ddf_status_e alps_dd_hsppad_run_test(sns_ddf_handle_t dd_handle,
    sns_ddf_test_e test, uint32_t *err)
{
    sns_ddf_status_e status, status2;

    *err = 0;
    status = SNS_DDF_EFAIL;
    status2  = SNS_DDF_EFAIL;

    switch (test) {
    case SNS_DDF_TEST_SELF:         /**< Hardware self test. */
        status2 = SNS_DDF_EINVALID_TEST;
        break;
    case SNS_DDF_TEST_IRQ:          /**< Interrupt test. */
        status2 = SNS_DDF_EINVALID_TEST;
        break;
    case SNS_DDF_TEST_CONNECTIVITY:  /**< Basic connectivity test. */
        status2 = SNS_DDF_EINVALID_TEST;
        break;
    default:
        status = SNS_DDF_EINVALID_ATTR;
        break;
    }

    if (status == SNS_DDF_SUCCESS)
    {
        return status2;
    }
    else
    {
        return status;
    }
}

static sns_ddf_status_e alps_prs_init(
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
    alps_prs_dd_state_t *alps_state_ptr;

    *dd_handle_ptr = NULL;
    *sensors       = NULL;

    HSPPAD038A_MSG_0(HIGH, "alps_prs_init - enter");

    status = sns_ddf_malloc((void **)&alps_state_ptr,
        sizeof(alps_prs_dd_state_t));

    if (status != SNS_DDF_SUCCESS)
    {
        return SNS_DDF_ENOMEM;
    }

    memset(alps_state_ptr, 0, sizeof(alps_prs_dd_state_t));

    status = sns_ddf_open_port(&(alps_state_ptr->alps_prs_i2c_handle),
        &(device_info->port_config));

    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    /* parameter backup */
    alps_state_ptr->alps_prs_smgr_handle = smgr_handle;
    alps_state_ptr->alps_prs_num_devices = num_devices;

    /* init timer */
    status = sns_ddf_timer_init(&alps_state_ptr->alps_prs_timer, 
                                alps_state_ptr->alps_prs_smgr_handle, 
                                &sns_dd_prs_hsppad_if, 
                                alps_state_ptr, //always return dd_handle_ptr
                                false);
    
    if(status != SNS_DDF_SUCCESS)
    {
       return status;
    }

    /* return sensors parameter */
    *num_sensors   = HSPPAD_NUM_DATATYPES;
#ifdef DDK
    (*sensors)[0] = alps_prs_sensors[0];
#else
    *sensors  = (sns_ddf_sensor_e *)alps_prs_sensors;
#endif

    *dd_handle_ptr = alps_state_ptr;

#ifdef DDK_DEBUG
    printf("prs_init\r\n");
#endif

    HSPPAD038A_MSG_0(HIGH, "alps_prs_init - exit");

    return SNS_DDF_SUCCESS;
}



static sns_ddf_status_e alps_prs_get_data(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensors[],
    uint32_t num_sensors,
    sns_ddf_memhandler_s *memhandler,
    sns_ddf_sensor_data_s **data)
{
    uint8_t i;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    alps_prs_dd_state_t *state = dd_handle;

    /* check work memory */
    if (state == NULL)
    {
        return SNS_DDF_ENOMEM;
    }

    HSPPAD038A_MSG_1(HIGH, "alps_prs_get_data - enter num_sensors=%d", num_sensors);

    /* check powerstate */
    if (state->alps_prs_powerstate != SNS_DDF_POWERSTATE_ACTIVE)
    {
        HSPPAD038A_MSG_0(ERROR, "In low power");
        return SNS_DDF_EFAIL;
    }

    for (i = 0; i < num_sensors; i++) {
        switch (sensors[i]) {
        case SNS_DDF_SENSOR_PRESSURE:
            HSPPAD038A_MSG_0(MED, "SNS_DDF_SENSOR_PRESSURE");
            state->pressure_enable = true;
            break;

        case SNS_DDF_SENSOR_TEMP:
            HSPPAD038A_MSG_0(MED, "SNS_DDF_SENSOR_TEMP");
            state->temp_enable = true;
            break;

        default:
            HSPPAD038A_MSG_1(MED, "Unknown sensor=%d", sensors[i]);
            return SNS_DDF_EFAIL;
        }
            }

    if(state->pressure_enable || state->temp_enable)
    {
        /* Request data */
                status = alps_dd_hsppad_get_prs_tmp(state);
                if (status != SNS_DDF_SUCCESS)
                {
           HSPPAD038A_MSG_1(ERROR, "alps_dd_hsppad_get_prs_tmp error=%d", status);
                    return status;
                }
            }

    HSPPAD038A_MSG_0(HIGH, "alps_prs_get_data - exit");

    return SNS_DDF_PENDING;
}


static sns_ddf_status_e alps_prs_set_attrib(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    void *value)
{
    alps_prs_dd_state_t *state = dd_handle;

    HSPPAD038A_MSG_3(HIGH, "Set Attr - sensor=%d attrib=%d value=%d", sensor, attrib, *(uint8_t *)value);

    /* check work memory */
    if (state == NULL)
    {
        return SNS_DDF_ENOMEM;
    }

    switch (attrib) {
    case SNS_DDF_ATTRIB_POWER_STATE:
        {
            sns_ddf_powerstate_e *power_state = value;
            return alps_dd_hsppad_set_mode(dd_handle, *power_state);
        }
    default:
        return SNS_DDF_EINVALID_ATTR;
    }
}


static sns_ddf_status_e alps_prs_get_attrib(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_attribute_e attrib,
    sns_ddf_memhandler_s *memhandler,
    void **value,
    uint32_t *num_elems)
{
    alps_prs_dd_state_t *state = dd_handle;

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
            *value = (void *)&(state->alps_prs_powerstate);
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
            power->active_current   = ALPS_PRS_SENSOR_POWER_ACTIVE;
            power->lowpower_current = ALPS_PRS_SENSOR_LOWPOWER;
            *value     = power;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }
    case SNS_DDF_ATTRIB_RANGE:
        {
            switch (sensor) {
            case SNS_DDF_SENSOR_PRESSURE:
                {
                    sns_ddf_range_s *ranges;
                    ranges = sns_ddf_memhandler_malloc(
                        memhandler,
                        sizeof(sns_ddf_range_s));
                    if (ranges == NULL)
                    {
                        return SNS_DDF_ENOMEM;
                    }
                    ranges[0].min = ALPS_PRS_SENSOR_RANGE_MIN;
                    ranges[0].max = ALPS_PRS_SENSOR_RANGE_MAX;
                    *value     = ranges;
                    *num_elems = 1;
                    return SNS_DDF_SUCCESS;
                }
            case SNS_DDF_SENSOR_TEMP:
                {
                    sns_ddf_range_s *ranges;
                    ranges = sns_ddf_memhandler_malloc(
                        memhandler,
                        sizeof(sns_ddf_range_s));
                    if (ranges == NULL)
                    {
                        return SNS_DDF_ENOMEM;
                    }
                    ranges[0].min = ALPS_TMP_SENSOR_RANGE_MIN;
                    ranges[0].max = ALPS_TMP_SENSOR_RANGE_MAX;
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
            case SNS_DDF_SENSOR_PRESSURE:
                {
                    sns_ddf_resolution_t *res;
                    res = sns_ddf_memhandler_malloc(
                        memhandler, sizeof(sns_ddf_resolution_t));
                    if (res == NULL)
                    {
                        return SNS_DDF_ENOMEM;
                    }
                    *res = FX_FLTTOFIX_Q16(ALPS_PRS_SENSOR_RESOLUTION);
                    *value     = res;
                    *num_elems = 1;
                    return SNS_DDF_SUCCESS;
                }
            case SNS_DDF_SENSOR_TEMP:
                {
                    sns_ddf_resolution_t *res;
                    res = sns_ddf_memhandler_malloc(
                        memhandler, sizeof(sns_ddf_resolution_t));
                    if (res == NULL)
                    {
                        return SNS_DDF_ENOMEM;
                    }
                    *res = FX_FLTTOFIX_Q16(ALPS_TMP_SENSOR_RESOLUTION);
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
            case SNS_DDF_SENSOR_TEMP:
            case SNS_DDF_SENSOR_PRESSURE:
                {
                    sns_ddf_resolution_adc_s *res_adc;
                    res_adc = sns_ddf_memhandler_malloc(memhandler,
                        sizeof(sns_ddf_resolution_adc_s));
                    if (res_adc == NULL)
                    {
                        return SNS_DDF_ENOMEM;
                    }
                    res_adc[0].bit_len = ALPS_PRS_SENSOR_BIT_LEN;
                    res_adc[0].max_freq = ALPS_PRS_SENSOR_MAX_FREQ;
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
            delays->time_to_active = ALPS_PRS_SENSOR_DELAY_ACTIVE;
            delays->time_to_data   = ALPS_PRS_SENSOR_DELAY_DATA;
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
            info->name    = ALPS_PRS_SENSOR_DRIVER_NAME;
            info->version = ALPS_PRS_SENSOR_DRIVER_VER;
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
            info->name    = ALPS_PRS_SENSOR_DEVICE_NAME;
            info->vendor  = ALPS_PRS_SENSOR_DEVICE_VENDOR;
            info->model   = ALPS_PRS_SENSOR_DEVICE_MODEL;
            info->version = ALPS_PRS_SENSOR_DEVICE_VER;
            *value     = info;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }
    default:
        return SNS_DDF_EINVALID_ATTR;
    }
    return SNS_DDF_SUCCESS;
}

static void alps_prs_handle_timer(
    sns_ddf_handle_t dd_handle,
    void *arg)
{
    alps_prs_dd_state_t *state = (alps_prs_dd_state_t *)arg;
    uint8_t count, buf[ALPS_PRS_SENSOR_READ_DATA_NUM], flg_ob;
    sns_ddf_status_e status;
    uint16_t data_tmp;
    sns_ddf_time_t timestamp = sns_ddf_get_timestamp();
    uint8_t num_sensor_data = 0;
    
    /* check work memory */
    if (state == NULL)
    {
        return;
}

    HSPPAD038A_MSG_0(MED, "alps_prs_handle_timer - enter");
       
#ifndef DDK
    status = sns_ddf_simple_read_port(state->alps_prs_i2c_handle,
        (uint8_t*)&buf[0],
        ALPS_PRS_SENSOR_READ_DATA_NUM,
        &count);
#else
    #warning "Caution: Abnormal operation for hsppad sensor."
    status = sns_ddf_read_port(state->alps_prs_i2c_handle,
        CMD_ACT, buf,
        ALPS_PRS_SENSOR_READ_DATA_NUM,
        &count);
#endif

    state->alps_prs_timer_is_running = false;

    if(status != SNS_DDF_SUCCESS)
    {
        HSPPAD038A_MSG_1(ERROR, "sns_ddf_simple_read_port error=%d", status);
    }

    HSPPAD038A_MSG_3(MED, "status=%02X pres_msb=%02X pres_lsb=%02X", buf[0], buf[1], buf[2]);
    HSPPAD038A_MSG_2(MED, "temp_msb=%02X temp_lsb=%02X", buf[3], buf[4]);

#if defined(DDK) && defined(DDK_DEBUG)
    printf("press_data, %02X, %02X, %02X, %02X, %02X\r\n",
                buf[0], buf[1], buf[2], buf[3], buf[4]);
#endif

#if defined(DDK) && defined(DDK_DEBUG)
    if ((buf[0] & 0xFE) != 0x60)
#else
    if ((buf[0] & 0xFE) != 0x40)
#endif
    {
        HSPPAD038A_MSG_1(ERROR, "status bit error=0x%02X", buf[0]);
    }

    flg_ob = 0;
    data_tmp = ((((uint16_t)buf[1])<<8) | ((uint16_t)buf[2]));
    if ((data_tmp == 0x0000) || (data_tmp == 0xffff))
    {
        flg_ob = 1;
    }
    state->sample_cache[HSPPAD_PRS].sample =
        FX_FLTTOFIX_Q16(data_tmp * ALPS_PRS_SENSOR_RESOLUTION
        + ALPS_PRS_SENSOR_INTERCEPT);
    state->out_of_bound_flag[HSPPAD_PRS] = flg_ob;

    flg_ob = 0;
    data_tmp = ((((uint16_t)buf[3])<<8) | ((uint16_t)buf[4]));
    if ((data_tmp == 0x0000) || (data_tmp == 0xffff))
    {
        flg_ob = 1;
    }
    state->sample_cache[HSPPAD_TMP].sample =
        FX_FLTTOFIX_Q16(data_tmp * ALPS_TMP_SENSOR_RESOLUTION
        + ALPS_TMP_SENSOR_INTERCEPT);
    state->out_of_bound_flag[HSPPAD_TMP] = flg_ob;
 
    if(state->pressure_enable)
    {
        /* Populate pressure data */
        state->sample_cache[HSPPAD_PRS].status = (sns_ddf_status_e)(
                                        state->out_of_bound_flag[HSPPAD_PRS] *
                                        SNS_DDF_EDATA_BOUND);
        state->sensor_data[num_sensor_data].sensor = SNS_DDF_SENSOR_PRESSURE;
        state->sensor_data[num_sensor_data].samples = &state->sample_cache[HSPPAD_PRS];
        state->sensor_data[num_sensor_data].status = (sns_ddf_status_e)(
                                        state->out_of_bound_flag[HSPPAD_PRS] *
                                        SNS_DDF_EDATA_BOUND);
        state->sensor_data[num_sensor_data].timestamp = timestamp;
        state->sensor_data[num_sensor_data].num_samples = 1;

        num_sensor_data++;
        state->pressure_enable = false;

        alps_dd_hsppad_log(state, SNS_DDF_SENSOR_PRESSURE, (uint32_t)(state->sample_cache[HSPPAD_PRS].sample));

    }

    if(state->temp_enable)
    {

        /* Populate temp data */
        state->sample_cache[HSPPAD_TMP].status = (sns_ddf_status_e)(
                                                 state->out_of_bound_flag[HSPPAD_TMP] *
                                                 SNS_DDF_EDATA_BOUND);
        state->sensor_data[num_sensor_data].sensor = SNS_DDF_SENSOR_TEMP;
        state->sensor_data[num_sensor_data].samples = &state->sample_cache[HSPPAD_TMP];
        state->sensor_data[num_sensor_data].status = (sns_ddf_status_e)(
                                                 state->out_of_bound_flag[HSPPAD_TMP] *
                                                 SNS_DDF_EDATA_BOUND);
        state->sensor_data[num_sensor_data].timestamp = timestamp;
        state->sensor_data[num_sensor_data].num_samples = 1;

        num_sensor_data++;
        state->temp_enable = false;

        alps_dd_hsppad_log(state, SNS_DDF_SENSOR_TEMP, (uint32_t)(state->sample_cache[HSPPAD_TMP].sample));

    }
    
    if(num_sensor_data != 0)
    {
        sns_ddf_smgr_notify_data(state->alps_prs_smgr_handle, state->sensor_data, num_sensor_data);
    }   

    HSPPAD038A_MSG_0(MED, "alps_prs_handle_timer - exit");    
}

#ifdef DDK
static void alps_prs_handle_irq(sns_ddf_handle_t dd_handle,
    sns_ddf_signal_irq_e irq)
#else
static void alps_prs_handle_irq(sns_ddf_handle_t dd_handle,
    uint32_t gpio_num,
    sns_ddf_time_t timestamp)
#endif

{
    alps_prs_dd_state_t *state = dd_handle;

    /* check work memory */
    if (state == NULL)
        return;
}

static sns_ddf_status_e alps_prs_reset(sns_ddf_handle_t dd_handle)
{
    alps_prs_dd_state_t *state = dd_handle;

    /* check work memory */
    if (state == NULL)
    {
        return SNS_DDF_ENOMEM;
    }

    state->pressure_enable = false;
    state->temp_enable     = false;
    state->alps_prs_timer_is_running = false;

    return SNS_DDF_SUCCESS;
}

static sns_ddf_status_e alps_prs_run_test(
    sns_ddf_handle_t dd_handle,
    sns_ddf_sensor_e sensor,
    sns_ddf_test_e test,
    uint32_t *err)
{
    sns_ddf_status_e status;
    alps_prs_dd_state_t *state = dd_handle;

    /* check work memory */
    /*testtesttest*/
    if (state == NULL)
    {
        return SNS_DDF_ENOMEM;
    }

    status = alps_dd_hsppad_run_test(state, test, err);
    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    return SNS_DDF_SUCCESS;
}

#ifndef DDK
static sns_ddf_status_e alps_prs_run_probe
    (
    sns_ddf_device_access_s* device_info,
    sns_ddf_memhandler_s*    memhandler,
    uint32_t*                num_sensors,
    sns_ddf_sensor_e**       sensors
    )
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;
    sns_ddf_handle_t port_handle;
    uint8_t          write_buf[1];
    uint8_t          count;
    uint8_t          buf[ALPS_PRS_SENSOR_READ_DATA_NUM];

    *num_sensors = 0;
    *sensors = NULL;

    status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
    if (status != SNS_DDF_SUCCESS) 
    {
        return status;
    }

    /* There is no ID register for this part. 
    We will use status register to verify if the part is present. */
    write_buf[0] = CMD_ACT;
    status = sns_ddf_simple_write_port(port_handle,
        (uint8_t*)&write_buf[0],
        1,
        &count);

    if(status != SNS_DDF_SUCCESS)
    {
        HSPPAD038A_MSG_1(ERROR, "sns_ddf_simple_write_port error=%d", status);
        sns_ddf_close_port(port_handle);
        return status;
    }

    // 10 ms delay before data is read
    sns_ddf_delay(10000);

    status = sns_ddf_simple_read_port(port_handle,
        (uint8_t*)&buf[0],
        ALPS_PRS_SENSOR_READ_DATA_NUM,
        &count);
    if(status != SNS_DDF_SUCCESS)
    {
        HSPPAD038A_MSG_1(ERROR, "sns_ddf_simple_read_port error=%d", status);
        sns_ddf_close_port(port_handle);
        return status;
    }

    /* Status register after a good read is 0x40 */
    if ((buf[0] & 0xFE) != 0x40)
    {
        sns_ddf_close_port(port_handle);
        return SNS_DDF_SUCCESS;
    }

    *num_sensors = HSPPAD_NUM_DATATYPES;
    *sensors = sns_ddf_memhandler_malloc( memhandler,
        sizeof(sns_ddf_sensor_e) * *num_sensors );

    if (*sensors != NULL)
    {
        (*sensors)[0] = SNS_DDF_SENSOR_PRESSURE;   
        (*sensors)[1] = SNS_DDF_SENSOR_TEMP;   
        status = SNS_DDF_SUCCESS;
    } 
    else 
    {
        status = SNS_DDF_ENOMEM;
    } 

    sns_ddf_close_port( port_handle );
    return status;
}
#endif // DDK

sns_ddf_driver_if_s sns_dd_prs_hsppad_if = {
    &alps_prs_init,
    &alps_prs_get_data,
    &alps_prs_set_attrib,
    &alps_prs_get_attrib,
    &alps_prs_handle_timer,
    &alps_prs_handle_irq,
    &alps_prs_reset,
    &alps_prs_run_test,
#ifndef DDK
    NULL,
    &alps_prs_run_probe
#endif
};
