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
  Copyright (c) 2015 Qualcomm Technologies, Inc.   All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
*******************************************************************************/

/*==============================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

    when         who     what, where, why
----------   ---     -----------------------------------------------------------
10-Apr-2015 AH    Qualcomm specific changes
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

#include "sns_log_types_public.h"
#include "sns_log_api_public.h"

/*============================================================================
GLOBAL VARIABLE DEFINITIONS
============================================================================*/

#ifdef ONE_MODE
extern int32_t coefficient_ary[14];
#else	/* TWO MODE */
extern int32_t coefficient_n[14];
extern int32_t coefficient_h[14];
#endif

extern uint8_t o_avg_cnt;

/*============================================================================
FUNCTION PROTOTYPES
============================================================================*/

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
    );

static sns_ddf_status_e sns_dd_dts201a_get_data
    (
    sns_ddf_handle_t        dd_handle,
    sns_ddf_sensor_e        sensors[],
    uint32_t                num_sensors,
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_data_s**  data /* ignored by this async driver */
    );

sns_ddf_status_e sns_dd_dts201a_set_attrib
    (
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    void*                value
    );

sns_ddf_status_e sns_dd_dts201a_get_attrib
    (
    sns_ddf_handle_t     dd_handle,
    sns_ddf_sensor_e     sensor,
    sns_ddf_attribute_e  attrib,
    sns_ddf_memhandler_s* memhandler,
    void**               value,
    uint32_t*            num_elems
    );

static void sns_dd_dts201a_handle_timer 
    (
    sns_ddf_handle_t dd_handle, 
    void* arg
    );

static void sns_dd_dts201a_handle_irq
    (
    sns_ddf_handle_t dd_handle, 
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp
    );

sns_ddf_status_e sns_dd_dts201a_reset
    (
    sns_ddf_handle_t dd_handle
    );

static sns_ddf_status_e sns_dd_dts201a_enable_sched_data
    (
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    bool              enable
    );

sns_ddf_status_e sns_dd_dts201a_run_test
    (
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    sns_ddf_test_e    test,
    uint32_t*         err
    );

sns_ddf_status_e sns_dd_dts201a_probe
    (
    sns_ddf_device_access_s* device_info,
    sns_ddf_memhandler_s*    memhandler,
    uint32_t*                num_sensors,
    sns_ddf_sensor_e**       sensors
    );

sns_ddf_driver_if_s DTS201A_DD_IF =
{
    &sns_dd_dts201a_init,
    &sns_dd_dts201a_get_data,
    &sns_dd_dts201a_set_attrib, 
    &sns_dd_dts201a_get_attrib,
    &sns_dd_dts201a_handle_timer,
    &sns_dd_dts201a_handle_irq,
    &sns_dd_dts201a_reset,
    &sns_dd_dts201a_run_test,
    &sns_dd_dts201a_enable_sched_data,
    &sns_dd_dts201a_probe
};

/*===========================================================================

FUNCTION:   

===========================================================================*/
/*!

*/
/*=========================================================================*/

static float dts201a_FT_Algorithm
    (
    float targetdegree
    )
{
	targetdegree=0.0136363636f*(targetdegree*targetdegree*targetdegree)
					-1.3433982684f*(targetdegree*targetdegree)
					+44.5416666666f*(targetdegree)-461.0329004326f;

	return targetdegree;
}

static float dts201a_get_sTmeas
    (
    float vBridge
    )
{
	vBridge=vBridge+273.15f;
	float exp1=(14387.752f/((9.364f*vBridge)+178.4f));
	float exp2=(exp(exp1))-1.0f;
	float sT=1/exp2;

	return sT;
}

static float dts201a_get_Tw_for_Ambient
    (
    float ambienttemp
    )
{
	float finalambient=(1.0f)/(exp(14387.752f/(9.364f*(ambienttemp+273.15f)+178.4f))-1);

	return finalambient;
}

static float dts201a_get_sTs
    (
    float emsval,
    float sTmeas, 
    float ambient
    )
{
	float ambval=dts201a_get_Tw_for_Ambient(ambient);
	float sts=((1.0f/emsval)*sTmeas)+(((emsval-1.0f)/emsval)*ambval);

	return sts;
}

