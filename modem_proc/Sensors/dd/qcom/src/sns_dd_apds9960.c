/*==============================================================================

    SENSORS RGB, CCT, AMBIENT LIGHT AND PROXIMITY  COMBO DRIVER

DESCRIPTION

   Implements the driver for Avago RGB, CCT, ALS and Proximity Sensor APDS-9960

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

  1.4     22Jun2015   KK   Revise RGB/CCT change gain mechanism in apds9960_dd_als_raw_data_ready()
  1.5     09Jul2015   AH   Fix compiler warnings
  1.6     17Jul2015   KK   Fix for OEM test
*
**************************************************************************/

/*============================================================================
INCLUDE FILES
============================================================================*/
#include "sns_ddf_attrib.h"
#include "sns_ddf_common.h"
#include "sns_ddf_comm.h"
#include "sns_memmgr.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_dd_apds9960_priv.h"

/*============================================================================
STATIC VARIABLE DEFINITIONS
============================================================================*/

extern volatile boolean EnableI2C;

extern uint8_t sns_dd_als_atime_tb[3]; /* 27.8ms,  50.04ms , 100.08ms */

extern uint32_t sns_dd_als_atime_us_tb[3]; /* 27.8ms,  50.04ms , 100.08ms */
extern uint16_t sns_dd_als_res_tb[3]; /* 10240, 21504, 43008 */
extern uint8_t sns_dd_als_adc_bit_tb[3];

extern uint8_t sns_dd_als_again_tb[4];
extern uint8_t sns_dd_als_again_bit_tb[4];

extern sns_ddf_odr_t sns_dd_apds9960_odr_tb[3]; // Hz

extern double RGB_COE_X[3];
extern double RGB_COE_Y[3];
extern double RGB_COE_Z[3];

extern double RGB_CIE_N1;
extern double RGB_CIE_N2;

extern double RGB_CIE_CCT1;
extern double RGB_CIE_CCT2;
extern double RGB_CIE_CCT3;
extern double RGB_CIE_CCT4;

extern sns_ddf_sensor_e prx_timer;
extern sns_ddf_sensor_e als_timer;
extern sns_ddf_sensor_e rgb_timer;
extern sns_ddf_sensor_e cct_timer;

extern uint32_t first_init;
extern bool first_probe;
extern int32_t als_prx_idx;
extern int32_t rgb_cct_idx;

extern apds9960_dd_head_s apds9960_dd_head;

/*============================================================================
STATIC FUNCTION PROTOTYPES
============================================================================*/

sns_ddf_status_e apds9960_dd_reset_registers
(
apds9960_dd_dev_s* dd_ptr
);

extern sns_ddf_status_e apds9960_dd_set_powerstate
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
sns_ddf_sensor_e sensor,
sns_ddf_powerstate_e op_mode,
bool init_data
);

extern void apds9960_dd_start_general_timer
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
uint32_t sensor,
uint32_t duration
);

extern void apds9960_dd_stop_general_timer
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
uint32_t sensor
);

extern sns_ddf_status_e apds9960_dd_set_enable
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  enable
);

extern sns_ddf_status_e apds9960_dd_set_atime
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  atime
);

extern sns_ddf_status_e apds9960_dd_set_wtime
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  wtime
);

extern sns_ddf_status_e apds9960_dd_set_ailt
(
apds9960_dd_dev_s* dd_ptr,
uint16_t  threshold
);

extern sns_ddf_status_e apds9960_dd_set_aiht
(
apds9960_dd_dev_s* dd_ptr,
uint16_t  threshold
);

extern sns_ddf_status_e apds9960_dd_set_pilt
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  threshold
);

extern sns_ddf_status_e apds9960_dd_set_piht
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  threshold
);

extern sns_ddf_status_e apds9960_dd_set_pers
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  pers
);

extern sns_ddf_status_e apds9960_dd_set_config
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  config
);

extern sns_ddf_status_e apds9960_dd_set_ppulse
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  ppulse
);

extern sns_ddf_status_e apds9960_dd_set_control
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  control
);

extern sns_ddf_status_e apds9960_dd_set_poffset_ur
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  poffset_ur
);

extern sns_ddf_status_e apds9960_dd_set_poffset_dl
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  poffset_dl
);

extern sns_ddf_status_e apds9960_dd_set_config2
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  config2
);

extern sns_ddf_status_e apds9960_dd_set_prx_thresh
(
apds9960_dd_dev_s* dd_ptr,
uint8_t  pilt,
uint8_t  piht
);

extern sns_ddf_status_e apds9960_dd_set_als_thresh
(
apds9960_dd_dev_s* dd_ptr,
uint16_t  ailt,
uint16_t  aiht
);

extern sns_ddf_status_e apds9960_dd_update_intg_time
(
apds9960_dd_dev_s* dd_ptr,
bool prx_enabled,
bool als_enabled,
bool static_setting
);

extern sns_ddf_status_e apds9960_dd_clear_int_reg
(
apds9960_dd_dev_s* dd_ptr,
uint8_t reg_addr
);

extern sns_ddf_status_e apds9960_dd_req_data
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
uint32_t sensor
);

extern sns_ddf_status_e apds9960_dd_rgb_init
(
apds9960_dd_dev_s* dd_ptr
);

extern sns_ddf_status_e apds9960_dd_cct_init
(
apds9960_dd_dev_s* dd_ptr
);

extern sns_ddf_status_e apds9960_dd_als_init
(
apds9960_dd_dev_s* dd_ptr
);

extern sns_ddf_status_e apds9960_dd_prx_init
(
apds9960_dd_dev_s* dd_ptr
);

extern sns_ddf_status_e apds9960_dd_sub_dev_init
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx
);

extern sns_ddf_status_e apds9960_dd_common_init
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx
);

extern sns_ddf_status_e apds9960_dd_init_data_struct
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx
);

/*===========================================================================

FUNCTION      apds9960_dd_find_same_port_dev

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
apds9960_dd_dev_s* apds9960_dd_find_same_port_dev
(
sns_ddf_port_config_s* port_config
)
{
	if (port_config == NULL) return NULL;
	if (port_config->bus_config.i2c == NULL) return NULL;
	
	if (port_config->bus_config.i2c->slave_addr == 0x39)
	{
		if (first_init == 0 && apds9960_dd_head.next == NULL)
		{
			first_init++;
			return apds9960_dd_head.next;
		}
		else if (first_init > 0)
		{
			if (apds9960_dd_head.next != NULL)
			{
				first_init++;
				return apds9960_dd_head.next;
			}
			else
			{
				first_init = 0;
				return apds9960_dd_head.next;
			}
		}
	}
	
	return NULL;
}

/*===========================================================================

FUNCTION      apds9960_dd_fill_sub_dev_slot

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
uint32_t apds9960_dd_fill_sub_dev_slot
(
apds9960_dd_sub_dev_s* sub_dev,
uint32_t num_sensors,
sns_ddf_sensor_e* sensors
)
{
	uint32_t i;
	
	if (sub_dev == NULL) return 0;
	if (num_sensors > APDS9960_DD_NUM_SENSOR_TYPES) num_sensors = APDS9960_DD_NUM_SENSOR_TYPES;
	
	sub_dev->num_sensors = num_sensors;
	for (i=0; i<num_sensors;i++)
	{
		sub_dev->sensors[i] = *(sensors+i);
	}
	
	return num_sensors;
}

/*===========================================================================

FUNCTION      apds9960_dd_find_free_sub_dev_slot

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
uint32_t apds9960_dd_find_free_sub_dev_slot
(
apds9960_dd_dev_s* dd_ptr
)
{
	if (dd_ptr == NULL) return -1;
	if (dd_ptr->sub_dev_cnt >= APDS9960_DD_MAX_SUB_DEV) return -1;
	
	return dd_ptr->sub_dev_cnt;
}

/*===========================================================================

FUNCTION      apds9960_dd_timer_init

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e apds9960_dd_timer_init
(
apds9960_dd_dev_s* apds9960_dd_dev_ptr,
uint32_t sub_dev_idx,
sns_ddf_sensor_e* sensors
)
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;

	ALSPRX_MSG_3(MED, "timer_init - sub_dev=%d, sensor[0]=%d, sensor[1]=%d", sub_dev_idx, *sensors, *(sensors+1));
	
	if ((*sensors == SNS_DDF_SENSOR_AMBIENT) || (*(sensors+1) == SNS_DDF_SENSOR_AMBIENT))
	{
		/* timer for AMBIENT*/
		status = sns_ddf_timer_init(&apds9960_dd_dev_ptr->als.timer,
									(sns_ddf_handle_t)((uint32_t)apds9960_dd_dev_ptr | sub_dev_idx),
									&APDS9960_DD_IF,
									(void *)&als_timer,
									FALSE);
		
		if ( status != SNS_DDF_SUCCESS )
		{
			return status;
		}

		als_prx_idx = sub_dev_idx;
	}
	
	if ((*sensors == SNS_DDF_SENSOR_PROXIMITY) || (*(sensors+1) == SNS_DDF_SENSOR_PROXIMITY))
	{
		
		/* timer for PROXIMITY */
		status = sns_ddf_timer_init(&apds9960_dd_dev_ptr->prx.timer,
									(sns_ddf_handle_t)((uint32_t)apds9960_dd_dev_ptr | sub_dev_idx),
									&APDS9960_DD_IF,
									(void *)&prx_timer,
									FALSE);
		
		if ( status != SNS_DDF_SUCCESS )
		{
			return status;
		}
		
		als_prx_idx = sub_dev_idx;
	}
	
	if ((*sensors == SNS_DDF_SENSOR_RGB) || (*(sensors+1) == SNS_DDF_SENSOR_RGB))
	{
		/* timer for RGB */
		status = sns_ddf_timer_init(&apds9960_dd_dev_ptr->rgb.timer,
									(sns_ddf_handle_t)((uint32_t)apds9960_dd_dev_ptr | sub_dev_idx),
									&APDS9960_DD_IF,
									(void *)&rgb_timer,
									FALSE);
		
		if ( status != SNS_DDF_SUCCESS )
		{
			return status;
		}

		rgb_cct_idx = sub_dev_idx;
	}
	
	if ((*sensors == SNS_DDF_SENSOR_CT_C) || (*(sensors+1) == SNS_DDF_SENSOR_CT_C))
	{
		
		/* timer for CT_C */
		status = sns_ddf_timer_init(&apds9960_dd_dev_ptr->cct.timer,
									(sns_ddf_handle_t)((uint32_t)apds9960_dd_dev_ptr | sub_dev_idx),
									&APDS9960_DD_IF,
									(void *)&cct_timer,
									FALSE);
		
		if ( status != SNS_DDF_SUCCESS )
		{
			return status;
		}
		
		rgb_cct_idx = sub_dev_idx;
	}
	
	return status;
}

/*===========================================================================

FUNCTION      apds9960_dd_set_cycle_time

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
void apds9960_dd_set_cycle_time
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
uint32_t sensor,
uint32_t odr
)
{
	uint32_t max_odr;

	max_odr = MAX(dd_ptr->common.als_odr,
				dd_ptr->common.prx_odr);

	max_odr = MAX(max_odr,
				dd_ptr->common.rgb_odr);

	max_odr = MAX(max_odr,
				dd_ptr->common.cct_odr);

	ALSPRX_MSG_3(MED, "sensor=%d, max odr=%d, req odr=%d", sensor, max_odr, odr);

	max_odr = MAX(max_odr, odr);

	if (max_odr >= 11) 
	{
		/* resulting freq = 20Hz */
		dd_ptr->common.cycle_time = 50000;
	}
	else if (max_odr >= 6) 
	{
		/* resulting freq = 10Hz */
		dd_ptr->common.cycle_time = 100000;
	}
	else if (max_odr > 0) 
	{
		/* resulting freq = 5Hz */
		dd_ptr->common.cycle_time = 200000;
	}
	else 
	{
		dd_ptr->common.cycle_time = 0;
	}

	if (sensor == SNS_DDF_SENSOR_AMBIENT)
	{
		dd_ptr->common.als_odr = odr; /* Hz */
	}
	else if (sensor == SNS_DDF_SENSOR_PROXIMITY)
	{
		dd_ptr->common.prx_odr = odr; /* Hz */
	}
	else if (sensor == SNS_DDF_SENSOR_RGB)
	{
		dd_ptr->common.rgb_odr = odr; /* Hz */
	}
	else if (sensor == SNS_DDF_SENSOR_CT_C)
	{
		dd_ptr->common.cct_odr = odr; /* Hz */
	}
}

