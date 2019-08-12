/**
 * @file sns_dd_daf_playback_accel.c
 * @brief DAF playback driver for Accel.
 *
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_daf_playback_accel.c#1 $
  $DateTime: 2016/12/13 08:00:23 $

  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2015-04-09 pn   Replaced calls to sns_em_get_timestamp64() with sns_ddf_get_timestamp()
  2015-01-15 SH   Initial revision
==============================================================================*/


#include <stdbool.h>
#include <stdio.h>
#include "sns_dd_daf_playback_accel_priv.h"
#include "driver_access_framework.h"
#include "fixed_point.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"

// Forward declaration of driver interface functions.
sns_ddf_driver_if_s sns_dd_daf_playback_accel_if;

static const sns_ddf_sensor_e sensor_instance = DAF_PLAYBACK_SENSOR_TYPE;

#define DAF_PLAYBACK_DEFAULT_ODR    250 /* Hz*/

// Sensitivity, Range, & Filter attribute values.
// These are fixed for the playback driver, so there is only one setting.
// Select the highest values out of existing sensors.
static const sns_ddf_resolution_t sns_dd_daf_playback_accel_sensitivity =
    FX_FLTTOFIX_Q16(G / (1024 * 8 * 2));

static const sns_ddf_range_s sns_dd_daf_playback_accel_range =
    { FX_FLTTOFIX_Q16(-16 * G), FX_FLTTOFIX_Q16(16 * G) };

static const q16_t sns_dd_daf_playback_accel_lpf =
    1 << 16; // value 1.0 hz 


static sns_ddf_status_e schedule_next_sample(
    sns_dd_daf_playback_accel_s* state);

static sns_ddf_status_e get_next_sample(
    sns_dd_daf_playback_accel_s* state,
    sns_ddf_sensor_data_s* data);

static void fill_dummy_data(
    sns_ddf_sensor_data_s* data);

static void log_data(
    sns_ddf_sensor_data_s* data_item);


/** 
 * @brief Retrieves the value of an attribute for a sensor.
 * @see sns_ddf_driver_if_s.get_attrib()
 */
sns_ddf_status_e sns_dd_daf_playback_accel_get_attr(
    sns_ddf_handle_t      dd_handle,
    sns_ddf_sensor_e      sensor,
    sns_ddf_attribute_e   attrib,
    sns_ddf_memhandler_s* memhandler,
    void**                value,
    uint32_t*             num_elems)
{
    sns_dd_daf_playback_accel_s* state = (sns_dd_daf_playback_accel_s*)dd_handle;

    if (dd_handle == NULL || value == NULL)
        return SNS_DDF_EINVALID_PARAM;

    switch (attrib)
    {
        case SNS_DDF_ATTRIB_POWER_INFO:
        {
            sns_ddf_power_info_s* power_attrib = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_power_info_s));
            if (power_attrib == NULL)
                return SNS_DDF_ENOMEM;

            power_attrib->active_current   = 0;
            power_attrib->lowpower_current = 0;

            *value     = power_attrib;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RANGE:
        {
            *value     = (void *)&sns_dd_daf_playback_accel_range;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RESOLUTION:
        {
            *value     = (void *)&sns_dd_daf_playback_accel_sensitivity;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_LOWPASS:
        {
            *value     = (void *)&sns_dd_daf_playback_accel_lpf;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_ODR:
        {
            DAF_PLAYBACK_MSG_1(HIGH, "get odr =%d", state->current_odr);
            *value     = &state->current_odr;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
        {
            uint16_t i = 0;
            uint16_t count = MIN( DAF_PLAYBACK_DEFAULT_ODR / 10, SNS_SMGR_MAX_SUPPORTED_ODR_NUM_V01 );
            sns_ddf_odr_t *odr_list = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_odr_t) * count);
            if (odr_list == NULL)
                return SNS_DDF_ENOMEM;

            // Support ODRs from 0 to DEFAULT_ODR, by 10. Going by 1 is impractical
            // because of the SNS_SMGR_MAX_SUPPORTED_ODR_NUM_V01 limit.
            for ( i = 0; i < count; i++ )
            {
                odr_list[i] = (i + 1) * 10;
            }

            *value     = odr_list;
            *num_elems = count;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DELAYS:
        {
            sns_ddf_delays_s *delays = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_delays_s));
            if (delays == NULL)
                return SNS_DDF_ENOMEM;

            delays->time_to_active = 0;
            delays->time_to_data   = 0;

            *value     = delays;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_RESOLUTION_ADC:
        {
            sns_ddf_resolution_adc_s *device_res = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_resolution_adc_s));
            if (device_res == NULL)
                return SNS_DDF_ENOMEM;

            device_res->bit_len  = 16;
            device_res->max_freq = state->recorded_odr;

            *value     = device_res;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DRIVER_INFO:
        {
            sns_ddf_driver_info_s* driver_info = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_driver_info_s));
            if (driver_info == NULL)
               return SNS_DDF_ENOMEM;

            driver_info->name    = "Qualcomm DAF_PLAYBACK Driver";
            driver_info->version = 1;

            *value     = driver_info;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_DEVICE_INFO:
        {
            sns_ddf_device_info_s* device_info = sns_ddf_memhandler_malloc(
                memhandler, sizeof(sns_ddf_device_info_s));
            if (device_info == NULL)
               return SNS_DDF_ENOMEM;

            device_info->name    = "Accelerometer";
            device_info->vendor  = "Qualcomm";
            device_info->model   = "DAF_PLAYBACK";
            device_info->version = 1;

            *value     = device_info;
            *num_elems = 1;
            return SNS_DDF_SUCCESS;
        }

        default:
        {
            return SNS_DDF_EINVALID_ATTR;
        }
    }
}