static float dts201a_get_finalTargetTemp
    (
    float sTs
    )
{
	float finalvalt=(14387.752f/(log((1/sTs)+1)))*(1/9.364f)-(178.4f/9.364f);
	float finalvaltemp=finalvalt-273.15f;

	return finalvaltemp;
}

static float dts201a_ApplyTPCoefficientsToRawData
    (
    int32_t *coefficients, 
    float rawTo, 
    float rawTa
    )
{
	int32_t	ORD_0_TP_To = 0;
	int32_t	ORD_1_TP_To = 1;
	int32_t	ORD_2_TP_To = 2;
	int32_t	ORD_3_TP_To = 3;
	int32_t	ORD_4_TP_To = 4;

	int32_t	ORD_0_TP_Ta = 5;
	int32_t	ORD_1_TP_Ta = 6;
	int32_t	ORD_2_TP_Ta = 7;
	int32_t	ORD_3_TP_Ta = 8;
	int32_t	ORD_4_TP_Ta = 9;

	int32_t BR_OFFSET = 10;

	float corrTa, corrTo;
	float To_raw_no_offset;
	float To_raw_Ta_comp;

	float norm = 8388608.0;

	rawTa -= norm;
	rawTo -= norm;

	rawTa /=norm;

	To_raw_no_offset =  rawTo - coefficients[BR_OFFSET];

	corrTa =   (coefficients[ORD_4_TP_Ta] * pow(rawTa,4.0) + coefficients[ORD_3_TP_Ta] * pow(rawTa,3.0) + 
				coefficients[ORD_2_TP_Ta] * pow(rawTa,2.0) + coefficients[ORD_1_TP_Ta] * rawTa +			
				coefficients[ORD_0_TP_Ta]);

	To_raw_Ta_comp = To_raw_no_offset + corrTa;

	To_raw_Ta_comp /= norm;

	corrTo =	(coefficients[ORD_4_TP_To] * pow(To_raw_Ta_comp,4.0) + coefficients[ORD_3_TP_To] * pow(To_raw_Ta_comp,3.0)	+ 
				coefficients[ORD_2_TP_To] * pow(To_raw_Ta_comp,2.0) + coefficients[ORD_1_TP_To] * To_raw_Ta_comp			+ 
				coefficients[ORD_0_TP_To]);

	corrTo  /= (16.0*64.0); 

	return corrTo;
}

static float dts201a_GetCorrectedTemp_TP
    (
    int32_t *coefficients, 
    float tempRaw
    )
{
	int32_t	TP_OFFSET_T = 11;
	int32_t	TP_GAIN_T = 12;
	int32_t	TP_SOT_T = 13;

	float unity = 8388608.0;
	float zero_point = 8388608.0;
	float N_max = 16777215;

	float maxTemp = 85;
	float minTemp = -20;
	float Trange = maxTemp-minTemp; 

	float tempDeg=0;

	float calibratedVal = 0;
	float N_ZT = 0;

	tempRaw -= 8388608.0;

	N_ZT = ((4*coefficients[TP_GAIN_T] * (tempRaw + 4*coefficients[TP_OFFSET_T])) / unity ) + zero_point ;
	calibratedVal = ( (N_ZT * (unity + ((4*coefficients[TP_SOT_T] * N_ZT) / unity))) / unity );

	tempDeg = calibratedVal/N_max*Trange + minTemp;

	return tempDeg;
}

static uint32_t dts201a_object_temp_moving_avg
(
    sns_dd_dts201a_state_t *state,
    uint32_t o_adc
)
{
	uint8_t i;
	uint32_t ret;
	uint32_t object_tot = 0;

	if( (o_avg_cnt&0x7f) >= DTS_MOVING_AVG ) { 
		o_avg_cnt = 0;
		o_avg_cnt |= 0x80;
	}
	
	if( o_avg_cnt&0x80 ) {
		state->out.object_ary[(o_avg_cnt&0x7f)] = o_adc;
		o_avg_cnt = (o_avg_cnt&0x7f)+1;
		o_avg_cnt |= 0x80;

		for(i=0; i<DTS_MOVING_AVG; i++)
			object_tot += state->out.object_ary[i];

		ret = (object_tot/DTS_MOVING_AVG);
	} else {
		state->out.object_ary[(o_avg_cnt&0x7f)] = o_adc;
		o_avg_cnt++;

		for(i=0; i<o_avg_cnt; i++)
			object_tot += state->out.object_ary[i];

		ret = (object_tot/o_avg_cnt);
	}

	return ret;
}

