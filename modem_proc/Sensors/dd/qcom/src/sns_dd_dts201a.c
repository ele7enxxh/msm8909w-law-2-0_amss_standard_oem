/*==============================================================================    

* Copyright (c) 2014, Partron Co., Ltd. - Sensor Lab.
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*        notice, this list of conditions and the following disclaimer in the
*        documentation and/or other materials provided with the distribution.
*     3. Neither the name of the Partron Co., Ltd. nor the
*        names of its contributors may be used to endorse or promote products
*        derived from this software without specific prior written permission.
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
  ******************************************************************************/
 
 /*******************************************************************************
   Copyright (c) 2015 Qualcomm Technologies, Inc.	All Rights Reserved.
   Qualcomm Technologies Proprietary and Confidential
 *******************************************************************************/
 
 /*==============================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

    when         who     what, where, why
----------   ---     -----------------------------------------------------------
09-Jul-2015 AH    Fix compiler warnings
03-Apr-2015 AH    Qualcomm specific changes
28-Nov-2014 msahn Initial release

==============================================================================*/

/*============================================================================
INCLUDE FILES
============================================================================*/
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_dd_dts201a.h"
#ifdef COMPILE_FOR_HD22
#include "qurt_elite_diag.h"
#endif
#include "math.h"

/*============================================================================
GLOBAL VARIABLE DEFINITIONS
============================================================================*/

#ifdef ONE_MODE
int32_t coefficient_ary[14]={0, };
#else	/* TWO MODE */
int32_t coefficient_n[14]={0, };
int32_t coefficient_h[14]={0, };
#endif

uint8_t o_avg_cnt = 0;

static const uint32_t  sns_dd_dts_range[2] =
{
	DTS_OBJECT_RANGE_NORMAL,
	DTS_OBJECT_RANGE_HUMAN
};

static sns_ddf_odr_t odr_list[] =
{
	DTS_ODR_1HZ_1000MS,
	DTS_ODR_5HZ_200MS,
	DTS_ODR_10HZ_100MS,
	DTS_ODR_20HZ_50MS
};

static uint32_t odr_list_size = sizeof(odr_list)/sizeof(odr_list[0]);

/*============================================================================
STATIC FUNCTION PROTOTYPES
============================================================================*/


/*===========================================================================
FUNCTION:   
===========================================================================*/
/*!

*/
/*=========================================================================*/

sns_ddf_status_e dts201a_write_byte_block
    (
    sns_ddf_handle_t port_handle,
    uint8_t reg_addr, 
    uint8_t *wbuf,
    uint8_t len    
    )
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	uint8_t out;
	uint8_t retry = 3;

	while (retry--) {
		status = sns_ddf_write_port(
			port_handle,
			reg_addr,
			wbuf,
			len,
			&out);
		if(status == SNS_DDF_SUCCESS)
		{
			return status;
		}
	}

	return status;
}

sns_ddf_status_e dts201a_read_byte_block
    (
    sns_ddf_handle_t port_handle,
    uint8_t *rbuf, 
    uint8_t len
    )
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	uint8_t out;
	uint8_t retry = 3;

	while (retry--) {
		status = sns_ddf_simple_read_port(
			port_handle,
			rbuf, 
			len,
			&out);
		if (status == SNS_DDF_SUCCESS)
		{
		 	return status;
		}
	}

	return status;
}