/*===========================================================================

FUNCTION      apds9960_dd_enable_interrupt

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e apds9960_dd_enable_interrupt
(
sns_ddf_handle_t  dd_handle,
sns_ddf_sensor_e  sensor
)
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	uint8_t i2c_data = 0x0;
	uint8_t bytes_rw;
	uint8_t i2c_pon_data = APDS9960_DD_PWR_ON;
	apds9960_dd_dev_s* dd_ptr;
	uint32_t sub_dev_idx;
	uint8_t enable_reg;
	
	sub_dev_idx = (uint32_t)dd_handle & (APDS9960_DD_HANDLE_ALIGN-1);
	dd_ptr = (apds9960_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9960_DD_HANDLE_ALIGN-1)));
		
	ALSPRX_MSG_3(MED, "enable_interrupt - sub device=%d, als_prx_idx=%d rgb_cct_idx=%d", 
		sub_dev_idx, als_prx_idx, rgb_cct_idx);
	
	ALSPRX_MSG_3(MED, "sensor=%d prx_en=%d als_en=%d",		
		sensor, dd_ptr->prx.enable, dd_ptr->als.enable);
	
	status = sns_ddf_read_port(dd_ptr->port_handle,					   
		APDS9960_DD_ENABLE_ADDR,
		&i2c_data, 1, &bytes_rw);
	
	CHECK_STATUS(status);
	
	enable_reg = i2c_data;
	
	ALSPRX_MSG_1(MED, "before enable_reg=0x%X", enable_reg);

	if ((sensor != SNS_DDF_SENSOR_PROXIMITY) && (sensor != SNS_DDF_SENSOR_AMBIENT) &&
		(sensor != SNS_DDF_SENSOR_RGB) && (sensor != SNS_DDF_SENSOR_CT_C))
	{
		return SNS_DDF_EINVALID_PARAM;
	}
	
	if (SNS_DDF_SENSOR_PROXIMITY == sensor)
	{
		if (dd_ptr->prx.enable == TRUE)
		{
			ALSPRX_MSG_0(ERROR, "enable_interrupt End - proximity already enabled");
			return SNS_DDF_SUCCESS;
		}

		/* ENABLE=PON */
		status = apds9960_dd_set_enable(dd_ptr, i2c_pon_data);
		CHECK_STATUS(status);		

		/* Clear interrupts */
		status = apds9960_dd_clear_int_reg(dd_ptr, APDS9960_DD_CMD_CLR_ALL_INT);
		CHECK_STATUS(status);

		// kk 19-May-2015
		// status = apds9960_dd_set_prx_thresh(dd_ptr, 0xFF, 0);  // force an interrupt
		status = apds9960_dd_set_pers(dd_ptr, APDS9960_DD_APERS_2); // force an interrupt, every prox adc cycle
		CHECK_STATUS(status);

		dd_ptr->prx.first_int_enable = TRUE;

		if (!dd_ptr->als.enable &&
			!dd_ptr->rgb.enable &&
			!dd_ptr->cct.enable)
		{
			status = apds9960_dd_update_intg_time(dd_ptr, TRUE, (dd_ptr->als.enable|dd_ptr->rgb.enable|dd_ptr->cct.enable), FALSE);
			CHECK_STATUS(status);

			i2c_data = (APDS9960_DD_PWR_ON|APDS9960_DD_PRX_ENABLE|
						 APDS9960_DD_PRX_INT_ENABLE | APDS9960_DD_WAIT_ENABLE);
		}
		else
		{
			i2c_data = enable_reg|(APDS9960_DD_PRX_INT_ENABLE|APDS9960_DD_PRX_ENABLE);
		}

		dd_ptr->prx.enable = TRUE;
	}
	else if (SNS_DDF_SENSOR_AMBIENT == sensor)
	{
		if (dd_ptr->als.enable == TRUE)
		{
			ALSPRX_MSG_0(ERROR, "enable_interrupt End - als already enabled");
			return SNS_DDF_SUCCESS;
		}
		
		/* ENABLE=PON */
		status = apds9960_dd_set_enable(dd_ptr, i2c_pon_data);
		CHECK_STATUS(status);
		
		/* Clear interrupts */
		status = apds9960_dd_clear_int_reg(dd_ptr, APDS9960_DD_CMD_CLR_ALL_INT);
		CHECK_STATUS(status);
		
		status = apds9960_dd_update_intg_time(dd_ptr, dd_ptr->prx.enable, TRUE, FALSE);
		CHECK_STATUS(status);

		status = apds9960_dd_set_als_thresh(dd_ptr, 0xFFFF, 0); // force AMBIENT interrupt
		CHECK_STATUS(status);

		/* ENABLE */
		i2c_data = enable_reg | (APDS9960_DD_PWR_ON|APDS9960_DD_ALS_ENABLE|APDS9960_DD_ALS_INT_ENABLE|
								 APDS9960_DD_WAIT_ENABLE);
		
		dd_ptr->als.enable = TRUE;
	}
	else if (SNS_DDF_SENSOR_RGB == sensor)
	{
		if (dd_ptr->rgb.enable == TRUE)
		{
			ALSPRX_MSG_0(ERROR, "enable_interrupt End - rgb already enabled");
			return SNS_DDF_SUCCESS;
		}
		
		/* ENABLE=PON */
		status = apds9960_dd_set_enable(dd_ptr, i2c_pon_data);
		CHECK_STATUS(status);
		
		/* Clear interrupts */
		status = apds9960_dd_clear_int_reg(dd_ptr, APDS9960_DD_CMD_CLR_ALL_INT);
		CHECK_STATUS(status);
		
		status = apds9960_dd_update_intg_time(dd_ptr, dd_ptr->prx.enable, TRUE, FALSE);
		CHECK_STATUS(status);

		status = apds9960_dd_set_als_thresh(dd_ptr, 0xFFFF, 0); // force AMBIENT interrupt
		CHECK_STATUS(status);
		
		/* ENABLE */
		i2c_data = enable_reg | (APDS9960_DD_PWR_ON|APDS9960_DD_ALS_ENABLE|APDS9960_DD_ALS_INT_ENABLE|
								 APDS9960_DD_WAIT_ENABLE);
		
		dd_ptr->rgb.enable = TRUE;
	}
	else if (SNS_DDF_SENSOR_CT_C == sensor)
	{
		if (dd_ptr->cct.enable == TRUE)
		{
			ALSPRX_MSG_0(ERROR, "enable_interrupt End - cct already enabled");
			return SNS_DDF_SUCCESS;
		}
		
		/* ENABLE=PON */
		status = apds9960_dd_set_enable(dd_ptr, i2c_pon_data);
		CHECK_STATUS(status);
		
		/* Clear interrupts */
		status = apds9960_dd_clear_int_reg(dd_ptr, APDS9960_DD_CMD_CLR_ALL_INT);
		CHECK_STATUS(status);

		status = apds9960_dd_update_intg_time(dd_ptr, dd_ptr->prx.enable, TRUE, FALSE);
		CHECK_STATUS(status);

		status = apds9960_dd_set_als_thresh(dd_ptr, 0xFFFF, 0); // force AMBIENT interrupt
		CHECK_STATUS(status);
		
		/* ENABLE */
		i2c_data = enable_reg | (APDS9960_DD_PWR_ON|APDS9960_DD_ALS_ENABLE|APDS9960_DD_ALS_INT_ENABLE|
								 APDS9960_DD_WAIT_ENABLE);
		
		dd_ptr->cct.enable = TRUE;
	}
	
	/* Update ENABLE bits*/
	ALSPRX_MSG_1(MED, "after enable_reg=0x%X", i2c_data);
	status = apds9960_dd_set_enable(dd_ptr, i2c_data);
	CHECK_STATUS(status);
	
	if (sub_dev_idx == als_prx_idx)
	{
		if (rgb_cct_idx != -1)
		{
			if ( !dd_ptr->sub_dev[rgb_cct_idx].dri_enabled ||
				(dd_ptr->sub_dev[rgb_cct_idx].dri_enabled && (!dd_ptr->rgb.enable && !dd_ptr->cct.enable)) )
			{
				dd_ptr->sub_dev[sub_dev_idx].dri_registration = TRUE;
				status = sns_ddf_signal_register(dd_ptr->sub_dev[sub_dev_idx].interrupt_gpio, dd_handle,
												 &APDS9960_DD_IF, SNS_DDF_SIGNAL_IRQ_FALLING); // SNS_DDF_SIGNAL_IRQ_LOW
				ALSPRX_MSG_3(MED, "sub device %d interrupt signal registered, gpio=%x, status=%d", sub_dev_idx, dd_ptr->sub_dev[sub_dev_idx].interrupt_gpio, status);
			}
		}
		else
		{
			dd_ptr->sub_dev[sub_dev_idx].dri_registration = TRUE;
			status = sns_ddf_signal_register(dd_ptr->sub_dev[sub_dev_idx].interrupt_gpio, dd_handle,
											 &APDS9960_DD_IF, SNS_DDF_SIGNAL_IRQ_FALLING); // SNS_DDF_SIGNAL_IRQ_LOW
			ALSPRX_MSG_3(MED, "sub device %d interrupt signal registered, gpio=%x, status=%d", sub_dev_idx, dd_ptr->sub_dev[sub_dev_idx].interrupt_gpio, status);
		}
	}
	else //if (sub_dev_idx == rgb_cct_idx)
	{
		if (als_prx_idx != -1)
		{
			if ( !dd_ptr->sub_dev[als_prx_idx].dri_enabled ||
				(dd_ptr->sub_dev[als_prx_idx].dri_enabled && (!dd_ptr->prx.enable && !dd_ptr->als.enable)) )
			{
				dd_ptr->sub_dev[sub_dev_idx].dri_registration = TRUE;
				status = sns_ddf_signal_register(dd_ptr->sub_dev[sub_dev_idx].interrupt_gpio, dd_handle,
												 &APDS9960_DD_IF, SNS_DDF_SIGNAL_IRQ_FALLING); // SNS_DDF_SIGNAL_IRQ_LOW
				ALSPRX_MSG_3(MED, "sub device %d interrupt signal registered, gpio=%x, status=%d", sub_dev_idx, dd_ptr->sub_dev[sub_dev_idx].interrupt_gpio, status);
			}
		}
		else  // kk 13-Nov-2013
		{
			dd_ptr->sub_dev[sub_dev_idx].dri_registration = TRUE;
			status = sns_ddf_signal_register(dd_ptr->sub_dev[sub_dev_idx].interrupt_gpio, dd_handle,
											 &APDS9960_DD_IF, SNS_DDF_SIGNAL_IRQ_FALLING); // SNS_DDF_SIGNAL_IRQ_LOW
			ALSPRX_MSG_3(MED, "sub device %d interrupt signal registered, gpio=%x, status=%d", sub_dev_idx, dd_ptr->sub_dev[sub_dev_idx].interrupt_gpio, status);
		}
	}

	ALSPRX_MSG_2(MED, "enable_interrupt End - enable=%x status=%d", i2c_data, status);
	
	return status;
}

