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

/*------------------------------------------------------------------------
   Copyright (c) 2014-2015 by Qualcomm Technologies, Inc.  All Rights Reserved
   Qualcomm Technologies Proprietary and Confidential
  ------------------------------------------------------------------------*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

when         who      what
----------   ---     -----------------------------------------------------------
07/09/15     AH      Fix compiler warnings
08/24/14     vy      Used new DDF apis to allocate/free mem supported in uImage
07/27/2014   vy      Created for uImage 
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

#include "sns_dd_mag_hscdtd008.h"

#ifndef DDK
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#endif


sns_ddf_driver_if_s sns_dd_mag_hscdtd_if =
{

  .init = &alps_mag_init,
  .get_data = &alps_mag_get_data,
  .set_attrib = &alps_mag_set_attrib,
  .get_attrib = &alps_mag_get_attrib,
  .handle_timer = &alps_mag_handle_timer,
  .handle_irq = &alps_mag_handle_irq,
  .reset = &alps_mag_reset,
  .run_test = &alps_mag_run_test,
  .enable_sched_data = NULL,
  .probe = &alps_mag_probe

};


/* get Magnetic field data */
sns_ddf_status_e alps_dd_hscdtd_get_mag(sns_ddf_handle_t dd_handle)
{
	uint8_t i;
	sns_ddf_status_e      status;
	alps_mag_dd_state_t*  state = dd_handle;
    q16_t                 mag_data[ MAG_NUM_AXES ];

	uint8_t count;

	/* Get MagneticFieldSensor sensor data */
	status = sns_ddf_read_port(state->alps_mag_i2c_handle, 
						NKR_OVX, state->alps_mag_i2c_buffer, 6, &count);

    //SNS_PRINTF_STRING_ID_HIGH_3(SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING3, 9916, state->alps_mag_i2c_buffer[0],ALPS_MAG_SENSOR_RESOLUTOIN );
    //SNS_PRINTF_STRING_ID_HIGH_3(SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING3, 9916, state->alps_mag_i2c_buffer[1],ALPS_MAG_SENSOR_RESOLUTOIN );
    //SNS_PRINTF_STRING_ID_HIGH_3(SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING3, 9916, state->alps_mag_i2c_buffer[2],ALPS_MAG_SENSOR_RESOLUTOIN );
    //SNS_PRINTF_STRING_ID_HIGH_3(SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING3, 9916, state->alps_mag_i2c_buffer[3],ALPS_MAG_SENSOR_RESOLUTOIN );
    //SNS_PRINTF_STRING_ID_HIGH_3(SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING3, 9916, state->alps_mag_i2c_buffer[4],ALPS_MAG_SENSOR_RESOLUTOIN );
    //SNS_PRINTF_STRING_ID_HIGH_3(SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING3, 9916, state->alps_mag_i2c_buffer[5],ALPS_MAG_SENSOR_RESOLUTOIN );

	for (i = 0; i < MAG_NUM_AXES; i++) {
		int16_t	data_tmp;

		data_tmp = (int16_t) ((((uint16_t)state->alps_mag_i2c_buffer[i*2+1])<<8) | ((uint16_t)state->alps_mag_i2c_buffer[i*2]));
		if (state->resolution_adc == ALPS_MAG_SENSOR_MAG_RESOLUTION_14) {
			if ((data_tmp == ((0x1fff))) || (data_tmp == ((0xe000)))) {
				state->out_of_bound_flag |= (0x0001 << i);
			}
		} else {
			if ((data_tmp == ((0x3fff))) || (data_tmp == ((0xc000)))) {
				state->out_of_bound_flag |= (0x0001 << i);
			}
		}
		//SNS_PRINTF_STRING_ID_HIGH_3(SNS_DBG_MOD_DSPS_SMGR, DBG_SMGR_GENERIC_STRING3, 9916, state->alps_mag_i2c_buffer[0],ALPS_MAG_SENSOR_RESOLUTOIN );
		mag_data[i] = FX_FLTTOFIX_Q16(data_tmp*ALPS_MAG_SENSOR_RESOLUTOIN);
	}  

    state->data_cache[0] = -mag_data[1];
    state->data_cache[1] = -mag_data[0];
    state->data_cache[2] = -mag_data[2];

	state->alps_mag_ready = true;
	return status;
}


