
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


/*==============================================================================

  S E N S O R S    G Y R O S C O P E    D R I V E R

  DESCRIPTION

  Impelements the driver for the Gyroscope sensor

  EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.




  when         who     what, where, why
  09/24/2014   lk      Improve timestamps in trigger_fifo_data()
  09/16/2014   MW      Skip data logging (0x14c1 logs) for invalid samples
  08/24/14     vy      Used new DDF apis to allocate/free mem supported in uImage
  08/14/2014   vy      Moved in read/write wrapper and moved out log-data
  07/29/2014   vy      Created for uImage support

  ----------   ---     -----------------------------------------------------------

  ==============================================================================*/

/*============================================================================

  INCLUDE FILES

  ============================================================================*/

#include "sns_dd_bmg160.h"

struct bmg160 *p_bmg160;

float bmg160_lsb_res_rad = 0;

sns_ddf_handle_t bmg160_port_handle;



sns_ddf_driver_if_s sns_bmg160_driver_fn_list =
{
    .init                 = &sns_dd_bmg160_init,
    .get_data             = &sns_dd_bmg160_get_data,
    .set_attrib           = &sns_dd_bmg160_set_attr,
    .get_attrib           = &sns_dd_bmg160_get_attr,
    .handle_irq           = &sns_dd_bmg160_interrupt_handler,
    .reset                = &sns_dd_bmg160_reset,
    .run_test             = &sns_dd_gyro_bmg160_self_test,
    .enable_sched_data    = &sns_dd_bmg160_enable_sched_data,
    .probe                = &sns_dd_bmg160_probe,
    .trigger_fifo_data    = &sns_dd_bmg160_triger_fifo_data
};


static void sns_dd_bmg160_update_sample_ts(
        sns_dd_bmg160_state_t *state,
        int in_irq,
        int number);

static sns_ddf_status_e bmg160_parse_fifo_frame(
        unsigned char *data,
        struct bmg160_data *gyro,
        sns_dd_bmg160_state_t *state);



sns_ddf_status_e sns_dd_bmg160_reg_dump(sns_dd_bmg160_state_t *state);

static void sns_dd_bmg160_convertNcorrect_sample(
        sns_dd_bmg160_state_t *state,
        struct bmg160_data *gyro_data,
        q16_t samples[3]);

static sns_ddf_status_e sns_dd_bmg160_reset_fifo(
        sns_dd_bmg160_state_t *state);

static sns_ddf_status_e bmg160_read_fifo_frames(
        sns_ddf_handle_t port_handle,
        uint32_t f_len,
        uint32_t f_count,
        uint8_t *buf);

static sns_ddf_status_e bmg160_smbus_read_byte_block(
        sns_ddf_handle_t port_handle,
        unsigned char reg_addr,
        unsigned char *data,
        uint32_t len);

extern
void sns_dd_gyro_log_fifo_backend(sns_ddf_sensor_data_s *gyro_data_ptr);

/********************************************************
  FUNCTION DEFINITIONS
 *********************************************************/

/*===========================================================================

FUNCTION:   sns_dd_mag_bosch_get_data_all

===========================================================================*/
/*!
  @brief Gets the magnetometer data from the bosch magnetometer

  @detail
  - read the accel data and temp data registers and scales the values
  and stores them in the driver state structure in Q16 format

  @param[in] state: Ptr to the driver structure

  @return
  The error code definition within the DDF
  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
  */