/*===========================================================================

FUNCTION      apds9960_dd_disable_interrupt

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e apds9960_dd_disable_interrupt
(
sns_ddf_handle_t  dd_handle,
sns_ddf_sensor_e  sensor
)
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	uint8_t i2c_data = APDS9960_DD_PWR_DOWN;
	apds9960_dd_dev_s* dd_ptr;
	uint32_t sub_dev_idx;
	uint8_t bytes_rw;
	uint8_t enable_reg;
	
	sub_dev_idx = (uint32_t)dd_handle & (APDS9960_DD_HANDLE_ALIGN-1);
	dd_ptr = (apds9960_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9960_DD_HANDLE_ALIGN-1)));
	
	if ((sensor != SNS_DDF_SENSOR_PROXIMITY) && (sensor != SNS_DDF_SENSOR_AMBIENT) &&
		(sensor != SNS_DDF_SENSOR_RGB) && (sensor != SNS_DDF_SENSOR_CT_C))
	{
		return SNS_DDF_EINVALID_PARAM;
	}
	
	ALSPRX_MSG_2(MED, "disable_interrupt - sensor=%d, sub device=%d", sensor, sub_dev_idx);
	
	status = sns_ddf_read_port(dd_ptr->port_handle,
							   APDS9960_DD_ENABLE_ADDR,
							   &i2c_data, 1, &bytes_rw);
	
	CHECK_STATUS(status);
	
	enable_reg = i2c_data;

	if (SNS_DDF_SENSOR_PROXIMITY == sensor)
	{
		if (!dd_ptr->prx.enable)
		{
			ALSPRX_MSG_0(ERROR, "disable_interrupt End - prx already disabled");
			return SNS_DDF_SUCCESS;
		}
		
		if (!dd_ptr->als.enable &&
			!dd_ptr->rgb.enable &&
			!dd_ptr->cct.enable)
		{
			i2c_data = APDS9960_DD_PWR_DOWN;
		}
		else //if (dd_ptr->als.enable||dd_ptr->rgb.enable||dd_ptr->cct.enable)
		{			
			i2c_data = enable_reg &
						(~APDS9960_DD_PRX_INT_ENABLE) &
						(~APDS9960_DD_PRX_ENABLE);
		}
		
		dd_ptr->prx.enable = FALSE;
		dd_ptr->common.prx_odr = 0;
	}
	else if ( SNS_DDF_SENSOR_AMBIENT == sensor )
	{
		if (!dd_ptr->als.enable)
		{
			ALSPRX_MSG_0(ERROR, "disable_interrupt End - als already disabled");
			return SNS_DDF_SUCCESS;
		}
		
		if (dd_ptr->prx.enable && !dd_ptr->rgb.enable && !dd_ptr->cct.enable)
		{
			i2c_data = enable_reg &
					(~APDS9960_DD_ALS_ENABLE) &
					(~APDS9960_DD_ALS_INT_ENABLE);		
		}
		else if (dd_ptr->rgb.enable || dd_ptr->cct.enable)
		{
			if (rgb_cct_idx != -1)
			{
				if (!dd_ptr->sub_dev[rgb_cct_idx].dri_enabled)
				{
					// Polling
					i2c_data = enable_reg &
								(~APDS9960_DD_ALS_INT_ENABLE);		
				}
				else
				{
					i2c_data = enable_reg;
				}
			}
		}
		else
		{
			i2c_data = APDS9960_DD_PWR_DOWN;
		}
		
		dd_ptr->als.enable = FALSE;
		dd_ptr->common.als_odr = 0;
	}
	else if ( SNS_DDF_SENSOR_RGB == sensor )
	{
		if (!dd_ptr->rgb.enable)
		{
			ALSPRX_MSG_0(ERROR, "disable_interrupt End - rgb already disabled");
			return SNS_DDF_SUCCESS;
		}
		
		if (dd_ptr->prx.enable && !dd_ptr->als.enable && !dd_ptr->cct.enable)
		{		
			i2c_data = enable_reg &
					(~APDS9960_DD_ALS_ENABLE) &
					(~APDS9960_DD_ALS_INT_ENABLE);		
		}
		else if (!dd_ptr->cct.enable && (dd_ptr->als.enable || dd_ptr->prx.enable))
		{
			if (als_prx_idx != -1)
			{
				if (!dd_ptr->sub_dev[als_prx_idx].dri_enabled)
				{
					// Polling
					i2c_data = enable_reg &
								(~APDS9960_DD_ALS_INT_ENABLE);		
				}
				else
				{
					i2c_data = enable_reg;
				}
			}
		}
		else if (dd_ptr->cct.enable)
		{
			i2c_data = enable_reg;
		}
		else
		{
			i2c_data = APDS9960_DD_PWR_DOWN;
		}
		
		dd_ptr->rgb.enable = FALSE;
		dd_ptr->common.rgb_odr = 0;
	}
	else if ( SNS_DDF_SENSOR_CT_C == sensor )
	{
		if (!dd_ptr->cct.enable)
		{
			ALSPRX_MSG_0(ERROR, "disable_interrupt End - cct already disabled");
			return SNS_DDF_SUCCESS;
		}
		
		if (dd_ptr->prx.enable && !dd_ptr->als.enable && !dd_ptr->rgb.enable)
		{		
			i2c_data = enable_reg &
					(~APDS9960_DD_ALS_ENABLE) &
					(~APDS9960_DD_ALS_INT_ENABLE);		
		}
		else if (!dd_ptr->rgb.enable && (dd_ptr->als.enable || dd_ptr->prx.enable))
		{
			if (als_prx_idx != -1)
			{
				if (!dd_ptr->sub_dev[als_prx_idx].dri_enabled)
				{
					// Polling
					i2c_data = enable_reg &
								(~APDS9960_DD_ALS_INT_ENABLE);		
				}
				else
				{
					i2c_data = enable_reg;
				}
			}
		}
		else if (dd_ptr->rgb.enable)
		{
			i2c_data = enable_reg;
		}
		else
		{
			i2c_data = APDS9960_DD_PWR_DOWN;
		}
		
		dd_ptr->cct.enable = FALSE;
		dd_ptr->common.cct_odr = 0;
	}

	status = apds9960_dd_set_enable(dd_ptr, i2c_data);
	CHECK_STATUS(status);

	if (sub_dev_idx == als_prx_idx)
	{
		status = apds9960_dd_clear_int_reg(dd_ptr, APDS9960_DD_CMD_CLR_ALL_INT);
		CHECK_STATUS(status);
		
		/* i2c_data is value in enable register */
		if (((i2c_data&(APDS9960_DD_ALS_INT_ENABLE)) != APDS9960_DD_ALS_INT_ENABLE) &&
			((i2c_data&(APDS9960_DD_PRX_INT_ENABLE)) != APDS9960_DD_PRX_INT_ENABLE))
		{
			dd_ptr->sub_dev[sub_dev_idx].dri_registration = FALSE;
			/* De-register INT notification */
			status = sns_ddf_signal_deregister(dd_ptr->sub_dev[sub_dev_idx].interrupt_gpio);
			CHECK_STATUS(status);			
		}
		else if (dd_ptr->als.enable || dd_ptr->prx.enable)
		{		
			status = apds9960_dd_update_intg_time(dd_ptr, dd_ptr->prx.enable,
												  (dd_ptr->als.enable|dd_ptr->rgb.enable|dd_ptr->cct.enable), FALSE);
			CHECK_STATUS(status);		
		}
	}
	else if (sub_dev_idx == rgb_cct_idx)
	{
		status = apds9960_dd_clear_int_reg(dd_ptr, APDS9960_DD_CMD_CLR_ALL_INT);
		CHECK_STATUS(status);
		
		/* i2c_data is value in enable register */
		if (((i2c_data&(APDS9960_DD_ALS_INT_ENABLE)) != APDS9960_DD_ALS_INT_ENABLE) &&
			((i2c_data&(APDS9960_DD_PRX_INT_ENABLE)) != APDS9960_DD_PRX_INT_ENABLE))
		{
			dd_ptr->sub_dev[sub_dev_idx].dri_registration = FALSE;
			/* De-register INT notification */
			status = sns_ddf_signal_deregister(dd_ptr->sub_dev[sub_dev_idx].interrupt_gpio);
			CHECK_STATUS(status);			
		}		
		else if (dd_ptr->rgb.enable || dd_ptr->cct.enable)
		{			
			status = apds9960_dd_update_intg_time(dd_ptr, dd_ptr->prx.enable,
												  (dd_ptr->als.enable|dd_ptr->rgb.enable|dd_ptr->cct.enable), FALSE);
			CHECK_STATUS(status);		
		}
	}
	
	ALSPRX_MSG_2(MED, "disable_interrupt End enable=%x, status=%d", i2c_data, status);
	return status;
}

/*===========================================================================

FUNCTION      sns_dd_apds9960_enable_sched_data

DESCRIPTION

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e sns_dd_apds9960_enable_sched_data
(
sns_ddf_handle_t dd_handle,
sns_ddf_sensor_e sensor,
bool enable
)
{
	apds9960_dd_dev_s* dd_ptr;
	uint32_t sub_dev_idx;

	sub_dev_idx = (uint32_t)dd_handle & (APDS9960_DD_HANDLE_ALIGN-1);
	dd_ptr = (apds9960_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9960_DD_HANDLE_ALIGN-1)));

    if (!dd_ptr)
    {
        ALSPRX_MSG_2(ERROR, "Invalid parameter enable_sched - sensor=%d enable=%d", sensor, enable);
        return SNS_DDF_EINVALID_PARAM;
    }

	ALSPRX_MSG_2(MED, "enable_sched - sensor=%d enable=%d", sensor, enable);
	
	if(enable)
	{
		dd_ptr->sub_dev[sub_dev_idx].dri_enabled = TRUE;
		return apds9960_dd_enable_interrupt(dd_handle, sensor);
	}
	else
	{
		return apds9960_dd_disable_interrupt(dd_handle, sensor);
	}
}

/*===========================================================================

FUNCTION      apds9960_dd_set_als_change_pcnt

DESCRIPTION   Sets the ALS change [ercentage

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None

===========================================================================*/
void apds9960_dd_set_als_change_pcnt
(
apds9960_dd_dev_s* dd_ptr,
uint32_t als_change_pcnt
)
{
	if ((als_change_pcnt <= APDS9960_DD_ALS_CHANGE_MIN_PCNT) ||
		(als_change_pcnt >= APDS9960_DD_ALS_CHANGE_MAX_PCNT))
	{
		dd_ptr->common.nv_db.als_change_pcnt = APDS9960_DD_DEFAULT_ALS_CHANGE_PCNT;
	}
	else
	{
		dd_ptr->common.nv_db.als_change_pcnt = als_change_pcnt;
	}
}