/**
 * @brief Sets a sensor attribute to a specific value.
 * @see sns_ddf_driver_if_s.set_attrib()
 */
sns_ddf_status_e sns_dd_daf_playback_accel_set_attr(
    sns_ddf_handle_t    dd_handle,
    sns_ddf_sensor_e    sensor,
    sns_ddf_attribute_e attrib,
    void*               value)
{
    sns_dd_daf_playback_accel_s* state = (sns_dd_daf_playback_accel_s*)dd_handle;

    if (dd_handle == NULL || value == NULL)
        return SNS_DDF_EINVALID_PARAM;

    switch (attrib)
    {
        case SNS_DDF_ATTRIB_POWER_STATE:
        case SNS_DDF_ATTRIB_RANGE:
        case SNS_DDF_ATTRIB_LOWPASS:
        {
            // These values are fixed
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_ODR:
        {
            if (0 == *(sns_ddf_odr_t*)value)
            {
                state->current_odr = 0;
            }
            else
            {
                // Set current ODR to match recorded ODR. Not requested value.
                state->current_odr = state->recorded_odr;
            }
            DAF_PLAYBACK_MSG_2(HIGH, "set odr req=%d cur=%d", *(sns_ddf_odr_t*)value, state->current_odr);
            return SNS_DDF_SUCCESS;
        }

        case SNS_DDF_ATTRIB_MOTION_DETECT:
        {
            state->md_enabled = *(sns_ddf_motion_detect_enable_t *)value;

            if (state->sched_enabled || state->md_enabled)
            {
                // Start timer
                schedule_next_sample(state);
            }
            else
            {
                // Stop timer
                sns_ddf_timer_cancel(state->timer);
            }

            DAF_PLAYBACK_MSG_2(HIGH, "set md_enabled=%d (sched_enabled=%d)",
                state->md_enabled, state->sched_enabled);
            return SNS_DDF_SUCCESS;
        }

        default:
        {
            return SNS_DDF_EINVALID_ATTR;
        }
    }
}


/**
 * @brief Begins device-scheduled sampling and enables notification via Data 
 *        Ready Interrupts (DRI).
 * @see sns_ddf_driver_if_s.reset()
*/
sns_ddf_status_e sns_dd_daf_playback_accel_enable_sched_data(
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    bool              enable)
{
    sns_dd_daf_playback_accel_s* state = (sns_dd_daf_playback_accel_s*)dd_handle;
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    state->sched_enabled = enable;

    if (state->sched_enabled || state->md_enabled)
    {
        // Start timer
        schedule_next_sample(state);
    }
    else
    {
        // Stop timer
        sns_ddf_timer_cancel(state->timer);
    }

    DAF_PLAYBACK_MSG_2(HIGH, "set sched_enabled=%d (md_enabled=%d)",
        state->sched_enabled, state->md_enabled);
    return status;
}


/**
 * @brief Starts a timer to expire at the timestamp of the next sample.
 */
static sns_ddf_status_e schedule_next_sample(
    sns_dd_daf_playback_accel_s* state)
{
    sns_ddf_status_e status;
    uint32_t next_sample_usec;
    int64_t diff_ticks;
    sns_ddf_time_t current_time;

    if (state->read_index == state->write_index)
    {
        // There are no more samples
        DAF_PLAYBACK_MSG_0(MED, "scheduling dummy data");
        if(state->current_odr != 0)
        {
            status = sns_ddf_timer_start(state->timer, 1000000 / state->current_odr);
        }
        else
        {
            status = sns_ddf_timer_start(state->timer, 1000000 / DAF_PLAYBACK_DEFAULT_ODR);
        }
        return SNS_DDF_EDATA_BOUND;
    }

    // Compare the next sample to the current time
    current_time = sns_ddf_get_timestamp();
    diff_ticks = state->data_cache[state->read_index].timestamp - current_time;
    if (diff_ticks <= 0)
    {
        next_sample_usec = 0;
    }
    else if (diff_ticks >= sns_em_convert_usec_to_dspstick(100000))
    {
        // If next time is more than 100 ms in the future, repeat the previous
        // sample after 100ms. This is done because if there are large gaps in
        // the accel data, some algorithms will stop functioning.
        // This value also corresponds to our lowest supported ODR of 10Hz.
        next_sample_usec = 100000;
    }
    else
    {
        next_sample_usec = sns_em_convert_dspstick_to_usec((uint32_t)diff_ticks);
    }

    DAF_PLAYBACK_MSG_2(LOW, "schedule wr %d rd %d", state->write_index, state->read_index);
    DAF_PLAYBACK_MSG_2(LOW, "schedule time %d now %d",
        (uint32_t)state->data_cache[state->read_index].timestamp, (uint32_t)current_time);
    DAF_PLAYBACK_MSG_1(LOW, "schedule diff %d usec", next_sample_usec);

    status = sns_ddf_timer_start(state->timer, next_sample_usec);

    return status;
}


/**
* @brief Called when the timer set by this driver has expired. This must be
*        the callback function submitted when initializing a timer.
* @see sns_ddf_driver_if_s.handle_timer()
*/
void sns_dd_daf_playback_accel_handle_timer(
    sns_ddf_handle_t dd_handle,
    void*            arg)
{
    sns_dd_daf_playback_accel_s* state = (sns_dd_daf_playback_accel_s*)dd_handle;

    DAF_PLAYBACK_MSG_3(LOW, "handle_timer start wr %d rd %d enabled %d",
        state->write_index, state->read_index, state->sched_enabled);

    if (state->md_enabled)
    {
        DAF_PLAYBACK_MSG_0(HIGH, "handle_timer sending SNS_DDF_EVENT_MOTION");

        // If motion event was requested, then send it (regardless of if actual
        // motion was detected or not).
        // TODO - should we analyze data and only send it we determine there was motion?
        sns_ddf_smgr_notify_event(state->smgr_handle,
                                    DAF_PLAYBACK_SENSOR_TYPE,
                                    SNS_DDF_EVENT_MOTION);
    }

    if (state->sched_enabled)
    {
        if (state->read_index == state->write_index)
        {
            // Send dummy data if no data is available in the cache
            fill_dummy_data(&state->sensor_data);

            sns_ddf_smgr_notify_data(state->smgr_handle, &state->sensor_data, 1);
            log_data(&state->sensor_data);
        }
        else
        {
            sns_ddf_status_e status = get_next_sample(state, &state->sensor_data);

            if (status != SNS_DDF_SUCCESS)
            {
                // No data is available yet. Resend previous sample with updated timestamp.
                state->sensor_data.timestamp = sns_ddf_get_timestamp();

                sns_ddf_smgr_notify_data(state->smgr_handle, &state->sensor_data, 1);
                log_data(&state->sensor_data);
            }
            else
            {
                do
                {
                    // Send up all current samples
                    sns_ddf_smgr_notify_data(state->smgr_handle, &state->sensor_data, 1);
                    log_data(&state->sensor_data);
                } while (SNS_DDF_SUCCESS == get_next_sample(state, &state->sensor_data));
            }
        }
    }

    if (state->md_enabled || state->sched_enabled)
    {
        // Schedule next timer expiration
        schedule_next_sample(state);
    }

    DAF_PLAYBACK_MSG_2(LOW, "handle_timer end wr %d rd %d",
        state->write_index, state->read_index);
}


/**
* @brief Called to get the next sample from the data_cache.
*
* @return SNS_DDF_EDATA_BOUND if there are no more samples, or
*         if the timestamp on the next sample is in the future.
*/
static sns_ddf_status_e get_next_sample(
    sns_dd_daf_playback_accel_s* state,
    sns_ddf_sensor_data_s* data)
{
    sns_ddf_time_t current_time;

    if (state->read_index == state->write_index)
    {
        // There are no new samples to read
        return SNS_DDF_EDATA_BOUND;
    }

    current_time = sns_ddf_get_timestamp();
    if (state->data_cache[state->read_index].timestamp > current_time)
    {
        // Timestamp is in the future. Don't report it yet.
        return SNS_DDF_EDATA_BOUND;
    }

    // Copy over one sample from the data_cache
    data->status            = SNS_DDF_SUCCESS;
    data->sensor            = DAF_PLAYBACK_SENSOR_TYPE;
    data->timestamp         = (sns_ddf_time_t)state->data_cache[state->read_index].timestamp;
    data->num_samples       = DAF_PLAYBACK_NUM_AXES;
    data->samples[0].sample = state->data_cache[state->read_index].sample[0];
    data->samples[1].sample = state->data_cache[state->read_index].sample[1];
    data->samples[2].sample = state->data_cache[state->read_index].sample[2];
    data->samples[0].status = SNS_DDF_SUCCESS;
    data->samples[1].status = SNS_DDF_SUCCESS;
    data->samples[2].status = SNS_DDF_SUCCESS;

    // Update read index
    state->read_index = (state->read_index + 1) % DAF_PLAYBACK_MAX_NUM_SAMPLES;

    return SNS_DDF_SUCCESS;
}


/**
* @brief Gets one sample of dummy data.
*/
static void fill_dummy_data(
    sns_ddf_sensor_data_s* data)
{
    data->status            = SNS_DDF_SUCCESS;
    data->sensor            = DAF_PLAYBACK_SENSOR_TYPE;
    data->timestamp         = sns_ddf_get_timestamp(); // Current time
    data->num_samples       = DAF_PLAYBACK_NUM_AXES;
    data->samples[0].sample = 0;
    data->samples[1].sample = 0;
    data->samples[2].sample = FX_FLTTOFIX_Q16(-9.81);
    data->samples[0].status = SNS_DDF_SUCCESS;
    data->samples[1].status = SNS_DDF_SUCCESS;
    data->samples[2].status = SNS_DDF_SUCCESS;
}


/**
* @brief Called to save a set of samples to the data cache.
*
* @return Number of samples saved
*/
static int8_t save_samples_to_cache(
    sns_dd_daf_playback_accel_s* state,
    sns_daf_playback_inject_data_3axis_req* req)
{
    uint64_t base_timestamp = state->start_time + (uint64_t)req->base_timestamp;
    int8_t num_samples = 0;

    DAF_PLAYBACK_MSG_2(LOW, "save start wr %d rd %d", state->write_index, state->read_index);

    while ((((state->write_index + 1) % DAF_PLAYBACK_MAX_NUM_SAMPLES) != state->read_index) &&
        (num_samples < req->data_array_len))
    {
        // Copy over one sample to the data_cache
        state->data_cache[state->write_index].sample[0] = req->data_array[num_samples].data[0];
        state->data_cache[state->write_index].sample[1] = req->data_array[num_samples].data[1];
        state->data_cache[state->write_index].sample[2] = req->data_array[num_samples].data[2];
        state->data_cache[state->write_index].timestamp = (uint64_t)req->data_array[num_samples].timestamp_offset
                                                        + base_timestamp;

        // Update write index
        state->write_index = (state->write_index + 1) % DAF_PLAYBACK_MAX_NUM_SAMPLES;
        num_samples++;
    }

    DAF_PLAYBACK_MSG_2(LOW, "save end wr %d rd %d", state->write_index, state->read_index);

    return num_samples;
}


static void log_data(
    sns_ddf_sensor_data_s* data_item)
{
    sns_err_code_e err_code;
    sns_log_sensor_data_pkt_s* log;
    uint8_t i;

    err_code = sns_logpkt_malloc(
        SNS_LOG_CONVERTED_SENSOR_DATA,
        sizeof(sns_log_sensor_data_pkt_s),
        (void**)&log);
    if (err_code != SNS_SUCCESS || log == NULL)
    {
        // Can't allocate log packet. Ignore silently.
        return;
    }

    log->version        = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log->sensor_id      = DAF_PLAYBACK_SENSOR_TYPE;
    log->vendor_id      = (uint8_t)-1; // We don't know which sensor recorded the data
    log->timestamp      = data_item->timestamp;
    log->num_data_types = DAF_PLAYBACK_NUM_AXES;

    for (i = 0; i < data_item->num_samples; i++)
    {
        log->data[i] = data_item->samples[i].sample;
    }

    // Commit and free the log packet.
    sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, log);
}