/*=========================================================================*/
static sns_ddf_status_e sns_dd_bmg160_get_data_all(
        sns_dd_bmg160_state_t *state)
{
    sns_ddf_status_e stat;
    struct bmg160_data value;
    BMG160_S16 tmp;


    if ((stat = BMG_CALL_API(get_dataXYZ)(&value)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    if (BMG160_DEV_SEL_NUM_BMI058 == state->dev_select) {
        tmp = value.datax;
        value.datax = value.datay;
        value.datay = tmp;
    }

    sns_dd_bmg160_convertNcorrect_sample(state, &value, state->data_cache);

    return SNS_DDF_SUCCESS;
}


/*===========================================================================
FUNCTION:   sns_dd_gyro_log_fifo
===========================================================================*/
/*!
  @brief log accel sensor data in fifo

  @detail
  - Logs latest set of sensor data sampled from the sensor.

  @param[in] accel_data_ptr: ptr to the driver data
  */
/*=========================================================================*/
void sns_dd_gyro_log_fifo(sns_ddf_sensor_data_s *data_collection, uint8_t context)
{

#if BMG160_CONFIG_ENABLE_UIMAGE
    if (sns_ddf_smgr_is_in_uimage()) {
        return;
    }
#endif

    sns_dd_gyro_log_fifo_backend(data_collection);

#if BMG160_CONFIG_ENABLE_LOG_DATA_L1
    BMG160_MSG_4(HIGH, "dl_meta <:0x%x, %d,%d,%d:>",
            (data_collection->sensor << 24) | (context << 16) | (data_collection->status << 8),
            data_collection->num_samples,
            data_collection->timestamp,
            data_collection->end_timestamp);
#endif

#if BMG160_CONFIG_ENABLE_LOG_DATA_L2
    uint32_t i;
    sns_ddf_sensor_sample_s  *samples;

    for (i = 0; i < data_collection->num_samples / 3; i++) {
        samples = data_collection->samples + i * 3;
        BMG160_MSG_4(HIGH, "dl_data<:0x%x, %d,%d,%d:>",
                ((data_collection->sensor << 16) | (context << 8)) | (i & 0xffff),
                samples[0].sample,
                samples[1].sample,
                samples[2].sample);
    }
#endif
}


/*===========================================================================

FUNCTION:   sns_dd_bmg160_get_data

===========================================================================*/
/*!
  @brief Called by the SMGR to get data

  @detail
  - Requests a single sample of sensor data from each of the specified
  sensors. Data is returned immediately after being read from the
  sensor, in which case data[] is populated in the same order it was
  requested,

  @param[in]  dd_handle    Handle to a driver instance.
  @param[in] sensors       List of sensors for which data isrequested.
  @param[in] num_sensors   Length of @a sensors.
  @param[in] memhandler    Memory handler used to dynamically allocate
  output parameters, if applicable.
  @param[out] data         Sampled sensor data. The number of elements
  must match @a num_sensors.

  @return
  The error code definition within the DDF
  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Update the each sample status after a check for bound is done
  */
/*=========================================================================*/
sns_ddf_status_e sns_dd_bmg160_get_data(
        sns_ddf_handle_t        dd_handle,
        sns_ddf_sensor_e        sensors[],
        uint32_t                num_sensors,
        sns_ddf_memhandler_s*   memhandler,
        sns_ddf_sensor_data_s*  data[])
{
    uint8_t i;
    sns_ddf_status_e stat;
    sns_ddf_sensor_data_s *data_ptr;
    sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;

    /* Sanity check*/
    for (i = 0; i < num_sensors; i++)
    {
        if (sensors[i] != SNS_DDF_SENSOR_GYRO)
        {
            return SNS_DDF_EINVALID_PARAM;
        }
    }

    /* this is a sync driver, so try to read data now */
    if ((stat = sns_dd_bmg160_get_data_all(
                    (sns_dd_bmg160_state_t*)dd_handle)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    if ((data_ptr = sns_ddf_memhandler_malloc_ex(memhandler,
                    (num_sensors)*(sizeof(sns_ddf_sensor_data_s)),state->smgr_handle)) == NULL)
    {
        return SNS_DDF_ENOMEM;
    }


    *data = data_ptr;

    for (i=0; i <num_sensors; i++)
    {
        data_ptr[i].sensor = sensors[i];
        data_ptr[i].status = SNS_DDF_SUCCESS;
        data_ptr[i].timestamp = sns_ddf_get_timestamp();

        if (sensors[i] == SNS_DDF_SENSOR_GYRO)
        {
            if ((data_ptr[i].samples = sns_ddf_memhandler_malloc_ex(memhandler,
                            SDD_GYRO_NUM_AXES*sizeof(sns_ddf_sensor_sample_s),state->smgr_handle)) == NULL)
            {
                return SNS_DDF_ENOMEM;
            }


            data_ptr[i].samples[0].sample  = state->data_cache[SDD_GYRO_X];
            data_ptr[i].samples[1].sample  = state->data_cache[SDD_GYRO_Y];
            data_ptr[i].samples[2].sample  = state->data_cache[SDD_GYRO_Z];

            data_ptr[i].samples[0].status = SNS_DDF_SUCCESS;
            data_ptr[i].samples[1].status = SNS_DDF_SUCCESS;
            data_ptr[i].samples[2].status = SNS_DDF_SUCCESS;

            data_ptr[i].num_samples = SDD_GYRO_NUM_AXES;
        }

    }

    return SNS_DDF_SUCCESS;
}


/*===========================================================================
FUNCTION:   sns_dd_bmg160_interrupt_handler
===========================================================================*/
/*!
  @brief handle interrupts

  @detail.

  @param[in] dd handle, int gpio pin, time stamp
  */
/*=========================================================================*/
void sns_dd_bmg160_interrupt_handler(
        sns_ddf_handle_t        dd_handle,
        uint32_t                gpio_num,
        sns_ddf_time_t          timestamp)
{
    sns_ddf_status_e status;
    sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;
    uint8_t int_stat1;
    int in_irq;
    uint8_t f_stat;
    uint8_t f_count;
    uint8_t f_overrun;
    uint8_t int_stat_fifo = 0;
#ifdef BMG160_DEBUG
    uint8_t int_stat_drdy = 0;
#endif // BMG160_DEBUG

    uint8_t dri_mockup_wmi = 0;
    uint8_t downsampling_factor = (state->odr_hw / state->odr_reported);

    uint8_t si_buf_wr;
    uint8_t tmp_r;

    bool    wmi_missing_possible = false;

    BMG160_MSG_1(HIGH, "<bmg160_iff_isr>ts:%u", timestamp);

    if (gpio_num != ((sns_dd_bmg160_state_t*)dd_handle)->gpio_num) {
        return;
    }

    state->ts_irq = timestamp;

#if BMG160_CONFIG_DRI_MOCKUP_WMI
    if ((state->en_fifo_int) && (1 == state->f_wml)) {
        dri_mockup_wmi = 1;
    }
#endif
    if ((state->en_dri) || (dri_mockup_wmi)) {
        //this is to reduce false drdy interrupts rate
        int32_t lapse;
        uint32_t false_irq = false;

        lapse           = ((int32_t)timestamp - (int32_t)state->ts_last_drdy);

        BMG160_MSG_3(LOW, "[bmg160_if_isr] anti_false_irq <%d %d %d>",
                lapse, state->itvl_drdy - lapse, state->itvl_drdy_tolerance);

        if (lapse < state->itvl_drdy) {
            if ((state->itvl_drdy - lapse) > state->itvl_drdy_tolerance) {
                false_irq = true;
            }
        }

        if (!false_irq) {
            state->num_irq_drdy++;

            if (state->sample_cnt_to_skip_odrc > 0) {
                state->sample_cnt_to_skip_odrc--;
            }

            state->ts_last_drdy = timestamp;
        } else {
            BMG160_MSG_0(HIGH, "[bmg160_if_isr] anti_false_irq false_irq detected");
        }

        if (0 == (state->num_irq_drdy % downsampling_factor)) {
            if (dri_mockup_wmi) {
                status = sns_ddf_smgr_notify_event(state->smgr_handle,
                        SNS_DDF_SENSOR_GYRO,
                        SNS_DDF_EVENT_FIFO_WM_INT);
            }

            sns_dd_bmg160_get_data_all(state);

            state->f_frames_cache.samples[SDD_GYRO_X].sample = state->data_cache[SDD_GYRO_X];
            state->f_frames_cache.samples[SDD_GYRO_Y].sample = state->data_cache[SDD_GYRO_Y];
            state->f_frames_cache.samples[SDD_GYRO_Z].sample = state->data_cache[SDD_GYRO_Z];

            if (0 >= state->sample_cnt_to_skip_odrc) {
                state->f_frames_cache.samples[SDD_GYRO_X].status = SNS_DDF_SUCCESS;
                state->f_frames_cache.samples[SDD_GYRO_Y].status = SNS_DDF_SUCCESS;
                state->f_frames_cache.samples[SDD_GYRO_Z].status = SNS_DDF_SUCCESS;

                state->f_frames_cache.status = SNS_DDF_SUCCESS;
            } else {
                state->f_frames_cache.samples[SDD_GYRO_X].status = SNS_DDF_EINVALID_DATA;
                state->f_frames_cache.samples[SDD_GYRO_Y].status = SNS_DDF_EINVALID_DATA;
                state->f_frames_cache.samples[SDD_GYRO_Z].status = SNS_DDF_EINVALID_DATA;

                state->f_frames_cache.status = SNS_DDF_EINVALID_DATA;

                BMG160_MSG_1(HIGH, "[bmg160_if_isr] sample_cnt_to_skip_odrc: %d",
                        state->sample_cnt_to_skip_odrc);
            }

            state->f_frames_cache.sensor = SNS_DDF_SENSOR_GYRO;

            /*! notice : the number samples value */
            state->f_frames_cache.num_samples = SDD_GYRO_NUM_AXES;
            sns_dd_bmg160_update_sample_ts(state, 1, 1 * downsampling_factor);

            status = sns_ddf_smgr_notify_data(state->smgr_handle,
                    &state->f_frames_cache, 1);
            if(state->f_frames_cache.status == SNS_DDF_SUCCESS) {
               sns_dd_gyro_log_fifo(&state->f_frames_cache,
                       dri_mockup_wmi ? DD_DATA_LOG_CONTEXT_FIFO_WMI_IMITATED : DD_DATA_LOG_CONTEXT_DRI);
            }
        }
    }

    if (state->en_fifo_int && (!dri_mockup_wmi) && (state->f_wml > 0)) {
        /* for FIFO, status1 does not need to be checked */
        int_stat1 = 0;
        if ((status = bmg160_get_interrupt_status_reg_1(&int_stat1)) != SNS_DDF_SUCCESS)
        {
            return;
        }
#ifdef BMG160_DEBUG
        int_stat_drdy = BMG160_GET_BITSLICE(int_stat1, BMG160_INT_STATUS1_DATA_INT);
#endif // BMG160_DEBUG
        int_stat_fifo = BMG160_GET_BITSLICE(int_stat1, BMG160_INT_STATUS1_FIFO_INT);

        f_stat = 0;
        if ((status = bmg160_get_fifostatus_reg(&f_stat))
                != SNS_DDF_SUCCESS) {
            return;
        }

        f_count = BMG160_GET_BITSLICE(f_stat,BMG160_FIFO_STATUS_FRAME_COUNTER);
        f_overrun = BMG160_GET_BITSLICE(f_stat, BMG160_FIFO_STATUS_OVERRUN);

        if (f_overrun) {
            status = sns_ddf_smgr_notify_event(state->smgr_handle,
                    SNS_DDF_SENSOR_GYRO,
                    SNS_DDF_EVENT_FIFO_OVERFLOW);
            BMG160_MSG_2(ERROR, "[bmg160_if_isr] SNS_DDF_EVENT_FIFO_OVERFLOW %d %d",
                    f_count, sns_ddf_get_timestamp());
            sns_dd_bmg160_reset_fifo(state);
            return;
        }

#ifdef BMG160_DEBUG
        BMG160_MSG_3_P(HIGH,
                "sns_dd_bmg160_interrupt_handler <---1---> int_stat_fifo=%d  int_stat_drdy=%d f_count=%d",
                int_stat_fifo, int_stat_drdy, f_count);
#endif

        in_irq = 1;
        while (int_stat_fifo
                || (f_count >= state->f_wml)
              ) {

            //this will solve the issue of potential timing inconsistancy that can happen during a boundary
            //condition when interrupt has fired but fifo count register is locked from being updated.
            if ((int_stat_fifo) && (f_count == (state->f_wml - 1))) {
                f_count = state->f_wml;
#ifdef BMG160_DEBUG
                BMG160_MSG_3_P(HIGH,
                        "sns_dd_bmg160_interrupt_handler <-----> int_stat_fifo= %d  int_stat_drdy=%d f_count=%d",
                        int_stat_fifo, int_stat_drdy, f_count);
#endif
            }

            if (int_stat_fifo
                    || (f_count >= state->f_wml)) {
                status = sns_ddf_smgr_notify_event(state->smgr_handle,
                        SNS_DDF_SENSOR_GYRO,
                        SNS_DDF_EVENT_FIFO_WM_INT);
            }

            tmp_r = f_count % state->f_wml;
            if (((tmp_r + 1) == state->f_wml) || (f_count >= (2 * state->f_wml))) {
                wmi_missing_possible = true;
                si_buf_wr = 0x00;
                bmg160_sbus_write(state->port_handle,
                        BMG160_INT_MAP_1, &si_buf_wr, 1, NULL);

                BMG160_MSG_3(ERROR, "[bmg160_if_isr]wmi_missing_possible 1"
                        "timestamp: %d wml: %d f_count: %d",
                        timestamp, state->f_wml, f_count | (in_irq << 8));
            }

            f_count = f_count / state->f_wml * state->f_wml;
#if 0
            if (f_count < 2 * state->f_wml) {
                f_count = state->f_wml;
            }
#endif
            bmg160_fifo_data_read_out_frames(state, f_count, in_irq);
            in_irq = 0;

            if (state->f_frames_cache.num_samples > 0) {
                status = sns_ddf_smgr_notify_data(state->smgr_handle,
                        &state->f_frames_cache, 1);
                if(state->f_frames_cache.status == SNS_DDF_SUCCESS) {
                   sns_dd_gyro_log_fifo(&state->f_frames_cache, DD_DATA_LOG_CONTEXT_FIFO_WMI);
                }

            }

            //poll the status and frame counter again
            int_stat1 = 0;
            if ((status = bmg160_get_interrupt_status_reg_1(&int_stat1)) != SNS_DDF_SUCCESS)
            {
                return;
            }
#ifdef BMG160_DEBUG
            int_stat_drdy = BMG160_GET_BITSLICE(int_stat1, BMG160_INT_STATUS1_DATA_INT);
#endif // BMG160_DEBUG
            int_stat_fifo = BMG160_GET_BITSLICE(int_stat1, BMG160_INT_STATUS1_FIFO_INT);

            f_stat = 0;
            if ((status = bmg160_get_fifostatus_reg(&f_stat))
                    != SNS_DDF_SUCCESS) {
                return;
            }

            f_count = BMG160_GET_BITSLICE(f_stat,BMG160_FIFO_STATUS_FRAME_COUNTER);
            f_overrun = BMG160_GET_BITSLICE(f_stat, BMG160_FIFO_STATUS_OVERRUN);

#ifdef BMG160_DEBUG
            BMG160_MSG_3_P(HIGH, "sns_dd_bmg160_interrupt_handler <---2---> int_stat_fifo= %d  int_stat_drdy= %d f_count =%d",
                    int_stat_fifo, int_stat_drdy, f_count);
#endif //BMG160_DEBUG
        }
    }


    if (wmi_missing_possible) {
        bmg160_sbus_write(state->port_handle, BMG160_INT_MAP_1, &state->regv_int_map_1, 1, NULL);
    }

    return;
}


/*===========================================================================
FUNCTION:   sns_dd_bmg160_triger_fifo_data
===========================================================================*/
/* @brief Retrieves a set of sensor data. Asynchronous API
 *
 * Requests sample of sensor data from the specified sensor.
 *
 * @note If a sensor has failed or
 *		 isn't available, @a sns_ddf_sensor_data_s.status must be used to
 *		 reflect this status.
 *
 * @param[in]  dd_handle	   Handle to a driver instance.
 * @param[in]  sensor		   sensor for which data is requested.
 *
 * @param[in]  num_samples	   number of samples to retrieve as available. Drain the FIFO if value is set to Zero.
 * @param[in]  trigger now	   trigger nofity fifo data now or
 *		 later when trigger_now is set to true.
 *
 *
 * @return SNS_DDF_SUCCESS	   if data was populated successfully.
 *		   via sns_ddf_smgr_data_notify() or if trigger_now is
 *		   set to false; Otherwise a specific error code is
 *		   returned.
 *
 * @see sns_ddf_data_notify_data() as this will be used to report the data.
 */
/*=========================================================================*/
sns_ddf_status_e sns_dd_bmg160_triger_fifo_data(
        sns_ddf_handle_t    dd_handle,
        sns_ddf_sensor_e    sensor,
        uint16_t            num_samples,
        bool                trigger_now)
{
    sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;
    sns_ddf_status_e stat;
    uint8_t f_count = 0;
    uint8_t downsampling_factor = (state->odr_hw / state->odr_reported);

    BMG160_MSG_3(HIGH, "<bmg160_if_ triger_fifo_data> sensor: %d num_samples=%d trigger_now=%d",
            sensor, num_samples, trigger_now);

    if (0 == trigger_now) {
        return SNS_DDF_SUCCESS;
    }

    if ((stat = BMG_CALL_API(get_fifo_framecount)(&f_count)) != SNS_DDF_SUCCESS) {
        return stat;
    }

    if (0 == num_samples) {
    } else {
        if (num_samples * downsampling_factor < f_count) {
            f_count = num_samples * downsampling_factor;
        }
    }

    if (f_count > 0) {
        bmg160_fifo_data_read_out_frames(state, f_count, 0);
        /*! notice : the number data len */
        if (state->f_frames_cache.num_samples > 0) {
            stat = sns_ddf_smgr_notify_data(state->smgr_handle,
                    &state->f_frames_cache, 1);
            if(state->f_frames_cache.status == SNS_DDF_SUCCESS) {
               sns_dd_gyro_log_fifo(&state->f_frames_cache, DD_DATA_LOG_CONTEXT_FIFO_FLUSH);
            }
        }
    }

    return stat;
}


static void sns_dd_bmg160_convertNcorrect_sample(
        sns_dd_bmg160_state_t *state,
        struct bmg160_data *gyro_data,
        q16_t samples[3])
{
    samples[SDD_GYRO_X] = FX_FLTTOFIX_Q16(gyro_data->datax
            * ((4000>>state->range) * bmg160_lsb_res_rad)) + state->bias[0];
    samples[SDD_GYRO_Y] = FX_FLTTOFIX_Q16(gyro_data->datay
            * ((4000>>state->range) * bmg160_lsb_res_rad)) + state->bias[1];
    samples[SDD_GYRO_Z] = FX_FLTTOFIX_Q16(gyro_data->dataz
            * ((4000>>state->range) * bmg160_lsb_res_rad)) + state->bias[2];

#if BMG160_CONFIG_ENABLE_VERBOSE_MESSAGE
    BMG160_MSG_3_P(HIGH, "gyro FIFO frame raw data %d %d %d",
            gyro_data->datax,
            gyro_data->datay,
            gyro_data->dataz);

    BMG160_MSG_3_P(HIGH, "gyro FIFO frame Q16 data before remapping %d %d %d",
            samples[SDD_GYRO_X],
            samples[SDD_GYRO_Y],
            samples[SDD_GYRO_Z]);
#endif

    sns_ddf_map_axes(&state->axes_map, samples);
}


sns_ddf_status_e sns_dd_bmg160_reg_dump(sns_dd_bmg160_state_t *state)
{
    sns_ddf_status_e stat;
    uint8_t readbyte = 0;
    uint8_t  out;

    BMG160_MSG_3_F(HIGH, "sns_dd_bmg160_reg_dump odr_reported: %d odr_hw: %d state->f_wml: %d",
            state->odr_reported, state->odr_hw, state->f_wml);

    if ((stat = bmg160_sbus_read(state->port_handle,
                    0x3d,
                    &readbyte,
                    1,
                    &out)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    BMG160_MSG_3_F(HIGH, "sns_dd_bmg160_reg_dump %d %d @0x3d: 0x%x",
            55555581, out, readbyte);

    if ((stat = bmg160_sbus_read(state->port_handle,
                    0x3e,
                    &readbyte,
                    1,
                    &out)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    BMG160_MSG_3_F(HIGH, "sns_dd_bmg160_reg_dump %d %d @0x3e: 0x%x",
            55555582, out, readbyte);

    if ((stat = bmg160_sbus_read(state->port_handle,
                    BMG160_DSP_BW_ADDR,
                    &readbyte,
                    1,
                    &out)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    BMG160_MSG_3_F(HIGH, "sns_dd_bmg160_reg_dump %d %d @0x10: 0x%x",
            55555581, out, readbyte);


    if ((stat = bmg160_sbus_read(state->port_handle,
                    BMG160_RST_LATCH_ADDR,
                    &readbyte,
                    1,
                    &out)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    BMG160_MSG_3_F(HIGH, "sns_dd_bmg160_reg_dump %d %d @0x21: 0x%x",
            5555582, out, readbyte);


    if ((stat = bmg160_sbus_read(state->port_handle,
                    BMG160_INT_ENABLE0,
                    &readbyte,
                    1,
                    &out)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    BMG160_MSG_3_F(HIGH, "sns_dd_bmg160_reg_dump %d %d @0x15: 0x%x",
            5555583, out, readbyte);


    if ((stat = bmg160_sbus_read(state->port_handle,
                    BMG160_INT_ENABLE1,
                    &readbyte,
                    1,
                    &out)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    BMG160_MSG_3_F(HIGH, "sns_dd_bmg160_reg_dump %d %d @0x16: 0x%x",
            5555584, out, readbyte);


    if ((stat = bmg160_sbus_read(state->port_handle,
                    BMG160_INT_MAP_1,
                    &readbyte,
                    1,
                    &out)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    BMG160_MSG_3_F(HIGH, "sns_dd_bmg160_reg_dump %d %d @0x18: 0x%x",
            5555585, out, readbyte);
    return stat;
}

sns_ddf_status_e bmg160_fifo_data_read_out_frames(
        sns_dd_bmg160_state_t *state,
        uint8_t f_count,
        int in_irq)
{
    int i;
    unsigned char fifo_data_out[BMG160_MAX_FIFO_LEVEL * BMG160_MAX_FIFO_F_BYTES] = {0};
    unsigned char *frame_pointer;
    unsigned char f_len = 6;
    //unsigned char fifo_datasel = 0;
    struct bmg160_data gyro_data = {0};
    sns_ddf_status_e stat;
    uint8_t flag_downsampling = (state->odr_hw > state->odr_reported);
    uint8_t downsampling_factor = (state->odr_hw / state->odr_reported);
    uint8_t frames_to_report = 0;

    int32_t frames_has_invalid_data = false;
    int32_t num_frames_invalid_data = 0;


    sns_dd_bmg160_update_sample_ts(state, in_irq, f_count);

    if ((stat = bmg160_read_fifo_frames(state->port_handle,
                    f_len, f_count, fifo_data_out))
            != SNS_DDF_SUCCESS)
        return stat;

#if 1
//BMG160_CONFIG_ENABLE_VERBOSE_MESSAGE
    BMG160_MSG_3_P(HIGH, "read_out_frames f_count = %d first_ts = %d end_ts = %d",
            f_count, state->f_frames_cache.timestamp, state->f_frames_cache.end_timestamp);
#endif
    //parse the frames
    frame_pointer = fifo_data_out;

#if 0
    buffer = 0x80;
    stat = bmg160_sbus_write(
            state->port_handle, BMA2X2_INT_CTRL_REG,
            &buffer, 1, &read_count);
#endif

    state->f_frames_cache.sensor            = SNS_DDF_SENSOR_GYRO;
    state->f_frames_cache.status            = SNS_DDF_SUCCESS;
    state->f_frames_cache.num_samples       = 0;

    for (i = 0; i < f_count; i++, frame_pointer += f_len) {
        if (state->sample_cnt_to_skip_odrc > 0) {
            state->sample_cnt_to_skip_odrc--;
        }

        if (flag_downsampling) {
            if (0 != ((i + 1) % downsampling_factor)) {
                continue;
            }
        }

        if (state->sample_cnt_to_skip_odrc > 0) {
            frames_has_invalid_data = true;
        }

        bmg160_parse_fifo_frame(frame_pointer, &gyro_data, state);

        sns_dd_bmg160_convertNcorrect_sample(state, &gyro_data, state->data_cache_fifo);

        state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*frames_to_report+SDD_GYRO_X].sample = state->data_cache_fifo[SDD_GYRO_X];
        state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*frames_to_report+SDD_GYRO_Y].sample = state->data_cache_fifo[SDD_GYRO_Y];
        state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*frames_to_report+SDD_GYRO_Z].sample = state->data_cache_fifo[SDD_GYRO_Z];


        if (0 >= state->sample_cnt_to_skip_odrc) {
            state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*frames_to_report+SDD_GYRO_X].status = SNS_DDF_SUCCESS;
            state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*frames_to_report+SDD_GYRO_Y].status = SNS_DDF_SUCCESS;
            state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*frames_to_report+SDD_GYRO_Z].status = SNS_DDF_SUCCESS;
            
#if BMG160_CONFIG_ENABLE_WA_EINVALID_DATA
    /* WA for TS issue,   reset the invalid data to  the first valid data, reset the status to success */
            int8_t count_to_reset_invalid_data;
            
            
            if ((0 == state->sample_cnt_to_skip_odrc)&&(state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*(frames_to_report - 1)+SDD_GYRO_X].status == SNS_DDF_EINVALID_DATA)) {

            for( count_to_reset_invalid_data = frames_to_report - 1; count_to_reset_invalid_data >= 0; count_to_reset_invalid_data--) {


                state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*count_to_reset_invalid_data+SDD_GYRO_X].sample = state->data_cache_fifo[SDD_GYRO_X];
                state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*count_to_reset_invalid_data+SDD_GYRO_Y].sample = state->data_cache_fifo[SDD_GYRO_Y];
                state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*count_to_reset_invalid_data+SDD_GYRO_Z].sample = state->data_cache_fifo[SDD_GYRO_Z];
        
                state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*count_to_reset_invalid_data+SDD_GYRO_X].status = SNS_DDF_SUCCESS;
                state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*count_to_reset_invalid_data+SDD_GYRO_Y].status = SNS_DDF_SUCCESS;
                state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*count_to_reset_invalid_data+SDD_GYRO_Z].status = SNS_DDF_SUCCESS;
                BMG160_MSG_1(HIGH, "[read_out_frames] count_to_reset_invalid_data: %d",
                                    count_to_reset_invalid_data);

            }

            //state->sample_cnt_to_skip_odrc--;
            
        }


#endif
            
        } else {
            state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*frames_to_report+SDD_GYRO_X].status = SNS_DDF_EINVALID_DATA;
            state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*frames_to_report+SDD_GYRO_Y].status = SNS_DDF_EINVALID_DATA;
            state->f_frames_cache.samples[SDD_GYRO_NUM_AXES*frames_to_report+SDD_GYRO_Z].status = SNS_DDF_EINVALID_DATA;

            BMG160_MSG_1(HIGH, "[read_out_frames] sample_cnt_to_skip_odrc: %d",
                    state->sample_cnt_to_skip_odrc);
        }

        frames_to_report++;

        if (frames_has_invalid_data) {
            if (0 < state->sample_cnt_to_skip_odrc) {
                num_frames_invalid_data = frames_to_report;
            }
        }

        //TODO: bias not applied
    }

    if (!frames_has_invalid_data)  {
    } else {
        //we have invalid samples, we will split this batch into invalid frames and valid frames
        uint32_t itvl = 0;

        if (frames_to_report > 1) {
            itvl = (state->f_frames_cache.end_timestamp - state->f_frames_cache.timestamp) / (frames_to_report - 1);
        }

        state->f_frames_cache.status            = SNS_DDF_EINVALID_DATA;
        state->f_frames_cache.num_samples       = num_frames_invalid_data * SDD_GYRO_NUM_AXES;

        if (frames_to_report > 1) {
            state->f_frames_cache.end_timestamp     = state->f_frames_cache.timestamp +
                (num_frames_invalid_data - 1) * itvl;
        }

        if (state->f_frames_cache.num_samples > 0) {
            stat = sns_ddf_smgr_notify_data(state->smgr_handle,
                    &state->f_frames_cache, 1);

            if(state->f_frames_cache.status == SNS_DDF_SUCCESS) {
               sns_dd_gyro_log_fifo(&state->f_frames_cache, DD_DATA_LOG_CONTEXT_FIFO_WMI);
            }
        }

        frames_to_report -= num_frames_invalid_data;
        state->f_frames_cache.timestamp = state->f_frames_cache.end_timestamp + itvl;
        state->f_frames_cache.end_timestamp = state->f_frames_cache.timestamp + itvl * (frames_to_report - 1);
        state->f_frames_cache.status            = SNS_DDF_SUCCESS;


        if (frames_to_report > 0) {
            memcpy(state->f_frames_cache.samples,
                    state->f_frames_cache.samples + num_frames_invalid_data * SDD_GYRO_NUM_AXES,
                    sizeof(state->f_frames_cache.samples[0]) * frames_to_report * SDD_GYRO_NUM_AXES);
        }

        BMG160_MSG_3(HIGH, "[read_out_frames] num_frames_invalid_data: %d timestamp: %d end_timestamp: %d",
                num_frames_invalid_data,
                state->f_frames_cache.timestamp,
                state->f_frames_cache.end_timestamp);
    }


    /*! notice : the number samples value */
    state->f_frames_cache.num_samples = frames_to_report * SDD_GYRO_NUM_AXES;

    return stat;
}


BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifostatus_reg(
        unsigned char *fifo_status)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    if (p_bmg160 == BMG160_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,\
                BMG160_FIFO_STATUS, fifo_status, 1);
    }
    return comres;
}


BMG160_RETURN_FUNCTION_TYPE bmg160_get_interrupt_status_reg_1(
        unsigned char *status1_data)
{
    BMG160_RETURN_FUNCTION_TYPE comres;
    if (p_bmg160 == BMG160_NULL)
    {
        comres = SNS_DDF_EINVALID_PARAM;
    }
    else
    {
        comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,
                BMG160_INT_STATUS1, status1_data, 1);
    }
    return comres;
}