/*===========================================================================
FUNCTION      sns_dd_dts201a_log_data

DESCRIPTION   Log the latest sensor data

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None
===========================================================================*/
void sns_dd_dts201a_log_data(
    sns_dd_dts201a_state_t*	      dd_ptr,
    sns_ddf_sensor_e       sensor_type,
    sns_ddf_time_t               timestamp )
{
	sns_err_code_e				err_code;
	sns_log_sensor_data_pkt_s*	log_struct_ptr;

	/* Allocate log packet */
	err_code = sns_logpkt_malloc(DTS201A_LOG_PACKET,
		sizeof(sns_log_sensor_data_pkt_s),
		(void**)&log_struct_ptr);

	if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
	{
		log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
		log_struct_ptr->sensor_id = sensor_type;
		log_struct_ptr->vendor_id = SNS_DDF_VENDOR_PARTRON;

		/* Timestamp the log with sample time */
		log_struct_ptr->timestamp = timestamp;

		/* Log sensor data */
		/* Two data items for both Non-contact Thermopile and Sensor temperature sensor types */
		if(sensor_type == SNS_DDF_SENSOR_OBJECT_TEMP)
		{
		        log_struct_ptr->num_data_types = SDD_OBJECT_NUM;
		        log_struct_ptr->data[0] = dd_ptr->data_cache[SDD_OBJECT_TEMP_N];
		        log_struct_ptr->data[1] = dd_ptr->data_cache[SDD_OBJECT_TEMP_H];
#ifdef COMPILE_FOR_HD22
		        log_struct_ptr->data[2] = dd_ptr->data_cache[SDD_OBJECT_TEMP_A];
#else
			 	log_struct_ptr->data[2] = dd_ptr->data_cache[SDD_SENSOR_TEMP_N];
#endif
		}
#ifdef COMPILE_FOR_HD22
		else
		{
		        log_struct_ptr->num_data_types = SDD_SENSOR_NUM;
		 	 log_struct_ptr->data[0] = dd_ptr->data_cache[SDD_SENSOR_TEMP_N];
			 log_struct_ptr->data[1] = dd_ptr->data_cache[SDD_SENSOR_TEMP_A];
		}
#endif
		/* Commit log (also frees up the log packet memory) */
		err_code = sns_logpkt_commit(DTS201A_LOG_PACKET, log_struct_ptr);
		if(err_code != SNS_SUCCESS)
		{
			DD_DTS201A_MSG_0(ERROR,"sns_logpkt_commit - error");
		}
	}
	else
	{
		//DD_DTS201A_MSG_0(ERROR,"sns_dd_dts201a_log_data - NO MEMORY");
	}
}

/*===========================================================================

FUNCTION:   sns_dd_dts201a_handle_irq

===========================================================================*/
/*!
@brief Interrupt handler

@detail
Called in response to an interrupt for this driver.

@see sns_ddf_driver_if_s.handle_irq()

@param[in] dd_handle  Handle to a driver instance. 
@param[in] irq        The IRQ representing the interrupt that occured. 

@return None 
*/
/*=========================================================================*/
static void sns_dd_dts201a_handle_irq
    (
    sns_ddf_handle_t dd_handle, 
    uint32_t          gpio_num,
    sns_ddf_time_t    timestamp
    )
{

}