/**
 * @brief Delivers a Driver Access Framework message to the driver.
 * @see sns_ddf_driver_if_s.process_daf_req()
 */
sns_ddf_status_e sns_dd_daf_playback_accel_process_daf_req(
    sns_ddf_handle_t       dd_handle,
    uint32_t               req_id,
    const void*            req_msg,
    uint32_t               req_size,
    sns_ddf_memhandler_s*  memhandler,
    void**                 resp_msg,
    uint32_t*              resp_size,
    const uint8_t*         trans_id_ptr,
    void*                  conn_handle)
{
    sns_dd_daf_playback_accel_s* state = (sns_dd_daf_playback_accel_s*)dd_handle;

    if (dd_handle == NULL)
        return SNS_DDF_EINVALID_PARAM;

    DAF_PLAYBACK_MSG_1(HIGH, "process_daf_req: req_id %d", req_id);

    // All of the RTT message processing happens in this switch statement
    switch (req_id)
    {
        case SNS_DAF_PLAYBACK_GET_CAPABILITIES:
        {
            sns_daf_playback_get_capabilities_resp* resp_msg_ptr;

            // Allocate the response message
            resp_msg_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_daf_playback_get_capabilities_resp));
            if (resp_msg_ptr == NULL)
            {
                return SNS_DDF_ENOMEM;
            }

            // Fill the response
            // -1 because read/write index logic prevents buffer for ever being completely full
            resp_msg_ptr->max_buffered_samples = DAF_PLAYBACK_MAX_NUM_SAMPLES - 1;

            *resp_msg = resp_msg_ptr;
            *resp_size = sizeof(sns_daf_playback_get_capabilities_resp);
            return SNS_DDF_SUCCESS;
        }

        case SNS_DAF_PLAYBACK_START_OF_NEW_DATA:
        {
            sns_daf_playback_start_of_new_data_req* req_msg_ptr = (sns_daf_playback_start_of_new_data_req*)req_msg;
            uint32_t nominal_odr_hz;

            // Validate the request message
            if ((req_msg == NULL) || (req_size != sizeof(sns_daf_playback_start_of_new_data_req)))
            {
                DAF_PLAYBACK_MSG_3(ERROR, "process_daf_req: Invalid DAF Request Message. req_msg 0x%x"
                                   "req_size %d (expected %d)",
                                   req_msg, req_size, sizeof(sns_daf_playback_start_of_new_data_req));
                return SNS_DDF_EINVALID_PARAM;
            }

            nominal_odr_hz = 1000000 / req_msg_ptr->nominal_odr_period;

            // Round up to multiple of 10
            nominal_odr_hz = ((nominal_odr_hz + 9) / 10) * 10;

            DAF_PLAYBACK_MSG_2(HIGH, "Start of New Data, ODR %d => %d",
                               state->recorded_odr,
                               nominal_odr_hz);

            // Set the new ODR
            if(state->recorded_odr != nominal_odr_hz)
            {
                state->recorded_odr = nominal_odr_hz;
                if(state->current_odr != 0)
                {
                    state->current_odr = state->recorded_odr;
                }

                // Notify SMGR when ODR changes
                sns_ddf_smgr_notify_event(state->smgr_handle,
                                          DAF_PLAYBACK_SENSOR_TYPE,
                                          SNS_DDF_EVENT_ODR_CHANGED);
            }

            // Save current time as start time
            state->start_time = sns_ddf_get_timestamp();

            // Empty the data cache because this is the start of a new series of data
            // Set indices equal to denote empty buffer
            state->write_index = 0;
            state->read_index  = 0;

            // There is no defined response
            *resp_size = 0;
            return SNS_DDF_SUCCESS;
        }

        case SNS_DAF_PLAYBACK_INJECT_PRIMARY_DATA_3AXIS:
        {
            sns_daf_playback_inject_data_3axis_req*  req_msg_ptr = (sns_daf_playback_inject_data_3axis_req*)req_msg;
            sns_daf_playback_inject_data_3axis_resp* resp_msg_ptr;

            // Validate the request message
            if ((req_msg == NULL) || (req_size != sizeof(sns_daf_playback_inject_data_3axis_req)))
            {
                DAF_PLAYBACK_MSG_3(ERROR, "process_daf_req: Invalid DAF Request Message. req_msg 0x%x"
                                   "req_size %d (expected %d)",
                                   req_msg, req_size, sizeof(sns_daf_playback_inject_data_3axis_req));
                return SNS_DDF_EINVALID_PARAM;
            }

            // Allocate the response message
            resp_msg_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_daf_playback_inject_data_3axis_resp));
            if (resp_msg_ptr == NULL)
            {
                return SNS_DDF_ENOMEM;
            }

            // If scheduled data is not enabled, then remove any old samples
            // to make room for the new ones.
            if ((state->sched_enabled == FALSE) && (state->md_enabled == FALSE))
            {
                sns_ddf_time_t current_time;
                current_time = sns_ddf_get_timestamp();

                while ((state->read_index != state->write_index) && // data cache is not empty
                       (state->data_cache[state->read_index].timestamp < current_time)) // timestamp is in the past
                {
                    DAF_PLAYBACK_MSG_3(HIGH, "removing rd %d time %d now %d",
                        state->read_index, (uint32_t)state->data_cache[state->read_index].timestamp, (uint32_t)current_time);

                    state->read_index = (state->read_index + 1) % DAF_PLAYBACK_MAX_NUM_SAMPLES;
                }
            }

            // Save new samples to data cache
            resp_msg_ptr->num_samples_saved = save_samples_to_cache(state, req_msg_ptr);

            // If any samples were saved, schedule next timer expiration
            if (resp_msg_ptr->num_samples_saved)
            {
                schedule_next_sample(state);
            }

            *resp_msg = resp_msg_ptr;
            *resp_size = sizeof(sns_daf_playback_inject_data_3axis_resp);
            return SNS_DDF_SUCCESS;
        }

        default:
        {
            // Unsupported DAF request
            DAF_PLAYBACK_MSG_1(HIGH, "unsupported request id %d", req_id);
            *resp_size = 0;
            return SNS_DDF_EINVALID_DAF_REQ;
        }
    }
}