#ifdef ONE_MODE
static sns_ddf_status_e dts201a_get_coefficient
    (
    sns_dd_dts201a_state_t *state,
    int32_t *coefficients
    )
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	uint8_t w_buffer[2] = {0x0, 0x0};
	uint8_t r_buffer[3] = {0x0, };
	uint8_t i, j;

	if (!state || !coefficients)
	{
		DD_DTS201A_MSG_0(ERROR, "dts201a_get_coefficient : Invalid argument");
		return SNS_DDF_EINVALID_PARAM;
	}

	for(i=STX; i<=ETX; i+=3)
	{
		for(j=0; j<2; j++)
		{
			status = dts201a_write_byte_block(state->port_handle, i+j, w_buffer, 2);
			if(status != SNS_DDF_SUCCESS) return SNS_DDF_EDEVICE;
			sns_ddf_delay(2500);	// 2.5msec
			status = dts201a_read_byte_block(state->port_handle, r_buffer, 3);
			if(status != SNS_DDF_SUCCESS) return SNS_DDF_EDEVICE;

			coefficients[(i+j-STX)-(uint8_t)((i-STX)/3)] = (int32_t)(((r_buffer[1]<<8) | r_buffer[2])&0x0000ffff);

//			DD_DTS201A_MSG_2(HIGH,"coefficients[%d]=0x%x\n", (i+j-STX)-(uint8_t)((i-STX)/3), coefficients[(i+j-STX)-(uint8_t)((i-STX)/3)]);
		}

		status = dts201a_write_byte_block(state->port_handle, i+2, w_buffer, 2);
		if(status != SNS_DDF_SUCCESS) return SNS_DDF_EDEVICE;
		sns_ddf_delay(2500);	// 2.5msec
		status = dts201a_read_byte_block(state->port_handle, r_buffer, 3);
		if(status != SNS_DDF_SUCCESS) return SNS_DDF_EDEVICE;

		coefficients[(i+0-STX)-(uint8_t)((i-STX)/3)] |= (int32_t)(((int8_t)r_buffer[2]<<16)&0xffff0000);
		coefficients[(i+1-STX)-(uint8_t)((i-STX)/3)] |= (int32_t)(((int8_t)r_buffer[1]<<16)&0xffff0000);

		if(coefficients[(i+0-STX)-(uint8_t)((i-STX)/3)] & 0x800000) {
			coefficients[(i+0-STX)-(uint8_t)((i-STX)/3)] = (coefficients[(i+0-STX)-(uint8_t)((i-STX)/3)] & 0x007fffff)-1;
			coefficients[(i+0-STX)-(uint8_t)((i-STX)/3)] ^= 0xffffffff;
		}
		if(coefficients[(i+1-STX)-(uint8_t)((i-STX)/3)] & 0x800000) {
			coefficients[(i+1-STX)-(uint8_t)((i-STX)/3)] = (coefficients[(i+1-STX)-(uint8_t)((i-STX)/3)] & 0x007fffff)-1;
			coefficients[(i+1-STX)-(uint8_t)((i-STX)/3)] ^= 0xffffffff;							
		}

		DD_DTS201A_MSG_2(HIGH,"coefficients[%d]=%d\n", (i+0-STX)-(uint8_t)((i-STX)/3), coefficients[(i+0-STX)-(uint8_t)((i-STX)/3)]);
		DD_DTS201A_MSG_2(HIGH,"coefficients[%d]=%d\n", (i+1-STX)-(uint8_t)((i-STX)/3), coefficients[(i+1-STX)-(uint8_t)((i-STX)/3)]);
	}

	return status;
}
#else	/* TWO MODE */
static sns_ddf_status_e dts201a_get_coefficient
    (
    sns_dd_dts201a_state_t *state,
    int32_t *coefficients1,
    int32_t *coefficients2
    )
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	uint8_t w_buffer[2] = {0x0, 0x0};
	uint8_t r_buffer[3] = {0x0, };
	int32_t temp[3] = {0x0, };
	uint8_t i;

	if (!state || !coefficients1 || !coefficients2)
	{
		DD_DTS201A_MSG_0(ERROR, "dts201a_get_coefficient : Invalid argument");
		return SNS_DDF_EINVALID_PARAM;
	}

	/* normal coefficient */
	for(i=STX_N1; i<=ETX_N1; i++)
	{
		status = dts201a_write_byte_block(state->port_handle, i, w_buffer, 2);
		if(status != SNS_DDF_SUCCESS) return SNS_DDF_EDEVICE;
		sns_ddf_delay(2500);	// 2.5msec
		status = dts201a_read_byte_block(state->port_handle, r_buffer, 3);
		if(status != SNS_DDF_SUCCESS) return SNS_DDF_EDEVICE;

		coefficients1[(i-STX_N1)] = (int32_t)(((r_buffer[1]<<8) | r_buffer[2])&0x0000ffff);
//		DD_DTS201A_MSG_2(HIGH,"coefficients1[%d]=0x%04x\n", (i-STX_N1), coefficients1[(i-STX_N1)]);
	}

	for(i=STX_M1; i<=ETX_M1; i++)
	{
		status = dts201a_write_byte_block(state->port_handle, i, w_buffer, 2);
		if(status != SNS_DDF_SUCCESS) return SNS_DDF_EDEVICE;
		sns_ddf_delay(2500);	// 2.5msec
		status = dts201a_read_byte_block(state->port_handle, r_buffer, 3);
		if(status != SNS_DDF_SUCCESS) return SNS_DDF_EDEVICE;

		temp[(i-STX_M1)] = (int32_t)(((r_buffer[1]<<8) | r_buffer[2])&0x0000ffff);
//		DD_DTS201A_MSG_2(HIGH,"temp[%d]=0x%04x\n", (i-STX_M1), temp[(i-STX_M1)]);
	}

	coefficients1[0] = (coefficients1[0] | ((temp[0]&0x0007)<<16))<<5;
	coefficients1[1] = (coefficients1[1] | ((temp[0]&0x0038)<<13))<<5;
	coefficients1[2] = (coefficients1[2] | ((temp[0]&0x01C0)<<10))<<5;
	coefficients1[3] = (coefficients1[3] | ((temp[0]&0x0E00)<<7))<<5;
	coefficients1[4] = (coefficients1[4] | ((temp[0]&0x7000)<<4))<<5;
	coefficients1[5] = (coefficients1[5] | ((temp[1]&0x0007)<<16))<<5;
	coefficients1[6] = (coefficients1[6] | ((temp[1]&0x0038)<<13))<<5;
	coefficients1[7] = (coefficients1[7] | ((temp[1]&0x01C0)<<10))<<5;
	coefficients1[8] = (coefficients1[8] | ((temp[1]&0x0E00)<<7))<<5;
	coefficients1[9] = (coefficients1[9] | ((temp[1]&0x7000)<<4))<<5;
	coefficients1[10] = (coefficients1[10] | ((temp[2]&0x0007)<<16))<<5;
	coefficients1[11] = (coefficients1[11] | ((temp[2]&0x0038)<<13))<<5;
	coefficients1[12] = (coefficients1[12] | ((temp[2]&0x01C0)<<10))<<5;
	coefficients1[13] = coefficients1[13]<<5;

	for(i=0; i<14; i++)
	{
		if(coefficients1[i] & 0x800000)
		{
			coefficients1[i] = (coefficients1[i] & 0x007fffff)-1;
			coefficients1[i] ^= 0xffffffff;
		}
		DD_DTS201A_MSG_3(HIGH,"coefficients1[%d]=0x%08x=%d\n", i, coefficients1[i], coefficients1[i]);
	}

	/* human coefficient */
	for(i=STX_H2; i<=ETX_H2; i++)
	{
		status = dts201a_write_byte_block(state->port_handle, i, w_buffer, 2);
		if(status != SNS_DDF_SUCCESS) return SNS_DDF_EDEVICE;
		sns_ddf_delay(2500);	// 2.5msec
		status = dts201a_read_byte_block(state->port_handle, r_buffer, 3);
		if(status != SNS_DDF_SUCCESS) return SNS_DDF_EDEVICE;

		coefficients2[(i-STX_H2)] = (int32_t)(((r_buffer[1]<<8) | r_buffer[2])&0x0000ffff);
//		DD_DTS201A_MSG_2(HIGH,"coefficients2[%d]=0x%04x\n", (i-STX_H2), coefficients2[(i-STX_H2)]);
	}

	for(i=STX_M2; i<=ETX_M2; i++)
	{
		status = dts201a_write_byte_block(state->port_handle, i, w_buffer, 2);
		if(status != SNS_DDF_SUCCESS) return SNS_DDF_EDEVICE;
		sns_ddf_delay(2500);	// 2.5msec
		status = dts201a_read_byte_block(state->port_handle, r_buffer, 3);
		if(status != SNS_DDF_SUCCESS) return SNS_DDF_EDEVICE;

		temp[(i-STX_M2)] = (int32_t)(((r_buffer[1]<<8) | r_buffer[2])&0x0000ffff);
//		DD_DTS201A_MSG_2(HIGH,"temp[%d]=0x%04x\n", (i-STX_M2), temp[(i-STX_M2)]);
	}

	coefficients2[0] = (coefficients2[0] | ((temp[0]&0x0007)<<16))<<5;
	coefficients2[1] = (coefficients2[1] | ((temp[0]&0x0038)<<13))<<5;
	coefficients2[2] = (coefficients2[2] | ((temp[0]&0x01C0)<<10))<<5;
	coefficients2[3] = (coefficients2[3] | ((temp[0]&0x0E00)<<7))<<5;
	coefficients2[4] = (coefficients2[4] | ((temp[0]&0x7000)<<4))<<5;
	coefficients2[5] = (coefficients2[5] | ((temp[1]&0x0007)<<16))<<5;
	coefficients2[6] = (coefficients2[6] | ((temp[1]&0x0038)<<13))<<5;
	coefficients2[7] = (coefficients2[7] | ((temp[1]&0x01C0)<<10))<<5;
	coefficients2[8] = (coefficients2[8] | ((temp[1]&0x0E00)<<7))<<5;
	coefficients2[9] = (coefficients2[9] | ((temp[1]&0x7000)<<4))<<5;
	coefficients2[10] = (coefficients2[10] | ((temp[2]&0x0007)<<16))<<5;
	coefficients2[11] = (coefficients2[11] | ((temp[2]&0x0038)<<13))<<5;
	coefficients2[12] = (coefficients2[12] | ((temp[2]&0x01C0)<<10))<<5;
	coefficients2[13] = coefficients2[13]<<5;

	for(i=0; i<14; i++)
	{
		if(coefficients2[i] & 0x800000)
		{
			coefficients2[i] = (coefficients2[i] & 0x007fffff)-1;
			coefficients2[i] ^= 0xffffffff;
		}
		DD_DTS201A_MSG_3(HIGH,"coefficients2[%d]=0x%08x=%d\n", i, coefficients2[i], coefficients2[i]);
	}

	return status;
}
#endif

