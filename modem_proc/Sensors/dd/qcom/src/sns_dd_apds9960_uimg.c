/*==============================================================================

    SENSORS RGB, CCT, AMBIENT LIGHT AND PROXIMITY  COMBO DRIVER

DESCRIPTION

   Implements the uImage driver for Avago RGB, CCT, ALS and Proximity Sensor APDS-9960

********************************************************************************
* Copyright (c) 2014 Avago Technologies.
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*        notice, this list of conditions and the following disclaimer in the
*        documentation and/or other materials provided with the distribution.
*     3. Neither the name of Avago nor the
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
*******************************************************************************

==============================================================================*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2012 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
-----------------------------------------------------------------------------*/
/**************************************************************************
*
* REVISON HISTORY
*
* VERSION	DATE        WHO  DESCRIPTION
*
* 1.0     10/10/2014  KK   Initial version 
                           Derived from sns_dd_apds9960 combo driver of 
                           ALS/PRX/RGB/CCT
  1.1     16Mar2015   KK   (i) Enable flag was not disabled at the end of 
                           handle_device_indication which should be disabled.
                           This caused the sensor not being enabled again in
                           next polling cycle.
                           (ii) Revert back IRQ trigger from SNS_DDF_SIGNAL_IRQ_LOW
                           to SNS_DDF_SIGNAL_IRQ_FALLING.
                           (iii) Set dri_enabled in enable_sched_data() instead of 
                           init()

  1.2     31Mar2015   HW   Clean up the pointers if dd initialization fails due
                           to error
  1.2     19May2015   KK   Revise force interrupt during enable_interrupt for prox

  1.3     11Jun2015   KK   Reset enable flag in set_powerstate()
  1.4     15Jun2015   AH   Refactored for uImage
  1.4.1   22Jun2015   KK   Revise RGB/CCT change gain mechanism in apds9960_dd_als_raw_data_ready()
  1.5     30Jun2015   AH   for each sample use same timestamp in 14c1 logging and SMGR notification
  1.6     17Jul2015   KK   Fix for OEM test and correct threshold update
*
**************************************************************************/

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
#include "sns_dd_apds9960_priv.h"

#if USE_DRAGONBOARD_BUILD
#include "sns_log_api_public.h"
#include "sns_log_types_public.h"
#else
#include "sns_debug_str.h"
#include "sns_debug_api.h"
#include "sns_memmgr.h"
#include "log_codes.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#endif	// USE_DRAGONBOARD_BUILD

/*============================================================================
STATIC VARIABLE DEFINITIONS
============================================================================*/

extern volatile boolean EnableI2C;

uint8_t sns_dd_als_atime_tb[3] = { 0xF6, 0xEE, 0xDC }; /* 27.8ms,  50.04ms , 100.08ms */

uint32_t sns_dd_als_atime_us_tb[3] = { 27800, 50040, 100080 }; /* 27.8ms,  50.04ms , 100.08ms */
uint16_t sns_dd_als_res_tb[3] = { 0x2801, 0x4801, 0x9001 }; /* 10240, 21504, 43008 */
uint8_t sns_dd_als_adc_bit_tb[3] = { 14, 15, 16 };

uint8_t sns_dd_als_again_tb[4] = { 1, 4, 16, 64 };
uint8_t sns_dd_als_again_bit_tb[4] = { 0x00, 0x01, 0x02, 0x03 };

sns_ddf_odr_t sns_dd_apds9960_odr_tb[3] = { 5, 10, 20 }; // Hz

double RGB_COE_X[3] = {-0.63337, 7.3903, -5.521};
double RGB_COE_Y[3] = {-0.89673, 7.3564, -5.1258};
double RGB_COE_Z[3] = {-1.45125, 4.6529, -1.9685};

double RGB_CIE_N1 = 0.332;
double RGB_CIE_N2 = 0.1858;

double RGB_CIE_CCT1 = 449.0;
double RGB_CIE_CCT2 = 3525.0;
double RGB_CIE_CCT3 = 6823.3;
double RGB_CIE_CCT4 = 5520.33;

sns_ddf_sensor_e prx_timer = SNS_DDF_SENSOR_PROXIMITY;
sns_ddf_sensor_e als_timer = SNS_DDF_SENSOR_AMBIENT;
sns_ddf_sensor_e rgb_timer = SNS_DDF_SENSOR_RGB;
sns_ddf_sensor_e cct_timer = SNS_DDF_SENSOR_CT_C;

uint32_t first_init=0;
bool first_probe=0;
int32_t als_prx_idx=-1;
int32_t rgb_cct_idx=-1;

apds9960_dd_head_s apds9960_dd_head = {NULL, {"APDS-9960", 1}};

/*============================================================================
STATIC FUNCTION PROTOTYPES
============================================================================*/

sns_ddf_status_e sns_dd_apds9960_get_data
(
sns_ddf_handle_t        dd_handle,
sns_ddf_sensor_e        sensors[],
uint32_t                num_sensors,
sns_ddf_memhandler_s*   memhandler,
sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
);

void sns_dd_apds9960_handle_timer
(
sns_ddf_handle_t dd_handle,
void* arg
);

void sns_dd_apds9960_handle_irq
(
sns_ddf_handle_t dd_handle,
uint32_t          gpio_num,
sns_ddf_time_t    timestamp
);

sns_ddf_status_e apds9960_dd_set_powerstate
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
sns_ddf_sensor_e sensor,
sns_ddf_powerstate_e op_mode,
bool init_data
);

/*===========================================================================
FUNCTION      sns_dd_apds9960_log

DESCRIPTION   Log the latest sensor data

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None
===========================================================================*/
void sns_dd_apds9960_log(
   apds9960_dd_dev_s* dd_ptr,
   sns_ddf_sensor_e       sensor,
   uint32_t               data1, 
   uint32_t               data2,
   uint32_t               data3,
   uint32_t               data4,
   uint32_t               data5,
   sns_ddf_time_t         timestamp)
{
   sns_err_code_e err_code;
   sns_log_sensor_data_pkt_s* log_struct_ptr;
   log_pkt_t log_pkt_type = APDS9960_LOG_PACKET;

   /* Allocate log packet */
   err_code = sns_logpkt_malloc(log_pkt_type,
                                sizeof(sns_log_sensor_data_pkt_s),
                                (void**)&log_struct_ptr);

   if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
   {
     log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
     log_struct_ptr->sensor_id = sensor;
     log_struct_ptr->vendor_id = SNS_DDF_VENDOR_AVAGO;

     /* Timestamp the log with sample time */
     log_struct_ptr->timestamp = timestamp;

     /* Log the sensor data */
     if (sensor == SNS_DDF_SENSOR_AMBIENT)
     {
       log_struct_ptr->num_data_types = 3;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data2;
       log_struct_ptr->data[2] = data3;
     }
     else if(sensor == SNS_DDF_SENSOR_PROXIMITY)
     {
       log_struct_ptr->num_data_types = 5;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data2;
       log_struct_ptr->data[2] = data3;
       log_struct_ptr->data[3] = data4;
       log_struct_ptr->data[4] = data5;
     }
		 else if(sensor == SNS_DDF_SENSOR_RGB)
     {
       log_struct_ptr->num_data_types = 3;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data2;
       log_struct_ptr->data[2] = data3;
     }
		 else if(sensor == SNS_DDF_SENSOR_CT_C)
     {
       log_struct_ptr->num_data_types = 2;
       log_struct_ptr->data[0] = data1;
       log_struct_ptr->data[1] = data2;
     }

     /* Commit log (also frees up the log packet memory) */
     err_code = sns_logpkt_commit(log_pkt_type,
                                  log_struct_ptr);
   }
   else
   {
     dd_ptr->dropped_logs++;
     ALSPRX_MSG_3(ERROR,"log - %d dropped=%d err=%d", 
                  11, dd_ptr->dropped_logs, err_code);
   }
}

sns_ddf_status_e apds9960_dd_set_enable
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  enable
)
{
  uint8_t bytes_w;

  return sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9960_DD_ENABLE_ADDR,
                              &enable, 1, &bytes_w);
}

sns_ddf_status_e apds9960_dd_set_atime
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  atime
)
{
	uint8_t bytes_w;
	
	return sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9960_DD_ATIME_ADDR,
                              &atime, 1, &bytes_w);
}

sns_ddf_status_e apds9960_dd_set_wtime
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  wtime
)
{
	uint8_t bytes_w;
	
	return sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9960_DD_WTIME_ADDR,
                              &wtime, 1, &bytes_w);
}

sns_ddf_status_e apds9960_dd_set_ailt
(
apds9960_dd_dev_s* dd_ptr,
uint16_t  threshold
)
{
	uint8_t bytes_w;
	
	return sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9960_DD_AILTL_ADDR,
                              (uint8_t*)&threshold, 2, &bytes_w);
}

sns_ddf_status_e apds9960_dd_set_aiht
(
apds9960_dd_dev_s* dd_ptr,
uint16_t  threshold
)
{
	uint8_t bytes_w;
	
	return sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9960_DD_AIHTL_ADDR,
                              (uint8_t*)&threshold, 2, &bytes_w);
}

sns_ddf_status_e apds9960_dd_set_pilt
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  threshold
)
{
	uint8_t bytes_w;
	
	return sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9960_DD_PITLO_ADDR,
                              &threshold, 1, &bytes_w);
}

sns_ddf_status_e apds9960_dd_set_piht
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  threshold
)
{
	uint8_t bytes_w;
	
	return sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9960_DD_PITHI_ADDR,
                              &threshold, 1, &bytes_w);
}

sns_ddf_status_e apds9960_dd_set_pers
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  pers
)
{
	uint8_t bytes_w;
	
	return sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9960_DD_PERS_ADDR,
                              &pers, 1, &bytes_w);
}

sns_ddf_status_e apds9960_dd_set_config
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  config
)
{
	uint8_t bytes_w;
	
	return sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9960_DD_CONFIG_ADDR,
                              &config, 1, &bytes_w);
}

sns_ddf_status_e apds9960_dd_set_ppulse
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  ppulse
)
{
	uint8_t bytes_w;
	
	return sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9960_DD_PPULSE_ADDR,
                              &ppulse, 1, &bytes_w);
}

sns_ddf_status_e apds9960_dd_set_control
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  control
)
{
	uint8_t bytes_w;
	
	return sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9960_DD_CONTROL_ADDR,
                              &control, 1, &bytes_w);
}

sns_ddf_status_e apds9960_dd_set_poffset_ur
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  poffset_ur
)
{
	uint8_t bytes_w;
	
	return sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9960_DD_POFFSET_UR_ADDR,
                              &poffset_ur, 1, &bytes_w);
}

sns_ddf_status_e apds9960_dd_set_poffset_dl
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  poffset_dl
)
{
	uint8_t bytes_w;
	
	return sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9960_DD_POFFSET_DL_ADDR,
                              &poffset_dl, 1, &bytes_w);
}

sns_ddf_status_e apds9960_dd_set_config2
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  config2
)
{
	uint8_t bytes_w;
	
	return sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9960_DD_CONFIG2_ADDR,
                              &config2, 1, &bytes_w);
}

/*===========================================================================

FUNCTION      apds9960_dd_set_prx_thresh

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e apds9960_dd_set_prx_thresh
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  pilt,
uint8_t  piht
)
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	
	status = apds9960_dd_set_pilt(dd_ptr, pilt);
	CHECK_STATUS(status);
	
	status = apds9960_dd_set_piht(dd_ptr, piht);
	CHECK_STATUS(status);
	
	return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      apds9960_dd_set_als_thresh

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e apds9960_dd_set_als_thresh
(
apds9960_dd_dev_s* dd_ptr,
uint16_t  ailt,
uint16_t  aiht
)
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	uint8_t bytes_w;
	uint8_t thresh[4];
	
	thresh[0] = (ailt & 0xFF); /* AILTL */
	thresh[1] = (ailt >> 8);   /* AILTH */
	thresh[2] = (aiht & 0xFF); /* AIHTL */
	thresh[3] = (aiht >> 8);   /* AIHTH */
	
	
	status = sns_ddf_write_port(dd_ptr->port_handle,
								APDS9960_DD_AILTL_ADDR,
								(uint8_t*)&thresh[0], 4, &bytes_w);
	CHECK_STATUS(status);
	
	return SNS_DDF_SUCCESS;
}


/*===========================================================================

FUNCTION      apds9960_dd_update_intg_time

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e apds9960_dd_update_intg_time
(
apds9960_dd_dev_s* dd_ptr,
bool prx_enabled,
bool als_enabled,
bool static_setting
)
{
	uint8_t atime=0;
	uint8_t wtime=0;
	uint32_t cycle_time_us;
	uint32_t atime_us=0;
	uint32_t ptime_us=0;
	int32_t wtime_us=0;
	int32_t wtime_32=0;
	int8_t res_idx;
	sns_ddf_status_e status;
	
	if (static_setting)
	{
		/* ATIME */
		atime  = sns_dd_als_atime_tb[dd_ptr->als.als_res_index];
		status = apds9960_dd_set_atime(dd_ptr, atime);
		CHECK_STATUS(status);
		
		wtime = APDS9960_DD_2_78MS_ADC_TIME;
		status = apds9960_dd_set_wtime(dd_ptr, wtime);
		CHECK_STATUS(status);
	}
	else
	{
		/* Set ATIME, PTIME, WTIME */
		cycle_time_us = dd_ptr->common.cycle_time;
		
		/* PTIME */
		if (prx_enabled)
		{
			ptime_us = 2780;
		}
		
		if (als_enabled)
		{
			/* ATIME */
			for (res_idx=2; res_idx >=0; res_idx--)
			{
				atime_us = sns_dd_als_atime_us_tb[res_idx];
				if (cycle_time_us > atime_us)
				{
					ALSPRX_MSG_3(MED, "update_intg_time - idx=%d cycle_time=%d atime=%d", res_idx, cycle_time_us, atime_us);
					
					break; /* current atime works */
				}
			} /* for */
			ALSPRX_MSG_1(MED, "update_intg_time - idx=%d", res_idx);
			
			if (res_idx >=0)
			{
				if (res_idx != dd_ptr->als.als_res_index)
				{
					/* reducing resolution and atime to get the right ODR */
					dd_ptr->als.als_res_index = res_idx;
				}
			}
			else
			{
				dd_ptr->als.als_res_index = 0; /* pick smallest atime */
				ALSPRX_MSG_0(ERROR, "update_intg_time - idx=0");
			}
			atime = sns_dd_als_atime_tb[dd_ptr->als.als_res_index];
			atime_us = sns_dd_als_atime_us_tb[dd_ptr->als.als_res_index];
		}
		else
		{
			atime_us = 0;
		}
		
		/* WTIME */
		wtime_us = (cycle_time_us - (atime_us + ptime_us));
		if (wtime_us <= 2780)
		{
			wtime = 0xFF; /* minumum one cycle */
		}
		else
		{
			wtime_32 = (int32_t)(256 - (wtime_us / 2780)); /* 32 bit math */
			wtime = (uint8_t)wtime_32; /* convert to 8 bits */
		}
		
		ALSPRX_MSG_2(MED, "update_intg_time - atime=%d wtime=%d",
					 atime, wtime);
		ALSPRX_MSG_2(MED, "update_intg_time - atime_us=%d wtime_us=%d",
					 atime_us, wtime_us);
		
		/* Program */
		status = apds9960_dd_set_atime(dd_ptr, atime);
		CHECK_STATUS(status);
		
		status = apds9960_dd_set_wtime(dd_ptr, wtime);
		CHECK_STATUS(status);
	}
	
	return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      apds9960_dd_clear_int_reg

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  sns_ddf_status_e

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e apds9960_dd_clear_int_reg
(
apds9960_dd_dev_s* dd_ptr,
uint8_t reg_addr
)
{
	uint8_t written = 0;
	sns_ddf_status_e status;
#ifndef QDSP6
	status = sns_ddf_write_port(dd_ptr->ZBW_port_handle, 0, &reg_addr, 1, &written);
#else
	status = sns_ddf_write_port(dd_ptr->port_handle, reg_addr, NULL, 0, &written);
#endif
	ALSPRX_MSG_1(MED, "clear_int_reg - reg_addr=0x%x", reg_addr);
	return status;
}