/**
* @brief Retrieves a single set of sensor data.
* @see sns_ddf_driver_if_s.get_data()
*/
sns_ddf_status_e sns_dd_daf_playback_accel_get_data(
    sns_ddf_handle_t       dd_handle,
    sns_ddf_sensor_e       sensors[],
    uint32_t               num_sensors,
    sns_ddf_memhandler_s*  memhandler,
    sns_ddf_sensor_data_s* data[])
{
    sns_dd_daf_playback_accel_s* state = (sns_dd_daf_playback_accel_s*)dd_handle;

    if (dd_handle == NULL)
        return SNS_DDF_EINVALID_PARAM;

    // Playback driver should always self-schedule.
    if (state->sched_enabled)
    {
        return SNS_DDF_PENDING;
    }
    else
    {
        return SNS_DDF_EFAIL;
    }
}


/** 
 * @brief Probes for the device with a given configuration.
 * @see sns_ddf_driver_if_s.init()
 */
sns_ddf_status_e sns_dd_daf_playback_accel_probe(
    sns_ddf_device_access_s* device_info,
    sns_ddf_memhandler_s*    memhandler,
    uint32_t*                num_sensors,
    sns_ddf_sensor_e**       sensors )
{
    *sensors     = (sns_ddf_sensor_e *)&sensor_instance;
    *num_sensors = 1;
    return SNS_DDF_SUCCESS;
}