static sns_ddf_status_e dts201a_config_range /* not use */
(
    sns_dd_dts201a_state_t *state
)
{
	DD_DTS201A_MSG_1(HIGH,"set_range = %d\n", state->range;);
	return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION:   sns_dd_dts201a_reset

===========================================================================*/
/*!
@brief Resets the driver and device so they return to the state they were 
in after init() was called.

@detail
Resets  the driver state structure

@param[in] handle  Handle to a driver instance.

@return Success if the driver was able to reset its state and the device.
Otherwise a specific error code is returned. 
*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_dts201a_reset
    (
    sns_ddf_handle_t dd_handle
    )
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	sns_dd_dts201a_state_t *state = (sns_dd_dts201a_state_t *)dd_handle;

	uint8_t i;
	uint8_t w_buffer[2] = {0x0, 0x0};
	uint8_t r_buffer[3] = {0x0, };
	uint16_t custom_id[2]={0x0, };

	if (!state)
	{
		DD_DTS201A_MSG_0(ERROR, "sns_dd_dts201a_reset : Invalid argument");
		return SNS_DDF_EINVALID_PARAM;
	}

	state->object_odr = 0;
	state->ambient_odr = 0;
	state->range = DTS_OBJECT_RANGE_NORMAL;
	state->out_of_bound_flag = 0;
	state->object_temp_enabled = false;
	state->ambient_temp_enabled = false;

	o_avg_cnt = 0;
	for(i=0; i<DTS_MOVING_AVG; i++) state->out.object_ary[i]=0;

	/* Check the DTS_CUSTOM_ID */
	status = dts201a_write_byte_block(state->port_handle, DTS_CUST_ID0, w_buffer, 2);
	sns_ddf_delay(2500);	// 2.5msec
	status = dts201a_read_byte_block(state->port_handle, r_buffer, 3);
	custom_id[0] = ((r_buffer[1]<<8) | r_buffer[2]);

	status = dts201a_write_byte_block(state->port_handle, DTS_CUST_ID1, w_buffer, 2);
	sns_ddf_delay(2500);	// 2.5msec
	status = dts201a_read_byte_block(state->port_handle, r_buffer, 3);
	custom_id[1] = ((r_buffer[1]<<8) | r_buffer[2]);

	if ((custom_id[1] != DTS201A_ID1) && (custom_id[0] != DTS201A_ID0))
	{
		DD_DTS201A_MSG_0(ERROR, "reset : CUSTOM_ID error");
		return SNS_DDF_EDEVICE;
	}

	return status;
}