static sns_ddf_status_e bmg160_parse_fifo_frame(
        unsigned char *data,
        struct bmg160_data *gyro,
        sns_dd_bmg160_state_t *state)
{
    sns_ddf_status_e comres = SNS_DDF_SUCCESS;

    switch (state->dev_select) {
        case BMG160_DEV_SEL_NUM_BMI058:
            data[0] = BMG160_GET_BITSLICE(data[0],BMG160_DATAY_LSB_VALUEY);
            gyro->datay = (BMG160_S16)((((BMG160_S16)((signed char)data[1])) << BMG160_SHIFT_8_POSITION) | (data[0]));

            data[2] = BMG160_GET_BITSLICE(data[2],BMG160_DATAX_LSB_VALUEX);
            gyro->datax = (BMG160_S16)((((BMG160_S16)((signed char)data[3])) << BMG160_SHIFT_8_POSITION) | (data[2]));
            break;
        default:
            data[0] = BMG160_GET_BITSLICE(data[0],BMG160_DATAX_LSB_VALUEX);
            gyro->datax = (BMG160_S16)((((BMG160_S16)((signed char)data[1])) << BMG160_SHIFT_8_POSITION) | (data[0]));

            data[2] = BMG160_GET_BITSLICE(data[2],BMG160_DATAY_LSB_VALUEY);
            gyro->datay = (BMG160_S16)((((BMG160_S16)((signed char)data[3])) << BMG160_SHIFT_8_POSITION) | (data[2]));
            break;
    }

    data[4] = BMG160_GET_BITSLICE(data[4],BMG160_DATAZ_LSB_VALUEZ);
    gyro->dataz = (BMG160_S16)((((BMG160_S16)((signed char)data[5])) << BMG160_SHIFT_8_POSITION) | (data[4]));

    return comres;
}


