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
   Copyright (c) 2013-2014 by Qualcomm Technologies, Inc.  All Rights Reserved
   Qualcomm Technologies Proprietary and Confidential
  ------------------------------------------------------------------------*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

when         who      what
----------   ---     -----------------------------------------------------------
09/17/14     vy      Updated memhandler api in probe fn
08/24/14     vy      Used new DDF apis to allocate/free mem supported in uImage
07/29/14     pn      Disabled uImage 
07/27/14     vy      Refactored for uImage 
06/10/13     an      Probe function Update.
04/02/13     AG      Add 14C1 log packets
03/22/13     ps      Add sns_ddf_map_axes
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


const static sns_ddf_sensor_e	alps_mag_sensors[] = {
	SNS_DDF_SENSOR_MAG
};

extern sns_ddf_driver_if_s sns_dd_mag_hscdtd_if;


sns_ddf_status_e alps_dd_hscdtd_reset(alps_mag_dd_state_t* state)
{
	uint8_t count;
	sns_ddf_status_e	status;

	/* CTRL3 HSCDTD Soft Reset Execute */
	state->alps_mag_i2c_buffer[0] = ( CS3_SRST );
	status = sns_ddf_write_port(state->alps_mag_i2c_handle, 
						NKR_CS3, state->alps_mag_i2c_buffer, 1, &count);
	/* wait Reset to Standby 3msec	*/
	sns_ddf_delay(3600);

	/* CTRL1 HSCDTD setting Stand-by Mode: Data Rate 000Hz: Normal State */
	state->alps_mag_i2c_buffer[0] = (           (CS1_ODR & 0x00));
	status = sns_ddf_write_port(state->alps_mag_i2c_handle, 
						NKR_CS1, state->alps_mag_i2c_buffer, 1, &count);

	/* CTRL2 HSCDTD setting  Data Redy Enabled: Data Redy Active Lo */
	state->alps_mag_i2c_buffer[0] = ( CS2_DEN          );
	status = sns_ddf_write_port(state->alps_mag_i2c_handle, 
						NKR_CS2, state->alps_mag_i2c_buffer, 1, &count);

	/* CTRL4 HSCDTD setting  Output VAlue 14bit: */
	state->alps_mag_i2c_buffer[0] = ( (CS4_MMD & 0x80) );
	status = sns_ddf_write_port(state->alps_mag_i2c_handle, 
						NKR_CS4, state->alps_mag_i2c_buffer, 1, &count);

	state->resolution_adc      = ALPS_MAG_SENSOR_MAG_RESOLUTION_14;

	state->alps_mag_powerstate = SNS_DDF_POWERSTATE_LOWPOWER;

	return status;
}

sns_ddf_status_e alps_dd_hscdtd_set_mode(sns_ddf_handle_t dd_handle, sns_ddf_powerstate_e power_state)
{
	uint8_t count;
	sns_ddf_status_e      status;
	alps_mag_dd_state_t*  state = dd_handle;

	switch (power_state) {
	case SNS_DDF_POWERSTATE_ACTIVE:
		/* CTRL1 HSCDTD setting Active Mode: Data Rate 100Hz: Normal State */
		state->alps_mag_i2c_buffer[0] = (CS1_PC1 | (CS1_ODR & 0x18));
		status = sns_ddf_write_port(state->alps_mag_i2c_handle, 
						NKR_CS1, state->alps_mag_i2c_buffer, 1, &count);
		sns_ddf_delay(30);
		state->alps_mag_powerstate = SNS_DDF_POWERSTATE_ACTIVE;
		status = SNS_DDF_SUCCESS;
		break;
	case SNS_DDF_POWERSTATE_LOWPOWER:
		/* CTRL1 HSCDTD setting Stand-by Mode: Data Rate 000Hz: Normal State */
		state->alps_mag_i2c_buffer[0] = (         (CS1_ODR & 0x00));
		status = sns_ddf_write_port(state->alps_mag_i2c_handle, 
						NKR_CS1, state->alps_mag_i2c_buffer, 1, &count);
		sns_ddf_delay(5);
		state->alps_mag_powerstate = SNS_DDF_POWERSTATE_LOWPOWER;
		status = SNS_DDF_SUCCESS;
		break;
	default:
		status = SNS_DDF_EINVALID_PARAM;
	}
	return status;
}