/*===========================================================================

FUNCTION:   sns_dd_dts201a_handle_timer

===========================================================================*/
/*!
@brief Called when the timer set by this driver has expired. This is  
the callback function submitted when initializing a timer.

@detail
This will be called within the context of the Sensors Manager task.
It indicates that sensor data is ready

@param[in] dd_handle  Handle to a driver instance. 
@param[in] arg        The argument submitted when the timer was set. 

@return None 
*/
/*=========================================================================*/
static void sns_dd_dts201a_handle_timer 
    (
    sns_ddf_handle_t dd_handle, 
    void* arg
    )
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	sns_dd_dts201a_state_t *state = (sns_dd_dts201a_state_t *)dd_handle;
	sns_ddf_sensor_data_s *report = NULL;
	
	uint8_t i;
	uint8_t r_buffer[7] = {0x0, };
       uint32_t num_sensors = 0;
	sns_ddf_sensor_e sensors[2];

	float object_temp = 0;
	float ambient_temp = 0;
	float sTmeas_output = 0;
	float sTs_output = 0;
	float object_normal = 0;
	float object_human = 0;

	if (!state)
	{
		DD_DTS201A_MSG_0(ERROR, "handle_timer : Invalid argument");
		return;
	}

	status = dts201a_read_byte_block(state->port_handle, r_buffer, 7);
	state->out.sstat = r_buffer[0];
//	state->out.object = ((r_buffer[1]<<16) | (r_buffer[2]<<8) | r_buffer[3]);
	state->out.object = dts201a_object_temp_moving_avg(state, ((r_buffer[1]<<16) | (r_buffer[2]<<8) | r_buffer[3]));
	state->out.ambient = ((r_buffer[4]<<16) | (r_buffer[5]<<8) | r_buffer[6]);

//	DD_MSG_3(HIGH,"[T]satus=0x%02x, object=0x%06x, ambient=0x%06x ", state->out.sstat, state->out.object, state->out.ambient);

#ifdef ONE_MODE
	object_temp = dts201a_ApplyTPCoefficientsToRawData(coefficient_ary, (float)state->out.object, (float)state->out.ambient);
	ambient_temp = dts201a_GetCorrectedTemp_TP(coefficient_ary, (float)state->out.ambient);

	sTmeas_output = dts201a_get_sTmeas(object_temp);

	sTs_output = dts201a_get_sTs(DTS_H_EMISSIVITY, sTmeas_output, ambient_temp);
	object_normal = dts201a_get_finalTargetTemp(sTs_output);
	object_human = dts201a_FT_Algorithm(object_normal);

	sTs_output = dts201a_get_sTs(DTS_N_EMISSIVITY, sTmeas_output, ambient_temp);
	object_normal = dts201a_get_finalTargetTemp(sTs_output);
#else	/* TWO MODE */
	object_temp = dts201a_ApplyTPCoefficientsToRawData(coefficient_h, (float)state->out.object, (float)state->out.ambient);
	ambient_temp = dts201a_GetCorrectedTemp_TP(coefficient_h, (float)state->out.ambient);

	sTmeas_output = dts201a_get_sTmeas(object_temp);

	sTs_output = dts201a_get_sTs(DTS_H_EMISSIVITY, sTmeas_output, ambient_temp);
	object_normal = dts201a_get_finalTargetTemp(sTs_output);
	object_human = dts201a_FT_Algorithm(object_normal);

	object_temp = dts201a_ApplyTPCoefficientsToRawData(coefficient_n, (float)state->out.object, (float)state->out.ambient);
	ambient_temp = dts201a_GetCorrectedTemp_TP(coefficient_n, (float)state->out.ambient);

	sTmeas_output = dts201a_get_sTmeas(object_temp);
	
	sTs_output = dts201a_get_sTs(DTS_N_EMISSIVITY, sTmeas_output, ambient_temp);
	object_normal = dts201a_get_finalTargetTemp(sTs_output);
#endif