/*===========================================================================

FUNCTION      apds9960_dd_start_general_timer

DESCRIPTION   Starts a timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void apds9960_dd_start_general_timer
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
uint32_t sensor,
uint32_t duration
)
{
	sns_ddf_status_e status;
	
	ALSPRX_MSG_2(MED, "start_general_timer - sensor=%d, duration=%d ", sensor, duration);
	
	if (sensor == SNS_DDF_SENSOR_AMBIENT)
	{
		if ( dd_ptr->als.timer_active )
		{
			ALSPRX_MSG_0(MED, "AMBIENT timer already active");
			return;
		}
		
		status = sns_ddf_timer_start(dd_ptr->als.timer, duration);
		
		if ( status != SNS_DDF_SUCCESS )
		{
			ALSPRX_MSG_0(ERROR, "apds9960_dd_start_general_timer - timer start failed ");
			return;
		}
		
		dd_ptr->als.timer_active = TRUE;
	}
	else if (sensor == SNS_DDF_SENSOR_RGB)
	{
		if ( dd_ptr->rgb.timer_active )
		{
			ALSPRX_MSG_0(MED, "RGB timer already active");
			return;
		}
		
		status = sns_ddf_timer_start(dd_ptr->rgb.timer, duration);
		
		if ( status != SNS_DDF_SUCCESS )
		{
			ALSPRX_MSG_0(ERROR, "apds9960_dd_start_general_timer - timer start failed ");
			return;
		}
		
		dd_ptr->rgb.timer_active = TRUE;
	}
	else if (sensor == SNS_DDF_SENSOR_CT_C)
	{
		if ( dd_ptr->cct.timer_active )
		{
			ALSPRX_MSG_0(MED, "CT_C timer already active");
			return;
		}
		
		status = sns_ddf_timer_start(dd_ptr->cct.timer, duration);
		
		if ( status != SNS_DDF_SUCCESS )
		{
			ALSPRX_MSG_0(ERROR, "apds9960_dd_start_general_timer - timer start failed ");
			return;
		}
		
		dd_ptr->cct.timer_active = TRUE;
	}
	else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
	{
		if ( dd_ptr->prx.timer_active )
		{
			ALSPRX_MSG_0(MED, "PROXIMITY timer already active");
			return;
		}
		
		status = sns_ddf_timer_start(dd_ptr->prx.timer, duration);
		if ( status != SNS_DDF_SUCCESS )
		{
			return;
		}
		
		dd_ptr->prx.timer_active = TRUE;
	}
}

/*===========================================================================

FUNCTION      apds9960_dd_stop_general_timer

DESCRIPTION   Stops a timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void apds9960_dd_stop_general_timer
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
uint32_t sensor
)
{
	ALSPRX_MSG_2(MED, "stop_general_timer - sensor=%d, sub_dev_idx=%d ", sensor, sub_dev_idx);

	if (sensor == SNS_DDF_SENSOR_AMBIENT)
	{
		//ALSPRX_MSG_1(MED, "als_timer=%d", dd_ptr->als.timer_active);

		if (TRUE == dd_ptr->als.timer_active)
		{
			sns_ddf_timer_cancel(dd_ptr->als.timer);
			dd_ptr->als.timer_active = FALSE;
		}
	}
	else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
	{
		//ALSPRX_MSG_1(MED, "prx_timer=%d", dd_ptr->prx.timer_active);

		if (TRUE == dd_ptr->prx.timer_active)
		{
			sns_ddf_timer_cancel(dd_ptr->prx.timer);
			dd_ptr->prx.timer_active = FALSE;
		}
	}
	else if (sensor == SNS_DDF_SENSOR_RGB)
	{
		//ALSPRX_MSG_1(MED, "rgb_timer=%d", dd_ptr->rgb.timer_active);

		if (TRUE == dd_ptr->rgb.timer_active)
		{
			sns_ddf_timer_cancel(dd_ptr->rgb.timer);
			dd_ptr->rgb.timer_active = FALSE;
		}
	}
	else if (sensor == SNS_DDF_SENSOR_CT_C)
	{
		//ALSPRX_MSG_1(MED, "cct_timer=%d", dd_ptr->cct.timer_active);
		if (TRUE == dd_ptr->cct.timer_active)
		{
			sns_ddf_timer_cancel(dd_ptr->cct.timer);
			dd_ptr->cct.timer_active = FALSE;
		}
	}
}

/*===========================================================================

FUNCTION      apds9960_dd_req_data

DESCRIPTION   A function that handles data request from sub-drivers

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e apds9960_dd_req_data
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
uint32_t sensor
)
{
	sns_ddf_status_e status=SNS_DDF_SUCCESS;
	ALSPRX_MSG_2(MED, "req_data - sub device=%d, sensor=%d", sub_dev_idx, sensor);
	
	if (sensor == SNS_DDF_SENSOR_AMBIENT)
	{
		if (dd_ptr->common.als_odr == 5) {
			dd_ptr->als.timer_value = 200000;
		}
		else if (dd_ptr->common.als_odr == 10) {
			dd_ptr->als.timer_value = 100000;
		}
		else{
			dd_ptr->als.timer_value = 50000;
		}

		apds9960_dd_start_general_timer (dd_ptr, sub_dev_idx, sensor,
										 dd_ptr->als.timer_value);
	}
	else if (sensor == SNS_DDF_SENSOR_RGB)
	{
		if (dd_ptr->common.rgb_odr == 5) {
			dd_ptr->rgb.timer_value = 200000;
		}
		else if (dd_ptr->common.rgb_odr == 10) {
			dd_ptr->rgb.timer_value = 100000;
		}
		else{
			dd_ptr->rgb.timer_value = 50000;
		}

		apds9960_dd_start_general_timer (dd_ptr, sub_dev_idx, sensor,
										 dd_ptr->rgb.timer_value);
	}
	else if (sensor == SNS_DDF_SENSOR_CT_C)
	{
		if (dd_ptr->common.cct_odr == 5) {
			dd_ptr->cct.timer_value = 200000;
		}
		else if (dd_ptr->common.cct_odr == 10) {
			dd_ptr->cct.timer_value = 100000;
		}
		else{
			dd_ptr->cct.timer_value = 50000;
		}

		apds9960_dd_start_general_timer (dd_ptr, sub_dev_idx, sensor,
										 dd_ptr->cct.timer_value);
	}
	else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
	{
		if (dd_ptr->common.prx_odr == 5) {
			dd_ptr->prx.timer_value = 200000;
		}
		else if (dd_ptr->common.prx_odr == 10) {
			dd_ptr->prx.timer_value = 100000;
		}
		else{
			dd_ptr->prx.timer_value = 50000;
		}

		apds9960_dd_start_general_timer (dd_ptr, sub_dev_idx, sensor,
										 dd_ptr->prx.timer_value);
	}
	else
	{
		status = SNS_DDF_EFAIL;
	}
	
	return status;
}

/*===========================================================================

FUNCTION      apds9960_dd_convert_to_irradiance
  
DESCRIPTION   Convert adc raw data to irradiance value

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None 

===========================================================================*/
void apds9960_dd_convert_to_irradiance 
(
apds9960_dd_dev_s* dd_ptr,
sns_ddf_sensor_e sensor_type,
uint16_t cdata,
uint16_t rdata,
uint16_t gdata,
uint16_t bdata,
bool calibration
)
{
	double irradiance_c=0.0;
	double irradiance_r=0.0;
	double irradiance_g=0.0;
	double irradiance_b=0.0;
	double normalized_factor;
	uint32_t prev_r_c_ratio;
	uint32_t r_c_ratio=0;

	normalized_factor = ((double)sns_dd_als_atime_us_tb[dd_ptr->als.als_res_index]*(double)sns_dd_als_again_tb[dd_ptr->als.als_gain_index])/1000.0;

	ALSPRX_MSG_3(MED, "convert_to_irradiance n_factor=%d, als_res_idx=%d, als_gain_idx=%d",
				 (int32_t)normalized_factor,
				 dd_ptr->als.als_res_index,
				 dd_ptr->als.als_gain_index);

	if (cdata > 0)
	{
		if (cdata >= sns_dd_als_res_tb[dd_ptr->als.als_res_index])
		{
			if (sensor_type == SNS_DDF_SENSOR_AMBIENT)
			{
				prev_r_c_ratio = dd_ptr->als.r_c_ratio;
			}
			else if (sensor_type == SNS_DDF_SENSOR_RGB)
			{
				prev_r_c_ratio = dd_ptr->rgb.r_c_ratio;
			}
			else
			{
				prev_r_c_ratio = dd_ptr->cct.r_c_ratio;
			}

			if (prev_r_c_ratio == 0)
			{
				r_c_ratio = 0;
			}
			else
			{
				r_c_ratio = prev_r_c_ratio;
			}
		}
		else
		{
			r_c_ratio = (rdata*100)/cdata;
		}

		if( r_c_ratio > 60 )
		{  // Incandescent/Halogen 2600K
			irradiance_c=((double)cdata*APDS9960_DD_IRRADIANCE_C_GA2)/(normalized_factor);
			irradiance_r=((double)rdata*APDS9960_DD_IRRADIANCE_R_GA2)/(normalized_factor);
			irradiance_g=((double)gdata*APDS9960_DD_IRRADIANCE_G_GA2)/(normalized_factor);
			irradiance_b=((double)bdata*APDS9960_DD_IRRADIANCE_B_GA2)/(normalized_factor);
		}
		else if( r_c_ratio > 40 )
		{  // WW light source
			irradiance_c=((double)cdata*APDS9960_DD_IRRADIANCE_C_GA3)/(normalized_factor);
			irradiance_r=((double)rdata*APDS9960_DD_IRRADIANCE_R_GA3)/(normalized_factor);
			irradiance_g=((double)gdata*APDS9960_DD_IRRADIANCE_G_GA3)/(normalized_factor);
			irradiance_b=((double)bdata*APDS9960_DD_IRRADIANCE_B_GA3)/(normalized_factor);
		}
		else
		{  // Fluorescent Daylight 6500K
			irradiance_c=((double)cdata*APDS9960_DD_IRRADIANCE_C_GA1)/(normalized_factor);
			irradiance_r=((double)rdata*APDS9960_DD_IRRADIANCE_R_GA1)/(normalized_factor);
			irradiance_g=((double)gdata*APDS9960_DD_IRRADIANCE_G_GA1)/(normalized_factor);
			irradiance_b=((double)bdata*APDS9960_DD_IRRADIANCE_B_GA1)/(normalized_factor);
		}
	}	

	if (calibration)
	{
		dd_ptr->als.cdata_irradiance = (uint32_t)irradiance_c;
		dd_ptr->cct.cdata_irradiance = (uint32_t)irradiance_c;
		dd_ptr->rgb.rdata_irradiance = (uint32_t)irradiance_r;
		dd_ptr->rgb.gdata_irradiance = (uint32_t)irradiance_g;
		dd_ptr->rgb.bdata_irradiance = (uint32_t)irradiance_b;
	
		return;
	}

	if (sensor_type == SNS_DDF_SENSOR_AMBIENT)
	{
		dd_ptr->als.cdata_irradiance = (uint32_t)((irradiance_c*(double)dd_ptr->common.nv_db.rgb_c_factor)/100.0);
	}
	else if (sensor_type == SNS_DDF_SENSOR_RGB)
	{
		dd_ptr->rgb.cdata_irradiance = (uint32_t)((irradiance_c*(double)dd_ptr->common.nv_db.rgb_c_factor)/100.0);
		dd_ptr->rgb.rdata_irradiance = (uint32_t)((irradiance_r*(double)dd_ptr->common.nv_db.rgb_r_factor)/100.0);
		dd_ptr->rgb.gdata_irradiance = (uint32_t)((irradiance_g*(double)dd_ptr->common.nv_db.rgb_g_factor)/100.0);
		dd_ptr->rgb.bdata_irradiance = (uint32_t)((irradiance_b*(double)dd_ptr->common.nv_db.rgb_b_factor)/100.0);
	}
	else if (sensor_type == SNS_DDF_SENSOR_CT_C)
	{
		dd_ptr->cct.cdata_irradiance = (uint32_t)((irradiance_c*(double)dd_ptr->common.nv_db.rgb_c_factor)/100.0);
		dd_ptr->cct.rdata_irradiance = (uint32_t)((irradiance_r*(double)dd_ptr->common.nv_db.rgb_r_factor)/100.0);
		dd_ptr->cct.gdata_irradiance = (uint32_t)((irradiance_g*(double)dd_ptr->common.nv_db.rgb_g_factor)/100.0);
		dd_ptr->cct.bdata_irradiance = (uint32_t)((irradiance_b*(double)dd_ptr->common.nv_db.rgb_b_factor)/100.0);
	}
}

/*===========================================================================

FUNCTION      apds9960_dd_rgb_convert_to_mlux

DESCRIPTION   Convert a raw data to a real milli lux

DEPENDENCIES  None

RETURN VALUE  milli lux value or 0 if there was a error

SIDE EFFECT   None

===========================================================================*/
uint32_t apds9960_dd_rgb_convert_to_mlux
(
apds9960_dd_dev_s* dd_ptr,
uint16_t cdata,
uint16_t rdata,
uint16_t gdata,
uint16_t bdata,
bool calibration
)
{
	uint32_t lux32;
	double luxf;
	uint32_t r_c_ratio=0;
	double atime_factor;
	
	ALSPRX_MSG_3(MED, "cdata %d atime %d again %d",
				 cdata,
				 sns_dd_als_atime_us_tb[dd_ptr->als.als_res_index],
				 sns_dd_als_again_tb[dd_ptr->als.als_gain_index]);
	

	atime_factor = (100.08*10.0)/((double)sns_dd_als_atime_us_tb[dd_ptr->als.als_res_index]/1000.0);	// magnify 10 times

	// magnify 100 times = total 1000 times --> mlux
	luxf = (((double)cdata*100.0*atime_factor)/(double)sns_dd_als_again_tb[dd_ptr->als.als_gain_index]);

	if (cdata > 0)
	{
		if (cdata >= sns_dd_als_res_tb[dd_ptr->als.als_res_index])
		{
			if (dd_ptr->als.r_c_ratio == 0)
			{
				r_c_ratio = 0;
			}
			else
			{
				r_c_ratio = dd_ptr->als.r_c_ratio;
			}
		}
		else
		{
			r_c_ratio = (rdata*100)/cdata;
		}

		//ALSPRX_MSG_1(MED, "r_c_ratio=%d", r_c_ratio);

		if (r_c_ratio > 60)
		{
			// Type A (incand/halogen) light source
			luxf=(luxf*APDS9960_DD_LUX_GA2);
		}
		else if ( r_c_ratio > 40 )
		{  // WW light source
			luxf=(luxf*APDS9960_DD_LUX_GA3);			
		}
		else if (r_c_ratio > 20 )
		{
			// Production white LED light source
			luxf=(luxf*APDS9960_DD_LUX_GA1);			
		}

		dd_ptr->als.r_c_ratio = r_c_ratio;
	}
	
	// for ALS calibration
	if (calibration)
	{
		lux32 = (uint32_t)(luxf);
		ALSPRX_MSG_2(MED, "Lux=%d b4 calibration, r_c_ratio=%d", (uint32_t)lux32, r_c_ratio);
		return ((uint32_t)lux32);
	}
	
	luxf = (luxf * (double)dd_ptr->common.nv_db.als_factor)/100.0;
	
	lux32 = (uint32_t)(luxf);
	
	ALSPRX_MSG_3(MED, "Lux (after correction of %d/100)=%d, r_c_ratio=%d", dd_ptr->common.nv_db.als_factor, (uint32_t)lux32, r_c_ratio);
	
	return ((uint32_t)lux32);
}