/*===========================================================================

FUNCTION      apds9960_dd_set_odr

DESCRIPTION   Sets ODR of the ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e apds9960_dd_set_odr
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
sns_ddf_sensor_e sensor,
uint32_t odr
)
{
	sns_ddf_status_e status=SNS_DDF_SUCCESS;
	bool change=TRUE;

	ALSPRX_MSG_3(MED, "set_odr sub_dev=%d, sensor=%d, odr=%d", sub_dev_idx, sensor, odr);
	
	switch ( sensor )
	{
    case SNS_DDF_SENSOR_AMBIENT:
		if (dd_ptr->common.als_odr == odr)
		{
			change = FALSE;
		}
		break;
		
    case SNS_DDF_SENSOR_PROXIMITY:
		if (dd_ptr->common.prx_odr == odr)
		{
			change = FALSE;
		}
		break;
		
    case SNS_DDF_SENSOR_RGB:
		if (dd_ptr->common.rgb_odr == odr)
		{
			change = FALSE;
		}
		break;
		
    case SNS_DDF_SENSOR_CT_C:
		if (dd_ptr->common.cct_odr == odr)
		{
			change = FALSE;
		}
		break;

    default:
		status = SNS_DDF_EFAIL;
	}
	
	if (change)
	{
		apds9960_dd_set_cycle_time(dd_ptr, sub_dev_idx, sensor, odr);
	}
	
	return status;
}

/*===========================================================================

FUNCTION      apds9960_dd_set_resolution

DESCRIPTION   Sets the resolution of the ALS/Proximity Sensor

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e apds9960_dd_set_resolution
(
apds9960_dd_dev_s* dd_ptr,
uint32_t sub_dev_idx,
sns_ddf_sensor_e  sensor,
apds9960_dd_res_e resolution
)
{
	uint8_t i2c_data = 0x0;
	sns_ddf_status_e status=SNS_DDF_EFAIL;

	ALSPRX_MSG_3(MED, "set_resolution sub_dev=%d, sensor=%d, resolution=%d", sub_dev_idx, sensor, resolution);
	
	switch ( sensor )
	{
    case SNS_DDF_SENSOR_AMBIENT:
    case SNS_DDF_SENSOR_RGB:
    case SNS_DDF_SENSOR_CT_C:
		if (resolution == sns_dd_als_adc_bit_tb[APDS9960_DD_RES_14BIT])
		{
			i2c_data  = sns_dd_als_atime_tb[APDS9960_DD_RES_14BIT];
			dd_ptr->als.als_res_index = APDS9960_DD_RES_14BIT;
		}
		else if (resolution == sns_dd_als_adc_bit_tb[APDS9960_DD_RES_15BIT])
		{
			i2c_data  = sns_dd_als_atime_tb[APDS9960_DD_RES_15BIT];
			dd_ptr->als.als_res_index = APDS9960_DD_RES_15BIT;
		}
		else
		{  // APDS9960_DD_RES_16BIT
			i2c_data  = sns_dd_als_atime_tb[APDS9960_DD_RES_16BIT];
			dd_ptr->als.als_res_index = APDS9960_DD_RES_16BIT;
		}
		
		status = apds9960_dd_set_atime(dd_ptr, i2c_data);
		
		break;
		
	default:
		status = SNS_DDF_EFAIL;
	}
	
	return status;
}

/*===========================================================================

FUNCTION      apds9960_dd_reset_registers

DESCRIPTION   Load pre-defined value to all registers

DEPENDENCIES  None

RETURN VALUE  SNS_DDF_SUCCESS on success

SIDE EFFECT   None

===========================================================================*/
sns_ddf_status_e apds9960_dd_reset_registers
(
apds9960_dd_dev_s* dd_ptr
)
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	
	/* DO NOT allow to reset registers if one of the sub dev is active */
	if (dd_ptr->prx.enable || dd_ptr->als.enable || dd_ptr->rgb.enable || dd_ptr->cct.enable)
	{
		return status;
	}
	
	ALSPRX_MSG_0(MED, "reset_registers");
	
	/* Initialize registers */
	/* ENABLE */
	status = apds9960_dd_set_enable(dd_ptr, APDS9960_DD_PWR_DOWN);	
	CHECK_STATUS(status);
	
	status = apds9960_dd_clear_int_reg(dd_ptr, APDS9960_DD_CMD_CLR_ALL_INT);
	CHECK_STATUS(status);
	
	/* PERS */
	status = apds9960_dd_set_pers(dd_ptr, APDS9960_DD_PPERS_2|APDS9960_DD_APERS_2);
	CHECK_STATUS(status);
	
	/* PPULSE */
	status = apds9960_dd_set_ppulse(dd_ptr, dd_ptr->prx.ppulse);
	CHECK_STATUS(status);
	
	/* CONTROL */
	status = apds9960_dd_set_control(dd_ptr, (sns_dd_als_again_bit_tb[dd_ptr->als.als_gain_index]|APDS9960_DD_PDRIVE_FOR_PS|APDS9960_DD_PGAIN_FOR_PS));
	CHECK_STATUS(status);
	
	status = apds9960_dd_update_intg_time(dd_ptr, FALSE, FALSE, TRUE);
	CHECK_STATUS(status);
	
	status = apds9960_dd_set_prx_thresh(dd_ptr, 0, dd_ptr->prx.thresh_near); // init threshold for proximity
	CHECK_STATUS(status);
	
	status = apds9960_dd_set_als_thresh(dd_ptr, 0xFFFF, 0);	// force first ALS interrupt to get the environment reading
	CHECK_STATUS(status);
	
	return status;
}

/*===========================================================================

FUNCTION:   sns_dd_apds9960_reset

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
sns_ddf_status_e sns_dd_apds9960_reset
(
sns_ddf_handle_t dd_handle
)
{
	apds9960_dd_dev_s* dd_ptr;
	sns_ddf_status_e init_status = SNS_DDF_SUCCESS;
	uint32_t sub_dev_idx;
	
	if ( dd_handle == NULL )
	{
		return SNS_DDF_EINVALID_PARAM;
	}
	
	sub_dev_idx = (uint32_t)dd_handle & (APDS9960_DD_HANDLE_ALIGN-1);
	dd_ptr = (apds9960_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9960_DD_HANDLE_ALIGN-1)));
	
	ALSPRX_MSG_1(MED, "reset sub_dev_idx=%d", sub_dev_idx);
	
	if ( (init_status = apds9960_dd_init_data_struct(dd_ptr, sub_dev_idx)) != SNS_DDF_SUCCESS )
	{
		return init_status;
	}
	
	/* init all registers with pre-defined value */
	if ( (init_status = apds9960_dd_reset_registers(dd_ptr)) != SNS_DDF_SUCCESS )
	{
		return init_status;
	}
	
	if ( (init_status = apds9960_dd_set_powerstate(dd_ptr, sub_dev_idx, SNS_DDF_SENSOR__ALL,
												   SNS_DDF_POWERSTATE_LOWPOWER, FALSE)) != SNS_DDF_SUCCESS )
	{
		return init_status;
	}
	
	return init_status;
}

/*===========================================================================

FUNCTION:   sns_dd_apds9960_init

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
sns_ddf_status_e sns_dd_apds9960_init
(
sns_ddf_handle_t*        dd_handle_ptr,
sns_ddf_handle_t         smgr_handle,
sns_ddf_nv_params_s*     nv_params,
sns_ddf_device_access_s  device_info[],
uint32_t                 num_devices,
sns_ddf_memhandler_s*    memhandler,
sns_ddf_sensor_e*        sensors[],
uint32_t*                num_sensors
)
{
	
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	apds9960_dd_dev_s* apds9960_dd_dev_ptr;
	apds9960_dd_nv_s* nv_ptr;
	uint32_t apds9960_num_sensors=0;
	
	ALSPRX_MSG_2(MED, "init sensor[0]=%d, num_sensor=%d", *sensors[0], *num_sensors);
	
	if ( (dd_handle_ptr == NULL) || (num_sensors == NULL) || (sensors == NULL) )
	{
		return SNS_DDF_EINVALID_PARAM;
	}

    /* uImage support */
#ifdef APDS9960_ENABLE_UIMG_SUPPORT
	sns_ddf_smgr_set_uimg_refac(smgr_handle);
