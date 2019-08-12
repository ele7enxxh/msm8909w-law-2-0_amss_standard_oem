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


#ifndef ALPS_DRIVER_MAG_H
#define ALPS_DRIVER_MAG_H

#include "sns_ddf_driver_if.h"

/*--------------------------------------------------------------------------------------------------------------*/
/*	Register address 																							*/
/*	地磁気センサ (HSCDTD008) のレジスタアドレス																	*/
/*--------------------------------------------------------------------------------------------------------------*/
#define		NKR_STB			0x0C			//	R   : Self-Test-B response
#define		NKR_MIF			0x0D			//	R   : “More Info” Register
#define		NKR_WHO			0x0F			//	R   : “Who I Am” Register
#define		NKR_OVX			0x10			//	R   : Output Value x Register
#define		NKR_OVY			0x12			//	R   : Output Value y Register
#define		NKR_OVZ			0x14			//	R   : Output Value z Register
#define		NKR_SR1			0x18			//	R   : Status Register1
#define		NKR_CS1			0x1B			//	R/W : Control setting Register1
#define		NKR_CS2			0x1C			//	R/W : Control setting Register2
#define		NKR_CS3			0x1D			//	R/W : Control setting Register3
#define		NKR_CS4			0x1E			//	R/W : Control setting Register4
#define		NKR_ODX			0x20			//	R/W : Offset drift x Value Register
#define		NKR_ODY			0x22			//	R/W : Offset drift y Value Register
#define		NKR_ODZ			0x24			//	R/W : Offset drift z Value Register
#define		NKR_ITH			0x26			//	R/W : Interrupt threshold Register
#define		NKR_TMP			0x31			//	R   : Temperature Value Register
#define     NKR_CHIP_ID     0x49            //  R   : Chip Id value for the Who am I register.
/*--------------------------------------------------------------------------------------------------------------*/
/*	Register bit define																							*/
/*	地磁気センサ (HSCDTD008) レジスタのビット設定																*/
/*--------------------------------------------------------------------------------------------------------------*/
#define		STR_NORMAL		0x55			//	Normal register value
#define		STR_SELFTEST	0xAA			//	After self test value
#define		STR_ICERR		0xF0			//	IC offset error
#define		STR_XERR		0x01			//	X sensor offset error
#define		STR_YERR		0x02			//	Y sensor offset error
#define		STR_ZERR		0x04			//	Z sensor offset error

#define		INF_TE			0x1115			//	More Info Register Eagle Type

#define		WHO_RES			0x49

#define		SR1_DRDY		0x40			//	Data ready (0:Not ready, 1:Ready)
#define		SR1_DOR			0x20			//	Output data overrun (0:Not overrrun, 1:Overrun)
#define		SR1_INT			0x10			//	Combind interrupt information (0:No interrupt, 1:Interrupt)
#define		SR1_FFU			0x04			//	
#define		SR1_TRDY		0x02			//	Temperature Data ready (0:Not ready, 1:Ready)
#define		SR1_ORDY		0x01			//	

#define		CS1_PC1			0x80			//	Power Mode (0:Stand-by, 1:Active)
#define		CS1_ODR			0x18			//	Output Data Rate (00:10Hz, 01:20Hz, 10:50Hz, 11:100Hz)
#define		CS1_FS1			0x02			//	Functional state (0:Normal, 1:Force)

#define		CS2_AVG			0x80			//	
#define		CS2_FCO			0x40			//	
#define		CS2_AOR 		0x20			//	
#define		CS2_FF			0x10			//	
#define		CS2_DEN			0x08			//	Data ready pin enable (0:Disable, 1:Enable)
#define		CS2_DRP			0x04			//	DRDY signal active    (0:High, 1:Low)
#define		CS2_DTS			0x02			//	
#define		CS2_DOS			0x01			//	

#define		CS3_SRST		0x80			//	Soft reset (0:No action, 1:Start immediately POR)
#define		CS3_FORCE		0x40			//	Start forced measurement period(0:No action, 1:Start)
#define		CS3_STCB		0x10			//	Self-test-B function challenge set(0:No action, 1:Set)
#define		CS3_TCS			0x02			//	Temprature meagurement start
#define		CS3_OCL			0x01			//	Offset calib.

#define		CS4_MMD			0xC0			//	
#define		CS4_RS			0x10			//	
#define		CS4_AS			0x08			//	

enum {
	HSCDTD_MAG_X,
	HSCDTD_MAG_Y,
	HSCDTD_MAG_Z,
	HSCDTD_NUM_DATATYPES
};

enum {
	ALPS_MAG_SENSOR_MAG_RESOLUTION_14,
	ALPS_MAG_SENSOR_MAG_RESOLUTION_15,
	ALPS_MAG_SENSOR_MAX_RESOLUTION
};

enum {
	ALPS_MAG_SENSOR_MAG_RANGE_14,
	ALPS_MAG_SENSOR_MAG_RANGE_15,
	ALPS_MAG_SENSOR_MAX_RANGE
};