/*===========================================================================

FUNCTION      apds9960_dd_rgb_convert_to_cct

DESCRIPTION   Convert a raw data to a real cct (correlated color temperature)

DEPENDENCIES  None

RETURN VALUE  milli lux value or 0 if there was a error

SIDE EFFECT   None

===========================================================================*/
uint32_t apds9960_dd_rgb_convert_to_cct
(
apds9960_dd_dev_s* dd_ptr,
uint16_t cdata,
uint16_t rdata,
uint16_t gdata,
uint16_t bdata,
bool calibration
)
{
	double X1, Y1, Z1;
	double x1, y1;
	double n;
	double cct;
	double sum_XYZ=0;
	uint32_t r_c_ratio=0;
	
	if (cdata > 0)
	{
		bdata = (gdata * 97)/100;
	}

	X1 = (RGB_COE_X[0]*(double)rdata) + (RGB_COE_X[1]*(double)gdata) + (RGB_COE_X[2]*(double)bdata);
	Y1 = (RGB_COE_Y[0]*(double)rdata) + (RGB_COE_Y[1]*(double)gdata) + (RGB_COE_Y[2]*(double)bdata);
	Z1 = (RGB_COE_Z[0]*(double)rdata) + (RGB_COE_Z[1]*(double)gdata) + (RGB_COE_Z[2]*(double)bdata);
	
	if ( (X1==0) && (Y1==0) && (Z1==0) )
	{
		x1 = y1 = 0;
	}
	else
	{
		sum_XYZ = (X1 + Y1 + Z1);
		if (sum_XYZ == 0)
		{
			x1 = y1 =0;
		}
		else
		{
			x1 = X1/sum_XYZ;
			y1 = Y1/sum_XYZ;
		}
	}
	
	if (cdata > 10)
	{
		n = (x1 - RGB_CIE_N1)/(RGB_CIE_N2 - y1);
		cct = (RGB_CIE_CCT1*(n*n*n))+ (RGB_CIE_CCT2*(n*n)) + ((RGB_CIE_CCT3*n) + RGB_CIE_CCT4);
	}
	else
	{
		n = 0;
		cct = 0;
	}

	ALSPRX_MSG_2(MED, "cdata %d rdata %d",
				 cdata,
				 rdata);

	ALSPRX_MSG_2(MED, "gdata %d bdata %d",
				 gdata,
				 bdata);
	
	ALSPRX_MSG_2(MED, "again %d atime %d",
				 sns_dd_als_again_tb[dd_ptr->als.als_gain_index],
				 sns_dd_als_atime_us_tb[dd_ptr->als.als_res_index]);

	ALSPRX_MSG_1(MED, "CCT=%d", (uint32_t)cct);
	
	if (cdata > 0)
	{
		if (cdata >= sns_dd_als_res_tb[dd_ptr->als.als_res_index])
		{
			if (dd_ptr->cct.r_c_ratio == 0)
			{
				r_c_ratio = 0;
			}
			else
			{
				r_c_ratio = dd_ptr->cct.r_c_ratio;
			}
		}
		else
		{
			r_c_ratio = (rdata*100)/cdata;
		}

		ALSPRX_MSG_1(MED, "r_c_ratio=%d", r_c_ratio);

		if (r_c_ratio > 60)
		{
			// Type A (incand/halogen) light source
			cct=(cct*APDS9960_DD_CCT_GA2);
		}
		else if ( r_c_ratio > 40 )
		{  // WW light source
			cct=(cct*APDS9960_DD_CCT_GA3);
		}
		else if ( r_c_ratio > 20 )
		{  // Production white LED/CW light source
			cct=(cct*APDS9960_DD_CCT_GA1);
		}

		dd_ptr->cct.r_c_ratio = r_c_ratio;
	}
	
	if (calibration)
	{
		ALSPRX_MSG_1(MED, "CCT (b4 calibration)=%d", (uint32_t)cct);
		return ((uint32_t)cct);	
	}
	
	cct = (cct * (double)dd_ptr->common.nv_db.cct_factor)/100.0;
	
	ALSPRX_MSG_2(MED, "CCT (after correction of %d/100)=%d", dd_ptr->common.nv_db.cct_factor, (uint32_t)cct);

	return ((uint32_t)cct);
}

/*===========================================================================

FUNCTION      apds9960_dd_async_notify

DESCRIPTION   Notify to SNSM with a sensor data.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void apds9960_dd_async_notify
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
sns_ddf_sensor_e sensor
)
{
	uint8_t num_sensors = 1, num_samples=2;
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	uint32_t data_lux;
	uint32_t data_cct;

	//ALSPRX_MSG_2(MED, "async_notify - sensor=%d idx=%d", sensor, sub_dev_idx);
	
	if (sensor == SNS_DDF_SENSOR_AMBIENT)
	{
		/* convert to lux */
		data_lux = (uint32_t) (dd_ptr->als.data_mlux / 1000);
		
		/* saturate */
		data_lux = (data_lux > APDS9960_DD_ALS_MAX_LUX) ? APDS9960_DD_ALS_MAX_LUX : data_lux;
		
		/* related to floating point library removal; change to FX_CONV_Q16(val,0) */
		dd_ptr->als.sensor_sample[0].sample = FX_CONV_Q16(data_lux, 0);  /* Lux Output */
		dd_ptr->als.sensor_sample[0].status = SNS_DDF_SUCCESS;
		
		dd_ptr->als.sensor_sample[1].sample = dd_ptr->als.cdata;  /* ADC output - cdata */
		dd_ptr->als.sensor_sample[1].status = SNS_DDF_SUCCESS;

		ALSPRX_MSG_2(MED, "als data Lux=%d cdata=%d", data_lux, dd_ptr->als.cdata);

		dd_ptr->common.sensor_data[0].status = status;
		dd_ptr->common.sensor_data[0].sensor = sensor;
		dd_ptr->common.sensor_data[0].num_samples = num_samples;

		if (dd_ptr->sub_dev[sub_dev_idx].dri_enabled)
		{
			dd_ptr->common.sensor_data[0].timestamp = dd_ptr->int_timestamp;
		}
		else
		{
			dd_ptr->common.sensor_data[0].timestamp = sns_ddf_get_timestamp();
		}

		dd_ptr->common.sensor_data[0].samples = dd_ptr->als.sensor_sample;

		sns_dd_apds9960_log(dd_ptr, sensor, 1000 * data_lux /*mLux in uint*/, dd_ptr->als.sensor_sample[0].sample /*Lux in Q16*/, dd_ptr->als.sensor_sample[1].sample /*ADC in uint*/, 0, 0, dd_ptr->common.sensor_data[0].timestamp);
		sns_ddf_smgr_notify_data(dd_ptr->sub_dev[sub_dev_idx].smgr_handle, dd_ptr->common.sensor_data, num_sensors);
	}
	else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
	{
		dd_ptr->prx.sensor_sample[0].sample = FX_CONV_Q16(dd_ptr->prx.detection_state, 0);  /* proximity binary */
		dd_ptr->prx.sensor_sample[0].status = SNS_DDF_SUCCESS;

		dd_ptr->prx.sensor_sample[1].sample = dd_ptr->prx.pdata;  /* ADC output - pdata */
		dd_ptr->prx.sensor_sample[1].status = SNS_DDF_SUCCESS;

		ALSPRX_MSG_2(MED, "prx data %d %d", dd_ptr->prx.detection_state, dd_ptr->prx.pdata);

		dd_ptr->common.sensor_data[0].status = status;
		dd_ptr->common.sensor_data[0].sensor = sensor;
		dd_ptr->common.sensor_data[0].num_samples = num_samples;
		ALSPRX_MSG_2(MED, "dri_enabled = %d, sub_dev_idx = %d", dd_ptr->sub_dev[sub_dev_idx].dri_enabled, sub_dev_idx);
		if (dd_ptr->sub_dev[sub_dev_idx].dri_enabled)
		{
			dd_ptr->common.sensor_data[0].timestamp = dd_ptr->int_timestamp;
		}
		else
		{
			dd_ptr->common.sensor_data[0].timestamp = sns_ddf_get_timestamp();
		}
		ALSPRX_MSG_1(MED, "timestamp = %d", dd_ptr->common.sensor_data[0].timestamp);
		dd_ptr->common.sensor_data[0].samples = dd_ptr->prx.sensor_sample;

		sns_dd_apds9960_log(dd_ptr, sensor, dd_ptr->prx.detection_state, dd_ptr->prx.sensor_sample[0].sample, 0, 0, dd_ptr->prx.sensor_sample[1].sample, dd_ptr->common.sensor_data[0].timestamp);
		sns_ddf_smgr_notify_data(dd_ptr->sub_dev[sub_dev_idx].smgr_handle, dd_ptr->common.sensor_data, num_sensors);
	}
	else if (sensor == SNS_DDF_SENSOR_RGB)
	{
		/* convert adc raw data to irradiance value */
		apds9960_dd_convert_to_irradiance(dd_ptr, sensor, dd_ptr->rgb.cdata, dd_ptr->rgb.rdata, dd_ptr->rgb.gdata, dd_ptr->rgb.bdata, FALSE);
		
		ALSPRX_MSG_3(MED, "R_raw=%d G_raw=%d B_raw=%d", dd_ptr->rgb.rdata, dd_ptr->rgb.gdata, dd_ptr->rgb.bdata);
		ALSPRX_MSG_3(MED, "R_rad=%d G_rad=%d B_rad=%d", dd_ptr->rgb.rdata_irradiance, dd_ptr->rgb.gdata_irradiance, dd_ptr->rgb.bdata_irradiance);

		// RData
		if (dd_ptr->rgb.rdata_irradiance > 65535) {
			dd_ptr->rgb.rdata_irradiance = 65535;
		}

		dd_ptr->rgb.sensor_sample[0].sample =  FX_CONV_Q16(dd_ptr->rgb.rdata_irradiance, 0); //dd_ptr->rgb.rdata_irradiance;
		dd_ptr->rgb.sensor_sample[0].status = SNS_DDF_SUCCESS;

		// GData
		if (dd_ptr->rgb.gdata_irradiance > 65535) {
			dd_ptr->rgb.gdata_irradiance = 65535;
		}

		dd_ptr->rgb.sensor_sample[1].sample = FX_CONV_Q16(dd_ptr->rgb.gdata_irradiance, 0); //dd_ptr->rgb.gdata_irradiance;
		dd_ptr->rgb.sensor_sample[1].status = SNS_DDF_SUCCESS;

		// BData
		if (dd_ptr->rgb.bdata_irradiance > 65535) {
			dd_ptr->rgb.bdata_irradiance = 65535;
		}

		dd_ptr->rgb.sensor_sample[2].sample = FX_CONV_Q16(dd_ptr->rgb.bdata_irradiance, 0); //dd_ptr->rgb.bdata_irradiance;
		dd_ptr->rgb.sensor_sample[2].status = SNS_DDF_SUCCESS;

		ALSPRX_MSG_3(MED, "sample[0]=%d sample[1]=%d sample[2]=%d", dd_ptr->rgb.sensor_sample[0].sample, dd_ptr->rgb.sensor_sample[1].sample, dd_ptr->rgb.sensor_sample[2].sample);
		
		num_samples = 3;  /* special case */

		dd_ptr->common.sensor_data[0].status = status;
		dd_ptr->common.sensor_data[0].sensor = sensor;
		dd_ptr->common.sensor_data[0].num_samples = num_samples;
		
		dd_ptr->common.sensor_data[0].timestamp = sns_ddf_get_timestamp();
		
		dd_ptr->common.sensor_data[0].samples = dd_ptr->rgb.sensor_sample;
		
		ALSPRX_MSG_3(MED, "smgr_handle=%x, num_sensors=%d, timestamp=%d", dd_ptr->sub_dev[sub_dev_idx].smgr_handle, num_sensors, dd_ptr->common.sensor_data[0].timestamp);

		sns_dd_apds9960_log(dd_ptr, sensor, dd_ptr->rgb.sensor_sample[0].sample, dd_ptr->rgb.sensor_sample[1].sample, dd_ptr->rgb.sensor_sample[2].sample, 0, 0, dd_ptr->common.sensor_data[0].timestamp);
		sns_ddf_smgr_notify_data(dd_ptr->sub_dev[sub_dev_idx].smgr_handle, dd_ptr->common.sensor_data, num_sensors);
	}
	else if (sensor == SNS_DDF_SENSOR_CT_C)
	{
		data_cct = (dd_ptr->cct.data_cct > 10000) ? 10000 : dd_ptr->cct.data_cct;
		
		dd_ptr->cct.sensor_sample[0].sample = FX_CONV_Q16(data_cct, 0);  /* CCT Output */
		dd_ptr->cct.sensor_sample[0].status = SNS_DDF_SUCCESS;

		/* convert adc raw data to irradiance value */
		apds9960_dd_convert_to_irradiance(dd_ptr, sensor, dd_ptr->cct.cdata, dd_ptr->cct.rdata, dd_ptr->cct.gdata, dd_ptr->cct.bdata, FALSE);  
		
		if (dd_ptr->cct.cdata_irradiance > 65535) {
			dd_ptr->cct.cdata_irradiance = 65535;
		}

		dd_ptr->cct.sensor_sample[1].sample = FX_CONV_Q16(dd_ptr->cct.cdata_irradiance, 0); //dd_ptr->cct.cdata_irradiance;
		dd_ptr->cct.sensor_sample[1].status = SNS_DDF_SUCCESS;

		ALSPRX_MSG_2(MED, "CCT=%d C_rad=%d", data_cct, dd_ptr->cct.cdata_irradiance);   
		ALSPRX_MSG_2(MED, "sample[0]=%x sample[1]=%d", dd_ptr->cct.sensor_sample[0].sample, dd_ptr->cct.sensor_sample[1].sample);
		
		dd_ptr->common.sensor_data[0].status = status;
		dd_ptr->common.sensor_data[0].sensor = sensor;
		dd_ptr->common.sensor_data[0].num_samples = num_samples;
		
		dd_ptr->common.sensor_data[0].timestamp = sns_ddf_get_timestamp();
		
		dd_ptr->common.sensor_data[0].samples = dd_ptr->cct.sensor_sample;
	
		ALSPRX_MSG_3(MED, "smgr_handle=%x, num_sensors=%d, timestamp=%d", dd_ptr->sub_dev[sub_dev_idx].smgr_handle, num_sensors, dd_ptr->common.sensor_data[0].timestamp);

		sns_dd_apds9960_log(dd_ptr, sensor, dd_ptr->cct.sensor_sample[0].sample, dd_ptr->cct.sensor_sample[1].sample, 0, 0, 0, dd_ptr->common.sensor_data[0].timestamp);
		sns_ddf_smgr_notify_data(dd_ptr->sub_dev[sub_dev_idx].smgr_handle, dd_ptr->common.sensor_data, num_sensors);
	}
}