static void sns_dd_bmg160_update_sample_ts(
        sns_dd_bmg160_state_t *state,
        int in_irq,
        int number)
{
    sns_ddf_time_t start;
    sns_ddf_time_t end;
    sns_ddf_time_t interval;
    uint8_t flag_downsampling = (state->odr_hw > state->odr_reported);
    uint8_t downsampling_factor = (state->odr_hw / state->odr_reported);

    if (0 == number) {
        BMG160_MSG_2(HIGH, "invalid parameters: %d %d", in_irq, state->f_wml);
        return;
    }

    start = state->ts_start_first_sample;
    if (in_irq) {
        end = state->ts_irq;

        if (state->f_wml > 0) {
            interval = (sns_ddf_time_t)((int32_t)end -
                    (int32_t)start) / state->f_wml;
        } else {
            interval = (sns_ddf_time_t)((int32_t)end -
                    (int32_t)start) / number;
        }
    } else {

        interval = state->ticks_per_ms  * 1000 / state->odr_hw;
        
        end = start + (interval * number);
    }

    if (!flag_downsampling)  {
        state->f_frames_cache.timestamp = state->ts_start_first_sample +
            interval;
        state->f_frames_cache.end_timestamp = state->ts_start_first_sample +
            interval * number;
    } else {
        if (number >= downsampling_factor) {
            state->f_frames_cache.timestamp = state->ts_start_first_sample +
                interval * downsampling_factor;
            state->f_frames_cache.end_timestamp = state->ts_start_first_sample +
                interval * (number / downsampling_factor * downsampling_factor);
        }
    }

#if BMG160_CONFIG_ENABLE_TS_ADJUST_FILTER

            state->f_frames_cache.timestamp -= (sns_ddf_time_t)(state->ts_adjust_gyr);
            state->f_frames_cache.end_timestamp -= (sns_ddf_time_t)(state->ts_adjust_gyr);
        
#endif



    if (!in_irq) {
        BMG160_MSG_3(HIGH, "<start: %d end: %d number: %d>",
                start,
                end,
                number);

        BMG160_MSG_3(HIGH, "<timestamp: %d end_timestamp: %d interval: %d>",
                start,
                end,
                interval);
    }
    //this works for trigger_fifo_data() as well
    state->ts_start_first_sample = end;
}