#define		ALPS_MAG_SENSOR_POWER_ACTIVE	(600)
#define		ALPS_MAG_SENSOR_LOWPOWER		(3)

/* 2.4mT = 24G */
#define		ALPS_MAG_SENSOR_RANGE14_MIN			(FX_FLTTOFIX_Q16(-12.0))
#define		ALPS_MAG_SENSOR_RANGE14_MAX			(FX_FLTTOFIX_Q16( 12.0))
#define		ALPS_MAG_SENSOR_RANGE15_MIN			(FX_FLTTOFIX_Q16(-24.0))
#define		ALPS_MAG_SENSOR_RANGE15_MAX			(FX_FLTTOFIX_Q16( 24.0))

/* 0.15uT = 0.0015G */
#define		ALPS_MAG_SENSOR_RESOLUTOIN			(0.0015f)

#define		ALPS_MAG_SENSOR_MAG_BIT_LEN14		(14)
#define		ALPS_MAG_SENSOR_MAG_BIT_LEN15		(15)
#define		ALPS_MAG_SENSOR_MAG_MAX_FREQ		(100)

#define		ALPS_MAG_SENSOR_DELAY_ACTIVE		(3005)
#define		ALPS_MAG_SENSOR_DELAY_DATA			(10000)

#define		ALPS_MAG_SENSOR_DRIVER_NAME			"ALPS GeoMagneticField device driver"
#define		ALPS_MAG_SENSOR_DRIVER_VER			(1)

#define		ALPS_MAG_SENSOR_DEVICE_NAME			"MagneticField"
#define		ALPS_MAG_SENSOR_DEVICE_VENDOR		"ALPS ELECTRIC CO. LTD."
#define		ALPS_MAG_SENSOR_DEVICE_MODEL		"HSCDTD008A"
#define		ALPS_MAG_SENSOR_DEVICE_VER			1

#define		MAG_NUM_AXES	(3)

#ifdef DDK
typedef bool		boolean;
#endif


typedef struct
{
	sns_ddf_handle_t			alps_mag_smgr_handle;
	uint32_t					alps_mag_num_devices;
	sns_ddf_memhandler_s*		alps_mag_memhandler;
	sns_ddf_handle_t			alps_mag_i2c_handle;
	uint8_t						alps_mag_i2c_buffer[8];
	uint8_t						resolution_adc;
	q16_t						data_cache[HSCDTD_NUM_DATATYPES];
	boolean						out_of_bound_flag;
	uint8_t						alps_mag_ready;
	uint8_t						alps_mag_ctrl[1];
//	sns_ddf_port_config_s		alps_mag_port;
//	sns_ddf_i2c_config_s		alps_mag_i2c;
	sns_ddf_powerstate_e		alps_mag_powerstate;
//	sns_ddf_device_access_s		device_info;
	sns_ddf_axes_map_s			alps_mag_axes_map;
} alps_mag_dd_state_t;


sns_ddf_status_e alps_mag_init(
			sns_ddf_handle_t* dd_handle_ptr,
			sns_ddf_handle_t smgr_handle,
			sns_ddf_nv_params_s* nv_params,
			sns_ddf_device_access_s device_info[],
			uint32_t num_devices,
			sns_ddf_memhandler_s* memhandler,
			sns_ddf_sensor_e** sensors,
			uint32_t* num_sensors);

sns_ddf_status_e alps_mag_get_data(
			sns_ddf_handle_t dd_handle,
			sns_ddf_sensor_e sensors[],
			uint32_t num_sensors,
			sns_ddf_memhandler_s* memhandler,
			sns_ddf_sensor_data_s** data);

sns_ddf_status_e alps_mag_set_attrib(
			sns_ddf_handle_t dd_handle,
			sns_ddf_sensor_e sensor,
			sns_ddf_attribute_e attrib,
			void* value);

sns_ddf_status_e alps_mag_get_attrib(
			sns_ddf_handle_t dd_handle,
			sns_ddf_sensor_e sensor,
			sns_ddf_attribute_e attrib,
			sns_ddf_memhandler_s* memhandler,
			void** value,
			uint32_t* num_elems);

void alps_mag_handle_timer(
			sns_ddf_handle_t dd_handle,
			void* arg);

void alps_mag_handle_irq(
        sns_ddf_handle_t  dd_handle,
        uint32_t          gpio_num,
        sns_ddf_time_t    timestamp);
			
sns_ddf_status_e alps_mag_reset(
			sns_ddf_handle_t dd_handle);

sns_ddf_status_e alps_mag_run_test(
			sns_ddf_handle_t dd_handle,
			sns_ddf_sensor_e sensor,
			sns_ddf_test_e test,
			uint32_t* err);

sns_ddf_status_e alps_mag_probe(
   sns_ddf_device_access_s *device_info,
   sns_ddf_memhandler_s *memhandler,
   uint32_t *num_sensors,
   sns_ddf_sensor_e **sensors);


#endif  //ALPS_DRIVER_MAG_H