#endif
	
	apds9960_dd_dev_ptr = apds9960_dd_find_same_port_dev(&device_info->port_config);
	
	if (apds9960_dd_dev_ptr==NULL)
	{

		if (sns_ddf_malloc_ex((void **)&apds9960_dd_dev_ptr, sizeof(apds9960_dd_dev_s), smgr_handle) != SNS_DDF_SUCCESS)
		{ 
			ALSPRX_MSG_1(ERROR, "sns_ddf_malloc error %d", sizeof(apds9960_dd_dev_s));
			return SNS_DDF_ENOMEM;
		}
		
		memset(apds9960_dd_dev_ptr, 0, sizeof(apds9960_dd_dev_s));

        *dd_handle_ptr = (sns_ddf_handle_t)apds9960_dd_dev_ptr;		

        apds9960_dd_dev_ptr->next = NULL;
		apds9960_dd_head.next = apds9960_dd_dev_ptr;
		apds9960_dd_dev_ptr->sub_dev_cnt = 0;

		sns_ddf_get_port_id(&device_info->port_config, &apds9960_dd_dev_ptr->port_id);
		
		ALSPRX_MSG_0(MED, "init - first");
		
		ALSPRX_MSG_1(MED, "smgr_handle = %x", smgr_handle);

		/* Fill out supported sensor info */
		apds9960_dd_dev_ptr->sub_dev[0].smgr_handle = smgr_handle;
		apds9960_num_sensors = apds9960_dd_fill_sub_dev_slot(&apds9960_dd_dev_ptr->sub_dev[0], *num_sensors, *sensors);
		
		*num_sensors = apds9960_num_sensors;
		*sensors = (sns_ddf_sensor_e *)apds9960_dd_dev_ptr->sub_dev[0].sensors;
		
		if (0 == apds9960_num_sensors)
		{
			sns_ddf_mfree_ex(apds9960_dd_dev_ptr, smgr_handle); // free the memory
			*dd_handle_ptr = NULL;
            apds9960_dd_head.next = NULL;
			ALSPRX_MSG_1(ERROR, "apds9960_num_sensors=%d", apds9960_num_sensors);
			return SNS_DDF_EINVALID_DATA;
		}
		
		apds9960_dd_dev_ptr->sub_dev_cnt++;
		
		/* open normal port */
		if ((status = sns_ddf_open_port(&apds9960_dd_dev_ptr->port_handle, &device_info->port_config))!= SNS_DDF_SUCCESS)
		{
			sns_ddf_mfree_ex(apds9960_dd_dev_ptr, smgr_handle); // free the memory
			*dd_handle_ptr = NULL;
            apds9960_dd_head.next = NULL;
			ALSPRX_MSG_1(ERROR, "sns_ddf_open_port error, status=%d", status);
			return status;
		}

        {
           uint8_t i2c_data = 0;
           uint8_t bytes_rw = 0;
           /* Read & Verify Device ID */
           status = sns_ddf_read_port( apds9960_dd_dev_ptr->port_handle,
                                  APDS9960_DD_ID_ADDR,
                                  &i2c_data,
                                  1,
                                  &bytes_rw );
       
           ALSPRX_MSG_2(HIGH, "ID=0x%X status=%d", i2c_data, status);

           if (status != SNS_DDF_SUCCESS || bytes_rw != 1)
           {
             ALSPRX_MSG_1(ERROR, "ddf_read_port error, status=%d", status);
             sns_ddf_mfree_ex(apds9960_dd_dev_ptr, smgr_handle); // free the memory
             *dd_handle_ptr = NULL;
             apds9960_dd_head.next = NULL;
             return SNS_DDF_EBUS;
           }
       
           if (i2c_data != 0xAB) 
           { /* APDS-9960 */
             ALSPRX_MSG_1(ERROR, "invalid sensor ID=0x%x", i2c_data);
             sns_ddf_mfree_ex(apds9960_dd_dev_ptr, smgr_handle); // free the memory
             *dd_handle_ptr = NULL;
             apds9960_dd_head.next = NULL;
             return SNS_DDF_EDEVICE;
           }
       }
		
		/* open additional port for zero byte write(ZBW) */
		device_info->port_config.bus_config.i2c->dev_type = SNS_DDF_I2C_DEVICE_DEFAULT;
		if ( (status = sns_ddf_open_port(&apds9960_dd_dev_ptr->ZBW_port_handle, &device_info->port_config))!= SNS_DDF_SUCCESS )
		{
			sns_ddf_mfree_ex(apds9960_dd_dev_ptr, smgr_handle); // free the memory
            *dd_handle_ptr = NULL;
            apds9960_dd_head.next = NULL;
			ALSPRX_MSG_1(ERROR, "sns_ddf_open_port error, status=%d", status);
			return status;
		}
		
#ifndef QDSP6
		/* open additional port for zero byte write(ZBW) */
		device_info->port_config.bus_config.i2c->dev_type = SNS_DDF_I2C_DEVICE_DEFAULT;
		if ((status = sns_ddf_open_port(&apds9960_dd_dev_ptr->ZBW_port_handle, &device_info->port_config))!= SNS_DDF_SUCCESS)
		{
			sns_ddf_mfree_ex(apds9960_dd_dev_ptr, smgr_handle); // free the memory
            *dd_handle_ptr = NULL;
            apds9960_dd_head.next = NULL;
			ALSPRX_MSG_1(ERROR, "sns_ddf_open_port error, status=%d", status);
			return status;
		}
#endif
		
		apds9960_dd_dev_ptr->sub_dev[0].interrupt_gpio = device_info->first_gpio;		
		apds9960_dd_dev_ptr->sub_dev[0].dri_enabled = FALSE;

		/* timer */
		status = apds9960_dd_timer_init(apds9960_dd_dev_ptr, 0, (sns_ddf_sensor_e *)*sensors);
		
		if ( status != SNS_DDF_SUCCESS )
		{
			sns_ddf_mfree_ex(apds9960_dd_dev_ptr, smgr_handle);
            *dd_handle_ptr = NULL;
            apds9960_dd_head.next = NULL;
			ALSPRX_MSG_1(ERROR, "apds9960_dd_timer_init error, status=%d", status);
			return status;
		}
		
		/* Recall NV parameters if any */
		if (nv_params)
		{
			bool data_from_registry = FALSE;
	  			
			if ((nv_params->status == SNS_DDF_SUCCESS) && (nv_params->data_len >= sizeof(apds9960_dd_nv_s)))
			{
				nv_ptr = (apds9960_dd_nv_s *)nv_params->data;

				if ((nv_ptr->version_num != 0) &&
					(nv_ptr->id != 0) &&
						(nv_params->data_len != 0) )
				{
					// update value from NV parameters
					data_from_registry = TRUE;
					apds9960_dd_dev_ptr->common.nv_size = nv_params->data_len;
					apds9960_dd_dev_ptr->common.nv_db.version_num = nv_ptr->version_num;
					apds9960_dd_dev_ptr->common.nv_db.id = nv_ptr->id;
					
					apds9960_dd_dev_ptr->common.nv_db.visible_ratio = nv_ptr->visible_ratio;
					apds9960_dd_dev_ptr->common.nv_db.ir_ratio = nv_ptr->ir_ratio;

					apds9960_dd_dev_ptr->common.nv_db.als_factor = nv_ptr->als_factor;

					/* RGBC factor */
					apds9960_dd_dev_ptr->common.nv_db.rgb_c_factor = nv_ptr->rgb_c_factor;
					apds9960_dd_dev_ptr->common.nv_db.rgb_r_factor = nv_ptr->rgb_r_factor;
					apds9960_dd_dev_ptr->common.nv_db.rgb_g_factor = nv_ptr->rgb_g_factor;
					apds9960_dd_dev_ptr->common.nv_db.rgb_b_factor = nv_ptr->rgb_b_factor;

					apds9960_dd_set_als_change_pcnt(apds9960_dd_dev_ptr, nv_ptr->als_change_pcnt);					

					apds9960_dd_dev_ptr->common.nv_db.cct_factor = nv_ptr->cct_factor;
					
					// update prx factor and thresh through this
					if(nv_ptr->prx_crosstalk == 0)
					{
						apds9960_dd_dev_ptr->common.nv_db.thresh_near = nv_ptr->thresh_near;
						apds9960_dd_dev_ptr->common.nv_db.thresh_far = nv_ptr->thresh_far;
						apds9960_dd_dev_ptr->common.nv_db.prx_crosstalk = nv_ptr->prx_crosstalk;
					}
					else
					{
						apds9960_dd_dev_ptr->common.nv_db.thresh_near = nv_ptr->prx_crosstalk + APDS9960_DD_PRX_FACTOR;
						apds9960_dd_dev_ptr->common.nv_db.thresh_far = apds9960_dd_dev_ptr->common.nv_db.thresh_near-10;
						apds9960_dd_dev_ptr->common.nv_db.prx_crosstalk = nv_ptr->prx_crosstalk;
					}

					apds9960_dd_dev_ptr->common.nv_db.ppulse = nv_ptr->ppulse;
					apds9960_dd_dev_ptr->common.nv_db.poffset_ur = nv_ptr->poffset_ur;
					apds9960_dd_dev_ptr->common.nv_db.poffset_dl = nv_ptr->poffset_dl;	
					
					ALSPRX_MSG_3(MED, "als_factor=%d, c_factor=%d, r_factor=%d", apds9960_dd_dev_ptr->common.nv_db.als_factor,
						apds9960_dd_dev_ptr->common.nv_db.rgb_c_factor,
						apds9960_dd_dev_ptr->common.nv_db.rgb_r_factor);

					ALSPRX_MSG_3(MED, "g_factor=%d, b_factor=%d, cct_factor=%d", apds9960_dd_dev_ptr->common.nv_db.rgb_g_factor,
						apds9960_dd_dev_ptr->common.nv_db.rgb_b_factor,
						apds9960_dd_dev_ptr->common.nv_db.cct_factor);

					ALSPRX_MSG_3(MED, "thresh_near=%d, thresh_far=%d, prx_crosstalk=%d", apds9960_dd_dev_ptr->common.nv_db.thresh_near,
						apds9960_dd_dev_ptr->common.nv_db.thresh_far,
						apds9960_dd_dev_ptr->common.nv_db.prx_crosstalk);

					ALSPRX_MSG_3(MED, "ppulse=0x%x, poffset_ur=%d, poffset_dl=%d", apds9960_dd_dev_ptr->common.nv_db.ppulse,
						apds9960_dd_dev_ptr->common.nv_db.poffset_ur,
						apds9960_dd_dev_ptr->common.nv_db.poffset_dl);
				}
			}

			if (!data_from_registry)
			{
				// default value of NV parameters
				apds9960_dd_dev_ptr->common.nv_size = 128;
				apds9960_dd_dev_ptr->common.nv_db.version_num = 1;
				/* 0001xxxx = APDS-9900, 0010xxxx = APDS-9930, 0011xxx = APDS-9190, 0100xxxx = APDS-9130, 0x0101xxxx = APDS-9950, 0x0110xxxx = APDS-9960 */
				apds9960_dd_dev_ptr->common.nv_db.id = 0x60|SNS_DDF_VENDOR_AVAGO;
				
				apds9960_dd_dev_ptr->common.nv_db.visible_ratio = 20; // 20% visible transmittance
				apds9960_dd_dev_ptr->common.nv_db.ir_ratio = 80; // 80% ir transmittance
				apds9960_dd_dev_ptr->common.nv_db.als_factor = 100; // factor of 1
				apds9960_dd_set_als_change_pcnt(apds9960_dd_dev_ptr, APDS9960_DD_DEFAULT_ALS_CHANGE_PCNT);

				  /* RGBC factor */
				apds9960_dd_dev_ptr->common.nv_db.rgb_c_factor = 100;
				apds9960_dd_dev_ptr->common.nv_db.rgb_r_factor = 100;
				apds9960_dd_dev_ptr->common.nv_db.rgb_g_factor = 100;
				apds9960_dd_dev_ptr->common.nv_db.rgb_b_factor = 100;

				apds9960_dd_dev_ptr->common.nv_db.cct_factor = 100; // factor of 1
				
				apds9960_dd_dev_ptr->common.nv_db.thresh_near = APDS9960_DD_PRX_NEAR_THRESHOLD;
				apds9960_dd_dev_ptr->common.nv_db.thresh_far = APDS9960_DD_PRX_FAR_THRESHOLD;
				apds9960_dd_dev_ptr->common.nv_db.prx_crosstalk = 0;
				apds9960_dd_dev_ptr->common.nv_db.ppulse = (APDS9960_DD_PPULSE_FOR_PS-1)|APDS9960_DD_PPULSE_LEN_FOR_PS;
				apds9960_dd_dev_ptr->common.nv_db.poffset_ur = 0;
				apds9960_dd_dev_ptr->common.nv_db.poffset_dl = 0;
				
				sns_ddf_smgr_notify_event(apds9960_dd_dev_ptr->sub_dev[0].smgr_handle, apds9960_dd_dev_ptr->sub_dev[0].sensors[0], SNS_DDF_EVENT_UPDATE_REGISTRY_GROUP);
			}
		}
		
		/* Resets the driver */ /* reset parameters after NV */
		status = sns_dd_apds9960_reset((sns_ddf_handle_t) apds9960_dd_dev_ptr);
		ALSPRX_MSG_0(MED, "init - first end");
	}
	else
	{
		sns_ddf_handle_t  apds9960_dd_handle;
		uint32_t  sub_dev_idx;
		
		ALSPRX_MSG_0(MED, "init - second");
		
		sub_dev_idx = apds9960_dd_find_free_sub_dev_slot(apds9960_dd_dev_ptr);
		
		if (sub_dev_idx == -1)
		{
			ALSPRX_MSG_1(ERROR, "find_free_sub_dev error, sub_dev=%d", sub_dev_idx);
			return SNS_DDF_ENOMEM;
		}
		else
		{
			ALSPRX_MSG_1(MED, "smgr_handle = %x", smgr_handle);

			// load with second sub-device
			
			apds9960_dd_dev_ptr->sub_dev[sub_dev_idx].smgr_handle = smgr_handle;
			
			apds9960_num_sensors = apds9960_dd_fill_sub_dev_slot(&apds9960_dd_dev_ptr->sub_dev[sub_dev_idx], *num_sensors, *sensors);
			
			*num_sensors = apds9960_num_sensors;
			*sensors = (sns_ddf_sensor_e *)apds9960_dd_dev_ptr->sub_dev[sub_dev_idx].sensors;
			
			if (0 == apds9960_num_sensors)
			{
				ALSPRX_MSG_1(ERROR, "apds9960_num_sensors =%d", apds9960_num_sensors);
				return SNS_DDF_EINVALID_DATA;
			}
			
			apds9960_dd_dev_ptr->sub_dev_cnt++;

            ALSPRX_MSG_1(HIGH, "asub_dev_cnt=%d", apds9960_dd_dev_ptr->sub_dev_cnt++);
			
			apds9960_dd_handle = (sns_ddf_handle_t)((uint32_t)apds9960_dd_dev_ptr | sub_dev_idx);
			*dd_handle_ptr = apds9960_dd_handle;
			
			apds9960_dd_dev_ptr->sub_dev[sub_dev_idx].interrupt_gpio = device_info->first_gpio;
			apds9960_dd_dev_ptr->sub_dev[sub_dev_idx].dri_enabled = FALSE;
					
			/* timer */
			status = apds9960_dd_timer_init(apds9960_dd_dev_ptr, sub_dev_idx, (sns_ddf_sensor_e *)*sensors);
			
			if ( status != SNS_DDF_SUCCESS )
			{
				ALSPRX_MSG_1(ERROR, "apds9960_dd_timer_init error, status=%d", status);
				return status;
			}
			
			status = sns_dd_apds9960_reset((sns_ddf_handle_t) apds9960_dd_handle);
			
			ALSPRX_MSG_0(MED, "init - second end");
		}
	}
	
	return status;
}

/*===========================================================================

FUNCTION:   sns_dd_apds9960_set_attrib

===========================================================================*/
/*!
@brief Sets an attribute of the ALS/Prx sensor

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
sns_ddf_status_e sns_dd_apds9960_set_attrib
(
sns_ddf_handle_t     dd_handle,
sns_ddf_sensor_e     sensor,
sns_ddf_attribute_e  attrib,
void*                value
)
{
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	apds9960_dd_dev_s* dd_ptr;
	uint32_t sub_dev_idx;

	if ( (dd_handle == NULL) || (value == NULL) )
	{
		return SNS_DDF_EINVALID_PARAM;
	}
	
	sub_dev_idx = (uint32_t)dd_handle & (APDS9960_DD_HANDLE_ALIGN-1);
	dd_ptr = (apds9960_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9960_DD_HANDLE_ALIGN-1)));

	ALSPRX_MSG_3(MED, "set_attr - sub_dev=%d, sensor=%d attr=%d", sub_dev_idx, sensor, attrib);
	
	switch ( attrib )
	{
    case SNS_DDF_ATTRIB_POWER_STATE:
		ALSPRX_MSG_0(MED, "SNS_DDF_ATTRIB_POWER_STATE");

		/* set power mode */		
		status = apds9960_dd_set_powerstate(dd_ptr,
											sub_dev_idx,
											sensor,
											*(sns_ddf_powerstate_e *)value,
											FALSE);
		break;
		
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
		ALSPRX_MSG_0(MED, "SNS_DDF_ATTRIB_RESOLUTION_ADC");

		/* set resolution */
		status = apds9960_dd_set_resolution(dd_ptr,
											sub_dev_idx,
											sensor,
											*(apds9960_dd_res_e *)value);
		break;
		
    case SNS_DDF_ATTRIB_ODR:
		ALSPRX_MSG_0(MED, "SNS_DDF_ATTRIB_ODR");

		/* output data rate */
		status = apds9960_dd_set_odr(dd_ptr,
									 sub_dev_idx,
									 sensor,
									 *(uint32_t *)value);
		break;

	case SNS_DDF_ATTRIB_RANGE:

		break;
		
    default:
		ALSPRX_MSG_1(ERROR, "unknown attrib %d", *(uint32_t *)value);
		return SNS_DDF_EINVALID_PARAM;
	}
	
	return(status);
}