static sns_ddf_status_e sns_dd_bmg160_reset_fifo(
        sns_dd_bmg160_state_t *state)
{
    sns_ddf_status_e stat;
    uint8_t readbyte = 0;
    uint8_t  out;

    if ((stat = bmg160_sbus_read(state->port_handle,
                    BMG160_FIFO_CGF0_ADDR,
                    &readbyte,
                    1,
                    &out)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    if ((stat = bmg160_sbus_write(state->port_handle,
                    BMG160_FIFO_CGF0_ADDR,
                    &readbyte,
                    1,
                    &out)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    return stat;

}


static sns_ddf_status_e bmg160_smbus_read_byte_block(
        sns_ddf_handle_t port_handle,
        unsigned char reg_addr,
        unsigned char *data,
        uint32_t len)
{
    sns_ddf_status_e stat;
    uint8_t out;

    if ((stat = bmg160_sbus_read(port_handle,
                    reg_addr,
                    data,
                    len,
                    &out)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }
    return stat;
}


#define SNS_DDF_PORT_READ_LEN_LIMIT 255
static sns_ddf_status_e bmg160_read_fifo_frames(
        sns_ddf_handle_t port_handle,
        uint32_t f_len,
        uint32_t f_count,
        uint8_t *buf)
{
    sns_ddf_status_e stat = SNS_DDF_SUCCESS;
    uint32_t bytes_to_read = f_len * f_count;
    uint32_t bytes_to_read_once;
    uint8_t *head;

    head = buf;
    while (bytes_to_read > 0) {
        if (bytes_to_read > SNS_DDF_PORT_READ_LEN_LIMIT) {
            bytes_to_read_once = SNS_DDF_PORT_READ_LEN_LIMIT
                / f_len * f_len;
        } else {
            bytes_to_read_once = bytes_to_read;
        }

        if ((stat = bmg160_smbus_read_byte_block(port_handle,
                        BMG160_FIFO_DATA_ADDR,
                        head,
                        bytes_to_read_once))
                != SNS_DDF_SUCCESS)
            return stat;

        head = head + bytes_to_read_once;
        bytes_to_read -= bytes_to_read_once;
    }

    return stat;
}


BMG160_RETURN_FUNCTION_TYPE bmg160_get_dataXYZ(
        struct bmg160_data *data)
{
    BMG160_RETURN_FUNCTION_TYPE comres;
    unsigned char a_data_u8r[6];
    if (p_bmg160 == BMG160_NULL)
    {
        comres = SNS_DDF_EINVALID_PARAM;
    }
    else
    {
        comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,BMG160_DATAX_LSB_VALUEX__REG,a_data_u8r,6);
        /* EasyCASE - */
        /* Data X */
        a_data_u8r[0] = BMG160_GET_BITSLICE(a_data_u8r[0],BMG160_DATAX_LSB_VALUEX);
        data->datax = (BMG160_S16)((((BMG160_S16)((signed char)a_data_u8r[1])) << BMG160_SHIFT_8_POSITION) | (a_data_u8r[0]));
        /* Data Y */
        a_data_u8r[2] = BMG160_GET_BITSLICE(a_data_u8r[2],BMG160_DATAY_LSB_VALUEY);
        data->datay = (BMG160_S16)((((BMG160_S16)((signed char)a_data_u8r[3])) << BMG160_SHIFT_8_POSITION) | (a_data_u8r[2]));

        /* Data Z */
        a_data_u8r[4] = BMG160_GET_BITSLICE(a_data_u8r[4],BMG160_DATAZ_LSB_VALUEZ);
        data->dataz = (BMG160_S16)((((BMG160_S16)((signed char)a_data_u8r[5])) << BMG160_SHIFT_8_POSITION) | (a_data_u8r[4]));
    }
    return comres;
}


BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_framecount(
        unsigned char *fifo_framecount)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,\
                BMG160_FIFO_STATUS_FRAME_COUNTER__REG, &v_data_u8r, 1);
        *fifo_framecount = BMG160_GET_BITSLICE(v_data_u8r,\
                BMG160_FIFO_STATUS_FRAME_COUNTER);
    }
    return comres;
}


BMG160_RETURN_FUNCTION_TYPE bmg160_set_int_od(unsigned char param, unsigned char int_od)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        switch (param) {
            case BMG160_INT1:
                comres = p_bmg160->BMG160_BUS_READ_FUNC\
                         (p_bmg160->dev_addr,\
                          BMG160_INT_ENABLE1_IT1_OD__REG, &v_data_u8r, 1);
                v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
                        BMG160_INT_ENABLE1_IT1_OD, int_od);
                comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
                         (p_bmg160->dev_addr,\
                          BMG160_INT_ENABLE1_IT1_OD__REG, &v_data_u8r, 1);
                break;
            case BMG160_INT2:
                comres = p_bmg160->BMG160_BUS_READ_FUNC\
                         (p_bmg160->dev_addr,\
                          BMG160_INT_ENABLE1_IT2_OD__REG, &v_data_u8r, 1);
                v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
                        BMG160_INT_ENABLE1_IT2_OD, int_od);
                comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
                         (p_bmg160->dev_addr,\
                          BMG160_INT_ENABLE1_IT2_OD__REG, &v_data_u8r, 1);
                break;
            default:
                comres = SNS_DDF_EINVALID_PARAM;
                break;
        }
    }
    return comres;
}