static sns_ddf_status_e alps_dd_hscdtd_run_test(sns_ddf_handle_t dd_handle, 
		sns_ddf_test_e test, uint32_t* err)
{
	uint8_t count;
	sns_ddf_status_e status, status2;
	alps_mag_dd_state_t*  state = dd_handle;

	*err = 0;
	status = SNS_DDF_EFAIL;
	status2  = SNS_DDF_EFAIL;

	switch (test) {
	case SNS_DDF_TEST_SELF:         /**< Hardware self test. */
		/* Control resister1 backup  */
		status = sns_ddf_read_port(state->alps_mag_i2c_handle, 
								NKR_CS1, state->alps_mag_ctrl, 1, &count);
		if (status != SNS_DDF_SUCCESS) break;
		sns_ddf_delay(1000);
		/* --------------- Self test B --------------- */
		/* dumy read  */
		status = sns_ddf_read_port(state->alps_mag_i2c_handle, 
								NKR_STB, state->alps_mag_i2c_buffer, 1, &count);
		if (status != SNS_DDF_SUCCESS) return status;
		sns_ddf_delay(1000);
		/* dumy read  */
		status = sns_ddf_read_port(state->alps_mag_i2c_handle, 
								NKR_STB, state->alps_mag_i2c_buffer, 1, &count);
		if (status != SNS_DDF_SUCCESS) return status;
		sns_ddf_delay(1000);
	    /* Get inital Self test B register  */
		status = sns_ddf_read_port(state->alps_mag_i2c_handle, 
								NKR_STB, state->alps_mag_i2c_buffer, 1, &count);
		if (status != SNS_DDF_SUCCESS) return status;
		sns_ddf_delay(1000);
		if (state->alps_mag_i2c_buffer[0] != STR_NORMAL) break;
		/* Active mode (Force state)  */
		state->alps_mag_i2c_buffer[0] = (CS1_PC1 | CS1_FS1);
		status = sns_ddf_write_port(state->alps_mag_i2c_handle, 
							NKR_CS1, state->alps_mag_i2c_buffer, 1, &count);
		if (status != SNS_DDF_SUCCESS) break;
	    /* Exec Self test B   */
		state->alps_mag_i2c_buffer[0] = (CS3_STCB);
		status = sns_ddf_write_port(state->alps_mag_i2c_handle, 
								NKR_CS3, state->alps_mag_i2c_buffer, 1, &count);
		if (status != SNS_DDF_SUCCESS) break;
		sns_ddf_delay(6000);
	    /* Get 1st Self test B register  */
		status = sns_ddf_read_port(state->alps_mag_i2c_handle, 
								NKR_STB, state->alps_mag_i2c_buffer, 1, &count);
		if (status != SNS_DDF_SUCCESS) break;
		if (state->alps_mag_i2c_buffer[0] != STR_SELFTEST) break;
		sns_ddf_delay(3000);
	    /* Get 2nd Self test B register  */
		status = sns_ddf_read_port(state->alps_mag_i2c_handle, 
								NKR_STB, state->alps_mag_i2c_buffer, 1, &count);
		if (status != SNS_DDF_SUCCESS) break;
		if (state->alps_mag_i2c_buffer[0] != STR_NORMAL) break;
		
		/* Control resister1 set  */
		status = sns_ddf_write_port(state->alps_mag_i2c_handle, 
								NKR_CS1, state->alps_mag_ctrl, 1, &count);
		if (status != SNS_DDF_SUCCESS) break;
		status2 = SNS_DDF_SUCCESS;
		break;
	case SNS_DDF_TEST_IRQ:          /**< Interrupt test. */
		status2 = SNS_DDF_EINVALID_TEST;
		break;
	case SNS_DDF_TEST_CONNECTIVITY:  /**< Basic connectivity test. */
		{
			/* Get More Info register */
			uint16_t minfo;
			status = sns_ddf_read_port(state->alps_mag_i2c_handle, 
									NKR_MIF, state->alps_mag_i2c_buffer, 2, &count);
			if (status != SNS_DDF_SUCCESS) break;
			minfo = (((uint16_t)state->alps_mag_i2c_buffer[1])<<8) | ((uint16_t)state->alps_mag_i2c_buffer[0]);
			if (minfo != INF_TE) break;
			status2 = SNS_DDF_SUCCESS;
		}
		break;
	default:
		status = SNS_DDF_EINVALID_ATTR;
		break;
	}
	
	if (status == SNS_DDF_SUCCESS)
		return status2;
	else
		return status;
}