/*===========================================================================

FUNCTION      apds9960_dd_als_rsp

DESCRIPTION   This function is called by the arbitration manager when ALS(light) data
is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void apds9960_dd_als_rsp
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
sns_ddf_sensor_e sensor
)
{
	uint16_t ailt;
	uint16_t aiht;
	uint16_t diff;
	
	if (dd_ptr->sub_dev[sub_dev_idx].dri_enabled)
	{
		if ( sensor == SNS_DDF_SENSOR_AMBIENT )
		{
			diff = PCNT_OF_RAW(dd_ptr->common.nv_db.als_change_pcnt,
							   dd_ptr->als.cdata);

			if (diff == 0)
			{
				diff = 5;
			}

			if (dd_ptr->als.cdata <= 5)
			{
				ailt = 0;
				aiht = (dd_ptr->als.cdata + 5);
			}
			else
			{
				ailt = (dd_ptr->als.cdata - diff);
				aiht = (dd_ptr->als.cdata + diff);
			}
			apds9960_dd_set_als_thresh(dd_ptr, ailt, aiht);
			ALSPRX_MSG_3(MED, "als_rsp - raw_data=%d ailt=%d aiht=%d",
						 dd_ptr->als.cdata, ailt, aiht);
		}
	}
	
	if (sensor == SNS_DDF_SENSOR_AMBIENT)
	{
		apds9960_dd_async_notify(dd_ptr, sub_dev_idx, sensor);
		dd_ptr->als.last_mlux  = dd_ptr->als.data_mlux;
	}
}

/*===========================================================================

FUNCTION      apds9960_dd_cct_raw_data_ready

DESCRIPTION   This function is called when the ALS raw data is ready

DEPENDENCIES  None

RETURN VALUE  int32_t	0 success, 1 pending, -1 error

SIDE EFFECT   None

===========================================================================*/
bool apds9960_dd_cct_raw_data_ready
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
sns_ddf_sensor_e sensor,
uint16_t cdata,
uint16_t rdata,
uint16_t gdata,
uint16_t bdata
)
{
	ALSPRX_MSG_2(MED, "cct_raw_data_ready sub device=%d, sensor=%d", sub_dev_idx, sensor);
	
	/* convert reading to cct */
	if (sensor == SNS_DDF_SENSOR_CT_C)
	{
		dd_ptr->cct.data_cct = apds9960_dd_rgb_convert_to_cct(dd_ptr,
															  cdata,
															  rdata,
															  gdata,
															  bdata,
															  FALSE);	/* not calibration mode */
		ALSPRX_MSG_1(MED, "data_cct=%d", dd_ptr->cct.data_cct);
	}
	
	return TRUE;
}

/*===========================================================================

FUNCTION      apds9960_dd_als_raw_data_ready

DESCRIPTION   This function is called when the ALS raw data is ready

DEPENDENCIES  None

RETURN VALUE  int32_t	0 success, 1 pending, -1 error

SIDE EFFECT   None

===========================================================================*/
bool apds9960_dd_als_raw_data_ready
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
sns_ddf_sensor_e sensor,
uint16_t cdata,
uint16_t rdata,
uint16_t gdata,
uint16_t bdata
)
{
	uint32_t adc_saturation_data;
	uint32_t adc_low_data;
	bool change_gain=FALSE;
	uint8_t i2c_data, i2c_read_data;
	uint8_t bytes_rw;
	
	//ALSPRX_MSG_2(MED, "als_raw_data_ready sub device=%d, sensor=%d", sub_dev_idx, sensor);
	
	if (sensor == SNS_DDF_SENSOR_AMBIENT)  // polling mode for ALS/PRX
	{
		/* convert reading to mlux */
		dd_ptr->als.data_mlux = apds9960_dd_rgb_convert_to_mlux(dd_ptr,
																cdata,
																rdata,
																gdata,
																bdata,
																FALSE); /* not calibratio mode */
		//ALSPRX_MSG_1(MED, "data_mlux=%d", dd_ptr->als.data_mlux);
	}
	
	/********************************************************
	* check lux value to see if a different range is needed
	*********************************************************/

	adc_saturation_data = sns_dd_als_res_tb[dd_ptr->als.als_res_index] * 99;
	adc_saturation_data = adc_saturation_data /100;
	
	adc_low_data = sns_dd_als_res_tb[dd_ptr->als.als_res_index] * 1;
	adc_low_data = adc_low_data /100;
	
	if ( cdata >= adc_saturation_data )
	{
		/* lower gain if possible */
		if (dd_ptr->als.als_gain_index != APDS9960_DD_ALS_GAIN_1X)
		{
			dd_ptr->als.als_gain_index--;
			change_gain = TRUE;
			
			ALSPRX_MSG_1(MED, "gain decrease to %d", sns_dd_als_again_tb[dd_ptr->als.als_gain_index]);
		}
	}
	else if (cdata < adc_low_data)
	{
		/* increase gain if possible */
		if (dd_ptr->als.als_gain_index != APDS9960_DD_ALS_GAIN_64X)
		{
			dd_ptr->als.als_gain_index++;
			change_gain = TRUE; // changing GAIN, don't inform SMGR
			
			ALSPRX_MSG_1(MED, "gain increase to %d", sns_dd_als_again_tb[dd_ptr->als.als_gain_index]);
		}
	}
	
	if (change_gain)
	{
		sns_ddf_read_port(dd_ptr->port_handle,
						  APDS9960_DD_CONTROL_ADDR,
						  (uint8_t*)&i2c_read_data, 1, &bytes_rw);
		
		i2c_data  = (i2c_read_data & 0xFC);
		i2c_data  = (i2c_data | sns_dd_als_again_bit_tb[dd_ptr->als.als_gain_index]);
		
		apds9960_dd_set_control(dd_ptr, i2c_data);
		
		return FALSE;	// changing GAIN, don't update SMGR first, wait for next cycle
	}
	else
	{
		if (sensor == SNS_DDF_SENSOR_AMBIENT)
		{
			apds9960_dd_als_rsp(dd_ptr, sub_dev_idx, sensor);
		}
		// kk 22-Jun-2015
		else if ( (sensor == SNS_DDF_SENSOR_RGB) || (sensor == SNS_DDF_SENSOR_CT_C) )
		{
			uint16_t ailt;
			uint16_t aiht;
			uint16_t diff;

			if (!dd_ptr->als.enable)
			{
				diff = PCNT_OF_RAW(dd_ptr->common.nv_db.als_change_pcnt,
								   cdata);

				if (diff == 0)
				{
					diff = 5;
				}

				if (cdata <= 5)
				{
					ailt = 0;
					aiht = (cdata + 5);
				}
				else
				{
					ailt = (cdata - diff);
					aiht = (cdata + diff);
				}

				apds9960_dd_set_als_thresh(dd_ptr, ailt, aiht);
			}
		}
	}
	
	return TRUE;	// ok, already update SMGR
}

/*===========================================================================

FUNCTION      apds9960_dd_prx_rsp

DESCRIPTION   This function is called by the proximity common handler when proximity data
is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void apds9960_dd_prx_rsp
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
uint8_t pdata
)
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	uint8_t bytes_rw;
	uint8_t i2c_data, psat_bit = 0x0;
	
	ALSPRX_MSG_3(MED, "thresh_far=%d, thresh_near=%d, pdata=%d",
				 dd_ptr->prx.thresh_far,
				 dd_ptr->prx.thresh_near,
				 pdata);
	
	status = sns_ddf_read_port(dd_ptr->port_handle,
							   APDS9960_DD_STATUS_ADDR,
							   (uint8_t*)&i2c_data, 1, &bytes_rw);
	
	if(status != SNS_DDF_SUCCESS)
	{
		ALSPRX_MSG_1(LOW, "apds9960_dd_prx_rsp status=%d", status);
		return;
	}
	
	psat_bit = (i2c_data&(APDS9960_DD_PSAT_STATUS|APDS9960_DD_PVALID_STATUS));
	
	if (psat_bit == (APDS9960_DD_PSAT_STATUS|APDS9960_DD_PVALID_STATUS))
	{
		ALSPRX_MSG_1(MED, "prx_rsp: PSAT is set, status=0x%x", i2c_data);
		
		if ( dd_ptr->prx.last_nearby != APDS9960_DD_PRX_FAR_AWAY )
		{
			dd_ptr->prx.detection_state = APDS9960_DD_PRX_FAR_AWAY;
			dd_ptr->prx.last_nearby = APDS9960_DD_PRX_FAR_AWAY;
			
			if (dd_ptr->sub_dev[sub_dev_idx].dri_enabled)
			{
				apds9960_dd_set_prx_thresh(dd_ptr,
										   APDS9960_DD_PILT_FAR,
										   dd_ptr->prx.thresh_near);
			}
		}
		
		/* need to clear the interrupt status in case of polling */
		if (!dd_ptr->sub_dev[sub_dev_idx].dri_enabled)
		{
			apds9960_dd_clear_int_reg(dd_ptr, APDS9960_DD_CMD_CLR_PS_INT);
		}
		
		return;
	}
	
	if ( pdata >= dd_ptr->prx.thresh_near )
	{
		if ( dd_ptr->prx.last_nearby != APDS9960_DD_PRX_NEAR_BY )
		{
			dd_ptr->prx.detection_state = APDS9960_DD_PRX_NEAR_BY;
		}
		
		dd_ptr->prx.last_nearby = APDS9960_DD_PRX_NEAR_BY;
		
		if (dd_ptr->sub_dev[sub_dev_idx].dri_enabled)
		{
			apds9960_dd_set_prx_thresh(dd_ptr, dd_ptr->prx.thresh_far, APDS9960_DD_PIHT_NEAR);
		}
	}
	else if ( pdata < dd_ptr->prx.thresh_far )
	{
		if ( dd_ptr->prx.last_nearby != APDS9960_DD_PRX_FAR_AWAY )
		{
			dd_ptr->prx.detection_state = APDS9960_DD_PRX_FAR_AWAY;
		}
		
		dd_ptr->prx.last_nearby = APDS9960_DD_PRX_FAR_AWAY;
		
		if (dd_ptr->sub_dev[sub_dev_idx].dri_enabled)
		{
			apds9960_dd_set_prx_thresh(dd_ptr, APDS9960_DD_PILT_FAR, dd_ptr->prx.thresh_near);
		}
	}

	/* kk 19-May-2015 */
	if (dd_ptr->prx.first_int_enable)
	{
		dd_ptr->prx.first_int_enable = FALSE;
		apds9960_dd_set_pers(dd_ptr, APDS9960_DD_PPERS_2|APDS9960_DD_APERS_2);
	}
}