/*==============================================================================

FUNCTION      apds9960_dd_prx_query

DESCRIPTION   Called by sns_dd_apds9960_get_attrib to get an attribute value for proximitydistance data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e apds9960_dd_prx_query
(
apds9960_dd_dev_s* dd_ptr,
sns_ddf_memhandler_s*  memhandler,
sns_ddf_attribute_e    attrib,
void**                 value,
uint32_t*              num_elems
)
{
	uint16_t* thresh_ptr;
	uint16_t* accu_ptr;
	sns_ddf_power_info_s* power_ptr;
	
	switch ( attrib )
	{
    case SNS_DDF_ATTRIB_RANGE:
		{
			sns_ddf_range_s *device_ranges;

			ALSPRX_MSG_0(MED, "prx_query - SNS_DDF_ATTRIB_RANGE");

			if( (*value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_range_s)))  == NULL)
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}

			device_ranges = *value;
			*num_elems = 1;
			device_ranges->min = FX_FLTTOFIX_Q16(0.00); /* From 0 mm*/
			device_ranges->max = FX_FLTTOFIX_Q16(0.05);  /* to 50 mm*/
			break;
		}
		
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
		{
			sns_ddf_resolution_adc_s* resp;

			ALSPRX_MSG_0(MED, "prx_query - SNS_DDF_ATTRIB_RESOLUTION_ADC");

			if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_adc_s))) == NULL )
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			*num_elems = 1;
			resp->bit_len = 8;
			resp->max_freq = 20; // Hz
			*value = resp;
			break;
		}
		
    case SNS_DDF_ATTRIB_RESOLUTION:
		{
			q16_t* resp;

			ALSPRX_MSG_0(MED, "prx_query - SNS_DDF_ATTRIB_RESOLUTION");

			if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(q16_t))) == NULL )
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			*num_elems = 1;
			*resp = APDS9960_DD_PRX_RES;
			*value = resp;
			break;
		}
		
    case SNS_DDF_ATTRIB_POWER_INFO:

		ALSPRX_MSG_0(MED, "prx_query - SNS_DDF_ATTRIB_POWER_INFO");

		if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
		{
			ALSPRX_MSG_0(ERROR, "malloc error");
			return SNS_DDF_ENOMEM;
		}
		*num_elems = 1;
		power_ptr->active_current = APDS9960_DD_PRX_PWR;
		power_ptr->lowpower_current = APDS9960_DD_PRX_LO_PWR;
		*(sns_ddf_power_info_s **)value = power_ptr;
		break;
		
    case SNS_DDF_ATTRIB_ACCURACY:

		ALSPRX_MSG_0(MED, "prx_query - SNS_DDF_ATTRIB_ACCURACY");

		if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
		{
			ALSPRX_MSG_0(ERROR, "malloc error");
			return SNS_DDF_ENOMEM;
		}
		*num_elems = 1;
		*accu_ptr = APDS9960_DD_PRX_ACCURACY;
		*(uint16_t **)value = accu_ptr;
		break;
		
    case SNS_DDF_ATTRIB_THRESHOLD:

		ALSPRX_MSG_0(MED, "prx_query - SNS_DDF_ATTRIB_THRESHOLD");

		if ( (thresh_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
		{
			ALSPRX_MSG_0(ERROR, "malloc error");
			return SNS_DDF_ENOMEM;
		}
		*num_elems = 1;
		*thresh_ptr = dd_ptr->prx.thresh_near;
		*(uint16_t **)value = thresh_ptr;
		break;
		
    case SNS_DDF_ATTRIB_ODR:
		{
			uint32_t* odr_ptr;
			
			ALSPRX_MSG_0(MED, "prx_query - SNS_DDF_ATTRIB_ODR");

			if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			*num_elems = 1;
			*odr_ptr = dd_ptr->common.prx_odr;
			*(uint32_t **)value = odr_ptr;
			ALSPRX_MSG_1(MED, "odr=%d", *odr_ptr);
			break;
		}

	case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
		{
			sns_ddf_odr_t* odr_ptr;
			uint32_t i;

			ALSPRX_MSG_0(MED, "als_query - SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST");

			if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, (sizeof(sns_dd_apds9960_odr_tb)/sizeof(sns_ddf_odr_t)) * sizeof(sns_ddf_resolution_adc_s))) == NULL )
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			
			*num_elems = sizeof(sns_dd_apds9960_odr_tb)/sizeof(sns_ddf_odr_t);

			for (i=0; i<*num_elems; i++)
			{		
				odr_ptr[i] = sns_dd_apds9960_odr_tb[i];
			}
			
			*value = odr_ptr;
		}
		break;
		
    default:
		ALSPRX_MSG_1(ERROR, "UNKNOWN prx_query - attrib=%d", attrib);
		return SNS_DDF_EINVALID_ATTR;
	}
	return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      apds9960_dd_als_query

DESCRIPTION   Called by sns_dd_apds9960_get_attrib to get an attribute value for ALS(light) data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e apds9960_dd_als_query
(
apds9960_dd_dev_s* dd_ptr,
sns_ddf_memhandler_s*  memhandler,
sns_ddf_attribute_e    attrib,
void**                 value,
uint32_t*              num_elems
)
{
	sns_ddf_power_info_s* power_ptr;
	uint16_t* accu_ptr;
	
	switch ( attrib )
	{
    case SNS_DDF_ATTRIB_RANGE:
		{
			sns_ddf_range_s *device_ranges;

			ALSPRX_MSG_0(MED, "als_query - SNS_DDF_ATTRIB_RANGE");

			if( (*value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_range_s)))  == NULL)
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			
			device_ranges = *value;
			*num_elems = 1;
			device_ranges->min = FX_FLTTOFIX_Q16(0);
			device_ranges->max = FX_FLTTOFIX_Q16(APDS9960_DD_ALS_MAX_LUX);
			break;
		}
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
		{
			sns_ddf_resolution_adc_s* resp;

			ALSPRX_MSG_0(MED, "als_query - SNS_DDF_ATTRIB_RESOLUTION_ADC");

			if ( (resp = sns_ddf_memhandler_malloc(memhandler, 3 * sizeof(sns_ddf_resolution_adc_s))) == NULL )
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			
			*num_elems = 3;
			
			resp[0].bit_len = sns_dd_als_adc_bit_tb[APDS9960_DD_RES_14BIT];
			resp[0].max_freq = 20; // Hz
			resp[1].bit_len = sns_dd_als_adc_bit_tb[APDS9960_DD_RES_15BIT];
			resp[1].max_freq = 10; // Hz
			resp[2].bit_len = sns_dd_als_adc_bit_tb[APDS9960_DD_RES_16BIT];
			resp[2].max_freq = 5; // Hz
			
			*value = resp;
			break;
		}
		
    case SNS_DDF_ATTRIB_RESOLUTION:
		{
			sns_ddf_resolution_t* resp;

			ALSPRX_MSG_0(MED, "als_query - SNS_DDF_ATTRIB_RESOLUTION");

			if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			*num_elems = 1;
			
			*resp = APDS9960_DD_ALS_RES;
			*value = resp;
			break;
		}
		
    case SNS_DDF_ATTRIB_POWER_INFO:

		ALSPRX_MSG_0(MED, "als_query - SNS_DDF_ATTRIB_POWER_INFO");

		if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
		{
			ALSPRX_MSG_0(ERROR, "malloc error");
			return SNS_DDF_ENOMEM;
		}
		*num_elems = 1;
		power_ptr->active_current = APDS9960_DD_ALS_PWR;
		power_ptr->lowpower_current = APDS9960_DD_ALS_LO_PWR;
		*(sns_ddf_power_info_s **)value = power_ptr;
		break;
		
    case SNS_DDF_ATTRIB_ACCURACY:

		ALSPRX_MSG_0(MED, "als_query - SNS_DDF_ATTRIB_ACCURACY");

		if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
		{
			ALSPRX_MSG_0(ERROR, "malloc error");
			return SNS_DDF_ENOMEM;
		}
		*num_elems = 1;
		*accu_ptr = APDS9960_DD_ALS_ACCURACY;
		*(uint16_t **)value = accu_ptr;
		break;
		
    case SNS_DDF_ATTRIB_ODR:
		{
			uint32_t* odr_ptr;
			
			ALSPRX_MSG_0(MED, "als_query - SNS_DDF_ATTRIB_ODR");

			if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			*num_elems = 1;
			*odr_ptr = dd_ptr->common.als_odr;
			*(uint32_t **)value = odr_ptr;
			ALSPRX_MSG_1(MED, "odr=%d", *odr_ptr);
			break;
		}

	case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
		{
			sns_ddf_odr_t* odr_ptr;
			uint32_t i;

			ALSPRX_MSG_0(MED, "als_query - SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST");

			if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, (sizeof(sns_dd_apds9960_odr_tb)/sizeof(sns_ddf_odr_t)) * sizeof(sns_ddf_resolution_adc_s))) == NULL )
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			
			*num_elems = sizeof(sns_dd_apds9960_odr_tb)/sizeof(sns_ddf_odr_t);

			for (i=0; i<*num_elems; i++)
			{		
				odr_ptr[i] = sns_dd_apds9960_odr_tb[i];
			}
			
			*value = odr_ptr;
		}
		break;
		
    default:
		ALSPRX_MSG_1(ERROR, "UNKNOWN als_query - attrib=%d", attrib);
		return SNS_DDF_EINVALID_ATTR;
	}
	return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      apds9960_dd_rgb_query

DESCRIPTION   Called by sns_dd_apds9960_get_attrib to get an attribute value for RGB data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e apds9960_dd_rgb_query
(
apds9960_dd_dev_s* dd_ptr,
sns_ddf_memhandler_s*  memhandler,
sns_ddf_attribute_e    attrib,
void**                 value,
uint32_t*              num_elems
)
{
	sns_ddf_power_info_s* power_ptr;
	uint16_t* accu_ptr;
	
	switch ( attrib )
	{
    case SNS_DDF_ATTRIB_RANGE:
		{
			sns_ddf_range_s *device_ranges;

			ALSPRX_MSG_0(MED, "rgb_query - SNS_DDF_ATTRIB_RANGE");

			if( (*value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_range_s)))  == NULL) 
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			
			device_ranges = *value;
			*num_elems = 1;
			device_ranges->min = FX_FLTTOFIX_Q16(0.0); // minimum irradiance
			device_ranges->max = FX_FLTTOFIX_Q16(65535.0);  // maximum irradiance
			break;
		}

    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
		{
			sns_ddf_resolution_adc_s* resp;

			ALSPRX_MSG_0(MED, "rgb_query - SNS_DDF_ATTRIB_RESOLUTION_ADC");

			if ( (resp = sns_ddf_memhandler_malloc(memhandler, 3 * sizeof(sns_ddf_resolution_adc_s))) == NULL )

			{
				return SNS_DDF_ENOMEM;
			}
			
			*num_elems = 3;
			
			resp[0].bit_len = sns_dd_als_adc_bit_tb[APDS9960_DD_RES_14BIT];
			resp[0].max_freq = 20; // Hz
			resp[1].bit_len = sns_dd_als_adc_bit_tb[APDS9960_DD_RES_15BIT];
			resp[1].max_freq = 10; // Hz
			resp[2].bit_len = sns_dd_als_adc_bit_tb[APDS9960_DD_RES_16BIT];
			resp[2].max_freq = 5; // Hz
			
			*value = resp;
			break;
		}
		
    case SNS_DDF_ATTRIB_RESOLUTION:
		{
			sns_ddf_resolution_t* resp;

			ALSPRX_MSG_0(MED, "rgb_query - SNS_DDF_ATTRIB_RESOLUTION");

			if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
			{
				return SNS_DDF_ENOMEM;
			}
			*num_elems = 1;
			
			*resp = APDS9960_DD_RGB_RES;
			*value = resp;
			break;
		}
		
    case SNS_DDF_ATTRIB_POWER_INFO:

		ALSPRX_MSG_0(MED, "rgb_query - SNS_DDF_ATTRIB_POWER_INFO");

		if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
		{
			return SNS_DDF_ENOMEM;
		}
		*num_elems = 1;
		power_ptr->active_current = APDS9960_DD_RGB_PWR;
		power_ptr->lowpower_current = APDS9960_DD_RGB_LO_PWR;
		*(sns_ddf_power_info_s **)value = power_ptr;
		break;
		
    case SNS_DDF_ATTRIB_ACCURACY:

		ALSPRX_MSG_0(MED, "rgb_query - SNS_DDF_ATTRIB_ACCURACY");

		if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
		{
			return SNS_DDF_ENOMEM;
		}
		*num_elems = 1;
		*accu_ptr = APDS9960_DD_ALS_ACCURACY;
		*(uint16_t **)value = accu_ptr;
		break;
		
    case SNS_DDF_ATTRIB_ODR:
		{
			uint32_t* odr_ptr;
			
			ALSPRX_MSG_0(MED, "rgb_query - SNS_DDF_ATTRIB_ODR");

			if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
			{
				return SNS_DDF_ENOMEM;
			}
			*num_elems = 1;
			*odr_ptr = dd_ptr->common.rgb_odr;
			*(uint32_t **)value = odr_ptr;
			ALSPRX_MSG_1(MED, "odr=%d", *odr_ptr);
			break;
		}

	case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
		{
			sns_ddf_odr_t* odr_ptr;
			uint32_t i;

			ALSPRX_MSG_0(MED, "als_query - SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST");

			if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, (sizeof(sns_dd_apds9960_odr_tb)/sizeof(sns_ddf_odr_t)) * sizeof(sns_ddf_resolution_adc_s))) == NULL )
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			
			*num_elems = sizeof(sns_dd_apds9960_odr_tb)/sizeof(sns_ddf_odr_t);

			for (i=0; i<*num_elems; i++)
			{		
				odr_ptr[i] = sns_dd_apds9960_odr_tb[i];
			}
			
			*value = odr_ptr;
		}
		break;
		
    default:
		ALSPRX_MSG_1(ERROR, "UNKNOWN rgb_query - attrib=%d", attrib);
		return SNS_DDF_EINVALID_ATTR;
	}
	return SNS_DDF_SUCCESS;
}