/*===========================================================================

FUNCTION:   sns_dd_dts201a_init

===========================================================================*/
/*!
@brief Initializes the Ambient Light Sensing and Proximity device driver
Allocates a handle to a driver instance, opens a communication port to 
associated devices, configures the driver and devices, and places 
the devices in the default power state. Returns the instance handle along 
with a list of supported sensors. This function will be called at init 
time.

@param[out] dd_handle_ptr  Pointer that this function must malloc and 
populate. This is a handle to the driver
instance that will be passed in to all other
functions. NB: Do not use @a memhandler to
allocate this memory.
@param[in]  smgr_handle    Handle used to identify this driver when it 
calls into Sensors Manager functions.
@param[in]  nv_params      NV parameters retrieved for the driver.
@param[in]  device_info    Access info for physical devices controlled by 
this driver. Used to configure the bus
and talk to the devices.
@param[in]  num_devices    Number of elements in @a device_info. 
@param[in]  memhandler     Memory handler used to dynamically allocate 
output parameters, if applicable. NB: Do not
use memhandler to allocate memory for
@a dd_handle_ptr.
@param[out] sensors        List of supported sensors, allocated, 
populated, and returned by this function.
@param[out] num_sensors    Number of elements in @a sensors.

@return Success if @a dd_handle_ptr was allocated and the driver was 
configured properly. Otherwise a specific error code is returned.

*/
/*=========================================================================*/                                             
sns_ddf_status_e sns_dd_dts201a_init
    (
    sns_ddf_handle_t*        dd_handle_ptr,
    sns_ddf_handle_t         smgr_handle,
    sns_ddf_nv_params_s*     nv_params,
    sns_ddf_device_access_s  device_info[],
    uint32_t                 num_devices,
    sns_ddf_memhandler_s*    memhandler,
    sns_ddf_sensor_e**        sensors,
    uint32_t*                num_sensors
    )
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	sns_dd_dts201a_state_t *state;

	static sns_ddf_sensor_e my_sensors[] =
	{
		SNS_DDF_SENSOR_OBJECT_TEMP /* SNS_DDF_SENSOR_OEM_SENSOR_01 */
#ifdef COMPILE_FOR_HD22
		, SNS_DDF_SENSOR_TEMP
#endif
	};

#ifdef UIMG_FUNCTIONS_AVAILABLE
	sns_ddf_smgr_set_uimg_refac(smgr_handle);
#endif

	/* Allocate a driver instance */
	status = sns_ddf_malloc_ex((void**)&state, sizeof(sns_dd_dts201a_state_t), smgr_handle);
	if(status != SNS_DDF_SUCCESS)
	{
		DD_DTS201A_MSG_0(ERROR, "init : malloc state_t error");
		return SNS_DDF_ENOMEM;
	}

	state->smgr_hndl = smgr_handle;

	/* Open communication port to the device */
	status = sns_ddf_open_port(&state->port_handle, &device_info->port_config);
	if(status != SNS_DDF_SUCCESS)
	{
		DD_DTS201A_MSG_0(ERROR, "init : open port error");
		sns_ddf_mfree_ex(state, smgr_handle);
		return status;
	}

	/* Init timers */
	status = sns_ddf_timer_init(&state->timer_obj, 
		(sns_ddf_handle_t)(uint32_t)state, 
		&DTS201A_DD_IF, 
		NULL, 
		FALSE);	/* one shot */
	if(status != SNS_DDF_SUCCESS)
	{
		DD_DTS201A_MSG_0(ERROR, "init : sns_ddf_timer_init error");
		sns_ddf_close_port(state->port_handle);
		sns_ddf_mfree_ex(state, smgr_handle);
		return status;
	}

	/* Fill out output parameters */
#ifdef COMPILE_FOR_HD22
	*num_sensors = 2;
#else
	*num_sensors = 1;