/*===========================================================================

FUNCTION      apds9960_dd_prxcomm_rsp

DESCRIPTION   This function is called by the arbitration manager when proximity
data is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void apds9960_dd_prxcomm_rsp
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx
)
{
	apds9960_dd_prx_rsp(dd_ptr, sub_dev_idx, dd_ptr->prx.pdata);
	apds9960_dd_async_notify(dd_ptr, sub_dev_idx, SNS_DDF_SENSOR_PROXIMITY);
}

/*===========================================================================

FUNCTION      apds9960_dd_handle_device_indication

DESCRIPTION   This function is called by the arbitration manager when proximity
data is available

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void apds9960_dd_handle_device_indication
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
sns_ddf_sensor_e sensor
)
{
	uint8_t i2c_read_data[10];
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	uint8_t bytes_rw;
	int32_t als_update_flag=0;
	uint32_t data_mlux;
	uint32_t	calibrated_irradiance;
  	bool registry_update=TRUE;

	ALSPRX_MSG_2(MED, "handle_dev_ind - sub device=%d, sensor=%d", sub_dev_idx, sensor);
	
	if (sensor == SNS_DDF_SENSOR_AMBIENT)
	{
		/* read cdata, rdata, gdata and bdata */
		status = sns_ddf_read_port(dd_ptr->port_handle,
								   APDS9960_DD_CDATAL_ADDR,
								   (uint8_t*)&i2c_read_data, 8, &bytes_rw);
		
		if ( status != SNS_DDF_SUCCESS )
		{
			ALSPRX_MSG_0(ERROR, "handle_dev_ind - error read");
			return;
		}
		
		dd_ptr->als.cdata = (int16_t)(((int16_t)i2c_read_data[1] << 8) | i2c_read_data[0]);
		dd_ptr->als.rdata = (int16_t)(((int16_t)i2c_read_data[3] << 8) | i2c_read_data[2]);
		dd_ptr->als.gdata = (int16_t)(((int16_t)i2c_read_data[5] << 8) | i2c_read_data[4]);
		dd_ptr->als.bdata = (int16_t)(((int16_t)i2c_read_data[7] << 8) | i2c_read_data[6]);
		
		ALSPRX_MSG_2(MED, "AMBIENT: cdata %d rdata %d", dd_ptr->als.cdata, dd_ptr->als.rdata);
		ALSPRX_MSG_2(MED, "AMBIENT: gdata %d bdata %d", dd_ptr->als.gdata, dd_ptr->als.bdata);
		
		if (dd_ptr->als.calibration)
		{
			dd_ptr->als.calibration = FALSE;
			als_update_flag = 1;

			// complete
			data_mlux = apds9960_dd_rgb_convert_to_mlux(dd_ptr, dd_ptr->als.cdata,
														dd_ptr->als.rdata,
														dd_ptr->als.gdata,
														dd_ptr->als.bdata,
														TRUE);

			data_mlux = data_mlux/1000; // convert mlux to lux
			
			ALSPRX_MSG_1(MED, "handle_dev_ind - lux =%d", data_mlux);
			
			apds9960_dd_convert_to_irradiance(dd_ptr,
											sensor,
											dd_ptr->als.cdata,
											dd_ptr->als.rdata,
											dd_ptr->als.gdata,
											dd_ptr->als.bdata,
											TRUE);

			ALSPRX_MSG_2(MED, "c=%d, r=%d", dd_ptr->als.cdata_irradiance, dd_ptr->rgb.rdata_irradiance);
			ALSPRX_MSG_2(MED, "g=%d, b=%d", dd_ptr->rgb.gdata_irradiance, dd_ptr->rgb.bdata_irradiance);

			// scale up 10 times
			calibrated_irradiance = ((APDS9960_DD_ALS_CALIBRATED_LUX*146)/100);

			ALSPRX_MSG_2(MED, "calibrated_als_lux=%d, calibrated_irradiance=%d",
						APDS9960_DD_ALS_CALIBRATED_LUX,
						calibrated_irradiance);	
			
			/* CLEAR irradiance */
			if (((dd_ptr->als.cdata_irradiance*10) <= (calibrated_irradiance*110)/100) &&
				((dd_ptr->als.cdata_irradiance*10) >= (calibrated_irradiance*90)/100))
			{
				dd_ptr->common.nv_db.rgb_c_factor = 100;
			}
			else if (((dd_ptr->als.cdata_irradiance*10) <= (calibrated_irradiance*140)/100) &&
          			((dd_ptr->als.cdata_irradiance*10) >= (calibrated_irradiance*10)/100))
			{
				dd_ptr->common.nv_db.rgb_c_factor = (calibrated_irradiance*100)/(dd_ptr->als.cdata_irradiance*10);
			}
			else
			{
				registry_update = FALSE;
			}

			/* RED irradiance */
			if (((dd_ptr->rgb.rdata_irradiance*10) <= (calibrated_irradiance*110)/100) &&
				((dd_ptr->rgb.rdata_irradiance*10) >= (calibrated_irradiance*90)/100))
			{
				dd_ptr->common.nv_db.rgb_r_factor = 100;
			}
			else if (((dd_ptr->rgb.rdata_irradiance*10) <= (calibrated_irradiance*140)/100) &&
          			((dd_ptr->rgb.rdata_irradiance*10) >= (calibrated_irradiance*10)/100))
			{
				dd_ptr->common.nv_db.rgb_r_factor = (calibrated_irradiance*100)/(dd_ptr->rgb.rdata_irradiance*10);
			}
			else
			{
				registry_update = FALSE;
			}

			/* GREEN irradiance */
			if (((dd_ptr->rgb.gdata_irradiance*10) <= (calibrated_irradiance*110)/100) &&
				((dd_ptr->rgb.gdata_irradiance*10) >= (calibrated_irradiance*90)/100))
			{
				dd_ptr->common.nv_db.rgb_g_factor = 100;
			}
			else if (((dd_ptr->rgb.gdata_irradiance*10) <= (calibrated_irradiance*140)/100) &&
          			((dd_ptr->rgb.gdata_irradiance*10) >= (calibrated_irradiance*10)/100))
			{
				dd_ptr->common.nv_db.rgb_g_factor = (calibrated_irradiance*100)/(dd_ptr->rgb.gdata_irradiance*10);
			}
			else
			{
				registry_update = FALSE;
			}

			/* BLUE irradiance */
			if (((dd_ptr->rgb.bdata_irradiance*10) <= (calibrated_irradiance*110)/100) &&
				((dd_ptr->rgb.bdata_irradiance*10) >= (calibrated_irradiance*90)/100))
			{
				dd_ptr->common.nv_db.rgb_b_factor = 100;
			}
			else if (((dd_ptr->rgb.bdata_irradiance*10) <= (calibrated_irradiance*140)/100) &&
          			((dd_ptr->rgb.bdata_irradiance*10) >= (calibrated_irradiance*10)/100))
			{
				dd_ptr->common.nv_db.rgb_b_factor = (calibrated_irradiance*100)/(dd_ptr->rgb.bdata_irradiance*10);
			}
			else
			{
				registry_update = FALSE;
			}

			if ( (data_mlux <= (APDS9960_DD_ALS_CALIBRATED_LUX*110)/100) &&
				(data_mlux >= (APDS9960_DD_ALS_CALIBRATED_LUX*90)/100)) 
			{   
				dd_ptr->common.nv_db.als_factor = 100;            
			}      
			else if ( (data_mlux >= (APDS9960_DD_ALS_CALIBRATED_LUX*10)/100) && 
						(data_mlux <= (APDS9960_DD_ALS_CALIBRATED_LUX*140)/100) ) 
			{
                   
				dd_ptr->common.nv_db.als_factor = (APDS9960_DD_ALS_CALIBRATED_LUX * 100)/data_mlux;          
			}
			else
			{
				registry_update = FALSE;
			}

			ALSPRX_MSG_1(MED, "als_factor=%d", dd_ptr->common.nv_db.als_factor);
			ALSPRX_MSG_2(MED, "c_f=%d, r_f=%d", dd_ptr->common.nv_db.rgb_c_factor, dd_ptr->common.nv_db.rgb_r_factor);
			ALSPRX_MSG_2(MED, "g_f=%d, b_f=%d", dd_ptr->common.nv_db.rgb_g_factor, dd_ptr->common.nv_db.rgb_b_factor);

			if (registry_update)
			{
				sns_ddf_smgr_notify_test_complete(dd_ptr->sub_dev[sub_dev_idx].smgr_handle, sensor, SNS_DDF_SUCCESS, 0);
				sns_ddf_smgr_notify_event(dd_ptr->sub_dev[sub_dev_idx].smgr_handle, sensor, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);				
			}
			else
			{
				sns_ddf_smgr_notify_test_complete(dd_ptr->sub_dev[sub_dev_idx].smgr_handle, sensor, SNS_DDF_EFAIL, APDS9960_DD_ALS_CAL_FAILED);				
			}

			// Revert back to previous register setting
			dd_ptr->als.enable = FALSE;
			apds9960_dd_set_powerstate(dd_ptr, sub_dev_idx, sensor, SNS_DDF_POWERSTATE_LOWPOWER, FALSE);

			return;
		}
		else if ((dd_ptr->sub_dev[sub_dev_idx].dri_enabled) ||
				 ((!(dd_ptr->sub_dev[sub_dev_idx].dri_enabled)) && (dd_ptr->als.polling_data))) // do I have to check these conditions???
		{
			/* indicate the data to the light(ALS) sub-driver */
			als_update_flag = apds9960_dd_als_raw_data_ready(dd_ptr,
															 sub_dev_idx,
															 sensor,
															 dd_ptr->als.cdata,
															 dd_ptr->als.rdata,
															 dd_ptr->als.gdata,
															 dd_ptr->als.bdata);
			if (als_update_flag)
			{
				// if this flag is set, it has already sent notification to SMGR
				dd_ptr->als.polling_data = FALSE;
			}
		}
	}
	else if (sensor == SNS_DDF_SENSOR_RGB)
	{
		/* read cdata, rdata, gdata and bdata */
		status = sns_ddf_read_port(dd_ptr->port_handle,
								   APDS9960_DD_CDATAL_ADDR,
								   (uint8_t*)&i2c_read_data, 8, &bytes_rw);
		
		if ( status != SNS_DDF_SUCCESS )
		{
			ALSPRX_MSG_0(LOW, "handle_dev_ind - error read");
			return;
		}
		
		dd_ptr->rgb.cdata = (int16_t)(((int16_t)i2c_read_data[1] << 8) | i2c_read_data[0]);
		dd_ptr->rgb.rdata = (int16_t)(((int16_t)i2c_read_data[3] << 8) | i2c_read_data[2]);
		dd_ptr->rgb.gdata = (int16_t)(((int16_t)i2c_read_data[5] << 8) | i2c_read_data[4]);
		dd_ptr->rgb.bdata = (int16_t)(((int16_t)i2c_read_data[7] << 8) | i2c_read_data[6]);
		
		/* check if gain need to adjust before reporting to SMGR */
		als_update_flag = apds9960_dd_als_raw_data_ready(dd_ptr,
														 sub_dev_idx,
														 sensor,
														 dd_ptr->rgb.cdata,
														 dd_ptr->rgb.rdata,
														 dd_ptr->rgb.gdata,
														 dd_ptr->rgb.bdata);
			
		if (als_update_flag)
		{
			apds9960_dd_async_notify(dd_ptr, sub_dev_idx, sensor);
			
			if (dd_ptr->rgb.polling_data)
			{
				dd_ptr->rgb.polling_data = FALSE;
			}
		}
	}
	else if (sensor == SNS_DDF_SENSOR_CT_C)
	{
		/* read cdata, rdata, gdata and bdata */
		status = sns_ddf_read_port(dd_ptr->port_handle,
								   APDS9960_DD_CDATAL_ADDR,
								   (uint8_t*)&i2c_read_data, 8, &bytes_rw);
		
		if ( status != SNS_DDF_SUCCESS )
		{
			ALSPRX_MSG_0(ERROR, "handle_dev_ind - error read");
			return;
		}
		
		dd_ptr->cct.cdata = (int16_t)(((int16_t)i2c_read_data[1] << 8) | i2c_read_data[0]);
		dd_ptr->cct.rdata = (int16_t)(((int16_t)i2c_read_data[3] << 8) | i2c_read_data[2]);
		dd_ptr->cct.gdata = (int16_t)(((int16_t)i2c_read_data[5] << 8) | i2c_read_data[4]);
		dd_ptr->cct.bdata = (int16_t)(((int16_t)i2c_read_data[7] << 8) | i2c_read_data[6]);
		
		ALSPRX_MSG_3(MED, "CT_C: rdata %d gdata %d bdata %d", dd_ptr->cct.rdata, dd_ptr->cct.gdata, dd_ptr->cct.bdata);

		/* check if gain need to adjust before reporting to SMGR */
		als_update_flag = apds9960_dd_als_raw_data_ready(dd_ptr,
														 sub_dev_idx,
														 sensor,
														 dd_ptr->cct.cdata,
														 dd_ptr->cct.rdata,
														 dd_ptr->cct.gdata,
														 dd_ptr->cct.bdata);
			
		if (als_update_flag)
		{
			apds9960_dd_cct_raw_data_ready(dd_ptr, sub_dev_idx, sensor,
											   dd_ptr->cct.cdata, dd_ptr->cct.rdata, dd_ptr->cct.gdata, dd_ptr->cct.bdata);
			apds9960_dd_async_notify(dd_ptr, sub_dev_idx, sensor);
				
			if (dd_ptr->cct.polling_data)
			{
				dd_ptr->cct.polling_data = FALSE;
			}
		}
	}
	else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
	{
		/* read pdata */
		status = sns_ddf_read_port(dd_ptr->port_handle,
								   APDS9960_DD_PDATA_ADDR,
								   (uint8_t*)&i2c_read_data, 1, &bytes_rw);
		
		if ( status != SNS_DDF_SUCCESS )
		{
			ALSPRX_MSG_0(ERROR, "handle_dev_ind - error read");
			return;
		}
		
		dd_ptr->prx.pdata = i2c_read_data[0];
		
		ALSPRX_MSG_1(MED, "PROXIMITY: pdata %d", dd_ptr->prx.pdata);
		
		if (dd_ptr->prx.calibration == 1 || dd_ptr->prx.calibration == 2)
		{
			if (dd_ptr->prx.calibration_loop <= 127)
			{
				if ((dd_ptr->prx.pdata <= APDS9960_DD_PS_CALIBRATED_XTALK) && (dd_ptr->prx.pdata >= APDS9960_DD_PS_CALIBRATED_XTALK_BASELINE))
				{
					if (dd_ptr->prx.calibration == 1)
					{
						dd_ptr->prx.poffset_ur = dd_ptr->common.temp_offset;
						dd_ptr->common.nv_db.poffset_ur = dd_ptr->common.temp_offset;
						
						dd_ptr->prx.calibration = 2;
						dd_ptr->prx.calibration_loop = 0;
						dd_ptr->common.temp_offset = dd_ptr->common.poffset_dl;
						
						apds9960_dd_set_enable(dd_ptr, 0x01);
						apds9960_dd_set_config2(dd_ptr, 0x29);		
						apds9960_dd_set_enable(dd_ptr, APDS9960_DD_PWR_ON|APDS9960_DD_PRX_ENABLE);
						
						apds9960_dd_start_general_timer (dd_ptr, sub_dev_idx, SNS_DDF_SENSOR_PROXIMITY, 10000);
					}
					else
					{
						dd_ptr->prx.poffset_dl = dd_ptr->common.temp_offset;
						dd_ptr->common.nv_db.poffset_dl = dd_ptr->common.temp_offset;

						dd_ptr->prx.enable = FALSE;
						dd_ptr->prx.calibration = 0;
						dd_ptr->prx.calibration_loop = 0;
						dd_ptr->common.temp_offset = 0;	// reset

						apds9960_dd_set_config2(dd_ptr, 0x00);
						apds9960_dd_set_poffset_ur(dd_ptr, dd_ptr->common.poffset_ur);
						apds9960_dd_set_poffset_dl(dd_ptr, dd_ptr->common.poffset_dl);

						apds9960_dd_set_powerstate(dd_ptr, sub_dev_idx, sensor, SNS_DDF_POWERSTATE_LOWPOWER, FALSE);

						sns_ddf_smgr_notify_test_complete(dd_ptr->sub_dev[sub_dev_idx].smgr_handle, SNS_DDF_SENSOR_PROXIMITY, SNS_DDF_SUCCESS, 0);
						sns_ddf_smgr_notify_event(dd_ptr->sub_dev[sub_dev_idx].smgr_handle, SNS_DDF_SENSOR_PROXIMITY, SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
					}
					
					return;
				}
				else
				{
					if (dd_ptr->prx.pdata > APDS9960_DD_PS_CALIBRATED_XTALK)
					{
						// reduce
						if ((dd_ptr->common.temp_offset >= 0) && (dd_ptr->common.temp_offset < 127))
						{
							dd_ptr->common.temp_offset += 1;
						}
						else
						{
							if (dd_ptr->common.temp_offset == 127) dd_ptr->common.temp_offset = 1;

							dd_ptr->common.temp_offset -= 1;
						}		
					}		
					else if (dd_ptr->prx.pdata < APDS9960_DD_PS_CALIBRATED_XTALK_BASELINE)
					{
						// increase
						if ((dd_ptr->common.temp_offset > 0) && (dd_ptr->common.temp_offset <= 127))
						{
							dd_ptr->common.temp_offset -= 1;
						}		
						else
						{
							if (dd_ptr->common.temp_offset == 255) dd_ptr->common.temp_offset = 0;; // something is wrong
							if (dd_ptr->common.temp_offset == 0) dd_ptr->common.temp_offset = 127; // start from 128

							dd_ptr->common.temp_offset += 1;	// start from 128
							dd_ptr->common.temp_offset = dd_ptr->common.temp_offset&0xFF;
						}					
					}		
				}
				
				if (dd_ptr->prx.calibration == 1)
				{
				    ALSPRX_MSG_1(MED, "write ur 0x%x", dd_ptr->common.temp_offset);
					apds9960_dd_set_poffset_ur(dd_ptr, dd_ptr->common.temp_offset);
				}
				else
				{
				    ALSPRX_MSG_1(MED, "write dl 0x%x", dd_ptr->common.temp_offset);
					apds9960_dd_set_poffset_dl(dd_ptr, dd_ptr->common.temp_offset);
				}
				
				dd_ptr->prx.calibration_loop++;
				apds9960_dd_start_general_timer (dd_ptr, sub_dev_idx, SNS_DDF_SENSOR_PROXIMITY, 10000);
				return; // continue getting data
			}
			else
			{
				// failed to calibrate
				// revert back previous registers setting
				apds9960_dd_set_config2(dd_ptr, 0x00);
				apds9960_dd_set_poffset_ur(dd_ptr, dd_ptr->common.poffset_ur);
				apds9960_dd_set_poffset_dl(dd_ptr, dd_ptr->common.poffset_dl);

				apds9960_dd_set_powerstate(dd_ptr, sub_dev_idx, sensor, SNS_DDF_POWERSTATE_LOWPOWER, TRUE);
				sns_ddf_smgr_notify_test_complete(dd_ptr->sub_dev[sub_dev_idx].smgr_handle, sensor, SNS_DDF_EFAIL, APDS9960_DD_PRX_CAL_FAILED);
			}
		}
		else if ((dd_ptr->sub_dev[sub_dev_idx].dri_enabled) ||
				 ((!dd_ptr->sub_dev[sub_dev_idx].dri_enabled) && (dd_ptr->prx.polling_data)))
		{
			/* indicate the data to the proximity common handler */
			apds9960_dd_prxcomm_rsp(dd_ptr, sub_dev_idx);
			dd_ptr->prx.polling_data = FALSE;
		}
	}
	
	if (!dd_ptr->sub_dev[sub_dev_idx].dri_enabled) // polling sub-device
	{
		if (sensor ==  SNS_DDF_SENSOR_AMBIENT)
		{
			if (!als_update_flag)
			{
				apds9960_dd_start_general_timer (dd_ptr, sub_dev_idx, sensor, dd_ptr->als.timer_value);
				return;
			}
			
			// kk 11-Jun-2015 : set FALSE during power off
			//dd_ptr->als.enable = FALSE;
		}
		else if (sensor == SNS_DDF_SENSOR_RGB)
		{
			if (!als_update_flag)
			{
				apds9960_dd_start_general_timer (dd_ptr, sub_dev_idx, sensor, dd_ptr->rgb.timer_value);
				return;
			}
			
			// kk 11-Jun-2015 : set FALSE during power off
			//dd_ptr->rgb.enable = FALSE;
		}
		else if (sensor == SNS_DDF_SENSOR_CT_C)
		{
			if (!als_update_flag)
			{
				apds9960_dd_start_general_timer (dd_ptr, sub_dev_idx, sensor, dd_ptr->cct.timer_value);
				return;
			}

			// kk 11-Jun-2015 : set FALSE during power off
			//dd_ptr->cct.enable = FALSE;
		}
		else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
		{
			// kk 11-Jun-2015 : set FALSE during power off
			//dd_ptr->prx.enable = FALSE;
		}
	}
}