/*==============================================================================

FUNCTION      apds9960_dd_cct_query

DESCRIPTION   Called by sns_dd_apds9960_get_attrib to get an attribute value for CT_C data type

DEPENDENCIES  None

RETURN VALUE  Attribute value pointer on success, NULL otherwise

SIDE EFFECT   None

==============================================================================*/
sns_ddf_status_e apds9960_dd_cct_query
(
apds9960_dd_dev_s* dd_ptr,
sns_ddf_memhandler_s*  memhandler,
sns_ddf_attribute_e    attrib,
void**                 value,
uint32_t*              num_elems
)
{
	sns_ddf_power_info_s* power_ptr;
	uint16_t* accu_ptr;
	
	switch ( attrib )
	{
    case SNS_DDF_ATTRIB_RANGE:
		{
			sns_ddf_range_s *device_ranges;

			ALSPRX_MSG_0(MED, "cct_query - SNS_DDF_ATTRIB_RANGE");

			if( (*value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_range_s)))  == NULL)
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			
			device_ranges = *value;
			*num_elems = 1;
			device_ranges->min = FX_FLTTOFIX_Q16(0);
			device_ranges->max = FX_FLTTOFIX_Q16(10000);
			break;
		}
    case SNS_DDF_ATTRIB_RESOLUTION_ADC:
		{
			sns_ddf_resolution_adc_s* resp;

			ALSPRX_MSG_0(MED, "cct_query - SNS_DDF_ATTRIB_RESOLUTION_ADC");

			if ( (resp = sns_ddf_memhandler_malloc(memhandler, 3 * sizeof(sns_ddf_resolution_adc_s))) == NULL )
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			
			*num_elems = 3;
			
			resp[0].bit_len = sns_dd_als_adc_bit_tb[APDS9960_DD_RES_14BIT];
			resp[0].max_freq = 20; // Hz
			resp[1].bit_len = sns_dd_als_adc_bit_tb[APDS9960_DD_RES_15BIT];
			resp[1].max_freq = 10; // Hz
			resp[2].bit_len = sns_dd_als_adc_bit_tb[APDS9960_DD_RES_16BIT];
			resp[2].max_freq = 5; // Hz
			
			*value = resp;
			break;
		}
		
    case SNS_DDF_ATTRIB_RESOLUTION:
		{
			sns_ddf_resolution_t* resp;

			ALSPRX_MSG_0(MED, "cct_query - SNS_DDF_ATTRIB_RESOLUTION");
			if ( (resp = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_resolution_t))) == NULL )
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			*num_elems = 1;
			
			*resp = APDS9960_DD_CCT_RES;
			*value = resp;
			break;
		}
		
    case SNS_DDF_ATTRIB_POWER_INFO:

		ALSPRX_MSG_0(MED, "cct_query - SNS_DDF_ATTRIB_POWER_INFO");

		if ( (power_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_power_info_s))) == NULL )
		{
			ALSPRX_MSG_0(ERROR, "malloc error");
			return SNS_DDF_ENOMEM;
		}
		*num_elems = 1;
		power_ptr->active_current = APDS9960_DD_CCT_PWR;
		power_ptr->lowpower_current = APDS9960_DD_CCT_LO_PWR;
		*(sns_ddf_power_info_s **)value = power_ptr;
		break;
		
    case SNS_DDF_ATTRIB_ACCURACY:

		ALSPRX_MSG_0(MED, "cct_query - SNS_DDF_ATTRIB_ACCURACY");

		if ( (accu_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint16_t))) == NULL )
		{
			ALSPRX_MSG_0(ERROR, "malloc error");
			return SNS_DDF_ENOMEM;
		}
		*num_elems = 1;
		*accu_ptr = APDS9960_DD_CCT_ACCURACY;
		*(uint16_t **)value = accu_ptr;
		break;
		
    case SNS_DDF_ATTRIB_ODR:
		{
			uint32_t* odr_ptr;
			
			ALSPRX_MSG_0(MED, "cct_query - SNS_DDF_ATTRIB_ODR");

			if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(uint32_t))) == NULL )
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}

			*num_elems = 1;
			*odr_ptr = dd_ptr->common.cct_odr;
			*(uint32_t **)value = odr_ptr;
			ALSPRX_MSG_1(MED, "odr=%d", *odr_ptr);
			break;
		}

	case SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST:
		{
			sns_ddf_odr_t* odr_ptr;
			uint32_t i;

			ALSPRX_MSG_0(MED, "als_query - SNS_DDF_ATTRIB_SUPPORTED_ODR_LIST");

			if ( (odr_ptr = sns_ddf_memhandler_malloc(memhandler, (sizeof(sns_dd_apds9960_odr_tb)/sizeof(sns_ddf_odr_t)) * sizeof(sns_ddf_resolution_adc_s))) == NULL )
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			
			*num_elems = sizeof(sns_dd_apds9960_odr_tb)/sizeof(sns_ddf_odr_t);

			for (i=0; i<*num_elems; i++)
			{		
				odr_ptr[i] = sns_dd_apds9960_odr_tb[i];
			}
			
			*value = odr_ptr;
		}
		break;
		
    default:
		ALSPRX_MSG_1(ERROR, "UNKNOWN cct_query - attrib=%d", attrib);
		return SNS_DDF_EINVALID_ATTR;
	}
	return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION:   sns_dd_apds9960_get_attrib

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

sns_ddf_status_e sns_dd_apds9960_get_attrib
(
sns_ddf_handle_t     dd_handle,
sns_ddf_sensor_e     sensor,
sns_ddf_attribute_e  attrib,
sns_ddf_memhandler_s* memhandler,
void**                value,
uint32_t*             num_elems
)
{
	apds9960_dd_dev_s* dd_ptr;
	uint32_t sub_dev_idx;
	sns_ddf_status_e status = SNS_DDF_SUCCESS;
	bool generic_attrib = FALSE;
	sns_ddf_driver_info_s *driver_info_ptr;
	sns_ddf_device_info_s *device_info_ptr;
	sns_ddf_registry_group_s *reg_group_ptr;
	uint8_t *reg_group_data_ptr;
	
	if ( (dd_handle == NULL) || (memhandler == NULL) )
	{
		ALSPRX_MSG_0(ERROR, "get_attrib handle is NULL");
		return SNS_DDF_EINVALID_PARAM;
	}
	
	sub_dev_idx = (uint32_t)dd_handle & (APDS9960_DD_HANDLE_ALIGN-1);
	dd_ptr = (apds9960_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9960_DD_HANDLE_ALIGN-1)));

    if (sub_dev_idx >= APDS9960_DD_MAX_SUB_DEV)
    {
       return SNS_DDF_EINVALID_PARAM;
    }

	ALSPRX_MSG_3(MED, "get_attr - sub_device=%d, sensor=%d attr=%d", sub_dev_idx, sensor, attrib);
	
	/* check first if the query is for generic attributes */
	switch ( attrib )
	{
    case SNS_DDF_ATTRIB_DRIVER_INFO:

		ALSPRX_MSG_0(MED, "SNS_DDF_ATTRIB_DRIVER_INFO");

		if ( (driver_info_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_driver_info_s))) == NULL )
		{
			ALSPRX_MSG_0(ERROR, "malloc error");
			return SNS_DDF_ENOMEM;
		}
		
		if (sensor == SNS_DDF_SENSOR_AMBIENT || sensor == SNS_DDF_SENSOR_PROXIMITY) {
			driver_info_ptr->name = "APDS-9960 Proximity & Light";
			driver_info_ptr->version = 0x1000; // version 1.0.0.0
		}
		else if (sensor == SNS_DDF_SENSOR_RGB || sensor == SNS_DDF_SENSOR_CT_C) {
			driver_info_ptr->name = "APDS-9960 RGB & CCT";
			driver_info_ptr->version = 0x1000; // version 1.0.0.0
		}
		else {
			ALSPRX_MSG_1(ERROR, "Invalide sensor=%d", sensor);
			return SNS_DDF_EINVALID_ATTR;
		}
		
		*(sns_ddf_driver_info_s**)value = driver_info_ptr;
		*num_elems = 1;
		generic_attrib = TRUE;
		break;
		
    case SNS_DDF_ATTRIB_DEVICE_INFO:

		ALSPRX_MSG_0(MED, "SNS_DDF_ATTRIB_DEVICE_INFO");

		if ( (device_info_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_device_info_s))) == NULL )
		{
			ALSPRX_MSG_0(ERROR, "malloc error");
			return SNS_DDF_ENOMEM;
		}
		
		if (sensor == SNS_DDF_SENSOR_AMBIENT) {
			device_info_ptr->name = "Light";
			device_info_ptr->vendor = "Avago";
			device_info_ptr->model = "APDS-9960";
			device_info_ptr->version = 0x1000; // version 1.0.0.0
		}
		else if (sensor == SNS_DDF_SENSOR_PROXIMITY) {
			device_info_ptr->name = "Proximity";
			device_info_ptr->vendor = "Avago";
			device_info_ptr->model = "APDS-9960";
			device_info_ptr->version = 0x1000; // version 1.0.0.0
		}
		else if ((sensor == SNS_DDF_SENSOR_RGB) || (sensor == SNS_DDF_SENSOR_CT_C)) {
			device_info_ptr->name = "RGB & CCT";
			device_info_ptr->vendor = "Avago";
			device_info_ptr->model = "APDS-9960";
			device_info_ptr->version = 0x1000; // version 1.0.0.0
		}
		else {
			ALSPRX_MSG_1(ERROR, "Invalide sensor=%d", sensor);
			return SNS_DDF_EINVALID_ATTR;
		}
		
		*(sns_ddf_device_info_s**)value = device_info_ptr; /* Is memcopy needed instead? */
		*num_elems = 1;
		generic_attrib = TRUE;
		break;
		
    case SNS_DDF_ATTRIB_REGISTRY_GROUP:

		ALSPRX_MSG_0(MED, "SNS_DDF_ATTRIB_REGISTRY_GROUP");

		if ( (reg_group_ptr = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_registry_group_s))) == NULL )
		{
			ALSPRX_MSG_0(ERROR, "malloc error");
			return SNS_DDF_ENOMEM;
		}
		
		if ( (reg_group_data_ptr = sns_ddf_memhandler_malloc(memhandler, dd_ptr->common.nv_size)) == NULL )
		{
			ALSPRX_MSG_0(ERROR, "malloc error");
			return SNS_DDF_ENOMEM;
		}
		
		SNS_OS_MEMSCPY(reg_group_data_ptr, sizeof(apds9960_dd_nv_s), &dd_ptr->common.nv_db, sizeof(apds9960_dd_nv_s));

		reg_group_ptr->group_data = reg_group_data_ptr;
		reg_group_ptr->size = dd_ptr->common.nv_size;
		*(sns_ddf_registry_group_s**)value = reg_group_ptr;
		
		ALSPRX_MSG_3(MED, "size=%d, data=%d, id=%d",
					 reg_group_ptr->size, (uint8_t) *reg_group_ptr->group_data, dd_ptr->common.nv_db.id);

		*num_elems = 1;
		generic_attrib = TRUE;
		break;

	case SNS_DDF_ATTRIB_DELAYS:
		{
			sns_ddf_delays_s *delays_ptr;

			ALSPRX_MSG_0(MED, "SNS_DDF_ATTRIB_DELAYS");

			if( (*value = sns_ddf_memhandler_malloc(memhandler, sizeof(sns_ddf_delays_s)))  == NULL)
			{
				ALSPRX_MSG_0(ERROR, "malloc error");
				return SNS_DDF_ENOMEM;
			}
			
			delays_ptr = *value;
			*num_elems = 1;
			delays_ptr->time_to_active = 6000;	//	6ms
			delays_ptr->time_to_data = 200000;	// 200ms
			break;
		}

		break;
		
    default:
		/* do nothing */
		break;
	}
	
	/* The query is not for generic attribute but is for specific data type */
	if ( generic_attrib != TRUE )
	{
		switch ( sensor )
		{
		case SNS_DDF_SENSOR_AMBIENT:
			status = apds9960_dd_als_query(dd_ptr, memhandler, attrib, value, num_elems);
			break;
			
		case SNS_DDF_SENSOR_RGB:
			status = apds9960_dd_rgb_query(dd_ptr, memhandler, attrib, value, num_elems);
			break;
			
		case SNS_DDF_SENSOR_CT_C:
			status = apds9960_dd_cct_query(dd_ptr, memhandler, attrib, value, num_elems);
			break;
			
		case SNS_DDF_SENSOR_PROXIMITY:
			status = apds9960_dd_prx_query(dd_ptr, memhandler, attrib, value, num_elems);
			break;
			
		default:
			status = SNS_DDF_EINVALID_ATTR;
		}
	}
	
	return status;
}