sns_ddf_status_e bmg160_sbus_read(
        sns_ddf_handle_t        port_handle,
        uint8_t                 rega,
        uint8_t                 *buf,
        uint8_t                 len,
        uint8_t                 *out)
{
    sns_ddf_status_e status;
    uint8_t out_dummy;

    if (NULL == out) {
        out = &out_dummy;
    }

    status = sns_ddf_read_port(port_handle, rega, buf, len, out);

    if (1 != len) {
    } else {
        BMG160_MSG_3(HIGH, "<sbus_read @0x%x=0x%x status: %d>",
                rega, buf[0], status);
    }

    return status;
}

sns_ddf_status_e bmg160_sbus_write(
        sns_ddf_handle_t        port_handle,
        uint8_t                 rega,
        uint8_t                 *buf,
        uint8_t                 len,
        uint8_t                 *out)
{
    sns_ddf_status_e status;
    uint8_t out_dummy;

    if (NULL == out) {
        out = &out_dummy;
    }

    status = sns_ddf_write_port(port_handle, rega, buf, 1, out);

    if (1 == len) {
        BMG160_MSG_3(HIGH, "<sbus_write @0x%x=0x%x status: %d>",
                rega, buf[0], status);
    }

    return status;
}


sns_ddf_status_e bmg_read_wrapper(
        uint8_t dev_addr,
        uint8_t reg_addr,
        uint8_t *bosch_read_data,
        uint8_t len)
{

    sns_ddf_status_e stat;
    uint8_t out;

    stat = bmg160_sbus_read(bmg160_port_handle,
            reg_addr,
            bosch_read_data,
            len,
            &out);

    return stat;
}


sns_ddf_status_e bmg_write_wrapper(uint8_t dev_addr,
        uint8_t reg_addr,
        uint8_t *bosch_write_buffer,
        uint8_t len)
{

    sns_ddf_status_e stat;
    uint8_t out;

    stat = bmg160_sbus_write(bmg160_port_handle,
            reg_addr,
            bosch_write_buffer,
            len,
            &out);

    return stat;
}



void bmg_delay(uint32_t msec)
{
    sns_ddf_delay(1000*msec);
}