#endif
	*sensors = my_sensors;
	*dd_handle_ptr = (sns_ddf_handle_t)state;

	/* Initialize Registry Items */
	if (!nv_params || (nv_params->data_len != sizeof(sns_dd_dts201a_nv_t)))
	{
		/* Setting default values */
		state->nv_source.object_temp1_bias	= 0;
		state->nv_source.object_temp1_scale = 100;
		state->nv_source.object_temp2_bias	= 0;
		state->nv_source.object_temp2_scale = 100;
		state->nv_source.object_temp3_bias	= 0;
		state->nv_source.object_temp3_scale = 100;
	}
	else
	{
		sns_dd_dts201a_nv_t 	 *nv_source = (sns_dd_dts201a_nv_t *)nv_params->data;
		state->nv_source.object_temp1_bias	= nv_source->object_temp1_bias;
		state->nv_source.object_temp1_scale = nv_source->object_temp1_scale;
		state->nv_source.object_temp2_bias	= nv_source->object_temp2_bias;
		state->nv_source.object_temp2_scale = nv_source->object_temp2_scale;
		state->nv_source.object_temp3_bias	= nv_source->object_temp3_bias;
		state->nv_source.object_temp3_scale = nv_source->object_temp3_scale;
	}

	/* Resets the dts201a */
	status = sns_dd_dts201a_reset(state);
	if(status != SNS_DDF_SUCCESS)
	{
		DD_DTS201A_MSG_0(ERROR, "init : sns_dd_dts201a_reset error");
		sns_ddf_close_port(state->port_handle);
		sns_ddf_mfree_ex(state, smgr_handle);
		return status;
	}

	/* Get only first time */
#ifdef ONE_MODE
	status = dts201a_get_coefficient(state, coefficient_ary);
#else	/* TWO MODE */
	status = dts201a_get_coefficient(state, coefficient_n, coefficient_h);
#endif
	if(status != SNS_DDF_SUCCESS)
	{
		DD_DTS201A_MSG_0(ERROR, "init : get coefficient error");
		sns_ddf_close_port(state->port_handle);
		sns_ddf_mfree_ex(state, smgr_handle);
		return status;
	}

	return status;
}

/*===========================================================================

FUNCTION:   sns_dd_dts201a_set_attrib

===========================================================================*/
/*!
@brief Sets an attribute of the Thermopile sensor

@detail
Called by SMGR to set certain device attributes that are
programmable. Curently its the power mode, resolution and ODR.

@param[in] dd_handle   Handle to a driver instance.
@param[in] sensor Sensor for which this attribute is to be set.
@param[in] attrib      Attribute to be set.
@param[in] value      Value to set this attribute.

@return
The error code definition within the DDF 
SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
SNS_DDF_EINVALID_PARAM

*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_dts201a_set_attrib
    (
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    void*                value
    )
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	sns_dd_dts201a_state_t *state = (sns_dd_dts201a_state_t *)dd_handle;

	if (!state)
	{
		DD_DTS201A_MSG_0(ERROR, "sns_dd_dts201a_set_attrib : Invalid argument");
		return SNS_DDF_EINVALID_PARAM;
	}

	switch(attrib)
	{
		case SNS_DDF_ATTRIB_POWER_STATE:
		{
			DD_DTS201A_MSG_0(HIGH,"set_attrib : POWER_STATE");
			return SNS_DDF_SUCCESS;
		}
		case SNS_DDF_ATTRIB_RANGE:
		{
			if (sensor == SNS_DDF_SENSOR_OBJECT_TEMP) /* SNS_DDF_SENSOR_OEM_SENSOR_01 */
			{
				if(*((uint8_t *)value) > SNS_DD_DTS_MAX_RANGE)
				{
					DD_DTS201A_MSG_0(ERROR, "set_attrib : MAX RANGE error");
					return SNS_DDF_EINVALID_ATTR;
				}
				else
				{
					state->range = sns_dd_dts_range[*(uint8_t *)value];
				}

				status = dts201a_config_range(state);
				if(status != SNS_DDF_SUCCESS)
				{
					DD_DTS201A_MSG_0(ERROR, "set_attrib : config RANGE error");
					return status;
				}
			}
			break;
		}
		case SNS_DDF_ATTRIB_ODR:
		{
			sns_ddf_odr_t odr;
			odr = *(sns_ddf_odr_t *)value;
			
			if(sensor == SNS_DDF_SENSOR_OBJECT_TEMP) /* SNS_DDF_SENSOR_OEM_SENSOR_01 */
			{
				state->object_odr = odr;
				DD_DTS201A_MSG_2(HIGH,"set_attrib : ODR, o=%d_a=%d", state->object_odr, state->ambient_odr);
				return SNS_DDF_SUCCESS;
			}
#ifdef COMPILE_FOR_HD22
			else
			{
				if(state->object_odr != 0)
					state->ambient_odr = state->object_odr;
				else
					state->ambient_odr = odr;
				DD_DTS201A_MSG_2(HIGH,"set_attrib : ODR, o=%d_a=%d", state->object_odr, state->ambient_odr);
				return SNS_DDF_SUCCESS;
			}
#endif
			DD_DTS201A_MSG_1(HIGH,"set_attrib : Invalid sensor type %d", sensor);
			return SNS_DDF_EINVALID_PARAM;
		}
		default:
			status = SNS_DDF_EINVALID_PARAM;
			DD_DTS201A_MSG_0(HIGH,"set_attrib : default");
			break;
	}

	return status;
}