/**
* @brief Sets the device to its default operational state
* @see sns_ddf_driver_if_s.reset()
*/
sns_ddf_status_e sns_dd_daf_playback_accel_reset(sns_ddf_handle_t dd_handle)
{
    return SNS_DDF_SUCCESS;
}


/**
 * @brief Initializes the driver and sets up devices.
 * @see sns_ddf_driver_if_s.init()
 */
sns_ddf_status_e sns_dd_daf_playback_accel_init(
    sns_ddf_handle_t*       dd_handle_ptr,
    sns_ddf_handle_t        smgr_handle,
    sns_ddf_nv_params_s*    nv_params,
    sns_ddf_device_access_s device_info[],
    uint32_t                num_devices,
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_e*       sensors[],
    uint32_t*               num_sensors)
{
    sns_ddf_status_e status;
    sns_dd_daf_playback_accel_s* state = NULL;

    if (dd_handle_ptr == NULL || num_sensors == NULL || sensors == NULL)
        return SNS_DDF_EINVALID_PARAM;
   
    status = sns_ddf_malloc((void**)&state, sizeof(sns_dd_daf_playback_accel_s));
    if (status != SNS_DDF_SUCCESS)
        return status;

    status = sns_ddf_malloc((void**)&(state->sensor_data.samples),
                            sizeof(sns_ddf_sensor_sample_s)
                            * DAF_PLAYBACK_NUM_AXES);
    if (status != SNS_DDF_SUCCESS)
        return status;

    state->write_index  = 0; // Set indices equal to denote empty buffer
    state->read_index   = 0;

    state->start_time   = 0;

    state->recorded_odr = DAF_PLAYBACK_DEFAULT_ODR;
    state->current_odr  = 0;
    state->smgr_handle  = smgr_handle;

    state->md_enabled   = FALSE;

    // Initialize data to dummy data
    fill_dummy_data(&state->sensor_data);

    status = sns_ddf_timer_init(&state->timer,
                                state,
                                &sns_dd_daf_playback_accel_if,
                                NULL,
                                FALSE); // not periodic
    if (status != SNS_DDF_SUCCESS)
        return status;

    *dd_handle_ptr = state;
    *sensors       = (sns_ddf_sensor_e *)&sensor_instance;
    *num_sensors   = 1;
    return SNS_DDF_SUCCESS;
}


// Playback driver API
sns_ddf_driver_if_s sns_dd_daf_playback_accel_if =
{
    .init               = &sns_dd_daf_playback_accel_init,
    .get_data           = &sns_dd_daf_playback_accel_get_data,
    .set_attrib         = &sns_dd_daf_playback_accel_set_attr,
    .get_attrib         = &sns_dd_daf_playback_accel_get_attr,
    .handle_timer       = &sns_dd_daf_playback_accel_handle_timer,
    .handle_irq         = NULL,
    .reset              = &sns_dd_daf_playback_accel_reset,
    .run_test           = NULL,
    .enable_sched_data  = &sns_dd_daf_playback_accel_enable_sched_data,
    .probe              = &sns_dd_daf_playback_accel_probe,
    .trigger_fifo_data  = NULL,
    .process_daf_req    = &sns_dd_daf_playback_accel_process_daf_req,
    .cancel_daf_trans   = NULL /* no DAF indications */
};