//	DD_DTS201A_MSG_3(HIGH,"[T]ambient_temp=%d object_human=%d object_normal=%d ", ambient_temp*100, object_human*100, object_normal*100);

	if(state->object_temp_enabled)
	{
		sensors[num_sensors] = SNS_DDF_SENSOR_OBJECT_TEMP;
		num_sensors++;
	}

	if(state->ambient_temp_enabled)
	{
		sensors[num_sensors] = SNS_DDF_SENSOR_TEMP;
		num_sensors++;
	}

	DD_DTS201A_MSG_3(MED, "handle_timer : object_normal %d, object_temp1_scale %d, object_temp1_bias %d",
		             object_normal, state->nv_source.object_temp1_scale, state->nv_source.object_temp1_bias);
	DD_DTS201A_MSG_3(MED, "handle_timer : object_human  %d, object_temp2_scale %d, object_temp2_bias %d",
		             object_human, state->nv_source.object_temp2_scale, state->nv_source.object_temp2_bias);
	DD_DTS201A_MSG_3(MED, "handle_timer : ambient_temp  %d, object_temp3_scale %d, object_temp3_bias %d",
		             ambient_temp, state->nv_source.object_temp3_scale, state->nv_source.object_temp3_bias);

	DD_DTS201A_MSG_2(MED, "handle_timer : out.object  %d, out.ambient %d",
		             state->out.object, state->out.ambient);
	
    object_normal = (object_normal * state->nv_source.object_temp1_scale / 100) - state->nv_source.object_temp1_bias;
	object_human  = (object_human  * state->nv_source.object_temp2_scale / 100) - state->nv_source.object_temp2_bias;
	ambient_temp  = (ambient_temp  * state->nv_source.object_temp3_scale / 100) - state->nv_source.object_temp3_bias;

	state->data_cache[SDD_OBJECT_TEMP_N] = FX_FLTTOFIX_Q16(object_normal);
	state->data_cache[SDD_OBJECT_TEMP_H] = FX_FLTTOFIX_Q16(object_human);
	state->data_cache[SDD_OBJECT_TEMP_A] = state->out.object;

	state->data_cache[SDD_SENSOR_TEMP_N] = FX_FLTTOFIX_Q16(ambient_temp);
	state->data_cache[SDD_SENSOR_TEMP_A] = state->out.ambient;

	status = sns_ddf_malloc_ex((void **) &report, num_sensors*sizeof(sns_ddf_sensor_data_s), state->smgr_hndl);
	if(status != SNS_DDF_SUCCESS)
	{
		DD_DTS201A_MSG_0(ERROR, "handle_timer : malloc num_sensor error");
		return;
	}

	for(i = 0; i < num_sensors; i++)
	{
//		DD_DTS201A__MSG_2(HIGH, "sensor=%d_%d\n",i, sensors[i]);
		report[i].sensor = sensors[i];
		report[i].status = SNS_DDF_SUCCESS;
		report[i].timestamp = sns_ddf_get_timestamp();

		if(sensors[i] == SNS_DDF_SENSOR_OBJECT_TEMP) /* SNS_DDF_SENSOR_OEM_SENSOR_01 */
		{
			status = sns_ddf_malloc_ex((void **) &report[i].samples, SDD_OBJECT_NUM*sizeof(sns_ddf_sensor_sample_s), state->smgr_hndl);
			if(status != SNS_DDF_SUCCESS)
			{
				DD_DTS201A_MSG_0(ERROR, "handle_timer : malloc SDD_OBJECT_NUM error");
				return;
			}

			report[i].samples[0].sample = state->data_cache[SDD_OBJECT_TEMP_N];
			report[i].samples[1].sample = state->data_cache[SDD_OBJECT_TEMP_H];
#ifdef COMPILE_FOR_HD22
			report[i].samples[2].sample = state->data_cache[SDD_OBJECT_TEMP_A];
#else
			report[i].samples[2].sample = state->data_cache[SDD_SENSOR_TEMP_N];
#endif
			report[i].samples[0].status = SNS_DDF_SUCCESS;
			report[i].samples[1].status = SNS_DDF_SUCCESS;
			report[i].samples[2].status = SNS_DDF_SUCCESS;

			report[i].num_samples = SDD_OBJECT_NUM;
			state->object_temp_enabled = false;
			sns_dd_dts201a_log_data(state, sensors[i], report[i].timestamp);
		}
#ifdef COMPILE_FOR_HD22
		else
		{
			status = sns_ddf_malloc_ex((void **) &report[i].samples, SDD_SENSOR_NUM*sizeof(sns_ddf_sensor_sample_s), state->smgr_hndl);
			if(status != SNS_DDF_SUCCESS)
			{
				DD_DTS201A_MSG_0(ERROR, "handle_timer : malloc SDD_SENSOR_NUM error");
				return;
			}

			report[i].samples[0].sample = state->data_cache[SDD_SENSOR_TEMP_N];
			report[i].samples[1].sample = state->data_cache[SDD_SENSOR_TEMP_A];

			report[i].samples[0].status = SNS_DDF_SUCCESS;
			report[i].samples[1].status = SNS_DDF_SUCCESS;

			report[i].num_samples = SDD_SENSOR_NUM;
			state->ambient_temp_enabled = false;
			sns_dd_dts201a_log_data(state, sensors[i], report[i].timestamp);
		}
#endif
	}


	if (state->smgr_hndl != NULL)
	{
		sns_ddf_smgr_notify_data(state->smgr_hndl, report, num_sensors);
	}