/*===========================================================================

FUNCTION:   sns_dd_dts201a_get_attrib

===========================================================================*/
/*!
@brief Called by the SMGR to retrieves the value of an attribute of
the sensor.

@detail
Returns the requested attribute

@param[in]  handle      Handle to a driver instance.
@param[in]  sensor      Sensor whose attribute is to be retrieved.
@param[in]  attrib      Attribute to be retrieved. 
@param[in]  memhandler  Memory handler used to dynamically allocate 
output parameters, if applicable.
@param[out] value       Pointer that this function will allocate or set 
to the attribute's value.
@param[out] num_elems   Number of elements in @a value.

@return
Success if the attribute was retrieved and the buffer was 
populated. Otherwise a specific error code is returned.
*/
/*=========================================================================*/

sns_ddf_status_e sns_dd_dts201a_get_attrib
    (
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    sns_ddf_memhandler_s* memhandler,
    void**               value,
    uint32_t*            num_elems
    )
{
	sns_dd_dts201a_state_t *state = (sns_dd_dts201a_state_t *)dd_handle;

	if (!state)
	{
		DD_DTS201A_MSG_0(ERROR, "sns_dd_dts201a_get_attrib : Invalid argument");
		return SNS_DDF_EINVALID_PARAM;
	}

	if ((sensor != SNS_DDF_SENSOR_OBJECT_TEMP)
#ifdef COMPILE_FOR_HD22
		&& (sensor != SNS_DDF_SENSOR_TEMP)
#endif
	)
	{
		DD_DTS201A_MSG_1(HIGH,"sns_dd_dts201a_get_attrib: unknown sensor type %d", sensor);
		return SNS_DDF_EINVALID_PARAM;
	}

	switch(attrib)
	{
		case SNS_DDF_ATTRIB_POWER_INFO:
		{
			sns_ddf_power_info_s* power_attrib;
			
			if( (*value = sns_ddf_memhandler_malloc_ex(memhandler,
				sizeof(sns_ddf_power_info_s),state->smgr_hndl)) == NULL)
			{
				DD_DTS201A_MSG_0(ERROR, "get_attrib : POWER_INFO error");
				return SNS_DDF_ENOMEM;
			}
			power_attrib = *value;
			power_attrib->active_current = 1100; /* 1.1 mA */
			power_attrib->lowpower_current = 0; /* 0.02 uA */
			*num_elems = 1;

			return SNS_DDF_SUCCESS;
		}
		case SNS_DDF_ATTRIB_DELAYS:
		{
			sns_ddf_delays_s *device_delay;
			
			if( (*value = sns_ddf_memhandler_malloc_ex(memhandler,
			              sizeof(sns_ddf_delays_s),state->smgr_hndl)) == NULL)
			{
				DD_DTS201A_MSG_0(ERROR, "get_attrib : DELAYS error");
				return SNS_DDF_ENOMEM;
			}
			device_delay = *value;
			device_delay->time_to_active = 1000; /* 1msec is the startup time */
			device_delay->time_to_data = 500; /* 0.5 msec */
			*num_elems = 1;

			return SNS_DDF_SUCCESS;
		}
		case SNS_DDF_ATTRIB_RANGE:
		{
			sns_ddf_range_s *device_ranges;

			if (sensor == SNS_DDF_SENSOR_OBJECT_TEMP) /* SNS_DDF_SENSOR_OEM_SENSOR_01 */
			{
				if( (*value = sns_ddf_memhandler_malloc_ex(memhandler,
					SNS_DD_DTS_MAX_RANGE*sizeof(sns_ddf_range_s),state->smgr_hndl)) == NULL)
				{
					DD_DTS201A_MSG_0(ERROR, "get_attrib : object temp RANGE error");
					return SNS_DDF_ENOMEM;
				}
				device_ranges = *value;
				device_ranges[0].min = DTS_OBJECT_TEMP_RANGE_N_MIN;
				device_ranges[0].max = DTS_OBJECT_TEMP_RANGE_N_MAX;
//				device_ranges[1].min = DTS_OBJECT_TEMP_RANGE_H_MIN;
//				device_ranges[1].max = DTS_OBJECT_TEMP_RANGE_H_MAX;
				*num_elems = 1; /* 2 */
			}
			else
			{
				if( (*value = sns_ddf_memhandler_malloc_ex(memhandler,
					sizeof(sns_ddf_range_s),state->smgr_hndl)) == NULL)
				{
					DD_DTS201A_MSG_0(ERROR, "get_attrib : sensor temp RANGE error");
					return SNS_DDF_ENOMEM;
				}
				device_ranges = *value;
				device_ranges->min = DTS_SENSOR_TEMP_RANGE_MIN;
				device_ranges->max = DTS_SENSOR_TEMP_RANGE_MAX;
				*num_elems = 1;
			}

			return SNS_DDF_SUCCESS;
		}
		case SNS_DDF_ATTRIB_RESOLUTION_ADC:
		{
			sns_ddf_resolution_adc_s *device_res;

			if( (*value = sns_ddf_memhandler_malloc_ex(memhandler,sizeof(sns_ddf_resolution_adc_s),state->smgr_hndl)) == NULL)
			{
				DD_DTS201A_MSG_0(ERROR, "get_attrib : RESOLUTION_ADC error");
				return SNS_DDF_ENOMEM;
			}

			device_res = *value;

			if (sensor == SNS_DDF_SENSOR_OBJECT_TEMP) /* SNS_DDF_SENSOR_OEM_SENSOR_01 */
			{
				device_res->bit_len = DTS_OBJECT_ADC_BIT_LEN;
				device_res->max_freq = 20;
			}
			else
			{
				device_res->bit_len = DTS_SENSOR_ADC_BIT_LEN;
				device_res->max_freq = 20;
			}
			*num_elems = 1;

			return SNS_DDF_SUCCESS;
		}
		case SNS_DDF_ATTRIB_RESOLUTION:
		{
			sns_ddf_resolution_t *device_res;

			if( (*value = sns_ddf_memhandler_malloc_ex(memhandler,sizeof(sns_ddf_resolution_t),state->smgr_hndl)) == NULL)
			{
				DD_DTS201A_MSG_0(ERROR, "get_attrib : RESOLUTION error");
				return SNS_DDF_ENOMEM;
			}

			device_res = *value;

			if (sensor == SNS_DDF_SENSOR_OBJECT_TEMP) /* SNS_DDF_SENSOR_OEM_SENSOR_01 */
			{
				*device_res = FX_FLTTOFIX_Q16(0.01);
			}
			else
			{
				*device_res = FX_FLTTOFIX_Q16(0.01);
			}
			*num_elems = 1;

			return SNS_DDF_SUCCESS;
		}
		case SNS_DDF_ATTRIB_DRIVER_INFO:
		{
			sns_ddf_driver_info_s *driver_info;
			
			if( (*value = sns_ddf_memhandler_malloc_ex(memhandler,sizeof(sns_ddf_driver_info_s),state->smgr_hndl)) == NULL)
			{
				DD_DTS201A_MSG_0(ERROR, "get_attrib : DRIVER_INFO error");
				return SNS_DDF_ENOMEM;
			}			
			driver_info = *value;
			driver_info->name = "sns_dd_dts201a driver";
			driver_info->version = 1;
			*num_elems = 1;

			return SNS_DDF_SUCCESS;
		}
		case SNS_DDF_ATTRIB_DEVICE_INFO:
		{
			sns_ddf_device_info_s *device_info;
			
			if( (*value = sns_ddf_memhandler_malloc_ex(memhandler,sizeof(sns_ddf_device_info_s),state->smgr_hndl)) == NULL)
			{
				DD_DTS201A_MSG_0(ERROR, "get_attrib : DEVICE_INFO error");
				return SNS_DDF_ENOMEM;
			}
			device_info = *value;
			device_info->model = "DTS201A";
			device_info->vendor = "PARTRON";
			device_info->name = "Non-contact Thermopile";
			device_info->version = 1;
			*num_elems = 1;

			return SNS_DDF_SUCCESS;
		}
		case SNS_DDF_ATTRIB_ACCURACY:
		{
			
			DD_DTS201A_MSG_0(HIGH,"get_attrib : ACCURACY");
			return SNS_DDF_EINVALID_ATTR;
//			return SNS_DDF_SUCCESS;
		}
		case SNS_DDF_ATTRIB_ODR:
		{
			uint32_t *odr_ptr;

			if((odr_ptr = sns_ddf_memhandler_malloc_ex(memhandler,sizeof(uint32_t),state->smgr_hndl)) == NULL)
			{
				DD_DTS201A_MSG_0(ERROR, "get_attrib : ODR error");
				return SNS_DDF_ENOMEM;
			}

			if(sensor == SNS_DDF_SENSOR_OBJECT_TEMP) /* SNS_DDF_SENSOR_OEM_SENSOR_01 */
			{
				*odr_ptr = state->object_odr;
			}
			else
			{
				*odr_ptr = state->ambient_odr;
			}
			*num_elems = 1;
			*(uint32_t **)value = odr_ptr;

			return SNS_DDF_SUCCESS;
		}
		case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
		{
			*value = &odr_list;
			*num_elems = odr_list_size;

			DD_DTS201A_MSG_0(HIGH,"get_attrib : SUPPORTED_ODR_LIST");
			return SNS_DDF_SUCCESS;
		}
		default:
			DD_DTS201A_MSG_0(HIGH,"get_attrib : default");
			return SNS_DDF_EINVALID_ATTR;
	}

//	return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION:   sns_dd_dts201a_run_test

===========================================================================*/
/*!
@ Called by the SMGR to run calibration test for Thermopile sensor.

@detail
Returns the cal_factor in error, to be stored in NV memory

@param[in]  handle      Handle to a driver instance.
@param[in]  sensor_type Sensor whose attribute is to be retrieved.
@param[in] test         Test ID used to decide what to test.
@param[out] err         prx_factor or als_factor.

@return
Success if no error. Otherwise a specific error code is returned.
*/
/*=========================================================================*/

sns_ddf_status_e sns_dd_dts201a_run_test
    (
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_test_e    test,
    uint32_t*         err
    )
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	sns_dd_dts201a_state_t *state = (sns_dd_dts201a_state_t *)dd_handle;
	
	uint8_t w_buffer[2] = {0x0, 0x0};
	uint8_t r_buffer[3] = {0x0, };
	uint16_t custom_id[2]={0x0, 0x0};

	if (!state)
	{
		DD_DTS201A_MSG_0(ERROR, "sns_dd_dts201a_run_test : Invalid argument");
		return SNS_DDF_EINVALID_PARAM;
	}

	if ((sensor != SNS_DDF_SENSOR_OBJECT_TEMP)
		&& (sensor != SNS_DDF_SENSOR__ALL)
#ifdef COMPILE_FOR_HD22
		&& (sensor != SNS_DDF_SENSOR_TEMP)
#endif
		)
	{
		DD_DTS201A_MSG_1(HIGH,"sns_dd_dts201a_run_test: unknown sensor type %d", sensor);
		return SNS_DDF_EINVALID_PARAM;
	}

	if(test == SNS_DDF_TEST_SELF)
	{
		/* Check the DTS_CUSTOM_ID */
		status = dts201a_write_byte_block(state->port_handle, DTS_CUST_ID0, w_buffer, 2);
		sns_ddf_delay(2500);	// 2.5msec
		status = dts201a_read_byte_block(state->port_handle, r_buffer, 3);
		custom_id[0] = ((r_buffer[1]<<8) | r_buffer[2]);

		status = dts201a_write_byte_block(state->port_handle, DTS_CUST_ID1, w_buffer, 2);
		sns_ddf_delay(2500);	// 2.5msec
		status = dts201a_read_byte_block(state->port_handle, r_buffer, 3);
		custom_id[1] = ((r_buffer[1]<<8) | r_buffer[2]);

		if ((custom_id[1] != DTS201A_ID1) && (custom_id[0] != DTS201A_ID0))
		{
			DD_DTS201A_MSG_0(ERROR, "run_test : CUSTOM_ID error");
			return SNS_DDF_EDEVICE;
		}
	}
	else
	{
		DD_DTS201A_MSG_0(ERROR, "run_test : TEST_ID error");	
		return SNS_DDF_EINVALID_TEST;
	}

	return status;
}