sns_ddf_status_e alps_dd_hscdtd_set_resolution(sns_ddf_handle_t dd_handle, uint8_t resolution_idx)
{
	uint8_t count;
	sns_ddf_status_e      status;
	alps_mag_dd_state_t*  state = dd_handle;

	if (resolution_idx >= ALPS_MAG_SENSOR_MAX_RESOLUTION) {
		return SNS_DDF_EINVALID_PARAM;
	}

	/* CTRL4 read */
	status = sns_ddf_read_port(state->alps_mag_i2c_handle, 
					NKR_CS4, state->alps_mag_i2c_buffer, 1, &count);

	sns_ddf_delay(30);
	
	if (resolution_idx == ALPS_MAG_SENSOR_MAG_RESOLUTION_14) {
		state->alps_mag_i2c_buffer[0] &= ~(CS4_RS);		// 14bit
	} else {
		state->alps_mag_i2c_buffer[0] |= (CS4_RS);		// 15bit
	}

	/* CTRL4 RSbit */
	status = sns_ddf_write_port(state->alps_mag_i2c_handle, 
						NKR_CS4, state->alps_mag_i2c_buffer, 1, &count);
	state->resolution_adc = resolution_idx;

	return status;
}


sns_ddf_status_e alps_mag_init(
			sns_ddf_handle_t* dd_handle_ptr,
			sns_ddf_handle_t smgr_handle,
			sns_ddf_nv_params_s* nv_params,
			sns_ddf_device_access_s device_info[],
			uint32_t num_devices,
			sns_ddf_memhandler_s* memhandler,
			sns_ddf_sensor_e** sensors,
			uint32_t* num_sensors)
{
	sns_ddf_status_e				status;
    alps_mag_dd_state_t*            alps_state_ptr;
	uint32_t	                    err;

    /* Update smgr sensor data for the driver to indicate uImage support */
    sns_ddf_smgr_set_uimg_refac(smgr_handle);
    
    *dd_handle_ptr = NULL;
    *sensors       = NULL;

    if(sns_ddf_malloc_ex((void **)&alps_state_ptr, sizeof(alps_mag_dd_state_t), smgr_handle) != SNS_DDF_SUCCESS) 
    { 
      return SNS_DDF_ENOMEM;
    }
    memset(alps_state_ptr, 0, sizeof(alps_mag_dd_state_t));

    if((status = sns_ddf_open_port(&(alps_state_ptr->alps_mag_i2c_handle), &(device_info->port_config)))!= SNS_DDF_SUCCESS)
    {
      sns_ddf_mfree_ex(alps_state_ptr, smgr_handle);
      return status;
    }

	alps_state_ptr->alps_mag_ready  = false;
	
	/* parameter bakup */
	alps_state_ptr->alps_mag_smgr_handle = smgr_handle;
	alps_state_ptr->alps_mag_num_devices = num_devices;
	alps_state_ptr->alps_mag_memhandler  = memhandler;

	/* return sensors parameter */
	*num_sensors   = 1;
	*sensors  = (sns_ddf_sensor_e *)alps_mag_sensors;

	sns_ddf_axes_map_init(
	&alps_state_ptr->alps_mag_axes_map, ((nv_params != NULL) ? nv_params->data : NULL));

	*dd_handle_ptr = alps_state_ptr;

	/* initializ HSCDTD (MagneticFieldSensor) */
	status = alps_dd_hscdtd_reset(alps_state_ptr);
	if (status != SNS_DDF_SUCCESS) {
        sns_ddf_close_port(alps_state_ptr->alps_mag_i2c_handle);
        sns_ddf_mfree_ex(alps_state_ptr, smgr_handle);
		return status;
	}
	
	printf("init Mag\r\n");


#if 1
/**
 * Factory tests.
 */
	status = alps_mag_run_test((sns_ddf_handle_t)alps_state_ptr,
							SNS_DDF_SENSOR_MAG, SNS_DDF_TEST_SELF, &err);
	printf("SNS_DDF_TEST_SELF: %d \r\n", status);
	status = alps_mag_run_test((sns_ddf_handle_t)alps_state_ptr,
							SNS_DDF_SENSOR_MAG, SNS_DDF_TEST_IRQ, &err);
	printf("SNS_DDF_TEST_IRQ: %d \r\n", status);
	status = alps_mag_run_test((sns_ddf_handle_t)alps_state_ptr,
							SNS_DDF_SENSOR_MAG, SNS_DDF_TEST_CONNECTIVITY, &err);
	printf("SNS_DDF_TEST_CONNECTIVITY: %d \r\n", status);
#endif

	return SNS_DDF_SUCCESS;
}