#if 0
	if (report->samples != NULL)
	{
		sns_ddf_mfree_ex(report->samples, state->smgr_hndl);
	}
#else
	for(i=0; i<num_sensors; i++)
	{
		if (report[i].samples != NULL)
			sns_ddf_mfree_ex(report[i].samples, state->smgr_hndl);		
	}
#endif
	if (report != NULL)
	{
		sns_ddf_mfree_ex(report, state->smgr_hndl);
	}
}

/*===========================================================================

FUNCTION:   sns_dd_dts201a_get_data

===========================================================================*/
/*!
@brief Called by the SMGR to get data

@detail
Requests a single sample of sensor data from each of the specified
sensors. Data is returned immediately after being read from the
sensor, in which case data[] is populated in the same order it was
requested

This driver is a pure asynchronous one, so no data will be written to buffer.
As a result, the return value will be always PENDING if the process does
not fail.  This driver will notify the Sensors Manager via asynchronous
notification when data is available.

@param[in]  dd_handle    Handle to a driver instance.
@param[in]  sensors      List of sensors for which data is requested.
@param[in]  num_sensors  Length of @a sensors.
@param[in]  memhandler   Memory handler used to dynamically allocate 
output parameters, if applicable.
@param[out] data         Sampled sensor data. The number of elements must 
match @a num_sensors.

@return SNS_DDF_SUCCESS if data was populated successfully. If any of the 
sensors queried are to be read asynchronously SNS_DDF_PENDING is
returned and data is via @a sns_ddf_smgr_data_notify() when
available. Otherwise a specific error code is returned.

*/
/*=========================================================================*/
static sns_ddf_status_e sns_dd_dts201a_get_data
    (
    sns_ddf_handle_t        dd_handle,
    sns_ddf_sensor_e        sensors[],
    uint32_t                num_sensors,
    sns_ddf_memhandler_s*   memhandler,
    sns_ddf_sensor_data_s**  data /* ignored by this async driver */
    )
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	sns_dd_dts201a_state_t *state = (sns_dd_dts201a_state_t *)dd_handle;
	
	uint8_t i;
	uint8_t w_buffer[2] = {0x0, 0x0};

	if (!state)
	{
		DD_DTS201A_MSG_0(ERROR, "sns_dd_dts201a_get_data : Invalid argument");
		return SNS_DDF_EINVALID_PARAM;
	}

	for (i = 0; i < num_sensors; i++)
	{
		if ((sensors[i] != SNS_DDF_SENSOR_OBJECT_TEMP) /* SNS_DDF_SENSOR_OEM_SENSOR_01 */
#ifdef COMPILE_FOR_HD22
			&& (sensors[i] != SNS_DDF_SENSOR_TEMP)
#endif
			)
		{
			DD_DTS201A_MSG_0(ERROR, "handle_timer : malloc num_sensors error");
			return SNS_DDF_EINVALID_PARAM;
		}
	}

	for (i = 0; i < num_sensors; i++)
	{
		if (sensors[i] == SNS_DDF_SENSOR_OBJECT_TEMP)
			state->object_temp_enabled = true;
#ifdef COMPILE_FOR_HD22
		else if(sensors[i] == SNS_DDF_SENSOR_TEMP)
			state->ambient_temp_enabled = true;
#endif
		else
			return SNS_DDF_EINVALID_PARAM;
	}

	status = dts201a_write_byte_block(state->port_handle, DTS_MEASURE, w_buffer, 2);
	status = sns_ddf_timer_start(state->timer_obj, DTS_CONVERSION_TIME);

	return SNS_DDF_PENDING;
}

static sns_ddf_status_e sns_dd_dts201a_enable_sched_data
    (
    sns_ddf_handle_t  dd_handle,
    sns_ddf_sensor_e  sensor,
    bool              enable
    )
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	
	return status;
}