/*===========================================================================

FUNCTION:   sns_dd_dts201a_probe

===========================================================================*/
/**
* @brief Probes for the device with a given configuration.
*
* This commands the driver to look for the device with the specified
* configuration (ie, I2C address/bus defined in the sns_ddf_device_access_s
* struct.
*
* @param[in]  dev_info    Access info for physicol devices controlled by 
*                         this driver. Used to determine if the device is
*                         physically present.
* @param[in]  memhandler  Memory handler used to dynamically allocate 
*                         output parameters, if applicable.
* @param[out] num_sensors Number of sensors supported. 0 if none.
* @param[out] sensor_type Array of sensor types supported, with num_sensor
*                         elements. Allocated by this function.
*
* @return SNS_DDF_SUCCESS if the part was probed function completed, even
*         if no device was found (in which case num_sensors will be set to
*         0).
*/
sns_ddf_status_e sns_dd_dts201a_probe
    (
    sns_ddf_device_access_s* device_info,
    sns_ddf_memhandler_s*    memhandler,
    uint32_t*                num_sensors,
    sns_ddf_sensor_e**       sensors
    )
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	sns_ddf_handle_t port_handle;
	
	uint8_t w_buffer[2] = {0x0, 0x0};
	uint8_t r_buffer[3] = {0x0, };
	uint16_t custom_id[2]={0x0, 0x0};

	/* Basic sanity check */
	if (device_info == NULL || sensors == NULL || num_sensors == NULL)
	{
		return SNS_DDF_EINVALID_PARAM;
	}

	*num_sensors = 0;
	*sensors = NULL;

	/* Open communication port to the device */
	status = sns_ddf_open_port(&port_handle, &device_info->port_config);
	if(status != SNS_DDF_SUCCESS)
	{
		DD_DTS201A_MSG_0(ERROR, "probe : open port error");
		return status;
	}

	/* Check the DTS_CUSTOM_ID */
	status = dts201a_write_byte_block(port_handle, DTS_CUST_ID0, w_buffer, 2);
	sns_ddf_delay(2500);	// 2.5msec
	status = dts201a_read_byte_block(port_handle, r_buffer, 3);
	custom_id[0] = ((r_buffer[1]<<8) | r_buffer[2]);

	status = dts201a_write_byte_block(port_handle, DTS_CUST_ID1, w_buffer, 2);
	sns_ddf_delay(2500);	// 2.5msec
	status = dts201a_read_byte_block(port_handle, r_buffer, 3);
	custom_id[1] = ((r_buffer[1]<<8) | r_buffer[2]);

	if ((custom_id[1] != DTS201A_ID1) && (custom_id[0] != DTS201A_ID0))
	{
		DD_DTS201A_MSG_0(ERROR, "probe : CUSTOM_ID error");
		sns_ddf_close_port(port_handle);
		return SNS_DDF_EDEVICE;
	}
	
	/* Found device */
#ifdef COMPILE_FOR_HD22
	*num_sensors = 2;
#else
	*num_sensors = 1;
#endif
	*sensors = sns_ddf_memhandler_malloc( memhandler,
		sizeof(sns_ddf_sensor_e) * *num_sensors );
	if( *sensors != NULL )
	{
		(*sensors)[0] = SNS_DDF_SENSOR_OBJECT_TEMP; /* SNS_DDF_SENSOR_OEM_SENSOR_01 */
#ifdef COMPILE_FOR_HD22
		(*sensors)[1] = SNS_DDF_SENSOR_TEMP;
#endif
		status = SNS_DDF_SUCCESS;
	} 
	else 
	{
		DD_DTS201A_MSG_0(ERROR, "probe : malloc sensor_e error");
		status = SNS_DDF_ENOMEM;
	}

	sns_ddf_close_port(port_handle);

	return status;
}