/*===========================================================================

FUNCTION:   sns_dd_apds9960_handle_irq

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
void sns_dd_apds9960_handle_irq
(
sns_ddf_handle_t dd_handle,
uint32_t gpio_num,
sns_ddf_time_t timestamp
)
{
	apds9960_dd_dev_s* dd_ptr;
	uint32_t sub_dev_idx;
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	uint8_t bytes_rw;
	uint8_t i2c_data = 0x0;
	uint8_t disable_data=APDS9960_DD_PWR_ON;
	uint8_t reg_addr=0;
	uint8_t status_reg=0;
	uint8_t enable_reg=0;
#if 0 // kk 22-Jun-2015
	uint16_t ailt;
	uint16_t aiht;
	uint16_t diff;	
#endif

	sub_dev_idx = (uint32_t)dd_handle & (APDS9960_DD_HANDLE_ALIGN-1);
	dd_ptr = (apds9960_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9960_DD_HANDLE_ALIGN-1)));

    if ((sub_dev_idx >= APDS9960_DD_MAX_SUB_DEV) || (dd_ptr == NULL))
    {
       return;
    }

	dd_ptr->int_timestamp = timestamp;

    //UMSG(MSG_SSID_SNS,DBG_HIGH_PRIO, "APDS9960 irq");
	
	ALSPRX_MSG_2(MED, "handle_irq start - sub device=%d, gpio_num=0x%x", sub_dev_idx, gpio_num);
	
	/* STATUS */
	status = sns_ddf_read_port(dd_ptr->port_handle,
							   APDS9960_DD_STATUS_ADDR,
							   (uint8_t*)&i2c_data, 1, &bytes_rw);
	
	if(status != SNS_DDF_SUCCESS)
	{
		ALSPRX_MSG_1(ERROR, "handle_irq - error read =%d", status);
		return;
	}
	
	ALSPRX_MSG_1(MED, "status_reg=0x%x", i2c_data);
	
	status_reg = i2c_data;
	
	status = sns_ddf_read_port(dd_ptr->port_handle,
                             APDS9960_DD_ENABLE_ADDR,
                             (uint8_t*)&i2c_data, 1, &bytes_rw);
  
	if(status != SNS_DDF_SUCCESS) 
	{
		ALSPRX_MSG_1(ERROR, "irq - error read =%d", status);
		return;
	}

	ALSPRX_MSG_1(MED, "enable=0x%x", i2c_data);

	enable_reg = i2c_data;

	status = sns_ddf_write_port(dd_ptr->port_handle,
                              APDS9960_DD_ENABLE_ADDR,
                              (uint8_t*)&disable_data, 
                              1,
                              &bytes_rw);
  
	if ( status != SNS_DDF_SUCCESS ) 
	{
		ALSPRX_MSG_1(ERROR, "handle_irq - error write =%d", status);
		return;
	}  


	/* Check which interrupts occured */
	if ((status_reg & APDS9960_DD_PINT_STATUS) && dd_ptr->prx.enable)
	{
		if (dd_ptr->sub_dev[als_prx_idx].dri_enabled)
		{
			/* Proximity Interrupt */
			apds9960_dd_handle_device_indication(dd_ptr,
												 als_prx_idx,
												 SNS_DDF_SENSOR_PROXIMITY);
		
			reg_addr |= APDS9960_DD_CMD_CLR_PS_INT;
		}
	}
	else if (status_reg & APDS9960_DD_PINT_STATUS)	/* kk 23-Jul-2014 */
	{
		ALSPRX_MSG_0(ERROR, "irq - prox but no enable");
		reg_addr |= APDS9960_DD_CMD_CLR_PS_INT;
	}
	
	if ((status_reg & APDS9960_DD_AINT_STATUS) && dd_ptr->als.enable)
	{
		if (dd_ptr->sub_dev[als_prx_idx].dri_enabled)
		{		
			// set flat to TRUE to inform handle_device_indication to report accordingly
			dd_ptr->als.polling_data = TRUE;
		
			apds9960_dd_handle_device_indication(dd_ptr,
												 als_prx_idx,
												 SNS_DDF_SENSOR_AMBIENT);
		
			reg_addr |= APDS9960_DD_CMD_CLR_ALS_INT;
		}
	}
	
	if((status_reg & APDS9960_DD_AINT_STATUS) && dd_ptr->rgb.enable)
	{
		if (dd_ptr->sub_dev[rgb_cct_idx].dri_enabled)
		{
			/* RGB Interrupt */
			ALSPRX_MSG_0(MED, "irq - rgb");
		
			dd_ptr->rgb.polling_data = TRUE;
		
			apds9960_dd_handle_device_indication(dd_ptr,
												 rgb_cct_idx,
												 SNS_DDF_SENSOR_RGB);
		
			reg_addr |= APDS9960_DD_CMD_CLR_ALS_INT;

#if 0 // kk 22-Jun-2015
			// Update ALS threshold here as ALS is not enabled
			if (!dd_ptr->als.enable)
			{
				diff = PCNT_OF_RAW(dd_ptr->common.nv_db.als_change_pcnt,
								   dd_ptr->rgb.cdata);

				if (diff == 0)
				{
					diff = 5;
				}

				if (dd_ptr->rgb.cdata <= 5)
				{
					ailt = 0;
					aiht = (dd_ptr->rgb.cdata + 5);
				}
				else
				{
					ailt = (dd_ptr->rgb.cdata - diff);
					aiht = (dd_ptr->rgb.cdata + diff);
				}

				apds9960_dd_set_als_thresh(dd_ptr, ailt, aiht);
			}
#endif
		}
	}
	
	if((status_reg & APDS9960_DD_AINT_STATUS) && dd_ptr->cct.enable)
	{
		if (dd_ptr->sub_dev[rgb_cct_idx].dri_enabled)
		{
			/* CCT Interrupt */
			ALSPRX_MSG_0(MED, "irq - cct");

			dd_ptr->cct.polling_data = TRUE;
			
			apds9960_dd_handle_device_indication(dd_ptr,
												 rgb_cct_idx,
												 SNS_DDF_SENSOR_CT_C);
			
			reg_addr |= APDS9960_DD_CMD_CLR_ALS_INT;

#if 0 // kk 22-Jun-2015
			// Update ALS threshold here as ALS is not enabled
			// i don't think I need to redo this if RGB has already done it
			if (!dd_ptr->als.enable)
			{
				diff = PCNT_OF_RAW(dd_ptr->common.nv_db.als_change_pcnt,
								   dd_ptr->cct.cdata);

				if (diff == 0)
				{
					diff = 5;
				}

				if (dd_ptr->cct.cdata <= 5)
				{
					ailt = 0;
					aiht = (dd_ptr->cct.cdata + 5);
				}
				else
				{
					ailt = (dd_ptr->cct.cdata - diff);
					aiht = (dd_ptr->cct.cdata + diff);
				}

				apds9960_dd_set_als_thresh(dd_ptr, ailt, aiht);
			}
#endif
		}
	}
	
#ifdef ALSPRX_DEBUG
    if ((status_reg & APDS9960_DD_PINT_STATUS ) && !(dd_ptr->prx.enable))
		MSG(MSG_SSID_SNS,DBG_ERROR_PRIO, "ALSPRX - PINT set, but PRX not enabled");
	
    if ((status_reg & APDS9960_DD_AINT_STATUS) && !(dd_ptr->als.enable))
		MSG(MSG_SSID_SNS,DBG_ERROR_PRIO, "ALSPRX - AINT set, but ALS not enabled");
#endif
	
	/* Clear interrupts */
	if (reg_addr != 0)
	{
		status = apds9960_dd_clear_int_reg(dd_ptr, reg_addr);
	
		ALSPRX_MSG_2(MED, "clear int=0x%x, result=%d", reg_addr, status);
	}
	
	/* Enable Sensor */
	i2c_data = enable_reg;  // write back the previous value
	status = sns_ddf_write_port(dd_ptr->port_handle,
								APDS9960_DD_ENABLE_ADDR,
								(uint8_t*)&i2c_data,
								1,
								&bytes_rw);

	ALSPRX_MSG_2(MED, "enable=0x%x, result=%d", i2c_data, status);
	
	if ( status != SNS_DDF_SUCCESS )
	{
		return;
	}
	
	ALSPRX_MSG_0(MED, "handle_irq end");
}

/*===========================================================================

FUNCTION:   sns_dd_apds9960_handle_timer

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
void sns_dd_apds9960_handle_timer
(
sns_ddf_handle_t dd_handle,
void* arg
)
{
	apds9960_dd_dev_s* dd_ptr;
	uint32_t sub_dev_idx;
	sns_ddf_sensor_e sensor=*(sns_ddf_sensor_e*)arg;
	
	sub_dev_idx = (uint32_t)dd_handle & (APDS9960_DD_HANDLE_ALIGN-1);
	dd_ptr = (apds9960_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9960_DD_HANDLE_ALIGN-1)));

    if ((sub_dev_idx >= APDS9960_DD_MAX_SUB_DEV) || (dd_ptr == NULL))
    {
       return;
    }
	
	ALSPRX_MSG_2(MED, "handle_timer - sub device=%d, sensor=%d", sub_dev_idx, sensor);
	
	if (sensor == SNS_DDF_SENSOR_AMBIENT)
	{
		if ( FALSE == dd_ptr->als.timer_active ) return; // when sensor is als
	}
	else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
	{
		if ( FALSE == dd_ptr->prx.timer_active ) return; // when sensor is prx
	}
	else if (sensor == SNS_DDF_SENSOR_RGB)
	{
		if ( FALSE == dd_ptr->rgb.timer_active ) return; // when sensor is rgb
	}
	else if (sensor == SNS_DDF_SENSOR_CT_C)
	{
		if ( FALSE == dd_ptr->cct.timer_active ) return; // when sensor is cct
	}
	
	apds9960_dd_stop_general_timer(dd_ptr, sub_dev_idx, sensor);
	
	apds9960_dd_handle_device_indication(dd_ptr, sub_dev_idx, sensor);
}

/*==============================================================================

FUNCTION      apds9960_dd_rgb_init

DESCRIPTION   Initializes the RGB driver

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e apds9960_dd_rgb_init
(
apds9960_dd_dev_s* dd_ptr
)
{
	apds9960_dd_rgb_s *rgb_ptr = &dd_ptr->rgb;
	uint8_t i2c_data;
	uint8_t bytes_rw;
	sns_ddf_status_e status = SNS_DDF_SUCCESS;

	status = sns_ddf_read_port(dd_ptr->port_handle,
					  APDS9960_DD_ATIME_ADDR,
					  (uint8_t*)&i2c_data, 1, &bytes_rw);

	if (status == SNS_DDF_SUCCESS)
	{
		// use the existing atime from sensor
		if (i2c_data == sns_dd_als_atime_tb[APDS9960_DD_RES_14BIT])
		{
			dd_ptr->als.als_res_index = APDS9960_DD_RES_14BIT;

		}
		else if (i2c_data == sns_dd_als_atime_tb[APDS9960_DD_RES_15BIT])
		{
			dd_ptr->als.als_res_index = APDS9960_DD_RES_15BIT;

		}
		else if (i2c_data == sns_dd_als_atime_tb[APDS9960_DD_RES_16BIT])
		{
			dd_ptr->als.als_res_index = APDS9960_DD_RES_16BIT;

		}
		else
		{
			dd_ptr->als.als_res_index = APDS9960_DD_RES_15BIT;
		}
	}
	else
	{
		// use default atime
		dd_ptr->als.als_res_index = APDS9960_DD_RES_15BIT;
	}
	
	status = sns_ddf_read_port(dd_ptr->port_handle,
					  APDS9960_DD_CONTROL_ADDR,
					  (uint8_t*)&i2c_data, 1, &bytes_rw);

	if (status == SNS_DDF_SUCCESS)
	{
		// use the existing gain from sensor
		dd_ptr->als.als_gain_index = (i2c_data&APDS9960_DD_ALS_GAIN_64X);
	}
	else
	{
		// use default gain
		dd_ptr->als.als_gain_index = APDS9960_DD_ALS_GAIN_4X;
	}
	
	ALSPRX_MSG_2(MED, "rgb_init gain_idx=%d, res_idx=%d", dd_ptr->als.als_gain_index, dd_ptr->als.als_res_index);
	
	rgb_ptr->enable = FALSE;
	rgb_ptr->timer_active = FALSE;	
	rgb_ptr->polling_data = FALSE;
	rgb_ptr->calibration = FALSE;
	rgb_ptr->r_c_ratio = 0;
	rgb_ptr->timer_value = 0;
	
	return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      apds9960_dd_cct_init

DESCRIPTION   initializes the CT_C

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e apds9960_dd_cct_init
(
apds9960_dd_dev_s* dd_ptr
)
{
	apds9960_dd_cct_s *cct_ptr = &dd_ptr->cct;
	uint8_t i2c_data;
	uint8_t bytes_rw;
	sns_ddf_status_e status = SNS_DDF_SUCCESS;

	status = sns_ddf_read_port(dd_ptr->port_handle,
					  APDS9960_DD_ATIME_ADDR,
					  (uint8_t*)&i2c_data, 1, &bytes_rw);

	if (status == SNS_DDF_SUCCESS)
	{
		// use the existing atime from sensor
		if (i2c_data == sns_dd_als_atime_tb[APDS9960_DD_RES_14BIT])
		{
			dd_ptr->als.als_res_index = APDS9960_DD_RES_14BIT;

		}
		else if (i2c_data == sns_dd_als_atime_tb[APDS9960_DD_RES_15BIT])
		{
			dd_ptr->als.als_res_index = APDS9960_DD_RES_15BIT;

		}
		else if (i2c_data == sns_dd_als_atime_tb[APDS9960_DD_RES_16BIT])
		{
			dd_ptr->als.als_res_index = APDS9960_DD_RES_16BIT;

		}
		else
		{
			dd_ptr->als.als_res_index = APDS9960_DD_RES_15BIT;
		}
	}
	else
	{
		// use default atime
		dd_ptr->als.als_res_index = APDS9960_DD_RES_15BIT;
	}
	
	status = sns_ddf_read_port(dd_ptr->port_handle,
					  APDS9960_DD_CONTROL_ADDR,
					  (uint8_t*)&i2c_data, 1, &bytes_rw);

	if (status == SNS_DDF_SUCCESS)
	{
		// use the existing gain from sensor
		dd_ptr->als.als_gain_index = (i2c_data&APDS9960_DD_ALS_GAIN_64X);
	}
	else
	{
		// use default gain
		dd_ptr->als.als_gain_index = APDS9960_DD_ALS_GAIN_4X;
	}
	
	ALSPRX_MSG_2(MED, "cct_init gain_idx=%d, res_idx=%d", dd_ptr->als.als_gain_index, dd_ptr->als.als_res_index);
	
	cct_ptr->enable = FALSE;
	cct_ptr->timer_active = FALSE;
	cct_ptr->polling_data = FALSE;
	cct_ptr->calibration = FALSE;
	cct_ptr->r_c_ratio = 0;
	cct_ptr->timer_value = 0;
	
	return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      apds9960_dd_als_init

DESCRIPTION   Initializes the light(ALS) driver

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e apds9960_dd_als_init
(
apds9960_dd_dev_s* dd_ptr
)
{
	apds9960_dd_als_s *als_ptr = &dd_ptr->als;
	uint8_t i2c_data;
	uint8_t bytes_rw;
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	
	als_ptr->last_mlux = APDS9960_DD_ALS_DFLT_MILLI_LUX;

	status = sns_ddf_read_port(dd_ptr->port_handle,
					  APDS9960_DD_ATIME_ADDR,
					  (uint8_t*)&i2c_data, 1, &bytes_rw);

	if (status == SNS_DDF_SUCCESS)
	{
		// use the existing atime from sensor
		if (i2c_data == sns_dd_als_atime_tb[APDS9960_DD_RES_14BIT])
		{
			als_ptr->als_res_index = APDS9960_DD_RES_14BIT;

		}
		else if (i2c_data == sns_dd_als_atime_tb[APDS9960_DD_RES_15BIT])
		{
			als_ptr->als_res_index = APDS9960_DD_RES_15BIT;

		}
		else if (i2c_data == sns_dd_als_atime_tb[APDS9960_DD_RES_16BIT])
		{
			als_ptr->als_res_index = APDS9960_DD_RES_16BIT;

		}
		else
		{
			als_ptr->als_res_index = APDS9960_DD_RES_15BIT;
		}
	}
	else
	{
		// use default atime
		als_ptr->als_res_index = APDS9960_DD_RES_15BIT;
	}

	
	status = sns_ddf_read_port(dd_ptr->port_handle,
					  APDS9960_DD_CONTROL_ADDR,
					  (uint8_t*)&i2c_data, 1, &bytes_rw);

	if (status == SNS_DDF_SUCCESS)
	{
		// use the existing gain from sensor
		als_ptr->als_gain_index = (i2c_data&APDS9960_DD_ALS_GAIN_64X);
	}
	else
	{
		// use default gain
		als_ptr->als_gain_index = APDS9960_DD_ALS_GAIN_4X;
	}
	
	ALSPRX_MSG_2(MED, "als_init gain_idx=%d, res_idx=%d", als_ptr->als_gain_index, als_ptr->als_res_index);

	als_ptr->enable = FALSE;
	als_ptr->timer_active = FALSE;	
	als_ptr->polling_data = FALSE;  // polling enable flag
	als_ptr->calibration = FALSE;	
	als_ptr->saturated_lux = APDS9960_DD_ALS_MAX_LUX;
	als_ptr->r_c_ratio = 0;
	als_ptr->timer_value = 0;

	return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      apds9960_dd_prx_init

DESCRIPTION   initializes the ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e apds9960_dd_prx_init
(
apds9960_dd_dev_s* dd_ptr
)
{
	apds9960_dd_prx_s *prx_ptr = &dd_ptr->prx;
	
	prx_ptr->last_nearby = APDS9960_DD_PRX_NEAR_BY_UNKNOWN;
	prx_ptr->thresh_near = dd_ptr->common.nv_db.thresh_near;
	prx_ptr->thresh_far = dd_ptr->common.nv_db.thresh_far;
	
	prx_ptr->ppulse = dd_ptr->common.nv_db.ppulse;
	prx_ptr->poffset_ur = dd_ptr->common.nv_db.poffset_ur;
	prx_ptr->poffset_dl = dd_ptr->common.nv_db.poffset_dl;
	
	prx_ptr->polling_data = FALSE;
	prx_ptr->calibration = 0;
	prx_ptr->calibration_loop = 0;
	
	prx_ptr->enable = FALSE;
	prx_ptr->timer_active = FALSE;
	prx_ptr->timer_value = 0;

	/* kk 19-May-2015 */
	prx_ptr->first_int_enable = FALSE;
	
	return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      apds9960_dd_sub_dev_init