/*===========================================================================
  FUNCTION:   sns_dd_maghscdtd008_log_data
===========================================================================*/
/*!
  @brief log sensor data for HSCDTD008 magnetometer
 
  @detail
  - Logs latest set of sensor data sampled from the mag sensor. 
 
  @param[in] state: ptr to the driver structure
  @param[in] sample_time: Time that the sensor was sampled

  @return
  None
 
*/
/*=========================================================================*/
static void sns_dd_maghscdtd008_log_data(alps_mag_dd_state_t *state,
	sns_ddf_time_t sample_time)
{
  sns_err_code_e err_code;
  sns_log_sensor_data_pkt_s* log_struct_ptr;
  
  DDF_MSG_0(HIGH, "HSCDTD008 Log Data");

  //Allocate log packet
  err_code = sns_logpkt_malloc(SNS_LOG_CONVERTED_SENSOR_DATA,
                               sizeof(sns_log_sensor_data_pkt_s),
                               (void**)&log_struct_ptr);

  if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
  {
    log_struct_ptr->version   = SNS_LOG_SENSOR_DATA_PKT_VERSION;
    log_struct_ptr->sensor_id = SNS_DDF_SENSOR_MAG;
    log_struct_ptr->vendor_id = SNS_DDF_VENDOR_ALPS;

    //Timestamp the log with sample time
    log_struct_ptr->timestamp = sample_time;

    //Log the sensor data
    log_struct_ptr->num_data_types = HSCDTD_NUM_DATATYPES;
    log_struct_ptr->data[0]        = state->data_cache[0];
    log_struct_ptr->data[1]        = state->data_cache[1];
    log_struct_ptr->data[2]        = state->data_cache[2];


    //Commit log (also frees up the log packet memory)
    sns_logpkt_commit(SNS_LOG_CONVERTED_SENSOR_DATA, log_struct_ptr);
  }
  else
  {
    DDF_MSG_1(HIGH, "HSCDTD008 Log Data - logpkt_malloc failed with err: %d", err_code);
  }
} 


sns_ddf_status_e alps_mag_get_data(
			sns_ddf_handle_t dd_handle,
			sns_ddf_sensor_e sensors[],
			uint32_t num_sensors,
			sns_ddf_memhandler_s* memhandler,
			sns_ddf_sensor_data_s** data)
{
	uint8_t  i;
	sns_ddf_time_t timestamp;
	sns_ddf_status_e      status;
	alps_mag_dd_state_t*  state = dd_handle;
	sns_ddf_sensor_data_s *data_ptr;
	
	/* check work memory */
	if (state == NULL) {
		return SNS_DDF_ENOMEM;
	}
	
	timestamp = sns_ddf_get_timestamp();
	
	/* check powerstate */
	if (state->alps_mag_powerstate != SNS_DDF_POWERSTATE_ACTIVE) {
		return SNS_DDF_EFAIL;
	}

	/* allocate return data work area */
	data_ptr = sns_ddf_memhandler_malloc_ex(
		memhandler, num_sensors * sizeof(sns_ddf_sensor_data_s), state->alps_mag_smgr_handle);
	if (data_ptr == NULL) {
		return SNS_DDF_ENOMEM;
	}

	*data = data_ptr;
	
	for (i = 0; i < num_sensors; i++) {
		data_ptr[i].sensor    = sensors[i];
		data_ptr[i].status    = SNS_DDF_SUCCESS;
		data_ptr[i].timestamp = timestamp;
		switch (sensors[i]) {
		case SNS_DDF_SENSOR_MAG:	/* MagneticFieldSensor */
			data_ptr[i].samples = sns_ddf_memhandler_malloc_ex(
				memhandler, MAG_NUM_AXES * sizeof(sns_ddf_sensor_sample_s),state->alps_mag_smgr_handle);
			if (data_ptr[i].samples == NULL) {
				return SNS_DDF_ENOMEM;
			}

			if (state->alps_mag_ready == false) {
				/* Magnetic field data not prepared */
				/* get Magnetic field data */
				status = alps_dd_hscdtd_get_mag(state);
				if (status != SNS_DDF_SUCCESS) return status;
			} else {
			}

			// Convert sensor data to the phone's coordinate system.
			sns_ddf_map_axes(&state->alps_mag_axes_map, state->data_cache);

			data_ptr[i].samples[0].sample = state->data_cache[HSCDTD_MAG_X];
			data_ptr[i].samples[0].status = (sns_ddf_status_e)(
				((state->out_of_bound_flag >> HSCDTD_MAG_Y) & 0x1) *
				SNS_DDF_EDATA_BOUND);
			data_ptr[i].samples[1].sample = state->data_cache[HSCDTD_MAG_Y];
			data_ptr[i].samples[1].status = (sns_ddf_status_e)(
				((state->out_of_bound_flag >> HSCDTD_MAG_X) & 0x1) *
				SNS_DDF_EDATA_BOUND);
			data_ptr[i].samples[2].sample = state->data_cache[HSCDTD_MAG_Z];
			data_ptr[i].samples[2].status = (sns_ddf_status_e)(
				((state->out_of_bound_flag >> HSCDTD_MAG_Z) & 0x1) *
				SNS_DDF_EDATA_BOUND);
			data_ptr[i].num_samples = MAG_NUM_AXES;

			state->alps_mag_ready = false;
			break;
		default:
			return SNS_DDF_EFAIL;
		}
	}

    sns_dd_maghscdtd008_log_data(state, timestamp);

	return SNS_DDF_SUCCESS;
}


void alps_mag_handle_timer(
			sns_ddf_handle_t dd_handle,
			void* arg)
{
	alps_mag_dd_state_t*  state = dd_handle;
	/* check work memory */
	if (state == NULL) {
		return;
	}
}

void alps_mag_handle_irq(
        sns_ddf_handle_t  dd_handle,
        uint32_t          gpio_num,
        sns_ddf_time_t    timestamp)

{
	alps_mag_dd_state_t*  state = dd_handle;

	/* check work memory */
	if (state == NULL) {
		return;
	}
}