/*===========================================================================

FUNCTION:   sns_dd_apds9960_calibration

===========================================================================*/
/*!
@ Called by the SMGR to run calibration test for ALS or PRX.

@detail
Returns the cal_factor in error, to be stored in NV memory

@param[in]  handle      Handle to a driver instance.
@param[in]  sensor Sensor whose attribute is to be retrieved.
@param[in] test         Test ID used to decide what to test.
@param[out] err         prx_factor or als_factor.

@return
Success if no error. Otherwise a specific error code is returned.
*/
/*=========================================================================*/

sns_ddf_status_e sns_dd_apds9960_calibration
(
sns_ddf_handle_t dd_handle,
sns_ddf_sensor_e sensor,
sns_ddf_test_e test,
uint32_t* err
)
{
	apds9960_dd_dev_s* dd_ptr = (apds9960_dd_dev_s *)dd_handle;
	uint32_t sub_dev_idx;
	sns_ddf_status_e status = SNS_DDF_PENDING;
	sns_ddf_status_e reg_status = SNS_DDF_EFAIL;
	uint8_t i2c_data = 0x0;
	uint8_t bytes_rw;
	
	if (dd_handle == NULL)
	{
		ALSPRX_MSG_0(ERROR, "calibration : handle is NULL");
		return SNS_DDF_ENOMEM;
	}
	
	sub_dev_idx = (uint32_t)dd_handle & (APDS9960_DD_HANDLE_ALIGN-1);
	dd_ptr = (apds9960_dd_dev_s*)((uint32_t)dd_handle & (~(APDS9960_DD_HANDLE_ALIGN-1)));

	ALSPRX_MSG_3(HIGH, "calibration - sub_dev=%d, sensor=%d, test=%d", sub_dev_idx, sensor, test);
	/* check what is the test type */
	
	switch ( test )
	{
    case SNS_DDF_TEST_SELF:
       // Perform basic connectivity test here - for all sensor types
       {
         /* Read & Verify Device ID */
         status = sns_ddf_read_port( dd_ptr->port_handle,
                                  APDS9960_DD_ID_ADDR,
                                  &i2c_data,
                                  1,
                                  &bytes_rw );
       
         ALSPRX_MSG_2(HIGH, "ID=0x%X status=%d", i2c_data, status);

         if (status != SNS_DDF_SUCCESS || bytes_rw != 1)
         {
           ALSPRX_MSG_1(ERROR, "ddf_read_port error, status=%d", status);
           return SNS_DDF_EBUS;
         }
       
         if (i2c_data != 0xAB) 
         { /* APDS-9960 */
           ALSPRX_MSG_1(ERROR, "invalid sensor ID=0x%x", i2c_data);
           return SNS_DDF_EDEVICE;
         }
       }

       break;
    case SNS_DDF_TEST_OEM:
		
		if (sensor == SNS_DDF_SENSOR_PROXIMITY)
		{
			if (dd_ptr->prx.enable || dd_ptr->als.enable || dd_ptr->rgb.enable || dd_ptr->cct.enable)
			{
				ALSPRX_MSG_0(ERROR, "sensor is busy, cannot run calibration now");
				return SNS_DDF_EDEVICE_BUSY;
			}
			else
			{
				apds9960_dd_set_powerstate(dd_ptr, sub_dev_idx, sensor, SNS_DDF_POWERSTATE_ACTIVE, FALSE);

				// kk 15-Jul-2015
				dd_ptr->als.als_gain_index = APDS9960_DD_ALS_GAIN_64X;
				reg_status = apds9960_dd_set_control(dd_ptr, (sns_dd_als_again_bit_tb[dd_ptr->als.als_gain_index]|APDS9960_DD_PDRIVE_FOR_PS|APDS9960_DD_PGAIN_FOR_PS));
				CHECK_STATUS(reg_status);

				reg_status = apds9960_dd_update_intg_time(dd_ptr, TRUE, FALSE, FALSE);
				CHECK_STATUS(reg_status);

				reg_status = sns_ddf_read_port(dd_ptr->port_handle,
											APDS9960_DD_POFFSET_UR_ADDR,
											&i2c_data, 1, &bytes_rw);
				CHECK_STATUS(reg_status);
				dd_ptr->common.poffset_ur = i2c_data;

				reg_status = sns_ddf_read_port(dd_ptr->port_handle,
											APDS9960_DD_POFFSET_DL_ADDR,
											&i2c_data, 1, &bytes_rw);
				CHECK_STATUS(reg_status);
				dd_ptr->common.poffset_dl = i2c_data;

				dd_ptr->prx.enable = TRUE;
			
				apds9960_dd_set_config2(dd_ptr, 0x26);  // D and L are masked, only U and R are enabled
				apds9960_dd_set_enable(dd_ptr, APDS9960_DD_PWR_ON|APDS9960_DD_PRX_ENABLE);
			}
			
			dd_ptr->prx.calibration = 1;
			dd_ptr->prx.calibration_loop = 0;
			
			dd_ptr->common.temp_offset = dd_ptr->common.poffset_ur;
			
			apds9960_dd_start_general_timer (dd_ptr, sub_dev_idx, sensor, 10000);
		}
		else if (sensor == SNS_DDF_SENSOR_AMBIENT)
		{			
			if (dd_ptr->prx.enable || dd_ptr->als.enable || dd_ptr->rgb.enable || dd_ptr->cct.enable)
			{
				ALSPRX_MSG_0(ERROR, "sensor is busy, cannot run calibration now");
				return SNS_DDF_EDEVICE_BUSY;
			}
			else
			{
				apds9960_dd_set_powerstate(dd_ptr, als_prx_idx, SNS_DDF_SENSOR__ALL, 
                                   SNS_DDF_POWERSTATE_ACTIVE, FALSE);
			

				reg_status = apds9960_dd_update_intg_time(dd_ptr, FALSE, TRUE, FALSE);
				CHECK_STATUS(reg_status);
	
				dd_ptr->als.enable = TRUE;
				
				apds9960_dd_set_enable(dd_ptr, APDS9960_DD_PWR_ON|APDS9960_DD_ALS_ENABLE);
			}
			
			dd_ptr->als.calibration = TRUE;
			apds9960_dd_req_data (dd_ptr, als_prx_idx, sensor);
		}
		
		break;
		
    default:
		/* do nothing */
		return SNS_DDF_EINVALID_TEST;
	}
	
	return status;
}

/*===========================================================================

FUNCTION:   sns_dd_apds9960_probe

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
* @param[out] sensor Array of sensor types supported, with num_sensor
*                         elements. Allocated by this function.
*
* @return SNS_DDF_SUCCESS if the part was probed function completed, even
*         if no device was found (in which case num_sensors will be set to
*         0).
*/
sns_ddf_status_e sns_dd_apds9960_probe
(
sns_ddf_device_access_s* device_info,
sns_ddf_memhandler_s*    memhandler,
uint32_t*                num_sensors,
sns_ddf_sensor_e**       sensors
)
{
	sns_ddf_status_e status;
	sns_ddf_handle_t port_handle;
	uint8_t i2c_buff;
	uint8_t bytes_read;
	
	*num_sensors = 0;
	*sensors = NULL;
	
	status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
	if (status != SNS_DDF_SUCCESS)
	{
		ALSPRX_MSG_1(ERROR, "ddf_open_port error, status=%d", status);
		return status;
	}
	
	/* Read & Verify Device ID */
	status = sns_ddf_read_port( port_handle,
							   APDS9960_DD_ID_ADDR,
							   &i2c_buff,
							   1,
							   &bytes_read );
	
	if (status != SNS_DDF_SUCCESS || bytes_read != 1)
	{
		sns_ddf_close_port( port_handle );
		ALSPRX_MSG_1(ERROR, "ddf_read_port error, status=%d", status);
		return status;
	}
	
	if (i2c_buff != 0xAB) { /* APDS-9960 */
		/* Incorrect value. Return now with nothing detected */
		sns_ddf_close_port( port_handle );
		ALSPRX_MSG_1(ERROR, "invalid sensor ID=0x%x", i2c_buff);
		return SNS_DDF_EDEVICE;
	}

	/* ID register is as expected - APDS9960 */
	*num_sensors = APDS9960_DD_NUM_SENSOR_TYPES;
	*sensors = sns_ddf_memhandler_malloc( memhandler, sizeof(sns_ddf_sensor_e) * *num_sensors );
  
	if (first_probe == FALSE) 
	{    
		if ( *sensors != NULL )
		{
			(*sensors)[0] = SNS_DDF_SENSOR_PROXIMITY;
			(*sensors)[1] = SNS_DDF_SENSOR_AMBIENT;
            
			status = SNS_DDF_SUCCESS;
			first_probe = TRUE;
		} 
		else 
		{
			ALSPRX_MSG_0(MED, "malloc error");
			status = SNS_DDF_ENOMEM;
		}
	}
	else 
	{
		if ( *sensors != NULL )  
		{
			(*sensors)[0] = SNS_DDF_SENSOR_RGB;
			(*sensors)[1] = SNS_DDF_SENSOR_CT_C;
            
			status = SNS_DDF_SUCCESS;
			first_probe = FALSE;	// reset
		} 
		else 
		{
			ALSPRX_MSG_0(MED, "malloc error");
			status = SNS_DDF_ENOMEM;
		}
	}

	return status;
}