DESCRIPTION   initializes the ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e apds9960_dd_sub_dev_init
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx
)
{
	apds9960_dd_sub_dev_s *sub_dev_ptr = &dd_ptr->sub_dev[sub_dev_idx];
	
	sub_dev_ptr->pwr_mode = SNS_DDF_POWERSTATE_LOWPOWER;
	sub_dev_ptr->state = APDS9960_DD_DEV_STOPPED;
	
	sub_dev_ptr->dri_registration = FALSE;
	
	return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      apds9960_dd_common_init

DESCRIPTION   initializes the ALS/Proximty

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e apds9960_dd_common_init
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx
)
{
	apds9960_dd_common_s *common_ptr = &dd_ptr->common;
	
	if (sub_dev_idx == als_prx_idx)
	{
		common_ptr->prx_odr = 0;
		common_ptr->als_odr = 0;
	}
	else if (sub_dev_idx == rgb_cct_idx)
	{
		common_ptr->rgb_odr = 0;
		common_ptr->cct_odr = 0;
	}

	return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION      apds9960_dd_init_data_struct

DESCRIPTION   Initialize data structures for ALS and PRX

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e apds9960_dd_init_data_struct
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx
)
{
	sns_ddf_status_e init_status = SNS_DDF_SUCCESS;
	
	/* Initialize the common database */
	if ( (init_status = apds9960_dd_common_init(dd_ptr, sub_dev_idx)) != SNS_DDF_SUCCESS )
	{
		return init_status;
	}
	
	if ( (init_status = apds9960_dd_sub_dev_init(dd_ptr, sub_dev_idx)) != SNS_DDF_SUCCESS )
	{
		return init_status;
	}
	
	if (sub_dev_idx == als_prx_idx)
	{
		if ((dd_ptr->sub_dev[sub_dev_idx].sensors[0] == SNS_DDF_SENSOR_AMBIENT) ||
			(dd_ptr->sub_dev[sub_dev_idx].sensors[1] == SNS_DDF_SENSOR_AMBIENT))
		{
			if ( (init_status = apds9960_dd_als_init(dd_ptr)) != SNS_DDF_SUCCESS )
			{
				return init_status;
			}
		}
		
		if ((dd_ptr->sub_dev[sub_dev_idx].sensors[0] == SNS_DDF_SENSOR_PROXIMITY) ||
			(dd_ptr->sub_dev[sub_dev_idx].sensors[1] == SNS_DDF_SENSOR_PROXIMITY))
		{
			if ( (init_status = apds9960_dd_prx_init(dd_ptr)) != SNS_DDF_SUCCESS )
			{
				return init_status;
			}
		}
	}
	else if (sub_dev_idx == rgb_cct_idx)
	{
		if ((dd_ptr->sub_dev[sub_dev_idx].sensors[0] == SNS_DDF_SENSOR_RGB) ||
			(dd_ptr->sub_dev[sub_dev_idx].sensors[1] == SNS_DDF_SENSOR_RGB))
		{
			if ( (init_status = apds9960_dd_rgb_init(dd_ptr)) != SNS_DDF_SUCCESS )
			{
				return init_status;
			}
		}
		
		if ((dd_ptr->sub_dev[sub_dev_idx].sensors[0] == SNS_DDF_SENSOR_CT_C) ||
			(dd_ptr->sub_dev[sub_dev_idx].sensors[1] == SNS_DDF_SENSOR_CT_C))
		{
			if ( (init_status = apds9960_dd_cct_init(dd_ptr)) != SNS_DDF_SUCCESS )
			{
				return init_status;
			}
		}
	}
	
	return init_status;
}

/*===========================================================================

FUNCTION      apds9960_dd_set_powerstate

DESCRIPTION   Sets the power state of the ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  TRUE on success, FALSE otherwise

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e apds9960_dd_set_powerstate
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
sns_ddf_sensor_e sensor,
sns_ddf_powerstate_e op_mode,
bool init_data
)
{
	uint8_t i2c_data = APDS9960_DD_PWR_DOWN;
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	uint8_t update_register = 0;
	uint8_t i2c_read_data;
	uint8_t bytes_rw;

	if (EnableI2C != TRUE)
	{
		ALSPRX_MSG_3(ERROR, "set_powerstate error EnableI2C is false, op_mode=%d, sub_dev=%d, sensor=%d", op_mode, sub_dev_idx, sensor);
		return SNS_DDF_SUCCESS;
	}

	ALSPRX_MSG_3(HIGH, "set_powerstate op_mode=%d, sub_dev=%d, sensor=%d", op_mode, sub_dev_idx, sensor);

	if ( op_mode == SNS_DDF_POWERSTATE_ACTIVE )
	{
		if (dd_ptr->sub_dev[sub_dev_idx].pwr_mode == SNS_DDF_POWERSTATE_ACTIVE)
		{
			// don't have to do this if other sensor type already did it
			return status;
		}

		status = sns_ddf_read_port(dd_ptr->port_handle, 
			APDS9960_DD_ENABLE_ADDR,
			(uint8_t*)&i2c_read_data, 1, &bytes_rw);

		CHECK_STATUS(status);

		if ( (i2c_read_data&APDS9960_DD_PWR_ON) != APDS9960_DD_PWR_ON )
		{
			// if another sub-device is already enabled, don't disturb the enable register
			/* ENABLE=PON */
			/* standy-by mode */
			i2c_data = APDS9960_DD_PWR_ON;
			status = apds9960_dd_set_enable(dd_ptr, i2c_data);
			CHECK_STATUS(status);
		}
		
		dd_ptr->sub_dev[sub_dev_idx].pwr_mode = op_mode;
		dd_ptr->sub_dev[sub_dev_idx].state = APDS9960_DD_DEV_CONFIGURED;
	}
	else /* when op_mode is SNS_DDF_POWERSTATE_LOWPOWER */
	{
		if (sub_dev_idx == als_prx_idx)
		{
			// SNS_DDF_SENSOR_AMBIENT or SNS_DDF_SENSOR_PROXIMITY or SNS_DDF_SENSOR__ALL (AMBIENT and PROXIMITY)
			if (sensor == SNS_DDF_SENSOR__ALL)
			{
				if (!dd_ptr->rgb.enable && !dd_ptr->cct.enable)
				{
					// we can power it down as no other sensor type is using it
					ALSPRX_MSG_0(MED, "power down als-prx");
					i2c_data = APDS9960_DD_PWR_DOWN;
					update_register = 1;
				}

				if  (!dd_ptr->sub_dev[sub_dev_idx].dri_enabled)
				{
					apds9960_dd_stop_general_timer( dd_ptr, sub_dev_idx, SNS_DDF_SENSOR_AMBIENT ); /* stop any pending timers */
					apds9960_dd_stop_general_timer( dd_ptr, sub_dev_idx, SNS_DDF_SENSOR_PROXIMITY ); /* stop any pending timers */
				}
				
				if (init_data == TRUE)
				{
					/* cleanup */
					status = apds9960_dd_init_data_struct(dd_ptr, sub_dev_idx);
					CHECK_STATUS(status);
				}

				/* kk 11-Jun-2015 */
				dd_ptr->als.enable = FALSE;
				dd_ptr->prx.enable = FALSE;
				
				dd_ptr->sub_dev[sub_dev_idx].pwr_mode = op_mode;
				dd_ptr->sub_dev[sub_dev_idx].state = APDS9960_DD_DEV_CONFIGURED;
			}
			else if (sensor == SNS_DDF_SENSOR_AMBIENT)
			{
				if (!dd_ptr->prx.enable && !dd_ptr->rgb.enable && !dd_ptr->cct.enable)
				{
					// we can power it down as no other sensor type is using it
					ALSPRX_MSG_0(MED, "power down als");
					i2c_data = APDS9960_DD_PWR_DOWN;
					update_register = 1;
				}

				/* kk 11-Jun-2015 */
				dd_ptr->als.enable = FALSE;

				if (!dd_ptr->prx.enable)
				{
					dd_ptr->sub_dev[sub_dev_idx].pwr_mode = op_mode;
					dd_ptr->sub_dev[sub_dev_idx].state = APDS9960_DD_DEV_CONFIGURED;
				}
			}
			else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
			{
				if (!dd_ptr->als.enable && !dd_ptr->rgb.enable && !dd_ptr->cct.enable)
				{
					// we can power it down as no other sensor type is using it
					ALSPRX_MSG_0(MED, "power down prx");
					i2c_data = APDS9960_DD_PWR_DOWN;
					update_register = 1;
				}

				// kk 11-Jun-2015
				dd_ptr->prx.enable = FALSE;

				if (!dd_ptr->als.enable)
				{
					dd_ptr->sub_dev[sub_dev_idx].pwr_mode = op_mode;
					dd_ptr->sub_dev[sub_dev_idx].state = APDS9960_DD_DEV_CONFIGURED;
				}
			}
			else
			{
				ALSPRX_MSG_1(ERROR, "unknown sensor=%d", sensor);
				return SNS_DDF_EINVALID_PARAM;
			}
		}
		else if (sub_dev_idx == rgb_cct_idx)
		{
			// SNS_DDF_SENSOR_RGB or SNS_DDF_SENSOR_CCT or SNS_DDF_SENSOR__ALL
			if (sensor == SNS_DDF_SENSOR__ALL)
			{
				if (!dd_ptr->prx.enable && !dd_ptr->als.enable) // cannot power down if other sensor type is active
				{
					ALSPRX_MSG_0(MED, "power down rgb_cct");
					i2c_data = APDS9960_DD_PWR_DOWN;
					update_register = 1;
				}

				if  (!dd_ptr->sub_dev[sub_dev_idx].dri_enabled)
				{
					apds9960_dd_stop_general_timer( dd_ptr, sub_dev_idx, SNS_DDF_SENSOR_RGB ); /* stop any pending timers */
					apds9960_dd_stop_general_timer( dd_ptr, sub_dev_idx, SNS_DDF_SENSOR_CT_C ); /* stop any pending timers */
				}
				
				if (init_data == TRUE)
				{
					/* cleanup */
					status = apds9960_dd_init_data_struct(dd_ptr, sub_dev_idx);
					CHECK_STATUS(status);
				}
		
				// kk 11-Jun-2015
				dd_ptr->rgb.enable = FALSE;
				dd_ptr->cct.enable = FALSE;
				
				dd_ptr->sub_dev[sub_dev_idx].pwr_mode = op_mode;
				dd_ptr->sub_dev[sub_dev_idx].state = APDS9960_DD_DEV_CONFIGURED;
			}
			else if (sensor == SNS_DDF_SENSOR_RGB)
			{
				if (!dd_ptr->cct.enable && !dd_ptr->als.enable && !dd_ptr->prx.enable)
				{
					// we can power it down as no other sensor type is using it
					ALSPRX_MSG_0(MED, "power down rgb");
					i2c_data = APDS9960_DD_PWR_DOWN;
					update_register = 1;
				}

				// kk 11-Jun-2015
				dd_ptr->rgb.enable = FALSE;

				if (!dd_ptr->cct.enable)
				{
					dd_ptr->sub_dev[sub_dev_idx].pwr_mode = op_mode;
					dd_ptr->sub_dev[sub_dev_idx].state = APDS9960_DD_DEV_CONFIGURED;
				}
			}
			else if (sensor == SNS_DDF_SENSOR_CT_C)
			{
				if (!dd_ptr->rgb.enable && !dd_ptr->als.enable && !dd_ptr->prx.enable)
				{
					// we can power it down as no other sensor type is using it
					ALSPRX_MSG_0(MED, "power down cct");
					i2c_data = APDS9960_DD_PWR_DOWN;
					update_register = 1;
				}

				// kk 11-Jun-2015
				dd_ptr->cct.enable = FALSE;

				if (!dd_ptr->rgb.enable)
				{
					dd_ptr->sub_dev[sub_dev_idx].pwr_mode = op_mode;
					dd_ptr->sub_dev[sub_dev_idx].state = APDS9960_DD_DEV_CONFIGURED;
				}
			}
			else
			{
				ALSPRX_MSG_1(ERROR, "unknown sensor=%d", sensor);
				return SNS_DDF_EINVALID_PARAM;
			}
		}
		
		if (update_register)
		{
			ALSPRX_MSG_1(MED, "set_powerstate - enable=0x%x", i2c_data);
			status = apds9960_dd_set_enable(dd_ptr, i2c_data);
			CHECK_STATUS(status);
		}
	}
	
	return status;
}