sns_ddf_status_e alps_mag_set_attrib(
			sns_ddf_handle_t dd_handle,
			sns_ddf_sensor_e sensor,
			sns_ddf_attribute_e attrib,
			void* value)
{
	alps_mag_dd_state_t*  state = dd_handle;

	/* check work memory */
	if (state == NULL) {
		return SNS_DDF_ENOMEM;
	}

	switch(attrib) {
	case SNS_DDF_ATTRIB_POWER_STATE:
	{
		sns_ddf_powerstate_e* power_state = value;
		return alps_dd_hscdtd_set_mode(dd_handle, *power_state);
	}
	case SNS_DDF_ATTRIB_RANGE:
	case SNS_DDF_ATTRIB_RESOLUTION_ADC:
	{
		uint8_t* resolution_idx = value;
		return alps_dd_hscdtd_set_resolution(dd_handle, *resolution_idx);
	}
	default:
		return SNS_DDF_EINVALID_ATTR;
	}
}


sns_ddf_status_e alps_mag_get_attrib(
			sns_ddf_handle_t dd_handle,
			sns_ddf_sensor_e sensor,
			sns_ddf_attribute_e attrib,
			sns_ddf_memhandler_s* memhandler,
			void** value,
			uint32_t* num_elems)
{
	alps_mag_dd_state_t*  state = dd_handle;

	/* check work memory */
	if (state == NULL) {
		return SNS_DDF_ENOMEM;
	}
	
	switch (attrib) {
	case SNS_DDF_ATTRIB_POWER_STATE:
	{
		sns_ddf_powerstate_e* power = sns_ddf_memhandler_malloc_ex(
			memhandler, sizeof(sns_ddf_powerstate_e),state->alps_mag_smgr_handle);
		if (power == NULL) {
			return SNS_DDF_ENOMEM;
		}
		*value = (void *)&(state->alps_mag_powerstate);
		*num_elems = 1;
		break;
	}
	case SNS_DDF_ATTRIB_POWER_INFO:
	{
		sns_ddf_power_info_s* power = sns_ddf_memhandler_malloc_ex(
			memhandler, sizeof(sns_ddf_power_info_s),state->alps_mag_smgr_handle);
		if (power == NULL) {
			return SNS_DDF_ENOMEM;
		}
		power->active_current   = ALPS_MAG_SENSOR_POWER_ACTIVE;
		power->lowpower_current = ALPS_MAG_SENSOR_LOWPOWER;
		*value     = power;
		*num_elems = 1;
		return SNS_DDF_SUCCESS;
	}
	case SNS_DDF_ATTRIB_RANGE:
	{
		switch (sensor) {
		case SNS_DDF_SENSOR_MAG:
		{
			sns_ddf_range_s* ranges = sns_ddf_memhandler_malloc_ex(
				memhandler, ALPS_MAG_SENSOR_MAX_RANGE * sizeof(sns_ddf_range_s),state->alps_mag_smgr_handle);
		
			if (ranges == NULL) {
				return SNS_DDF_ENOMEM;
			}
			ranges[0].min = ALPS_MAG_SENSOR_RANGE14_MIN;
			ranges[0].max = ALPS_MAG_SENSOR_RANGE14_MAX;
			ranges[1].min = ALPS_MAG_SENSOR_RANGE15_MIN;
			ranges[1].max = ALPS_MAG_SENSOR_RANGE15_MAX;
			
			*value     = ranges;
			*num_elems = ALPS_MAG_SENSOR_MAX_RANGE;
			return SNS_DDF_SUCCESS;
		}
		default:
			return SNS_DDF_EINVALID_ATTR;
		}
	}
	case SNS_DDF_ATTRIB_RESOLUTION:
	{
		switch (sensor) {
		case SNS_DDF_SENSOR_MAG:
		{
			sns_ddf_resolution_t *res = sns_ddf_memhandler_malloc_ex(
					memhandler ,sizeof(sns_ddf_resolution_t),state->alps_mag_smgr_handle);
			if (res == NULL) {
				return SNS_DDF_ENOMEM;
			}
			*res = FX_FLTTOFIX_Q16(ALPS_MAG_SENSOR_RESOLUTOIN);

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
		case SNS_DDF_SENSOR_MAG:
		{
			sns_ddf_resolution_adc_s* res_adc = sns_ddf_memhandler_malloc_ex(memhandler,
						ALPS_MAG_SENSOR_MAX_RESOLUTION * sizeof(sns_ddf_resolution_adc_s), state->alps_mag_smgr_handle);
		
			if (res_adc == NULL) {
				return SNS_DDF_ENOMEM;
			}
			res_adc[0].bit_len = ALPS_MAG_SENSOR_MAG_BIT_LEN14;
			res_adc[1].bit_len = ALPS_MAG_SENSOR_MAG_BIT_LEN15;
			
			res_adc[0].max_freq = ALPS_MAG_SENSOR_MAG_MAX_FREQ;
			res_adc[1].max_freq = ALPS_MAG_SENSOR_MAG_MAX_FREQ;
			
			*value     = res_adc;
			*num_elems = ALPS_MAG_SENSOR_MAX_RESOLUTION;
			return SNS_DDF_SUCCESS;
		}
		default:
			return SNS_DDF_EINVALID_ATTR;
		}
	}
	case SNS_DDF_ATTRIB_DELAYS:
	{
		sns_ddf_delays_s *delays = sns_ddf_memhandler_malloc_ex(
				memhandler, sizeof(sns_ddf_delays_s),state->alps_mag_smgr_handle);
		if (delays == NULL) {
			return SNS_DDF_ENOMEM;
		}
		delays->time_to_active = ALPS_MAG_SENSOR_DELAY_ACTIVE;
		delays->time_to_data   = ALPS_MAG_SENSOR_DELAY_DATA;
		
		*value     = delays;
		*num_elems = 1;
		return SNS_DDF_SUCCESS;
	}

	case SNS_DDF_ATTRIB_DRIVER_INFO:
	{
		sns_ddf_driver_info_s *info = sns_ddf_memhandler_malloc_ex(
				memhandler,sizeof(sns_ddf_driver_info_s),state->alps_mag_smgr_handle);
		if (info == NULL) {
			return SNS_DDF_ENOMEM;
		}

		info->name    = ALPS_MAG_SENSOR_DRIVER_NAME;
		info->version = ALPS_MAG_SENSOR_DRIVER_VER;
		
		*value     = info;
		*num_elems = 1;
		return SNS_DDF_SUCCESS;
	}

	case SNS_DDF_ATTRIB_DEVICE_INFO:
	{
		sns_ddf_device_info_s *info = sns_ddf_memhandler_malloc_ex(
				memhandler, sizeof(sns_ddf_device_info_s), state->alps_mag_smgr_handle);
		if (info == NULL) {
			return SNS_DDF_ENOMEM;
		}
		info->name    = ALPS_MAG_SENSOR_DEVICE_NAME;
		info->vendor  = ALPS_MAG_SENSOR_DEVICE_VENDOR;
		info->model   = ALPS_MAG_SENSOR_DEVICE_MODEL;
		info->version = ALPS_MAG_SENSOR_DEVICE_VER;
		
		*value     = info;
		*num_elems = 1;
		return SNS_DDF_SUCCESS;
	}
	default:
		return SNS_DDF_EINVALID_ATTR;
	}
	return SNS_DDF_SUCCESS;
}


sns_ddf_status_e alps_mag_reset(sns_ddf_handle_t dd_handle)
{
	sns_ddf_status_e status;
	alps_mag_dd_state_t*  state = dd_handle;

	/* check work memory */
	if (state == NULL) {
		return SNS_DDF_ENOMEM;
	}

	status = alps_dd_hscdtd_reset(state);
	if (status != SNS_DDF_SUCCESS) {
		return status;
	}

	return SNS_DDF_SUCCESS;
}

sns_ddf_status_e alps_mag_run_test(
			sns_ddf_handle_t dd_handle,
			sns_ddf_sensor_e sensor,
			sns_ddf_test_e test,
			uint32_t* err)
{
	sns_ddf_status_e status;
	alps_mag_dd_state_t*  state = dd_handle;

	/* check work memory */
	/*testtesttest*/
	if (state == NULL) {
		return SNS_DDF_ENOMEM;
	}

	status = alps_dd_hscdtd_run_test(state, test, err);
	if (status != SNS_DDF_SUCCESS) {
		return status;
	}

	return SNS_DDF_SUCCESS;

}

/*=============================================================================
 * Probe function for Geomagnetic sensor.
 =============================================================================*/
sns_ddf_status_e alps_mag_probe(
  sns_ddf_device_access_s* device_info,
  sns_ddf_memhandler_s*    memhandler,
  uint32_t*                num_sensors,
  sns_ddf_sensor_e**       sensors )
{
  sns_ddf_status_e status;
  sns_ddf_handle_t port_handle;
  uint8_t i2c_buff;
  uint8_t bytes_read;

  *num_sensors = 0;
  *sensors = NULL;

  status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
  if(status != SNS_DDF_SUCCESS)
  {
    return status;
  }

  /* Read & Verify WHO AM I register */
  status = sns_ddf_read_port( port_handle,
                              NKR_WHO,
                              &i2c_buff,
                              1,
                              &bytes_read );
                             
  if(status != SNS_DDF_SUCCESS || bytes_read != 1)
  {
    sns_ddf_close_port( port_handle );
    return status;
  }
  if( i2c_buff != NKR_CHIP_ID )
  {
    /* Incorrect value. Return now with nothing detected */
    sns_ddf_close_port( port_handle );
    return SNS_DDF_SUCCESS;
  }

  /* The Chip ID register content are correct. This is probably HSCDTD008 magnetometer */
  *num_sensors = 1;
  *sensors = sns_ddf_memhandler_malloc( memhandler,
               sizeof(sns_ddf_sensor_e) * *num_sensors );
  if( *sensors != NULL )
  {
    (*sensors)[0] = SNS_DDF_SENSOR_MAG;
    status = SNS_DDF_SUCCESS;
  } 
  else 
  {
    status = SNS_DDF_ENOMEM;
  }
  sns_ddf_close_port( port_handle );
  return status;

} /* End alps_mag_probe */