/*===========================================================================

FUNCTION      apds9960_dd_common_request_data

DESCRIPTION   As a async driver, this driver needs preparing a sensor data.
This function calls an appropriate preparing function according to
the data type.

DEPENDENCIES  None

RETURN VALUE  FALSE if successful, TRUE otherwise

SIDE EFFECT   None
===========================================================================*/
bool apds9960_dd_common_request_data
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
sns_ddf_sensor_e sensor
)
{
	uint8_t i2c_data = 0x0;
	uint8_t enable_reg;
	uint8_t bytes_rw;
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	
	ALSPRX_MSG_2(MED, "common_request_data - sub_dev=%d, sensor=%d", sub_dev_idx, sensor);
	
	/* make sure device is in the right state */
	if ( dd_ptr->sub_dev[sub_dev_idx].state != APDS9960_DD_DEV_CONFIGURED ||
		dd_ptr->sub_dev[sub_dev_idx].pwr_mode != SNS_DDF_POWERSTATE_ACTIVE )
	{
		/* device state is stopped OR invalid */
		/* make sure device is in the right state */
		ALSPRX_MSG_0(ERROR, "device not configured yet");
		apds9960_dd_set_powerstate(dd_ptr, 0, SNS_DDF_SENSOR__ALL,
								   SNS_DDF_POWERSTATE_ACTIVE, FALSE);
	}
	
	status = sns_ddf_read_port(dd_ptr->port_handle,
							   APDS9960_DD_ENABLE_ADDR,
							   &i2c_data, 1, &bytes_rw);
	
	if (status != SNS_DDF_SUCCESS) return TRUE;
	
	enable_reg = i2c_data;
	
	ALSPRX_MSG_1(MED, "before: enable_reg=0x%x", enable_reg);

	/* Request data from device */
	if (sensor == SNS_DDF_SENSOR_AMBIENT)
	{
		dd_ptr->als.polling_data = TRUE;
		
		if (dd_ptr->als.enable == FALSE)
		{
			status = apds9960_dd_update_intg_time(dd_ptr, dd_ptr->prx.enable, TRUE, FALSE);
			if (status != SNS_DDF_SUCCESS) return TRUE;

			/* ENABLE */
			i2c_data = enable_reg | (APDS9960_DD_PWR_ON|APDS9960_DD_ALS_ENABLE);
			
			dd_ptr->als.enable = TRUE;
			
			ALSPRX_MSG_1(MED, "after: enable_reg=0x%x", i2c_data);

			apds9960_dd_set_enable(dd_ptr, i2c_data);

			apds9960_dd_req_data(dd_ptr, sub_dev_idx, sensor);
		}
		else
		{
			apds9960_dd_start_general_timer (dd_ptr, sub_dev_idx, sensor, 5000); // data should be available
		}		
	}
	else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
	{
		dd_ptr->prx.polling_data = TRUE;
		
		if (dd_ptr->prx.enable == FALSE)
		{
			status = apds9960_dd_update_intg_time(dd_ptr, TRUE, (dd_ptr->als.enable|dd_ptr->rgb.enable|dd_ptr->cct.enable), FALSE);
			if (status != SNS_DDF_SUCCESS) return TRUE;

			/* ENABLE */
			i2c_data = enable_reg|(APDS9960_DD_PWR_ON|APDS9960_DD_PRX_ENABLE|APDS9960_DD_WAIT_ENABLE);

			dd_ptr->prx.enable = TRUE;
			
			ALSPRX_MSG_1(MED, "after: enable_reg=0x%x", i2c_data);

			apds9960_dd_set_enable(dd_ptr, i2c_data);

			apds9960_dd_req_data(dd_ptr, sub_dev_idx, SNS_DDF_SENSOR_PROXIMITY);
		}
		else
		{
			apds9960_dd_start_general_timer (dd_ptr, sub_dev_idx, sensor, 5000); // data should be available
		}
	}
	else if (sensor == SNS_DDF_SENSOR_RGB)
	{
		dd_ptr->rgb.polling_data = TRUE;

		if (dd_ptr->rgb.enable == FALSE)
		{
		
			status = apds9960_dd_update_intg_time(dd_ptr, dd_ptr->prx.enable, TRUE, FALSE);
			if (status != SNS_DDF_SUCCESS) return TRUE;

			/* ENABLE */
			if ((dd_ptr->als.enable == FALSE) && (dd_ptr->cct.enable == FALSE))
			{
				i2c_data = enable_reg | (APDS9960_DD_PWR_ON|APDS9960_DD_ALS_ENABLE);	

				dd_ptr->rgb.enable = TRUE;
				
				ALSPRX_MSG_1(MED, "after: enable_reg=0x%x", i2c_data);
				apds9960_dd_set_enable(dd_ptr, i2c_data);
			}
		
			apds9960_dd_req_data(dd_ptr, sub_dev_idx, sensor);
		}
		else
		{
			apds9960_dd_start_general_timer (dd_ptr, sub_dev_idx, sensor, 5000); // data should be available
		}
	}
	else if (sensor == SNS_DDF_SENSOR_CT_C)
	{
		dd_ptr->cct.polling_data = TRUE;
		
		if (dd_ptr->cct.enable == FALSE)
		{			
			status = apds9960_dd_update_intg_time(dd_ptr, dd_ptr->prx.enable, TRUE, FALSE);
			if (status != SNS_DDF_SUCCESS) return TRUE;

			/* ENABLE */
			if ((dd_ptr->als.enable == FALSE) && (dd_ptr->rgb.enable == FALSE))
			{		
				i2c_data = enable_reg | (APDS9960_DD_PWR_ON|APDS9960_DD_ALS_ENABLE);	

				dd_ptr->cct.enable = TRUE;
				
				ALSPRX_MSG_1(MED, "after: enable_reg=0x%x", i2c_data);
				apds9960_dd_set_enable(dd_ptr, i2c_data);
			}
			
			apds9960_dd_req_data(dd_ptr, sub_dev_idx, sensor);
		}
		else
		{
			apds9960_dd_start_general_timer (dd_ptr, sub_dev_idx, sensor, 5000); // data should be available
		}
	}

	return FALSE;
}

/*===========================================================================

FUNCTION:   sns_dd_apds9960_get_data

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
sns_ddf_status_e sns_dd_apds9960_get_data
(
sns_ddf_handle_t        dd_handle,
sns_ddf_sensor_e        sensors[],
uint32_t                num_sensors,
sns_ddf_memhandler_s*   memhandler,
sns_ddf_sensor_data_s*  data[] /* ignored by this async driver */
)
{
	sns_ddf_status_e status = SNS_DDF_PENDING;
	uint8_t i;
	apds9960_dd_dev_s* dd_ptr;
	uint32_t sub_dev_idx;
	sns_ddf_time_t         timestamp;
	if ( (dd_handle == NULL) || (sensors == NULL) ||
		(num_sensors < 1) || (num_sensors > 2) )
	{
		return SNS_DDF_EINVALID_PARAM;
	}

    //UMSG(MSG_SSID_SNS,DBG_HIGH_PRIO, "APDS9960");
	
	sub_dev_idx = (uint32_t)dd_handle & (APDS9960_DD_HANDLE_ALIGN-1);
	dd_ptr = (apds9960_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9960_DD_HANDLE_ALIGN-1)));
	
	ALSPRX_MSG_3(MED, "get_data - sub_dev=%d, sensors[0]=%d, sensors[1]=%d", sub_dev_idx, sensors[0], sensors[1]);
	ALSPRX_MSG_1(MED, "EnableI2C = %d", EnableI2C);
	if (EnableI2C == FALSE) 
	//if (1) 
	{
		static uint8_t toggle[2] = {0};
		uint8_t num_samples=2;
		sns_ddf_sensor_data_s *data_ptr;
		sns_ddf_sensor_sample_s *sample_ptr;
		ALSPRX_MSG_1(MED, "1 EnableI2C = %d", EnableI2C);
		*data = sns_ddf_memhandler_malloc_ex(memhandler, num_sensors * sizeof(sns_ddf_sensor_data_s), dd_ptr->sub_dev[sub_dev_idx].smgr_handle);
		
		if(*data == NULL)
		{
			ALSPRX_MSG_0(MED, "malloc error");
			return SNS_DDF_ENOMEM;
		}

		timestamp = sns_ddf_get_timestamp();

		for (i=0; i < num_sensors; i++)
		{
			data_ptr = &((*data)[i]);
			
			if( (sample_ptr = sns_ddf_memhandler_malloc_ex(memhandler, (ALSPRX_MAX_NUM_AXES * sizeof(sns_ddf_sensor_sample_s)), dd_ptr->sub_dev[sub_dev_idx].smgr_handle)) == NULL)
			{
				ALSPRX_MSG_0(MED, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			
			sns_ddf_delay(50);

			if (sensors[i] == SNS_DDF_SENSOR_PROXIMITY)
			{
				data_ptr->sensor = SNS_DDF_SENSOR_PROXIMITY;
				if (toggle[i])
				{
					num_samples = 2;
					sample_ptr[0].sample = FX_CONV_Q16(0, 0); //far = 0; near = 1
					sample_ptr[0].status = SNS_DDF_SUCCESS;
					sample_ptr[1].sample = 100; //ADC
					sample_ptr[1].status = SNS_DDF_SUCCESS;

					sns_dd_apds9960_log(dd_ptr, sensors[i], 0, sample_ptr[0].sample, 0, 0, sample_ptr[1].sample, timestamp);
				}
				else
				{
					num_samples = 2;
					sample_ptr[0].sample = FX_CONV_Q16(1, 0); //far = 0; near = 1
					sample_ptr[0].status = SNS_DDF_SUCCESS;
					sample_ptr[1].sample = 500; //ADC
					sample_ptr[1].status = SNS_DDF_SUCCESS;

					sns_dd_apds9960_log(dd_ptr, sensors[i], 1, sample_ptr[0].sample, 0, 0, sample_ptr[1].sample, timestamp);
				}
			}
			else if (sensors[i] == SNS_DDF_SENSOR_AMBIENT)
			{
				data_ptr->sensor = SNS_DDF_SENSOR_AMBIENT;
				if (toggle[i])
				{
					num_samples = 2;
					sample_ptr[0].sample = FX_CONV_Q16(20, 0); //20 lux
					sample_ptr[0].status = SNS_DDF_SUCCESS;
					sample_ptr[1].sample = 60; //ADC
					sample_ptr[1].status = SNS_DDF_SUCCESS;

					sns_dd_apds9960_log(dd_ptr, sensors[i], 20, sample_ptr[0].sample, sample_ptr[1].sample, 0, 0, timestamp);
				}
				else
				{
					num_samples = 2;
					sample_ptr[0].sample = FX_CONV_Q16(100, 0); //100 lux
					sample_ptr[0].status = SNS_DDF_SUCCESS;
					sample_ptr[1].sample = 300; //ADC
					sample_ptr[1].status = SNS_DDF_SUCCESS;

					sns_dd_apds9960_log(dd_ptr, sensors[i], 100, sample_ptr[0].sample, sample_ptr[1].sample, 0, 0, timestamp);
				}

			}
			else if (sensors[i] == SNS_DDF_SENSOR_RGB) {
				data_ptr->sensor = SNS_DDF_SENSOR_RGB;
				if (toggle[i])
				{
					num_samples = 3;
					sample_ptr[0].sample = 100; //FX_CONV_Q16(65535, 0); //R
					sample_ptr[0].status = SNS_DDF_SUCCESS;
					sample_ptr[1].sample = 100; //FX_CONV_Q16(65535, 0); //G
					sample_ptr[1].status = SNS_DDF_SUCCESS;
					sample_ptr[2].sample = 100; //FX_CONV_Q16(65535, 0); //B
					sample_ptr[2].status = SNS_DDF_SUCCESS;

					sns_dd_apds9960_log(dd_ptr, sensors[i], FX_CONV_Q16(100, 0), FX_CONV_Q16(100, 0), FX_CONV_Q16(100, 0), 0, 0, timestamp);
				}
				else
				{
					num_samples = 3;
					sample_ptr[0].sample = 200; //FX_CONV_Q16(-1, 0); //R
					sample_ptr[0].status = SNS_DDF_SUCCESS;
					sample_ptr[1].sample = 200; //FX_CONV_Q16(-1, 0); //G
					sample_ptr[1].status = SNS_DDF_SUCCESS;
					sample_ptr[2].sample = 200; //FX_CONV_Q16(-1, 0); //B
					sample_ptr[2].status = SNS_DDF_SUCCESS;

					sns_dd_apds9960_log(dd_ptr, sensors[i], FX_CONV_Q16(200, 0), FX_CONV_Q16(200, 0), FX_CONV_Q16(200, 0), 0, 0, timestamp);
				}
			}
			else {
				data_ptr->sensor = SNS_DDF_SENSOR_CT_C;
				if (toggle[i])
				{
					num_samples = 2;
					sample_ptr[0].sample = FX_CONV_Q16(2600, 0); //2600K
					sample_ptr[0].status = SNS_DDF_SUCCESS;
					sample_ptr[1].sample = 100; //FX_CONV_Q16(1, 0); //C
					sample_ptr[1].status = SNS_DDF_SUCCESS;

					sns_dd_apds9960_log(dd_ptr, sensors[i], sample_ptr[0].sample, FX_CONV_Q16(100, 0), 0, 0, 0, timestamp);
				}
				else
				{
					num_samples = 2;
					sample_ptr[0].sample = FX_CONV_Q16(6000, 0); //2600K
					sample_ptr[0].status = SNS_DDF_SUCCESS;
					sample_ptr[1].sample = 200; //FX_CONV_Q16(-1, 0); //C
					sample_ptr[1].status = SNS_DDF_SUCCESS;

					sns_dd_apds9960_log(dd_ptr, sensors[i], sample_ptr[0].sample, FX_CONV_Q16(200, 0), 0, 0, 0, timestamp);
				}
			}
			
			data_ptr->status = SNS_DDF_SUCCESS;
			data_ptr->num_samples = num_samples;
			data_ptr->timestamp = timestamp;
			data_ptr->samples = sample_ptr;
			toggle[i] = ~toggle[i];
		}
		
		return SNS_DDF_SUCCESS;
	}
	ALSPRX_MSG_1(MED, "2 EnableI2C = %d", EnableI2C);
	/* Look at all the requests */
	for (i=0; i < num_sensors; i++)
	{
		if (sensors[i] != SNS_DDF_SENSOR_AMBIENT &&
			sensors[i] != SNS_DDF_SENSOR_PROXIMITY &&
			sensors[i] != SNS_DDF_SENSOR_RGB &&
			sensors[i] != SNS_DDF_SENSOR_CT_C)
		{
			status = SNS_DDF_EINVALID_PARAM;
			break;
		}
				
		if ( apds9960_dd_common_request_data(dd_ptr, sub_dev_idx, sensors[i]) == TRUE ) {
			status = SNS_DDF_EFAIL;
			break;
		}
	}
	
	return status;
}

sns_ddf_driver_if_s APDS9960_DD_IF =
{
	&sns_dd_apds9960_init,
	&sns_dd_apds9960_get_data,
	&sns_dd_apds9960_set_attrib,
	&sns_dd_apds9960_get_attrib,
	&sns_dd_apds9960_handle_timer,
	&sns_dd_apds9960_handle_irq,
	&sns_dd_apds9960_reset,
	&sns_dd_apds9960_calibration,
	&sns_dd_apds9960_enable_sched_data,
	&sns_dd_apds9960_probe
};
